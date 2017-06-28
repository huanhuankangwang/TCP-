#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "messageRecord.h"
#include <pthread.h>


MessageRecord* next(MessageRecord *record) 
{
    return record->fNext;
}

unsigned cseq(MessageRecord *record) 
{
    return record->fCSeq;
}

MessageType getMessageType(MessageRecord *record)
{
    return record->messageType;
}

char* contentStr(MessageRecord *record)
{
    return record->fContentStr;
}


int free_record(MessageRecord *record)
{
	record->fNext = NULL;
	free(record->fContentStr);
	free(record);
	return 0;
}

MessageRecord* malloc_record(unsigned cseq, MessageType messageType, char const* contentStr)
{
	int len = 0 ;
	len = strlen(contentStr);
	
	if(len <= 0)
	{
		return NULL;
	}
	MessageRecord *record = malloc(sizeof(MessageRecord));
	
	if(record == NULL)
	{
		return NULL;
	}
	
	record->fContentStr = malloc(sizeof(char) * (len + 1));
	if(record->fContentStr == NULL)
	{
		free(record);
		record = NULL;
	}
	
	strcpy(record->fContentStr, contentStr);
	record->fCSeq = cseq;
	record->messageType = messageType;
	
	return record;
}