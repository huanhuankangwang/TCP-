#include "packet.h"
#include "private_packet_define.h"


#define    		SOH             0x01
#define    		STX             0x02
#define    		EOT             0x04
#define    		ACK             0x06
#define    		NAK             0x15
#define    		CAN             0x18
#define    		CTRLZ           0x1A

#ifdef     USE_ACK
#define    RETRY_TIMES				(1)
#else
#define    RETRY_TIMES				(0)
#endif
#define  isPacketHead(phead)	(*phead==STX)
#define  set_packet_Head(phead) (*(phead))=STX


#ifdef  USE_SCOKET_MANAGE
//最大同时下载量每个用户同时下载量
#define       MAX_ONLINE_DOWNLOAD			10
#else
#define		  MAX_ONLINE_DOWNLOAD			1
#endif


void debug_print_packet(PT_Packet packet)
{
	printf( "head %d %d %d %d %d ",packet->packet_head,packet->left_packet_num,\
		packet->packet_num,packet->packet_type,packet->packet_data_len);
	printf("   checksum =%d \r\n", packet->check_sum);
}

int client_tcp_send(unsigned char *buf , int size)
{
	int len;
	int ret = 0,  retry =0;
	do{
		if(sock_can_write(sockfd) != 1)
		{
			printf("sock can write error\r\n");
			ret =0;
			return ret;
		}
		len = send(sockfd, buf, size, 0);
		if(len < 0)
		{
			if(errno == EAGAIN)//无数据读
			{
				ret = 0;
				break ;
			}
			else if(errno == EINTR)//由于信号中断没有读到数据
			{
				ret = 0;
				break ;
			}else
			{
				ret = -1;
				return ret;
			}
		}else if(len == 0)
		{  
			printf("write err\r\n");
			ret = -1;
			return ret;
		}        
		buf += len;
		size -= len;
		ret += len;
	}while(size > 0);

	return ret;
}

int sock_can_read(int fd)
{
	int rc;
	fd_set fds;
	static struct timeval out;
	FD_ZERO(&fds);
	FD_SET(fd,&fds);

	out.tv_sec  = 1;
	out.tv_usec = 80000;//0.1s
	
	rc = select(fd+1, &fds, NULL, NULL, &out);
	if (rc < 0)   //error
		return -1;

	return FD_ISSET(fd,&fds) ? 1 : 0;
}

int sock_can_write(int fd)
{
	int ret = 0;
	fd_set write_fdset;
	struct timeval timeout;
	
	FD_ZERO(&write_fdset);
	FD_SET(fd, &write_fdset);
	
	timeout.tv_sec = 1;
	timeout.tv_usec = 80000;
	
	ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
	
	if(ret <0)
		return ;

	return FD_ISSET(fd,&write_fdset) ? 1 : 0;
}


int client_tcp_recv(unsigned char *buf, int size)
{    
	int len;
	int ret = 0;
	do{
		if(sock_can_read(sockfd) != 1){
			//printf("no data ret =%d\r\n",ret);
			ret =0;
			break;
		}
		len = recv(sockfd, buf, size, 0);
		if(len < 0)
		{
			if(errno == EAGAIN)//无数据读
			{
				ret = 0;
				break ;
			}
			else if(errno == EINTR)//由于信号中断没有读到数据
			{
				ret = 0;
				break;
			}else
			{
				ret = -1;
				return ret;
			}
		}else if(len == 0)        
		{    
			return ret;
		}  

		
		//printf("recv len =%d\r\n",len);
		buf += len;
		size -= len;
		ret += len;
	}while(size > 0);

	return ret;
}


PT_Service_List service_malloc()
{
	PT_Service_List plist = NULL;
	plist = (PT_Service_List*)malloc( sizeof(T_Service_List));
	if(plist)
		plist->next = NULL;
	return plist;
}

void service_free(PT_Service_List list)
{
	if(list)
	{
		list->next =NULL;
		free((void*)list);
	}
}

//只将他插入列表中去
int insert_packet(PT_Service_List *head,PT_Service_List packet)
{
	PT_Service_List phead;
	
	if(packet == NULL && head == NULL)
			return 0;
	phead = *head;
	
	if(phead == NULL)
	{
			*head = packet;//修改下
			packet->next =NULL;
			return 1;
	}
		
	while(phead->next)
	{
		//printf("next\r\n");
		phead = phead->next;
	}

	packet->next =NULL;
	phead->next = packet;
	return 1;
}

static unsigned char _calculate_check_sum(PT_Packet packet)
{
	unsigned char *ppacket = (unsigned char *)packet;
	
	unsigned short i =0;
	unsigned check_sum = 0;
	for(i = 0;i < sizeof(T_Packet) - sizeof(packet->check_sum) ;i++)
	{
		check_sum ^= ppacket[i];
	}

	return check_sum;
}	

void delete_packet_list(PT_Service_List *manage)
{
	PT_Service_List phead = NULL,pnext;
	
	packet_debug("enter delete_packet_list\r\n");
	if(manage == NULL || *manage == NULL)
		return;

	phead 		   = *manage;
	
	while(phead)
	{
		pnext = phead->next;
		service_free(phead);
		phead = pnext;
	}

	packet_debug("exit delete_packet_list\r\n");
	*manage = NULL;
}

//准备一个发送列表包
static PT_Service_List _prepare_send_packet(int type,unsigned char *data,int len)
{
	if(data == NULL)
		return;
	//int len = strlen(data);
	int index=0;
	int max_index = (len / PACKET_DATA_LEN) + 1;

	PT_Service_List head =NULL ,service_list =NULL;

	packet_debug("max_index =%d \r\n",max_index);
	while(max_index)
	{
		service_list = service_malloc();
		set_packet_Head(&service_list->packet.packet_head);
		service_list->packet.left_packet_num = max_index-1;
		service_list->packet.packet_num  = index+1;
		service_list->packet.packet_type = type;
		if(max_index == 1)
		{
			memset(service_list->packet.data, CTRLZ, PACKET_DATA_LEN);
			service_list->packet.packet_data_len = len%PACKET_DATA_LEN;

			//service_list->packet.packet_data_len = htons(service_list->packet.packet_data_len);
			memcpy(service_list->packet.data , data+index*PACKET_DATA_LEN,service_list->packet.packet_data_len);
		}else{
			service_list->packet.packet_data_len = PACKET_DATA_LEN;
			memcpy(service_list->packet.data , data+index*PACKET_DATA_LEN,PACKET_DATA_LEN);
		}

		//printf("left =%d num =%d data_len =%d \r\n",service_list->packet.left_packet_num,service_list->packet.packet_num,service_list->packet.packet_data_len);
//计算校验和	
		service_list->packet.check_sum   = _calculate_check_sum(&service_list->packet);

		service_list->next = NULL;
		//debug_print_packet(service_list);
		insert_packet(&head,service_list);
		//printf("exit done....1\r\n");
		index++;
		max_index--;
	}

	//printf("end prepare\r\n");
	return head;
}

//等待ack确认 返回值为-1  网络错误
// 返回值为 1  收到ack 
//返回值为2 收到nak,
//否则接收失败

//就是发送一个ACK 字符

//发送成功 返回1  发送错误返回0  网络错误返回-1
int _send_packet(PT_Packet  packet)
{
	int ret,retry = RETRY_TIMES;
#ifdef USE_BIG_PACKET
	packet->left_packet_num= htons(packet->left_packet_num);
	packet->packet_num= htons(packet->packet_num);
	packet->packet_data_len = htons(packet->packet_data_len);
	//printf(" send data len =%d \r\n",packet->packet_data_len);
#endif
send_again:
	ret = client_tcp_send((unsigned char*)packet , sizeof(T_Packet));
	if(ret < 0)
		return ret;
	else
	if(ret == sizeof(T_Packet))
		return 1;

	if(retry > 0)
	{
		retry--;
		goto send_again;
	}else{
		
		return 0;//发送失败
	}

	//ack 1
	return 1;
}


static int _send_service_list(PT_Service_List list)
{
	int ret =1;

	while(list){
		//debug_print_packet(&list->packet);
		ret = _send_packet(&list->packet);
		if(ret <= 0)
			break;
		list = list->next;
	}

	return ret;
}


//进行数据包的校验，校验成功返回1 否则返回0
//异或校验
static int _checkSum(PT_Packet packet)
{
	unsigned char i =0;
	unsigned check_sum = 0;

	if(packet ==0 || !isPacketHead(&packet->packet_head)){
	//if(packet ==0 || packet->packet_head != STX){
		//printf("packet =%d packet->packet_head =%d\r\n",packet,packet->packet_head);
		return 0;
	}

#if 0
	check_sum = packet->left_packet_num;
	check_sum ^=packet->packet_num;
	check_sum ^=packet->packet_type;
	check_sum ^=packet->packet_data_len;

	for(i=0;i < PACKET_DATA_LEN ;i++)
	{
		check_sum ^= packet->data[i];
	}
#else
	check_sum = _calculate_check_sum( packet);
	
	//printf("check_sum =%d , packet->check_sum =%d \r\n",check_sum,packet->check_sum);
#endif
	//
	if(check_sum == packet->check_sum)
		return 1;
	else{
		//printf("check_sum =%d , packet->check_sum =%d \r\n",check_sum,packet->check_sum);
		return 0;
	}
}


//接收 一个数据包接收到了一个数据包返回1  
//网络异常 -1    没有收到一个包，或数据出错返回0
int _receive_Onepacket(PT_Packet packet)
{
	int ret =0,retry = RETRY_TIMES;
	unsigned char  head =0;
	unsigned char *ppacket = (unsigned char*)packet;
	ppacket = ppacket + sizeof(head);
read_head:
	ret = client_tcp_recv((unsigned char*)(&head),sizeof(head));
	if(ret <= 0)
		return ret;

	//printf("head =%d\r\n",head);
	if(ret != sizeof(head)){
		//printf("not head\r\n");
		return 0;
	}
	
	if(!isPacketHead(&head)){
		//printf("not head\r\n");
		goto read_head;
	}

	packet->packet_head = head;
again:
	ret = client_tcp_recv(ppacket,sizeof(T_Packet)-sizeof(head));

	//printf("len ret =%d sizeof(T_Packet) =%d\r\n",ret,sizeof(T_Packet));
	if(ret < 0)
	{
		return -1;
	}
	
	if(ret != sizeof(T_Packet) - sizeof(head)){
		//printf("not equal\r\n");
		return 0;
	}
#ifdef USE_BIG_PACKET
	packet->packet_num= ntohs(packet->packet_num);
	packet->left_packet_num= ntohs(packet->left_packet_num);

	packet->packet_data_len = ntohs(packet->packet_data_len);
	//printf(" recv data len =%d num=%d left=%d\r\n",packet->packet_data_len,packet->packet_num,packet->left_packet_num);

#endif	

#if 0
	if( _checkSum(packet) != 1 ){
		if(retry){
			retry--;
			goto again;
		}else
			return 0;
	}
#endif
	return 1;
}

//读取包
/************************************
**************************************
*输入参数: head  设置为空
**************************************
**************************************/

#if 0
static PT_Service_List receive_service_list(int *err)
{
	PT_Service_List head = NULL,list =NULL;
	static T_Service_List packet;
	int ret;
	unsigned int left_packet_num;
	
	packet.next = NULL;
	ret = _receive_Onepacket(&packet.packet);
	if(ret <= 0){
		head = NULL;
		goto exit;
	}
copy:
	head = service_malloc();
	if(head)
		memcpy(head,&packet,sizeof(T_Service_List));
	else
		goto exit;
	head->next = NULL;
	//printf("receive_service_list head =%d\r\n",head);
	if(head->packet.left_packet_num <=0)
		goto exit;
//继续进行接收
again:
	list = service_malloc();
	if(list == NULL){
		_free_all_service_list(head);
		return NULL;
	}
	ret = _receive_Onepacket(&list->packet);
//网络出错  数据出错直接退出。
	if(ret <= 0){
		free(list);
		_free_all_service_list(head);
		head = NULL;
		goto exit;
	}
	else{
		left_packet_num = list->packet.left_packet_num;
		
		//printf("packet data left_packet_num =%d\r\n",left_packet_num);
		//读到了一个包
		if( insert_packet(&head,list) !=1 ){
			printf("add failed\r\n");
		}
		list = NULL;

		if(left_packet_num > 0)
			goto again;
	}
exit:
	*err = ret;
	return head;
}

int read_data_nopool(int *type,unsigned char *data,int *len)
{
	int err;
	if(type == NULL || data ==NULL || len == NULL)
		return 0;
	*len = 0;
	int size=0;
	*type = -1;

	PT_Service_List head = receive_service_list(&err);

	if(err < 0)
		return err;
	
	if(head == NULL)
		return 0;

	//printf("head = %d\r\n",head);
	//进行数据的拼接

	*type = head->packet.packet_type;

	while(head)
	{	
		memcpy( data+size ,head->packet.data , head->packet.packet_data_len);
		size += head->packet.packet_data_len;
		//printf("left_num =%d,len=%d packet_len =%d\r\n",head->packet.left_packet_num,size,head->packet.packet_data_len);
		head = head->next;
	}

	_free_all_service_list(head);

	*len = size;
	return *len;
}

#endif
//成功返回1 失败返回0   网络错误返回 -1
int write_data(int type,unsigned char *data,int len)
{
	int ret ;
	PT_Service_List head = _prepare_send_packet(type,data,len);
	
	ret = _send_service_list(head);
	packet_debug("send_data ret =%d\r\n",ret);
	delete_packet_list(&head);
	
	return ret;
}

enum{
	PACKET_MODE_WRITE=0,
	PACKET_MODE_READ,
};

enum{
	PACKET_STATUS_TIMEOUT =0,
	PACKET_STATUS_IN_USE,
	PACKET_STATUS_NO_USE,
	PACKET_STATUS_COMPLETE,
};

static PT_Service_List packet_manage[MAX_ONLINE_DOWNLOAD]={0};

//在系统初始化中调用一次进行初始化操作
void init_packet_manage()
{
	int i=0;
	
	for(i=0;i<MAX_ONLINE_DOWNLOAD;i++)
	{//默认写模式
		packet_manage[i] = NULL;
	}
}

int isSamePacket(PT_Service_List list1,PT_Service_List list2)
{
	if(list1->packet.packet_type == list2->packet.packet_type)
		return 1;
	else
		return 0;
}


//判断既是第一个也是最后一个包
//也就是说这个包只有一个包组成
int packet_start_end(PT_Service_List packet)
{
	if(packet == NULL)
		return 0;

	if(packet->packet.packet_num == 1 && packet->packet.left_packet_num <=0)
		return 1;
	else
		return 0;
}

//是第一个包
int is_first_packet(PT_Service_List packet)
{
	if(packet->packet.packet_num == 1)
		return 1;
	else
		return 0;
}

//查找一个插入位置
int poll_packet(PT_Service_List list)
{
	int i,first_packet = 0;
	
	if(is_first_packet( list ) == 1)
	{
		first_packet =1;
	}

	for(i=0;i < MAX_ONLINE_DOWNLOAD;i++)
	{
		if(first_packet)
		{
			if(packet_manage[i] == NULL)
				return i;
		}else
		{
			if(packet_manage[i] && packet_manage[i]->packet.packet_type \
				== list->packet.packet_type)
			{
				return i;
			}
		}			
	}

	return -1;
}

int	read_data(int *type,unsigned char *data,int *len)
{	
	static T_Service_List static_packet;
	PT_Service_List list = NULL;
	int i=0,size =0,insert_postion=-1;
	
	static_packet.next = NULL;
//读取一个包
	i = _receive_Onepacket(&static_packet.packet);
	if(i <= 0){
		return i;
	}

	*type = -1;
	*len  = 0;
//这个包只有一个包裹	
	if(packet_start_end( &static_packet) == 1)
	{
		//可以进行组合
		goto read_data_one;//进行读取一个包
	}
//将接受到的数据添加到接受数列中去
copy:
	list = service_malloc();
	if(list)
		memcpy(list,&static_packet,sizeof(T_Service_List));
	else
		return 0;
	list->next = NULL;

	//printf("packet num =%d,len=%d \r\n",list->packet.packet_num,list->packet.packet_data_len);
//先将包插入到包列表中去
	insert_postion = poll_packet( list);
	if(insert_postion >= 0)
	{
		//printf("insert packet \r\n");
		insert_packet(&packet_manage[i],list);

		if(list->packet.left_packet_num <= 0)
		{
			goto read_data;
		}

		return 0;//未有数据读取
	}
	//插入不了
	service_free(list);
	return 0;
//只有一个包的数据包读取
read_data_one:
	*type  = static_packet.packet.packet_type;
	size   = static_packet.packet.packet_data_len;
	memcpy(data,static_packet.packet.data,size);
	*len = size;
	return size;
read_data:
	list = packet_manage[insert_postion];
	
	*type = list->packet.packet_type;
	while(list)
	{
		memcpy( data+size ,list->packet.data , list->packet.packet_data_len);
		size += list->packet.packet_data_len;
		//printf("left_num =%d,len=%d packet_len =%d\r\n",head->packet.left_packet_num,size,head->packet.packet_data_len);
		list = list->next;
	}
	
	delete_packet_list(&packet_manage[i]);
	*len = size;
	return size;
}

