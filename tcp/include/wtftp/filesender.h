#ifndef _FILE_SENDER_H_
#define _FILE_SENDER_H_

#include <sender.h>
#include <fileReader.h>


typedef struct _FileSender {
    PT_Sender      sender;
    PT_FileReader  filereader;

    int             readEnd;

    int             isRunning;
    int             flag;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    pthread_t      pid;
} T_FileSender,*PT_FileSender;


PT_FileSender openFileSender(char *filename,char *remoteIp,int port,int bindport,int filesize);
int closeFileSender(PT_FileSender filesender);

int FileSenderJoin(PT_FileSender filesender);

#endif//_FILE_SENDER_H_
