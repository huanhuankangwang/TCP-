#include <filesender.h>
#include <pthread_define.h>

#include <malloc.h>

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
		if(ret <0)
		{
			break;
		}
		
	}while(sender->isRunning == RUNNING);

	sender->flag      = FLAG_NOT_VALID;
	sender->isRunning = RUNNING_QUIT;
}

PT_FileSender openFileSender(char *filename,char *remoteIp,int port,int bindport)
{
	int ret;
	PT_FileSender  sender = NULL;
	do
	{
		sender  = malloc(sizeof(T_FileSender));
		if(!sender)
			break;
		sender->filereader = openFileReader(filename,1024*20);
		if(!sender->filereader)
		{
			free(sender);
			sender = NULL;
			break;
		}

		sender->sender = openSender(remoteIp,port,bindport);
		if(!sender->sender)
		{
			closeFileReader(sender->filereader);
			sender->filereader = NULL;
			free(sender);
			sender = NULL;
			break;
		}

		ret = pthread_create(&sender->pid,NULL,do_fileSender_thread,(void*)sender);
		if(ret < 0)
		{
			closeSender(sender->sender);
			sender->sender = NULL;
			closeFileReader(sender->filereader);
			sender->filereader = NULL;
			free(sender);
			sender = NULL;
			break;			
		}

		sender->flag      = FLAG_VALID;
		sender->isRunning = RUNNING;
	}while(0);

	return sender;
}


int closeFileSender(PT_FileSender sender)
{
	int ret = 0;
	
	if(!sender)
		return ret;

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
	            printf(" running =%d flag = %d ",sender->isRunning,sender->flag);
	            usleep(200);
	        }//等待退出成功
		}

        //pthread_kill(tid, SIGTERM); //强制杀死
        //pthread_mutex_destroy(&sender->mutex);
        //pthread_cond_destroy(&sender->cond);
		closeSender(sender->sender);
		sender->sender = NULL;
		closeFileReader(sender->filereader);
		sender->filereader = NULL;
		free(sender);
		sender = NULL;

	}	
}

