/* ----------------------------------------------------------------------------
    Name:       ryanConfig.h
    Title:      config file library
    Lib:        Ryan Config library
	Platform:	Solaris,WIN

    Written:    2002/07/12  Ryan Chen

    Copyright:  Ryan Chen
    Revision:   1.0
 -----------------------------------------------------------------------------*/

#ifndef _RYAN_CONFIG_H_
#define _RYAN_CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ONELINE_LEN     1024

void TrimBlank(const char *sSrcStr, char *sDestStr);
char *GetLine(char *sBuf, char *sLine);
int ConfigStrcmp(const char *sString, const char *sCompString);

int WriteConfigString(const char *sFileName, const char *sSegment, const char *sItem, const char *sString);
int WriteConfigInt(const char *sFileName, const char *sSegment, const char *sItem, int nValue);
int WriteConfigUInt(const char *sFileName, const char *sSegment, const char *sItem, unsigned int nValue);
int GetConfigString(const char *sFileName, const char *sSegment, const char *sItem, const char *sDefault, char *sValue, int nLen);
int GetConfigInt(const char *sFileName, const char *sSegment, const char *sItem, int nDefault, int *nValue=NULL);
int GetConfigUInt(const char *sFileName, const char *sSegment, const char *sItem, unsigned int nDefault, unsigned int *nValue=NULL);

#endif //_RYAN_CONFIG_H_

