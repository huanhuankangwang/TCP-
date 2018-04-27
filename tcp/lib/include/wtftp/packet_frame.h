#ifndef _PACKET_FRAME_H_
#define _PACKET_FRAME_H_

	/*
	*  ����ֵ
	*/
	enum {
	    BusErr_NoError                  =   0,
	    BusErr_Unknown                  =  -1,
	    BusErr_NoMemory                 =  -2,
	    BusErr_BadParam                 =  -3,
	    BusErr_Invalid                  =  -4,
	    BusErr_OpenCtrlConnFail         =  -5,
	    BusErr_OpenMonitorConnFail      =  -6,
	    BusErr_AttachFail               =  -7,
	    BusErr_SocketPairFail           =  -8,
	    BusErr_DetachFail               =  -9,
	    BusErr_InterestFail             =  -10,
	    BusErr_CtrlConnNoExist          =  -11,
	    BusErr_MonitorConnNoExist       =  -12,
	    BusErr_ReceiveFail              =  -13,
	    BusErr_ReceiveEOF               =  -14,
	    BusErr_SendFail                 =  -15,

	};

#define BUS_ADDR_MAX_LEN (24)
#define BUS_MSGTYPE_MAX_LEN (16)
#define BUS_MSGDATA_MAX_LEN (1024)

typedef struct {
    char ip[BUS_ADDR_MAX_LEN]; /*ip��ַ����ʽ: 192.168.1.2*/
    unsigned short port; /*�˿ں�*/
} BusAddr;

typedef struct {
    BusAddr remoteAddr; /*Զ�ˣ����ֻ���PAD��Ӧ�õĵ�ַ*/
    char mode; /*ģʽ���������㲥��֪ͨ*/
    char msgType[BUS_MSGTYPE_MAX_LEN + 1]; /*��Ϣ����*/
    char msgData[BUS_MSGDATA_MAX_LEN]; /*��Ϣ����*/
	int  mCseq;//������Ϣ�����к�
    int msgDataSize; /*��Ϣ���ݴ�С*/
} BusMsg;



int send_busMsg(int sockfd,BusMsg *msg);
int receive_busMsg(int nSocketFd, BusMsg *data);


int ctrl_compose_frame(BusMsg *data,const char *frame);
int ctrl_decompose_frame(char * msg,int msgLen,BusMsg * pOutdata);


#endif//_PACKET_FRAME_H_
