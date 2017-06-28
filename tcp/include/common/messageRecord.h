#ifndef _MESSAGE_RECORD_H_
#define _MESSAGE_RECORD_H_


#define     MessageType int

typedef struct _MessageRecord
{
    struct _MessageRecord* fNext;
    int fCSeq;
    MessageType messageType;
    char* fContentStr;
}MessageRecord;


MessageRecord* next(MessageRecord *record);
unsigned cseq(MessageRecord *record);
char* contentStr(MessageRecord *record);
int free_record(MessageRecord *record);
MessageRecord* malloc_record(unsigned cseq, MessageType messageType, char const* contentStr);


#endif//_MESSAGE_RECORD_H_