#ifndef _EASY_BUS_COMMON_H_
#define _EASY_BUS_COMMON_H_

#ifndef LOGD
#define LOGD printf
#endif
#ifndef LOGE
#define LOGE printf
#endif

#define EB_DEBUG

//#define PROPERTY_VALUE_MAX 16
#define EASY_BUS_BUFF_MAX_LEN (1280)

#ifdef EB_DEBUG
#define EB_LOG_LEVEL  "easybus.level"

#define EB_LOG_EVER  "0" //ever debugging messages
#define EB_LOG_BRIEF  "1" //brief debugging messages
#define EB_LOG_NORMAL  "2" //normal debugging messages
#define EB_LOG_VERBOSE  "3" //verbose debugging messages

#define EB_LOGD(level, fmt, args...) easy_print(level, "[%s, %d] "fmt, __FUNCTION__, __LINE__, ##args);

#define  EB_PRINT_MEM  easy_print_mem

#else
#define  EB_LOGD(...)  ((void)0)
#define  EB_PRINT_MEM
#endif

#define EB_LOGE(fmt, args...) LOGE("[%s, %d] "fmt, __FUNCTION__, __LINE__, ##args);

extern unsigned long easy_crc32(void *pvStartAddress, unsigned long dwSizeInBytes);
extern void easy_print_mem(char *msg, char *buf, int size);
extern void easy_print(char *level, const char *fmt, ...);

#endif
