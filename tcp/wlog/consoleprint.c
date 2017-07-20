#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>


static  char levelProperty[4];

void console_print(char *level, const char *fmt, ...)
{
    struct tm *ptm;
    long ts;
    int y,m,d,h,n,s;
    char timeInfo[48];
    
    if (strcmp(level, levelProperty) <= 0) 
    {
        va_list ap;
        char *buf;
        const int buflen = 10 * 1024;

        buf = malloc(buflen);
        if (buf == NULL) {
            return;
        }
        memset(buf, 0, sizeof(buf));

        va_start(ap, fmt);
        vsnprintf(buf, buflen, fmt, ap);
        va_end(ap);

        ts = time(NULL);
        ptm = localtime(&ts);
        y   =   ptm-> tm_year+1900;
        m   =   ptm-> tm_mon+1;
        d   =   ptm-> tm_mday;
        h   =   ptm-> tm_hour;
        n   =   ptm-> tm_min;
        s   =   ptm-> tm_sec;

        sprintf(timeInfo, "[%02d-%02d-%02d %02d:%02d:%02d] [%s]",y, m,d,h,n,s,level);
        LOGD("%s",timeInfo);
        LOGD("%s", buf);

        free(buf);
    }

}


int setConsolePrint(char *level)
{
	int len = 0;
	len = strlen(level);

	if(len<0 && len >= sizeof(levelProperty))
		return 1;

	strncpy(levelProperty,level,len);
	return 0;
}


