#include <fileReader.h>
#include <fileWriter.h>

#define   readfile  "makefile"
#define   outfile   "temp"

#define  BUFFSIZE   (1024*20)

#define    USING_READER         1
#define    USING_WRITE          1

int main()
{
    int ret;
    char  tmp[1024];
    PT_FileReader reader = NULL;
    PT_FileWriter writer = NULL;

    if( create_file(outfile))
        return 1;
#if (USING_READER || USING_WRITE)

    reader = openFileReader(readfile,BUFFSIZE);
    if(!reader)
        return 1;
#if (USING_WRITE)
    writer = openFileWriter(outfile,BUFFSIZE);
    if(!writer)
    {
        closeFileReader(reader);
        return 1;
    }
#endif

    printf("main  do while\r\n");
    do{
       ret = readFileReader(reader,tmp,sizeof(tmp));
       if(ret <=  0)
       {
         if(isEof(reader))
            break;
       }

       printf("read len =%d\r\n",ret);
#if (USING_WRITE)
       writeFileWriter(writer,tmp,ret);
#endif        
    }while(1);
   
quit:
    printf("will quit \r\n");
    closeFileReader(reader);
#if (USING_WRITE)
    closeFileWriter(writer);
#endif

#endif

    printf("all commplete\r\n");
    return 0;
}
