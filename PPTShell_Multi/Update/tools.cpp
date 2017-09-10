#include "stdafx.h"

#include "tools.h"

#include <stdio.h>
#include "Tinyxml/tinyxml.h"
#include <WinInet.h>
#include <mmsystem.h>

#pragma comment ( lib, "Wininet.lib")

enum
{
	INTERNET_ERROR_OPENURL = 1,
	INTERNET_ERROR_FILEOPEN,
	INTERNET_ERROR_READFILE,
	INTERNET_ERROR_OPEN,
};

int __stdcall InternetDownloadFile(const char* URL, HANDLE hFileHandle);
UINT __stdcall InternetGetFile(HINTERNET IN hOpen, // Handle from InternetOpen
							   const char* szUrl,
							   HANDLE hFileHandle
							   );
/********************************************************************
功能：以二进制和Ascii码方式,记录日志文件
参数：
char * pszName [in]	记录到日志中的标题
unsigned char * pszStr [in]	记录的日志中的数据，分别已二进制和Ascii码方式记录
int len [in]	pszStr的长度
********************************************************************/
int BitDisp_log(char *pszFileName, unsigned char *pszStr, int len)
{
	int i,j,iNum = 20;
	FILE	*fp;
	SYSTEMTIME st;
	char szFileName[100];
	
	if(0 == len)
		return 0;
	
	GetLocalTime(&st);
	
	sprintf(szFileName,"%s%04d%02d%02d.log",pszFileName,st.wYear,st.wMonth,st.wDay);	
	if ((fp = fopen(szFileName,"a+"))==NULL)
		return(-1);
	
	fprintf(fp,"[%04d.%02d.%02d %02d:%02d:%02d.%03d] len=[0x%02x(=%02d)]\n",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,
		len,len);

	for(i = 0;i < len;i += iNum)
	{
		for(j = 0;j < iNum;j++)
		{
			if(10 == j)
				fprintf(fp,"  ");
			if(i + j < len)
				fprintf(fp," %02x",pszStr[i+j]);
			else
				fprintf(fp,"   ");
		}
		
		fprintf(fp,"	| ");
		
		for(j = 0;j < iNum;j++)
		{
			if(10 == j)
				fprintf(fp,"  ");
			if(i + j < len)
				fprintf(fp,"%c",pszStr[i+j]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	fflush(fp);
	fclose(fp);
	
	return 0;	
}

/*记录日志*/
int WriteLog(char *format, ...)
{
	SYSTEMTIME st;
	FILE *fp;
	char	buf[1024];
	char	szFileName[100];
	char	szBakFileName[100];
	va_list	arg;
	
	GetLocalTime(&st);
	
	va_start(arg,format);
	vsprintf(buf,format,arg);
	va_end(arg);
	
	ZeroMemory(szFileName, sizeof(szFileName));
	_snprintf(szFileName, sizeof(szFileName)-1, "temp\\increment.log");
	fp = fopen(szFileName,"a+");
	if(NULL == fp)
		return -1;

	fseek(fp, 0, SEEK_END);

	int nLocalFileSize = ftell(fp);
	fclose(fp);

// 	if (nLocalFileSize > 200 * 1024)
// 	{
// 		ZeroMemory(szBakFileName, sizeof(szBakFileName));
// 		_snprintf(szBakFileName, sizeof(szBakFileName)-1, "APClientInterface.log.1");		
// 
// 		DeleteFile(szBakFileName);
// 		MoveFile(szFileName, szBakFileName);
// 	}

	fp = fopen(szFileName,"a+");
	if(NULL == fp)
		return -1;

	fprintf(fp,"[%04d.%02d.%02d %02d:%02d:%02d.%03d] %s \n",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,
		buf);

	fflush(fp);
	fclose(fp);

	return 0;
}

/*
功能：将字符串转换成BCD码
int conv_len, 需要转换的字节长度
unsigned char type, 当转换的长度为“奇数”时， 0表示后补0，其他表示前补0

  例子：
  asc_to_bcd(bcd, (UCHAR*)"123", 3, 0); //结果 为 "0x12 0x30"
  asc_to_bcd(bcd, (UCHAR*)"123", 3, 1); //结果 为 "0x01 0x23"
*/
void asc_to_bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int conv_len, unsigned char type)
{
	int    cnt;
	char   ch, ch1;
	
	if (conv_len&0x01 && type ) 	/*conv_len为奇数且type不为0时*/
		ch1=0;
	else
		ch1=0x55;
	for (cnt=0; cnt<conv_len; ascii_buf++, cnt++) {
		if (*ascii_buf >= 'a' ) 
			ch = *ascii_buf-'a' + 10;
		else if ( *ascii_buf >= 'A' )
			ch =*ascii_buf- 'A' + 10;
		else if ( *ascii_buf >= '0' )
			ch =*ascii_buf-'0';
		else ch = 0;
		if (ch1==0x55)
			ch1=ch;
		else {
			*bcd_buf++=ch1<<4 | ch;
			ch1=0x55;
		}
	}
	if (ch1!=0x55) 
		*bcd_buf=ch1<<4;
}

bool CreateTempFile(char* szInfoFileName)
{
	DWORD dwRetVal = 0;

	LOG_TRACE("---CreateTempFile start---");

	dwRetVal = GetTempPath(MAX_PATH * 2, szInfoFileName);
	if (dwRetVal > MAX_PATH * 2 || (dwRetVal == 0))
	{
		LOG_ERROR("GetTempPath failed with error:%d", GetLastError());
		return false;
	}

	dwRetVal = GetTempFileName(szInfoFileName, "", 0, szInfoFileName);
	if (dwRetVal == 0)
	{
		LOG_ERROR("Create temp File failed with error:%d", GetLastError());
		return false;
	}

	LOG_TRACE("---CreateTempFile end---");
	return true;
}


bool GetFileVersionFromXml(string& strVersion, const char* szXmlName)
{
	LOG_TRACE("---GetFileVersionFromXml start---");
	LOG_TRACE("File:%s", szXmlName);

	if (!szXmlName)
	{
		LOG_ERROR("GetFileVersionFromXml: open xml file fail:%s", szXmlName);
		return false;
	}

	TiXmlDocument doc;
	bool result = doc.LoadFile(szXmlName);

	if (!result)
	{
		LOG_ERROR("GetFileVersionFromXml: open xml file fail:%s", szXmlName);
		return false;
	}

	TiXmlNode* node = doc.FirstChild("info");
	if (!node)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}
	TiXmlNode* node2 = node->FirstChild("mode");
	if (!node2)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}
	TiXmlElement* element = node2->ToElement();
	if (!element)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}
	string str = element->Attribute("patchurl");

	size_t nFirst = str.rfind("_");
	if (nFirst != string::npos)
	{
		size_t nEnd = str.find(".", nFirst);
		if (nEnd != string::npos)
		{
			strVersion = str.substr(nFirst+1, nEnd-nFirst-1);
			LOG_TRACE("---GetFileVersionFromXml end ---");
			return true;
		}
	}

	LOG_TRACE("Parse version error");
	LOG_TRACE("---GetFileVersionFromXml end ---");
	return false;
}

bool UrlRandDownload(const char* szUrl, string strCurVersion, string& strDownFileName)
{
	HANDLE hFile;

	int i = 0;

	LOG_TRACE("---UrlRandDownload start---");

	hFile = CreateFile(strDownFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		LOG_ERROR("CreateFile: %s fail with error code: %d", strDownFileName.c_str(), GetLastError());
		return false;
	}

	LOG_TRACE("download url:%s", szUrl);

	int nRes = InternetDownloadFile((char*)szUrl, hFile);
	if (0 == nRes)
	{
		LOG_TRACE("---UrlRandDownload end---");
		CloseHandle(hFile);
		return true;
	}

	LOG_ERROR("download url:%s fail.", szUrl);
	CloseHandle(hFile);

	LOG_TRACE("---UrlRandDownload end---");
	return false;
}

int __stdcall InternetDownloadFile(const char* URL, HANDLE hFileHandle)
{
	DWORD dwFlags = 0;
	DWORD dwResult = INTERNET_ERROR_OPEN;

	// 检查该设备是否可上网
	InternetGetConnectedState(&dwFlags, 0);

	if (dwFlags & INTERNET_CONNECTION_OFFLINE)
	{
		return INTERNET_ERROR_OPEN;
	}

	char szAgent[64];

	ZeroMemory(szAgent, sizeof(szAgent));
	_snprintf(szAgent, sizeof(szAgent)-1, "Agent%ld", GetTickCount());

	HINTERNET hOpen;

	if (!(dwFlags & INTERNET_CONNECTION_PROXY))
	{
		hOpen = InternetOpenA(szAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	}
	else
	{
		hOpen = InternetOpenA(szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	}

	if (hOpen)
	{
		dwResult = InternetGetFile(hOpen, URL, hFileHandle);
		InternetCloseHandle(hOpen);
	}
	else
	{
		return INTERNET_ERROR_OPEN;
	}

	return dwResult;
}

UINT __stdcall InternetGetFile(HINTERNET IN hOpen, // Handle from InternetOpen
							   const char* szUrl,
							   HANDLE hFileHandle
							   )
{
	DWORD dwSize = 0;
	char szHead[] = "Accept: */*\r\n\r\n";
	VOID* szTemp[16384];
	HINTERNET hConnect;

	ULONG ConnectTimeout = 5*1000;
	ULONG RevConnectTimeout = 5*1000;

	InternetSetOption(hOpen, INTERNET_OPTION_CONNECT_TIMEOUT, &ConnectTimeout, sizeof(ULONG));
	InternetSetOption(hOpen, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &RevConnectTimeout, sizeof(ULONG));


	if (!(hConnect = InternetOpenUrlA(hOpen, szUrl, szHead,
		lstrlenA(szHead), INTERNET_FLAG_TRANSFER_ASCII, 0)))
	{
		return INTERNET_ERROR_OPENURL;
	}

	TCHAR   szBuf[10]={0};	
	DWORD   dwLen   =  sizeof(szBuf);   
	DWORD dwBytes = 0;

	if (HttpQueryInfo(hConnect, HTTP_QUERY_STATUS_CODE, (LPVOID)szBuf, &dwLen, NULL))
	{
		int   dwRtn  = atoi(szBuf);
		if (dwRtn != HTTP_STATUS_OK)
		{	
			return INTERNET_ERROR_OPEN;	
		}
	}
// 
// 	if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
// 						(LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
// 	{
// 		dwByteToRead = 0;
// 	}

	DWORD start = 0;
	DWORD end = 0;
	DWORD time = 0;

	time = 10;
	start = GetTickCount();

	do 
	{
		// Keep coping in 16KB chunks, while file has any data left.
		// Note: bigger buffer will greatly improve performance.
		if (!InternetReadFile(hConnect, szTemp, 16384, &dwSize))
		{
			return INTERNET_ERROR_READFILE;
		}

		if (!dwSize)
		{
			break;
		}
		else
		{
			DWORD dwWrite;
			WriteFile(hFileHandle, szTemp, dwSize, &dwWrite, NULL);
		}

		dwBytes += dwSize;
	} while (true);

	return 0;
}
