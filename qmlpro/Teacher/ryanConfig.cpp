/* ----------------------------------------------------------------------------
    Name:       ryanConfig.cpp
    Title:      config file library
    Lib:        Ryan Config library
	Platform:	Solaris,WIN

    Written:    2002/07/12  Ryan Chen

    Copyright:  Ryan Chen
    Revision:   1.0
 -----------------------------------------------------------------------------*/
#include "ryanConfig.h"

/* È¥³ý¼ä¸ô·û */
void TrimBlank(const unsigned char *sSrcStr, unsigned char *sDestStr)
{
    int     i,j;

    i = 0;
    j = 0;

    while(sSrcStr[i]==' ' || sSrcStr[i]=='\t')
        i++;

    while(sSrcStr[i]>0x20 && sSrcStr[i]!=0x7F && sSrcStr[i]!=0xFF 
            && sSrcStr[i]!='#' && sSrcStr[i]!=';')
        sDestStr[j++] = sSrcStr[i++];

    sDestStr[j] = 0;
}

/* ´Ó»º´æÖÐÈ¡µÃÒ»ÐÐ(²»°üÀ¨»»ÐÐ·û) */
/* ·µ»ØÏÂÒ»ÐÐµÄÖ¸Õë*/
char *GetLine(char *sBuf, char *sLine)
{
    int     i,j;

    if(sBuf==NULL)
    {
        sLine[0] = 0;
        return NULL;
    }

    i = 0;
    j = 0;

    while(sBuf[i]==' ' || sBuf[i]=='\t')
        i++;

    while( sBuf[i]!=0 && sBuf[i]!='\n' && sBuf[i]!='\r' && j<(ONELINE_LEN-1) )
        sLine[j++] = sBuf[i++];
    sLine[j] = 0;

    if(sBuf[i]=='\r')
    {
        while(sBuf[i]!='\n')
            i++;
    }else if(sBuf[i]==0)
        return NULL;

    return sBuf+i+1;
}

/* ·µ»Ø:0ÏàÍ¬ */
int ConfigStrcmp(const char *sString, const char *sCompString)
{
    int     nLen, n;
    char    ch1, ch2;

    nLen	= strlen(sString);
    if(nLen!=(int)strlen(sCompString))
        return(-1);

	for(n=0; n<nLen; n++)
	{
        ch1 = tolower(sString[n]);
        ch2 = tolower(sCompString[n]);
		if(ch1==ch2)
			continue;
        else if(ch1>ch2)
            return(1);
        else
            return(-1);
	}

    return(0);
}

// Ð´ÅäÖÃÎÄ¼þ×Ö·û´®Öµ
// ¶Î»òÌõÄ¿²»´æÔÚ»áÖ÷¶¯¼ÓÈë
// 0:³É¹¦
int WriteConfigString(const char *sFileName, const char *sSegment, 
                        const char *sItem, const char *sString)
{
    char        *p1,*p2,*pFind1;
    char        sTemp[256];
    struct stat sbuf;
    FILE        *pFile;
    int         nReturnCode, nFileLen;

    if(strlen(sFileName)==0 || strlen(sSegment)==0 || strlen(sItem)==0 || strlen(sString)==0)
        return(-1);

    /* È¡ÎÄ¼þ´óÐ¡ */
    memset(&sbuf, 0, sizeof(struct stat));
    if(stat(sFileName, &sbuf) != 0 )
    {
        /* ÎÄ¼þ²»´æÔÚ */
        if(errno!=ENOENT)
            return(-1);
    }

    nReturnCode = 0;
    nFileLen = sbuf.st_size;
    /* ·ÖÅä»º´æ */
    p1 = (char *)malloc(nFileLen+2);
    p2 = (char *)malloc(nFileLen+ONELINE_LEN);
    if(p1==NULL || p2==NULL)
    {
        nReturnCode = -1;
        goto WRITEEND;
    }
    memset(p1, 0, nFileLen+2);
    memset(p2, 0, nFileLen+ONELINE_LEN);

    pFile = fopen(sFileName, "rb");
    if(pFile == NULL)
    {
        /* ÎÄ¼þ²»´æÔÚ */
        if(errno!=ENOENT)
        {
            nReturnCode = -1;
            goto WRITEEND;
        }
        sprintf(p2,"[%s]\n%-20s= %s\n",sSegment, sItem, sString);
    }else
    {
        pFind1 = NULL;
        //¶ÁÎÄ¼þ,ÎÄ¼þ²»ÒÔ»»ÐÐ½áÊø,¼ÓÈë»»ÐÐ
        if(nFileLen>0)
        {
            fread(p1, nFileLen, 1, pFile);
            if(p1[nFileLen-1]!='\n')
                p1[nFileLen] = '\n';
            /* ËÑË÷¶Î */
            sprintf(sTemp, "[%s]", sSegment);
            pFind1 = strstr(p1, sTemp);
        }
        fclose(pFile);

        if(pFind1==NULL)
        {
            /* ¼ÓÔÚÎ²²¿ */
            sprintf(p2, "%s\n[%s]\n%-20s= %s\n", p1, sSegment, sItem, sString);
        }else
        {
            char sLine[ONELINE_LEN];
            char sComp[ONELINE_LEN];
            char *pFind2,*pFind3;
            int  i;

            //¶Î´æÔÚ,ÖÃpFind1Îª¶ÎÆðÊ¼µØÖ·
            pFind1 = GetLine(pFind1, sLine);
            if(pFind1==NULL)
            {
                /* ±íÃ÷×îºóÒ»ÐÐÎª¶ÎÍ·,¼ÓÔÚÎ²²¿ */
                sprintf(p2, "%s\n[%s]\n%-20s= %s\n", p1, sSegment, sItem, sString);
            }else
            {
                pFind3 = pFind1;
                sprintf(sTemp,"%s",sItem);
                while(1)
                {
                    pFind2 = pFind3;
                    pFind3 = GetLine(pFind2, sLine);

                    //µ½´ïÎÄ¼þÎ²
                    if(pFind3==NULL && strlen(sLine)==0)
                    {
                        pFind2 = NULL;
                        break;
                    }

                    //µ½´ïÏÂÒ»¸ö¶Î,·µ»Ø
                    if(sLine[0]=='[')
                    {
                        pFind2 = NULL;
                        break;
                    }

                    //ÎÞÐ§ÐÐ
                    if(strlen(sLine)==0 || sLine[0]=='#' || sLine[0]==';')
                        continue;

                    //ºË¶ÔÌõÄ¿
                    i = 0;
                    for(i=0;i<ONELINE_LEN;i++)
                    {
                        if(sLine[i] == '=')
                            sLine[i] = 0;
                        if(sLine[i] == 0)
                            break;
                    }
                    TrimBlank((const unsigned char*)sLine, (unsigned char*)sComp);
                    if(ConfigStrcmp(sItem, sComp)==0)
                    {
                        //Ìæ»»¸ÃÌõÄ¿
                        memcpy(p2, p1, pFind2-p1);
                        if(pFind3==NULL)
                            sprintf(p2, "%s%-20s= %s\n", p2, sItem, sString);
                        else
                            sprintf(p2, "%s%-20s= %s\n%s", p2, sItem, sString, pFind3);
                        break;
                    }
                }
                if(pFind2==NULL)
                {
                    //±¾¶ÎÃ»ÓÐ¸ÃÌõÄ¿,¼ÓÈëÔÚ¶ÎÆðÊ¼
                    memcpy(p2, p1, pFind1-p1);
                    sprintf(p2, "%s%-20s= %s\n%s", p2, sItem, sString, pFind1);
                }
            }

        }
    }

    //Ð´ÅäÖÃÎÄ¼þ
    pFile = fopen(sFileName, "wt");
    if(pFile == NULL)
    {
        nReturnCode = -1;
        goto WRITEEND;
    }
    fwrite(p2, strlen(p2), 1, pFile);
    fclose(pFile);

WRITEEND:
    if(p1)
        free(p1);
    if(p2)
        free(p2);

    return nReturnCode;
}

int WriteConfigInt(const char *sFileName, const char *sSegment, 
                    const char *sItem, int nValue)
{
    char sBuf[10];

    sprintf(sBuf, "%d", nValue);

    return WriteConfigString(sFileName, sSegment, sItem, sBuf);
}

int WriteConfigUInt(const char *sFileName, const char *sSegment, 
                    const char *sItem, unsigned int nValue)
{
    char sBuf[10];

    sprintf(sBuf, "%u", nValue);

    return WriteConfigString(sFileName, sSegment, sItem, sBuf);
}

int GetConfigString(const char *sFileName, const char *sSegment, 
                        const char *sItem, const char *sDefault,
					    char *sValue, int nLen)
{
    char        *p1,*pFind1;
    char        sTemp[256];
    struct stat sbuf;
    FILE        *pFile;
    int         nReturnCode, nFileLen;

    if( (int)strlen(sDefault) >= nLen)
    {
        memcpy(sValue, sDefault, nLen-1);
        sValue[nLen-1] = 0;
    }else
        strcpy(sValue, sDefault);

    if(strlen(sFileName)==0 || strlen(sSegment)==0 || strlen(sItem)==0)
        return(-1);

    /* È¡ÎÄ¼þ´óÐ¡ */
    if(stat(sFileName, &sbuf) != 0 )
        return(-1);
    if(sbuf.st_size<=0)
        return(-1);

    nReturnCode = 0;
    nFileLen = sbuf.st_size;

    /* ·ÖÅä»º´æ */
    p1 = (char *)malloc(nFileLen+1);
    if(p1==NULL)
        return(-1);

    memset(p1, 0, nFileLen+1);

    pFile = fopen(sFileName, "rb");
    if(pFile == NULL)
    {
        nReturnCode = -1;
        goto READEND;
    }

    fread(p1, nFileLen, 1, pFile);
    fclose(pFile);

    /* ËÑË÷¶Î */
    sprintf(sTemp, "[%s]", sSegment);
    pFind1 = strstr(p1, sTemp);

    if(pFind1==NULL)
    {
        nReturnCode = -1;
        goto READEND;
    }

    char sLine[ONELINE_LEN];
    char sComp[ONELINE_LEN];
    int  i;

    //¶Î´æÔÚ,ÖÃpFind1Îª¶ÎÆðÊ¼µØÖ·
    pFind1 = GetLine(pFind1, sLine);
    while(1)
    {
        if(pFind1==NULL)
        {
            nReturnCode = -1;
            break;
        }
        pFind1 = GetLine(pFind1, sLine);
        if(strlen(sLine)==0)
            continue;
        if(sLine[0]=='[')
        {
            nReturnCode = -1;
            break;
        }
        //ºË¶ÔÌõÄ¿
        i = 0;
        memset(sTemp, 0, 256);
        for(i=0;i<ONELINE_LEN;i++)
        {
            if(sLine[i] == '=')
            {
                memcpy(sComp, sLine, i);
                sComp[i] = 0;
                TrimBlank((const unsigned char*)sComp, (unsigned char*)sTemp);
                break;
            }
            if(sLine[i] == 0)
                break;
        }

        if(ConfigStrcmp(sItem, sTemp)==0)
        {
            strcpy(sComp, &sLine[i+1]);
            TrimBlank((const unsigned char*)sComp, (unsigned char*)sTemp);
            if( (int)strlen(sTemp) >= nLen)
            {
                memcpy(sValue, sTemp, nLen-1);
                sValue[nLen-1] = 0;
            }else
                strcpy(sValue, sTemp);
            break;
        }
    }

READEND:
    if(p1)
        free(p1);

    return nReturnCode;
}

int GetConfigInt(const char *sFileName, const char *sSegment, 
                        const char *sItem, int nDefault,
					    int *nValue)
{
    char sDefault[10];
    char sValue[20];
    int  nReturnCode = nDefault;

    sprintf(sDefault, "%d", nDefault);
    memset(sValue, 0, 20);

    nReturnCode = GetConfigString(sFileName, sSegment, sItem, sDefault, sValue, 20);
    nReturnCode = atoi(sValue);

    if(nValue!=NULL)
      *nValue = nReturnCode;

    return nReturnCode;
}

int GetConfigUInt(const char *sFileName, const char *sSegment, 
                        const char *sItem, unsigned int nDefault,
					    unsigned int *nValue)
{
    char sDefault[10];
    char sValue[20];
    unsigned int  nReturnCode=nDefault;

    sprintf(sDefault, "%u", nDefault);
    memset(sValue, 0, 20);

    nReturnCode = GetConfigString(sFileName, sSegment, sItem, sDefault, sValue, 20);
    if(sValue[0]=='0' && (sValue[1]=='X' || sValue[1]=='x'))
    {
      for(int n=0; sValue[n]!=0; n++)
          sValue[n] = toupper(sValue[n]);

      sscanf(sValue, "0X%X", &nReturnCode);
    }else
      sscanf(sValue, "%u", &nReturnCode);

    if(nValue!=NULL)
      *nValue = nReturnCode;

    return nReturnCode;
}

