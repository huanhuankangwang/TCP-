#include "packet.h"

#include "config.h"

#include "fileoperation.h"
#include "parser_list_file.h"

#include <pthread.h>

#define BACKLOG         10 // max numbef of client connection


int server_fd;
int sockfd;
unsigned char data[BUFFER_MAX_LEN];


int server_init(unsigned int server_port) {
    int bReuseaddr = 1;
    struct sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket err\r\n");
        return -1;
    }

    /* setting server's socket */
    server_addr.sin_family = AF_INET;         // IPv4 network protocol
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY; // auto IP detect
    memset(&(server_addr.sin_zero),0, 8);


    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(int));
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr))== -1) {
        perror("bind:");
        return -1;
    }

    /*
    * watting for connection ,
    * and server permit to recive the requestion from sockfd
    */
    if (listen(server_fd, BACKLOG) == -1) { // BACKLOG assign thd max number of connection
        printf("listen:\r\n");
        return -1;
    }

    usleep(500);

    return 0;
}

int read_pthread_loop(void *parm) {
    unsigned char readbuf[1024];
    int len,ret,type =-1;

    while(1) {
        ret = read_data(&type,readbuf,&len);
        if(ret <0)
            break;
        if(ret == 0)
            continue;

        switch(type) {
            case 0://接收到桌牌
                printf("rev deskid = %s \r\n",readbuf);
                write_data(12,"ok",strlen("ok"));
                write_data(2,"meeting|zxf|wangkang|head|20161018235959",strlen("meeting|zxf|wangkang|head|20160901235959"));
                break;
            case 10:
                //接收到了数据 投票
                printf("revc vote cmd =%s \r\n",readbuf);
                if(strlen(readbuf) == 0)
                    printf("note voted\r\n");
                break;
            case 99://请求心跳包
                printf(" %s send heart bag\r\n",readbuf);
                write_data(100,"20160901235959",strlen("20161018235959"));//回复心跳
                break;
            default:
                break;
        }
    }
}

int read_pthread_create(void*parm) {
    int write_id;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&write_id,NULL,(void *)read_pthread_loop,NULL);
    pthread_attr_destroy(&attr);
    return 0;
}

int send_pic_pthread_loop(void *parm) {
    unsigned char  filename[128]= {0};
    unsigned char  dir[128]= {0};
    unsigned char  myfilename[128] = {0};


    int  file_size = parse_file_list("read/List.txt");
    int i=0,len;
    int type =102;
    for(i=0; i<file_size; i++) {
        //_splitpath(list_file[i],dir,filename);

        //printf("full path =%s filename =%s dir =%s\r\n",list_file[i],filename,dir);
        //mkdirs(dir);

        memset(data,0,BUFFER_MAX_LEN);
        len = read_from_file(get_filename(i),data,BUFFER_MAX_LEN);
        printf("read filename =%s len =%d\r\n",get_filename(i),len);
        if(len <= 0)
            continue;

        switch(type) {
            case 102:
                //case 103:
                //case 104:
                write_data(type,data,len);
                break;
            default:
                type =102;
                write_data(type,data,len);
                break;
        }
        type ++;
        //usleep(50);
    }
}

int send_many_pic_pthread_create(void *parm) {
    int pic_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&pic_id,NULL,(void *)send_pic_pthread_loop,NULL);
    pthread_attr_destroy(&attr);
    return 0;
}

int main() {
    int addr_size;
    struct sockaddr_in client_addr;
again:
    if( server_init(SERVER_PORT) < 0) {
        printf("server init err\r\n");
    }

    addr_size = sizeof(struct sockaddr);

    printf("server init sucess server_fd =%d addr_size =%d\r\n",server_fd,addr_size);

accept_err:
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    if ((sockfd = accept(server_fd, (struct sockaddr_in *)&client_addr, &addr_size)) == -1) {
        /* Nonblocking mode */
        perror("accept:");
        return 0;
        goto accept_err;
    }

#if 0
    char data[]="wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"
                "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"
                "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"
                "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"
                "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"
                "gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"
                "gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"
                "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                "gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"
                "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
                "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
                "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
    int len = sizeof(data) - 1;
#else


    int len = read_from_file(READ_FILE,data,BUFFER_MAX_LEN);
    printf("exit read_from_file\r\n ");
    if(len < 0)
        printf("read_from_file error\r\n");


#endif
    int type = 2;

    printf("new client connect len =%d\r\n",len);

    int ret ;
    int id;
    void *status;
    //\xE5\x8D\x9A\xE8\x96\x84
    //\x77\x61\x6E\x67\x0D\x0A\x6B\x61\x6E\x67
    unsigned char  scrollmessage[]= {0x06,0x77,0x61,0x6E,0x67,0x0D,0x0A,0x6B,0x61,0x6E,0x67};
    //投票内容(是否记名投票(1代表记名，2是不记名)+是否多选
    //1是单选，2是多选+ 投票时长（分钟）+  投票标题+投票选项。整个投票内容每一项数据以^符号分割)
#define  VOTE_NUM       10

#if(VOTE_NUM <=2)
    unsigned char  vote[] = {'1','^','2','^','2','^','t','i','^','w','^','a'};
#elif(VOTE_NUM <=4)
    unsigned char  vote[] = {'1','^','2','^','2','^','t','i','^','w','^','a','^','n'};
#elif(VOTE_NUM <=6)
    unsigned char  vote[] = {'1','^','2','^','2','^','t','i','^','w','^','a','^','n','^','g','^','k','^','a'};
#elif(VOTE_NUM <=8)
    unsigned char  vote[] = {'1','^','2','^','2','^','t','i','^','w','^','a','^','n','^','g','^','k','^','a','^','n','^','g'};
#elif(VOTE_NUM <=10)
    unsigned char  vote[] = {'1','^','2','^','2','^','t','i','^','w','^','a','^','n','^','g','^','k','^','a','^','n','^','g','^','g','^','g'};
#else
    unsigned char  vote[] = {'1','^','2','^','2','^','t','i','^','w','^','a','^','n','^','g','^','k','^','a','^','n','^','g','^','g','^','g','^','g'};
#endif
    //unsigned char  message[]={0x20,0x62,0x62,0x73,0x66,0x62,0x0D,0x0A,0x64,0x61,0x73,0x66,0x64,0x61,0x73};
    unsigned char message[]="wangasdfsakfjaskfjsakgfsakgjasangasdfsakfjaskfjsakgfsakgjas";
    while(1) {
        id = read_pthread_create(NULL);

        while(1) {
            static  int type;
            scanf("%d",&type);

            switch(type) {
                case 8://短消息
                    printf("\r\n send short message\r\n");
                    write_data(type,message,strlen(message));
                    break;
                case 9://投票操作
                    printf("\r\n send short message\r\n");
                    write_data(type,vote,strlen(vote));
                    break;

                case 6://滚动消息
                    printf("\r\n send short message\r\n");
                    write_data(type,message,strlen(message));
                    break;
                case 14:
                    printf("\r\n send cancel vote  \r\n");
                    write_data(type,"cancel",strlen("cancel"));
                    break;
                case 101:
                    printf("\r\n send pic to client len=%d \r\n",len);
                    write_data(type,data,len);
                    break;
                case 102:
                    printf("\r\n send many pic to client \r\n");
                    send_many_pic_pthread_create(NULL);
                    break;
                default:
                    break;
            }

            type = -1;
        }

        continue;
        sleep(1);
        ret = write_data(type,data,len);
        if ( ret < 0) {
            printf("net error \r\n");
            break;
        } else if(ret == 0) {

            printf("send error\r\n");
        } else {
            printf("ret =%d \r\n",ret);
            //break;
        }
    }
}
