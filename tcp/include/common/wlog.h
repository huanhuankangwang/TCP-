#ifndef _WLOG_H_
#define _WLOG_H_

#define	APP_EMERG	"<0>"	/* system is unusable			*/
#define	APP_ALERT	"<1>"	/* action must be taken immediately	*/
#define	APP_CRIT	"<2>"	/* critical conditions			*/
#define	APP_ERR	    "<3>"	/* error conditions			*/
#define	APP_WARNING	"<4>"	/* warning conditions			*/
#define	APP_NOTICE	"<5>"	/* normal but significant condition	*/
#define	APP_INFO	"<6>"	/* informational			*/
#define	APP_DEBUG	"<7>"	/* debug-level messages			*/

/**这种 需要设置打印级别才能看的了**/
#define WTFTP_LOGE(TAG,fmt, args...) DebugPrint(APP_ERR, TAG,"[%s, %d]: "fmt, __FUNCTION__, __LINE__, ##args);
#define WTFTP_LOGW(TAG,fmt, args...) DebugPrint(APP_WARNING,TAG, "[%s, %d]: "fmt, __FUNCTION__, __LINE__, ##args);
#define WTFTP_LOGI(TAG,fmt, args...) DebugPrint(APP_INFO,TAG, "[%s, %d]: "fmt, __FUNCTION__, __LINE__, ##args);
#define WTFTP_LOGD(TAG,fmt, args...) DebugPrint(APP_DEBUG,TAG, "[%s, %d]: "fmt, __FUNCTION__, __LINE__, ##args);


int SetDbgLevel(char *strBuf);
int SetDbgChanel(char *strBuf);
int DebugInit(void);
int DebugPrint(char *level,char *tag,const char *pcFormat, ...);
int InitDebugChanel(void);

#endif//_WLOG_H_
