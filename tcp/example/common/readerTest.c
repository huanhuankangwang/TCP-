#include <fileReader.h>
#include <fileWriter.h>

#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define   readfile  "0.jpg"
#define   outfile   "1.jpg"

#define  BUFFSIZE   (1024*20)

#define    USING_READER         1


static void WidebrightSegvHandler(int signum) 
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i, j;

    signal(signum, SIG_DFL);

    size = backtrace (array, 10);
    strings = (char **)backtrace_symbols (array, size);

    fprintf(stderr, "widebright received SIGSEGV! Stack trace:\n");
    for (i = 0; i < size; i++) {
        fprintf(stderr, "%d %s \n",i,strings[i]);
    }

    free (strings);
    exit(1);
}

int main()
{
    int ret;
    char  tmp[1024];
    PT_FileReader reader = NULL;

	signal(SIGSEGV, WidebrightSegvHandler); // SIGSEGV      11       Core    Invalid memory reference
    signal(SIGABRT, WidebrightSegvHandler); // SIGABRT       6       Core    Abort signal from

	reader = openFileReader(readfile,BUFFSIZE);
    if(!reader)
	{
		printf("open fileReader err\r\n");
        return 1;
	}
    printf("main  do while\r\n");
    do{
       ret = readFileReader(reader,tmp,sizeof(tmp));
       if(ret <  0)
       {
          break;
       }else if(ret == 0)
       {
       	  //printf("no data read\r\n");
       	  continue;//没有读到数据
       }

       printf("read len =%d\r\n",ret);
    }while(1);
   
quit:
    printf("will quit \r\n");
    closeFileReader(reader);

    printf("all commplete\r\n");
    return 0;
}

