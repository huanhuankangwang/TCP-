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
	int ret = 0,len =0;

    while(maxsize > 0)
    {
        ret = read(fd,buf,maxsize);
        if(ret <=0)
        {
            printf("read fd=%d reason: %s\r\n",fd,strerror(errno));
            break;
        }

        buf += ret;
        maxsize -= ret;
        len += ret;
    }

	return len;
}

int write_fd(int fd,const char *buf,int size)
{
    int ret = 0,len =0;

	while(size > 0)
    {
		ret = write(fd,buf,size);
		if(ret <= 0)
        {
            printf("write fd=%d reason: %s\r\n",fd,strerror(errno));
            break;
		}
		buf += ret;
		size -= ret;
		len += ret;
	}while(size > 0);

	return len;
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
				//�ǻس�����
				*pbuf = '\0';
				break;
			}
		}
	}

	return len;
}

