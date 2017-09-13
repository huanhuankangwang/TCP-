/*******************************************************************
 **                    Message Queue definitions                  **
 *******************************************************************/
typedef struct
{
    DWORD  Name;
    DWORD  ByteQueueSize;                   /* Size of the queue in bytes   */
    DWORD  ByteNodeSize;                    /* Message size in bytes        */
    DWORD  MsgCount;
    DWORD  Head;                             /* Head offset from queue start */
    DWORD  Tail;                            /* Tail offset from queue start */
    CSUDI_HANDLE QMutex;                 /* Queue mutex                  */
    CSUDI_HANDLE QEvent;                /* Queue event                  */
    CSUDI_HANDLE QEventAvailSpace;/* Queue event                  */
    BYTE  *StartPtr;                         /* Pointer to queue start       */
    BOOL    m_bIsRun;
} MSG_QUEUE;

CSUDI_Error_Code CSUDIOSMsgQueueCreate (const char* pstrName,int nMaxMsgs,int nMsgLen, CSUDI_HANDLE * phMsgQueue)
{
    CSUDI_Error_Code        enRet = CSUDI_FAILURE;
    CSUDI_HANDLE        Qmutex = (CSUDI_HANDLE)CSUDI_NULL;
    CSUDI_HANDLE        Qevent = (CSUDI_HANDLE)CSUDI_NULL;
    CSUDI_HANDLE        QEventAvailSpace = (CSUDI_HANDLE)CSUDI_NULL;
    MSG_QUEUE*          Qptr = CSUDI_NULL;
    DWORD               nNameLen = 0;

    CSASSERT(nMsgLen > 0);

    if(pstrName != CSUDI_NULL)
    {
        nNameLen = strlen(pstrName);
    }

    CSASSERT(nNameLen < 32);

    if((nMsgLen <= 0) || (nNameLen >= 32) || (nMaxMsgs <= 0) || (phMsgQueue == CSUDI_NULL))
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    *phMsgQueue = CSUDI_NULL;
    /*================================================
     * Create memory, mutex and event for the queue
     *===============================================*/
    Qptr = (MSG_QUEUE *) malloc ( ( sizeof ( MSG_QUEUE ) + (DWORD)( nMaxMsgs * ( (( nMsgLen + 3 ) / 4) * 4 ) ) ) * 2 ) ;

    if ( Qptr != CSUDI_NULL)
    {
        enRet = CSUDIOSMutexCreate("MsgQueueMutex", 0,&Qmutex);

        if ( enRet == CSUDI_SUCCESS)
        {
            enRet = CSUDIOSEventCreate(0,0 ,&Qevent);

            if ( enRet == CSUDI_SUCCESS)
            {
                enRet = CSUDIOSEventCreate( 0, CSUDIOS_EVENT_INITIAL_STATUS,&QEventAvailSpace);

                if ( enRet == CSUDI_SUCCESS)
                {
                    /*==========================================
                    * Initialize the queue
                    *=========================================*/
                    enRet = CSUDIOSMutexWait(Qmutex, CSUDIOS_TIMEOUT_INFINITY);

                    if(enRet == CSUDI_SUCCESS)
                    {
                        Qptr->Name              = 0;//pstrName;
                        Qptr->ByteQueueSize     = (DWORD)(nMaxMsgs * nMsgLen * 2);
                        Qptr->ByteNodeSize      = (DWORD)(nMsgLen * 2);
                        Qptr->MsgCount          = 0;
                        Qptr->Head              = 0;
                        Qptr->Tail              = 0;
                        Qptr->QMutex            = Qmutex;
                        Qptr->QEvent            = Qevent;
                        Qptr->QEventAvailSpace  = QEventAvailSpace;
                        Qptr->StartPtr          = (BYTE *) ( Qptr) + sizeof ( MSG_QUEUE ) * 2;
                        Qptr->m_bIsRun          = TRUE;

                        enRet = CSUDIOSMutexRelease(Qmutex);
                        if(enRet == CSUDI_SUCCESS)
                        {
                            *phMsgQueue = (CSUDI_HANDLE)Qptr;
                            enRet = CSUDI_SUCCESS;
                        }
                    }
                }
            }
        }

        if ( *phMsgQueue == CSUDI_NULL )
        {
            CSASSERT(*phMsgQueue != CSUDI_NULL);
            if ( Qevent != CSUDI_NULL )
            {
                enRet = CSUDIOSEventDestroy( Qevent );
                if(enRet != CSUDI_SUCCESS)
                {
                    CSDEBUG(MODULE_NAME,INFO_LEVEL,"CSDestroyEventdwRet=%d\r\n",enRet);
                }
            }

            if ( Qmutex != (CSUDI_HANDLE)CSUDI_NULL )
            {
                enRet = CSUDIOSMutexDestroy(Qmutex);
                if(enRet != CSUDI_SUCCESS)
                {
                    CSDEBUG(MODULE_NAME,INFO_LEVEL,"CSDestroyMutex=%d\r\n",enRet);
                }
            }

            if(CSUDI_NULL != Qptr)
            {
                free (Qptr);
            }
        }
    }

    return enRet ;
}

CSUDI_Error_Code CSUDIOSMsgQueueDestroy(CSUDI_HANDLE hMsgQueue)
{
    CSUDI_Error_Code        enRet = CSUDI_FAILURE;
    CSUDI_HANDLE            Qmutex;
    MSG_QUEUE               *Qptr;

    if (hMsgQueue == CSUDI_NULL)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    Qptr = (MSG_QUEUE *) hMsgQueue;
    Qmutex = Qptr->QMutex;

    if ( Qmutex != 0 )
    {
        /*=============================================
        * Delete queue event, mutex and memory
        *============================================*/
        enRet = CSUDIOSMutexWait( Qmutex, CSUDIOS_TIMEOUT_INFINITY );
        if(enRet == CSUDI_SUCCESS)
        {
            if ( Qptr->m_bIsRun == TRUE )
            {
                Qptr->m_bIsRun = FALSE;

                enRet = CSUDIOSEventDestroy(Qptr->QEvent);
                enRet = CSUDIOSEventDestroy(Qptr->QEventAvailSpace);
                enRet = CSUDIOSMutexRelease (Qmutex);
                enRet = CSUDIOSMutexDestroy (Qmutex);

                Qptr->QMutex = 0;
                Qptr->QEvent = 0;
                Qptr->QEventAvailSpace = 0;

                free (Qptr);

                enRet = CSUDI_SUCCESS;
            }
        }
    }

    CSASSERT(enRet == CSUDI_SUCCESS);

    return enRet;
}

CSUDI_Error_Code CSUDIOSMsgQueueSend(CSUDI_HANDLE hMsgQueue, const void * pvMsg, int nMsgBytes, unsigned int dwTimeout)
{
    CSUDI_Error_Code        enRet = CSUDI_FAILURE;
    CSUDI_HANDLE            Qmutex;
    MSG_QUEUE               *Qptr;
    DWORD               Tail;//Head

    CSASSERT(hMsgQueue != NULL && pvMsg != NULL && nMsgBytes > 0);
    if (hMsgQueue == CSUDI_NULL || pvMsg == CSUDI_NULL || nMsgBytes <= 0 )
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    Qptr = (MSG_QUEUE *) hMsgQueue;
    Qmutex = Qptr->QMutex;

    enRet = CSUDIOSEventWait( Qptr->QEventAvailSpace, dwTimeout);

    if ( enRet == CSUDI_SUCCESS)
    {
        enRet  = CSUDIOSMutexWait( Qmutex, CSUDIOS_TIMEOUT_INFINITY );

        if ( enRet == CSUDI_SUCCESS)
        {
            Tail = Qptr->Tail;

            if ( Qptr->ByteQueueSize >= ( ( Qptr->MsgCount + 1 ) * Qptr->ByteNodeSize ) )
            {
                DWORD dwCopySize = ( (DWORD)nMsgBytes > ( Qptr->ByteNodeSize / 2 ) ? ( Qptr->ByteNodeSize / 2 ) : (DWORD)nMsgBytes );

                memcpy( Qptr->StartPtr + Tail, pvMsg, dwCopySize );

                Tail += Qptr->ByteNodeSize;

                if ( Tail >= Qptr->ByteQueueSize )
                {
                    Tail = 0;
                }

                Qptr->Tail = Tail;
                Qptr->MsgCount++;

                if ( Qptr->MsgCount >= ( Qptr->ByteQueueSize/Qptr->ByteNodeSize ) )
                {
                    CSUDIOSEventReset(Qptr->QEventAvailSpace);
                }

                CSUDIOSEventSet( Qptr->QEvent );            /* Set the queue event  */

                enRet = CSUDI_SUCCESS;
            }

            CSUDIOSMutexRelease(Qmutex);
        }
        else
        {
            CSASSERT( enRet == CSUDI_SUCCESS);
        }
    }
    else if ( enRet == CSUDIOS_ERROR_TIMEOUT )
    {
        enRet = CSUDIOS_ERROR_TIMEOUT;
    }

    CSASSERT(enRet == CSUDI_SUCCESS);

    return enRet;
}

CSUDI_Error_Code CSUDIOSMsgQueueReceive(CSUDI_HANDLE hMsgQueue,void * pvMsg,int nMaxMsgBytes,unsigned int dwTimeout)
{
    CSUDI_Error_Code        enRet = CSUDI_FAILURE;
    CSUDI_HANDLE Qmutex;
    MSG_QUEUE   *Qptr;
    DWORD      Head = 0;//, Tail;

    CSASSERT(hMsgQueue != NULL && pvMsg != NULL && nMaxMsgBytes > 0);

    if (hMsgQueue == NULL || pvMsg == NULL || nMaxMsgBytes <= 0)
    {
        return CSUDIOS_ERROR_BAD_PARAMETER;
    }

    Qptr = (MSG_QUEUE *) hMsgQueue;
    Qmutex = Qptr->QMutex;

    enRet = CSUDIOSEventWait(Qptr->QEvent, dwTimeout);

    if ( enRet == CSUDI_SUCCESS)
    {
        enRet = CSUDIOSMutexWait( Qmutex, CSUDIOS_TIMEOUT_INFINITY );

        if ( enRet == CSUDI_SUCCESS )
        {
            Head = Qptr->Head;
            //Tail = Qptr->Tail;

            if ( Qptr->MsgCount > 0 )
            {
                DWORD dwCopySize = (DWORD)nMaxMsgBytes > ( Qptr->ByteNodeSize / 2 ) ? ( Qptr->ByteNodeSize / 2 ) : (DWORD)nMaxMsgBytes;

                memcpy ( pvMsg, Qptr->StartPtr + Head, dwCopySize );
                memset( Qptr->StartPtr + Head, 0, dwCopySize );

                Head += Qptr->ByteNodeSize;

                if ( Head >= Qptr->ByteQueueSize )
                {
                    Head = 0;
                }

                Qptr->Head = Head;
                Qptr->MsgCount--;

                if ( Qptr->MsgCount == 0 )
                {
                    CSUDIOSEventReset(Qptr->QEvent);
                }

                CSUDIOSEventSet( Qptr->QEventAvailSpace );

                enRet = CSUDI_SUCCESS;
            }

            CSUDIOSMutexRelease( Qmutex );
        }
        else
        {
            CSASSERT( enRet == CSUDI_SUCCESS );
        }
    }
    else if (enRet == CSUDIOS_ERROR_TIMEOUT)
    {
        enRet = CSUDIOS_ERROR_TIMEOUT;
    }

    CSASSERT( enRet == CSUDI_SUCCESS || enRet == CSUDIOS_ERROR_TIMEOUT);

    return enRet;
}


