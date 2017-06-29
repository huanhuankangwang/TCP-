#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fileReader.h>

void *do_read_thread(void*arg)
{
    int  ret = 0,len;
	char tmp[1024];
	PT_FileReader reader = (PT_FileReader)arg;
	do
	{
	    memset(tmp,0,sizeof(tmp));
        pthread_mutex_lock(&reader->mutex);
		ret = read_fd(reader->fd,tmp,sizeof(1024));
        if(ret <= 0)
        {
            break;
        }

        writeFileReader(reader,tmp,ret);
        pthread_mutex_unlock(&reader->mutex);

        usleep(1);
			
	}while(reader->isRunning);

    reader->isRunning = -1;//成功退出
}

PT_FileReader openFileReader(const char* filename,int bufSize)
{
	int ret;
	PT_FileReader filereader = NULL;

	do
	{
		filereader = malloc(sizeof(T_FileReader));
		if(!filereader)
		{
			break;
		}
		
		filereader->ringbuf = mallocRingBuffer(bufSize);
		if(!filereader->ringbuf)
		{
			free(filereader);
			filereader = NULL;
			break;
		}

		filereader->fd = openfile(filename,O_RDONLY);
		if(filereader->fd <0)
		{
			freeRingBuffer(filereader->ringbuf);
			filereader->ringbuf = NULL;
			free(filereader);
			filereader = NULL;
			break;
		}

		pthread_mutex_init(&filereader->mutex, NULL);
		pthread_cond_init(&filereader->cond,NULL);
		filereader->isRunning  = 1;

		ret = pthread_create(&filereader->pid,NULL,do_read_thread,(void*)filereader);
	    if(ret!=0)  
	    {  
			closefile(filereader->fd);
			freeRingBuffer(filereader->ringbuf);
			filereader->ringbuf = NULL;
			free(filereader);
			filereader = NULL;
			break;
	    }  

	}while(0);	

	return filereader;
}

int closeFileReader(PT_FileReader reader)
{
	if(reader)
	{
		reader->isRunning = 0;
        
        pthread_mutex_lock(&reader->mutex);
        pthread_cond_signal(&reader->cond);
        pthread_mutex_unlock(&reader->mutex);

        while(reader->isRunning != -1);//等待退出成功

        pthread_cond_destroy(&reader->cond);
		closefile(reader->fd);
		freeRingBuffer(reader->ringbuf);
		reader->ringbuf = NULL;
		free(reader);
		reader = NULL;
	}

	return 0;
}

int readFileReader(PT_FileReader reader,char *str,int maxsize)
{
	int  ret = 0,size = maxsize;

    pthread_mutex_lock(&reader->mutex);

    do
    {
       ret = readString(reader->ringbuf,str,size);
       if(ret <= 0)
       {
           maxsize -= size;
           if(maxsize <= 0)
           {
             maxsize = -1;//读取到文件尾巴
           }
           
           break;
       }
       size  -= ret;
       str  += ret;
    }while(size > 0);
    pthread_cond_signal(&reader->cond);
    pthread_mutex_unlock(&reader->mutex);

    return maxsize;
}

//不对外部开放的接口
int writeFileReader(PT_FileReader reader,char *str,int len)
{
    int  ret = 0,size = len;
    do
    {
       ret = writeString(reader->ringbuf,str,size);
       if(ret <= 0)
       {
          pthread_cond_wait(&reader->cond,&reader->mutex);
          //当环形缓冲区为满 时 阻塞在这里
       }
       size  -= ret;
       str  += ret;
    }while(size > 0);

    return len;
}
