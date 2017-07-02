#ifndef _FILE_RECEIVER_H_
#define _FILE_RECEIVER_H_

#include <receiver.h>
#include <fileWriter.h>

typedef struct
{
	PT_FileWriter  writer;
	PT_Receiver    receiver;

	int             isRunning;
	int 			flag;
	pthread_t      pid;
}T_FileReceiver,*PT_FileReceiver;


PT_FileReceiver openFileReceiver(const char * filename,const char * remoteIp,
		int port,int bindPort);

int closeFilesReceiver(PT_FileReceiver recv);


#endif//_FILE_RECEIVER_H_