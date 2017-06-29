#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fileReader.h>

#define   FILE_READER_DEBUG(...)

void *do_read_thread(void*arg)
{
    int  ret = 0,len;
	char tmp[1024];
	PT_FileReader reader = (PT_FileReader)arg;
	do
	{
	    memset(tmp,0,sizeof(tmp));
        pthread_mutex_lock(&reader->mutex);
        printf("lock in do_read_thread\r\n");
		ret = read_fd(reader->fd,tmp,sizeof(tmp));
        if(ret <= 0)
        {
            printf("commpeter read \r\n");
            reader->flag = END_OF_FILE;
            pthread_mutex_unlock(&reader->mutex);
            break;
        }

        printf("reader read file size =%d\r\n",ret);
        writeFileReader(reader,tmp,ret);

        printf("unlock in do_read_thread\r\n");
        pthread_mutex_unlock(&reader->mutex);
			
	}while(reader->isRunning);

    reader->flag = END_OF_FILE;
    reader->isRunning = NOT_RUNNING;//成功退出
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
		filereader->isRunning  = RUNNING;
        filereader->flag = NO_END_OF_FILE;

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

    usleep(2);

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

        printf("wait for exit %d\r\n",reader->isRunning);

        //pthread_kill(tid, SIGTERM); //强制杀死
        while(reader->isRunning == RUNNING)
        {
            printf(" running =%d flag = %d ",reader->isRunning,reader->flag);
            sleep(1);
        }//等待退出成功

        pthread_cond_destroy(&reader->cond);
		closefile(reader->fd);
		freeRingBuffer(reader->ringbuf);
		reader->ringbuf = NULL;
		free(reader);
		reader = NULL;
	}

    printf("closeFileReader\r\n");
	return 0;
}

int readFileReader(PT_FileReader reader,char *str,int maxsize)
{
	int  ret = 0,size = maxsize;

    pthread_mutex_lock(&reader->mutex);
    FILE_READER_DEBUG("lock in readFileReader\r\n");

    do
    {
       ret = readString(reader->ringbuf,str,size);
       if(ret <= 0)
       {
           FILE_READER_DEBUG("in readFileReader ret = %d running =%d flag = %d \r\n",ret,reader->isRunning,reader->flag);
           break;
       }
       size  -= ret;
       str  += ret;
    }while(size > 0);

    
    pthread_cond_signal(&reader->cond);
    FILE_READER_DEBUG("unlock in readFileReader\r\n");
    pthread_mutex_unlock(&reader->mutex);

    return maxsize -size;
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
          printf("wait reader\r\n");
          pthread_cond_wait(&reader->cond,&reader->mutex);
          //当环形缓冲区为满 时 阻塞在这里
       }
       size  -= ret;
       str  += ret;
    }while(size > 0);

    return len;
}

int isEof(PT_FileReader reader)
{
    return (reader->flag == END_OF_FILE);
}

