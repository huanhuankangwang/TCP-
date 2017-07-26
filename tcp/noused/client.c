#include "packet.h"
#include "config.h"

#include "fileoperation.h"
#include <pthread.h>

#include <unistd.h>
#include <signal.h>

typedef  void (*time_callback)();
int  timeout = 0;
time_callback time_cb = NULL;
void sigalrm_fn(int sig) {
    if(time_cb)
        time_cb();

    printf("sigalrm_fn \r\n");
    alarm(timeout);
    return;
}
void  setTimer(int time,time_callback callback) {
    if(time <= 0)
        return;
    printf("setTimer\r\n");
    time_cb = callback;
    timeout = time;
    signal(SIGALRM, sigalrm_fn);
    alarm(time);
}


struct timeval tval= {0,800000};
fd_set rset,wset;

int sockfd;

#define  network_debug   printf

//return -1  连接失败 0 连接成功
int client_init(unsigned char *server_ip, unsigned int server_port) {
    int res,flags,error,len;
    struct sockaddr_in serv_addr;
    network_debug("client configuration\n");

    tval.tv_sec=0;
    tval.tv_usec= 800000;
    //创建socket描述符
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        network_debug("socket error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(server_port);
    serv_addr.sin_addr.s_addr=inet_addr(server_ip);
    bzero(&(serv_addr.sin_zero),8);

    flags=fcntl(sockfd,F_GETFL,0);
    fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);//设置为非阻塞

    network_debug("start connect server %s:%d .\n",server_ip,server_port);

    res = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));

    if( res < 0) { //失败
        if(errno != EINPROGRESS) {
            close(sockfd);
            sockfd = 0;
            return -1;
        } else {
            res = 0;
        }
    }

    sleep(2);
//清空读写fd集合
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_SET(sockfd,&rset);
    wset = rset;

    //判断socket描述符是否可写
    if( ( res = select(sockfd+1, NULL, &wset, NULL,&tval) ) <= 0) {
        close(sockfd);
        sockfd = -1;
        return -1;
    } else {
        len=sizeof(error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
        if(error) {
            close(sockfd);
            sockfd = -1;
            fprintf(stderr, "Error in connection() %d - %s\n", error, strerror(error));
            return  -1;
        }
    }

    return 0;
}


#define         seatNUM         "012"
void request_desk() {
    write_data(0,seatNUM,strlen(seatNUM));
}

int  heartflag;
int  connect_server = 0;
int heartbeat_pthread_loop(void *parm) {
    int times = 0;
    while(1) {

        write_data(99,seatNUM,strlen(seatNUM));
        sleep(5);

        if(heartflag == 0) {
            times++;
            if(times >= 3) {
                connect_server = 0;
                times = 0;
                break;
            }
        } else {
            times =0;
            heartflag = 0;
        }
    }

    printf("heart bag failed\r\n");
    pthread_exit(0);

    return 0;
}

void  proc_heart_bag() {
    write_data(99,"001",strlen("001"));
    if(heartflag)
        heartflag =0;
    else {
        printf("heart bag failed\r\n");
    }
}
int heartbeat_pthread_create(void*parm) {
    int write_id;

    //keep_alive = 1;

#if 01
    heartflag= 0;
    connect_server = 1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&write_id,NULL,(void *)heartbeat_pthread_loop,NULL);
    pthread_attr_destroy(&attr);
#else
    heartflag = 1;

    setTimer(2,proc_heart_bag);
#endif
    return 0;
}


int main(int argc,char *argv[]) {
    //unsigned char  server_ip[20]={0};
    unsigned char  readbuf[BUFFER_MAX_LEN];
    int type;
    int len;
    int ret;

    signal(SIGPIPE, SIG_IGN);//接收到 sigpipe 忽略
again:
    if(argc == 2)
        ret = client_init(argv[1],SERVER_PORT);
    else
        ret = client_init(SERVER_IP,SERVER_PORT);
    if(ret < 0) {
        sleep(5);
        goto again;
    }
    //成功只有可读可写
    network_debug("connect sucessed \r\n");

    int  file_count=0;
    unsigned char filename[100];

    sleep(1);

    request_desk();

    heartbeat_pthread_create(NULL);
    //T_Packet packet;
    while(1) {

        //usleep(500);
#if 0
        ret = client_tcp_recv(&packet,sizeof(T_Packet));
        if(ret == 0)
            usleep(500);
        else if(ret == sizeof(T_Packet)) {
            printf("packet data T_Packet=%s\r\n",packet.data);
        }
#else

        if (connect_server == 0) {
            printf("reconnected \r\n");
            close(sockfd);
            goto again;
        }

        //printf("while(1)\r\n");
        memset(readbuf,0,BUFFER_MAX_LEN);

        ret = read_data(&type,readbuf,&len);
        if(ret <= 0)
            continue;

        switch(type) {
            case 6:
                printf("scroll message time %d message =%s\r\n",readbuf[0],readbuf+1);
            case 8:
                printf("read message %s\r\n",readbuf);
                break;
            case 12:
                printf("desk reply=%s \r\n",readbuf);
                break;
            case 100:
                heartflag = 1;
                printf("time %s\r\n",readbuf);
                break;
            case 101:
            case 102:
            case 103:
            case 104:
                printf("type = %d len =%d \r\n",type,len);
                sprintf(filename,"write/write%d.png",file_count);
                write_to_file(filename,readbuf,len);
                file_count++;
                break;
            default:
                break;
        }

        type  = -1;


#endif
    }
}
