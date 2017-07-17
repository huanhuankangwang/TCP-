#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <malloc.h>

#include <unistd.h>

#include <sender.h>
#include <packet_frame.h>
#include <pthread_define.h>


#define	 SENDER_MAX_QUEUE_SIZE			(20)

#define  SENDER_MSG_TYPE				"sender"
#define  REPLY_MSG_TYPE					"reply"
#define  REPLAY_OK						"reply ok"
#define  REPLAY_FAILED					"replay failed"

static void signal_handle(int signum)
{
	pthread_exit(NULL);
}

PT_Sender  malloc_sender(char *remoteIp,int remotePort,int bindport,int size)
{
	PT_Sender sender = NULL;

	do
	{
		if(!remoteIp)
			break;
		sender = malloc(sizeof(T_Sender));
		if(!sender)
			break;
		init_messageQueue(&sender->queue);
		pthread_mutex_init(&sender->mutex, NULL);
		pthread_cond_init(&sender->cond,NULL);
		sender->flag   	   = FLAG_VALID;
		sender->isRunning  = RUNNING;
		sender->mSenderIsRunning = RUNNING;
		sender->port	   = remotePort;
		sender->cseq	   = 0;
		sender->mSize      = size;
	    memcpy(sender->remoteIp,remoteIp,MAX_REMOTE_IP_LEN);
	}while(0);

	return sender;
}

int free_sender(PT_Sender sender)
{
	if(sender)
	{
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
		pthread_mutex_destroy(&sender->mutex);
        pthread_cond_destroy(&sender->cond);
		deinit_messageQueue(&sender->queue);
		memset(sender,0,sizeof(T_Sender));
		free(sender);
		sender = NULL;
	}

	return 0;
}

int close_sender_senderthread(PT_Sender sender)
{

    sender->mSenderIsRunning = NOT_RUNNING;
    while(sender->mSenderIsRunning != RUNNING_QUIT)
    {
        sleep(1);
        pthread_mutex_lock(&sender->mutex);
		pthread_cond_signal(&sender->cond);
		pthread_mutex_unlock(&sender->mutex);
    }
	pthread_join(sender->send_pid,NULL);
    return 0;
}

int close_sender_receviethread(PT_Sender sender)
{
	sender->isRunning = NOT_RUNNING;
    return 0;
}

static int sender_send(PT_Sender sender,char *cmd,int len,int Cseq)
{
	BusMsg data;
	memset( &data , 0 , sizeof(BusMsg));
	strncpy(data.remoteAddr.ip , sender->remoteIp , 
		MAX_REMOTE_IP_LEN > BUS_ADDR_MAX_LEN ? BUS_ADDR_MAX_LEN : MAX_REMOTE_IP_LEN);
	data.remoteAddr.port  = sender->port;
	strcpy(data.msgType , SENDER_MSG_TYPE);

	data.msgDataSize  = len > BUS_MSGDATA_MAX_LEN ? BUS_MSGDATA_MAX_LEN : len ;
	memcpy(data.msgData , cmd , data.msgDataSize);
	data.mode   = 0;
	data.mCseq  = Cseq;
	
	return send_busMsg(sender->sockfd , &data);
}

static int sender_receive(PT_Sender sender,BusMsg * msg)
{
	return receive_busMsg(sender->sockfd , msg);
}

static void *do_sender_receive_thread(void*arg)
{
	BusMsg		msg;
	PT_Sender sender = (PT_Sender)arg;
	MessageRecord  *record = NULL;
	if(!sender)
		return NULL;

	do
	{
		memset(&msg ,0 , sizeof(BusMsg));
		if( sender_receive(sender,&msg) > 0)
		{
			//成功接收到了
			if(strcmp(msg.msgType , REPLY_MSG_TYPE ) == 0)
			{
				//找到相应的位置
				if(strcmp(msg.msgData,REPLAY_OK) == 0)
				{
					//printf("receive replay_ok type=%s msgdata=%s\r\n",msg.msgType,msg.msgData);
					record = removeOneByCseq(&sender->queue, msg.mCseq);
					if(record)
					{
						sender->mSize -= record->mLen;
						//printf("removeOneByCseq1 \r\n");
						pthread_mutex_lock(&sender->mutex);
						pthread_cond_signal(&sender->cond);
						pthread_mutex_unlock(&sender->mutex);
						//printf("removeOneByCseq2 \r\n");
						free_record(record);
						record = NULL;
					}

					if(sender->mSize <= 0)
						break;
				}
			}
		}	
		
	}while(sender->isRunning == RUNNING);

	sender->isRunning  = RUNNING_QUIT;
	printf("exit do_sender_receive_thread1\r\n");
	//退出发送线程
	close_sender_senderthread(sender);
	sender->flag       = FLAG_NOT_VALID;

	printf("exit do_sender_receive_thread2\r\n");

	return NULL;
}

static void *do_sender_sender_thread(void*arg)
{
	BusMsg		msg;
	PT_Sender sender = (PT_Sender)arg;
	MessageRecord  *record = NULL;
	if(!sender)
		return NULL;

	wsignal();

	do
	{
		pthread_mutex_lock(&sender->mutex);
		record = dequeue(&sender->queue);
		if(record)
		{
			sender_send(sender,record->fContentStr,record->mLen,record->fCSeq);
			//putAtHead(&sender->queue,record);
			enqueue(&sender->queue,record);//继续放入其中 直到被读出为止
		}else{
			//历史中没有记录 所以阻塞在这里等待 新记录
			//usleep(20000);
			printf("sender wait1 running =%d\r\n",sender->mSenderIsRunning);
			pthread_cond_wait(&sender->cond,&sender->mutex);
			printf("sender wait2\r\n");
		}

		//printf("loop do_sender_sender_thread\r\n");
		pthread_mutex_unlock(&sender->mutex);	
		
	}while(sender->mSenderIsRunning == RUNNING);

	sender->mSenderIsRunning  = RUNNING_QUIT;
	printf("exit do_sender_sender_thread\r\n");

	return NULL;
}

PT_Sender openSender(char *remoteIp,int remotePort,int bindport,int totalSize)
{
	int  ret = 0;
	PT_Sender  sender = NULL;
	int flags;
    int on = 1;
	struct sockaddr_in servAddr;

	do
	{
		sender = malloc_sender(remoteIp,remotePort,bindport,totalSize);
		if(!sender)
			break;

		if((sender->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			free_sender(sender);
			sender = NULL;
			break;
		}
		if((setsockopt(sender->sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
        {  
        	close(sender->sockfd);
			free_sender(sender);
			sender = NULL;
            perror("setsockopt failed");
            break;
        }

		/*bind*/
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(bindport);
		if((ret = bind(sender->sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0) 
		{
			close(sender->sockfd);
			free_sender(sender);
			sender = NULL;		
			break;
		}
		flags = fcntl(sender->sockfd , F_GETFL , 0);
		fcntl(sender->sockfd,F_SETFL,flags|O_NONBLOCK);//设置为非阻塞

		ret = pthread_create(&sender->send_pid,NULL,do_sender_sender_thread,(void*)sender);
	    if(ret!=0)  
	    {
			close(sender->sockfd);
			free_sender(sender);
			sender = NULL;
			break;
	    }

		ret = pthread_create(&sender->reply_pid,NULL,do_sender_receive_thread,(void*)sender);
	    if(ret!=0)  
	    {
			close_sender_senderthread(sender);
			close(sender->sockfd);
			free_sender(sender);
			sender = NULL;
			break;
	    }

        printf("sender ip:%s port:%d  sockfd:%d\r\n",sender->remoteIp,bindport,sender->sockfd);
	}while(0);

	return sender;
}

int closeSender(PT_Sender sender)
{
	if(sender)
	{
		if(sender->isRunning == RUNNING)
		{
			close_sender_receviethread(sender);
			pthread_join(sender->reply_pid,NULL);			
		}
		printf("closeSender1 %p\r\n",sender);
		close(sender->sockfd);
		printf("closeSender2 %p\r\n",sender);
		free_sender(sender);
		sender = NULL;
	}
    //FILE_READER_DEBUG("closeFileReader\r\n");
	return 0;
}

int writeSender(PT_Sender sender,char *cmd,int len)
{
	MessageRecord  *record = NULL;
	int ret =0;
	int mlen = 0;

	do
	{
		if(!cmd || !sender || len <= 0)
		{
			ret = -1;
			return ret;
		}

		mlen = getQueueLength(&sender->queue);
		printf("wait for wait len=%d\r\n",mlen);
		if(mlen >= SENDER_MAX_QUEUE_SIZE)
		{
			pthread_mutex_lock(&sender->mutex);

			printf("sender pthread_cond_wait\r\n");
			//队列太长在 等待
			pthread_cond_wait(&sender->cond,&sender->mutex);
			pthread_mutex_unlock(&sender->mutex);
		}

		pthread_mutex_lock(&sender->mutex);
		record   = malloc_record(sender->cseq,0,cmd,len);
        printf("writeSender cseq =%d len =%d\r\n",sender->cseq,len);
		if(!record)
		{
			ret = -1;
			return ret;
		}

		sender_send(sender,cmd,len ,sender->cseq++);
		enqueue(&sender->queue,record);
		
		//唤醒
		pthread_cond_signal(&sender->cond);
		pthread_mutex_unlock(&sender->mutex);
	}while(0);

	return ret;
}

//wait for sender exit
int SenderJoin(PT_Sender sender)
{
	//pthread_join(sender->send_pid,NULL);
	pthread_join(sender->reply_pid,NULL);
}

