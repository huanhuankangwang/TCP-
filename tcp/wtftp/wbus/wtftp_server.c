

typedef struct wtftp_server{
    pthread_mutex_t   mutex;
    pthread_cond_t    cond;

    int               isOpen;
    int               sockfd;
    pthread_t         pid;
    PT_WBus           wbusHead;
}T_WtftpServer,*PT_WtftpServer;


PT_WtftpServer  wtftpserver;


static int wtftpserver_receive(PT_Sender sender,BusMsg * msg,int timeout)
{
    return receive_busMsg(sender->sockfd , msg ,timeout);
}


static int funCallBack(BusMsg *msg)
{
    PT_WBus  ptWbus = wtftpserver->wbusHead;

    for(;ptWbus!= NULL;ptWbus = ptWbus->mNext)
    {
        if(strcmp(ptWbus->mMsgType,type) == 0 && ptWbus->mPfun != NULL)
        {
            ptWbus->mPfun(msg->type,msg->data,msg->dataSize);
            if(ptWbus->mRecvPfun)
                ptWbus->mRecvPfun(wtftpserver->sockfd,msg->remoteAddr.ip,msg->remoteAddr.port);
        }
    }
}


static void *do_server_receive(void *arg)
{
    BusMsg   msg;
    int  ret;
    
    do
    {
        memset(&msg,0,sizeof(msg));
        
        ret = wtftpserver_receive(wtftpserver->sockfd,&msg);
        if(ret > 0)
        {
            funCallBack(&msg);
        }
        
    }while(1);
}

static PT_WtftpServer malloc_wtftpserver()
{
    PT_WtftpServer  ptWtftpServer = NULL;
    ptWtftpServer = malloc(sizeof(T_WtftpServer));
    if(ptWtftpServer == NULL)
    {
        return NULL;
    }

    memset(ptWtftpServer,0,sizeof(T_WtftpServer));
    pthread_mutex_init(&ptWtftpServer->mutex, NULL);
    pthread_cond_init(&ptWtftpServer->cond,NULL);
}

static int free_wtftpserver(PT_WtftpServer ptWtftpServer)
{
    int ret = -1;

    do
    {
        if(ptWtftpServer == NULL)
        {
            return break;
        }
        pthread_mutex_destroy(&ptWtftpServer->mutex);
        pthread_cond_destroy(&ptWtftpServer->cond);
        memset(ptWtftpServer,0,sizeof(T_WtftpServer));

        ret = 0;
    }while(0);

    return ret;
}


int open_wtftp_server(char *bindpord)
{
    int  ret = -1;
    PT_Sender  sender = NULL;
    int flags;
    int on = 1;
    struct sockaddr_in servAddr;
    int sockfd;
    int pid;
    PT_WtftpServer ptWtftpServer = NULL;

    do
    {
        ptWtftpServer = malloc_wtftpserver();
        if(ptWtftpServer == NULL)
        {
            break;
        }

        if((ptWtftpServer->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            free_wtftpserver(ptWtftpServer);
            break;
        }
        if((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
        {  
            close(sockfd);
            perror("setsockopt failed");
            free_wtftpserver(ptWtftpServer);
            break;
        }

        /*bind*/
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(bindpord);
        if((ret = bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0) 
        {
            close(sockfd);
            perror("setsockopt failed");
            free_wtftpserver(ptWtftpServer);
            break;
        }
        flags = fcntl(sender->sockfd , F_GETFL , 0);
        fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);//ÉèÖÃÎª·Ç×èÈû

        ret = pthread_create(&pid,NULL,do_server_receive,NULL);
        if(ret!=0)
        {
            close(sockfd);
            perror("setsockopt failed");
            free_wtftpserver(ptWtftpServer);
            break;
        }

        wtftpserver = ptWtftpServer;
        wtftpserver->isOpen = 1;
        ret = 0;
    }while(0);
    
    return 0;
}

int registerMsgCallback(char *msgType,pfCallback  cb,pfRecvCallBack recvCb)
{
    PT_WBus pWbus = NULL,*ptWbus =NULL;

    pWbus  = malloc(sizeof(T_WBus));
    if(pWbus == NULL)
    {
        return -1;
    }
    strncpy(pWbus->mMsgType,msgType,sizeof(pWbus->mMsgType));
    pWbus->mPfun = cb;
    pWbus->mRecvPfun = recvCb;
    pWbus->mNext = NULL;
    ptWbus = wBusHead;
    
    if(ptWbus == NULL)
    {
       wBusHead = pWbus;
    }else
    {
       for(;ptWbus->mNext != NULL;ptWbus= ptWbus->mNext);
       ptWbus->mNext = pWbus;
    }

    return 0;
}

