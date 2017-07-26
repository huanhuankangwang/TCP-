#ifndef _FILE_WRITER_H_
#define _FILE_WRITER_H_

#include <ringbuffer.h>
#include <pthread.h>

#pragma pack(4)
typedef struct _fileWriter {
    PT_RingBuffer  ringbuf;
    int            fd;//文件句柄

    int            isRunning;
    int            flag;//读取操作
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    pthread_t       pid;
} T_FileWriter,*PT_FileWriter;
#pragma pack()

//打开一个文件操作
PT_FileWriter openFileWriter(const char* filename,int bufSize);

//关闭一个文件操作
int closeFileWriter(PT_FileWriter writer);


//写入文件里面的内容
/*************************************************************/
/*
    **********************************************************
    返回值  >=0  实际写入文件数
            <0  写入失败
*/
int writeFileWriter(PT_FileWriter writer,char *str,int len);

int FileWriter_isRuning(PT_FileWriter writer);


#endif//_FILE_WRITER_H_