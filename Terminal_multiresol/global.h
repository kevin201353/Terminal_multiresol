/***********
//filename: global
//author: zhaosenhua
//version: 1.0
//funciton list:LogInfo
//description: 自定义函数，供其它函数调用
//Date: 2016/6/23
**************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "c_list.h"
#include <gtk/gtk.h>
#include <pthread.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h> /* the key value defines can be found here */


#define MAX_RESOL_SIZE  			3
#define MAX_DATA_SIZE               1024
#define MAX_BUFF_SIZE               100
#define ERROR_START_VALUE           -3000
#define FILE_OVIRT_INFO_PATH        "ovirtInfo.xml"
#define FILE_OVIRT_TICKET_PATH      "ticket.xml"
#define FILE_OVIRT_INFOTMP_PATH     "ovirtInfoTmp.xml"

#define  SY_VM_COMMON_SPICE         "SPICE"
#define  SY_VM_COMMON_RDP           "RDP"
#define  STR_OVIRT_LOGIN            "api"
#define  STR_OVIRT_GET_VMS          "api/vms"
#define  STR_OVIRT_GET_CLUSTER      "api/clusters"

typedef int (*CallBackFun)(char *);
struct list_head head, *plist;
//static char ovirt_url[] = "https://192.168.0.220/ovirt-engine/";  //test
char ovirt_url[MAX_BUFF_SIZE];
static const gchar* g_home_css = "mygtk.css";

int  g_selectProto;   //0:sh   1:mirsoft  2: cit  3:vm
static pthread_mutex_t g_mutex1 = PTHREAD_MUTEX_INITIALIZER;
static gint g_screen_width = 1920;
static gint g_screen_height = 1080;
int  g_nVmCount;
char g_resol[MAX_BUFF_SIZE][MAX_BUFF_SIZE];
unsigned int g_resolCount;

GObject *g_mainWindow;
int   g_citExit;
static int g_auto_get_ns = 1;

enum Error {
  SY_USER_ISNULL = ERROR_START_VALUE + 1,
  SY_PASSWORD_ISNULL,
  SY_OVIRT_LOGIN_FAILED,
  SY_OVIRT_GETVMS_FAILED,
  SY_OVIRT_GETCLUSTERS_FAILED,
  SY_OVIRT_STARTVMS_FAILED,
  SY_OVIRT_SHUTDOWNVMS_FAILED,
  SY_OVIRT_SUSPENDVMS_FAILED,
  SY_OVIRT_GETVMSTICKET_FAILED
};

enum LOGIN_STATUS{
    LOGIN_STATUS_CONNECTING = 1001, //connecting, wait
    LOGIN_STATUS_FAILED, //login failed
    LOGIN_STATUS_GETVMS_FAILD,  //get vms list failed
    LOGIN_STATUS_GETVMS,  //connect successed , get vms
    LOGIN_STATUS_USER_PASS_ERROR,  //user and password is error
    LOGIN_STATUS_CONNECT_FAILED_AIR,  //the connection failed, make sure information
    LOGIN_STATUS_IP_ERROR, // ip error
    LOGIN_STATUS_IP_NONULL,  //ip not NULL
    LOGIN_STATUS_TYPE_ERROR, //ip type error
    LOGIN_STATUS_PORT_NONULL,  //port not NULL
    LOGIN_STATUS_USERNAME_NONULL,  //user name not NULL
    LOGIN_STATUS_PASS_NONULL,  //password not null
    LOGIN_STATUS_VM_DESKTOP, //connect vm desktop failed
    LOGIN_SUCCESSED
 };

int g_loginstatus;

struct Vms{
   char name[MAX_BUFF_SIZE];  //name
   char os[MAX_BUFF_SIZE];  //os name
   char tab[MAX_BUFF_SIZE]; //tab
   unsigned int status; //运行状态
   unsigned int vcpu;  //cpu 个数
   unsigned long memory; //内存  MB
   char ip[MAX_BUFF_SIZE]; //IP 地址
   unsigned int usb;   // sub策略
   char vmid[MAX_BUFF_SIZE];  //vm id
   int  port; //vm
};

struct Vms_Node {
    struct Vms  val;
    struct list_head list;
};
//
struct Vms  g_vmsComUpdate[MAX_BUFF_SIZE];
int  g_vmsComCount;

//登录用户信息
struct LoginInfo{
   char proto[MAX_BUFF_SIZE];
   char user[MAX_BUFF_SIZE];
   char pass[MAX_BUFF_SIZE];
   char ip[MAX_BUFF_SIZE];
   int  port;
   short protype;  //0:spice 1: rdp
   int  repass;
};

//服务器配置信息
struct ServerInfo{
   char szIP[MAX_BUFF_SIZE];
   unsigned int nport;
   char szUser[MAX_BUFF_SIZE];
   char szPass[MAX_BUFF_SIZE];
   unsigned short resol;
   int  manresol;
   char szResol[MAX_BUFF_SIZE];
};

//写日志
void LogInfo(const char* ms, ... );
void Init_Curlc();
void Unit_Curlc();
void Xml_Open();
void Xml_Close();

//调用libcurl 库发送 http 请求
int Http_Request(char *url, char *user, char *password);
int Http_Request2(char *url, char *user, char* password, char *path);
//登录Ovirt
int Ovirt_Login(char *url, char *user, char *password);
//获取Ovirt上的虚拟机列表
int Ovirt_GetVms(char *url, char *user, char *password);
//获取Ovirt 上的集群列表
int Ovirt_GetClusters(char *url, char *user, char *password);
//获取服务器虚拟机列表
int SY_GetVms();
//虚拟机列表资源释放
void SY_FreeVmsList();
//启动界面
void SY_topwindow_main();
//登录界面
void SY_loginwindow_main();
//虚拟机列表界面
void SY_vmlistwindow_main();
//配置界面（包括网络设置， 服务器地址设置界面）
int SY_Setting_main();

int Ovirt_StartVms(char *url, char *user, char *password, char *vm);
int Ovirt_ShutdownVms(char *url, char *user, char *password, char *vm);
int Ovirt_SuspendVms(char *url, char *user, char *password, char *vm);
int Ovirt_GetVm2(char *url, char *user, char* password, char *vm);

void Parsexml(char *element,  char *value, int ntype);
void SaveLogin(struct LoginInfo info);
int Http_Post(char *url, char *user, char* password, char *data);
void SY_GetVmsTicket(char * szTicket);

void Start_Session();
void Close_Session();
unsigned short SY_GetVmState(char* vmid);
//获取虚拟列表， 在虚拟机列表界面中
//启动，关闭，待机下使用
int SY_GetVms2();
//获取虚拟机列表，该函数被线程调用
//每个5S向服务器索取虚拟机状态
int Ovirt_GetVmsTmp(char *url, char *user, char* password);
void SaveServerInfo(struct ServerInfo info);
int SY_NetworkSet_main();
GObject * GetNetWorkSetLayout();
int GetServerInfo(struct ServerInfo info);
int GetServerInfo2(struct ServerInfo *pInfo);
int GetLoginInfo(struct LoginInfo *pInfo);
void SaveMirLogin(struct LoginInfo info);
void GetMirLoginInfo(struct LoginInfo *pInfo);
void SaveVMareLogin(struct LoginInfo info);
void GetVmareLoginInfo(struct LoginInfo *pInfo);
void SetLoginWindowFocus();
int GetResol();
void SY_logincit_main();
void SYMsgDialog(int nflag, char *msg);
int ShenCloud_login();
void SetSymsgContext(int msg);
int ShowMsgWindow();
void Checking_log();
void SYMsgDialog2(int nflag, char *msg);
void SYMsgDialogVm(int nflag, char *msg);
int Ovirt_GetVm3(char *url, char *vm);
int Http_Request3(char *url, char *path);
void SYNetInfoWin();

#endif //_GLOBAL_H
