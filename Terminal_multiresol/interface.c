#include "interface.h"

param s;

/***************************************
 * flag = 1;Open Log
 * flag = 0;Close Log
 * ************************************/
void SetLogFlag(INT32 flag)
{
    if(!flag)
    {
        s.iLogFlag = 0;
    }
    else
    {
        s.iLogFlag =1;
        if(InitLog() < 0)
        {
            printf("Log no use!Error[%d]\n",errno);
        }
    }
}
INT32 InitLog(void)
{
    if(!s.iLogFlag)
    {
        return 0;
    }
    if(s.fd > 2)
    {
        close(s.fd);
    }
    if(access("Interface.log",F_OK) < 0)
    {
        s.fd = open("Interface.log",O_CREAT|O_APPEND|O_RDWR,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    }
    else
    {
        s.fd = open("Interface.log",O_APPEND|O_RDWR);
    }
    if(s.fd < 0)
    {
        perror("Open LogFile Error!");
        s.iLogFlag = 0;
        return -1;
    }
    return 0;
}
INT32 CloseLog(void)
{
    if(!s.iLogFlag)
    {
        return 0;
    }
    if(s.fd > 0)
    {
        close(s.fd);
        s.fd = 0;
    }
    return 0;
}
INT32 WriteLog(const INT8* sFormat, ... )
{
    if(!s.iLogFlag)
    {
        return 0;
    }
    va_list arg;
    struct tm *pTm;
    time_t Time;
    Time = time(NULL);
    pTm = localtime(&Time);
    int iLen = 0;
    INT8 LogBuf[LOGSIZE_MAX] = {0};

    sprintf(LogBuf,"[%04d-%02d-%02d %02d:%02d:%02d] ",1900+pTm->tm_year, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    if(write(s.fd,LogBuf,22) < 0)
    {
        perror("Write Log Time Error");
    }
    printf("%s",LogBuf);

    memset(LogBuf,0,LOGSIZE_MAX);
    va_start(arg,sFormat);
    vsnprintf(LogBuf,LOGSIZE_MAX-2,sFormat,arg);
    va_end(arg);
    iLen = strlen(LogBuf);
    if(iLen < LOGSIZE_MAX-1)
    {
        LogBuf[iLen] = '\n';
    }
    LogBuf[iLen+1] = 0;
    if(write(s.fd,LogBuf,iLen+1) < 0)
    {
        perror("Write Log Error");
    }
    printf("%s",LogBuf);
}
/***************************************************
 * exe_path             Exe Path
 * Mode          1      RDP Mode
 * Mode          0      Vmware Mode
 * Mode			2       Citrix Mode
 * ************************************************/
INT32 GetExeName(const INT8 *exe_path,INT32 Mode)
{
    INT8 *p = exe_path;
    INT8 *dp = NULL;
    if(strlen(p) == 0)
    {
        WriteLog("Not Find EXE.");
        return -1;
    }
    dp = strrchr(p,'/');
    if(dp)
    {
        switch(Mode)
        {
        case 0:
        {
            strcpy(s.sVMware.strExeName,dp+1);
        }break;
        case 1:
        {
            strcpy(s.sRDP.strExeName,dp+1);
        }break;
        case 2:
        {
            strcpy(s.sCitrix.strExeName,dp+1);
        }break;
        default:
            break;
        }
    }
    else
    {
        switch(Mode)
        {
        case 0:
        {
            strcpy(s.sVMware.strExeName,p+1);
        }break;
        case 1:
        {
            strcpy(s.sRDP.strExeName,p+1);
        }break;
        case 2:
        {
            strcpy(s.sCitrix.strExeName,p+1);
        }break;
        default:
            break;
        }
    }
    return 0;
}
/*********************************************
 * Name:user name
 * Passwod:user passwd
 * IP:server ip
 * Return           -1 ----- args Error
 *                  -2 ----- IP Error
 *                  -3 ----- No Path
 *                  -4 ----- Fork Error
 * *******************************************/
INT32 Run_FreeRDP(INT8 *name,INT8 *passwd,INT8 *IP)
{
    INT32 iRet = -1;
    INT32 iStatus = -1;
    INT8  strArg[1024];
    INT8 strPio1[10];
    INT8 ReadBuf[100];
    INT32 Pio[2];
    BOOL  bReadFlag = FALSE;
    WriteLog("Login With FreeRDP------------------------");
    WriteLog("Name[%s] Passwd[%s] IP[%s]",name,passwd,IP);
    if(name==NULL||passwd==NULL||IP==NULL)
    {
        WriteLog("Error:Please Check Name,Passwd,IP");
        return -1;
    }
    //Test IP is OK
    if(TestIP(IP,"3389",1000) < 0)
    {
        WriteLog("------------Check IP------------");
        return -2;
    }
    //Information Right Start Run
    strncpy(s.sRDP.strUsrName,name,100);
    strncpy(s.sRDP.strPasswd,passwd,64);
    strncpy(s.sRDP.strIP,IP,16);
    if(GetExeName(RDPEXE_PATH,1) < 0)
    {
        WriteLog("Unknow ExeName[%s]",s.sRDP.strExeName);
        return -3;
    }
    //Create Pipe
    if(pipe(Pio) == -1)
    {
        WriteLog("Create pipe Error[%d]",errno);
        return -4;
    }
    sprintf(strPio1,"%d",Pio[1]);

    s.sRDP.Argv[0] = s.sRDP.strExeName;
    s.sRDP.Argv[1] = "-u";
    s.sRDP.Argv[2] = s.sRDP.strUsrName;
    s.sRDP.Argv[3] = "-p";
    s.sRDP.Argv[4] = s.sRDP.strPasswd;
    s.sRDP.Argv[5] = "-v";
    s.sRDP.Argv[6] = s.sRDP.strIP;
    s.sRDP.Argv[7] = "-f";
    s.sRDP.Argv[8] = "-usb";
    s.sRDP.Argv[9] = "auto";
    s.sRDP.Argv[10] = "-sound";
    s.sRDP.Argv[11] = "-rfx";
    s.sRDP.Argv[12] = "-t";
    s.sRDP.Argv[13] = "ShenCloud";
//    s.sRDP.Argv[14] = NULL;
    s.sRDP.Argv[14] = "-pipe";
    s.sRDP.Argv[15] = strPio1;
    s.sRDP.Argv[16] = NULL;
    memset(strArg,0,1024);
    for(int i = 0;s.sRDP.Argv[i] != NULL;i++)
    {
        strcat(strArg,s.sRDP.Argv[i]);
        strcat(strArg," ");
    }
    WriteLog("Command:%s",strArg);
    pid_t pid;

    //Fork
    pid = fork();
    if(pid < 0)
    {
        WriteLog("Error:Fork Error!");
        return -5;
    }

    if(pid == 0)
    {
        iRet = execv(RDPEXE_PATH,s.sRDP.Argv);
        if(iRet < 0)
        {
            WriteLog("execv ret:%d error:%d error:%s",iRet,errno,strerror(errno));
        }
        exit(0);
    }
    if(pid > 0)
    {
        //Read pipe[0]
        struct timeval tval;
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(Pio[0],&rfds);
        tval.tv_sec = 10;
        tval.tv_usec = 0;
        INT32 iReturn = -1;
        iReturn = select(Pio[0]+1,&rfds,NULL,NULL,&tval);
        if(iReturn == 0)
        {
            WriteLog("Select Pipe[%d] TimeOut UseTime:%d s",Pio[0],10);
            //bReadFlag = TRUE;
        }
        if(iReturn < 0)
        {
            WriteLog("Select Pipe Error[%d]",errno);
           bReadFlag = TRUE;
        }
        if(iReturn > 0)
        {
            if(FD_ISSET(Pio[0],&rfds))
            {
                memset(ReadBuf,0,100);
                if(read(Pio[0],ReadBuf,100) < 0)
                {
                    WriteLog("Read PIPE Buf Error!");
                    bReadFlag = TRUE;
                }
            }
        }
        pid = wait(&iStatus);
        WriteLog("Wait Pid[%u] return Status[%d].",pid,iStatus);
    }
    WriteLog("-------------------------------------------");
    if(bReadFlag)
        return -6;
    if(strcmp(ReadBuf,"disconnect") == 0)
    {
        WriteLog("Connect Error Check Information");
        return -7;
    }
    return 0;
}
INT32 Run_VmwareView(INT8 *name,INT8 *passwd,INT8 *IP,INT8 *desktop)
{
    INT32 iStatus;
    INT32 iRet;
    WriteLog("Login With VMwareView..................");
    WriteLog("Name[%s] Passwd[%s] IP[%s] DeskTop[%s]",name,passwd,IP,desktop);
    if(name == NULL||passwd == NULL||IP == NULL||desktop == NULL)
    {
        WriteLog("Error:Please Check Name,Passwd,IP,DeskTop");
        return -1;
    }

    strncpy(s.sVMware.strUsrName,name,100);
    strncpy(s.sVMware.strPasswd,passwd,64);
    strncpy(s.sVMware.strIP,IP,16);
    if(GetExeName(VMWAREEXE_PATH,0) < 0)
    {
        WriteLog("Unknow ExeName[%s]",s.sVMware.strExeName);
        return -2;
    }

    s.sVMware.Argv[0] = s.sVMware.strExeName;
    s.sVMware.Argv[1] = NULL;
    //    s.sVMware.Argv[1] = "-u";
    s.sVMware.Argv[2] = s.sVMware.strUsrName;
    s.sVMware.Argv[3] = "-p";
    s.sVMware.Argv[4] = s.sVMware.strPasswd;
    s.sVMware.Argv[5] = "-s";
    s.sVMware.Argv[6] = s.sVMware.strIP;
    s.sVMware.Argv[7] = "-n";
    s.sVMware.Argv[8] = s.sVMware.strDeskTop;
    s.sVMware.Argv[9] = "protocol=BLAST";
    s.sVMware.Argv[10] = "fullscreen";
    s.sVMware.Argv[11] = "-q";
    s.sVMware.Argv[12] = NULL;

    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        WriteLog("Error:Fork Error!");
        return -3;
    }
    if(pid == 0)
    {
        iRet = execv(VMWAREEXE_PATH,s.sVMware.Argv);
        if(iRet < 0)
        {
            WriteLog("execv ret:%d error:%d error:%s",iRet,errno,strerror(errno));
        }
        exit(0);
    }
    if(pid > 0)
    {
        pid = wait(&iStatus);
        WriteLog("Wait Pid[%u] return Status[%d].",pid,iStatus);
    }
    WriteLog("-------------------------------------------");
}
INT32 Run_Citrix(INT8 *IP)
{
    INT32 iStatus;
    INT32 iRet;
    WriteLog("Login With Citrix..................");
    WriteLog("IP[%s]",IP);
    if(IP == NULL)
    {
        WriteLog("Error:Please IP");
        return -1;
    }
    strncpy(s.sCitrix.strDomain,IP,100);
    if(GetExeName(FIREFOX_PATH,2) < 0)
    {
        WriteLog("Unknow ExeName[%s]",s.sCitrix.strExeName);
        return -2;
    }
    s.sCitrix.Argv[0] = s.sCitrix.strExeName;
    s.sCitrix.Argv[1] = "-URL";
    s.sCitrix.Argv[2] = s.sCitrix.strDomain;
    s.sCitrix.Argv[3] = NULL;


    pid_t pid = 0;
 //   pid = fork();
    if(pid < 0)
    {
        WriteLog("Error:Fork Error!");
        return -3;
    }
    if(pid == 0)
    {
        iRet = execv(FIREFOX_PATH,s.sCitrix.Argv);
        if(iRet < 0)
        {
            WriteLog("execv ret:%d error:%d error:%s",iRet,errno,strerror(errno));
        }
        exit(0);
    }
    if(pid > 0)
    {
        pid = wait(&iStatus);
        WriteLog("Wait Pid[%u] return Status[%d].",pid,iStatus);
    }
    WriteLog("-------------------------------------------");
}

INT32 TestIP(INT8 *IP,INT8 *Port,UINT32 TimeOut)
{
    struct addrinfo *addr;
    struct addrinfo *result;
    struct addrinfo hints;
    INT32 istatus = -1;
    INT32 iSocket = -1;
    INT32 iFcnl = -1;
    memset(&hints,0,sizeof(hints));
    istatus = getaddrinfo(IP,Port,&hints,&result);
    if(istatus)
    {
        WriteLog("Getaddrinfo:%s",gai_strerror(istatus));
        return -1;
    }
    addr = result;
    if((addr->ai_family == AF_INET) && (addr->ai_next != NULL))
    {
        while((addr = addr->ai_next))
        {
            if(addr->ai_family == AF_INET)
                break;
        }
        if(addr == NULL)
            addr = result;
    }
    iSocket = socket(addr->ai_family,addr->ai_socktype,addr->ai_protocol);
    if(iSocket < 0)
    {
        freeaddrinfo(result);
        WriteLog("Socket Error!");
        return -2;
    }
//setfcntl
    iFcnl = fcntl(iSocket,F_GETFL);
    if(iFcnl < 0)
    {
        WriteLog("Fcntl Error[%d]",iFcnl);
        return -3;
    }
    fcntl(iSocket,F_SETFL,O_NONBLOCK|iFcnl);
//Connect .........
    istatus = connect(iSocket,addr->ai_addr,addr->ai_addrlen);
    if(istatus == 0)
    {
        WriteLog("Connect IP[%s] Port[%s] Success!",IP,Port);
        freeaddrinfo(result);
        close(iSocket);
        return 0;
    }
    istatus = Select(iSocket,TimeOut);
    if(istatus == 0)
    {
        WriteLog("Connect IP[%s] Port[%s] Success!",IP,Port);
        freeaddrinfo(result);
        close(iSocket);
        return 0;
    }
    if(istatus < 0)
    {
        freeaddrinfo(result);
        WriteLog("Connect IP[%s] Port[%s] Error[%d] Errno[%d]",IP,Port,istatus,errno);
        return -4;
    }
    return 0;
}
/*****************************************
 * fd       -------- INT
 * timeOut -------- Ms
*****************************************/
INT32 Select(INT32 fd,UINT32 timeOut)
{
    struct timeval tval;
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(fd,&wfds);
    tval.tv_sec = timeOut/1000;
    tval.tv_usec = (timeOut%1000)*1000;
    INT32 iReturn = -1;
    iReturn = select(fd+1,NULL,&wfds,NULL,&tval);
    if(iReturn == 0)
    {
        WriteLog("Select Socket[%d] TimeOut UseTime:%d Ms",fd,timeOut);
        return -1;
    }
    if(iReturn < 0)
    {
        WriteLog("Select Error[%d]",errno);
       return -2;
    }
    if(iReturn > 0)
    {
        if(FD_ISSET(fd,&wfds))
        {
            INT32 error;
            socklen_t len = sizeof(error);
            if(getsockopt(fd,SOL_SOCKET,SO_ERROR,&error,&len) < 0)
            {
                WriteLog("GetsocketOpt Error");
                return -3;
            }
            if(error == 0)
            {
                WriteLog("Connect OK!");
                return 0;
            }
            else
                return -4;
        }
    }
    return 0;
}
//INT32 TestConnect_tls(INT8 *IP,INT8 *User,INT8 *Passwd)
//{
//    rdpTls *tls = NULL;
//    BIO *frontBio;
//    tls->strIP = IP;
//    tls->iPort = Passwd;

//}
//INT32 tls_connect(rdpTls tls,BIO *underlying)
//{

//}
