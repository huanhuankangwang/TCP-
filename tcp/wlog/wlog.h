#ifndef _WLOG_H_
#define _WLOG_H_



#define       WLOG_LOGD()


#define WTFTP_LOGE(fmt, args...) console_print(EB_LOG_EVER, "[%s, %d] "fmt, __FUNCTION__, __LINE__, ##args);



#endif//_WLOG_H_
