#ifndef _RECEIVER_H_
#define _RECEIVER_H_
#include <pthread.h>


#include <messageQueue.h>



#define            MAX_REMOTE_IP_LEN      20

typedef struct
{
	MessageQueue    queue;//存放发送的记录
	int    			sockfd;
	int             isRunning;
	int 			flag;
	int				port;
	int 			cseq;//当前处理的 cseq
	int 			mRecvSize;//待接收的大小
	
	char			remoteIp[MAX_REMOTE_IP_LEN];

	pthread_cond_t	cond;
	pthread_t 		pid;
	pthread_mutex_t mutex;	
}T_Receiver,*PT_Receiver;


PT_Receiver openReceiver(const char *remoteIp,int port,int bindPort,int size);
int closeReceiver(PT_Receiver recv);


/*返回值 >=0  实际读取的长度*/
/*返回值 <0   读到了结束符*/
int readReceiver(PT_Receiver recv,const char *cmd,int maxsize);
int receiverJoin(PT_Receiver recv);


#endif //_RECEIVER_H_
