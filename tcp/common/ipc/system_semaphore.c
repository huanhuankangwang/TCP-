typedef struct _Semaphore_S
{
    char  strName[32];
    char*  pstrStatus;
    unsigned int nWaitTimeout;
    sem_t *sem; 
    pthread_t thread; 
    BOOL bUsed;
    int waited_count;
}WTSemaphore_S;

#define MAX_UDI_SEMS 2000
static BOOL bSemsInited = FALSE;
static sem_t *udisem_protect = NULL;
static WTSemaphore_S udi_sems[MAX_UDI_SEMS];

void CSUDIOSDump()
{
    int nIndex;
    CSDebugSet("CSUDIOSDump", DEBUG_LEVEL);
    CSDebug("CSUDIOSDump",DEBUG_LEVEL,"udi semphores info:\n");
    for(nIndex = 0; nIndex < MAX_UDI_SEMS;nIndex++)
    {
        if(udi_sems[nIndex].bUsed)
        {
            CSDebug("CSUDIOSDump",DEBUG_LEVEL,"udi_sem[%d]={%s,%s,%d,0x%x %d}",nIndex,udi_sems[nIndex].strName,udi_sems[nIndex].pstrStatus,udi_sems[nIndex].nWaitTimeout,udi_sems[nIndex].thread,udi_sems[nIndex].waited_count);
        }
    }
}
/*******************************************************************
 **                     Semaphore definitions                     **
 *******************************************************************/
WT_Error_Code CSUDIOSSemCreate(const char * pstrName,int nInitialCount,int nMaxCount,WT_HANDLE * phSemaphore)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;
    DWORD dwNameLen = 0;

    CSASSERT(pstrName != NULL);

    if(bSemsInited == FALSE)
    {
        memset(udi_sems,0,sizeof(udi_sems));
        bSemsInited = TRUE;
        udisem_protect = ( sem_t *)malloc( sizeof( sem_t ) );
        if(udisem_protect)
        {
            if(sem_init( udisem_protect, 0, 1)!=0  )
            {
                 free(udisem_protect);
                 udisem_protect = NULL;
            }
        }
    }
    if(pstrName != NULL)
    {
        dwNameLen = strlen(pstrName);
    }

    CSASSERT((dwNameLen <= 32) && (nMaxCount >= nInitialCount) && (nInitialCount >= 0) && (nMaxCount > 0));
    if ((dwNameLen <= 32) && (nMaxCount >= nInitialCount) && (nInitialCount >= 0) && (nMaxCount > 0))
    {
        int nRet = 1;

        sem_t *sem = NULL;

        *phSemaphore = NULL;

        sem = ( sem_t *)malloc( sizeof( sem_t ) ); /* allocate Memory  */

        CSASSERT(sem != NULL);

        if ( sem != NULL )
        {
            /*the semaphore is local to the current process( pshared is zero ) */
            nRet = sem_init( sem, 0, (DWORD)nInitialCount );
            /*return 0 on success and -1 on unknown error */
            if ( nRet == 0 )
            {
                int nIndex;
                if(udisem_protect)
                {
                    sem_wait((sem_t *)udisem_protect);
                }
                for(nIndex = 0;nIndex < MAX_UDI_SEMS; nIndex++)
                {
                    if(udi_sems[nIndex].bUsed == FALSE)
                    {
                        udi_sems[nIndex].bUsed  = TRUE;
                        udi_sems[nIndex].sem = sem;
                        if(pstrName)
                        {
                            strcpy(udi_sems[nIndex].strName,pstrName);
                        }
                        else
                        {
                            udi_sems[nIndex].strName[0] = 0;
                        }
                        udi_sems[nIndex].pstrStatus = "Created";
                        udi_sems[nIndex].waited_count = 0;
                        udi_sems[nIndex].thread = pthread_self();
                        *phSemaphore = (WT_HANDLE)&udi_sems[nIndex];
                        enRet = CSUDI_SUCCESS;
                        break;
                    }
                }
                if(udisem_protect)
                {
                    sem_post((sem_t *)udisem_protect);
                }
            }
            else
            {
                CSASSERT(0);
                free( sem ); /* free memory.  */
            }
        }
    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }

    return enRet;
}

WT_Error_Code CSUDIOSSemWait(WT_HANDLE hSemaphore,unsigned int dwTimeout)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;

    WTSemaphore_S *udi_sem = (WTSemaphore_S *)hSemaphore;
    CSASSERT(hSemaphore != NULL);

    if (udi_sem != NULL)
    {
        int nRet;
          udi_sem->waited_count += 1 ;
        if(dwTimeout == 0xFFFFFFFF)
        {
 /*Jameswen modify for wait time 0xFFFFFFF to muti 60 sec 2011-12-10*/          
#if 1           
                    while(1)
                    {             
                  struct timespec ts;
#if USE_SYSTEM_TIME_MONOTONIC
            signed long long dwMinitime;
            dwMinitime = getsystemtime(SYSTEM_TIME_MONOTONIC);
            ts.tv_sec = dwMinitime/1000000000LL + (int)(60);
            ts.tv_nsec = dwMinitime%1000000000LL + (int)((100)*1000000);
#else
            struct timeval tv;
            nRet = gettimeofday (&tv, NULL);
            CSASSERT(nRet == 0);
            ts.tv_sec = tv.tv_sec + (int)(60);
            ts.tv_nsec = tv.tv_usec*1000 + (int)(100)*1000000;
#endif          
            adjusttimespec(&ts);

#if USE_SYSTEM_TIME_MONOTONIC
            nRet = sem_timedwait_monotonic((sem_t *)udi_sem->sem, &ts);
#else
            nRet = sem_timedwait((sem_t *)udi_sem->sem, &ts);
#endif
             if (nRet == 0)
            {
                enRet = CSUDI_SUCCESS;
                break;
            }
            
            
            enRet = CSUDIOS_ERROR_TIMEOUT;
            //zdd test
            
//            dump_stack_trace();
              
           }
    
#else        
              nRet = sem_wait((sem_t *)udi_sem->sem);
            if (nRet == 0)
            {
                enRet = CSUDI_SUCCESS;
            }
            else
            {
                //printf("Wait(%d)nRet is %d\n", dwTimeout, nRet);
                enRet = CSUDI_FAILURE;
            }

#endif 
/*Jameswen modify end 2011-12-10*/          
        }
        else if (dwTimeout == 0)
        {
            nRet = sem_trywait((sem_t *)udi_sem->sem);
            if (nRet == 0)
            {
                enRet = CSUDI_SUCCESS;
            }
            else if(errno == EAGAIN)
            {
                enRet = CSUDIOS_ERROR_TIMEOUT;
            }
            else
            {
                /*printf("Wait(%d)nRet is %d\n", dwTimeout, nRet);*/
                enRet = CSUDI_FAILURE;
            }
        }
        else
        {
            struct timespec ts;
#if USE_SYSTEM_TIME_MONOTONIC
            signed long long dwMinitime;
            dwMinitime = getsystemtime(SYSTEM_TIME_MONOTONIC);
            ts.tv_sec = dwMinitime/1000000000LL + (int)(dwTimeout/1000);
            ts.tv_nsec = dwMinitime%1000000000LL + (int)((dwTimeout%1000)*1000000);
#else
            struct timeval tv;
            nRet = gettimeofday (&tv, NULL);
            CSASSERT(nRet == 0);
            ts.tv_sec = tv.tv_sec + (int)(dwTimeout/1000);
            ts.tv_nsec = tv.tv_usec*1000 + (int)(dwTimeout%1000)*1000000;
#endif          
            adjusttimespec(&ts);

#if USE_SYSTEM_TIME_MONOTONIC
            nRet = sem_timedwait_monotonic((sem_t *)udi_sem->sem, &ts);
#else
            nRet = sem_timedwait((sem_t *)udi_sem->sem, &ts);
#endif
            if (nRet == 0)
            {
                enRet = CSUDI_SUCCESS;
            }
            else if(errno == ETIMEDOUT)
            {
                enRet = CSUDIOS_ERROR_TIMEOUT;
            }
            else
            {
                enRet = CSUDI_FAILURE;
                CSDEBUG("CS_OS",ERROR_LEVEL,"CSWaitForSemaphore.CS_OSP_FAILURE.errno=%d",errno);
            }
        }
    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }
//CSUDISYSReboot();
    if(enRet == CSUDI_SUCCESS)
    {
        udi_sem->nWaitTimeout = dwTimeout;
        udi_sem->pstrStatus = "Locked";
        udi_sem->thread = pthread_self();
        udi_sem->waited_count -= 1 ;
       
    }
    
    return enRet;
}

WT_Error_Code CSUDIOSSemRelease(WT_HANDLE hSemaphore)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;
    WTSemaphore_S *udi_sem = (WTSemaphore_S *)hSemaphore;

    CSASSERT(hSemaphore != CSUDI_NULL);

    if (hSemaphore != CSUDI_NULL)
    {
        if (0 == sem_post((sem_t *)udi_sem->sem))
        {
            enRet = CSUDI_SUCCESS;
            udi_sem->pstrStatus = "Unlocked";
            udi_sem->thread = pthread_self();
            
        }
    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }

    CSASSERT(enRet == CSUDI_SUCCESS);
    return enRet;
}

WT_Error_Code CSUDIOSSemDestroy(WT_HANDLE hSemaphore)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;
    int nSemVal=0;
    WTSemaphore_S *udi_sem = (WTSemaphore_S *)hSemaphore;
    CSASSERT(hSemaphore != NULL);
   // CSDEBUG("CS_OS",INFO_LEVEL,"%s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
    if (hSemaphore != NULL)
    {
        //android 系统实现的semphore，要求在destroy前必须保证可用的nSemVal大于0，以防止程序死锁
        sem_getvalue((sem_t *)udi_sem->sem,&nSemVal);

        while(nSemVal< 1)
        {
            sem_post((sem_t *)udi_sem->sem);
            sem_getvalue((sem_t *)udi_sem->sem,&nSemVal);
        }
      //  CSDEBUG("CS_OS",INFO_LEVEL,"%s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
        if (sem_destroy((sem_t *)udi_sem->sem) == 0)
        {
            free((sem_t *)udi_sem->sem); /* free memory.  */
            enRet = CSUDI_SUCCESS;
            if(udisem_protect)
            {
                sem_wait((sem_t *)udisem_protect);
            }
         //   CSDEBUG("CS_OS",INFO_LEVEL,"%s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
            udi_sem->bUsed = FALSE;
            udi_sem->sem = NULL;
            udi_sem->strName[0] = 0;
            udi_sem->pstrStatus = NULL;
            udi_sem->waited_count = 0 ;
            if(udisem_protect)
            {
                sem_post((sem_t *)udisem_protect);
            }
        }

    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }

    CSASSERT(enRet == CSUDI_SUCCESS);

    return enRet;
}

WT_Error_Code CSUDIOSMutexCreate(const char * pstrName, unsigned int dwFlags,WT_HANDLE * phMutex)
{
    cs_mutex_t*             mutex = NULL;
    DWORD               dwNameLen = 0;
    WT_Error_Code        enRet = CSUDI_FAILURE;

    CSSTD_UNUSED(dwFlags);

    if(pstrName != CSUDI_NULL)
    {
        dwNameLen = strlen(pstrName);
    }

    if(phMutex == CSUDI_NULL)
    {
        CSASSERT(phMutex != CSUDI_NULL);
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    if(dwNameLen >= 31)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    *phMutex = CSUDI_NULL;
    mutex =(cs_mutex_t*) CSUDIOSMalloc(sizeof(cs_mutex_t));

    CSASSERT(mutex);

    if( CSUDI_NULL != mutex )
    {
        WT_HANDLE semaphore;
        enRet = CSUDIOSSemCreate (pstrName,1,1,&semaphore) ;
        CSASSERT(semaphore);

        if( enRet == CSUDI_SUCCESS)
        {
            if (dwFlags == CSUDIOS_MUTEX_OBTAIN)
            {
                enRet = CSUDIOSSemWait(semaphore,CSUDIOS_TIMEOUT_IMMEDIATE);
                CSASSERT(enRet == CSUDI_SUCCESS);
                if (enRet == CSUDI_SUCCESS)
                {
                    mutex->count = 1;
                    mutex->owner = CSGetThreadId();
                }
            }
            else
            {
                mutex->count = 0;
                mutex->owner = (DWORD)CSUDI_NULL;
            }
            mutex->semaphore = semaphore;
            *phMutex = mutex;
        }

        if (enRet != CSUDI_SUCCESS)
        {
            if(mutex)
            {
                CSUDIOSFree(mutex);
                mutex = CSUDI_NULL;
            }
            CSDEBUG(MODULE_NAME,ERROR_LEVEL,"[USP_OSP] can't create semaphore for a mutex  !\n");
        }
    }
    else
    {
        CSDEBUG(MODULE_NAME,ERROR_LEVEL,"[USP_OSP] can't malloc memory for  a mutex  !\n");
    }

    return enRet;
}

WT_Error_Code CSUDIOSMutexDestroy(WT_HANDLE hMutex)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;

    if(hMutex)
    {
        cs_mutex_t* mutex = (cs_mutex_t*) hMutex;

        if(CSUDIOSSemDestroy( mutex->semaphore) == CSUDI_SUCCESS)
        {
            CSUDIOSFree(mutex);
            enRet = CSUDI_SUCCESS;
        }
        //CSDEBUG(MODULE_NAME,INFO_LEVEL,"[CS_OS]: DestroyMutex %xh\r\n", hMutex);
    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }

    return enRet;
}

WT_Error_Code CSUDIOSMutexWait(WT_HANDLE hMutex,unsigned int uTimeout)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;
    CSASSERT(hMutex);
    if(hMutex)
    {
        cs_mutex_t* mutex = (cs_mutex_t*) hMutex;

        DWORD  tid;

        tid = CSGetThreadId();

        if(mutex->owner == tid)
        {
            ++mutex->count;
            enRet = CSUDI_SUCCESS;
        }
        else
        {
            enRet = CSUDIOSSemWait(mutex->semaphore,uTimeout);
            if( enRet == CSUDI_SUCCESS)
            {
                mutex->owner = tid;
                mutex->count = 1;
            }
            else
            {
                CSDEBUG(MODULE_NAME,ERROR_LEVEL,"[CS_OS] can't  acquire semaphore for a mutex %xh, dwTimeout=%d !\r\n", mutex, uTimeout);
            }
        }
    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }

    return enRet;
}


WT_Error_Code CSUDIOSMutexRelease(WT_HANDLE hMutex)
{
    WT_Error_Code        enRet = CSUDI_FAILURE;

    if(hMutex)
    {
        cs_mutex_t* mutex = (cs_mutex_t*) hMutex;

        DWORD  tid;
        tid = CSGetThreadId();

        if (mutex->owner != tid)
        {
            CSDEBUG(MODULE_NAME,WARN_LEVEL,"[USP_OSP] can't release mutex %08x, count=%d, owner=%x, curtask=%x\n", hMutex, mutex->count, mutex->owner, tid);
            return CSUDI_FAILURE;
        }

        if(mutex->count > 1 )
        {
            enRet = CSUDI_SUCCESS;
            --mutex->count;
            //CSDEBUG(MODULE_NAME,INFO_LEVEL,"[USP_OSP]: Release Mutex %xh  but  Count > 0\r\n", hMutex);
        }
        else if (mutex->count == 1)
        {
            //save
            DWORD pOwner = mutex->owner ;
            int nCount = mutex->count;

            //clear
            mutex->owner = (DWORD)CSUDI_NULL;
            mutex->count = 0;

            //release
            if((enRet=CSUDIOSSemRelease(mutex->semaphore)) != CSUDI_SUCCESS)
            {
                //failed then restore
                mutex->owner = pOwner;
                mutex->count = nCount;
                CSDEBUG(MODULE_NAME,ERROR_LEVEL,"[USP_OSP] can't relase semaphore for a mutex %08X !\n", hMutex);
            }
        }
        else
        {
            CSDEBUG(MODULE_NAME,WARN_LEVEL,"[USP_OSP] can't release mutex %08XH, count=%d!\n", hMutex, mutex->count);
        }
    }
    else
    {
        enRet = CSUDIOS_ERROR_BAD_PARAMETER;
    }

    return enRet;

}

