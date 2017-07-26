

typedef int(*pFunWtftpClienCallback)(char *msgtype,char *msgData,int dataSize);

typedef struct _wtftp_client {
    char    mServerIp[20];
    int     mPort;
    int     sockfd;
    int     mCseq;
    pthread_t pid;
    PT_WtftpQueue  mQueue;
} T_WtftpClient,*PT_WtftpClient;


typedef struct {
} T_WtftpClientRecord,*PT_WtftpClientRecord;



static PT_WtftpClient  sPtWtftpClient = NULL;

static PT_WtftpClient malloc_wtftpClient(char *ip,int port) {
    PT_WtftpClient  wtftpclient = NULL;

    do {
        if(ip == NULL || port < 0) {
            break;
        }

        wtftpclient = malloc(T_WtftpClient);
        if(wtftpclient == NULL) {
            break;
        }

        wtftpclient->mQueue = malloc_wtftpQueue();
        if(wtftpclient->mQueue == NULL)
        {
            free(wtftpclient);
            wtftpclient = NULL;
            break;
        }
        strncpy(wtftpclient->mServerIp,ip,sizeof(wtftpclient->mServerIp));
        wtftpclient->mPort = port;
    } while(0);

    return wtftpclient;
}

static int wtftpclient_receive(PT_WtftpClient wtftpclient,BusMsg * msg,int timeout) {
    return receive_busMsg(wtftpclient->sockfd, msg,timeout);
}

int wtftpclient_send(char *msgType,char *cmd,int len,pFunWtftpClienCallback cb) {
    BusMsg data;
    PT_WtftpClient wtftpclient = sPtWtftpClient;
    memset( &data, 0, sizeof(BusMsg));
    strncpy(data.remoteAddr.ip, wtftpclient->mServerIp,
            MAX_REMOTE_IP_LEN > BUS_ADDR_MAX_LEN ? BUS_ADDR_MAX_LEN : MAX_REMOTE_IP_LEN);
    data.remoteAddr.port  = wtftpclient->mPort;
    strcpy(data.msgType, msgType);

    data.msgDataSize  = len > BUS_MSGDATA_MAX_LEN ? BUS_MSGDATA_MAX_LEN : len ;
    memcpy(data.msgData, cmd, data.msgDataSize);
    data.mode   = 0;
    data.mCseq  = wtftpclient->mCseq;

    return send_busMsg(wtftpclient->sockfd, &data);
}

static int free_wtftpClient(PT_WtftpClient wtftpclient) {
    if(wtftpclient) {
        free_wtftpQueue(wtftpclient->mQueue);
        wtftpclient->mQueue = NULL; 
        free(wtftpclient );
        return 0;
    }

    return -1;
}


void *do_wtftp_client_thread(void *arg) {
    BusMsg  msg;
    do {
        memset(msg,0,sizeof(msg));
        wtftpclient_receive(sPtWtftpClient,&msg,3000);
    } while(1);
}

int open_wtftp_client(char *serverIp,int port) {
    int  ret = -1;
    int flags;
    int on = 1;
    struct sockaddr_in servAddr;

    PT_WtftpClient  wtftpclient = NULL;

    do {
        wtftpclient = malloc_wtftpClient(serverIp, port);
        if(wtftpclient == NULL) {
            break;
        }

        if((wtftpclient->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            free_wtftpClient(wtftpclient);
            wtftpclient = NULL;
            break;
        }
        if((setsockopt(wtftpclient->sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0) {
            free_wtftpClient(wtftpclient);
            wtftpclient = NULL;
            close(wtftpclient->sockfd);
            perror("setsockopt failed");
            break;
        }

        flags = fcntl(sender->sockfd, F_GETFL, 0);
        fcntl(wtftpclient->sockfd,F_SETFL,flags|O_NONBLOCK);//ÉèÖÃÎª·Ç×èÈû

        ret = pthread_create(&wtftpclient->pid,NULL,do_wtftp_client_thread,(void*)NULL);
        if(ret!=0) {
            free_wtftpClient(wtftpclient);
            wtftpclient = NULL;
            close(wtftpclient->sockfd);
            break;
        }

        sPtWtftpClient = wtftpclient;

        ret = 0;
    } while(0);

    return ret;
}

