#ifndef _FILE_OPERATION_H_
#define _FILE_OPERATION_H_

#include <stdio.h>

/******************成功 返回 0 否则返回 非零***********************/
int create_file(const char *filename);

/******************************************************************/
/*
	**************打开文件 mode 与open 的第二个参数一致***********
	*****************返回值  负数 打开失败，成功返回大于零的值****
	******************成功返回为文件句柄**************************
*/
int openfile(const char *filename,int mode);
int closefile(int fd);


int read_fd(int fd,const char *buf,int maxsize);

int write_fd(int fd,const char *buf,int size);

int read_line(int fd,const char *buf,int maxsize);

int create_file(const char *filename);
#endif//_FILE_OPERATION_H_