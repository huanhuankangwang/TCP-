
#ifndef _DEBUG_MANAGER_H
#define _DEBUG_MANAGER_H

#include "wlog.h"

#define DEFAULT_DBGLEVEL  4

typedef struct DebugOpr {
	char *name;
	int isCanUse;
	int (*DebugInit)(void);
	int (*DebugExit)(void);
	int (*DebugPrint)(char *strData);
	struct DebugOpr *ptNext;
}T_DebugOpr, *PT_DebugOpr;

int RegisterDebugOpr(PT_DebugOpr ptDebugOpr);
void ShowDebugOpr(void);
PT_DebugOpr GetDebugOpr(char *pcName);


#endif /* _DEBUG_MANAGER_H */
  
