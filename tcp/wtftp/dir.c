#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include <fcntl.h>
#include "fileoperation.h"

/*生成临时的文件temp.json*/
/*
{
    "version": "v1.1.0",
    "domain": "www.baidu.com",
    "pathname": "/root/",
    "files": [
        {
            "name": "",
            "mode": "rrx",
            "link": "rrx",
            "user":"",
            "group":"",
            "size": "232",
            "date": "sd"

        }
    ]
}

*/

#define     VERSION      "version"
#define     DOMAIN       "domain"
#define     PATHNAME      "pathname"
#define     FILES        "files"

#define     NAME         "name"
#define     MODE         "mode"
#define     LINK         "link"
#define     USER         "user"
#define     GROUP        "group"
#define     SIZE         "size"
#define     DATE         "date"


#define  getMode(st,mode)           \
        {if (st.st_mode & S_IRUSR)  \
            mode  |= S_IRUSR;      \
        if (st.st_mode & S_IWUSR)  \
            mode  |= S_IWUSR;      \
        if (st.st_mode & S_IXUSR)  \
            mode  |= S_IXUSR;      \
        if (st.st_mode & S_IRGRP)  \
            mode  |= S_IRGRP;      \
        if (st.st_mode & S_IWGRP)  \
            mode  |= S_IWGRP;      \
        if (st.st_mode & S_IXGRP)  \
            mode  |= S_IXGRP;      \
        if (st.st_mode & S_IROTH)  \
            mode  |= S_IROTH;      \
        if (st.st_mode & S_IWOTH)  \
            mode  |= S_IWOTH;      \
        if (st.st_mode & S_IXOTH)  \
            mode  |= S_IXOTH;}        

int isExistDir(char *filename) {
    return access(filename,F_OK);
}

int isChildDir(char *startPath,char *start,char *path) {
    char  dir[1024] = {0};
    if( !start || !path)
        return -1;
    int nStart = 0;
    nStart = strlen(start);
    if(strncmp(start,path,nStart) == 0 || strcmp(start,path) != 0) {
        //说明是一个子目录
        sprintf(dir,"%s%s",startPath,path+nStart);
        return isExistDir(dir);
    }

    return -1;
}

static int search_for_dir(char *buf,int len,int fd,char *pDirName);

int search_dir_to_file(char *rootPath,char *start,char *path,char *filename) {
    int ret= 0;
    int fd = -1;
    char  mFilename[1024]= {0};
    char  mBuf[1024]= {0};
    int len = 0;

    do {
        ret = isChildDir(rootPath,start,path);
        if(ret != 0) {
            printf("is not ChildDir\r\n");
            break;
        }

        sprintf(mFilename,"%s%s",rootPath,filename);
        ret = create_file(mFilename);
        if(ret != 0) {
            break;
        }

        ret = openfile(mFilename,O_WRONLY );
        if(ret < 0) {
            printf("open filename =%s err\r\n",mFilename);
            break;
        }

        fd = ret;
        sprintf(mBuf,"%s","{\r\n\"version\": \"v1.1.0\",\r\n \"domain\": \"www.baidu.com\",\r\n \"pathname\": \"/root/\",\r\n \"files\": [\r\n");
        write_fd(fd,mBuf,strlen(mBuf));
        len = strlen(start);
        sprintf(mFilename,"%s%s",rootPath,path+len);

        search_for_dir(mBuf,sizeof(mBuf),fd,mFilename);

        sprintf(mBuf,"%s","\r\n]\r\n}");
        write_fd(fd,mBuf,strlen(mBuf));

        ret = 0;
    } while(0);

    return ret;
}


static int search_for_dir(char *buf,int len,int fd,char *pDirName) {
    DIR *dir;
    struct dirent *ent;
    int off = 0;
    char timebuf[30] = {0};
    struct stat st;
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    struct tm *ptm = NULL;
    int timelen = 0;
    char *filename = NULL;
    char *mFilename[1024];

    if (((dir = opendir(pDirName))) == NULL) {
        printf("opendir is NULL dirname =%s\n",pDirName);
        return -1;
    }

    buf[0] = '\0';
    while ((ent = readdir(dir)) != NULL) {
        buf[0] = '\0';
        if(off != 0)
            write_fd(fd,",\r\n",sizeof(",\r\n"));
        off = 0;

        filename = ent->d_name;
        char mode[] = "----------";

        memset(timebuf, 0, sizeof(timebuf));
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0 )
            continue;
        sprintf(mFilename,"%s/%s",pDirName,filename);

        if (stat(mFilename, &st) < 0) {
            //nslog(NS_ERROR, "stat:%s %s Dir:%s", strerror(errno), filename, pDirName);
            continue;
        }

        if (getpwuid(st.st_uid) == NULL || getgrgid(st.st_gid) == NULL) {
            continue;
        }
        if (S_ISDIR(st.st_mode))
            mode[0] = 'd';
        if (st.st_mode & S_IRUSR)
            mode[1] = 'r';
        if (st.st_mode & S_IWUSR)
            mode[2] = 'w';
        if (st.st_mode & S_IXUSR)
            mode[3] = 'x';
        if (st.st_mode & S_IRGRP)
            mode[4] = 'r';
        if (st.st_mode & S_IWGRP)
            mode[5] = 'w';
        if (st.st_mode & S_IXGRP)
            mode[6] = 'x';
        if (st.st_mode & S_IROTH)
            mode[7] = 'r';
        if (st.st_mode & S_IWOTH)
            mode[8] = 'w';
        if (st.st_mode & S_IXOTH)
            mode[9] = 'x';
        mode[10] = '\0';

        off += snprintf(buf + off, len - off, "\"%s\":\"%s\",\r\n",NAME, filename);
        off += snprintf(buf + off, len - off, "\"mode\":\"%s\",\r\n", mode);


        /* hard link number, this field is nonsense for ftp */
        off += snprintf(buf + off, len - off, "\"%s\":\"%d\",\r\n",LINK, 1);

        /* user */
        if ((pwd = getpwuid(st.st_uid)) == NULL) {
            closedir(dir);
            return -1;
        }

        off += snprintf(buf + off, len - off, "\"%s\":\"%s\",\r\n",USER, pwd->pw_name);

        /* group */
        if ((grp = getgrgid(st.st_gid)) == NULL) {
            closedir(dir);
            return -1;
        }
        off += snprintf(buf + off, len - off, "\"%s\":\"%s\",\r\n",GROUP, grp->gr_name);

        /* size */
        off += snprintf(buf + off, len - off, "\"%s\":\"%lld\",\r\n",SIZE, (long long int)st.st_size);

        /* mtime */
        ptm = localtime(&st.st_mtime);
        if (ptm && (timelen = strftime(timebuf, sizeof(timebuf), "%b %d %H:%S", ptm)) > 0) {
            timebuf[timelen] = '\0';
            off += snprintf(buf + off, len - off, "\"%s\":\"%s\",\r\n",DATE, timebuf);
        } else {
            closedir(dir);
            return -1;
        }

        write_fd(fd,"{",sizeof("{"));
        write_fd(fd,buf,off);
        write_fd(fd,"}",sizeof("}"));

    }
    if(dir) {
        closedir(dir);
    }
    return off;
}
#if 0
static int search_for_dir(char buf[], int len, int socket, char *pDirName) {
    DIR *dir;
    struct dirent *ent;
    int off = 0;
    char timebuf[FTPBUFSIZE] = {0};
    struct stat st;
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    struct tm *ptm = NULL;
    int timelen = 0;
    char *filename = NULL;
    if (((dir = opendir(pDirName))) == NULL) {
        nslog(NS_ERROR, "opendir is NULL\n");
        return FALSE;
    }
    FTPSendReq(socket, g_ftpCmd_Req[0]);//"150 Begin transfer"
    buf[0] = '\0';
    while ((ent = readdir(dir)) != NULL) {
        filename = ent->d_name;
        char mode[] = "----------";

        memset(timebuf, 0, sizeof(timebuf));
        if (strcmp(filename, ".") == 0)
            continue;
        if (stat(filename, &st) < 0) {
            nslog(NS_ERROR, "stat:%s %s Dir:%s", strerror(errno), filename, pDirName);
            continue;
        }
        if (getpwuid(st.st_uid) == NULL || getgrgid(st.st_gid) == NULL) {
            continue;
        }
        if (S_ISDIR(st.st_mode))
            mode[0] = 'd';
        if (st.st_mode & S_IRUSR)
            mode[1] = 'r';
        if (st.st_mode & S_IWUSR)
            mode[2] = 'w';
        if (st.st_mode & S_IXUSR)
            mode[3] = 'x';
        if (st.st_mode & S_IRGRP)
            mode[4] = 'r';
        if (st.st_mode & S_IWGRP)
            mode[5] = 'w';
        if (st.st_mode & S_IXGRP)
            mode[6] = 'x';
        if (st.st_mode & S_IROTH)
            mode[7] = 'r';
        if (st.st_mode & S_IWOTH)
            mode[8] = 'w';
        if (st.st_mode & S_IXOTH)
            mode[9] = 'x';
        mode[10] = '\0';
        off += snprintf(buf + off, len - off, "%s ", mode);

        /* hard link number, this field is nonsense for ftp */
        off += snprintf(buf + off, len - off, "%d ", 1);

        /* user */
        if ((pwd = getpwuid(st.st_uid)) == NULL) {
            closedir(dir);
            return FALSE;
        }
        off += snprintf(buf + off, len - off, "%s ", pwd->pw_name);

        /* group */
        if ((grp = getgrgid(st.st_gid)) == NULL) {
            closedir(dir);
            return FALSE;
        }
        off += snprintf(buf + off, len - off, "%s ", grp->gr_name);

        /* size */
        off += snprintf(buf + off, len - off, "%*lld ", 10, (long long int)st.st_size);

        /* mtime */
        ptm = localtime(&st.st_mtime);
        if (ptm && (timelen = strftime(timebuf, sizeof(timebuf), "%b %d %H:%S", ptm)) > 0) {
            timebuf[timelen] = '\0';
            off += snprintf(buf + off, len - off, "%s ", timebuf);
        } else {
            closedir(dir);
            return FALSE;
        }

        off += snprintf(buf + off, len - off, "%s\r\n", filename);

    }
    if(dir) {
        closedir(dir);
    }
    return off;
}

#endif

//(char *startPath,char *start,char *path,char *filename)

#define  LOG_DBG        printf

//copy dir form path to path
int copy_file(char *from,char *to)
{
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    char *fromPath = NULL;
    char *toPath  = NULL;
    char *filename = NULL;
    int  mode = 0;
    int  ret = 0;

    if(!from || !to)
    {
        return -1;
    }
    
    //检测该文件是否存在
    if(access(from,0) != 0)
    {
        //not access
        LOG_DBG("copy_file from:%s is not exist\r\n",from);
        return -1;
    }
    
    if(access(to,0) != 0)
    {
        if (stat(from, &st) < 0)
        {
            LOG_DBG("stat:%s Dir:%s", strerror(errno), from);
            return -1;
        }
        //不存在时创建
        getMode(st,mode);
        umask(0);
        if((ret = mkdir(to,mode)) != 0)
        {
            LOG_DBG("mkdir:%s mode=%d err\r\n",to,mode);
            return -1;
        }
        chown(to,st.st_uid,st.st_gid);
        mode = 0;
    }

    fromPath  = (char*)malloc(sizeof(char)*1024);
    if(!fromPath)
    {
        return -1;
    }
    toPath  = (char*)malloc(sizeof(char)*1024);
    if(!toPath)
    {
        free(fromPath);
        fromPath = NULL;
        return -1;
    }

    if (((dir = opendir(from))) == NULL)
    {
        free(fromPath);
        fromPath = NULL;
        free(toPath);
        toPath = NULL;
        LOG_DBG("opendir is NULL\n");
        return -1;
    }
    while ((ent = readdir(dir)) != NULL)
    {
        filename = ent->d_name;
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0 )
        {
            continue;
        }
        
        sprintf(fromPath,"%s/%s",from,filename);
        if (stat(fromPath, &st) < 0)
        {
            LOG_DBG("stat:%s Dir:%s", strerror(errno), fromPath);
            continue;
        }
        sprintf(toPath,"%s/%s",to,filename);

        getMode(st,mode);
        if (S_ISDIR(st.st_mode))
        {
            //目录执行此分支
            mkdir(toPath,mode);
            if( (ret = chmod(toPath,mode))  != 0)
            {
                LOG_DBG("chmod:%s err\r\n",to);
                break;
            }
            ret = copy_file(fromPath,toPath);
            if(ret != 0)
            {
                LOG_DBG("copy_file form:%s to:%s err\r\n",fromPath,toPath);
                break;
            }
            
        }else
        {
            int from_fd,to_fd;
            int size = 0;
            char buf[1024];
            from_fd = open(fromPath,O_RDONLY,mode);
            if(from_fd < 0)
            {
                LOG_DBG("open file:%s err \r\n",fromPath);
                ret = -1;
                break;
            }
            umask(0);
            to_fd = open(toPath,O_CREAT | O_WRONLY,mode);
            if(to_fd < 0)
            {
                close(from_fd);
                from_fd = -1;
                ret = -1;
                LOG_DBG("open file:%s reason:%s err\r\n",toPath,strerror(errno));
                break;
            }

            //copy common file data
            while((size = read(from_fd,buf,sizeof(buf))) >0)
            {
               if( write(to_fd,buf,size) <= 0)
               {
                    ret = -1;
                    break;
               }
            }

           close(from_fd);
           from_fd = -1;
           close(to_fd);
           to_fd = -1;
           if(ret < 0)
           {
               break;
           }
        }
        chown(toPath,st.st_uid,st.st_gid);
    }

    if(dir)
    {
         closedir(dir);
         dir = NULL;
    }

    return ret;
}

//仅仅创建文件夹
int just_mkdir(char *from,char *to)
{
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    char *fromPath = NULL;
    char *toPath  = NULL;
    char *filename = NULL;
    int  mode = 0;
    int  ret = 0;

    if(!from || !to)
    {
        return -1;
    }
    
    //检测该文件是否存在
    if(access(from,0) != 0)
    {
        //not access
        LOG_DBG("copy_file from:%s is not exist\r\n",from);
        return -1;
    }
    
    if(access(to,0) != 0)//判断目标文件是否存在
    {
        if (stat(from, &st) < 0)
        {
            LOG_DBG("stat:%s Dir:%s", strerror(errno), from);
            return -1;
        }
        //不存在时创建
        getMode(st,mode);
        umask(0);
        if((ret = mkdir(to,mode)) != 0)
        {
            LOG_DBG("mkdir:%s mode=%d err\r\n",to,mode);
            return -1;
        }
        LOG_DBG("to =%s uid=%d pid=%d \r\n",to,st.st_uid,st.st_gid);
        chown(to,st.st_uid,st.st_gid);
        mode = 0;
    }

    return ret;
}



int main(int argc,char **argv) {
    search_dir_to_file("/home/wangkang/workdir/jscon_file/","root/","root/file","out.json");
    return 0;
}

