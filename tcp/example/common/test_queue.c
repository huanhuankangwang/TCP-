#include <stdio.h>
#include <string.h>

#include <messageQueue.h>

#define          CMD_NULL       ""
#define          CMD_ADD        "add"
#define          CMD_DEL        "del"
#define          CMD_DEQUEUE    "dequeue"
#define          CMD_LIST       "list"
#define          CMD_EXIT       "exit"



int print_queue(MessageQueue *queue)
{
    MessageRecord *p = queue->fHead;
    while(p)
    {
        printf("list cseq = %d  sucess\r\n",p->fCSeq);
        if(p == queue->fTail)
            break;
        p = p->fNext;
    }

   return 0;
}


int main()
{
    MessageQueue  *queue = NULL;
    char   cmd[100] = {0};
    char   context[]="test";
    int  Cseq = 1,cseq;
    MessageRecord  *record = NULL;

    queue = malloc_messageQueue();
    if(!queue)
    {
        printf("malloc_messageQueue err \r\n");
        return -1;
    }

    while(1)
    {
        memset(cmd,0,sizeof(cmd));
        scanf("%s",cmd);
        if(strcmp(cmd,CMD_ADD) == 0)
        {
            record = malloc_record(Cseq,0,context,strlen(context));
            if(!record)
            {
                printf("malloc_record err \r\n");
                break;
            }

            Cseq++;

            enqueue(queue,record);
            printf("add cseq = %d  sucess\r\n",record->fCSeq);
        }else
        if(strcmp(cmd,CMD_DEL) == 0)
        {
             scanf("%d",&cseq);
             if(cseq <= 0)
             {
                continue;
             }

            record = removeOneByCseq(queue,cseq);
            if(!record)
            {
                printf("cannot find cseq =%d\r\n",cseq);
                continue;
            }

            printf("rm  cseq =%d sucess\r\n",cseq);
            free_record(record);
            record = NULL;
        }else
        if(strcmp(cmd,CMD_DEQUEUE) == 0)
        {
            record = dequeue(queue);
            if(!record)
            {
                printf("dequeue err \r\n");
                continue;
            }

            printf("dequeue cseq = %d  sucess\r\n",record->fCSeq);
            free_record(record);
            record = NULL;
        }else
        if(strcmp(cmd,CMD_EXIT) == 0)
        {
            free_messageQueue(queue);
            queue = NULL;
            break;
        }else
        if(strcmp(cmd,CMD_LIST) == 0)
        {
            print_queue(queue);
        }else
        {

            continue;
        }
        
    }
    return 0;
}

