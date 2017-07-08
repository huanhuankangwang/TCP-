#include <filereceiver.h>
#include <pthread_define.h>

#include <string.h>
#include <malloc.h>

#include <easy_common.h>

static void *do_filereceiver_thread(void*arg)
{
	
	PT_FileReceiver recv = (PT_FileReceiver)arg;
	if(!recv)
		return NULL;
	char  tmp[1024];
	int ret,len;

	do
	{
		ret = readReceiver(recv->receiver,tmp,sizeof(tmp));
		if(ret >0)
		{
			len  = ret;
			ret = writeFileWriter(recv->writer,tmp,len);
			if(ret != len)
			{
				break;
			}
		}else if(ret < 0)
		{
			break;//准备退出
		}
	}while(recv->isRunning == RUNNING);

	if(recv->receiver)
		pthread_join(recv->receiver->pid,NULL);
	closeReceiver(recv->receiver);
	recv->receiver = NULL;
	
	closeFileWriter(recv->writer);
	recv->writer = NULL;
	//等待这两个线程退出
	recv->isRunning = RUNNING_QUIT;
	recv->flag      = FLAG_NOT_VALID;
}

PT_FileReceiver openFileReceiver(const char * filename,const char * remoteIp,
		int port,int bindPort,int filesize)
{
	PT_FileReceiver recv = NULL;
	int  ret;

	
	do
	{
		if(!filename || !remoteIp)
			break;

		recv = (PT_FileReceiver)malloc(sizeof(T_FileReceiver));
		if(!recv)
			break;

		memset(recv ,0 ,sizeof(T_FileReceiver));		
		recv->writer = openFileWriter(filename,1024*10);
		if(!recv->writer)
		{
			free(recv);
			recv = NULL;
			break;
		}
		EB_LOGE("openFileReceiver \r\n");

		recv->receiver = openReceiver(remoteIp,port,bindPort,filesize);
		if(!recv->receiver)
		{
			closeFileWriter(recv->writer);
			recv->writer = NULL;
			free(recv);
			recv = NULL;
			break;
		}

		EB_LOGE("openFileReceiver \r\n");

		ret = pthread_create(&recv->pid,NULL,do_filereceiver_thread,recv);
		if(ret != 0)
		{
			closeReceiver(recv->receiver);
			recv->receiver = NULL;
			closeFileWriter(recv->writer);
			recv->writer = NULL;
			free(recv);
			recv = NULL;
			break;	
		}

		recv->flag       = FLAG_VALID;
		recv->isRunning  = RUNNING;
	}while(0);

	EB_LOGE("openFileReceiver \r\n");
	return recv;
}

int closeFileReceiver(PT_FileReceiver recv)
{
	if(!recv)
		return 0;

	closeFileWriter(recv->writer);
	recv->writer = NULL;
	closeReceiver(recv->receiver);
	recv->receiver = NULL;	
	if(recv->flag == FLAG_VALID)
	{
		recv->isRunning = NOT_RUNNING;
		while(recv->isRunning != RUNNING_QUIT);
	}

	free(recv);
	recv = NULL;

	return 0;
}

int FileReceiverJoin(PT_FileReceiver  recv)
{
	pthread_join(recv->pid,NULL);
}

