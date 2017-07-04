#include <fileReader.h>
#include <fileWriter.h>

#include <stdio.h>

#define   outfile   "temp"

#define  BUFFSIZE   (1024*20)

#define    USING_READER         1
#define    USING_WRITE          1

int main()
{
    int ret;
    char  tmp[200]={"wangkangwangkang\r\n this my test \r\n"};
    PT_FileWriter writer = NULL;

    if( create_file(outfile))
        return 1;
	
    writer = openFileWriter(outfile,BUFFSIZE);
    if(!writer)
    {
        return 1;
    }

    printf("main  do while Wpos =%d Rpos =%d\r\n",writer->ringbuf->mWritePos,writer->ringbuf->mReadPos);
    do{

#if 01
	   printf("mWritePos =%d mReadPos =%d\r\n",writer->ringbuf->mWritePos,writer->ringbuf->mReadPos);
       ret = writeFileWriter(writer,tmp,sizeof(tmp));
	   if(ret == sizeof(tmp))
	   {
	   	  printf(" writeFileWriter %d bytes data to file %s\r\n",ret,outfile);
		  break;
	   }

	   printf("%d\r\n",ret);
#endif
    }while(1);
   
quit:
    printf("will quit \r\n");
    closeFileWriter(writer);
    printf("all commplete\r\n");
    return 0;
}

