#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <ringbuffer.h>
#include <fileoperation.h>


pthread_t  pid;
PT_RingBuffer ringbuf = NULL;


static void *do_ringbuffer_test_thread(void*arg)
{
	PT_RingBuffer buf = (PT_RingBuffer)ringbuf;
	char tmp[1024];
	int ret;

	do
	{
		memset(tmp,0,1024);
	    ret = readString(buf,tmp,1024);

		if(ret >0)
			printf("readstring =%s\r\n",tmp);
	}while(1);
}

int main()
{
	char  tmp[1024];
	int   len,ret;

	ringbuf = mallocRingBuffer(10);

	printf("sizeof(tmp) =%d\r\n", (int)sizeof(tmp));

	
	ret = pthread_create(&pid,NULL,do_ringbuffer_test_thread,ringbuf);
	if(ret <0)
	{
		printf("pthread_create err! \r\n");
		return 0;
	}

	do{
		memset(tmp,0,1024);
		scanf("%s",tmp);

		printf("tmp:%s\r\n",tmp);

		len  = strlen(tmp);
		if(strcmp(tmp,"") == 0)
		{
			printf("empty\r\n");
			continue;
		}

		ret = writeString(ringbuf,tmp,len);
	    if(ret != len)
		{
			printf("write to ringbuf err\r\n");
			break;
		}
		
	}while(1);

}

