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


#include <filereceiver.h>
#include <filesender.h>
#include <fileoperation.h>

#include <common.h>
#include <wlog.h>



#undef TAG
#define    TAG          "WTFTP_TEST"

#define    EB_LOGE(fmt,args...)  WTFTP_LOGE(TAG,fmt, ##args)
#define    EB_LOGD(fmt,args...)  WTFTP_LOGD(TAG,fmt, ##args)

#define    LOGD                 printf

int print_usage() {
    LOGD("arg: sendfilename receivefilename\r\n");
    return 0;
}

int getLocalIp(char *localIp) {
    int sockfd;
    char ipaddr[50];

    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        EB_LOGE("socket create failse...getLocalIp!\r\n");
        return -1;
    }

    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "ens32", sizeof(ifr_ip.ifr_name) - 1);

    if( ioctl( sockfd, SIOCGIFADDR, &ifr_ip) < 0 ) {
        return -1;
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));
    strncpy(localIp,ipaddr,20);
    close( sockfd);

    return 0;
}

int main(int argc,char **argv) {
    char  ip[20]= {0};
    PT_FileSender   sender = NULL;
    PT_FileReceiver recv   = NULL;
    char  senderfilename[20] = "0.jpg";
    char  recvfilename[20]  =  "1.jpg";
    int   bindport = 3451;
    int   port     = 4531;
    int   filesize = 0;


    if( initCommon() ) {
        LOGD("initCommon err\r\n");
        return 0;
    }

    switch(argc) {
        case 3:
            strcpy(senderfilename,argv[1]);
            strcpy(recvfilename,argv[2]);
            break;
        default:
            break;
    }
    if(getLocalIp(ip) != 0 ) {
        EB_LOGE("get local ip err\r\n");
        return 0;
    }

    EB_LOGE("ip =%s \r\n",ip);

    create_file(recvfilename);

    filesize = getFileSize(senderfilename);

    recv  = openFileReceiver(recvfilename,ip,port,bindport,filesize);
    if(!recv) {
        return 0;
    }

    sender = openFileSender(senderfilename,ip,bindport,port,filesize);
    if(!sender) {
        closeFileReceiver( recv);
        return 0;
    }

    //等待这两个退出
    FileSenderJoin(sender);
    FileReceiverJoin(recv);

    closeFileReceiver(recv);
    recv = NULL;
    closeFileSender( sender);
    sender = NULL;
    return 0;
}
