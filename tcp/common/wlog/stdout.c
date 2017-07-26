#include "debug_manager.h"
#include <stdio.h>
#include <string.h>


static int StdoutDebugPrint(char *strData) {
    /* 直接把输出信息用printf打印出来 */
    printf("%s", strData);
    return strlen(strData);
}

static T_DebugOpr g_tStdoutDbgOpr = {
    .name       = "stdout",
    .isCanUse   = 1,
    .DebugPrint = StdoutDebugPrint,
};

int StdoutInit(void) {
    return RegisterDebugOpr(&g_tStdoutDbgOpr);
}

