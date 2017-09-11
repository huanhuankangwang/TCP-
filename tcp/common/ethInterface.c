#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>


#define   DBG_ERR  printf

int getAddr(const char *ifname, int flag, char *ipBuf, int bufLen)
{
    int                  fd;
    struct ifreq         ifr;
    struct sockaddr_in *sa = NULL;

    if(NULL == ifname || NULL == ipBuf || 16 > bufLen)
    {
        DBG_ERR("VPSUtil::getIfaceAddr got invalid parm. 0x%x 0x%x %d", (
unsigned int)ifname, (unsigned int)ipBuf, bufLen);
        return -1;
    }

    *ipBuf = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(fd, flag, &ifr) < 0)
    {
        DBG_ERR("ioctl error:%s iface[%s]", strerror(errno), ifr.ifr_name);
        close(fd);
        return -1;
    }

    sa = (struct sockaddr_in *)(&ifr.ifr_addr);
    inet_ntop(AF_INET, &sa->sin_addr, ipBuf, bufLen);
    close(fd);
    return 0;
}

//获取网口的ip
int getIfaceAddr(const char *ifname, char *ipBuf, int bufLen)
{
    return getAddr(ifname, SIOCGIFADDR, ipBuf, bufLen);
}

//获取网卡的mask
int getIfaceNetMask(const char *ifname, char *maskBuf, int bufLen)
{
    return getAddr(ifname, SIOCGIFNETMASK, maskBuf, bufLen);
}

