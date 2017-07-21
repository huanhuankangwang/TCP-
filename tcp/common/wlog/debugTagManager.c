#include "debugTagManager.h"


#include <string.h>
#include <stdio.h>
#include <malloc.h>


#define      DEBUG_TAG(...)
//#define      DEBUG_TAG   printf


typedef struct _DebugTag{
    char  tag[50];
    struct _DebugTag *ptNext;
}T_DebugTag,*PT_DebugTag;

static PT_DebugTag  ptDebugTagHead = NULL;


#define    LOGCAT   "logcat"
#define    CLEAR    "-c;"
#define    SELECT   "-s"
#define    UNSELECT  "-us"

static int addDbgIgnoreTag(char *tag)
{
    PT_DebugTag  ptTem = NULL,ptTag = NULL,pptTag;
    if(!tag)
    {
        return -1;
    }

    ptTem = malloc(sizeof(T_DebugTag));
    if(!ptTem)
    {
        return -1;
    }

    memset(ptTem,0,sizeof(T_DebugTag));
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

        for(ptTag = ptDebugTagHead;ptTag != NULL;ptTag = ptTag->ptNext)
        {
            pptTag  = ptTag;
            if(strcmp(ptTag->tag,tag) == 0)
            {
                printf("add same (((((((((((((((((((((((((((((((\r\n");
                ptTem->ptNext = NULL;
                free(ptTem);
                return -1;
            }
        }
        pptTag->ptNext = ptTem;
    }while(0);
    
    return 0;
}


static int rmoveDbgIgnoreTag(char *tag)
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

static int clearAllDebugTag()
{
	PT_DebugTag ptTag = NULL,pptTag= NULL;

	ptTag  = ptDebugTagHead;
	if(ptTag)
	{
	    ptTag = ptTag->ptNext;
		ptDebugTagHead = NULL;
		ptDebugTagHead->ptNext = NULL;
        free(ptDebugTagHead);
	}

	while(ptTag)
	{
		pptTag = ptTag;
		ptTag = ptTag->ptNext;
		pptTag->ptNext = NULL;
		free(pptTag);
	}

	return 0;
}


static int listAllDebugTag()
{
	PT_DebugTag ptTag = NULL ;

	ptTag  = ptDebugTagHead;

	while(ptTag)
	{
		
		printf("tag: [%s]\r\n",ptTag->tag);
		ptTag = ptTag->ptNext;
	}

	return 0;
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


char *strcpyc(char *dest,char *src,char ch)
{
	char *pdest = dest;
	//DEBUG_TAG("%c",*src);
	while(*src  && *src != ch){
		
		*dest = *src;
		//DEBUG_TAG("%c",*dest);
		dest++;
		src++;
	}

	*dest = '\0';
	return pdest;
}

static char *findChar(char *contain,char ch)
{
	char *pc = contain;
	if(pc == NULL)
		return NULL;

	while(*pc)
	{
		if(ch == *pc)
		{
			return ++pc;
		}
		pc++;
	}

	return NULL;
}


//logcat -c;logcat -s TAG -s TAG
int execCmdDebugTag(char *cmd)
{
	int ret =0,i = 0;
	char  *pc = cmd,*pcmd =cmd;
	
	char  cmds[20][50] ={0};
	char  nCmd = 0;

	char  cmdsting[10][50] ={0};
	int   cmdtype[10]={0};
	int   nCmdstring = 0;
    int   logcat = 0;

	while(pcmd)
	{
	    if(nCmd >= 20)
        {
            return -1;
        }   
		pcmd = findChar(pc,' ');
		strcpyc(cmds[nCmd],pc,' ');
		DEBUG_TAG("cmds[%d] =%s\r\n",nCmd,cmds[nCmd] );
        nCmd++;
		pc = pcmd;
	}
    

	printf("end**********************\r\n");

	for(i = 0;i< nCmd;i++)
	{
	    DEBUG_TAG("cmds[%d] =%s\r\n",i,cmds[i] );
		if(strcmp(cmds[i],LOGCAT) == 0)
		{
		    logcat++;
		}else
        if(strcmp(cmds[i],CLEAR) == 0 && logcat == 1)
        {
            DEBUG_TAG("cmd Clear\r\n");
            logcat = 0;
            strcpy(cmdsting[nCmdstring],cmds[i]);
            cmdtype[nCmdstring] = 2;
            nCmdstring++;
        }else if(strcmp(cmds[i],SELECT) == 0 && logcat == 1)
        {
            DEBUG_TAG("cmd SELECT\r\n");
            i++;
            if( i < nCmd)
            {
                strcpy(cmdsting[nCmdstring],cmds[i]);
                cmdtype[nCmdstring] = 3;
                nCmdstring++;
            }else
            {
                ret = -1;
                break;
            }
        }else if(strcmp(cmds[i],UNSELECT) == 0 && logcat == 1)
        {
            DEBUG_TAG("cmd UNSELECT\r\n");
            i++;
            if( i < nCmd)
            {
                strcpy(cmdsting[nCmdstring],cmds[i]);
                cmdtype[nCmdstring] = 4;
                nCmdstring++;
            }else
            {
                ret = -1;
                break;
            }
        }else
        {
            ret = -1;
            break;
        }

	}

	if(ret == 0)
	{//ÕýÈ·µÄÃüÁî
		for(i=0;i<nCmdstring;i++)
		{
			switch(cmdtype[i])
			{
				case 2:
					clearAllDebugTag();
					break;
				case 3:
					addDbgIgnoreTag(cmdsting[i]);
					break;
				case 4:
					rmoveDbgIgnoreTag(cmdsting[i]);
					break;
				default:
					break;
			}
		}	
	}
	
    return ret;
}
int test()
{
    int i = 0;
    char arry[10][50] ={"wangkang","tshi","tshi"};
    for(i=0;i<3;i++)
        printf("%s\r\n",arry[i]);
    for(i=0;i<4;i++)
    {
        scanf("%s",arry[i]);
    }

    for(i=0;i<4;i++)
        printf("%s\r\n",arry[i]);
}

#if 0
int main()
{
	char  cmds[1000];
	
    //test();
    sprintf(cmds,"%s","logcat -c; logcat -s wangkang -s tangwuke -us tangwuke");
	if( execCmdDebugTag( cmds ) != 0)
		printf("exec cmd error:%s \r\n",cmds);

    DEBUG_TAG("*************************************************\r\n");
    listAllDebugTag();
    DEBUG_TAG("*************************************************\r\n");
    
    sprintf(cmds,"%s","logcat -s wangkang -s tangwuke -us tangwuke -s kangwang -s Apmservice");
	if( execCmdDebugTag( cmds ) != 0)
		printf("exec cmd error:%s \r\n",cmds);

    DEBUG_TAG("*************************************************\r\n");
    listAllDebugTag();
    DEBUG_TAG("*************************************************\r\n");

    sprintf(cmds,"%s","logcat -s wangkang -s tangwuke -us tangwuke -s kangwang -s Apmservice -s wangTag");
	if( execCmdDebugTag( cmds ) != 0)
		printf("exec cmd error:%s \r\n",cmds);

    DEBUG_TAG("*************************************************\r\n");
    listAllDebugTag();
    DEBUG_TAG("*************************************************\r\n");	
}
#endif

