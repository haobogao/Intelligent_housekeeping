#include "XML_analyse.h" 
#include <stdlib.h>

char *   XML_get_rawtext(const  char * XML_data) {

	if (XML_data == NULL) {
		puts("XML data is empty ! ");
		return NULL;
	}

	char *  start_rawtext = NULL;
	char *  end_rawtext  = NULL;
	if(NULL ==  (start_rawtext = strstr(XML_data, "<rawtext>") ) ) {
		puts("XML do not have  data !");
		return NULL;
	}
	if (NULL ==  (end_rawtext = strstr(XML_data, "</rawtext>") ) ) {
		puts("XML do not have  end !");
		return NULL;
	}

	start_rawtext = start_rawtext + strlen("<rawtext>");

	char * analyse_result = (char *)malloc(sizeof(char) * (end_rawtext - start_rawtext + 2));
	if (analyse_result == NULL) {
		perror("Aply memory for result error ! \n");
		return NULL;
	}
	memset(analyse_result, 0, sizeof(analyse_result));
	strncpy(analyse_result, start_rawtext, end_rawtext - start_rawtext);
	analyse_result[end_rawtext - start_rawtext]  = '\0';

	return analyse_result;

}

void free_analyse_result(char * analyse_result) {
	free(analyse_result);
}
