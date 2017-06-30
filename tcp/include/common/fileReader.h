#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include <ringbuffer.h>
#include <pthread.h>

#define      END_OF_FILE        1
#define      NO_END_OF_FILE     0


typedef struct _fileReader
{
	PT_RingBuffer  ringbuf;
	int            fd;//文件句柄

	int  		   isRunning;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	pthread_t		pid;

    int         flag;//读取操作
}T_FileReader,*PT_FileReader;


//打开一个文件操作
PT_FileReader openFileReader(const char* filename,int bufSize);

//关闭一个文件操作
int closeFileReader(PT_FileReader filereader);


//读取文件里面的内容
/*************************************************************/
/*
    **********************************************************
    返回值  >0  实际读取文件数
            <0  文件结束/没有内容可读
*/
int readFileReader(PT_FileReader reader,char *str,int maxsize);

int isEof(PT_FileReader reader);
#endif//_FILE_READER_H_
