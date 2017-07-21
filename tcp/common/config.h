#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <wlog.h>


#undef TAG
#define    TAG          "COMMON"

#define    EB_LOGE(fmt,args...)  WTFTP_LOGE(TAG,fmt, ##args)
#define    EB_LOGD(fmt,args...)  WTFTP_LOGD(TAG,fmt, ##args)

#define    LOGD                 printf
#define    LOGE                 printf


#endif//_CONFIG_H_

