#ifndef _FILE_WRITER_H_
#define _FILE_WRITER_H_

#include <ringbuffer.h>
#include <pthread.h>

typedef struct _fileWriter
{
	PT_RingBuffer  ringbuf;
	int            fd;//文件句柄

	int  		   isRunning;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	pthread_t		pid;
}T_FileWriter,*PT_FileWriter;


//打开一个文件操作
PT_FileWriter openFileWriter(const char* filename,int bufSize);

//关闭一个文件操作
int closeFileWriter(PT_FileWriter writer);


//读取文件里面的内容
/*************************************************************/
/*
    **********************************************************
    返回值  >0  实际读取文件数
            <0  文件结束/没有内容可读
*/
int wiriteFileWriter(PT_FileWriter writer,char *str,int maxsize);


#endif//_FILE_WRITER_H_