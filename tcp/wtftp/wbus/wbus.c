

/*
    ���еĽ��� �������WBus������
    ֧�ֶ���û� ͬʱ���Ʋ���
*/

typedef  int(*msg_cb)(char *,int);


typedef struct{
    char  mMsgType[16];
    msg_cb cb;
}WBusInterest;

typedef struct _WBus{
    WBusInterest  mInterest;
    struct _WBus  *mNext;
}WBus;

WBus   * wbusHead = NULL;

int open_wbus_server()
{
    
    return 0;
}


int open_wbus_client()
{
    
}

int register_interest(char *type,msg_cb cb)
{
}