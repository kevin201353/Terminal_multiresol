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
FILE *g_response = NULL;

static char respose[1024] = {0};


CURL *g_curllong = NULL;
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

size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = 0;
	if (ptr != NULL)
	{
		memset(respose, 0, sizeof(respose));
		written = size*nmemb;
		strcpy(respose, ptr);
	}
    return written;
}


void set_share_handle(CURL *curl_handle)
{
	static CURLSH* share_handle = NULL;
	if (!share_handle)
	{
		share_handle = curl_share_init();
		curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
	}
	curl_easy_setopt(curl_handle, CURLOPT_SHARE, share_handle);
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 20);
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
    g_curl = NULL;
    g_curl = curl_easy_init();
    //set_share_handle(g_curl);
}

void Close_Session()
{
    if (g_curl != NULL)
    {
        curl_easy_cleanup(g_curl);
        g_curl = NULL;
    }
}

void init_long_session()
{
	g_curllong = NULL;
	g_curllong = curl_easy_init();
}

void close_long_session()
{
	if (NULL != g_curllong)
	{
		curl_easy_cleanup(g_curllong);
        g_curllong = NULL;
	}
}

int http_login_request(char *url, char *user, char* password)
{
	if (g_curllong == NULL)
	{
	    return -1;
	}
	Xml_Open();
	char szbuf[512] = {0};
	strcat(szbuf, user);
	strcat(szbuf, ":");
	strcat(szbuf, password);
	char * p = strstr(user, "admin@internal");
	struct curl_slist *headers = NULL;
	CURLcode res;
	LogInfo(" Debug: curl.c  Http_Request g_szUser and pass : %s.\n", szbuf);
	if (p == NULL)
	{
	    set_share_handle(g_curllong);
		headers = curl_slist_append(headers, "filter: true");
		headers = curl_slist_append(headers, "Prefer: persistent-auth");
		headers = curl_slist_append(headers, "Connection: Keep-alive");
		curl_easy_setopt(g_curllong, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		curl_easy_setopt(g_curllong, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(g_curllong, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(g_curllong, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(g_curllong, CURLOPT_URL, url);
		curl_easy_setopt(g_curllong, CURLOPT_USERPWD, szbuf);
		curl_easy_setopt(g_curllong, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(g_curllong, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(g_curllong,  CURLOPT_TIMEOUT, 3);  //add http timeout
		if (g_curllong != NULL)
		{
			res = curl_easy_perform(g_curllong);
		}
		if (headers != NULL)
		{
			curl_slist_free_all(headers);
		}
	}else{
		set_share_handle(g_curllong);
		headers = curl_slist_append(headers, "filter: false");
		headers = curl_slist_append(headers, "Connection: Keep-alive");
		headers = curl_slist_append(headers, "Prefer: persistent-auth");
		curl_easy_setopt(g_curllong, CURLOPT_HTTPHEADER, headers);
	    curl_easy_setopt(g_curllong, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		curl_easy_setopt(g_curllong, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(g_curllong, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(g_curllong, CURLOPT_URL, url);
		curl_easy_setopt(g_curllong, CURLOPT_USERPWD, szbuf);
		curl_easy_setopt(g_curllong, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(g_curllong, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(g_curllong,  CURLOPT_TIMEOUT, 3);
		if (g_curllong != NULL)
		{
			res = curl_easy_perform(g_curllong);
		}
		if (headers != NULL)
		{
			curl_slist_free_all(headers);
		}
	}
	if(res != CURLE_OK)
	{
		 LogInfo("http_login_request, curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		 Xml_Close();
		 return -1;
	}
	LogInfo("http_login_request success.\n" );
	Xml_Close();
	return 0;
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
	strcat(szbuf, ":");
	strcat(szbuf, password);
	char * p = strstr(user, "admin@internal");
	struct curl_slist *headers = NULL;
	CURLcode res;
	LogInfo(" Debug: curl.c  Http_Request g_szUser and pass : %s.\n", szbuf);
	if (p == NULL)
	{
	    //LogInfo("Debug:###########    00000 filter : true.\n");
	    set_share_handle(g_curl);
		headers = curl_slist_append(headers, "filter: true");
		headers = curl_slist_append(headers, "Prefer: persistent-auth");
		headers = curl_slist_append(headers, "Connection: close");
		curl_easy_setopt(g_curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(g_curl, CURLOPT_URL, url);
		curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
		curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 3);  //add http timeout
		// LogInfo("Debug:###########    00000 filter aaaaaa: true.\n");
		if (g_curl != NULL)
		{
			res = curl_easy_perform(g_curl);
		}
		if (headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		//LogInfo("Debug:###########    00000 filter bbbbbbb: true.\n");
	}else{
			 //LogInfo("Debug:###########    00000 111 filter : true.\n");
		set_share_handle(g_curl);
		headers = curl_slist_append(headers, "filter: false");
		headers = curl_slist_append(headers, "Connection: close");
		headers = curl_slist_append(headers, "Prefer: persistent-auth");
		curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);
	    curl_easy_setopt(g_curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(g_curl, CURLOPT_URL, url);
		curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
		curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 3);
		if (g_curl == NULL)
		{
			printf("http request g_curl == null.\n");
		}
		if (g_curl != NULL)
		{
			res = curl_easy_perform(g_curl);
		}
		if (headers != NULL)
		{
			curl_slist_free_all(headers);
		}
	}
	if(res != CURLE_OK)
	{
		 //fprintf(stderr, "curl_easy_perform() failed: %s\n",
		 //        curl_easy_strerror(res));
		 LogInfo("Http_Request, curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		 Close_Session();
		 Xml_Close();
		 return -1;
	}
	LogInfo("Http_Request success.\n" );
	Close_Session();
	Xml_Close();
	return 0;
}

int Http_Post(char *url, char *user, char* password, char *data)
{
    Start_Session();
    if (g_curl == NULL)
    {
        //printf("Http_Post g_curl == null ,return.\n");
        return -1;
    }
    if((g_ticket = fopen(FILE_OVIRT_TICKET_PATH,"w")) == NULL)
    {
        printf("http_post fopen ticket xml file failed.\n");
        Close_Session();
        return -1;
    }
    char szbuf[512] = {0};
    strcat(szbuf, user);
    //strcat(szbuf, "@internal");
    strcat(szbuf, ":");
    strcat(szbuf, password);
    struct curl_slist *headers = NULL;
	char * p = strstr(user, "admin@internal");
	LogInfo(" Debug: curl.c  Http_Post g_szUser and pass : %s.\n", szbuf);
	if (p == NULL)
	{
		headers = curl_slist_append(headers, "filter: true");
	}else
	{
		headers = curl_slist_append(headers, "filter: false");
	}
	headers = curl_slist_append(headers, "Prefer: persistent-auth");
	headers = curl_slist_append(headers, "Connection: close");
    headers = curl_slist_append(headers, "Content-Type: application/xml");
	set_share_handle(g_curl);
    curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);// 改协议头
    //curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt"); // 指定cookie文件
    curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_ticket);
    curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
    curl_easy_setopt(g_curl, CURLOPT_URL, url);
    curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(g_curl, CURLOPT_POSTFIELDSIZE, strlen(data));
    curl_easy_setopt(g_curl, CURLOPT_COPYPOSTFIELDS, data);
	curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 3); //add http timeout
    CURLcode res;
	if (g_curl != NULL)
	{
		res = curl_easy_perform(g_curl);
	}
    /* Check for errors */
    if(res != CURLE_OK)
    {
	   LogInfo("Http_Post, curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
       curl_slist_free_all(headers);
       Close_Session();
       fclose(g_ticket);
       return -1;
   }
   if (headers != NULL)
   {
   	   curl_slist_free_all(headers);
   }
   if (g_ticket != NULL)
   {
     fclose(g_ticket);
     g_ticket = NULL;
   }
   Close_Session();
   return 0;
}

int Http_Request3(char *url, char *path)
{
	if((g_fptmp = fopen(path,"w")) == NULL)
	{
	   printf("http_request3 fopen ovirt info file failed.\n");
	   return -1;
	}
	Start_Session();
	if (g_curl == NULL)
	{
		printf("http request3 g_curl == null.\n");
	}
	LogInfo(" Debug: curl.c  Http_Request3 url : %s.\n", url);
	curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
	curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_dataTmp);
	curl_easy_setopt(g_curl, CURLOPT_URL, url);
	curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 3);	
	CURLcode res;
	if (g_curl != NULL)
		res = curl_easy_perform(g_curl);
	if(res != CURLE_OK)
	{
		 //fprintf(stderr, "curl_easy_perform() failed: %s\n",
		 //        curl_easy_strerror(res));
		 LogInfo("Http_Request3, curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		 Close_Session();
		 fclose(g_fptmp);
		 return -1;
	}
	//printf("http request3 success, url = %s.\n", url);
	if (g_fptmp != NULL)
	{
		 fclose(g_fptmp);
		 g_fptmp = NULL;
	}
	Close_Session();
}

int Http_Request2(char *url, char *user, char* password, char *path)
{
    if((g_fptmp = fopen(path,"w")) == NULL)
    {
        printf("http_request2 fopen ovirt info file failed.\n");
        return -1;
    }
    Start_Session();
    if (g_curl == NULL)
    {
        printf("Http_request2 g_curl == null ,return.\n");
		fclose(g_fptmp);
        return -1;
    }
    char szbuf[512] = {0};
	CURLcode res;
    strcat(szbuf, user);
    //strcat(szbuf, "@internal");
    strcat(szbuf, ":");
    strcat(szbuf, password);
	char * p = strstr(user, "admin@internal");
	struct curl_slist *headers = NULL;
	LogInfo(" Debug: curl.c  Http_Request2 g_szUser and pass : %s.\n", szbuf);
    if (p == NULL)
	{
		headers = curl_slist_append(headers, "filter: true");
		headers = curl_slist_append(headers, "Prefer: persistent-auth");
		headers = curl_slist_append(headers, "Connection: close");
		curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_dataTmp);
		curl_easy_setopt(g_curl, CURLOPT_URL, url);
		curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
		curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 3);
		if (g_curl != NULL)
		{
			res = curl_easy_perform(g_curl);
		}
		if (headers != NULL)
		{
			curl_slist_free_all(headers);
		}
    }
	else
	{
		headers = curl_slist_append(headers, "Connection: close");
		headers = curl_slist_append(headers, "filter: false");
		headers = curl_slist_append(headers, "Prefer: persistent-auth");
		curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);
    	curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
        curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_dataTmp);
        curl_easy_setopt(g_curl, CURLOPT_URL, url);
        curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
        curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
	    curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 3);
		if (g_curl == NULL)
		{
		    printf("http request g_curl == null.\n");
		}
		if (g_curl != NULL)
		{
			res = curl_easy_perform(g_curl);
		}
		if (headers != NULL)
		{
			curl_slist_free_all(headers);
		}
   }
   /* Check for errors */
   if(res != CURLE_OK)
   {
     //fprintf(stderr, "curl_easy_perform() failed: %s\n",
     //        curl_easy_strerror(res));
     LogInfo("Http_Request2, curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
     Close_Session();
	 fclose(g_fptmp);
     return -1;
   }
   Close_Session();
   if (g_fptmp != NULL)
   {
     fclose(g_fptmp);
     g_fptmp = NULL;
   }
   return 0;
}



int Http_Post2(char *url, char *user, char* password, char* data)
{
	Start_Session();
	if (g_curl == NULL)
	{
		LogInfo("Http_Post g_curl == null ,return.\n");
		return -1;
	}
	if((g_response = fopen(FILE_OVIRT_RESPONSE_PATH,"w")) == NULL)
	{
		LogInfo("http_post fopen ticket xml file failed.\n");
		Close_Session();
		return -1;
	}
	//add
	char szurl[512] = {0};
	strcpy(szurl, url);
	strcat(szurl, "?");
	strcat(szurl, data);
	//add end
	char szbuf[512] = {0};
	strcat(szbuf, user);
	strcat(szbuf, ":");
	strcat(szbuf, password);
	struct curl_slist *headers = NULL;
	char * p = strstr(user, "admin@internal");
	LogInfo(" Debug: curl.c  Http_Post g_szUser and pass : %s.\n", szbuf);
	if (p == NULL)
	{
		headers = curl_slist_append(headers, "filter: true");
	}else
	{
		headers = curl_slist_append(headers, "filter: false");
	}
	headers = curl_slist_append(headers, "Prefer: persistent-auth");
	headers = curl_slist_append(headers, "Connection: close");
	headers = curl_slist_append(headers, "Content-Type: application/xml");
	curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(g_curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
	curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(g_curl, CURLOPT_URL, szurl);
//	curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
//	curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
//	curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
//	curl_easy_setopt(g_curl, CURLOPT_POSTFIELDSIZE, strlen(data));
//	curl_easy_setopt(g_curl, CURLOPT_COPYPOSTFIELDS, data);
	curl_easy_setopt(g_curl,  CURLOPT_TIMEOUT, 5); //add http timeout
	CURLcode res;
	if (g_curl != NULL)
	{
		res = curl_easy_perform(g_curl);
	}
	if(res != CURLE_OK)
	{
		//fprintf(stderr, "curl_easy_perform() failed: %s\n",
		//     curl_easy_strerror(res));
		LogInfo("Http_Post2, curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_slist_free_all(headers);
		Close_Session();
		fclose(g_response);
		return -1;
	}
	char sz_debug[512] = {0};
	sprintf(sz_debug, "Http_Post2  url : %s, szbuf :%s, data :%s", url, szbuf, data);
	LogInfo(sz_debug);
	if (headers != NULL)
	{
		curl_slist_free_all(headers);
	}
	if (g_response != NULL)
	{
		fclose(g_response);
		g_response = NULL;
	}
	Close_Session();
	return 0;
}



int http_get(char *url, char* data, char *ret)
{
	CURL *urll;
	CURLcode retrn;
	urll = curl_easy_init();
	if (urll == NULL)
		return;
	/*
	if((g_response = fopen(FILE_OVIRT_RESPONSE_PATH,"w")) == NULL)
	{
		LogInfo("http_post fopen ticket xml file failed.\n");
		curl_easy_cleanup(urll);
		return -1;
	}*/
	char sz_debug[10240] = {0};
	char szurl[512] = {0};
	strcpy(szurl, url);
	strcat(szurl, "?");
	strcat(szurl, data);
	LogInfo("Debug: curl.c http_get data = %s.\n", szurl);
	curl_easy_setopt(urll, CURLOPT_URL, szurl);
	curl_easy_setopt(urll, CURLOPT_WRITEFUNCTION, write_response);
	retrn = curl_easy_perform(urll);
	if (retrn == CURLE_OK)
	{
		//char *pbuff = NULL;
		//retrn = curl_easy_getinfo(urll, CURLINFO_CONTENT_TYPE, &pbuff);
		//if (retrn == CURLE_OK && pbuff)
		{
			sprintf(sz_debug, "http_get curl_easy_perform success, get respone data: %s.\n", respose);
			LogInfo(sz_debug);
			strcpy(ret, respose);
		}
	}else {
		curl_easy_cleanup(urll);
		return -1;
	}
	curl_easy_cleanup(urll);
	return 0;
}
