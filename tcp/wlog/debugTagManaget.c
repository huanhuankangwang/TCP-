#include "debugTagManager.h"


#include <string.h>
#include <stdio.h>
#include <malloc.h>

typedef struct _DebugTag{
    char  tag[50];
    struct _DebugTag *ptNext;
}T_DebugTag,*PT_DebugTag;

PT_DebugTag  ptDebugTagHead = NULL;


#define    LOGCAT   "logcat"
#define    CLEAR    "-c;"
#define    DEPART   ";"

#define    SELECT   "-s"
#define    UNSELECT  "-us"


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

int clearAllDebugTag()
{
	PT_DebugTag ptTag = NULL,pptTag= NULL;

	ptTag  = ptDebugTagHead;
	if(ptTag)
	{
		free(ptDebugTagHead);
		ptDebugTagHead = NULL;
		ptTag = ptTag->ptNext;
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


int listAllDebugTag()
{
	PT_DebugTag ptTag = NULL ;

	ptTag  = ptDebugTagHead; 

	while(ptTag)
	{
		
		printf("tag:%s",ptTag->tag);
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
	//printf("%c",*src);
	while(*src  && *src != ch){
		
		*dest = *src;
		printf("%c",*dest);
		dest++;
		src++;
	}

	*dest = '\0';
	printf("\r\n");
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

	printf("cmds[1] = %x cmds[2] =%x size = %d\r\n",cmds[1],cmds[2],cmds[2]-cmds[1]);

	while(pcmd)
	{
		pcmd = findChar(pc,' ');
		strcpyc(cmds[nCmd++],pc,' ');
		printf("cmds[%d] =%s\r\n",nCmd,cmds[nCmd] );
		pc = pcmd;
	}

	printf("end\r\n");

	for(i = 0;i< nCmd;i++)
	{
		if(strcmp(cmds[i],LOGCAT) == 0)
		{
			if( i < nCmd)
			{
				i++;
				if(strcmp(cmds[i],CLEAR) == 0)
				{
					printf("cmd clear\r\n");
					strcpy(cmdsting[nCmdstring],cmd[i]);
					cmdtype[nCmdstring] = 2;
					nCmdstring++;
				}else if(strcmp(cmds[i],SELECT) == 0)
				{
					i++;
					if( i < nCmd)
					{
						printf("cmd SELECT\r\n");
						strcpy(cmdsting[nCmdstring],cmd[i]);
						cmdtype[nCmdstring] = 3;
						nCmdstring++;
					}else
					{
						ret = -1;
						break;
					}
				}else if(strcmp(cmds[i],UNSELECT) == 0)
				{
					i++;
					if( i < nCmd)
					{
						printf("cmd UNSELECT\r\n");
						strcpy(cmdsting[nCmdstring],cmd[i]);
						cmdtype[nCmdstring] = 3;
						nCmdstring++;
					}else
					{
						ret = -1;
						break;
					}
				}else{
					printf("not cmds value\r\n");
					ret = -1;
					break;
				}
			}else{
				ret = -1;
				break;
			}
		}
		else
		{
			printf("not logcat \r\n");
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


int main()
{
	char  cmds[1000];
	
#if 0
	while(1)
	{
		memset(cmds,0,sizeof(cmds));
		scanf("%s",cmds);

		if(strcmp(cmds,"exec") == 0)
		{
			gets(cmds);
			memset(cmds,0,sizeof(cmds));
			//scanf("%[^\n]",cmds);
			gets(cmds);

			if( execCmdDebugTag(cmds) != 0)
				printf("exec cmd error:%s",cmds);
		}else if(strcmp(cmds,"list") == 0)
		{
			listAllDebugTag();
		}
	
	}

#else
		if( execCmdDebugTag("logcat -c; logcat -s wangkang") != 0)
				printf("exec cmd error:%s",cmds);
#endif
	
}
