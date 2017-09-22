#include "netconfig.h"
#include <net/if.h>
#include "type.h"

NetConfig::NetConfig()
{
    m_pNetConfig = (struct NetConfigS*)malloc(sizeof(struct NetConfigS));
    memset(m_pNetConfig,0,sizeof(struct NetConfigS));
    m_pDNSFileBuf = (char *)malloc(FILESIZE);
    memset(m_pDNSFileBuf,0,FILESIZE);
    m_pNetFileBuf = (char *)malloc(FILESIZE);
    memset(m_pNetFileBuf,0,FILESIZE);
}

NetConfig::~NetConfig()
{
    if(m_pNetConfig)
        free(m_pNetConfig);
    if(m_pDNSFileBuf)
        free(m_pDNSFileBuf);
    if(m_pNetFileBuf)
        free(m_pNetFileBuf);
}

int NetConfig::ReadFile()
{
    char *pTemp;
    char TempBuf[100];
    memset(m_pNetConfig,0,sizeof(struct NetConfigS));
    memset(m_pDNSFileBuf,0,FILESIZE);
    memset(m_pNetFileBuf,0,FILESIZE);

    m_pDNSFile = fopen(DNS_CONFIGFILE,"r");
    if(m_pDNSFile == NULL)
    {
        printf("Open File Error[%s]\n",DNS_CONFIGFILE);
        return -1;
    }
    fread(m_pDNSFileBuf,FILESIZE,1,m_pDNSFile);
#ifdef _DEBUG_PRINT
    printf("=====\n%s\n",m_pDNSFileBuf);
#endif

    m_pNETFile = fopen(NET_CONFIGFILE,"r");
    if(m_pNETFile == NULL)
    {
        printf("Open File Error[%s]\n",NET_CONFIGFILE);
        return -1;
    }
    fread(m_pNetFileBuf,FILESIZE,1,m_pNETFile);
#ifdef _DEBUG_PRINT
    printf("=====\n%s\n",m_pNetFileBuf);
#endif

    fclose(m_pDNSFile);
    fclose(m_pNETFile);
    pTemp = strstr(m_pDNSFileBuf,"nameserver");
    if(pTemp == NULL)
    {
        m_pNetConfig->s_bDNS = true;
    }
    else
    {
        m_pNetConfig->s_bDNS = false;
        pTemp += 10;
        while(*pTemp == ' ')
        {
            pTemp++;
        }
        sscanf(pTemp,"%s",m_pNetConfig->s_strDNS[0]);
#ifdef _DEBUG_PRINT
        printf("=====\n%s\n",m_pNetConfig->s_strDNS[0]);
#endif
        pTemp = strstr(pTemp,"nameserver");
        if(pTemp != NULL)
        {
            pTemp += 10;
            while(*pTemp == ' ')
            {
                pTemp++;
            }
            sscanf(pTemp,"%s",m_pNetConfig->s_strDNS[1]);
#ifdef _DEBUG_PRINT
            printf("=====\n%s\n",m_pNetConfig->s_strDNS[1]);
#endif
            pTemp = strstr(pTemp,"nameserver");
            if(pTemp != NULL)
            {
                pTemp += 10;
                while(*pTemp == ' ')
                {
                    pTemp++;
                }
                sscanf(pTemp,"%s",m_pNetConfig->s_strDNS[2]);
#ifdef _DEBUG_PRINT
                printf("=====\n%s\n",m_pNetConfig->s_strDNS[2]);
#endif
            }
        }
    }

    pTemp = strstr(m_pNetFileBuf,ETH0);
    pTemp += strlen(ETH0);
    if(pTemp != NULL)
        pTemp = strstr(pTemp,"inet");
    while(*pTemp == ' ')
        pTemp++;
    memset(TempBuf,0,100);
    pTemp += 4;
    sscanf(pTemp,"%s",TempBuf);

#ifdef _DEBUG_PRINT
    printf("====\n%s\n",TempBuf);
#endif

    if(strcmp(TempBuf,"dhcp") == 0)
    {
        m_pNetConfig->s_bIP = true;
        GetIPAddr(m_pNetConfig->s_strIP);
        GetNetMask(m_pNetConfig->s_strIPMask);
        GetGateWay(m_pNetConfig->s_strGateWay);
    }
    if(strcmp(TempBuf,"static") == 0)
    {
        m_pNetConfig->s_bIP = false;
        pTemp = strstr(pTemp,"address");
        if(pTemp != NULL)
            pTemp += 7;
        while(*pTemp == ' ')
            pTemp++;
        sscanf(pTemp,"%s",m_pNetConfig->s_strIP);
#ifdef _DEBUG_PRINT
        printf("=====IP[%s]\n",m_pNetConfig->s_strIP);
#endif
        pTemp = strstr(pTemp,"netmask");
        if(pTemp != NULL)
            pTemp += 7;
        while(*pTemp == ' ')
            pTemp++;
        sscanf(pTemp,"%s",m_pNetConfig->s_strIPMask);
#ifdef _DEBUG_PRINT
        printf("=====Mask[%s]\n",m_pNetConfig->s_strIPMask);
#endif
        pTemp = strstr(pTemp,"gateway");
        if(pTemp != NULL)
            pTemp += 7;
        while(*pTemp == ' ')
            pTemp++;
        sscanf(pTemp,"%s",m_pNetConfig->s_strGateWay);
#ifdef _DEBUG_PRINT
        printf("=====GateWay[%s]\n",m_pNetConfig->s_strGateWay);
#endif
    }
}
int NetConfig::WriteFile()
{
    char TempBuf[100];
    m_pDNSFile = fopen(DNS_CONFIGFILE,"w");
    if(m_pDNSFile == NULL)
    {
        printf("Fopen Error!\n");
        return -1;
    }
    if(m_pNetConfig->s_bDNS)
    {

    }
    else
    {
        for(int i = 0;i < 3;i++)
        {
            if(!strlen(m_pNetConfig->s_strDNS[i]))
                continue;
            memset(TempBuf,0,100);
            sprintf(TempBuf,"nameserver %s\n",m_pNetConfig->s_strDNS[i]);
            fwrite(TempBuf,strlen(TempBuf),1,m_pDNSFile);
        }
    }
    fclose(m_pDNSFile);

    m_pNETFile = fopen(NET_CONFIGFILE,"w");
    if(m_pNETFile == NULL)
    {
        printf("Fopen Error\n");
        return -2;
    }
    if(m_pNetConfig->s_bIP)
    {
        memset(TempBuf,0,100);
        sprintf(TempBuf,"auto %s\niface %s inet dhcp",ETH0,ETH0);
        fwrite(TempBuf,strlen(TempBuf),1,m_pNETFile);
    }
    else
    {
        memset(TempBuf,0,100);
        sprintf(TempBuf,"auto %s\niface %s inet static\n",ETH0,ETH0);
        fwrite(TempBuf,strlen(TempBuf),1,m_pNETFile);

        memset(TempBuf,0,100);
        sprintf(TempBuf,"address %s\n",m_pNetConfig->s_strIP);
        fwrite(TempBuf,strlen(TempBuf),1,m_pNETFile);

        memset(TempBuf,0,100);
        sprintf(TempBuf,"netmask %s\n",m_pNetConfig->s_strIPMask);
        fwrite(TempBuf,strlen(TempBuf),1,m_pNETFile);

        memset(TempBuf,0,100);
        sprintf(TempBuf,"gateway %s\n",m_pNetConfig->s_strGateWay);
        fwrite(TempBuf,strlen(TempBuf),1,m_pNETFile);
    }
    fclose(m_pNETFile);
}
int NetConfig::GetIPAddr(char *ip)
{
    struct ifreq ifr;
    int socketfd = socket(AF_INET,SOCK_STREAM,0);
    if(socketfd < 0)
    {
        printf("Socket Error\n");
        return -1;
    }
    strcpy(ifr.ifr_name,ETH0);
    if(ioctl(socketfd,SIOCGIFADDR,&ifr) < 0)
    {
        printf("ioctl Error!\n");
        close(socketfd);
        return -2;
    }
    struct sockaddr_in sock_in;
    memset(&sock_in, 0, sizeof(sock_in));
    memcpy(&sock_in, &ifr.ifr_addr, sizeof(sock_in));
    strcpy(ip,inet_ntoa(sock_in.sin_addr));
    close(socketfd);
    return 0;

}
int NetConfig::GetNetMask(char *ip)
{
    struct ifreq ifr;
    int socketfd = socket(AF_INET,SOCK_STREAM,0);
    if(socketfd < 0)
    {
        printf("Socket Error\n");
        return -1;
    }
    strcpy(ifr.ifr_name,ETH0);
    if(ioctl(socketfd,SIOCGIFNETMASK,&ifr) < 0)
    {
        printf("ioctl Error!\n");
        close(socketfd);
        return -2;
    }
    struct sockaddr_in sock_in;
    memset(&sock_in, 0, sizeof(sock_in));
    memcpy(&sock_in, &ifr.ifr_addr, sizeof(sock_in));
    inet_ntop(AF_INET, &sock_in.sin_addr.s_addr, ip, 16);
    close(socketfd);
    return 0;
}
int NetConfig::GetGateWay(char *ip)
{
    FILE *fp = fopen("/proc/net/route", "r");
    if(NULL == fp)
    {
        printf("%s : open file failed!\n", __FUNCTION__);
        perror("fopen");
        return -1;
    }

    int ret;
    char iface[16];
    unsigned long dest_addr;
    unsigned long gate_way;
    char buf[256] = "";
    fgets(buf, sizeof(buf), fp);
    while(fgets(buf, sizeof(buf), fp))
    {
        ret = sscanf(buf, "%s\t%lX\t%lX", iface, &dest_addr, &gate_way);
        if(3 == ret && 0 == strcmp(iface, ETH0))
            break;
    }
    inet_ntop(AF_INET, &gate_way, ip, 16);
    fclose(fp);
    return 0;
}

int NetConfig::GetMacAdd(char *mac,bool flag)
{
    struct ifreq ifr;
    int sock;
    if((sock = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("socket");
        return -1;
    }
    strcpy(ifr.ifr_name,ETH0);
    if(ioctl(sock,SIOCGIFHWADDR,&ifr) < 0)
    {
        perror("ioctl");
        return -2;
    }
    if(flag)
    {
        sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[0],
                                                (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                                                (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                                                (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                                                (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                                                (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
    }
    else
    {
        sprintf(mac,"%02x%02x%02x%02x%02x%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[0],
                                                    (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                                                    (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                                                    (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                                                    (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                                                    (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

    }
}
