#include <config.h>
#include <debug_manager.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>


static PT_DebugOpr g_ptDebugOprHead;
static int g_iDbgLevelLimit = 8;

#define   STR(s)      #s

int RegisterDebugOpr(PT_DebugOpr ptDebugOpr)
{
	PT_DebugOpr ptTmp;

	if (!g_ptDebugOprHead)
	{
		g_ptDebugOprHead   = ptDebugOpr;
		ptDebugOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDebugOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDebugOpr;
		ptDebugOpr->ptNext = NULL;
	}

	return 0;
}

void ShowDebugOpr(void)
{
	int i = 0;
	PT_DebugOpr ptTmp = g_ptDebugOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_DebugOpr GetDebugOpr(char *pcName)
{
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}


/* strBuf = "dbglevel=6" */
int SetDbgLevel(char *strBuf)
{
	g_iDbgLevelLimit = strBuf[9] - '0';
	return 0;
}

/*
 * stdout=0			   : 关闭stdout打印
 * stdout=1			   : 打开stdout打印
 * netprint=0		   : 关闭netprint打印
 * netprint=1		   : 打开netprint打印
 */

int SetDbgChanel(char *strBuf)
{
	char *pStrTmp;
	char strName[100];
	PT_DebugOpr ptTmp;

	pStrTmp = strchr(strBuf, '=');
	if (!pStrTmp)
	{
		return -1;
	}
	else
	{
		strncpy(strName, strBuf, pStrTmp-strBuf);
		strName[pStrTmp-strBuf] = '\0';
		ptTmp = GetDebugOpr(strName);
		if (!ptTmp)
			return -1;

		if (pStrTmp[1] == '0')
			ptTmp->isCanUse = 0;
		else
			ptTmp->isCanUse = 1;
		return 0;
	}
	
}

//打印格式 时间 打印级别 TAG [函数名,行号] 打印信息
int DebugPrint(char *level,char *tag,const char *pcFormat, ...)
{
	char strTmpBuf[1000];
    char printBuf[1000+100];
	char *pcTmp;
	va_list tArg;
	int iNum;
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	int dbglevel = DEFAULT_DBGLEVEL;
    struct tm *ptm;
    long ts;
    int y,m,d,h,n,s;
    char timeInfo[48];

    if(level == NULL || tag == NULL)
    {
        return -1;
    }
	
	/* 根据打印级别决定是否打印 */
	if ((level[0] == '<') && (level[2] == '>'))
	{
		dbglevel = level[1] - '0';
		if (dbglevel < 0 && dbglevel > 9)
		{
			dbglevel = DEFAULT_DBGLEVEL;
		}
	}

	if (dbglevel > g_iDbgLevelLimit)
	{
		return -1;
	}

    if( isExistDebugTag(tag) == 0)
    {
        return -1;
    }

    va_start (tArg, pcFormat);
	iNum = vsprintf (strTmpBuf, pcFormat, tArg);
	va_end (tArg);
	strTmpBuf[iNum] = '\0';
    
    ts = time(NULL);
    ptm = localtime(&ts);
    y   =   ptm-> tm_year+1900;
    m   =   ptm-> tm_mon+1;
    d   =   ptm-> tm_mday;
    h   =   ptm-> tm_hour;
    n   =   ptm-> tm_min;
    s   =   ptm-> tm_sec;
    sprintf(timeInfo, "[%02d-%02d-%02d %02d:%02d:%02d] [%s]",y, m,d,h,n,s,level);

    sprintf(printBuf,"[%s] [%s] [%s] [%s]:",timeInfo,level,tag,strTmpBuf);
    pcTmp = printBuf;

	/* 调用链表中所有isCanUse为1的结构体的DebugPrint函数 */
	while (ptTmp)
	{
		if (ptTmp->isCanUse)
		{
			ptTmp->DebugPrint(pcTmp);
		}
		ptTmp = ptTmp->ptNext;
	}

	return 0;
	
}

int DebugInit(void)
{
	int iError;

	iError = StdoutInit();
	iError |= NetPrintInit();
	return iError;
}

int InitDebugChanel(void)
{
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	while (ptTmp)
	{
		if (ptTmp->isCanUse && ptTmp->DebugInit)
		{
			ptTmp->DebugInit();
		}
		ptTmp = ptTmp->ptNext;
	}

	return 0;

}

