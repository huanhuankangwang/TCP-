#include "debugTagManager.h"

typedef struct _DebugTag{
    char  tag[50];
    struct _DebugTag *ptNext;
}T_DebugTag,*PT_DebugTag;

PT_DebugTag  *ptDebugTagHead = NULL;




#define    LOGCAT   "logcat"
#define    CLEAR    "-c"
#define    DEPART   ";"

#define    SELECT   "-s"
#define    USELECT  "-us"


int addDbgIgnoreTag(char *tag)
{
    PT_DebugTag  ptTem = NULL,ptTag = NULL;
    if(!tag)
    {
        return -1;
    }

    ptTem = malloc(sizeof(T_DebugTag));
    if(!ptTem)
    {
        return -1;
    }

    ptTem->ptNext = NULL;
    strncpy(ptTem->tag,tag,sizeof(ptTem->tag));

    do
    {
        ptTag = ptDebugTagHead;
        if(ptTag == NULL)
        {
            ptDebugTagHead = ptTem;
            break;
        }

        for(ptTag = ptDebugTagHead;ptTag->ptNext != NULL;ptTag = ptTag->ptNext);
        ptTag->ptNext = ptTem;
    }while(0);
    return 0;
}


int rmoveDbgIgnoreTag(char *tag)
{
    int ret = 0;
    PT_DebugTag  ptTem = NULL,pptTem = NULL;
    if(!tag)
    {
        return -1;
    }

    do
    {
        ptTem = ptDebugTagHead;
        if(ptTem == NULL)
        {
            ret = -1;
            break;
        }else if(strcmp(ptTem->tag,tag) == 0)
        {
            ptDebugTagHead = ptDebugTagHead->ptNext;
            free(ptTem);
            ptTem = NULL;
            break;
        }

        for(pptTem = ptTem,ptTem = ptTem->ptNext;ptTem != NULL;pptTem = ptTem,ptTem = ptTem->ptNext)
        {
            if(strcmp(ptTem->tag,tag) == 0)
            {
                pptTem->ptNext = ptTem->ptNext;
                free(ptTem);
                ptTem = NULL;
                break;
            }
        }
        ret = -1;
    }while(0);
    
    return ret;
}

int isExistDebugTag(char *tag)
{
    PT_DebugTag  ptTem = NULL;

    if(!tag)
        return -1;

    for(ptTem = ptDebugTagHead;ptTem != NULL;ptTem = ptTem->ptNext)
    {
        if(strcmp(ptTem->tag,tag) == 0)
        {
            return 0;
        }
    }
    
    return -1;
}


int execCmdDebugTag(char *cmd)
{
    if(cmd == NULL)
        return -1;

    
}


