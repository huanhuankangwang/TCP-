#include "fileoperation.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


int create_file(const char *filename)
{
	int ret = -1,fd;
	fd = open(filename,O_CREAT);
	if(fd > 0){
		ret = 0;
		close(fd);
	}
	return ret;
}
int _read_fd(int fd,const char *buf,int maxsize)
{
	int len;
	int ret = 0;
	printf("begin\r\n");
	do{
		len = read(fd,buf,maxsize);//¶Á³ö
		if(len < 0)
		{
			printf("len\r\n");
			return -1;
		}if(len == 0)
			break;
		buf += len;
		maxsize -= len;
		ret += len;
	}while(maxsize > 0);

	return ret;
}
int _write_fd(int fd,const char *buf,int size)
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
int read_from_file(const char *filename,const char *readBuf,int maxlen)
{
	int ret,fd;
	if(filename == NULL || readBuf == NULL)
		return 0;
	printf("enter read_from_file\r\n ");
	fd = open(filename,O_RDONLY);
	if(fd <0){
		printf("open filename=%s failed\r\n",filename);
		return -1;
	}
	printf("enter _read_fd\r\n ");
	ret = _read_fd(fd,readBuf,maxlen);

	close(fd);

	return ret;
}


int write_to_file(const char *filename,const char *writeBuf,int maxlen)
{
	int ret,fd;
	if(filename == NULL || writeBuf == NULL)
		return 0;

	ret = create_file(filename);
	if(ret < 0){
		printf("create file %s failure!\n",filename);
		return ret;
	}

	fd = open(filename,O_WRONLY);

	if(fd <0){
		printf("open filename=%s failed\r\n",filename);
		return -1;
	}

	_write_fd(fd,writeBuf,maxlen);

	close(fd);
	return 1;
}

#if 0
int main()
{	
	#define  MAX_LEN	  (1024*20)
	#define  WRITE_FILE		"write.jpeg"
	#define  READ_FILE		"read.jpeg"

	
	unsigned char  readbuf[MAX_LEN];
	unsigned char  writebuf[MAX_LEN];

	int len;

	len = read_from_file(READ_FILE,readbuf,MAX_LEN);
	printf("exit read_from_file\r\n ");
	if(len < 0)
		printf("read_from_file error\r\n");

	write_to_file(WRITE_FILE,readbuf,len);
	//just for testing
}

#endif
