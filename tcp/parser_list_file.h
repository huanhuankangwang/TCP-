#ifndef _PARSER_LIST_FILE_H_
#define _PARSER_LIST_FILE_H_

//�ļ�������
#define   FULL_FILENAME_LEN			1024
//�ļ�����
#define   MAX_FILE_SIZE				50

typedef struct
{
	int  size;
	char filename[MAX_FILE_SIZE][FULL_FILENAME_LEN];
}T_ListFile,*PT_ListFile;


unsigned int parse_file_list(const unsigned char *filename);
unsigned char *get_filename(int index);


#endif//_PARSER_LIST_FILE_H_
