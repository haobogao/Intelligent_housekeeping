#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include <stdio.h>
#include "nopoll.h"
#include "debug.h"
#include "protcol_data.h"

int init_websocket_v12();
int websocket_send(void * msg,unsigned int len);
void listener_on_message (noPollCtx * ctx, noPollConn * conn, noPollMsg * msg, noPollPtr  user_data);

#endif
