#include <filereceiver.h>
#include <filesender.h>


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


#define    EB_LOGE  printf


int print_usage()
{
	printf("arg: sendfilename receivefilename\r\n");
	return 0;
}

int getLocalIp(char *localIp)
{
    int sockfd;
    char ipaddr[50];
  
    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;
  
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         EB_LOGE("socket create failse...getLocalIp!\r\n");  
         return -1;
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
   
    if( ioctl( sockfd, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
         return -1;     
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));    
    EB_LOGE("local ip:%s \r\n",ipaddr);
	strncpy(localIp,ipaddr,20);
    close( sockfd);  
      
    return 0;  
}

int main(int argc,char **argv)
{
	char  ip[20]={0};
	PT_FileSender   sender = NULL;
	PT_FileReceiver recv   = NULL;
	char  *senderfilename = argv[1];
	char  *recvfilename   = argv[2];
	int   bindport = 12345;
	int   port     = 13245;
	
	if(argc != 3)
	{
		print_usage();
		return 0;
	}

	if(getLocalIp(ip) != 0 )
	{
		printf("get local ip err\r\n");
		return 0;
	}

	printf("ip =%s \r\n",ip);

	create_file(recvfilename);
	
	recv  = openFileReceiver(recvfilename,ip,port,bindport);
	if(!recv)
	{
		closeFileSender( sender);
		return 0;
	}
	
	sender = openFileSender(senderfilename,ip,bindport,port);
	if(!sender)
	{
		return 0;
	}

	while(1);



	closeFileReceiver(recv);
	closeFileSender( sender);
	return 0;
}