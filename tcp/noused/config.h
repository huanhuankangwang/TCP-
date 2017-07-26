#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <stdio.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#include <sys/time.h>
#include <time.h>


#define  SERVER_IP              "192.168.1.100"
//#define  SERVER_IP                "127.0.0.1"

#define  SERVER_PORT            520

#define  BUFFER_MAX_LEN         (1024*100)
#define  WRITE_FILE             "write.png"
#define  READ_FILE              "read.png"

//使用接收pool
#define   USE_SCOKET_MANAGE

//使用大文件传输
#define     USE_BIG_PACKET

#endif
