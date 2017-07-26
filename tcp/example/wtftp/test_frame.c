#include <string.h>
#include <stdio.h>

#include <packet_frame.h>


#define   BUS_FRAME_MIN_LEN   56

int main(int argc,char **argv) {
    BusMsg  out;
    BusMsg  input;
    int ret,len;
    char  frame[BUS_FRAME_MIN_LEN + BUS_MSGDATA_MAX_LEN + 1];

    memset(frame,0,sizeof(frame));
    memset(&out,0,sizeof(BusMsg));
    memset(&input,0,sizeof(BusMsg));
    strcpy(input.msgData,"wangkang");
    strcpy(input.msgType,"test");
    input.msgDataSize = strlen("wangkang");

    ret = ctrl_compose_frame(&input,frame);
    if(ret <0) {
        printf("compose_frame err\r\n");
        return 0;
    }
    printf("compose_frame ret=%d\r\n",ret);

    len = ret;
    ret = ctrl_decompose_frame(frame,len,&out);
    if(ret != 0) {
        printf("\r\n decompose_frame err\r\n");
        return 0;
    }

    printf("msgData =%s,msgType =%s\r\n",out.msgData,out.msgData);
    return 0;
}
