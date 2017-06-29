#ifndef _FILE_WRITER_H_
#define _FILE_WRITER_H_

#include <ringbuffer.h>
#include <pthread.h>

typedef struct _fileWriter
{
	PT_RingBuffer  ringbuf;
	int            fd;//�ļ����

	int  		   isRunning;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	pthread_t		pid;
}T_FileWriter,*PT_FileWriter;


//��һ���ļ�����
PT_FileWriter openFileWriter(const char* filename,int bufSize);

//�ر�һ���ļ�����
int closeFileWriter(PT_FileWriter writer);


//��ȡ�ļ����������
/*************************************************************/
/*
    **********************************************************
    ����ֵ  >0  ʵ�ʶ�ȡ�ļ���
            <0  �ļ�����/û�����ݿɶ�
*/
int wiriteFileWriter(PT_FileWriter writer,char *str,int maxsize);


#endif//_FILE_WRITER_H_