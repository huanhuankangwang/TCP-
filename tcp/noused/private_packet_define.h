#ifndef _PRIVATE_PACKET_DEFINE_H_
#define _PRIVATE_PACKET_DEFINE_H_

#include "config.h"


#ifdef USE_BIG_PACKET
#define    PACKET_DATA_LEN			512
#else
#define    PACKET_DATA_LEN			128
#endif

#pragma pack(1)


typedef struct packet_t{
	unsigned char  packet_head;
#ifdef USE_BIG_PACKET
	unsigned short left_packet_num;
	unsigned short packet_num;
#else
	unsigned char  left_packet_num;
	unsigned char  packet_num;
#endif
	unsigned char  packet_type;
#ifdef USE_BIG_PACKET
	unsigned short packet_data_len;
#else
	unsigned char  packet_data_len;
#endif
	unsigned char  data[PACKET_DATA_LEN];
	unsigned char  check_sum;
}T_Packet,*PT_Packet;

typedef struct service_list_t{
	T_Packet packet;
	struct service_list_t *next;
}T_Service_List,*PT_Service_List;

#endif//_PRIVATE_PACKET_DEFINE_H_
