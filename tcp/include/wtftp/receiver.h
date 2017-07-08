#ifndef _RECEIVER_H_
#define _RECEIVER_H_
#include <pthread.h>


#include <messageQueue.h>



#define            MAX_REMOTE_IP_LEN      20

typedef struct
{
	MessageQueue    queue;//��ŷ��͵ļ�¼
	int    			sockfd;
	int             isRunning;
	int 			flag;
	int				port;
	int 			cseq;//��ǰ����� cseq
	int 			mRecvSize;//�����յĴ�С
	
	char			remoteIp[MAX_REMOTE_IP_LEN];

	pthread_cond_t	cond;
	pthread_t 		pid;
	pthread_mutex_t mutex;	
}T_Receiver,*PT_Receiver;


PT_Receiver openReceiver(const char *remoteIp,int port,int bindPort,int size);
int closeReceiver(PT_Receiver recv);


/*����ֵ >=0  ʵ�ʶ�ȡ�ĳ���*/
/*����ֵ <0   �����˽�����*/
int readReceiver(PT_Receiver recv,const char *cmd,int maxsize);
int receiverJoin(PT_Receiver recv);


#endif //_RECEIVER_H_
