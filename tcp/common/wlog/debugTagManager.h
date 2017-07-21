#ifndef _DEBUG_TAG_MANAGER_H_
#define _DEBUG_TAG_MANAGER_H_


//判断是否存在该过滤Tag
int isExistDebugTag(char *tag);


//执行添加删除tag命令脚本
//参照logcat命令格式 logcat -c; logcat -s TAG -s TAG -u TAG
int execCmdDebugTag(char *cmd);


#endif//_DEBUG_TAG_MANAGER_H_