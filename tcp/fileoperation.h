#ifndef _FILE_OPERATION_H_
#define _FILE_OPERATION_H_


#define  FILENAME_LEN					1024



int read_from_file(const char *filename,const char *readBuf,int maxlen);
int write_to_file(const char *filename,const char *writeBuf,int maxlen);
#endif//_FILE_OPERATION_H_