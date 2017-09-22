#include<stdio.h>  
#include<openssl/md5.h>  
#include<string.h>

#define WRITE_TMPFILE(file, x) ({ FILE* fp = fopen(file, "w"); \
     if (fp != NULL){ \
	 fwrite(x, 1, strlen(x), fp); \
	 fclose(fp); }\
	 })

int read_tmp_data(char *file, char *data)
{
	if (data == NULL)
		return -1;
	
	FILE *fp;
	fp = fopen(file,"r+"); 
	if (fp == NULL)
	{
		printf("open file failed.\n");
		return -1;
	}
	char szTmp[100] = {0};
	while (fgets(szTmp, 100 , fp) != NULL)
	{
		sleep(1);
	}
	strcpy(data, szTmp);
	fclose(fp);
	return 0;
}


void sy_md5_encryption(char* pdata, char* pencry_data)
{
	if (pdata == NULL || pencry_data == NULL)
		return;
	unsigned char md[16] = {0};
	char tmp[3]={'/0'},buf[33]={'/0'};
	MD5(pdata, strlen(pdata), md);
	int i = 0;
	for (i = 0; i < 16; i++)
	{  
        sprintf(tmp,"%2.2x",md[i]);  
        strcat(buf,tmp);  
    }
	printf("%s\n",buf);
	strcpy(pencry_data, buf);
}
  
//int main( int argc, char **argv )  
//{  
//	unsigned char *data = "12356";
//	char encry_data[100] = {0};
//	sy_md5_encryption(data, encry_data);
//	printf("ency context : %s \n", encry_data);
//	WRITE_TMPFILE("tmp", encry_data);
//	//test
//	char encry_data1[100] = {0};
//	sy_md5_encryption(data, encry_data1);

//	char tmp_encry_data[100] = {0};
//	read_tmp_data("tmp", tmp_encry_data);
//	//test
//	printf("read file : %s.\n", tmp_encry_data);
//	if (strcmp(tmp_encry_data, encry_data1) == 0)
//	{
//		printf("password equal .\n");
//	}
//	return 0;
//} 

