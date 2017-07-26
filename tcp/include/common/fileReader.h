#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include <ringbuffer.h>
#include <pthread.h>

#define      END_OF_FILE        1
#define      NO_END_OF_FILE     0


typedef struct _fileReader {
    PT_RingBuffer  ringbuf;
    int            fd;//�ļ����
    int            isRunning;
    int         flag;//��ȡ����

    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    pthread_t       pid;
} T_FileReader,*PT_FileReader;


//��һ���ļ�����
PT_FileReader openFileReader(const char* filename,int bufSize);

//�ر�һ���ļ�����
int closeFileReader(PT_FileReader filereader);


//��ȡ�ļ����������
/*************************************************************/
/*
    **********************************************************
    ����ֵ  >0  ʵ�ʶ�ȡ�ļ���
            <0  �ļ�����
            =0  /û�����ݿɶ�
*/
int readFileReader(PT_FileReader reader,char *str,int maxsize);

#endif//_FILE_READER_H_
