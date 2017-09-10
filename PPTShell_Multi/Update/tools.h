#ifndef __TOOLS_H_
#define __TOOLS_H_

#include <stdio.h>
#include <string>
using namespace std;

#define LOG_TRACE WriteLog
#define LOG_ERROR WriteLog
#define LOG_INFO WriteLog



int BitDisp_log(char *pszFileName, unsigned char *pszStr, int len);
int WriteLog(char *format, ...);
void asc_to_bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int conv_len, unsigned char type);


bool CreateTempFile(char* szInfoFileName);

bool GetFileVersionFromXml(string& strVersion, const char* szXmlName);
bool UrlRandDownload(const char* szUrl, string strCurVersion, string& strDownFileName);

#endif