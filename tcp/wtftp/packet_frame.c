#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>


#include <packet_frame.h>

#include "common.h"
#include "easy_common.h"

#define BUS_FRAME_MIN_LEN  (56)
#define BUS_MAIN_VER  (1)
#define BUS_SUB_VER  (1)

static char s_aucMsgHead[4] = {0x45, 0x43, 0x45, 0x42};


int ctrl_compose_frame(BusMsg *data,const char *frame)
{
    char *tmp;
    int contentLen;
    char crcFlag = 1;
    unsigned int crcGen;
    char encryptFlag = 1;
	int  cseq;

    EB_LOGD(EB_LOG_NORMAL, "start");

    if (data == NULL) return -1;

	tmp = frame;
    if (frame == NULL) {
        return -2;
    }

    //帧起始符
    memcpy((void *)tmp, (void *)s_aucMsgHead, 4);
    tmp += 4;

    //主板本号
    tmp[0] = BUS_MAIN_VER;
    tmp++;

    //次板本号
    tmp[0] = BUS_SUB_VER;
    tmp++;

    //校验标识
    if (crcFlag == 1) 
	{
        tmp[0] = 1;
    }
    tmp++;

    //加密标识
    if (encryptFlag == 1) {
        tmp[0] = 1;
    }
    tmp++;

    //保留字段
    //todo...
    tmp += 16;

    //消息序号
    //todo...
    
    cseq = htonl(data->mCseq);
    memcpy((void *)tmp, (void *)&cseq, 4);
    tmp += 4;

    //消息类型
    memcpy((void *)tmp, (void *)data->msgType, BUS_MSGTYPE_MAX_LEN);
    tmp += BUS_MSGTYPE_MAX_LEN;

    //消息数据长度
    EB_LOGD(EB_LOG_NORMAL, "data->msgDataSize:%d", data->msgDataSize);
    contentLen = htonl(data->msgDataSize);
    memcpy((void *)tmp, (void *)&contentLen, 4);
    tmp += 4;

    //消息数据
    memcpy((void *)tmp, (void *)data->msgData, data->msgDataSize);
    tmp += data->msgDataSize;

    //校验码
    if (crcFlag == 1) {
        unsigned int crcTmp;

        crcGen = easy_crc32(frame, tmp - frame);
        EB_LOGD(EB_LOG_NORMAL, "crcGen:0x%x", crcGen);
        crcTmp = htonl(crcGen);
        memcpy((void *)tmp, (void *)&crcTmp, 4);
    }
    tmp += 4;

    //加密码
    if (encryptFlag == 1) 
	{
        unsigned int encryptGen;
        char *p, *q;

        p = (char *)&crcGen;
        q = (char *)&encryptGen;
        q[1] = p[3] >> 2;
        q[0] = p[2] << 3;
        q[3] = p[1] >> 4;
        q[2] = p[0] << 5;
        EB_LOGD(EB_LOG_NORMAL, "encryptGen:0x%x", encryptGen);
        encryptGen = htonl(encryptGen);
        memcpy((void *)tmp, (void *)&encryptGen, 4);
    }
    tmp += 4;

    EB_PRINT_MEM("the composed frame:", frame, tmp - frame);

    return tmp -frame;
}

int ctrl_decompose_frame(char *msg, int msgLen,BusMsg *pOutdata)
{
    char *pcMsgTemp = msg;
    char *pData = NULL;
    BusMsg *data;
    int dataLen = 0;
    int msgNum = 0;
    char crcFlag;
    unsigned int crcRecv;
    char encryptFlag;

	data  = pOutdata;

    EB_LOGD(EB_LOG_NORMAL, "msgLen: %d", msgLen);

    if(msg == NULL || msgLen <= BUS_FRAME_MIN_LEN) {
        EB_LOGE("parameter error !");
        return -1;
    }

    memset((void *)data, 0, sizeof(BusMsg));

    EB_PRINT_MEM("the receive frame:", msg, msgLen);

    //帧起始符
    if (memcmp((void *)pcMsgTemp, s_aucMsgHead, 4) != 0) {
        EB_LOGE("the msg is not formatted correctly !");
        return -2;
    }
    pcMsgTemp += 4;

    //主板本号
    if (pcMsgTemp[0] != BUS_MAIN_VER) {
        EB_LOGE("main version error !");
        return -3;
    }
    pcMsgTemp++;

    //次板本号
    if (pcMsgTemp[0] != BUS_SUB_VER) {
        EB_LOGE("sub version error !");
        return -4;
    }
    pcMsgTemp++;

    //校验标识
    crcFlag = pcMsgTemp[0];
    EB_LOGD(EB_LOG_NORMAL, "crcFlag=%d", crcFlag);
    if (crcFlag != 1) {
        return -5;
    }
    pcMsgTemp++;

    //加密标识
    encryptFlag = pcMsgTemp[0];
    EB_LOGD(EB_LOG_NORMAL, "encryptFlag=%d", encryptFlag);
    if (encryptFlag != 1) {
        return -6;
    }
    pcMsgTemp++;

    //保留字段
    //todo...
    pcMsgTemp += 16;

    //消息序号
    //todo...
    memcpy((void *)&msgNum, (void *)pcMsgTemp, 4);
    msgNum = ntohl(msgNum);
	data->mCseq = msgNum;
    EB_LOGD(EB_LOG_NORMAL, "msgNum=%d", msgNum);
    pcMsgTemp += 4;

    //消息类型
    memcpy((void *)data->msgType, (void *)pcMsgTemp, BUS_MSGTYPE_MAX_LEN);
    EB_LOGD(EB_LOG_NORMAL, "msgType=%s", data->msgType);
    pcMsgTemp += BUS_MSGTYPE_MAX_LEN;

    //消息数据长度
    memcpy((void *)&dataLen, (void *)pcMsgTemp, 4);
    data->msgDataSize = ntohl(dataLen);
    EB_LOGD(EB_LOG_NORMAL, "data.msgDataSize=%d", data->msgDataSize);
    pcMsgTemp += 4;

    //消息数据
    pData = pcMsgTemp;
    pcMsgTemp += data->msgDataSize;

    //校验码
    if (crcFlag == 1) {
        unsigned int crcGen;

        memcpy((void *)&crcRecv, (void *)pcMsgTemp, 4);
        crcRecv = ntohl(crcRecv);
        crcGen = easy_crc32(msg, pcMsgTemp - msg);
        EB_LOGD(EB_LOG_NORMAL, "crcRecv:0x%x, crcGen:0x%x", crcRecv, crcGen);
        if (crcRecv != crcGen) {
            EB_LOGE("crc error !\r\n");
            return -7;
        }
    }
    pcMsgTemp += 4;

    //加密码
    if (encryptFlag == 1)
	{
        unsigned int encryptRecv, encryptGen;
        unsigned int crcTmp;
        char *p, *q;

        memcpy((void *)&encryptRecv, (void *)pcMsgTemp, 4);
        encryptRecv = ntohl(encryptRecv);
        p = (char *)&crcRecv;
        q = (char *)&encryptGen;
        q[1] = p[3] >> 2;
        q[0] = p[2] << 3;
        q[3] = p[1] >> 4;
        q[2] = p[0] << 5;
        EB_LOGD(EB_LOG_NORMAL, "encryptRecv:0x%x, encryptGen:0x%x", encryptRecv, encryptGen);
        if (encryptRecv != encryptGen) {
            EB_LOGE("encrypt error !");
            return -8;
        }
    }
    pcMsgTemp += 4;

    if (pcMsgTemp - msg != msgLen) 
	{
        EB_LOGE("parse error !");
        return -9;
    }

    memcpy((void *)data->msgData, (void *)pData, data->msgDataSize);
    EB_LOGD(EB_LOG_NORMAL, "msgData=%s", data->msgData);
    return 0;
}

int receive_busMsg(int nSocketFd, BusMsg *data)
{	 
	int  nRecvLen;
	int nAddrLen;
	struct sockaddr_in recvAddr;
	char acData[BUS_FRAME_MIN_LEN+BUS_MSGDATA_MAX_LEN + 1];
	int contentLen = 0;
	int ret;

	nAddrLen = sizeof(recvAddr);

	do {
		nRecvLen = recvfrom(nSocketFd, acData, (BUS_FRAME_MIN_LEN+BUS_MSGDATA_MAX_LEN) , 0, (struct sockaddr *)(&recvAddr), &nAddrLen);

		if(nRecvLen > 0) {
			BusAddr addr;

			memset((void *)&addr, 0, sizeof(addr));
			//strcpy(addr.ip, (char* )inet_ntoa(recvAddr.sin_addr));
			addr.port = ntohs(recvAddr.sin_port);

			memset((void *)data, 0, sizeof(BusMsg));
			ret = ctrl_decompose_frame(acData, nRecvLen, data);
			if(!ret)
				contentLen = data->msgDataSize;
			//EB_LOGE("cseq =%d,msgtype=%s\r\n",data->mCseq,data->msgType);
			//EB_LOGE( "recvAddr:%s:%d, ret =%d \r\n", addr.ip, addr.port,ret);

            break;
		}

        else if(nRecvLen < 0)
        {
            if(errno == EINTR)
                continue;

            contentLen = -1;
            break;
        }
        
		EB_LOGD(EB_LOG_NORMAL, "nRecvLen = %d contentLen: %d, errno = %d", nRecvLen, contentLen, errno);
	} while(1);

	return contentLen;
}

static int send_(int sockfd,char *ip, int port, char *cmd, int cmd_len)
{
    struct sockaddr_in clientAddr;

    EB_LOGD(EB_LOG_NORMAL, "start");

    if (ip == NULL || cmd == NULL) 
	{
        EB_LOGE("parameter error !");
        return -1;
    }

    EB_LOGD(EB_LOG_NORMAL, "ip:%s:%d, cmd_len:%d", ip, port, cmd_len);

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    clientAddr.sin_port = htons(port);
	
    if (sendto(sockfd, cmd, cmd_len, 0, (struct clientAddr *) &clientAddr,
               sizeof(clientAddr)) < 0)
   	{
        EB_LOGE("sendto failed, %s\r\n", strerror(errno));
        return -1;
    }

    EB_LOGD(EB_LOG_NORMAL, "end");
    return 0;
}


int send_busMsg(int sockfd,BusMsg *msg)
{
	int ret;
	char  frame[BUS_FRAME_MIN_LEN + BUS_MSGDATA_MAX_LEN + 10];
	ret = ctrl_compose_frame(msg , frame);
	if( ret <= 0 )
	{
      EB_LOGE(" sensor state easybus send error, ret = %d\n", ret);
	}
	else
	{
	   send_(sockfd,msg->remoteAddr.ip,msg->remoteAddr.port,frame,ret);
	   //EB_LOGE(" sensor state easybus send ok, ip=%s port=%d ret = %d msg.mCseq =%d \r\n", msg->remoteAddr.ip,msg->remoteAddr.port,ret,msg->mCseq);
	}
	return ret;
}

