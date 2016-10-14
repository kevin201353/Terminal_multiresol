/***********
//filename: curlc
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 自定义函数，负责与ovirt-engine ovirt 服务端通信的低层库，供其它函数调用
//如：登录 ovirt 服务端，获取 ovirt 参数信息
//Date: 2016/6/24
**************/
#include <stdio.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "global.h"

FILE *g_fp = NULL;  //定义FILE类型指针
CURL *g_curl = NULL;

FILE *g_ticket = NULL;
FILE *g_fptmp = NULL;

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的
//完成数据保存功能
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = 0;
    if (g_fp)
    {
        written = fwrite(ptr, size, nmemb, (FILE *)g_fp);
    }
    return written;
}

size_t write_ticket(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = 0;
    if (g_ticket)
    {
        written = fwrite(ptr, size, nmemb, (FILE *)g_ticket);
    }
    return written;
}

size_t write_dataTmp(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = 0;
    if (g_fptmp)
    {
        written = fwrite(ptr, size, nmemb, (FILE *)g_fptmp);
    }
    return written;
}

void Init_Curlc()
{
    //在使用该库时， 必须首先调用该函数进行初始化，
    //程序结束时，使用 'uinit_curlc()' 进行释放
    curl_global_init(CURL_GLOBAL_ALL);
}

void Start_Session()
{
    //在使用该库时，应先获取一个 CURL handler， 在后面的库函数中，都要输入该 handler
    //作为参数来调用。否则， 会失败或出错。
    g_curl = curl_easy_init();
}

void Close_Session()
{
    if (g_curl != NULL)
    {
        curl_easy_cleanup(g_curl);
        g_curl = NULL;
    }
}

void Unit_Curlc()
{
  /* always cleanup */
   curl_global_cleanup();
}

void Xml_Open()
{
    if((g_fp = fopen(FILE_OVIRT_INFO_PATH,"w")) == NULL)
    {
        printf("http_request fopen ovirt info file failed.\n");
        return;
    }
}

void Xml_Close()
{
    if (g_fp != NULL)
    {
      fclose(g_fp);
      g_fp = NULL;
    }
}

int Http_Request(char *url, char *user, char* password)
{
    Start_Session();
    if (g_curl == NULL)
    {
        printf("Http_request g_curl == null ,return.\n");
        return -1;
    }
    Xml_Open();
    char szbuf[512] = {0};
    strcat(szbuf, user);
    //strcat(szbuf, "@internal");
    strcat(szbuf, ":");
    strcat(szbuf, password);
    char * p = strstr(user, "internal");
    struct curl_slist *headers = NULL;
    CURLcode res;
    if (p == NULL)
    {
        headers = curl_slist_append(headers, "filter: true");
	    headers = curl_slist_append(headers, "Prefer: persistent-auth"); //add by kevin 2016/9/30
		headers = curl_slist_append(headers, "Connection: close"); //add by kevin 2016/9/30
        curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(g_curl, CURLOPT_URL, url);
        curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
        curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
	    curl_easy_setopt(g_curl, CURLOPT_EXPECT_100_TIMEOUT_MS, 10000);  //add http timeout
        res = curl_easy_perform(g_curl);
        curl_slist_free_all(headers);
    }else{
        curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(g_curl, CURLOPT_URL, url);
        curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
        curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(szbuf));
        //curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, szbuf);
        //printf("Http_request url :%s.\n", url);
        //printf("Http_request user and password :%s.\n", szbuf);

        if (g_curl == NULL)
        {
            printf("http request g_curl == null.\n");
        }
        res = curl_easy_perform(g_curl);
   }
   if(res != CURLE_OK)
   {
     fprintf(stderr, "curl_easy_perform() failed: %s\n",
             curl_easy_strerror(res));
     Close_Session();
     return -1;
   }
   printf("http request success.\n" );
   Close_Session();
   Xml_Close();
    return 0;
}

int Http_Post(char *url, char *user, char* password, char *data)
{
    Start_Session();
    if (g_curl == NULL)
    {
        printf("Http_Post g_curl == null ,return.\n");
        return -1;
    }
    pthread_mutex_lock( &g_mutex1 );
    if((g_ticket = fopen(FILE_OVIRT_TICKET_PATH,"w")) == NULL)
    {
        pthread_mutex_unlock( &g_mutex1 );
        printf("http_post fopen ticket xml file failed.\n");
        Close_Session();
        return -1;
    }
    pthread_mutex_unlock( &g_mutex1 );
    char szbuf[512] = {0};
    strcat(szbuf, user);
    //strcat(szbuf, "@internal");
    strcat(szbuf, ":");
    strcat(szbuf, password);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/xml");
    curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);// 改协议头
    curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt"); // 指定cookie文件
    curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_ticket);
    curl_easy_setopt(g_curl, CURLOPT_URL, url);
    curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(g_curl, CURLOPT_POSTFIELDSIZE, strlen(data));
    curl_easy_setopt(g_curl, CURLOPT_COPYPOSTFIELDS, data);
	curl_easy_setopt(g_curl, CURLOPT_EXPECT_100_TIMEOUT_MS, 10000);  //add http timeout
    //printf("Http_Post url :%s.\n", szbuf);
    //printf("Http_Post user and password :%s.\n", szbuf);

    CURLcode res = curl_easy_perform(g_curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
       fprintf(stderr, "curl_easy_perform() failed: %s\n",
             curl_easy_strerror(res));
       curl_slist_free_all(headers);
       Close_Session();
       fclose(g_ticket);
       return -1;
   }
   curl_slist_free_all(headers);
   if (g_ticket != NULL)
   {
     fclose(g_ticket);
     g_ticket = NULL;
   }
   Close_Session();
   return 0;
}


int Http_Request2(char *url, char *user, char* password, char *path)
{
    pthread_mutex_lock( &g_mutex1 );
    if((g_fptmp = fopen(path,"w")) == NULL)
    {
        pthread_mutex_unlock( &g_mutex1 );
        printf("http_request2 fopen ovirt info file failed.\n");
        return -1;
    }
    pthread_mutex_unlock( &g_mutex1 );
    Start_Session();
    if (g_curl == NULL)
    {
        printf("Http_request2 g_curl == null ,return.\n");
        return -1;
    }
    char szbuf[512] = {0};
    strcat(szbuf, user);
    //strcat(szbuf, "@internal");
    strcat(szbuf, ":");
    strcat(szbuf, password);
    curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_dataTmp);
    curl_easy_setopt(g_curl, CURLOPT_URL, url);
    curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
	 curl_easy_setopt(g_curl, CURLOPT_EXPECT_100_TIMEOUT_MS, 10000);  //add http timeout
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(szbuf));
    //curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, szbuf);
    //printf("Http_request2 url :%s.\n", url);
    //printf("Http_request2 user and password :%s.\n", szbuf);

    if (g_curl == NULL)
    {
        printf("http request2 g_curl == null.\n");
    }
    CURLcode res = curl_easy_perform(g_curl);
   /* Check for errors */
   if(res != CURLE_OK)
   {
     fprintf(stderr, "curl_easy_perform() failed: %s\n",
             curl_easy_strerror(res));
     Close_Session();
     return -1;
   }
   printf("http request2 success.\n" );
   Close_Session();
   if (g_fptmp != NULL)
   {
     fclose(g_fptmp);
     g_fptmp = NULL;
   }
   return 0;
}
