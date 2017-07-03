#ifndef _SENDER_H_
#define _SENDER_H_

#include <messageQueue.h>
#include <pthread.h>

#define            MAX_REMOTE_IP_LEN      20


typedef struct _sender{
	MessageQueue    queue;//存放发送的记录
	int    			sockfd;
	int             isRunning;
	int 			flag;
	int				port;
	int 			cseq;
	
	char			remoteIp[MAX_REMOTE_IP_LEN];

	pthread_t 		pid;
	pthread_cond_t	cond;
	pthread_mutex_t mutex;

	pthread_mutex_t write_mutex;
	pthread_cond_t  write_cond;
}T_Sender,*PT_Sender;


PT_Sender openSender(char *remoteIp,int remotePort,int bindport);
int 	closeSender(PT_Sender sender);
int writeSender(PT_Sender sender,char *cmd,int len);

#endif//_SENDER_H_