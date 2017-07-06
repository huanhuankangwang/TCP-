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
		len  = ret;
		if(len >0)
		{
		    printf("writeFileWriter =%d\r\n",ret);
			ret = writeFileWriter(recv->writer,tmp,len);
			if(ret != len)
			{
				break;
			}
		}
	}while(recv->isRunning == RUNNING);

	//等待这两个线程退出
		if(recv->writer)
			pthread_join(recv->writer->pid,NULL);
		if(recv->receiver)
			pthread_join(recv->receiver->pid,NULL);

	recv->isRunning = RUNNING_QUIT;
	recv->flag      = FLAG_NOT_VALID;
}

PT_FileReceiver openFileReceiver(const char * filename,const char * remoteIp,
		int port,int bindPort)
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

		recv->receiver = openReceiver(remoteIp,port,bindPort);
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

