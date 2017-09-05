/*
	更新记录:
		接收语音文件线程   --->   由协议数据分析引入
		获得客户端上传的语音文件 --->  内存申请
 */
#include "voice_recognise.h"
#include "protcol_data.h"
#include "debug.h"

typedef struct _UserData {
	int     build_fini; //标识语法构建是否完成
	int     update_fini; //标识更新词典是否完成
	int     errcode; //记录语法构建或更新词典回调错误码
	char    grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
}UserData;

static 	UserData asr_data; 

char *get_audio_file(void); //选择进行离线语法识别的语音文件
int build_grammar(UserData *udata); //构建离线识别语法网络
int update_lexicon(UserData *udata); //更新离线识别语法词典
int run_asr(UserData *udata); //进行离线语法识别

int send_recogResult_to_client(char * recogResult, unsigned int resultLen) {

	protocol_ptr_t  prctND = create_protc_nd();

	bzero(prctND, sizeof(prctND));

	prctND->command.c_command.dst = BOARD;
	prctND->command.c_command.cmd = reslt_voice_recog;

	send_prctdata_toBoard(prctND);

	free_prctND(prctND);
	
	return send_msg_to_client(recogResult, resultLen);
}

// 获取客户端上传的语音文件
char* get_audio_file(void)
{
	char buff_recvName[255] = {0};
	char* recv_file_path = (char *)malloc(sizeof(buff_recvName) + strlen(RECV_FILEDIR_SERVER));

	if (recv_file_from_client(RECV_FILEDIR_SERVER,  buff_recvName) < 0) {
		return NULL;
	}

	bzero(recv_file_path, sizeof(recv_file_path) );
	sprintf(recv_file_path, "%s/%s", RECV_FILEDIR_SERVER, buff_recvName);
	
	return recv_file_path;
}

int build_grm_cb(int ecode, const char *info, void *udata)
{
	UserData *grm_data = (UserData *)udata;

	if (NULL != grm_data) {
		grm_data->build_fini = 1;
		grm_data->errcode = ecode;
	}

	if (MSP_SUCCESS == ecode && NULL != info) {
	
#ifdef 	_DEBUG_M_
		printf("构建语法成功！ 语法ID:%s\n", info);
#endif

		if (NULL != grm_data)
			snprintf(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, "%s", info);
	}
	else
		printf("构建语法失败！%d\n", ecode);

	return 0;
}

int build_grammar(UserData *udata)
{
	FILE *grm_file                           = NULL;
	char *grm_content                        = NULL;
	unsigned int grm_cnt_len                 = 0;
	char grm_build_params[MAX_PARAMS_LEN]    = {0};
	int ret                                  = 0;

	grm_file = fopen(GRM_FILE, "rb");	
	if(NULL == grm_file) {
		printf("打开\"%s\"文件失败！[%s]\n", GRM_FILE, strerror(errno));
		return -1; 
	}

	fseek(grm_file, 0, SEEK_END);
	grm_cnt_len = ftell(grm_file);
	fseek(grm_file, 0, SEEK_SET);

	grm_content = (char *)malloc(grm_cnt_len + 1);
	if (NULL == grm_content)
	{
		printf("内存分配失败!\n");
		fclose(grm_file);
		grm_file = NULL;
		return -1;
	}
	fread((void*)grm_content, 1, grm_cnt_len, grm_file);
	grm_content[grm_cnt_len] = '\0';
	fclose(grm_file);
	grm_file = NULL;

	snprintf(grm_build_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH
		);
	ret = QISRBuildGrammar("bnf", grm_content, grm_cnt_len, grm_build_params, build_grm_cb, udata);

	free(grm_content);
	grm_content = NULL;

	return ret;
}

int update_lex_cb(int ecode, const char *info, void *udata)
{
	UserData *lex_data = (UserData *)udata;

	if (NULL != lex_data) {
		lex_data->update_fini = 1;
		lex_data->errcode = ecode;
	}

	if (MSP_SUCCESS == ecode)
		printf("更新词典成功！\n");
	else
		printf("更新词典失败！%d\n", ecode);

	return 0;
}

int update_lexicon(UserData *udata)
{
	const char *lex_content                   = "丁伟\n黄辣椒";
	unsigned int lex_cnt_len                  = strlen(lex_content);
	char update_lex_params[MAX_PARAMS_LEN]    = {0}; 

	snprintf(update_lex_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, text_encoding = UTF-8, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, grammar_list = %s, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH,
		udata->grammar_id);
	return QISRUpdateLexicon(LEX_NAME, lex_content, lex_cnt_len, update_lex_params, update_lex_cb, udata);
}

int run_asr(UserData *udata)
{
	char asr_params[MAX_PARAMS_LEN]    = {0};
	const char *rec_rslt               = NULL;
	const char *session_id             = NULL;
	char *asr_audiof             = NULL;
	FILE *f_pcm                        = NULL;
	char *pcm_data                     = NULL;
	long pcm_count                     = 0;
	long pcm_size                      = 0;
	int last_audio                     = 0;
	int aud_stat                       = MSP_AUDIO_SAMPLE_CONTINUE;
	int ep_status                      = MSP_EP_LOOKING_FOR_SPEECH;
	int rec_status                     = MSP_REC_STATUS_INCOMPLETE;
	int rss_status                     = MSP_REC_STATUS_INCOMPLETE;
	int errcode                        = -1;

	// 获得语音文件 的 文件名
	if ( (asr_audiof = get_audio_file() ) == NULL) {
		printf("Get audio file failed !\n");
		return -1;
	}

#ifdef _DEBUG_M_
	printf("语音文件名： %s\n", asr_audiof);
#endif

	// 开始解析文件
	f_pcm = fopen(asr_audiof, "rb");
	if (NULL == f_pcm) {
		printf("打开\"%s\"失败！[%s]\n", (char *)asr_audiof, strerror(errno));
		goto run_error;
	}

	// 释放内存
	free(asr_audiof);

	// 获取该文件的大小 
	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm);
	fseek(f_pcm, 0, SEEK_SET);

	// 获取文件内容
	pcm_data = (char *)malloc(pcm_size);
	if (NULL == pcm_data)
		goto run_error;
	fread((void *)pcm_data, pcm_size, 1, f_pcm);

	fclose(f_pcm);
	f_pcm = NULL;

	// puts("语音文件解析完成！");
	
	//离线语法识别参数设置
	snprintf(asr_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, local_grammar = %s, \
		result_type = xml, result_encoding = UTF-8, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH,
		udata->grammar_id
		);
	session_id = QISRSessionBegin(NULL, asr_params, &errcode);
	if (NULL == session_id)
		goto run_error;
	printf("开始识别...\n");

	while (1) {
		unsigned int len = 6400;

		if (pcm_size < 12800) {
			len = pcm_size;
			last_audio = 1;
		}

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;

		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		if (len <= 0)
			break;

		printf(">");
		fflush(stdout);
		errcode = QISRAudioWrite(session_id, (const void *)&pcm_data[pcm_count], len, aud_stat, &ep_status, &rec_status);
		if (MSP_SUCCESS != errcode)
			goto run_error;

		pcm_count += (long)len;
		pcm_size -= (long)len;

		//检测到音频结束
		if (MSP_EP_AFTER_SPEECH == ep_status)
			break;

		usleep(150 * 1000); //模拟人说话时间间隙
	}
	//主动点击音频结束
	QISRAudioWrite(session_id, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_status, &rec_status);

	free(pcm_data);
	pcm_data = NULL;

	//获取识别结果
	while (MSP_REC_STATUS_COMPLETE != rss_status && MSP_SUCCESS == errcode) {
		rec_rslt = QISRGetResult(session_id, &rss_status, 0, &errcode);
		usleep(150 * 1000);
	}
	printf("\n识别结束：\n");

	printf("=================================================\n");
	if (NULL != rec_rslt) {
		// printf("%s\n", rec_rslt);
		char * analyse_result =  (char *)XML_get_rawtext(rec_rslt);	//将识别结果的 XML  解析
		printf("%s \n", analyse_result);

		// send_msg_to_client(analyse_result_buff, strlen(analyse_result_buff));
		// send_msg_to_client(analyse_result_buff);			//将识别结果返回下位机
		if (0 < send_recogResult_to_client(analyse_result, strlen(analyse_result) + 1 ) ) {
#ifdef _DEBUG_M_
			puts("Sended result to clint !");
#endif
		}
		free_analyse_result(analyse_result);
	}else{
		printf("没有识别结果！\n");
		// send_msg_to_client("No command !" );
		if (0 < send_recogResult_to_client("No command !", strlen("No command !") + 1 ) ) {
#ifdef _DEBUG_M_
			puts("Sended result to clint !");
#endif			
		}
	}
	printf("================================================\n");
	
	goto run_exit;

run_error:
	if (NULL != pcm_data) {
		free(pcm_data);
		pcm_data = NULL;
	}
	if (NULL != f_pcm) {
		fclose(f_pcm);
		f_pcm = NULL;
	}
run_exit:
	QISRSessionEnd(session_id, NULL);
	return errcode;
}

int voice_recognise_init() {
// void * aitalk_voiceRecog(void * arg)
// {
	const char *login_config    = 	"appid = 5992beb7"; 	//登录参数
	int ret                    	     = 0 ;
	char c;

	// puts("语音识别模块初始化...");

	ret = MSPLogin(NULL, NULL, login_config); //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret) {
		printf("登录失败：%d\n", ret);
		goto exit;
	}

	memset(&asr_data, 0, sizeof(UserData));

	// printf("构建离线识别语法网络...\n");
	
	ret = build_grammar(&asr_data);  //第一次使用某语法进行识别，需要先构建语法网络，获取语法ID，之后使用此语法进行识别，无需再次构建
	if (MSP_SUCCESS != ret) {
		printf("构建语法调用失败！\n");
		goto exit;
	}
	while (1 != asr_data.build_fini)
		usleep(300 * 1000);
	if (MSP_SUCCESS != asr_data.errcode)
		goto exit;
	printf("语音识别模块初始化完成!\n");	

	// while(1) {
	// 	ret = run_asr(&asr_data);
	// 	if (MSP_SUCCESS != ret) {
	// 		printf("离线语法识别出错: %d \n", ret);
	// 		goto exit;
	// 	}
	// }

	return;
exit:
	MSPLogout();
	printf("请按任意键退出...\n");
	getchar();
	return;
}

// int voice_recognise_init() {

// 	pthread_t aitalk_thread_id;
// 	if(0 != pthread_create(&aitalk_thread_id,NULL, aitalk_voiceRecog, NULL) ) {
// 		perror("Voice recognise initialize failed ! n");
// 		return -1;
// 	}

// 	return 0;
// }

int start_speech_recognize() {

	int ret                    	     = 0 ;

#ifdef _DEBUG_M_
	printf("\n开始语音识别!\n");
#endif

	if (MSP_SUCCESS != (ret = run_asr(&asr_data))) {
		printf("离线语法识别出错: %d \n", ret);
		MSPLogout();
		return -1;
	}
	
	return 0;
}