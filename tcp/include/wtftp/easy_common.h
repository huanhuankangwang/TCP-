#ifndef _EASY_BUS_COMMON_H_
#define _EASY_BUS_COMMON_H_


#if 0
#define  EB_PRINT_MEM  easy_print_mem
#else
	#define  EB_PRINT_MEM(...)
#endif


extern unsigned long easy_crc32(void *pvStartAddress, unsigned long dwSizeInBytes);
extern void easy_print_mem(char *msg, char *buf, int size);

#endif
