#include <malloc.h>
#include <string.h>
#include <unistd.h>


#include <filesender.h>
#include <pthread_define.h>

#include "config.h"



#define     FILE_READ_END				(1)
#define     FILE_NOT_READ_END			(0)

void *do_fileSender_thread(void*arg)
{
	char  tmp[1024];
	int  ret;
	int  len;
	
	PT_FileSender sender = (PT_FileSender)arg;
	if(!sender)
		return NULL;
	
	do
	{
        memset((void*)tmp ,0,sizeof(tmp));
        EB_LOGD(" readFileReader \r\n");
        ret  = readFileReader(sender->filereader,tmp,sizeof(tmp));
        if(ret <  0)
        {
            break;
        }else if(ret == 0)
        {
            continue;
        }
 
		len  = ret;
		ret = writeSender(sender->sender,tmp,len);
        EB_LOGD("writeSender len =%d  ret =%d\r\n",len,ret);
		if(ret < 0)
		{
			break;
		}
		
	}while(sender->isRunning == RUNNING);

//等待这两个线程退出
	//等待 发送端退出
	EB_LOGE("filesender senderJoin1\r\n");
	SenderJoin(sender->sender);
	EB_LOGE("filesender senderJoin2\r\n");
	closeSender(sender->sender);
	EB_LOGE("filesender closeSender\r\n");
	sender->sender = NULL;
	sender->flag      = FLAG_NOT_VALID;
	sender->isRunning = RUNNING_QUIT;
}

PT_FileSender openFileSender(char *filename,char *remoteIp,int port,int bindport,int filesize)
{
	int ret;
	PT_FileSender  sender = NULL;
	do
	{
		sender  = (PT_FileSender)malloc(sizeof(T_FileSender));
		if(!sender)
			break;
		sender->filereader = openFileReader(filename,1024*20);
		if(!sender->filereader)
		{
			free(sender);
			sender = NULL;
			break;
		}

		
		sender->sender = openSender(remoteIp,port,bindport,filesize);
		if(!sender->sender)
		{
			closeFileReader(sender->filereader);
			sender->filereader = NULL;
			free(sender);
			sender = NULL;
			break;
		}

		ret = pthread_create(&sender->pid,NULL,do_fileSender_thread,(void*)sender);
		if(ret != 0)
		{
			closeSender(sender->sender);
			sender->sender = NULL;
			closeFileReader(sender->filereader);
			sender->filereader = NULL;
			free(sender);
			sender = NULL;
			break;
		}

		//pthread_mutex_init(&sender->mutex, NULL);
		//pthread_cond_init(&sender->cond,NULL);
		sender->flag      = FLAG_VALID;
		sender->isRunning = RUNNING;
		sender->readEnd   = FILE_NOT_READ_END;
	}while(0);

	return sender;
}


int closeFileSender(PT_FileSender sender)
{
	int ret = 0;
	
	if(!sender)
		return ret;

	//先让子线程 退出
	closeFileReader(sender->filereader);
	sender->filereader = NULL;
	closeSender(sender->sender);
	sender->sender = NULL;

	if(sender)
	{
		if(isFlagValid(sender))
		{
        	//pthread_mutex_lock(&sender->mutex);
        	//pthread_cond_signal(&sender->cond);
        	//pthread_mutex_unlock(&sender->mutex);
		}else
		{
			sender->isRunning = NOT_RUNNING;
			while(sender->isRunning == RUNNING_QUIT)
	        {
	            EB_LOGE(" running =%d flag = %d ",sender->isRunning,sender->flag);
	            usleep(200);
	        }//等待退出成功
		}

        //pthread_kill(tid, SIGTERM); //强制杀死
        //pthread_mutex_destroy(&sender->mutex);
        //pthread_cond_destroy(&sender->cond);

		free(sender);
		sender = NULL;

	}	
}

int FileSenderJoin(PT_FileSender filesender)
{
	pthread_join(filesender->pid,NULL);
	return 0;
}

