#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>  
#include <net/if.h>

#include "easy_common.h"
#include "easy_parsemsg.h"
#include "common.h"

#define EASY_BUS_FRAME_MAX_SIZE (1024*2)

#define EASY_BUS_MSG_TYPE_SENSOR  "easysensor"

#define EASYBUS_FRAME_MIN_LEN  (56)
#define EASYBUS_MAIN_VER  (1)
#define EASYBUS_SUB_VER  (1)


static char s_aucMsgHead[4] = {0x45, 0x43, 0x45, 0x42};


int send_(int sockfd,char *ip, int port, char *cmd, unsigned short cmd_len);

int easy_ctrl_decompose_frame(char *msg, int msgLen, EasybusAddr *addr, EasybusMsg *pOutdata)
{
    char *pcMsgTemp = msg;
    char *pData = NULL;
    EasybusMsg data;
    int dataLen = 0;
    int msgNum = 0;
    char crcFlag;
    unsigned int crcRecv;
    char encryptFlag;

    EB_LOGD(EB_LOG_NORMAL, "msgLen: %d", msgLen);

    if(msg == NULL || msgLen <= EASYBUS_FRAME_MIN_LEN || addr == NULL) {
        EB_LOGE("parameter error !");
        return -1;
    }

    memset((void *)&data, 0, sizeof(data));
    memcpy((void *)&data.remoteAddr, (void *)addr, sizeof(EasybusAddr));

    EB_PRINT_MEM("the receive frame:", msg, msgLen);

    //帧起始符
    if (memcmp((void *)pcMsgTemp, s_aucMsgHead, 4) != 0) {
        EB_LOGE("the msg is not formatted correctly !");
        return -2;
    }
    pcMsgTemp += 4;

    //主板本号
    if (pcMsgTemp[0] != EASYBUS_MAIN_VER) {
        EB_LOGE("main version error !");
        return -3;
    }
    pcMsgTemp++;

    //次板本号
    if (pcMsgTemp[0] != EASYBUS_SUB_VER) {
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
    EB_LOGD(EB_LOG_NORMAL, "msgNum=%d", msgNum);
    pcMsgTemp += 4;

    //消息类型
    memcpy((void *)data.msgType, (void *)pcMsgTemp, EASYBUS_MSGTYPE_MAX_LEN);
    EB_LOGD(EB_LOG_NORMAL, "msgType=%s", data.msgType);
    pcMsgTemp += EASYBUS_MSGTYPE_MAX_LEN;

    //消息数据长度
    memcpy((void *)&dataLen, (void *)pcMsgTemp, 4);
    data.msgDataSize = ntohl(dataLen);
    EB_LOGD(EB_LOG_NORMAL, "data.msgDataSize=%d", data.msgDataSize);
    pcMsgTemp += 4;

    //消息数据
    pData = pcMsgTemp;
    pcMsgTemp += data.msgDataSize;

    //校验码
    if (crcFlag == 1) {
        unsigned int crcGen;

        memcpy((void *)&crcRecv, (void *)pcMsgTemp, 4);
        crcRecv = ntohl(crcRecv);
        crcGen = easy_crc32(msg, pcMsgTemp - msg);
        EB_LOGD(EB_LOG_NORMAL, "crcRecv:0x%x, crcGen:0x%x", crcRecv, crcGen);
        if (crcRecv != crcGen) {
            EB_LOGE("crc error !");
            return -7;
        }
    }
    pcMsgTemp += 4;

    //加密码
    if (encryptFlag == 1) {
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

    if (pcMsgTemp - msg != msgLen) {
        EB_LOGE("parse error !");
        return -9;
    }

    //
    memcpy((void *)data.msgData, (void *)pData, data.msgDataSize);
    EB_LOGD(EB_LOG_NORMAL, "msgData=%s", data.msgData);
    if (strcmp(data.msgType, EASY_BUS_MSG_TYPE_SENSOR) == 0 && pOutdata != NULL) {
        memcpy((void *)pOutdata, (void *)&data, sizeof(data));
    } else {

    }

    return 0;
}

int easy_ctrl_compose_frame(int sockfd,EasybusMsg *data)
{
    char *frame, *tmp;
    int contentLen;
    char crcFlag = 1;
    unsigned int crcGen;
    char encryptFlag = 1;

    EB_LOGD(EB_LOG_NORMAL, "start");

    if (data == NULL) return -1;

    if ((frame = malloc(EASY_BUS_FRAME_MAX_SIZE)) == NULL) {
        return -2;
    }
    memset(frame, 0, EASY_BUS_FRAME_MAX_SIZE);

    tmp = frame;

    //帧起始符
    memcpy((void *)tmp, (void *)s_aucMsgHead, 4);
    tmp += 4;

    //主板本号
    tmp[0] = EASYBUS_MAIN_VER;
    tmp++;

    //次板本号
    tmp[0] = EASYBUS_SUB_VER;
    tmp++;

    //校验标识
    if (crcFlag == 1) {
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
    tmp += 4;

    //消息类型
    memcpy((void *)tmp, (void *)data->msgType, EASYBUS_MSGTYPE_MAX_LEN);
    tmp += EASYBUS_MSGTYPE_MAX_LEN;

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
    if (encryptFlag == 1) {
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

    send_(sockfd,data->remoteAddr.ip, data->remoteAddr.port, frame, tmp - frame);

    if (frame != NULL) free(frame);

    return 0;
}


int easy_receive(int nSocketFd, void *pvBuff, int bufSize)
{	 int  nRecvLen;
	int nAddrLen;
	struct sockaddr_in recvAddr;
	char acData[1024];
	int contentLen;

	nAddrLen = sizeof(recvAddr);

	do {
		nRecvLen = recvfrom(nSocketFd, acData, sizeof(acData), 0, (struct sockaddr *)(&recvAddr), &nAddrLen);

		if(nRecvLen > 0) {
			EasybusAddr addr;
			EasybusMsg data;

			memset((void *)&addr, 0, sizeof(addr));
			strcpy(addr.ip, (char* )inet_ntoa(recvAddr.sin_addr));
			addr.port = ntohs(recvAddr.sin_port);

			memset((void *)&data, 0, sizeof(data));
			easy_ctrl_decompose_frame(acData, nRecvLen, &addr, &data);
			contentLen = strlen(data.msgData);
			memcpy(pvBuff, data.msgData, contentLen < bufSize ? contentLen : bufSize);
			EB_LOGD(EB_LOG_NORMAL, "recvAddr:%s:%d, data.msgData = %s", addr.ip, addr.port, data.msgData);
		}

		EB_LOGD(EB_LOG_NORMAL, "nRecvLen = %d contentLen: %d, errno = %d", nRecvLen, contentLen, errno);
	} while (nRecvLen < 0 && errno == EINTR);

	return contentLen;
}

int send_(int sockfd,char *ip, int port, char *cmd, unsigned short cmd_len)
{
    struct sockaddr_in clientAddr;

    EB_LOGD(EB_LOG_NORMAL, "start");

    if (ip == NULL || cmd == NULL) {
        EB_LOGE("parameter error !");
        return -1;
    }

    EB_LOGD(EB_LOG_NORMAL, "ip:%s:%d, cmd_len:%d", ip, port, cmd_len);

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    clientAddr.sin_port = htons(port);
	
    if (sendto(sockfd, cmd, cmd_len, 0, (struct clientAddr *) &clientAddr,
               sizeof(clientAddr)) < 0) {
        EB_LOGE("sendto failed, %s", strerror(errno));
        return -1;
    }

    EB_LOGD(EB_LOG_NORMAL, "end");
    return 0;
}


int easy_send(int sockfd,char *ip, int port, char *cmd, unsigned short cmd_len)
{
    int ret = -1;
    EasybusMsg sendMsg;

    memset(&sendMsg,0,sizeof(sendMsg)); 
    strcpy(sendMsg.remoteAddr.ip, ip);
    sendMsg.remoteAddr.port = port;
    strcpy( sendMsg.msgType, "easyheartbeat");
	strncpy(sendMsg.msgData,cmd,cmd_len);
    
    sendMsg.msgData[sizeof(sendMsg.msgData) - 1] = '\0';       
    sendMsg.msgDataSize = cmd_len;
    
    EB_LOGE("ip = %s, port = %d, type = %s,data = %s, len = %d\n", 
        sendMsg.remoteAddr.ip, sendMsg.remoteAddr.port, sendMsg.msgType, sendMsg.msgData, sendMsg.msgDataSize  );
	ret = easy_ctrl_compose_frame(sockfd,&sendMsg);
	if( ret != 0 )
	{
      EB_LOGE(" sensor state easybus send error, ret = %d\n", ret);
	}
	else
	{
	   EB_LOGE(" sensor state easybus send ok, ret = %d\n", ret);
	}
	 
   return ret;

}

