#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <malloc.h>


#include <sender.h>
#include <packet_frame.h>
#include <pthread_define.h>


#define	 SENDER_MAX_QUEUE_SIZE			(20)

#define  SENDER_MSG_TYPE				"sender"
#define  REPLY_MSG_TYPE					"reply"
#define  REPLAY_OK						"reply ok"
#define  REPLAY_FAILED					"replay failed"

static int sender_send(PT_Sender sender,char *cmd,int len,int Cseq)
{
	BusMsg data;
	memset( &data , 0 , sizeof(BusMsg));
	strncpy(data.remoteAddr.ip , sender->remoteIp , 
		MAX_REMOTE_IP_LEN > BUS_ADDR_MAX_LEN ? BUS_ADDR_MAX_LEN : MAX_REMOTE_IP_LEN);
	data.remoteAddr.port  = sender->port;
	strcpy(data.msgType , SENDER_MSG_TYPE);

	data.msgDataSize  = len > BUS_MSGDATA_MAX_LEN ? BUS_MSGDATA_MAX_LEN : len ;
	strncpy(data.msgData , cmd , data.msgDataSize);
	data.mode   = 0;
	data.mCseq  = Cseq;
	
	return send_busMsg(sender->sockfd , &data);
}

static int sender_receive(PT_Sender sender,BusMsg * msg)
{
	return receive_busMsg(sender->sockfd , msg);
}


static void *do_sender_thread(void*arg)
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
					printf("receive replay_ok type=%s msgdata=%s\r\n",msg.msgType,msg.msgData);
					printf("this way msg.mCseq =%d\r\n",msg.mCseq);
					record = removeOneByCseq(&sender->queue, msg.mCseq);
					printf("this way record =%d\r\n",record);
					if(record)
					{
						printf("removeOneByCseq \r\n");
						pthread_mutex_lock(&sender->mutex);
						pthread_cond_signal(&sender->cond);
						pthread_mutex_unlock(&sender->mutex);
						free_record(record);
						record = NULL;
					}
					
				}
			}
		}

		pthread_mutex_lock(&sender->mutex);
		record = dequeue(&sender->queue);
		if(record)
		{
			sender_send(sender,record->fContentStr,record->mLen,record->fCSeq);

			putAtHead(&sender->queue,record);
			//enqueue(&sender->queue,record);//继续放入其中 直到被读出为止
		}else{
			//历史中没有记录 所以阻塞在这里等待 新记录
			pthread_cond_wait(&sender->cond,&sender->mutex);
		}
		pthread_mutex_unlock(&sender->mutex);		
		
	}while(1);

	sender->isRunning  = RUNNING_QUIT;
	sender->flag       = FLAG_NOT_VALID;

	return NULL;
}

PT_Sender openSender(char *remoteIp,int remotePort,int bindport)
{
	int  ret = 0;
	PT_Sender  sender = NULL;
	int flags;
	struct sockaddr_in servAddr;

	do
	{
		if(remoteIp == NULL)
			break;
		sender = malloc(sizeof(T_Sender));
		if(!sender)
			break;
		memset(sender,0,sizeof(T_Sender));
		if((sender->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			free(sender);
			sender = NULL;
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
			free(sender);
			sender = NULL;		
			break;
		}
		ret = pthread_create(&sender->pid,NULL,do_sender_thread,(void*)sender);
	    if(ret!=0)  
	    {
	    	close(sender->sockfd);
			free(sender);
			sender = NULL;
			break;
	    }

		flags = fcntl(sender->sockfd , F_GETFL , 0);
		fcntl(sender->sockfd,F_SETFL,flags|O_NONBLOCK);//设置为非阻塞
		init_messageQueue(&sender->queue);
		pthread_mutex_init(&sender->mutex, NULL);
		pthread_cond_init(&sender->cond,NULL);
		sender->flag   	   = FLAG_VALID;
		sender->isRunning  = RUNNING;
		sender->port	   = remotePort;
		sender->cseq	   = 0;
		strncpy(sender->remoteIp,remoteIp,MAX_REMOTE_IP_LEN);		
	}while(0);
	
	return sender;
}

int closeSender(PT_Sender sender)
{
	if(sender)
	{
		if(isFlagValid(sender))
		{
        	pthread_mutex_lock(&sender->mutex);
        	pthread_cond_signal(&sender->cond);
        	pthread_mutex_unlock(&sender->mutex);
		}else
		{
			sender->isRunning = NOT_RUNNING;
			while(sender->isRunning == RUNNING_QUIT)
	        {
	            printf(" running =%d flag = %d ",sender->isRunning,sender->flag);
	            usleep(200);
	        }//等待退出成功
		}

        //pthread_kill(tid, SIGTERM); //强制杀死
        pthread_mutex_destroy(&sender->mutex);
        pthread_cond_destroy(&sender->cond);
		close(sender->sockfd);
		free_messageQueue(&sender->queue);
		free(sender);
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
		if(!cmd || !sender)
		{
			ret = -1;
			return ret;
		}

		mlen = getQueueLength(&sender->queue);
		printf("wait for wait len=%d\r\n",mlen);
		if(mlen > SENDER_MAX_QUEUE_SIZE)
		{
			pthread_mutex_lock(&sender->mutex);

			printf("sender pthread_cond_wait\r\n");
			//队列太长在 等待
			pthread_cond_wait(&sender->cond,&sender->mutex);
			pthread_mutex_unlock(&sender->mutex);
		}

		printf("writeSender cseq =%d\r\n",sender->cseq);
		record   = malloc_record(sender->cseq,0,cmd,len);
		if(!record)
		{
			ret = -1;
			return ret;
		}

		sender_send(sender,cmd,len ,sender->cseq++);
		enqueue(&sender->queue,record);
		pthread_mutex_lock(&sender->mutex);
		//唤醒
		pthread_cond_signal(&sender->cond);
		pthread_mutex_unlock(&sender->mutex);
	}while(0);

	return ret;
}

