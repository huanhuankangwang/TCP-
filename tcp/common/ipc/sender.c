#include <sys/types.h>   
#include <sys/ipc.h>   
#include <sys/msg.h>

#include <stdio.h>
#include <string.h>

#include "msg.h"

int main()
{
    char  tmp[100];
    HID_MSG_Info_S   info=
    {
        11211212,
        1,
        1,
        1,
        1,
        3,
        12567,
        5,
        9,
        "wangkang"
    };
    HID_MSG_Info_S  * pinfo;

    int len;
    CSUDI_HANDLE  handler = NULL;
    CSUDI_Error_Code ret = MsgQueueCreate("wangkang",23,46,&handler);
    if(ret != CSUDI_SUCCESS)
    {
        printf("MsgQueueCreate wangkang  err\r\n");
        return 0;
    }

    pinfo = malloc(sizeof(HID_MSG_Info_S));
    if(!pinfo)
    {
        return 0;
    }
    memcpy(pinfo,&info,sizeof(HID_MSG_Info_S));

    printf("handler =%d sizeof(long)=%d \r\n",*(int*)handler,sizeof(long));
    
    do{
        memset(tmp,0,sizeof(tmp));
        scanf("%s",tmp);

        if(strcmp(tmp,"send") != 0)
        {
            continue;
        }
        
        ret = MsgQueueSend(handler,pinfo,sizeof(HID_MSG_Info_S),100);

        if(len != CSUDI_SUCCESS)
        {
             printf("MsgQueueSend send failed \r\n ");
        }else{
            printf("MsgQueueSend send ok \r\n");
        }

       print_info(pinfo);
    }while(1);

    return 0;
}
