#include <sys/types.h>   
#include <sys/ipc.h>   
#include <sys/msg.h>

#include <stdio.h>
#include <string.h>

#include "msg.h"

int main()
{
    char  tmp[100];
    int len;
    CSUDI_HANDLE  handler = NULL;
    CSUDI_Error_Code ret = MsgQueueCreate("wangkang",23,46,&handler);
    if(ret != CSUDI_SUCCESS)
    {
        printf("MsgQueueCreate wangkang  err\r\n");
        return 0;
    }

    HID_MSG_Info_S info;

    printf("handler =%d sizeof(long)=%d \r\n",*(int*)handler,sizeof(long));
    
    do{
        memset(tmp,0,sizeof(info));

        len = MsgQueueReceive(handler,&info,sizeof(info),100);

        if(len != CSUDI_SUCCESS)
        {
             printf("MsgQueueReceive receive current \r\n");
        }else{
            printf("MsgQueueReceive receive ok \r\n");
        }

        print_info(&info);
        //printf("recv =%s\r\n",(char*)&info);
       
    }while(1);

    return 0;
}

