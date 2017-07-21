#include "wlog.h"

#include <stdio.h>
#include "config.h"

int initCommon()
{
    int  ret =0;
    do
    {
        if( DebugInit() )
        {
            LOGE("DebugInit err\r\n");
            ret = -1;
            break;
        }

        if( InitDebugChanel() )
        {
            LOGE("InitDebugChanel err\r\n");
            ret = -1;
            break;
        }
     }while(0);

    return ret;
}
