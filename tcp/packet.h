#ifndef _PACKET_H_
#define _PACKET_H_

extern  int sockfd;


int 	read_data(int *type,unsigned char *data,int *len);
int 	write_data(int type,unsigned char *data,int len);
//#define     	packet_debug			printf
#define     	packet_debug(...)


//#define   		USE_ACK

#endif//_PACKET_H_
