/*************************************************************      
    FileName : config2.c  
    FileFunc : 定义实现文件     
    Version  : V0.1      
    Author   : Sunrier      
    Date     : 2016-11-03
    Descp    : Linux下获取配置文件信息     
*************************************************************/  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
//#include <unistd.h>
#define  MAX_DATA_SIZE   200
#define FILE_OVIRT_PATH  "/var/ovirt.conf"
#define FILE_TCM_PATH    "/var/upgrade.info"

#define STRING_BASEURL_FLAG  	"baseurl="
#define STRING_BASEURL_FLAG2   "baseurl = "

int set_ovirt_baseurl(char * szurl)
{
	FILE *fp;
	fp=fopen(FILE_OVIRT_PATH,"r+"); 
	if (fp == NULL)
		return 0;
	char szTmp[100][200] = {0};
	int nflag = 0;
	int new_len = strlen(szurl);
	int i = 0;
	while (fgets(szTmp[i], 200, fp) != NULL)
	{
		 int len_szTmp = strlen(szTmp[i]);
		 if (strstr(szTmp[i], "oVirt") != NULL)
		 {
			 nflag = 1;
			 i++;
			 continue;
		 }
		 if (nflag == 1 && strstr(szTmp[i], "baseurl") != NULL)
		 {
		    int len2 = 0;
		    if (strstr(szTmp[i], STRING_BASEURL_FLAG) != NULL)
				len2 = strlen(STRING_BASEURL_FLAG);
			if (strstr(szTmp[i], STRING_BASEURL_FLAG2) != NULL)
				len2 = strlen(STRING_BASEURL_FLAG2);
			char szxx[100] = {0};
			strcpy(szxx, szTmp[i]);
			char *p = strtok(szxx, "=");
			p = strtok(NULL, "=");
			int len = strlen(p);
			memcpy(szTmp[i] + len2, szurl, new_len);
			szTmp[i][len2 + new_len] = '\n';
			szTmp[i][len2 + new_len + 1] = '\0';
			nflag = 0;
		 }
		 i++;
	}
	fclose(fp);
	fp = NULL;
	fp = fopen(FILE_OVIRT_PATH,"w+");
	if (fp == NULL)
		return 0;
	int j = 0;
	for(j=0;j<i;j++)  
	{  
	   fwrite(szTmp[j],strlen(szTmp[j]),1,fp);  
	}  
    fclose(fp);  
	return 0;
}

int set_ovirt_tcm(char * tcmserver, char *teacher)
{
	FILE *fp;
	fp=fopen(FILE_TCM_PATH,"r+"); 
	if (fp == NULL)
		return -1;
	char szTmp[100][200] = {0};
	int nflag1 = 0;
	int nflag2 = 0;
	int i = 0;
	int tcm_len = 0;
	int teach_len = 0;
	while (fgets(szTmp[i], 200, fp) != NULL)
	{
		 if (strstr(szTmp[i], "tcm server") != NULL)
		 {
			 nflag1= 1;
			 i++;
			 continue;
		 }
		 if (strstr(szTmp[i], "teacher manager") != NULL)
		 {
			 nflag2= 1;
			 i++;
			 continue;
		 }
		 if (nflag1 == 1)
		 {
			 tcm_len = strlen(tcmserver);
			memcpy(szTmp[i], tcmserver, tcm_len);
			szTmp[i][tcm_len] = '\n';
			szTmp[i][tcm_len + 1] = '\0';
			nflag1 = 0;
		 }
		  if (nflag2 == 1)
		 {
			 teach_len = strlen(teacher);
			memcpy(szTmp[i], teacher, teach_len);
			szTmp[i][teach_len] = '\n';
			szTmp[i][teach_len + 1] = '\0';
			nflag2 = 0;
		 }
		 i++;
	}
	fclose(fp);
	fp = NULL;
	fp = fopen(FILE_TCM_PATH,"w+");
	if (fp == NULL)
		return -1;
	int j = 0;
	for(j=0;j<i;j++)  
	{  
	   fwrite(szTmp[j],strlen(szTmp[j]),1,fp);  
	}  
    fclose(fp);
	return 0;
}

int set_ovirt_conf(char * key, char *value)
{
	FILE *fp;
	fp=fopen(FILE_OVIRT_PATH,"r+"); 
	if (fp == NULL)
		return -1;
	char szTmp[100][200] = {0};
	char sznew[100] = {0};
	int i = 0;
	while (fgets(szTmp[i], 200, fp) != NULL)
	{
		 if (strstr(szTmp[i], key) != NULL)
		 {
			strcpy(sznew, key);
			strcat(sznew, " = ");
			strcat(sznew, value);
			strcat(sznew, "\r\n");
			memset(szTmp[i], 0, 200);
			strcpy(szTmp[i], sznew);
		 }
		 i++;
	}
	fclose(fp);
	fp = NULL;
	fp = fopen(FILE_OVIRT_PATH,"w+");
	if (fp == NULL)
		return -1;
	int j = 0;
	for(j=0;j<i;j++)  
	{  
	   fwrite(szTmp[j],strlen(szTmp[j]),1,fp);  
	}  
    fclose(fp);
	return 0;
}

int get_ovirt_conf(char *key, char *value)
{
	if (value == NULL || key == NULL)
		return -1;
	FILE *fp;
	fp=fopen(FILE_OVIRT_PATH,"r+"); 
	if (fp == NULL)
		return -1;
	char data[200] = {0};
	int i = 0;
	while (fgets(data, 200, fp) != NULL)
	{
		 if (strstr(data, key) != NULL)
		 {
			char* token = strtok(data, "=");
			if (token != NULL)
			{
				char * tmp = strtok(NULL, "");
				strcpy(value, tmp);
				if (tmp = strstr(value, "\n"))
					*tmp = '\0';
				if (tmp = strstr(value, "\r"))
					*tmp = '\0';
				break;
			}
		 }
		 i++;
	}
	fclose(fp);
	return 0;
}

//int main()
//{
//	char  sznew[100] = {0};
//	strcpy(sznew, "https://192.168.0.170:443/api");
//	set_ovirt_baseurl(sznew);
//	
//	memset(sznew, 0, 100);
//	strcpy(sznew, "http://192.168.0.170:8000");
//	char teacher[100] = {0};
//	strcpy(teacher, "http://192.168.0.170:8090");
//	set_ovirt_tcm(sznew, teacher);

//	set_ovirt_conf("user_name", "zhao122");
//	set_ovirt_conf("user_dpt_name", "zhao111");
//	char szTmp[100] = {0};
//	get_ovirt_conf("user_name", szTmp);
//	printf("get value : %s .\n", szTmp);
//	memset(szTmp, 0, 100);
//	get_ovirt_conf("user_dpt_name", szTmp);
//	printf("get value : %s .\n", szTmp);
//	return 0;
//}
