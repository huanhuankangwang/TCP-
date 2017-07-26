#ifndef _WTFTP_SERVER_H_
#define WTFTP_SERVER_H_



int registerMsgCallback(char *msgType,pfCallback  cb,pfRecvCallBack recvCb);
int open_wtftp_server(char *bindpord);



#endif//WTFTP_SERVER_H_
