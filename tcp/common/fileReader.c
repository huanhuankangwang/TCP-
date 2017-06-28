#include <ringbuffer.h>

typedef struct _fileReader
{
	PT_RingBuffer  * ringbuf = NULL;
	int            fd;//文件句柄

	int  		   isRunning;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	pthread_t		pid;
}T_FileReader,*PT_FileReader;


void *do_read_thread(void*arg)
{
	char tmp[1024];
	PT_FileReader reader = (PT_FileReader)arg;
	do
	{
		read_fd(reader->fd,tmp,sizeof(1024))
			
	}while(reader->isRunning)
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

		pthread_mutex_init(&filereader->mutex);
		pthread_cond_init(&filereader->cond);
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

int closeFileReader(PT_FileReader *filereader)
{
	if(filereader)
	{
		filereader->isRunning = 0;
		closefile(filereader->fd);
		freeRingBuffer(filereader->ringbuf);
		filereader->ringbuf = NULL;
		free(filereader);
		filereader = NULL;
	}

	return 0;
}

int readFileReader(char *str,int maxsize)
{
	
}

//不对外部开放的接口
int writeFileReader(char *str,int len)
{
	
}
