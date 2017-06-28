#include<errno.h>
#include <sys/stat.h>  


#include <fileoperation.h>

long fileSize(const char *path)
{  
	long size = -1;	  
	struct stat statbuff;  
	if(stat(path, &statbuff) < 0){	
		return size;  
	}else{	
		size = statbuff.st_size;  
	}  
	return size;  
}  


int create_file(const char *filename)
{
	int fd;
	fd = openfile(filename,O_CREAT);
	if(fd <0)
	{
		return 1;
	}

	return 0;
}

int openfile(const char *filename,int mode)
{
	int fd;

	fd = open(filename,mode);
	if(fd <0)
	{
		printf("open file %s err reason: %s\r\n",filename,strerror(errno));
		return -1;
	}

	return fd;
}

int closefile(int fd)
{
	close(fd);
	return 0;
}


int read_fd(int fd,const char *buf,int maxsize)
{
	int len;
	int ret = 0;

	do{
		len = read(fd,buf,maxsize);//读出
		if(len < 0)
		{
			return -1;
		}if(len == 0)
			break;
		buf += len;
		maxsize -= len;
		ret += len;
	}while(maxsize > 0);

	return ret;
}

int write_fd(int fd,const char *buf,int size)
{
	int len;
	int ret = 0;
	do{
		len = write(fd,buf,size);
		if(len <= 0)
		{
			
			return -1;
		}
		buf += len;
		size -= len;
		ret += len;
	}while(size > 0);

	return ret;
}

int mkdirs(const char *dir)
{
    char tmp[1024];
    const char *p;
    int len,ret =0;

    p = dir;
    while(p != NULL)
    {
    	memset(tmp,0,sizeof(tmp));
        p = strchr(p+1, '/');
        if(p == NULL)
            break;
        
        len = p - dir;
        strncpy(tmp, dir, len);
        tmp[len] = 0;

        ret = mkdir(tmp, 0777);
		if(ret)
       	{
       		printf("mkdir %s err reason:%s\r\n",dir,strerror(errno));
       		break;
       	}
    }

	return ret;
}

int read_line(int fd,const char *buf,int maxsize)
{
	int len,ret;
	char *pbuf = buf;
	
	for(len =0; len < maxsize;len++,pbuf++)
	{
		ret = read(fd,pbuf,1);
		if(ret != 1)
		{
			break;
		}

		if(*pbuf == '\r')
		{
			ret = read(fd,pbuf,1); 
			if(ret != 1)
			{
				break;
			}

			if(*pbuf == '\n')
			{
				//是回车换行
				*pbuf = '\0';
				break;
			}
		}
	}

	return len;
}


