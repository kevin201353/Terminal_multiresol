#include <stdio.h>
#include <string.h>
#include <pcre.h>

extern int check_ipv4_valid(char *s);
//return 0: invalid  1:valid
int check_ipv4_valid(char *s)
{
	if (s == NULL || strlen(s) == 0)
		return -1;
	pcre *code;
	int rc;
	int ovector[2*3];
	const char *errptr;
	const char *ip;
	int erroffet;
	char *p;
	char buf[20];

	code = pcre_compile("(\\d{1,3}\\.){3}\\d{1,3}", 0,
	        &errptr, &erroffet, NULL);
	if((rc = pcre_exec(code,NULL, s, strlen(s), 0, 0, ovector,2*3)) > 0)
	{
	    pcre_get_substring(s, ovector, rc, 0, &ip);
	    snprintf(buf,20,"%s",ip);
	    pcre_free_substring(ip);

	    for(p = strtok(buf,".");p != NULL;p = strtok(NULL,"."))
	    {
	        if(atoi(p) > 255)
	            goto end;
	    }

	    printf("Correct ip!\n");
	    return 1;
	}
	end:printf("Error ip!\n");
	pcre_free(code);
	return 0;
}

/*******************************************/
//test
/*int main(int argc,char **argv)
{
    //char *s="192.168.1.1";
    char sTmp[80] = {0};
	char *s = fgets(sTmp, 80, stdin);
    if (check_ipv4_valid(s))
		printf("main Correct ip!.\n");
	else
		printf("main Error ip!.\n");
    return 0;
}*/
/*******************************************/
