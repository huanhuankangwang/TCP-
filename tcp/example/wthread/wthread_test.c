#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <wthread.h>

void *start(void*arg)
{
    int ff = *(int*)arg;
	do{
		sleep(1);
		printf("continue %d\r\n",ff);
	}while(1);

	printf("exit start\r\n");
}

int main()
{
    pthread_t  pid;
    int   ret;
	char  tmp[100];


  	ret = wthread_create(&pid,NULL,start,NULL);
	if(ret < 0)
	{
		printf("wthread_create error\r\n");
        return 1;
	}
    sleep(1);

	while(1)
	{
		memset(tmp,0,sizeof(tmp));

		scanf("%s",tmp);
		if(strcmp(tmp,"quit") == 0)
		{
			wthread_close(pid);
            break;
		}
	}

    wthread_join(pid);//µÈ´ýÍË³ö
	return 0;
}

