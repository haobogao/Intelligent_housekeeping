#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <stdio.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include "protcol_data.h"

#ifndef  RECV_FILEDIR_SERVER
#define RECV_FILEDIR_SERVER   	"../recieved_data"
#endif

int socket_server_init();
int wait_connect();
int recv_file_from_client(char * file_path_dir, char * recvd_fileName);
int send_msg_to_client(const void * msg, unsigned int msg_len );
int close_socket_server() ;

#endif