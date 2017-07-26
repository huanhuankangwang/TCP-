

/*
    所有的交互 都由这个WBus来控制
    支持多个用户 同时控制操作
*/

#define          SERVER_IP       "172.30.17.30"
#define          SERVER_PORT     13982


typedef  int(*msg_cb)(char *,int);


typedef struct {
    char  mMsgType[16];
    msg_cb cb;
} WBusInterest;


typedef int(*pfCallback)(char *type,char *data,int dataSize);
typedef int(*pfRecvCallBack)(int sockfd,char *remoteIp,int port);

typedef struct _WBus {
    char  mMsgType[24];
    pfCallback   mPfun;
    pfRecvCallBack mRecvPfun;

    struct _WBus *mNext;
} T_WBus,*PT_WBus;

WBus  * wBusHead = NULL;




