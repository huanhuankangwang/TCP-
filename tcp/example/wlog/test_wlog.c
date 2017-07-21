#include <wlog.h>

#include <stdio.h>

#define   TAG_WANG       "wangkang"
#define   TAG            "test_wlog"

int main(int argc,char **argv)
{
    if( DebugInit() )
    {
        printf("DebugInit err\r\n");
        return -1;
    }

    if( InitDebugChanel() )
    {
        printf("InitDebugChanel err\r\n");
        return -1;
    }

    execCmdDebugTag("logcat -s wangkang -s test_wlog");
    
    WTFTP_LOGE(TAG,"jst for test\r\n");
    WTFTP_LOGD(TAG,"jst for test\r\n");
    WTFTP_LOGI(TAG,"jst for test\r\n");
    WTFTP_LOGW(TAG,"jst for test\r\n");

    WTFTP_LOGE(TAG_WANG,"jst for test\r\n");
    WTFTP_LOGD(TAG_WANG,"jst for test\r\n");
    WTFTP_LOGI(TAG_WANG,"jst for test\r\n");
    WTFTP_LOGW(TAG_WANG,"jst for test\r\n");

    return 0;
}
