#include <stdio.h>

#include <fileReader.h>
#include <fileWriter.h>
#include <fileoperation.h>


#define   READ_FILE  "0.jpg"
#define   OUT_FILE   "1.jpg"

#define  BUFFSIZE   (1024*20)

#define    USING_READER         1
#define    USING_WRITE          1

int main(int argc ,char **argv)
{
    int ret;
    char  tmp[1024];
    PT_FileReader reader = NULL;
    PT_FileWriter writer = NULL;

	char readfile[100] = READ_FILE;
	char outfile[100]  = OUT_FILE;
	if( argc == 3)
	{
		strcpy(readfile,argv[1]);
		strcpy(outfile,argv[2]);
	}

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
       if(ret <  0)
       {
         //if(isEof(reader))
            break;
       }else if(ret == 0){
			continue;
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
