#ifndef _PARSER_LIST_FILE_H_


unsigned int parse_file_list(const unsigned char *filename);
unsigned char *get_filename(int index);
void splitpath(char *path, char *dir, char *filename);
void mkdirs(const char *dir);

#define _PARSER_LIST_FILE_H_
#endif//_PARSER_LIST_FILE_H_

