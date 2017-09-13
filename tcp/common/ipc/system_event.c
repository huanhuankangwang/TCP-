/*******************************************************************
 **                     Event definitions                         **
 *******************************************************************/

typedef struct
{
    pthread_cond_t  condvar;
    pthread_mutex_t mutex;
    int             signaled;
    int             manual_reset;
} T_WTEvent;

WT_Error_Code WTEventCreate(const char * pstrName,unsigned int  dwFlags,WT_HANDLE * phEvent)
{
    WT_Error_Code       enRet = WT_FAILURE;
    T_WTEvent           *pEvent;
    DWORD               dwNameLen = 0;
    int                 nRet;

    if(pstrName != WT_NULL)
    {
        dwNameLen = strlen(pstrName);
    }

    if (dwNameLen > 32 || phEvent == WT_NULL)
    {
        WTASSERT(dwNameLen <=32);
        WTASSERT(phEvent != WT_NULL);
        return WT_ERROR_BAD_PARAMETER;
    }

    *phEvent = WT_NULL;
    pEvent = (T_WTEvent *)malloc(sizeof(T_WTEvent));
    WTASSERT(pEvent != NULL);
    if (pEvent != NULL)
    {
        if((dwFlags&CSUDIOS_EVENT_AUTO_RESET) ==  CSUDIOS_EVENT_AUTO_RESET)
        {
            pEvent->manual_reset = FALSE;
        }
        else
        {
            pEvent->manual_reset = TRUE;
        }

        if((dwFlags&CSUDIOS_EVENT_INITIAL_STATUS) == CSUDIOS_EVENT_INITIAL_STATUS)
        {
            pEvent->signaled = TRUE;
        }
        else
        {
            pEvent->signaled = FALSE;
        }

        nRet = pthread_mutex_init( &pEvent->mutex, NULL );
        WTASSERT(nRet == 0);
        if(nRet == 0)
        {
            nRet = pthread_cond_init( &pEvent->condvar, NULL );
            WTASSERT(nRet == 0);
            if (nRet == 0)
            {
                enRet = WT_SUCCESS;
                *phEvent = (CSUDI_HANDLE)pEvent;
            }
        }

        if (enRet != WT_SUCCESS)
        {
            free(pEvent);
        }
    }

    WTASSERT(enRet == WT_SUCCESS);

    return enRet;
}

WT_Error_Code CSUDIOSEventReset(WT_HANDLE hEvent)
{
    WT_Error_Code        enRet = WT_FAILURE;
    T_WTEvent             *event;
    int                     nRet;

    if (hEvent == WT_NULL)
    {
        WTASSERT(hEvent != WT_NULL);
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    event = (T_WTEvent *)hEvent;

    nRet = pthread_mutex_lock(&event->mutex);

    event->signaled = FALSE;

    nRet += pthread_mutex_unlock(&event->mutex);

    WTASSERT(nRet == 0);

    if (nRet == 0)
    {
        enRet = WT_SUCCESS;
    }

    WTASSERT(enRet == WT_SUCCESS);

    return enRet;
}

WT_Error_Code CSUDIOSEventSet(WT_HANDLE hEvent)
{
    CSUDI_Error_Code      enRet = WT_FAILURE;
    T_WTEvent             *event;
    int                   nRet;

    if (hEvent == WT_NULL)
    {
        WTASSERT(hEvent != WT_NULL);
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    event = (T_WTEvent *)hEvent;

    nRet = pthread_mutex_lock(&event->mutex);

    event->signaled = TRUE;
    /* Wake up one or all depending on whether the event is auto-reseting. */
    if( event->manual_reset )
        nRet += pthread_cond_broadcast(&event->condvar);
    else
        nRet += pthread_cond_signal(&event->condvar);

    nRet += pthread_mutex_unlock(&event->mutex);

    WTASSERT(nRet == 0);

    if (nRet == 0)
    {
        enRet = WT_SUCCESS;
    }


    WTASSERT(enRet == WT_SUCCESS);

    return enRet;
}

WT_Error_Code CSUDIOSEventDestroy(WT_HANDLE hEvent)
{
    WT_Error_Code        enRet = WT_FAILURE;
    T_WTEvent             *event;
    int                     nRet;

    if (hEvent == WT_NULL)
    {
        WTASSERT(hEvent != WT_NULL);
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    event = (T_WTEvent *)hEvent;

    nRet = pthread_cond_broadcast( &event->condvar );
    WTASSERT(nRet == 0);

    nRet = pthread_cond_destroy( &event->condvar );
    WTASSERT(nRet == 0);

    nRet = pthread_mutex_destroy(&event->mutex);
    WTASSERT(nRet == 0);

    free(event);
    enRet = WT_SUCCESS;

    return enRet;
}

WT_Error_Code CSUDIOSEventWait(WT_HANDLE hEvent,unsigned int dwTimeout)
{
    WT_Error_Code        enRet = WT_FAILURE;
    T_WTEvent             *event;
    int                     nRet;

    if (hEvent == WT_NULL)
    {
        WTASSERT(hEvent != WT_NULL);
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    event = (T_WTEvent *)hEvent;

    nRet = pthread_mutex_lock(&event->mutex);
    WTASSERT(nRet == 0);

    /* Return immediately if the event is signalled. */
    if(event->signaled)
    {
        if(!event->manual_reset)
        {
            event->signaled = FALSE;
        }

        nRet += pthread_mutex_unlock(&event->mutex);
        WTASSERT(nRet == 0);
        return( WT_SUCCESS);
    }

    /* If just testing the state, return OSAL_TIMEOUT. */
    if( dwTimeout == 0 )
    {
        /*Jameswen midfy form pthread_mutex 2011-12-09 */
       // nRet += pthread_mutex_unlock(&event->mutex);
       // WTASSERT(nRet == 0);
        enRet = CSUDIOS_ERROR_TIMEOUT;
    }
    else if (dwTimeout == 0xFFFFFFFF)
    {
/*Jameswen modify for wait time 0xFFFFFFF to muti 60 sec 2011-12-10*/           
            
    nRet = pthread_cond_wait(&event->condvar, &event->mutex);
#if 0
      while(1)
      {
      
       struct timespec ts;
       unsigned int timeout = 10000;
#if USE_SYSTEM_TIME_MONOTONIC
         signed long long dwMinitime;
           dwMinitime = getsystemtime(SYSTEM_TIME_MONOTONIC);
           ts.tv_sec = dwMinitime/1000000000LL + (int)(timeout/1000);
           ts.tv_nsec = dwMinitime%1000000000LL + (int)((timeout%1000)*1000000);
#else           
        struct timeval tv;
        nRet = gettimeofday (&tv, NULL);
        WTASSERT(nRet == 0);
        ts.tv_sec = tv.tv_sec + (int)(timeout/1000);
        ts.tv_nsec = tv.tv_usec*1000 + (int)(timeout%1000)*1000000;
#endif  
        adjusttimespec(&ts);

#if USE_SYSTEM_TIME_MONOTONIC
            nRet = pthread_cond_timedwait_monotonic(&event->condvar, &event->mutex, &ts);
#else
        nRet = pthread_cond_timedwait(&event->condvar, &event->mutex, &ts);
#endif
        if (nRet == 0)
        {
            enRet = WT_SUCCESS;
            break;
        }
        dump_stack_trace();
      }
#endif        
 /*Jameswen modify end 2011-12-10*/       
        if (nRet == 0)
        {
            enRet = WT_SUCCESS;
        }
        else
        {
            CSDEBUG(MODULE_NAME,ERROR_LEVEL, "[CSWaitForSingleEvent]ERROR: pthread_cond_wait(%d) return %d\n", dwTimeout, nRet);
            enRet = WT_FAILURE;
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
        WTASSERT(nRet == 0);
        ts.tv_sec = tv.tv_sec + (int)(dwTimeout/1000);
        ts.tv_nsec = tv.tv_usec*1000 + (int)(dwTimeout%1000)*1000000;
#endif  
        adjusttimespec(&ts);

#if USE_SYSTEM_TIME_MONOTONIC
        nRet = pthread_cond_timedwait_monotonic(&event->condvar, &event->mutex, &ts);
#else
        nRet = pthread_cond_timedwait(&event->condvar, &event->mutex, &ts);
#endif
        //CSDEBUG(MODULE_NAME,3, "nRet=%d, EINVAL:%d, ETIMEDOUT:%d\n", nRet, EINVAL, ETIMEDOUT);
        if (nRet == 0)
        {
            enRet = WT_SUCCESS;
        }
        else if (nRet == ETIMEDOUT)
        {
            enRet = CSUDIOS_ERROR_TIMEOUT;
        }
        else
        {
            CSDEBUG(MODULE_NAME,ERROR_LEVEL, "[CSWaitForSingleEvent]ERROR: pthread_cond_timedwait(%d) return %d\n", dwTimeout, nRet);
            enRet = WT_FAILURE;
        }

    }

    if(WT_SUCCESS == enRet)
    {
        if(!event->manual_reset)
        {
            event->signaled = FALSE;
        }
    }
    nRet = pthread_mutex_unlock(&event->mutex);
    WTASSERT(nRet == 0);

    WTASSERT(enRet == WT_SUCCESS || enRet == CSUDIOS_ERROR_TIMEOUT);
    return enRet;
}


