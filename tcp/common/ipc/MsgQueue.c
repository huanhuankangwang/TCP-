#ifndef _MSG_H_
#define _MSG_H_

typedef    int        CSUDI_Error_Code;
typedef    void*      CSUDI_HANDLE;


#define    MSG_TYPE    23234

#define      CSUDI_SUCCESS   (CSUDI_Error_Code)0

typedef struct msg_st{
    long type;
    long time;
    char  data[600];
}msg_st;


#define      HID_MSGDATA_LENGTH   (512)

#define   BOOL              int
#define   CSUDIRDIEvent_E   int
#define   CSUDI_UINT32      int
#define   hid_msg_type_e    int

typedef struct _HID_MSG_Info_S{
    BOOL bWorkFlag;     //结构体成员是否使用
    BOOL bDevDescAck;   //是否收到设备描述符应答
    BOOL bDgramDescAck; //是否收到报告描述符应答
    BOOL bPlugEventAck; //是否收到插拔事件
    BOOL bNotifyAddFlag;    //HID数据回调是否注册
    CSUDIRDIEvent_E m_eEvtType;  //驱动设备事件
    CSUDI_UINT32 m_dwDeviceId;      //设备Id
    hid_msg_type_e m_eHidMsgType;   //HID设备事件
    int datalength;     //data数据长度
    char data[HID_MSGDATA_LENGTH - 12];
}HID_MSG_Info_S;

#define     CUSD_DEBUG      printf


long getCurrentTime()
{
    long  S64Usecnow;
    struct timeval tv={0,0};
    if( gettimeofday(&tv,NULL))
        printf("gettimeofday SetStartTime err\r\n");
    S64Usecnow  =   tv.tv_sec;
    S64Usecnow  *=  1000000;
    S64Usecnow  +=  tv.tv_usec;
    return S64Usecnow;
}

inline int  queueRecv(int msgid,char *pvMsg,int nMaxMsgBytes)
{
    int nRead,len;
    static msg_st  msg;
    msg.type  = MSG_TYPE;

    len   = nMaxMsgBytes + 2*sizeof(long);
    
    while(nMaxMsgBytes > 0)
    {
        nRead = msgrcv(msgid,&msg,len,msg.type,0);
        memcpy(pvMsg,msg.data,nMaxMsgBytes);
        break;
    }

    printf("time =%ld \r\n",msg.time);

    return nRead == len ? nMaxMsgBytes:0;
}


inline int  queueSend(int msgid,char *pvMsg,int nMaxMsgBytes)
{
    int nRead;
    static msg_st  msg;
    msg.type  = MSG_TYPE;

    msg.time = getCurrentTime();
    
    while(nMaxMsgBytes > 0)
    {
        memcpy(msg.data,pvMsg,nMaxMsgBytes);
        nRead = msgsnd(msgid,(&msg),nMaxMsgBytes+ 2* sizeof(long),0);
        break;
    }

    CUSD_DEBUG("msgid =%d nMaxMsgBytes =%d nRead =%d time =%ld\r\n",msgid,nMaxMsgBytes,nRead,msg.time);

    return nRead == 0 ? nMaxMsgBytes :0;
}


CSUDI_Error_Code MsgQueueCreate(const char * pcName,int nMaxMsgs,int nSizePerMsg,CSUDI_HANDLE * phMsgQueue)
{
    CSUDI_Error_Code ret = -11;
    int msgid = -1;
    CSUDI_HANDLE  pch = NULL;

    do
    {
        if(!phMsgQueue)
        {
            break;
        }
        msgid = msgget((key_t)13456, 0666 | IPC_CREAT);
        if(msgid <0)
        {
            break;
        }
        CUSD_DEBUG("msgid = %d\r\n",msgid);

        pch = malloc(sizeof(int));
        if(!pch)
        {
            break;
        }

        *(int*)pch = msgid;
        *phMsgQueue = pch;

        ret = CSUDI_SUCCESS;
    }while(0);

    return ret;
}   

CSUDI_Error_Code MsgQueueDestroy(CSUDI_HANDLE hMsgQueue)
{
    CSUDI_Error_Code ret = -11;
    do{
        if(!hMsgQueue)
        {
            break;
        }

        
        
        ret = CSUDI_SUCCESS;
    }while(0);

    return ret;
}

CSUDI_Error_Code MsgQueueReceive(CSUDI_HANDLE hMsgQueue,void * pvMsg,int nMaxMsgBytes,unsigned int uTimeout)
{
    int msgid;
    int len;
    CSUDI_Error_Code ret = -11;

    do
    {
        if(!hMsgQueue)
        {
            break;
        }

        msgid  = *((int*)hMsgQueue);

        len =  queueRecv(msgid,pvMsg,nMaxMsgBytes);

        CUSD_DEBUG("read nRead =%d\r\n",len);
        if(len != nMaxMsgBytes)
        {
            break;
        }
        ret = CSUDI_SUCCESS;
    }while(0);

    return ret;
}

CSUDI_Error_Code MsgQueueSend(CSUDI_HANDLE hMsgQueue, const void * pvMsg, int nMsgBytes, unsigned int uTimeout)
{
    int msgid;
    int len;
    CSUDI_Error_Code ret = -11;

    do
    {
        if(!hMsgQueue)
        {
            break;
        }

        msgid  = *((int*)hMsgQueue);

        len =  queueSend(msgid,pvMsg,nMsgBytes);
        if(len != nMsgBytes)
        {
            break;
        }
        ret = CSUDI_SUCCESS;
    }while(0);

    return ret;
}

void  print_info(HID_MSG_Info_S *info)
{
    CUSD_DEBUG("bWorkFlag      =%d \r\n" ,info->bWorkFlag     );
    CUSD_DEBUG("bDevDescAck    =%d \r\n" ,info->bDevDescAck   );
    CUSD_DEBUG("bDgramDescAck  =%d \r\n" ,info->bDgramDescAck );
    CUSD_DEBUG("bPlugEventAck  =%d \r\n" ,info->bPlugEventAck );
    CUSD_DEBUG("bNotifyAddFlag =%d \r\n" ,info->bNotifyAddFlag);
    CUSD_DEBUG("m_eEvtType     =%d \r\n" ,info->m_eEvtType    );
    CUSD_DEBUG("m_dwDeviceId   =%d \r\n" ,info->m_dwDeviceId  );
    CUSD_DEBUG("m_eHidMsgType  =%d \r\n" ,info->m_eHidMsgType );
    CUSD_DEBUG("data           =%s \r\n" ,info->data          );
    CUSD_DEBUG("datalength     =%d \r\n" ,info->datalength    );
}

#endif//_MSG_H_