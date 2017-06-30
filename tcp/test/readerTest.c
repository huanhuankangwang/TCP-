#include <fileReader.h>
#include <fileWriter.h>

#define   readfile  "0.jpg"
#define   outfile   "1.jpg"

#define  BUFFSIZE   (1024*20)

#define    USING_READER         1

int main()
{
    int ret;
    char  tmp[1024];
    PT_FileReader reader = NULL;

    if( create_file(outfile))
        return 1;
    reader = openFileReader(readfile,BUFFSIZE);
    if(!reader)
        return 1;

    printf("main  do while\r\n");
    do{
       ret = readFileReader(reader,tmp,sizeof(tmp));
       if(ret <=  0)
       {
         if(isEof(reader))
            break;
       }

       printf("read len =%d\r\n",ret);
    }while(1);
   
quit:
    printf("will quit \r\n");
    closeFileReader(reader);

    printf("all commplete\r\n");
    return 0;
}

