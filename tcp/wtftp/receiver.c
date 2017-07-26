#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>


#include <receiver.h>
#include <packet_frame.h>
#include <pthread_define.h>

#include <easy_common.h>
#include "config.h"


#define  SENDER_MSG_TYPE                "sender"
#define  REPLY_MSG_TYPE                 "reply"
#define  REPLAY_OK                      "reply ok"
#define  REPLAY_FAILED                  "replay failed"


static int receive_reply(PT_Receiver recv,char *cmd,int len,int Cseq) {
    BusMsg data;
    memset( &data, 0, sizeof(BusMsg));
    strncpy(data.remoteAddr.ip, recv->remoteIp,
            MAX_REMOTE_IP_LEN > BUS_ADDR_MAX_LEN ? BUS_ADDR_MAX_LEN : MAX_REMOTE_IP_LEN);
    data.remoteAddr.port  = recv->port;
    strcpy(data.msgType, REPLY_MSG_TYPE);

    data.msgDataSize  = len > BUS_MSGDATA_MAX_LEN ? BUS_MSGDATA_MAX_LEN : len ;
    memcpy(data.msgData, cmd, data.msgDataSize);
    data.mode   = 0;
    data.mCseq  = Cseq;

    return send_busMsg(recv->sockfd, &data);
}

static int receiver_receive(PT_Receiver recv,BusMsg * msg,int timeout) {
    return receive_busMsg(recv->sockfd, msg, timeout);
}

PT_Receiver malloc_receiver(const char *remoteIp,int port,int bindPort,int size) {
    PT_Receiver  recv = NULL;
    do {
        if(!remoteIp)
            break;
        recv = (PT_Receiver)malloc(sizeof(T_Receiver));
        if(!recv)
            break;
        memset(recv,0, sizeof(T_Receiver));
        init_messageQueue(&recv->queue);
        pthread_mutex_init(&recv->mutex, NULL);
        pthread_cond_init(&recv->cond,NULL);
        recv->flag         = FLAG_VALID;
        recv->isRunning  = RUNNING;
        recv->port     =  port;
        recv->cseq     = 0;
        recv->mRecvSize= size;
        memcpy(recv->remoteIp,remoteIp,MAX_REMOTE_IP_LEN);
    } while(0);

    return recv;
}

int free_receiver(PT_Receiver recv) {
    if(recv) {
        pthread_mutex_destroy(&recv->mutex);
        pthread_cond_destroy(&recv->cond);
        deinit_messageQueue(&recv->queue);
        memset(recv,0,sizeof(T_Receiver));
        free(recv);
        recv = NULL;
    }
}

static void *do_receive_thread(void*arg) {
    PT_Receiver recv = (PT_Receiver)arg;
    MessageRecord  * record = NULL;
    if(!arg)
        return NULL;
    BusMsg  msg;
    int ret;

    wsignal();

    do {
        memset(&msg,0,sizeof(BusMsg));
#if 0
        //接收
        if ( receiver_receive(recv,&msg,5000) >0) {
            printf("recv queue len =%d msg.cseq =%d\r\n",recv->queue.mLen,msg.mCseq);
            receive_reply(recv,REPLAY_OK,strlen(REPLAY_OK),msg.mCseq);
            record   = malloc_record(msg.mCseq,0,msg.msgData,msg.msgDataSize);
            if(!record) {
                break;
            }
            enqueue(&recv->queue,record);

            //发送
        }

#else
        if ( receiver_receive(recv,&msg,3000) >0) {
            receive_reply(recv,REPLAY_OK,strlen(REPLAY_OK),msg.mCseq);
            //发送
            record  = findByCSeq(&recv->queue,msg.mCseq);
            if(!record) {
                EB_LOGD("recv queue len =%d msg.cseq =%d\r\n",recv->queue.mLen,msg.mCseq);
                record   = malloc_record(msg.mCseq,0,msg.msgData,msg.msgDataSize);
                if(!record) {
                    break;
                }
                enqueue(&recv->queue,record);
            }
        }


        if(recv->mRecvSize <= 0) {
            break;
        }

#endif

    } while(recv->isRunning == RUNNING);

    recv->isRunning = RUNNING_QUIT;
    recv->flag      = FLAG_NOT_VALID;
}

PT_Receiver openReceiver(const char *remoteIp,int port,int bindPort,int size) {
    int  ret = 0;
    PT_Receiver  recv = NULL;
    int flags;
    int on = 1;
    struct sockaddr_in servAddr;

    do {
        recv = malloc_receiver(remoteIp,port,bindPort,size);
        if(!recv)
            break;

        if((recv->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            free_receiver(recv);
            recv = NULL;
            break;
        }

        /*设置为非阻塞*/
        flags = fcntl(recv->sockfd, F_GETFL, 0);
        fcntl(recv->sockfd,F_SETFL,flags|O_NONBLOCK);

        /*设置端口复用*/
        if((setsockopt(recv->sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0) {
            perror("setsockopt failed");
            free_receiver(recv);
            recv = NULL;
            break;
        }

        /*bind*/
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(bindPort);
        if((ret = bind(recv->sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0) {
            EB_LOGE("wangkang bind error: %d, %s", ret, strerror(errno));
            close(recv->sockfd);
            free_receiver(recv);
            recv = NULL;
            break;
        }

        ret = pthread_create(&recv->pid,NULL,do_receive_thread,(void*)recv);
        if(ret != 0) {
            close(recv->sockfd);
            free_receiver(recv);
            recv = NULL;
            break;
        }

        EB_LOGE("receive ip:%s port:%d sockfd =%d\r\n",recv->remoteIp,bindPort,recv->sockfd);
    } while(0);

    return recv;
}

int closeReceiver(PT_Receiver recv) {
    if(recv) {
        if(isFlagValid(recv)) {
            pthread_mutex_lock(&recv->mutex);
            pthread_cond_signal(&recv->cond);
            pthread_mutex_unlock(&recv->mutex);
        } else {
            recv->isRunning = NOT_RUNNING;
            while(recv->isRunning == RUNNING_QUIT) {
                EB_LOGE(" running =%d flag = %d ",recv->isRunning,recv->flag);
                usleep(200);
            }//等待退出成功
        }

        close(recv->sockfd);
        //pthread_kill(tid, SIGTERM); //强制杀死
        free_receiver(recv);
        recv = NULL;
    }

    return 0;
}

int readReceiver(PT_Receiver recv,const char *cmd,int maxsize) {
    int ret = 0;
    MessageRecord  *record =NULL;

    if(recv->mRecvSize <= 0) {
        recv->isRunning = NOT_RUNNING;
        return -1;
    }

    record = removeOneByCseq(&recv->queue, recv->cseq);
    if(record) {
        EB_LOGD("readReceiver cseq =%d\r\n",recv->cseq);
        recv->cseq++;
        ret = record->mLen > maxsize ? maxsize : record->mLen;
        memcpy(cmd,record->fContentStr, ret);

        recv->mRecvSize -= ret;
        free_record(record);
        record = NULL;
    } else {
        ret = 0;
    }

    return ret;
}


int receiverJoin(PT_Receiver recv) {
    pthread_join(recv->pid,NULL);
}
