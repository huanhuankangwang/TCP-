#ifndef _FILE_OPERATION_H_
#define _FILE_OPERATION_H_

#include <stdio.h>

/******************�ɹ� ���� 0 ���򷵻� ����***********************/
int create_file(const char *filename);

/******************************************************************/
/*
	**************���ļ� mode ��open �ĵڶ�������һ��***********
	*****************����ֵ  ���� ��ʧ�ܣ��ɹ����ش������ֵ****
	******************�ɹ�����Ϊ�ļ����**************************
*/
int openfile(const char *filename,int mode);
int closefile(int fd);


int read_fd(int fd,const char *buf,int maxsize);

int write_fd(int fd,const char *buf,int size);

int read_line(int fd,const char *buf,int maxsize);

int create_file(const char *filename);
#endif//_FILE_OPERATION_H_