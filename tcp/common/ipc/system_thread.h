#ifndef _SYSTEM_THREAD_H_
#define _SYSTEM_THREAD_H_

typedef void (*CSUDIOSThreadEntry_F)(void * pvParam);


WT_Error_Code WTThreadCreate(const char * pcName,int nPriority,int nStackSize,
                    CSUDIOSThreadEntry_F fnThreadEntry,void * pvArg,WT_HANDLE * phThread);

WT_Error_Code WTThreadDestroy (WT_HANDLE hThread);
WT_Error_Code WTThreadSuspend(WT_HANDLE hThread);
WT_Error_Code WTThreadResume(WT_HANDLE hThread);
WT_Error_Code WTThreadJoin (WT_HANDLE hThread);

WT_Error_Code  WTThreadSelf(WT_HANDLE * hThread);

/*能够的的确确实现精确延时*/
void WTThreadSleep(unsigned int uMilliSeconds);
void WTThreadYield (void);



#endif//_SYSTEM_THREAD_H_
