#include <stdio.h>

#include <wthread.h>

void *wthread_calback(void*arg)
{
	int *status;
	char buf[30];
	if(arg == NULL)
		return NULL;
	status = (int*)arg;
	while(1)
	{
		scanf("%s",buf);
		if(strcmp("quit",buf) == 0)
		{
			*status = 2;
			break;
		}
	}
	return NULL;
}

int main()
{
	int ret = 0;
	int status = 0;
	WthreadHandle handle = NULL;
	
	//create wthread
	ret = wthread_create(&handle,wthread_calback,(void*)&status);

	while(1)
	{
		if(status == 2)
			break;
		sleep(2);
	}
	printf("wthread will be closed");
	wthread_close(handle);
	return -1;
}
