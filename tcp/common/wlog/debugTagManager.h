#ifndef _DEBUG_TAG_MANAGER_H_
#define _DEBUG_TAG_MANAGER_H_


//�ж��Ƿ���ڸù���Tag
int isExistDebugTag(char *tag);


//ִ�����ɾ��tag����ű�
//����logcat�����ʽ logcat -c; logcat -s TAG -s TAG -u TAG
int execCmdDebugTag(char *cmd);


#endif//_DEBUG_TAG_MANAGER_H_