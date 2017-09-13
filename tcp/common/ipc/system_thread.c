#include "system_thread.h"

#include <pthread.h>
#include <errno.h>


#define        TASK_PRIOR_STEP          (17)

/*******************************************************************
 **         Task definitions                      **
 *******************************************************************/
WT_Error_Code WTThreadCreate(const char * pcName,int nPriority,int nStackSize, CSUDIOSThreadEntry_F fnThreadEntry,void * pvArg,WT_HANDLE * phThread)
{
    WT_Error_Code            enRet = WT_FAILURE;
    unsigned int             dwNameLen = 0;
    unsigned int             stacksize = 0;
    int                      nRetVal;
    int                      nNewPrio;
    int                      nTempPrio = nPriority/TASK_PRIOR_STEP;
    pthread_t                thread;
    pthread_attr_t           attr;

    if (phThread == WT_NULL || fnThreadEntry == WT_NULL)
    {
        WTASSERT(phThread != WT_NULL);
        WTASSERT(fnThreadEntry != WT_NULL);
        return WT_ERROR_BAD_PARAMETER;
    }

    if (nStackSize < 0 || nPriority < 0 || nPriority > 255)
    {
        WTASSERT((nStackSize > 0) && (nPriority >= 0 && nPriority <= 255));
        return WT_ERROR_BAD_PARAMETER;
    }


    WTASSERT(pcName != NULL);

    if(pcName != NULL)
    {
        dwNameLen = strlen(pcName);
    }

    if (dwNameLen > 32)
    {
        WTASSERT(dwNameLen < 32);
        return WT_ERROR_BAD_PARAMETER;
    }

    *phThread = WT_NULL;
    if(nTempPrio < 4)
    {
        nNewPrio = POSIX_TASK_LOWEST;
    }
    else if(nTempPrio  < 8)
    {
        nNewPrio = POSIX_TASK_LOW;
    }
    else if(nTempPrio  < 12)
    {
        nNewPrio = POSIX_TASK_NORMAL;
    }
    else if(nTempPrio  < 14)
    {
        nNewPrio = POSIX_TASK_HIGH;
    }
    else
    {
        nNewPrio = POSIX_TASK_HIGHEST;
    }

    nNewPrio = nNewPrio;//ȥwarning
    
    /*
        typedef struct __pthread_attr_s
        {
              int __detachstate;
              int __schedpolicy;
              struct __sched_param __schedparam;
              int __inheritsched;
              int __scope;
              size_t __guardsize;
              int __stackaddr_set;
              void *__stackaddr;
              size_t __stacksize;
        } pthread_attr_t;
    */
    nRetVal = pthread_attr_init(&attr);
#if 1 /*<!-- shenshaohui 2007/12/6 16:15:33 */
    WTASSERT(nRetVal == 0);
    if(nStackSize<0x4000)
    {
        nStackSize = 0x4000;
    }
    stacksize = (DWORD)nStackSize+NPTL_ADDED_STACK_SIZE;
    nRetVal = pthread_attr_setstacksize(&attr,stacksize);
    WTASSERT(nRetVal == 0);

    stacksize = 0;
    nRetVal = pthread_attr_getstacksize(&attr,(size_t *)&stacksize);
    WTASSERT(nRetVal == 0);

#endif /*0*//* shenshaohui 2007/12/6 16:15:33 --!>*/
    pthread_cancel_initialize();
    nRetVal = pthread_create_cancel (&thread, &attr, (void *(*)(void *))fnThreadEntry, pvArg);
    WTASSERT(nRetVal == 0);

    //nRetVal = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    //WTASSERT(nRetVal == 0);

    //nRetVal = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    //WTASSERT(nRetVal == 0);

    if (nRetVal == 0 && thread != 0)
    {
        /*use default shecdule no task priority*/
#if 0 /*<!-- shenshaohui 2007/12/13 16:07:27 */
        int nPolicy = SCHED_RR;
        param.sched_priority = nNewPrio;
        nRetVal = pthread_setschedparam (thread, nPolicy, &param);
        WTASSERT(nRetVal == 0);
#endif /*0*//* shenshaohui 2007/12/13 16:07:27 --!>*/

        *phThread = (WT_HANDLE)thread;

        enRet = WT_SUCCESS ;
    }

    nRetVal = pthread_attr_destroy(&attr);
    WTASSERT(nRetVal >= 0);

    //CSDEBUG(MODULE_NAME,INFO_LEVEL, "[CS_OS] Warnning: nStackSize can not be set!!.\n",nStackSize);
    //CSDEBUG(MODULE_NAME,INFO_LEVEL, "[CS_OS] CSCreateThread(%s,%d,0x%08x)\n",pstrName, nPriority,nStackSize);

    return enRet;
}

WT_Error_Code WTThreadDestroy (WT_HANDLE hThread)
{
    WT_Error_Code enRet = WT_FAILURE;
    int nRetVal;

    WTASSERT (hThread != WT_NULL);

    if (hThread == WT_NULL)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    nRetVal = pthread_cancel_cancel ((pthread_t)hThread, false);         /* true:judge busy  false:don't judge busy   */

    if (nRetVal == 1)
        return CSUDIOS_ERROR_THREAD_BUSY;

    nRetVal = pthread_join ((pthread_t) hThread, WT_NULL);    /* Wait for task to exit */

    WTASSERT(nRetVal == 0);

    enRet = WT_SUCCESS;

    return enRet;
}


WT_Error_Code WTThreadSuspend(WT_HANDLE hThread)
{
#if 0
    WT_Error_Code enRet = WT_FAILURE;

    WTASSERT (hThread != NULL);

    if (hThread == WT_NULL)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }
    if (pthread_kill((pthread_t)hThread, SIGSTOP) == 0)
    {
        enRet = WT_SUCCESS;
    }

    WTASSERT(enRet == WT_SUCCESS);

    return enRet;
#else
    CSSTD_UNUSED(hThread);
    return CSUDIOS_ERROR_FEATURE_NOT_SUPPORTED;
#endif
}

WT_Error_Code WTThreadResume(WT_HANDLE hThread)
{
#if 0
    WT_Error_Code enRet = WT_FAILURE;

    WTASSERT (hThread != NULL);

    if (hThread == WT_NULL)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    if (pthread_kill((pthread_t)hThread,SIGCONT) == 0)
    {
        enRet = WT_SUCCESS;
    }

    WTASSERT(enRet == WT_SUCCESS);

    return enRet;
#else
    WT_UNUSED(hThread);
    return CSUDIOS_ERROR_FEATURE_NOT_SUPPORTED;
#endif
}

WT_Error_Code WTThreadJoin (WT_HANDLE hThread)
{
    WT_Error_Code enRet = WT_FAILURE;

    WTASSERT (hThread != WT_NULL);

    if (hThread == WT_NULL)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    if (pthread_join((pthread_t)hThread,(void**)WT_NULL) == 0)
    {
        enRet = WT_SUCCESS;
    }

    return enRet;
}

WT_Error_Code  WTThreadSelf(WT_HANDLE * hThread)
{
    WT_Error_Code enRet = WT_FAILURE;

    WTASSERT (hThread != NULL);

    if (hThread == WT_NULL)
    {
        return WT_ERROR_BAD_PARAMETER;
    }

    if (WT_NULL != (*hThread = (WT_HANDLE)pthread_self()))
    {
        enRet = WT_SUCCESS;
    }

    return enRet;
}

static DWORD WTGetThreadId(VOID)
{
    return (DWORD)pthread_self();
}

void WTThreadSleep(unsigned int uMilliSeconds)
{
    struct timespec delay;
    struct timespec rem;
    int rc;

    if(uMilliSeconds==0)
    {
        return;
    }

    delay.tv_sec = (int)uMilliSeconds/1000;
    delay.tv_nsec = 1000 * 1000 * (uMilliSeconds%1000);

    for(;;) {
        rc = nanosleep(&delay, &rem); /* [u]sleep can't be used because it uses SIGALRM */
        if (rc!=0) {
            if (errno == EINTR) {
                delay = rem; /* sleep again */
                continue;
            }

            WTASSERT(0);

            return ;
        }
        break; /* done */
    }

    return;
}

void WTThreadYield (void)
{
    CSUDIOSThreadSleep(3);
}

