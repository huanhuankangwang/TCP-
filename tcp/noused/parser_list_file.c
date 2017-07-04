#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fileoperation.h>
#include <parser_list_file.h>


static T_ListFile  listfile;


void splitpath(char *path, char *dir, char *filename)
{
	char *p_whole_name;
	
	if (NULL == path)
	{
		dir[0] = '\0';
		filename[0] = '\0';
		return;
	}

	p_whole_name = rindex(path, '/');
	if (NULL != p_whole_name)
	{//Õû¸öÃû×Ö
		p_whole_name++;
		
		snprintf(dir, p_whole_name - path +1, "%s", path);
		strcpy( filename , p_whole_name );
	}
	else
	{
		strcpy( filename , p_whole_name );
		dir[0] = '\0';
	}
  
}

int parse_file_list(const unsigned char *filename)
{
	int fd,ret,i;
	
	fd = openfile(filename,O_RDONLY);
	if(fd < 0)
	{
		return 0;
	}
	
	for(i=0;i < MAX_FILE_SIZE;i++)
	{
		memset(listfile.filename[i],0,FULL_FILENAME_LEN);
		ret = read_line(fd,listfile.filename[i],FULL_FILENAME_LEN);
		if(ret <= 0)
		{
			break;
		}
	}
exit:
	close(fd);
	listfile.size = i;
	return i;
}

unsigned char *get_filename(int index)
{
	if(index >= listfile.size)
		return NULL;

	return listfile.filename[index];
}
