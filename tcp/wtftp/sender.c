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

#include "config.h"


#define	 SENDER_MAX_QUEUE_SIZE			(20)

#define  SENDER_MSG_TYPE				"sender"
#define  REPLY_MSG_TYPE					"reply"
#define  REPLAY_OK						"reply ok"
#define  REPLAY_FAILED					"replay failed"


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
		EB_LOGE("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
		pthread_mutex_destroy(&sender->mutex);
        pthread_cond_destroy(&sender->cond);
		deinit_messageQueue(&sender->queue);
		memset(sender,0,sizeof(T_Sender));
		free(sender);
		sender = NULL;
	}

	return 0;
}

static int sender_receive(PT_Sender sender,BusMsg * msg,int timeout)
{
	return receive_busMsg(sender->sockfd , msg ,timeout);
}

static int sender_send(PT_Sender sender,char *cmd,int len,int Cseq)
{
    int ret = -1;
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
	
	send_busMsg(sender->sockfd , &data);

    memset( &data , 0 , sizeof(BusMsg));
    if( sender_receive(sender,&data ,3000) > 0)
    {
        if( (strcmp(data.msgType,REPLY_MSG_TYPE) == 0)
            && (strcmp(data.msgData,REPLAY_OK) == 0) && (data.mCseq == Cseq))
        {            
            EB_LOGD("sender_receive msgType=%s msgData=%s mCseq= %d sendCseq=%d\r\n",data.msgType,data.msgData,data.mCseq,Cseq);
            ret = 0;
        }
    }else{
       return -1;
    }

    return ret;
}



static void *do_sender_sender_thread(void*arg)
{
	BusMsg		msg;
	PT_Sender sender = (PT_Sender)arg;
	MessageRecord  *record = NULL;
	if(!sender)
		return NULL;

	do
	{
	    if(sender->mSize <= 0)
        {
            break;
        }
        
		record = dequeue(&sender->queue);
		if(record != NULL)
		{
			if( 0 != sender_send(sender,record->fContentStr,record->mLen,record->fCSeq))
            {
                EB_LOGD("enqueue(&sender->queue,record) send failed\r\n");
			    enqueue(&sender->queue,record);//继续放入其中 直到被读出为止
			    //putAtHead(&sender->queue,record);
			}else
            {
                EB_LOGD("send ok\r\n");
                sender->mSize -= record->mLen;
                free_record(record);
                record = NULL;
                pthread_mutex_lock(&sender->mutex);
                pthread_cond_signal(&sender->cond);
    		    pthread_mutex_unlock(&sender->mutex);
            }

            usleep(1000);
		}else{
			//历史中没有记录 所以阻塞在这里等待 新记录
			//usleep(20000);
			EB_LOGD("sender wait1 running =%d\r\n",sender->isRunning);
            pthread_mutex_lock(&sender->mutex);
			pthread_cond_wait(&sender->cond,&sender->mutex);
            pthread_mutex_unlock(&sender->mutex);
			EB_LOGD("sender wait2\r\n");
		}
	    EB_LOGD("loop do_sender_sender_thread\r\n");
		
	}while(sender->isRunning == RUNNING);

	sender->isRunning = RUNNING_QUIT;
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

        EB_LOGD("sender ip:%s port:%d  sockfd:%d\r\n",sender->remoteIp,bindport,sender->sockfd);
	}while(0);

	return sender;
}

int close_sender_receviethread(PT_Sender sender)
{
    if(sender->isRunning == RUNNING)
    {
        while(sender->isRunning != RUNNING_QUIT)
        {
            pthread_mutex_lock(&sender->mutex);
            pthread_cond_signal(&sender->cond);   
            pthread_mutex_unlock(&sender->mutex);               
        }
    }

    return 0;
}

int closeSender(PT_Sender sender)
{
	if(sender)
	{
        close_sender_receviethread(sender);
		EB_LOGE("closeSender1 %p\r\n",sender);
		close(sender->sockfd);
		EB_LOGE("closeSender2 %p\r\n",sender);
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
		//EB_LOGD("wait for wait len=%d\r\n",mlen);
		if(mlen >= SENDER_MAX_QUEUE_SIZE)
		{
			pthread_mutex_lock(&sender->mutex);
			//EB_LOGD("sender pthread_cond_wait\r\n");
			//队列太长在 等待
			pthread_cond_wait(&sender->cond,&sender->mutex);
			pthread_mutex_unlock(&sender->mutex);
		}

		record   = malloc_record(sender->cseq,0,cmd,len);
        //EB_LOGD("writeSender cseq =%d len =%d\r\n",sender->cseq,len);
		if(!record)
		{
			ret = -1;
			return ret;
		}

		//sender_send(sender,cmd,len ,sender->cseq++);
		sender->cseq++;
		enqueue(&sender->queue,record);

		//唤醒
		pthread_mutex_lock(&sender->mutex);
		pthread_cond_signal(&sender->cond);
		pthread_mutex_unlock(&sender->mutex);
	}while(0);

	return ret;
}

//wait for sender exit
int SenderJoin(PT_Sender sender)
{
	pthread_join(sender->send_pid,NULL);
}

