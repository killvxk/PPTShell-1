//===========================================================================
// FileName:				Util.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "Util.h"
#include "tlhelp32.h" 
#include "NDCloud/ChapterTree.h"
#include "NDCloud/NDCloudFile.h"
#include "openssl/evp.h"
#include "openssl/bio.h"
#include "openssl/buffer.h"
#include <algorithm>
#include <sys/timeb.h> 
#include "Util/Base64.h"
#include <CdErr.h>
#include <math.h>
#include "time.h"
#include <openssl/md5.h>
#include <functional>

#pragma comment(lib,"Iphlpapi.lib")

void SleepRunNull(int iMilliSeconds)
{
	clock_t start, finish;
	static float fRate = (1.0f/CLOCKS_PER_SEC)*1000;
	
	start = clock();
	while(true)
	{
		finish = clock();
		if ((finish-start)*fRate>=iMilliSeconds)
		{
			break;
		}
	}

}

tstring& replace_all_distinct(tstring&   str,const   tstring&   old_value,const   tstring&   new_value)     
{     
	for(tstring::size_type   pos(0);   pos!=tstring::npos;   pos+=new_value.length())   {     
		if(   (pos=str.find(old_value,pos))!=tstring::npos   )     
			str.replace(pos,old_value.length(),new_value);     
		else   break;     
	}     
	return   str;     
}

string Utf8ToAnsi(string utf)
{
	// utf8 --> unicode
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, utf.c_str(), utf.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	MultiByteToWideChar(CP_UTF8, NULL, utf.c_str(), utf.length(), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	// unicode --> ansi
	int ansiLen = WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
	char* szAnsi = new char[ansiLen + 1];

	WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	string str = szAnsi;
	delete szAnsi;
	delete wszString;

	return str;
}

wstring Utf8ToUnicode(string utf)
{
	// utf8 --> unicode
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, utf.c_str(), utf.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	MultiByteToWideChar(CP_UTF8, NULL, utf.c_str(), utf.length(), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	wstring str = wszString;
	delete wszString;

	return str;
}

string AnsiToUtf8(string ansi)
{
	// ansi --> unicode
	int wcsLen = MultiByteToWideChar(CP_ACP, NULL, ansi.c_str(), ansi.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	MultiByteToWideChar(CP_ACP, NULL, ansi.c_str(), ansi.length(), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	// unicode --> utf8
	int utfLen = WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
	char* szString = new char[utfLen+1];

	WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szString, utfLen, NULL, NULL);
	szString[utfLen] = '\0';

	string str = szString;
	delete szString;
	delete wszString;

	return str;
}

string		UnicodeToUtf8(wstring unicode)
{
	int utfLen = WideCharToMultiByte(CP_UTF8, NULL, unicode.c_str(), wcslen(unicode.c_str()), NULL, 0, NULL, NULL);
	char* szString = new char[utfLen+1];

	WideCharToMultiByte(CP_UTF8, NULL, unicode.c_str(), wcslen(unicode.c_str()), szString, utfLen, NULL, NULL);
	szString[utfLen] = '\0';

	string str = szString;
	delete szString;

	return str;
}

wstring AnsiToUnicode(string ansi)
{
	// ansi --> unicode
	int wcsLen = MultiByteToWideChar(CP_ACP, NULL, ansi.c_str(), ansi.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	MultiByteToWideChar(CP_ACP, NULL, ansi.c_str(), ansi.length(), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	wstring str = wszString;
	delete wszString;

	return str;
}

string UnicodeToAnsi( wstring unicode )
{
	// unicode --> ansi
	int ansiLen = WideCharToMultiByte(CP_ACP, NULL, unicode.c_str(), unicode.length(), NULL, 0, NULL, NULL);
	char* szAnsi = new char[ansiLen + 1];

	WideCharToMultiByte(CP_ACP, NULL, unicode.c_str(), unicode.length(), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	string str = szAnsi;
	delete szAnsi;

	return str;
}


tstring		Ansi2Str(const string& str)
{
#ifdef _UNICODE	
	return AnsiToUnicode(str);
#else
	return str;
#endif
}

tstring		Un2Str(const wstring& str)
{
#ifdef _UNICODE	
	return str;
#else
	return UnicodeToAnsi(str);
#endif
}

tstring		Utf82Str(const string& str)
{
#ifdef _UNICODE	
	return Utf8ToUnicode(str);
#else
	return Utf8ToAnsi(str);
#endif
}

wstring		Str2Unicode(const tstring& str)
{
#ifdef _UNICODE	
	return str;
#else
	return AnsiToUnicode(str);
#endif
}

string Str2Ansi(const tstring& str)
{
#ifdef _UNICODE	
	return UnicodeToAnsi(str);
#else
	return str;
#endif
}

string Str2Utf8( const tstring& str )
{
#ifdef _UNICODE	
	return UnicodeToUtf8(str);
#else
	return AnsiToUtf8(str);
#endif
}

tstring GetHtmlPlayerPath()
{
	return  GetLocalPath()+_T("\\Package\\nodejs\\app\\player\\index.html");
}
tstring GetUnzipExePath()
{
	return GetLocalPath()+_T("\\bin\\CoursePlayer\\")+UNZIP_EXE_NAME;
}
tstring GetCoursePlayerExePath()
{
	return GetLocalPath()+_T("\\bin\\CoursePlayer\\CoursePlayer.exe");
}
tstring GetCoursePlayerPath()
{
	return GetLocalPath()+_T("\\bin\\CoursePlayer");
}
tstring GetAndCefX64Path()
{
	return GetLocalPath()+_T("\\bin\\AndCefX64");
}
tstring GetLocalPath()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	TCHAR* p = _tcsrchr(szPath, '\\');
	*p = '\0';

	return szPath;
}

tstring GetSkinsDir()
{
	return GetLocalPath()+_T("\\skins\\");
}

tstring GetModulePath()
{
	TCHAR s_buf[MAX_PATH];
	s_buf[0] = 0x0;
	DWORD n_result = ::GetModuleFileName(NULL, s_buf, sizeof(TCHAR) * MAX_PATH);
	TCHAR	s_drive[MAX_PATH];
	s_drive[0] = 0x0;
	TCHAR s_dir[MAX_PATH];
	s_dir[0] = 0x0;
	/*errno_t n_err_no =*/ 
	_tsplitpath(s_buf, s_drive, s_dir, NULL, NULL);
	//assert(n_err_no == 0);
	_tcscpy_s(s_buf, s_drive);
	_tcscat_s(s_buf, s_dir);

	return s_buf;
}
DWORD  CalcFileCRC(LPCTSTR lptcsPath)
{
	FILE* fp = NULL;
	_tfopen_s(&fp, lptcsPath, _T("rb"));
	if (!fp)
	{
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	DWORD dwFileSize= ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* lpBuffer = new char[dwFileSize];
	fread(lpBuffer, dwFileSize, 1, fp);

	DWORD dwCrc = CalcCRC(lpBuffer, dwFileSize);

	delete lpBuffer;
	fclose(fp);
	return dwCrc;
}
DWORD  CalcCRC(char* ptr, DWORD Size)
{
	static DWORD crcTable[256];
	static bool bTableIniFlag = false;
	if (!bTableIniFlag)
	{
		DWORD crcTmp1;
		for (int i=0; i<256; i++)
		{
			crcTmp1 = i;
			for (int j=8; j>0; j--)
			{
				if (crcTmp1&1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
				else crcTmp1 >>= 1;
			}

			crcTable[i] = crcTmp1;
		}
		bTableIniFlag = true;
	}
	

	DWORD dwRet = 0;

	DWORD crcTmp2 = 0xFFFFFFFF;
	while(Size--)
	{
		crcTmp2 = ((crcTmp2>>8) & 0x00FFFFFF) ^ crcTable[ (crcTmp2^(*ptr)) & 0xFF ];
		ptr++;
	}

	dwRet=(crcTmp2^0xFFFFFFFF);
	return dwRet;
}

tstring GetPCName()
{
	TCHAR szBuffer[MAX_PATH];
	DWORD dwSize;

	szBuffer[0] = '\0';

	// 获取计算机名，如果获取失败，尝试用其他函数来获取
	int nRet = GetComputerName(szBuffer, &dwSize);
	DWORD dwErr = 0;
	if(nRet == 0)
	{
		dwErr = GetLastError();
		WRITE_LOG_LOCAL(_T("GetComputerName Failed, ErrorCode=%d"), dwErr);
	}
	if(_tcslen(szBuffer) > 0)
		return szBuffer;

	nRet = GetComputerNameEx(ComputerNameNetBIOS, szBuffer, &dwSize);
	if(nRet == 0)
	{
		// 这个函数有时候即使获取到计算机名，也会返回一个234的错误
		dwErr = GetLastError();
		WRITE_LOG_LOCAL(_T("GetComputerNameEx-ComputerNameNetBIOS Failed, ErrorCode=%d"), dwErr);
	}
	WRITE_LOG_LOCAL(_T("GetComputerName by GetComputerNameEx-ComputerNameNetBIOS, Name=%s, Length=%d"), szBuffer, dwSize);
	if(_tcslen(szBuffer) > 0)
		return szBuffer;

	char szName[MAX_PATH];
	memset(szName, 0, MAX_PATH);
	nRet = gethostname(szName, MAX_PATH);
	if(nRet == SOCKET_ERROR)
	{
		dwErr = WSAGetLastError();
		WRITE_LOG_LOCAL(_T("gethostname Failed, ErrorCode=%d"), dwErr);
	}
	if(strlen(szName) > 0)
	{
#ifdef _UNICODE
		MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, szBuffer, MAX_PATH); 
#else
		memcpy(szBuffer, szName, strlen(szName) +1);
#endif
	}
	WRITE_LOG_LOCAL(_T("GetComputerName by gethostname, Name=%s"), szBuffer);

	return szBuffer;
}

void GetScreenResolution(int& nScreenWidth, int& nScreenHeight)
{
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
}

string GetMac()
{
	return "";
}

DWORD GetPCID()
{
	return 0;
}

unsigned char ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}

unsigned char FromHex(unsigned char x) 
{ 
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else
		y = 0;

	return y;
}

tstring UrlEncode(string str, bool bFullUrl /*= true*/)
{
	string strTemp = "";
	size_t length = str.length();

	for (size_t i = 0; i < length; i++)
	{
		char c = str[i];

		if( str[i] == ' ')
			strTemp += "%20";

		else if( str[i] == '+' )
			strTemp += "%2b";

		else if( !bFullUrl && (str[i] == '/' || str[i] == '=') )
		{
			if( str[i] == '/' )
				strTemp += "%2f";

			else if( str[i] == '=' )
				strTemp += "%3d";
		}
		else if (isalnum((unsigned char)str[i]) || 
			(str[i] == '-') ||
			(str[i] == '_') || 
			(str[i] == '.') || 
			(str[i] == '~') ||
			c >= 0 )
			strTemp += str[i];

		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}

	return Utf82Str(strTemp);
}

tstring	UrlEncodeEx(string str)
{
	std::string strTemp = "";  
	size_t length = str.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (isalnum((unsigned char)str[i]) ||   
			(str[i] == '-') ||  
			(str[i] == '_') ||   
			(str[i] == '.') ||   
			(str[i] == '~'))  
			strTemp += str[i];  
		else if (str[i] == ' ')  
			strTemp += "+";  
		else  
		{  
			strTemp += '%';  
			strTemp += ToHex((unsigned char)str[i] >> 4);  
			strTemp += ToHex((unsigned char)str[i] % 16);  
		}  
	}  

	return Utf82Str(strTemp);
}
vector<tstring> SplitString( tstring strData, size_t nSize, TCHAR chSpe, bool bPassNull )
{
	vector<tstring> result;

	int iTmpLen		= 0;
	while(iTmpLen < nSize)
	{
		int	iPos = strData.find(chSpe, iTmpLen);

		if (iPos == -1)
			break;

		tstring tmpStr = strData.substr(iTmpLen, iPos - iTmpLen);
		iTmpLen = iPos+1;
		result.push_back(tmpStr);
	}

	if (iTmpLen < nSize)
		result.push_back(strData.substr(iTmpLen, nSize - iTmpLen));

	return result;

}


//Win32Api:
void AdjustPrivilege()
{
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		}
		CloseHandle(hToken);
	}
}

BOOL FindProcess(tstring strExeName) 
{ 
	PROCESSENTRY32 my; 
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				_tcsupr(my.szExeFile);
				if ( _tcsicmp(my.szExeFile, strExeName.c_str()) == 0) { 
						CloseHandle(l); 
						return TRUE;
				}
			}while (Process32Next(l, &my));
		} 
		CloseHandle(l); 
	} 
	return FALSE;
}

DWORD GetCoursePlayerPid()
{  
	DWORD nProcessIdtemp = 0;
	HWND _hwnd = FindWindow(_T("NdCefOSRWindowsClass"),NULL);
	if (IsWindow(_hwnd)){
		GetWindowThreadProcessId(_hwnd,&nProcessIdtemp);
	}
	return nProcessIdtemp;  
}

int KillExeCheckParentPidNotPlayer(tstring strExeName, BOOL bCloseAll ) 
{ 
	AdjustPrivilege();
	PROCESSENTRY32 my; 
	DWORD nProcessIdtemp =  GetCoursePlayerPid();
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				_tcsupr(my.szExeFile);
				if (_tcsstr(my.szExeFile, strExeName.c_str()) != NULL
					|| _tcsicmp(my.szExeFile, strExeName.c_str()) == 0) { 
						if (my.th32ProcessID==nProcessIdtemp){
							continue;
						}
						HANDLE hProcessHandle;
						hProcessHandle = OpenProcess(PROCESS_TERMINATE, true, my.th32ProcessID); 
						if (hProcessHandle != NULL)
						{ 
							if(TerminateProcess(hProcessHandle, 0) != NULL)
							{
								CloseHandle(hProcessHandle);
							}
						}
						if (!bCloseAll)//2015.12.03 cws
						{
							CloseHandle(l);					
							return 0; 
						}
				}
			}while (Process32Next(l, &my));
		} 
		CloseHandle(l); 
	} 
	return 0;
}
int KillExeCheckParentPid(tstring strExeName, BOOL bCloseAll ) 
{ 
	AdjustPrivilege();
	PROCESSENTRY32 my; 
	int nCount = 0;
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				_tcsupr(my.szExeFile);
				if (_tcsstr(my.szExeFile, strExeName.c_str()) != NULL
					|| _tcsicmp(my.szExeFile, strExeName.c_str()) == 0) { 
					HANDLE hProcessHandle;
					hProcessHandle = OpenProcess(PROCESS_TERMINATE, true, my.th32ProcessID); 
					if (hProcessHandle != NULL)
					{ 
						if(TerminateProcess(hProcessHandle, 0) != NULL)
						{
							CloseHandle(hProcessHandle);
							nCount++;
						}
					}
					if (!bCloseAll)//2015.12.03 cws
					{
						CloseHandle(l);					
						return 1; 
					}
				}
			}while (Process32Next(l, &my));
		} 
		CloseHandle(l); 
	} 
	return nCount;
}

tstring& trim(tstring &s) 
{
	if (s.empty()) 
	{
		return s;
	}

	s.erase(0,s.find_first_not_of(_T(" ")));
	s.erase(s.find_last_not_of(_T(" ")) + 1);
	return s;
}


void CreateShortFileName(TCHAR * szFileName,TCHAR * szShortFileName,bool bShowExt = true)
{
	_tcscpy(szShortFileName,szFileName);
	if(!bShowExt)
	{
		TCHAR * pos = _tcsrchr(szShortFileName,'.');
		if(pos) *pos = 0;
	}
	if(_tcslen(szShortFileName) < 4)
	{
		TCHAR szShortFileNameTemp[MAX_PATH];
		wsprintf(szShortFileNameTemp,_T("  %s  "),szShortFileName);
		_tcscpy(szShortFileName,szShortFileNameTemp);
	}
}

//
unsigned __int64 GetCycleCount() 
{ 
	__asm _emit 0x0F
	__asm _emit 0x31 
} 



//usage:flash生成缩略图
//CreateFlashThumbFile("F:\\NDCloud\\Video\\《柳树醒了》课堂练习flash课件1.swf");
tstring CreateFlashThumbFile(tstring strFlashFile)
{

	 tstring szImagePath = strFlashFile;
	 tstring::size_type point = strFlashFile.length() - 3;
	 if(point != tstring::npos)
	 {
		szImagePath.replace(point, 3, _T("jpg"));
	 }

    TCHAR szCmdInfo[MAX_PATH * 4] = {0};

	HANDLE hFile = CreateFile(szImagePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		TCHAR szExe[MAX_PATH * 2];
		wsprintf(szExe,_T("%s\\bin\\floatPlayer.exe"), GetLocalPath().c_str());
		wsprintf(szCmdInfo, _T("\"%s\\bin\\floatPlayer.exe\" \"%s\" -s"), GetLocalPath().c_str(), strFlashFile.c_str());

		PROCESS_INFORMATION pi;
		STARTUPINFO si;      //隐藏进程窗口
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		si.cbReserved2 = NULL;
		si.lpReserved2 = NULL;
		BOOL ret = CreateProcess(szExe,szCmdInfo,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
		if(ret)
		{
			WaitForSingleObject(pi.hProcess, 3*1000);
			return szImagePath;
		}
		else
		{
			return _T("");
		}
		
	}
	else
		CloseHandle( hFile );
	return szImagePath;
}

//
// obtain ip address and mac address
//
bool GetLocalIpMacs(vector<pair<string, string>>& vecIpMacs)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter=NULL;
	DWORD dwRetVal=0;

	pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo=(IP_ADAPTER_INFO*)malloc(ulOutBufLen);
	}

	vector<PIP_ADAPTER_INFO> vecWifi;
	vector<PIP_ADAPTER_INFO> vecEthernet;

	if((dwRetVal = GetAdaptersInfo(pAdapterInfo,&ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;

		while(pAdapter)
		{
			//
			// pAdapter->Description中包含"PCI"为：物理网卡
			// pAdapter->Type是71为：无线网卡
			//
			if( pAdapter->Type == MIB_IF_TYPE_ETHERNET && strstr(pAdapter->Description, "VMware") == NULL )		
				vecEthernet.push_back(pAdapter);

			else if( pAdapter->Type == IF_TYPE_IEEE80211 )
				vecWifi.push_back(pAdapter);

			pAdapter = pAdapter->Next;
		}
	}

	// first ethenet
	for(int i = 0; i < vecEthernet.size(); i++)
	{
		pAdapter = vecEthernet[i];

		string strMac = "";
		for(UINT i = 0 ;i < pAdapter->AddressLength; i++)
		{
			char szMac[16];
			sprintf_s(szMac, "%02X",pAdapter->Address[i]);
			strMac += szMac;

			if( i < pAdapter->AddressLength-1 )
				strMac += "-";
		}

		char szIP[64];
		strcpy_s(szIP, 64, pAdapter->IpAddressList.IpAddress.String);

		if( _strcmpi(szIP, "0.0.0.0") == 0 )
			continue;

		vecIpMacs.push_back(make_pair(szIP, strMac));
	}

	// second wifi
	for(int i = 0; i < vecWifi.size(); i++)
	{
		pAdapter = vecWifi[i];

		string strMac = "";
		for(UINT i = 0 ;i < pAdapter->AddressLength; i++)
		{
			char szMac[16];
			sprintf_s(szMac, "%02X",pAdapter->Address[i]);
			strMac += szMac;

			if( i < pAdapter->AddressLength-1 )
				strMac += "-";
		}

		char szIP[64];
		strcpy_s(szIP, 64, pAdapter->IpAddressList.IpAddress.String);

		if( _strcmpi(szIP, "0.0.0.0") == 0 )
			continue;

		vecIpMacs.push_back(make_pair(szIP, strMac));
	}


	free(pAdapterInfo);

	return true;
}


BOOL isEndChapterTreeNode(ChapterNode* pNode, BOOL bDirect)
{
	BOOL bRet = FALSE;

	ChapterNode * pSlibing = pNode;
	while(pSlibing)
	{
		if(bDirect)
		{
			if(pSlibing->pPrevSlibing)
				break;
		}
		else
		{
			if(pSlibing->pNextSlibing)
				break;
		}
		pSlibing = pSlibing->pParent;
		if(pSlibing == NULL)
		{
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

ChapterNode* TraversalChildChapterTree(ChapterNode* pNode, BOOL bDirect)
{
	ChapterNode* pSlibing = NULL;
	if(bDirect)
	{
		pNode = pNode->pFirstChild;
		while(pNode->pNextSlibing)
		{
			pNode = pNode->pNextSlibing;
		}
		if( pNode->pFirstChild != NULL )
		{
			pSlibing = TraversalChildChapterTree(pNode, bDirect);
		}
		else
			pSlibing = pNode;
	}
	else
	{
		pNode = pNode->pFirstChild;
		if( pNode->pFirstChild != NULL )
		{
			pSlibing = TraversalChildChapterTree(pNode, bDirect);
		}
		else
			pSlibing = pNode;
	}

	return pSlibing;
}

ChapterNode* TraversalParentChapterTree(ChapterNode* pNode, BOOL bDirect)
{
	ChapterNode* pSlibing = pNode->pParent;

	if(bDirect)
	{
		if(pSlibing->pPrevSlibing == NULL)
		{
			if( pSlibing->pParent != NULL )
				pSlibing = TraversalParentChapterTree(pSlibing, bDirect);
		}
		else
		{
			pSlibing = pSlibing->pPrevSlibing;
			if(pSlibing->pFirstChild != NULL)
			{
				pSlibing = TraversalChildChapterTree(pSlibing, bDirect);
			}
		}
	}
	else
	{
		if(pSlibing->pNextSlibing == NULL)
		{
			if( pSlibing->pParent != NULL )
				pSlibing = TraversalParentChapterTree(pSlibing, bDirect);
		}
		else
		{
			pSlibing = pSlibing->pNextSlibing;
			if(pSlibing->pFirstChild != NULL)
			{
				pSlibing = TraversalChildChapterTree(pSlibing, bDirect);
			}
		}
	}

	return pSlibing;
}

ChapterNode* TraversalChapterTree(ChapterNode* pNode, BOOL bDirect)
{
	ChapterNode* pSlibing = NULL;
	if(bDirect)
	{
		if( pNode->pPrevSlibing == NULL )
		{
			if( pNode->pParent != NULL )
				pSlibing = TraversalParentChapterTree(pNode, bDirect);
		}
		else
		{
			pSlibing = pNode->pPrevSlibing;
			if( pSlibing->pFirstChild != NULL )
			{
				pSlibing = TraversalChildChapterTree(pSlibing, bDirect);
			}
		}
	}
	else
	{
		if( pNode->pNextSlibing == NULL )
		{
			if( pNode->pParent != NULL )
				pSlibing = TraversalParentChapterTree(pNode, bDirect);
		}
		else
		{
			pSlibing = pNode->pNextSlibing;
			if( pSlibing->pFirstChild != NULL )
			{
				pSlibing = TraversalChildChapterTree(pSlibing, bDirect);
			}
		}
	}

	return pSlibing;
}

tstring LTrim( tstring &str )
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
		std::not1(std::ptr_fun(::isspace))));
	return str;
}

tstring RTrim( tstring &str )
{
	str.erase(std::find_if(str.rbegin(), str.rend(),
		std::not1(std::ptr_fun(::isspace))).base(),
		str.end());
	return str;
}

tstring Trim( tstring &str )
{
	return RTrim(LTrim(str));
}


INT64 GetMilliTime()
{
	// since 1970.1.1 
	timeb tv;
	ftime(&tv);

	INT64 totalMilliseconds =  tv.time*1000+tv.millitm;
	return totalMilliseconds;
}

INT64 GetNanoTime()
{
	// from 1.1.1  to 1970.1.1  unit: 100ns
	INT64 start = 621355968000000000;

	INT64 millisecond = GetMilliTime();
	millisecond *= 10000;

	INT64 time = start + millisecond;
	return time;
}

//============== pic transfer =================================

bool GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid)  
{  
	UINT  unNum = 0;          // number of image encoders  
	UINT  unSize = 0;         // size of the image encoder array in bytes  

	ImageCodecInfo* pImageCodecInfo = NULL;  

	// How many encoders are there?  
	// How big (in bytes) is the array of all ImageCodecInfo objects?  
	GetImageEncodersSize( &unNum, &unSize );  
	if ( 0 == unSize ) {  
		return false;  // Failure  
	}  

	// Create a buffer large enough to hold the array of ImageCodecInfo  
	// objects that will be returned by GetImageEncoders.  
	pImageCodecInfo = (ImageCodecInfo*)( malloc(unSize) );  
	if ( !pImageCodecInfo ) {  
		return false;  // Failure  
	}  

	// GetImageEncoders creates an array of ImageCodecInfo objects  
	// and copies that array into a previously allocated buffer.   
	// The third argument, imageCodecInfos, is a pointer to that buffer.   
	GetImageEncoders( unNum, unSize, pImageCodecInfo );  

	for ( UINT j = 0; j < unNum; ++j ) {  
		if ( wcscmp( pImageCodecInfo[j].MimeType, pszFormat ) == 0 ) {  
			*pClsid = pImageCodecInfo[j].Clsid;  
			free(pImageCodecInfo);  
			pImageCodecInfo = NULL;  
			return true;  // Success  
		}      
	}  

	free( pImageCodecInfo );  
	pImageCodecInfo = NULL;  
	return false;  // Failure  
}  

bool CompressImageQuality(   
						  const WCHAR* pszOriFilePath,   
						  const WCHAR* pszDestFilePah,  
						  ULONG quality,
						  WCHAR* format /*= L"image/jpeg"*/)  
{  

	Status stat = GenericError;  

	// Get an image from the disk.  
	Image* pImage = new Image(pszOriFilePath);  

	do {  
		if ( NULL == pImage ) {  
			break;  
		}  

		// 获取长宽  
		UINT ulHeight = pImage->GetHeight();  
		UINT ulWidth = pImage->GetWidth();  
		if ( ulWidth < 1 || ulHeight < 1 ) {  
			break;  
		}  

		// Get the CLSID of the JPEG encoder.  
		CLSID encoderClsid;  
		if ( !GetEncoderClsid(format, &encoderClsid) ) {  
			break;  
		}  

		// The one EncoderParameter object has an array of values.  
		// In this case, there is only one value (of type ULONG)  
		// in the array. We will let this value vary from 0 to 100.  
		EncoderParameters encoderParameters;  
		encoderParameters.Count = 1;  
		encoderParameters.Parameter[0].Guid = EncoderQuality;  
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;  
		encoderParameters.Parameter[0].NumberOfValues = 1;  
		encoderParameters.Parameter[0].Value = &quality;  
		stat = pImage->Save(pszDestFilePah, &encoderClsid, &encoderParameters);  
	} while(0);  

	if ( pImage ) {  
		delete pImage;  
		pImage = NULL;  
	}  

	return ( ( stat == Ok ) ? true : false );  
}  

bool CompressImagePixel(   
						const WCHAR* pszOriFilePath,   
						const WCHAR* pszDestFilePah,   
						UINT ulMaxHeigth,   
						UINT ulMaxWidth,
						WCHAR* format/* = L"image/jpeg"*/)  
{  
	Status stat = GenericError;    


	// Get an image from the disk.  
	Image* pImage = new Image(pszOriFilePath); 
	do {  
		if ( NULL == pImage ) {  
			break;  
		}
		// 获取长宽  
		UINT unOriHeight = pImage->GetHeight();  
		UINT unOriWidth = pImage->GetWidth();  

		int maxHeight = ulMaxHeigth;  //图片高度最大限制
		int maxWidth = ulMaxWidth;   //图片宽度最大限制


		int imgWidth = unOriWidth;
		int imgHeight = unOriHeight;

		float toImgWidth = (float)ulMaxWidth;
		float toImgHeight= (float)ulMaxHeigth;

		if (imgWidth > imgHeight)  //如果宽度超过高度以宽度为准来压缩
		{
			if (imgWidth > maxWidth)  //如果图片宽度超过限制
			{
				toImgWidth = maxWidth;   //图片压缩后的宽度
				toImgHeight = imgHeight / (float)(imgWidth / toImgWidth); //图片压缩后的高度
			}
		}
		else
		{
			if (imgHeight > maxHeight)
			{
				toImgHeight = maxHeight;
				toImgWidth = imgWidth / (float)(imgHeight / toImgHeight);
			}
		}

		REAL ulNewWidth=toImgWidth;
		REAL ulNewHeigth=toImgHeight;


		do {  
			CLSID encoderClsid;  
			if ( unOriWidth < 1 || unOriHeight < 1 ) {  
				break;  
			}  

			// Get the CLSID of the JPEG encoder.  
			if ( !GetEncoderClsid(format, &encoderClsid) ) {  
				break;  
			}  

			REAL fSrcX = 0.0f;  
			REAL fSrcY = 0.0f;  
			REAL fSrcWidth = (REAL) unOriWidth;  
			REAL fSrcHeight = (REAL) unOriHeight ;  
			RectF RectDest( 0.0f, 0.0f, (REAL)ulNewWidth, (REAL)ulNewHeigth);  

			Bitmap* pTempBitmap = new Bitmap( ulNewWidth, ulNewHeigth );  
			Graphics* graphics = NULL;  

			do {  
				if ( !pTempBitmap ) {  
					break;  
				}  

				graphics = Graphics::FromImage( pTempBitmap );  
				if ( !graphics ) {  
					break;  
				}  

				stat = graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);  
				if ( Ok != stat ) {  
					break;  
				}  

				stat = graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);  
				if ( Ok != stat ) {  
					break;  
				}  

				stat = graphics->DrawImage( pImage, RectDest, fSrcX, fSrcY, fSrcWidth, fSrcHeight,  
					UnitPixel, NULL, NULL, NULL);  
				if ( Ok != stat ) {  
					break;  
				}  

				stat = pTempBitmap->Save( pszDestFilePah, &encoderClsid, NULL );  
				if ( Ok != stat ) {  
					break;  
				}  

			} while(0);  

			if ( NULL != graphics ) {  
				delete graphics;  
				graphics = NULL;  
			}  

			if ( NULL != pTempBitmap ) {  
				delete pTempBitmap;  
				pTempBitmap = NULL;  
			}  
		} while(0);  
	} while (0);  

	if ( pImage ) {  
		delete pImage;  
		pImage = NULL;  
	}  

	return ( ( Ok == stat ) ? true : false );  
}

bool GetImageSize(tstring strPath, UINT& nWidth, UINT& nHeight)
{
	Image* pImage = new Image(Str2Unicode(strPath).c_str()); 
	if ( NULL != pImage ) 
	{  
		nHeight		= pImage->GetHeight();  
		nWidth		= pImage->GetWidth(); 
		return true;
	}

	return false;
}


tstring CreateAdaptiveScreenPicture( tstring strPicPath )
{
	Gdiplus::Image* pImage = new Image(Str2Unicode(strPicPath).c_str());
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();

	tstring strImagePath = strPicPath;

	int nSX = GetSystemMetrics(SM_CXSCREEN);
	int nSY = GetSystemMetrics(SM_CYSCREEN);

	if( nWidth > nSX || nHeight > nSY )
	{
		WCHAR wszTempPath[MAX_PATH];
		GetTempPathW(MAX_PATH, wszTempPath);

		DWORD dwCrc = CalcFileCRC(strPicPath.c_str());

		WCHAR wszTempFileName[MAX_PATH];
		swprintf_s(wszTempFileName, L"%s%X.png", wszTempPath, dwCrc);

		DWORD dwRet = GetFileAttributesW(wszTempFileName);

		BOOL bSucess = TRUE;

		if (dwRet == INVALID_FILE_ATTRIBUTES)
		{
			bSucess = CompressImagePixel(Str2Unicode(strPicPath).c_str(), wszTempFileName, nSX, nSY, L"image/png");
		}
			
		if ( bSucess )
			strImagePath = Un2Str(wszTempFileName);

	}

	delete pImage;

	return strImagePath;
}

TiXmlElement* GetElementsByTagName( TiXmlElement* pElement, string strTagName )
{
	TiXmlElement* p = NULL;
	if( pElement->Value() == strTagName )
		return pElement;

	// find child
	TiXmlElement* pChild = pElement->FirstChildElement();
	if( pChild )
	{
		p = GetElementsByTagName(pChild, strTagName);
		if( p != NULL )
			return p;
	}

	// find slibing
	TiXmlElement* pSlibing = pElement->NextSiblingElement();
	if( pSlibing )
		p = GetElementsByTagName(pSlibing, strTagName);

	return p;
}

bool createDirWithFullPath(TCHAR* strDirName)
{
	tstring strRelativePath(strDirName);
	int nPos = 1;
	tstring strTmpPath;
	tstring strCompletePath = _T("");
	while(strRelativePath.length() > 0)
	{
		nPos = strRelativePath.find_first_of(_T('\\'));
		if (nPos != -1)
		{
			strTmpPath = strRelativePath.substr(0 , nPos);
			strRelativePath = strRelativePath.substr(nPos + 1, strRelativePath.length() - (nPos + 1));
		}
		else
		{
			break;
// 			strTmpPath = strRelativePath.substr(0, strRelativePath.length());
// 			strRelativePath = "";
		}

		strCompletePath += strTmpPath;
		CreateDirectory(strCompletePath.c_str(), NULL);
		strCompletePath += _T('\\');
	}

	return TRUE;
}

bool isICRPlayer()
{
	tstring strPath = GetLocalPath();
	strPath += _T("\\icrplayer\\ND_ICR_WPF_PLAYER.exe");

	DWORD ret = GetFileAttributes(strPath.c_str());
	if( ret == INVALID_FILE_ATTRIBUTES )
		return FALSE;	

	return TRUE;
}


string NewGuid(bool bCross, bool bLowerCase)
{
	static char buf[64] = {0};
	GUID guid;

	if( CoCreateGuid(&guid) == S_OK )
	{
		if( bCross )
		{
			sprintf(buf, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X", 
					guid.Data1, guid.Data2,
					guid.Data3, guid.Data4[0], 
					guid.Data4[1], guid.Data4[2], 
					guid.Data4[3], guid.Data4[4], 
					guid.Data4[5], guid.Data4[6],
					guid.Data4[7] );

		}
		else
		{
			sprintf(buf, "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X", 
				guid.Data1, guid.Data2,
				guid.Data3, guid.Data4[0], 
				guid.Data4[1], guid.Data4[2], 
				guid.Data4[3], guid.Data4[4], 
				guid.Data4[5], guid.Data4[6],
				guid.Data4[7] );
		}

	}

	if( bLowerCase )
		_strlwr(buf);

	return buf;
}

bool SavePassword( tstring strUserName, tstring strPassword )
{
	PASSWORD_UNIT PasswordUnit;
	srand(GetTickCount());
	for(int i =0 ; i < sizeof(PasswordUnit.szRandKeyStart) ; i++)
	{
		PasswordUnit.szRandKeyStart[i] = rand() % 0xFF;
	}

	char szKey[] = {'a','e','i','o','u','\0'};
	char szEncryptUserName[MAX_PATH * 2] = {0};
	EncryptA(Str2Ansi(strUserName).c_str(), strUserName.length(), szEncryptUserName, szKey);
	string strOut;
	CBase64::Encode((unsigned char *)szEncryptUserName, strlen(szEncryptUserName), strOut);

	strcpy_s(PasswordUnit.szUserName, MAX_PATH * 2, strOut.c_str());
	PasswordUnit.szUserName[MAX_PATH * 2] = 0;

	char szEncryptPassword[MAX_PATH * 2] = {0};
	EncryptA(Str2Ansi(strPassword).c_str(), strPassword.length(), szEncryptPassword, Str2Ansi(strUserName).c_str());
	CBase64::Encode((unsigned char *)szEncryptPassword, strlen(szEncryptPassword), strOut);

	strcpy_s(PasswordUnit.szPassword, MAX_PATH * 2, strOut.c_str());
	PasswordUnit.szPassword[MAX_PATH * 2] = 0;

	for(int i =0 ; i < sizeof(PasswordUnit.szRandKeyEnd) ; i++)
	{
		PasswordUnit.szRandKeyEnd[i] = rand() % 0xFF;
	}

	DWORD dwCRC = CalcCRC((char *)&PasswordUnit, sizeof(PASSWORD_UNIT) - sizeof(DWORD));
	PasswordUnit.dwCRC = dwCRC;

	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Account.dat");

	FILE * fp = _tfopen(strPath.c_str(), _T("wb+") );
	if(fp)
	{
		fwrite(&PasswordUnit, sizeof(PASSWORD_UNIT), 1, fp);
		fclose(fp);
		return true;
	}
	return false;
}

bool GetPassword(LPTSTR szUserName, LPTSTR szPassword)
{
	PASSWORD_UNIT PasswordUnit;

	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Account.dat");

	FILE * fp = _tfopen(strPath.c_str(), _T("rb") );
	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		int nSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if( nSize < sizeof(PASSWORD_UNIT))
		{
			fclose(fp);
			return false;
		}


		fread(&PasswordUnit, sizeof(PASSWORD_UNIT), 1, fp);

		DWORD dwCRC = CalcCRC((char *)&PasswordUnit, sizeof(PASSWORD_UNIT) - sizeof(DWORD));
		if(dwCRC != PasswordUnit.dwCRC)
		{
			fclose(fp);
			return false;
		}

		char szDecryptUserName[MAX_PATH] = {0};
		char szDecryptUserName1[MAX_PATH] = {0};
		unsigned long nLen = MAX_PATH;
		const string strDecryptUserName = PasswordUnit.szUserName;
		if(!CBase64::Decode(strDecryptUserName, (unsigned char *)szDecryptUserName, &nLen))
		{
			fclose(fp);
			return false;
		}
		szDecryptUserName[nLen] = 0;

		CHAR szKey[] = {'a','e','i','o','u','\0'};
		DecryptA(szDecryptUserName, strlen(szDecryptUserName), szDecryptUserName1, szKey);
		_tcscpy_s(szUserName, MAX_PATH - 1, Ansi2Str(szDecryptUserName1).c_str());

		char szDecryptPassword[MAX_PATH * 2] = {0};
		char szDecryptPassword1[MAX_PATH * 2] = {0};
		const string strDecryptPassword = PasswordUnit.szPassword;
		nLen = MAX_PATH * 2;
		if(!CBase64::Decode(strDecryptPassword, (unsigned char *)szDecryptPassword, &nLen))
		{
			fclose(fp);
			return false;
		}
		szDecryptPassword[nLen] = 0;

		DecryptA(szDecryptPassword, strlen(szDecryptPassword), szDecryptPassword1, szDecryptUserName1);
		
		_tcscpy_s(szPassword, MAX_PATH - 1, Ansi2Str(szDecryptPassword1).c_str());
		fclose(fp);

		return true;
	}
	return false;
}

void EncryptA(const char* pInput , int nInPutLen, char * pOutput, const char * szKey)
{
	char szTemp[3] = {0};

	for(int i = 0 ; i < nInPutLen ; i++)
	{
		memset(szTemp,0x00,sizeof(szTemp));
		sprintf(szTemp,"%0.2x",pInput[i]^szKey[i % strlen(szKey)]);
		pOutput[i*2] = szTemp[0];
		pOutput[i*2+1] = szTemp[1];
	}
}

void DecryptA(const char * pInput,int nInPutLen ,char * pOutput, const char * szKey)
{
	char szTemp[3] = {0};
	UINT nValue;
	for(int i = 0 ; i < nInPutLen/2 ; i++)
	{
		szTemp[0] = pInput[i*2];
		szTemp[1] = pInput[i*2+1];
		nValue = strtoul(szTemp, NULL , 16);
		nValue = nValue ^ szKey[i % strlen(szKey)];
		pOutput[i] = nValue;
	}
}

bool RemoveSaveAccount()
{
	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Account.dat");
	DeleteFile(strPath.c_str());

	return true;
}

// zip
bool ZipPackDir( string strSrcDicPath)
{
	return CZipper::ZipFolder(strSrcDicPath.c_str(), true);
}

bool UnZipPackDir(string strSrcFile, string strDestPath )
{
	CUnZipper UnZipper;
	return UnZipper.UnZip(strSrcFile.c_str(), strDestPath.c_str());
}

//

BOOL	m_bFileDialogShow	= FALSE;

BOOL FileSelectDialog( LPCTSTR lptcsTitile, LPCTSTR lptcsFilter, DWORD Flags, vector<tstring>& vctrFiles, HWND hWndParent )
{

	m_bFileDialogShow		= TRUE;
	
	OPENFILENAME	ofn		= {0};
	ofn.nMaxFile			= sizeof(TCHAR) * 500 * MAX_PATH;
	TCHAR*			szFile	= new TCHAR[ofn.nMaxFile + 1];
	memset(szFile, 0, ofn.nMaxFile);
	ofn.hwndOwner			= hWndParent;
	ofn.lStructSize			= sizeof(OPENFILENAME);  
	ofn.lpstrFilter			= lptcsFilter;
	ofn.lpstrFile			= szFile;
	
	ofn.nMaxFileTitle		= MAX_PATH;  
	//ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= lptcsTitile;//_T("上传文件");
	ofn.Flags				= Flags | OFN_EXPLORER;//OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST| OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	BOOL bRet = GetOpenFileName(&ofn);
	DWORD dwRet = CommDlgExtendedError();
	if (bRet)
	{
		vector<tstring> vctrFileList;
		TCHAR szPath[MAX_PATH * 2] = {0};
		TCHAR szCopy[MAX_PATH * 2] = {0};
		_tcsncpy_s(szPath, szFile, ofn.nFileOffset);

		if(szPath[_tcslen(szPath) - 1] != TEXT('\\'))
		{
			_tcscat_s(szPath, MAX_PATH, TEXT("\\"));
		}

		TCHAR* pFilePath = szFile + ofn.nFileOffset;

		while( *pFilePath )
		{
			ZeroMemory(szCopy, sizeof(szCopy));
			_tcscpy_s(szCopy, szPath);
			_tcscat_s(szCopy, pFilePath);
			vctrFiles.push_back(szCopy);
			pFilePath += _tcslen(pFilePath) + 1;
		}
	}
	else if ( !bRet && dwRet == FNERR_BUFFERTOOSMALL )
		bRet = -1;

	delete szFile;

	m_bFileDialogShow		= FALSE;

	return bRet;
}

BOOL FileSaveDialog( LPCTSTR lptcsTitile, LPCTSTR lptcsDefalutName, LPCTSTR lptcsFilter, DWORD Flags, tstring& strFile, HWND hWndParent )
{
	m_bFileDialogShow		= TRUE;
	
	LPCTSTR lpExt = lptcsFilter + _tcslen(lptcsFilter) + 2 * sizeof(TCHAR);

	TCHAR	szFile[MAX_PATH * 2]	= {0};
	wsprintf(szFile, _T("%s%s"), lptcsDefalutName, lpExt);

	OPENFILENAME	ofn		= {0};
	ofn.hwndOwner			= hWndParent;
	ofn.lStructSize			= sizeof(OPENFILENAME);  
	ofn.lpstrFilter			= lptcsFilter;  
	ofn.lpstrFile			= szFile;
	ofn.nMaxFile			= sizeof(szFile);
	ofn.lpstrFileTitle		= szFile;
	ofn.nMaxFileTitle		= MAX_PATH;  
	ofn.lpstrTitle			= lptcsTitile;
	ofn.Flags				= Flags | OFN_EXPLORER | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt			= lpExt;

	strFile					= _T("");

	BOOL bRet	= GetSaveFileName(&ofn);
	if (bRet)
		strFile = szFile;

	m_bFileDialogShow		= FALSE;

	return bRet;
}

BOOL FileOpenDialog( LPCTSTR lptcsTitile, LPCTSTR lptcsFilter, DWORD Flags, tstring& strFile, BOOL& bReadOnly, HWND hWndParent /*= NULL*/ )
{
	m_bFileDialogShow		= TRUE;
	
	TCHAR			szFile[5*MAX_PATH]	= {0};

	OPENFILENAME	ofn		= {0};
	ofn.hwndOwner			= hWndParent;
	ofn.lStructSize			= sizeof(OPENFILENAME);  
	ofn.lpstrFilter			= lptcsFilter;
	ofn.lpstrFile			= szFile;
	ofn.nMaxFile			= sizeof(TCHAR)*5*MAX_PATH;
	ofn.nMaxFileTitle		= MAX_PATH;  
	//ofn.lpstrInitialDir		= NULL;  
	ofn.lpstrTitle			= lptcsTitile;
	ofn.Flags				= Flags & (~ OFN_ALLOWMULTISELECT) | OFN_EXPLORER;//OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST| OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	strFile					= _T("");
	::SetForegroundWindow(hWndParent); //设置焦点后可以看到窗口 2016.03.10 cws

	BOOL bRet = GetOpenFileName(&ofn);
	if (bRet)
	{
		bReadOnly	= ofn.Flags & OFN_READONLY;
		strFile		= szFile;
	}

	m_bFileDialogShow		= FALSE;

	return bRet;
}


UINT_PTR
CALLBACK OFNHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	static OPENFILENAME* pOfn = NULL;
	switch(uiMsg)
	{
	case WM_INITDIALOG:
		{
			pOfn = reinterpret_cast<OPENFILENAME*>(lParam);
		}
		break;
	case WM_NOTIFY:
		{
			NMHDR* pHdr = reinterpret_cast<NMHDR*>(lParam);
			switch(pHdr->code)
			{
			case CDN_SELCHANGE:
				{
					HWND hRealDlg = GetParent(hdlg);
					// +2 = one for slash, one for null
					UINT lenReqd = CommDlg_OpenSave_GetFolderPath(hRealDlg, 0, NULL) + 2;
					// +2 = one for intervening null, one for final null
					lenReqd += CommDlg_OpenSave_GetSpec(hRealDlg, 0, NULL) + 2;

					if(lenReqd > pOfn->nMaxFile)
					{
						delete [] pOfn->lpstrFile;
						pOfn->lpstrFile = new TCHAR[lenReqd];
						pOfn->nMaxFile = lenReqd;
					}
				}
				break;
			}
		}
		break;
	}
	return 0;
}

BOOL IsFileDialogShow()
{	
	return m_bFileDialogShow;	
}

int base64_encode(char *str,int str_len,char *encode,int encode_len)
{
	BIO *bmem,*b64;
	BUF_MEM *bptr;

	b64=BIO_new(BIO_f_base64());
	bmem=BIO_new(BIO_s_mem());
	b64=BIO_push(b64,bmem);

	BIO_write(b64,str,str_len); //encode

	BIO_flush(b64);
	BIO_get_mem_ptr(b64,&bptr);

	if(bptr->length > encode_len)
		return -1; 
	
	encode_len=bptr->length;
	memcpy(encode,bptr->data,bptr->length);

	BIO_free_all(b64);
	return encode_len;
}

int base64_decode(char *str,int str_len,char *decode,int decode_buffer_len)
{
	int len=0;
	BIO *b64,*bmem;

	b64=BIO_new(BIO_f_base64());

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem=BIO_new_mem_buf(str,str_len);

	bmem=BIO_push(b64,bmem);
	len=BIO_read(bmem,decode,str_len);

	decode[len]=0;
	BIO_free_all(bmem);

	return len;
}

bool CopyDir(CString strSrcPath, CString strDstPath, BOOL bFailIfExists)
{
	CreateDirectory(strDstPath,0);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strSrcPath + "\\" + "*.*");
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strFileName = finder.GetFileName();
		CString strSrc = strSrcPath + "\\" + strFileName;
		CString strDst = strDstPath + "\\" + strFileName;
		if(!finder.IsDots())
		{
			if(finder.IsDirectory())
			{
				if(!CopyDir(strSrc, strDst))
				{
					return false;
				}
			}
			else
			{
				CopyFile(strSrc, strDst, bFailIfExists);
// 				if(!CopyFile(strSrc, strDst, FALSE))
// 				{
// 					return false;
// 				}
			}

		}
	}      
	return true;
}

bool CopyDir( CString strSrcPath, CString strDstPath, LPTSTR szFailPath )
{
	CreateDirectory(strDstPath,0);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strSrcPath + "\\" + "*.*");
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strFileName = finder.GetFileName();
		CString strSrc = strSrcPath + "\\" + strFileName;
		CString strDst = strDstPath + "\\" + strFileName;
		if(!finder.IsDots())
		{
			if(finder.IsDirectory())
			{
				if(!CopyDir(strSrc, strDst, szFailPath))
				{
					return false;
				}
			}
			else
			{
				wstring wstrSrc, wstrDst;
				strSrc.Replace(_T("\\\\"), _T("\\"));
				strDst.Replace(_T("\\\\"), _T("\\"));
				wstrSrc = Str2Unicode(strSrc.GetString());
				wstrDst = Str2Unicode(strDst.GetString());
				BOOL bRet = CopyFileW(wstrSrc.c_str(), wstrDst.c_str(), FALSE);
				if(!bRet)
				{
					_tcscpy(szFailPath, strSrc.GetString());
					return false;
				}
				
			}

		}
	}      
	return true;
}

bool DeleteDir(CString strPath)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + "\\" + "*.*");
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strFileName = finder.GetFileName();
		CString strTempPath = strPath + "\\" + strFileName;
		if(!finder.IsDots())
		{
			if(finder.IsDirectory())
			{
				if(!DeleteDir(strTempPath))
				{
					return false;
				}
			}
			else
			{
				DeleteFile(strTempPath);
				//if(DeleteFile(strTempPath) == FALSE)
					//return false;
			}

		}
	}      
	RemoveDirectory(strPath);

	return true;
}


BOOL EnableDebugPriv()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	LUID Luid;

	if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
	{
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid ))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = Luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS) 
	{
		CloseHandle(hToken);
		return FALSE; 
	}

	CloseHandle(hToken);
	return TRUE;

}

void _SetProcessDPIAware()
{
	HINSTANCE hUser32 = LoadLibrary(_T("user32.dll"));
	if( hUser32 )
	{
		typedef BOOL ( WINAPI* LPSetProcessDPIAware )( void );
		LPSetProcessDPIAware pSetProcessDPIAware = ( LPSetProcessDPIAware )GetProcAddress( hUser32, "SetProcessDPIAware");
		if( pSetProcessDPIAware )
		{
			pSetProcessDPIAware();
		}
		FreeLibrary( hUser32 );
	}
}

BOOL SetPowerPointDPIAware(HWND hwnd)
{

	DWORD dwError = ERROR_SUCCESS;
	DWORD dwProcessId;
	HANDLE hProcess = NULL;
	tstring strDirectory;

	GetWindowThreadProcessId(hwnd, &dwProcessId);
	if(dwProcessId == 0)
	{
		dwError = GetLastError();
		goto _Cleanup;
	}

	if(!EnableDebugPriv())
	{
		dwError = GetLastError();
		goto _Cleanup;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if(NULL == hProcess)
	{
		dwError = GetLastError();
		goto _Cleanup;
	}


	typedef BOOL (WINAPI *pFnSetProcessDPIAware)();
	pFnSetProcessDPIAware pSetProcessDPIAware = (pFnSetProcessDPIAware)GetProcAddress(GetModuleHandle(TEXT("user32.dll")),"SetProcessDPIAware");
	if(pSetProcessDPIAware == NULL)
	{
		dwError = GetLastError();
		goto _Cleanup;
	}

	HANDLE hRemoteThread;
	//启动远程线程
	if( (hRemoteThread = CreateRemoteThread(hProcess,NULL,0, (LPTHREAD_START_ROUTINE)pSetProcessDPIAware, NULL, 0, NULL)) == NULL )
	{
		dwError = GetLastError();
		goto _Cleanup;
	}

_Cleanup:
	if(hProcess)
		CloseHandle(hProcess);

	return TRUE;
}


tstring GenerateQuestionThumbnail(tstring strPath)
{
	// thumbnail path	
	tstring strLocalPath = GetLocalPath();
	tstring strThumbPath = strPath;
	strThumbPath += _T(".jpg");

	// check whether
	DWORD ret = GetFileAttributes(strThumbPath.c_str());
	if( ret != INVALID_FILE_ATTRIBUTES )
		return strThumbPath;

	TCHAR szPlayerPath[2048] = {0};
	TCHAR szParam[2048]={0};

	tstring strAppDir = GetLocalPath();

	// translate slash
	tstring strQuestionPath = strPath;
	for(int i = 0; i < strQuestionPath.length(); i++)
	{
		if( strQuestionPath.at(i) == _T('\\') )
			strQuestionPath.replace(i, 1, _T("/"));
	}

	_stprintf_s(szPlayerPath, _T("file:\\\%s\\Package\\nodejs\\app\\player\\index.html"), strAppDir.c_str());

	_stprintf_s(szParam, _T("thumbnail %s?main=/%s&sys=pptshell&hidePage=footer \"%s\""), 
		UrlEncode(Str2Utf8(szPlayerPath)).c_str(), 
		UrlEncode(Str2Utf8(strQuestionPath)).c_str(), 
		strThumbPath.c_str());


	// generate question thumbnail
	TCHAR szExePath[1024]={0};
	_stprintf_s(szExePath, _T("%s\\bin\\CoursePlayer\\CoursePlayer.exe"), strLocalPath.c_str());


	tstring strWorkDirectory = strLocalPath;
	strWorkDirectory += _T("\\bin\\CoursePlayer");

	ShellExecute(NULL, _T("open"), szExePath, szParam, strWorkDirectory.c_str(), SW_HIDE);

	BOOL bGenerated = FALSE;
	int nTryCount = 0;
	while(nTryCount++ <= 100 )
	{
		DWORD ret = GetFileAttributes(strThumbPath.c_str());
		if( ret == INVALID_FILE_ATTRIBUTES )
			Sleep(200);
		else
		{
			bGenerated = TRUE;
			break;
		}
	}

	//if( !bGenerated )
		//return _T("");

	return strThumbPath;
}

tstring GetToolsPath()
{
	return GetLocalPath() +_T("\\Package\\nodejs\\app\\tools\\");
}

tstring GetQuestionJsonPath()
{
	return GetLocalPath() +_T("\\Package\\nodejs\\app\\prepare\\mapping");
}

bool g_bNodejsIsRunning =false;
bool isNodeJSRunning()
{
	return g_bNodejsIsRunning;
}
void setNodeJsRunning()
{
	g_bNodejsIsRunning=true;
}

OS_BIT_TYPE GetOsVersion()
{
	OS_BIT_TYPE eRet = OS_BIT_TYPE_32;

	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		eRet = OS_BIT_TYPE_64;
	}

	return eRet;
}


bool CheckBinIs64(string strBinFile)
{
	IMAGE_DOS_HEADER idh;
	FILE *f = fopen(strBinFile.c_str(), "rb");
	if (!f)
	{
		return false;
	}

	fread(&idh, sizeof(idh), 1, f);
	IMAGE_FILE_HEADER ifh; 
	fseek(f, idh.e_lfanew + 4, SEEK_SET); 
	fread(&ifh, sizeof(ifh), 1, f);
	fclose(f);

	int n = ifh.Machine;

	if (n == 0x014C)
		return false;		// 32位
	else if (n == 0x0200)
		return true;		// 纯64位
	else if (n == 0x8664)
		return true;		// 64位
	else
		return false;		// 未知

	return true;
}


int FindChar_f(LPSTR t, LPSTR s)
{
	char* p = strstr(t, s);
	if (NULL == p) return -1;
	return (int)(p-t);
}

int FindChar_b(LPSTR t, LPSTR s)
{
	char* p = strstr(t, s);
	if (NULL == p) return -1;
	char* pp = p;
	while (true)
	{
		p = strstr(&p[1], s);
		if (NULL == p) break;
		pp = p;
	}
	return (int)(pp-t);
}

int FindChar_n(LPSTR t, LPSTR s, int n)
{
	int lenS = strlen(s);
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<lenS; j++)
		{
			if (s[j] != t[i+j])
				break;
			if (j == lenS-1)
				return i;
		}
	}
	return -1;
}


int ConnWithTime(SOCKET& s_conn, int nTimeOut, int nReConnTime, const char* szServ, const int nPort, BOOL bStop)
{
	if (0 == strlen(szServ)) return -1;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);

	if (INADDR_NONE == inet_addr(szServ))
	{
		hostent* hp = gethostbyname(szServ);
		if (NULL == hp) return -1;
		addr.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(szServ);
	}

	int TimeOut = nTimeOut * 1000;
	unsigned long ul = 1;
	for (int i=0; i<nReConnTime && bStop; i++)
	{
		s_conn = socket(AF_INET, SOCK_STREAM, 0);

		//设置发送超时、接收超时、非阻塞方式连接
		if (SOCKET_ERROR == setsockopt(s_conn, SOL_SOCKET, SO_SNDTIMEO, (char*)&TimeOut, sizeof(TimeOut)) 
			||  SOCKET_ERROR == setsockopt(s_conn, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut)) 
			||  SOCKET_ERROR == ioctlsocket(s_conn, FIONBIO, (unsigned long*)&ul))
			return -1;
		//		if (SOCKET_ERROR == ioctlsocket(s_conn, FIONBIO, (unsigned long*)&ul))
		//			return -1;

		connect(s_conn, (sockaddr*)&addr, sizeof(addr));
		timeval timeout;
		fd_set r;

		FD_ZERO(&r);
		FD_SET(s_conn, &r);
		timeout.tv_sec  = nTimeOut;
		timeout.tv_usec = 0;
		if (select(0, 0, &r, 0, &timeout) <= 0)
			closesocket(s_conn);
		else
			break;

		if (nReConnTime - 1 == i)
		{
			return -1;
		}
	}

	if (!bStop)
		return -1;

	//设置阻塞方式连接
	unsigned long ul1 = 0;
	if(SOCKET_ERROR == ioctlsocket(s_conn, FIONBIO, (unsigned long*)&ul1))
	{
		closesocket(s_conn);
		return -1;
	}

	return 0;
}

tstring CalcMD5( tstring str )
{
	MD5_CTX ctx;  
	unsigned char digest[16] = {0}; 

	MD5_Init(&ctx);
	MD5_Update (&ctx, str.c_str(), str.length());  
	MD5_Final (digest, &ctx);  

	char buf[33] = {0};  
	char tmp[3] = {0};  

	for(int i = 0; i < 16; i++ )  
	{  
		sprintf(tmp,"%02x", digest[i]);
		strcat(buf, tmp);
	}  

	return Ansi2Str(buf);
}

tstring CalcFileMD5(tstring strFilePath)
{
	FILE* fp = NULL;

	errno_t err = _tfopen_s(&fp, strFilePath.c_str(), _T("rb"));
	if( err != 0 )
		return _T("");

	fseek(fp, 0, SEEK_END);
	int nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pBuffer = new char[nSize];
	if( pBuffer == NULL )
		return _T("");


	fread(pBuffer, nSize, 1, fp);
	fclose(fp);

	MD5_CTX ctx;  
	unsigned char digest[16] = {0}; 

	MD5_Init(&ctx);
	MD5_Update (&ctx, pBuffer, nSize);  
	MD5_Final (digest, &ctx);  

	delete pBuffer;

	char buf[33] = {0};  
	char tmp[3] = {0};  

	for(int i = 0; i < 16; i++ )  
	{  
		sprintf(tmp,"%02x", digest[i]);
		strcat(buf, tmp);
	}  

	return Ansi2Str(buf);
}

BOOL ChangeWindowMessageFilterForWin8()
{
	OSVERSIONINFOEX verinfo;
	ZeroMemory(&verinfo, sizeof(OSVERSIONINFOEX));
	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((LPOSVERSIONINFO)(&verinfo)))
		return FALSE;

	if ((verinfo.dwMajorVersion ==6 && verinfo.dwMinorVersion == 2) || // Windows 8
		(verinfo.dwMajorVersion ==6 && verinfo.dwMinorVersion == 3)    // Windows 8.1
		)
	{
		HMODULE hModule = LoadLibrary(_T("user32.dll"));
		if (!hModule)
		{
			return FALSE;
		}


#define MSGFLT_ADD 1
#define MSGFLT_REMOVE 2
#define WM_COPYGLOBALDATA 0x0049

		typedef BOOL (WINAPI* PFN_ChangeWindowMessageFilter)(UINT message, DWORD dwFlag);


		PFN_ChangeWindowMessageFilter pfnChangeWindowMessageFilter = 
			(PFN_ChangeWindowMessageFilter)GetProcAddress(hModule, "ChangeWindowMessageFilter");

		if (!pfnChangeWindowMessageFilter)
		{
			return FALSE;
		}

		pfnChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		pfnChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		pfnChangeWindowMessageFilter(WM_COPYGLOBALDATA , MSGFLT_ADD);

		FreeLibrary(hModule);
		
	}
	return FALSE;
}

