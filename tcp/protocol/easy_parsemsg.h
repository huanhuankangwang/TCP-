#ifndef _EASY_PARSEMSG_H_
#define _EASY_PARSEMSG_H_

#include "easy_common.h"












int easy_send(int sockfd,char *ip, int port, char *cmd, unsigned short cmd_len);
int easy_receive(int nSocketFd, void *pvBuff, int bufSize);




#endif/*_EASY_PARSEMSG_H_*/
