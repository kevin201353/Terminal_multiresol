#ifndef NETCONFIG_H
#define NETCONFIG_H

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
//#include <net/if.h>
#include <netinet/in.h>
#include <net/route.h>
#include <unistd.h>

#define DNS_CONFIGFILE   "/etc/resolvconf/resolv.conf.d/base"
#define NET_CONFIGFILE   "/etc/network/interfaces"
#define FILESIZE    1024
//#define ETH0            "enp2s0"
#define ETH0            "eth0"

struct  NetConfigS
{
    bool s_bDNS;     //true is DHCP
    bool s_bIP;
    char s_strIP[16];
    char s_strIPMask[16];
    char s_strGateWay[16];
    char s_strDNS[3][16];
};
class NetConfig
{
public:

    NetConfig();
    int ReadFile();
    int WriteFile();
    int GetMacAdd(char *mac, bool flag);
    int GetIPAddr(char *ip);
    ~NetConfig();
public:
    NetConfigS   *m_pNetConfig;
private:
    char *m_pDNSFileBuf;
    char *m_pNetFileBuf;
    FILE    *m_pDNSFile;
    FILE    *m_pNETFile;
private:

    int GetNetMask(char *ip);
    int GetGateWay(char *ip);
};

#endif // NETCONFIG_H
