#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser_list_file.h"

//文件名长度
#define   FULL_FILENAME_LEN			256
//文件个数
#define   MAX_FILE_SIZE				200
#define   TXT_READ_SIZE_ONCE		(1024*2)

static   int   file_size = 0;
unsigned char  list_file[MAX_FILE_SIZE][FULL_FILENAME_LEN]={0};

void mkdirs(const char *dir)
{
    char tmp[1024];
    const char *p;
    int32_t len;

    p = dir;
    while(p != NULL)
    {
        p = strchr(p+1, '/');
        if(p == NULL)
            break;
        
        len = p - dir;
        strncpy(tmp, dir, len);
        tmp[len] = 0;

        mkdir(tmp, 0777);
    }
}

void splitpath(char *path, char *dir, char *filename)
{
	char *p_whole_name;
	
	if (NULL == path)
	{
		dir[0] = '\0';
		filename[0] = '\0';
		return;
	}

	p_whole_name = rindex(path, '/');
	if (NULL != p_whole_name)
	{//整个名字
		p_whole_name++;
		
		snprintf(dir, p_whole_name - path +1, "%s", path);
		strcpy( filename , p_whole_name );
	}
	else
	{
		strcpy( filename , p_whole_name );
		dir[0] = '\0';
	}
  
}

unsigned int parse_file_list(const unsigned char *filename)
{
	unsigned char *pbuf=0;
	unsigned char buf[TXT_READ_SIZE_ONCE]={0};
	int file_size =0,file_number =0;
	int i,j=0;
	FILE *fp=0;

	fp = fopen(filename,"r");
	if(fp <= 0)
		printf("file  %s open error!\n",filename);
	else
	{
		fseek(fp,0,SEEK_END);
		file_size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		
		if(file_size <=0)	
			return 0;
		else if(file_size>TXT_READ_SIZE_ONCE)
			file_size = TXT_READ_SIZE_ONCE;

		pbuf = buf;
		fread(buf,file_size,1,fp);

		printf("debug file_size =%d\r\n",file_size);

		file_number = 0;
		memset(list_file[file_number],0,FULL_FILENAME_LEN);
 		for(i=0,j=0;i< file_size ;i++,pbuf++)
		{	
			
			if(*pbuf=='\0')
			{
				printf("file read end\n");
				break;
			}
			if(*pbuf=='\r' && *(pbuf+1)=='\n') 
			{
				pbuf++;
				file_number++;
				j=0;
				memset(list_file[file_number],0,FULL_FILENAME_LEN);
				continue;
			}
			
			list_file[file_number][j++] = *pbuf;
		}		
	}
	
exit:
	if(fp)
		fclose(fp);
		
	return file_number;
}

unsigned char *get_filename(int index)
{
	if(index >= MAX_FILE_SIZE)
		return NULL;
	return list_file[index];
}
#if 0
void main()
{
	unsigned char  filename[128]= {0};
	unsigned char  dir[128]={0};
	
	int  file_size = parse_file_list("read/List.txt");
	int i=0;
	for(i=0;i<file_size;i++)
	{
		splitpath(list_file[i],dir,filename);

		printf("full path =%s filename =%s dir =%s\r\n",list_file[i],filename,dir);
		mkdirs(dir);
	}
}
#endif
