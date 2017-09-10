#ifndef _UTIL_H_
#define _UTIL_H_

#include <winsock2.h>

#include <string>
using namespace std;

//下载类型
#define DOWN_TYPE_NULL		-1
#define DOWN_TYPE_HTTP		1
#define DOWN_TYPE_FTP		2

//连接相关配置
#define TIMEOUT				10	//超时时间
#define RE_CONN_TIME		2	//重连次数

//缓冲区配置
#define URL_LEN_MAX			2048

//自定义消息
#define WM_DOWNMESSAGE (WM_USER+300)

#define UPDATE_TEMP_DIR		"temp\\"

typedef struct _DownInfo
{
	BOOL		bIsDown;
	int			nPer;
	LONG		nDownSize;
	LONG		nFileSize;
	int			nID;
	
	_DownInfo() : bIsDown(FALSE), nPer(0), nDownSize(0), nFileSize(-1), nID(0) {}
}DownInfo, *PDownInfo;

//字符串
int FindChar_f(LPSTR t, LPSTR s);
int FindChar_b(LPSTR t, LPSTR s);
int FindChar_n(LPSTR t, LPSTR s, int n);

//网络
int JudgeProtocol(LPCSTR szUrl);
int ConnWithTime(SOCKET& s_conn, const char* szServ, const int nPort);

//HTTP
int SplitHttpUrl(char* szUrl, char* szServ, int& nPort, char* szPath);
LONG HttpSize(char* szUrl);
int HttpSend(SOCKET& s_conn, const char* szServ, const char* szPath, const LONG nLow=0, const LONG nRange=0);
LONGLONG GetNetFileSize(char* szUrl, PLONG plStop);

//安全
BOOL CheckApiInLineHook(PVOID ApiAddress);
BOOL RemoveHook(LPCSTR szDllPath, LPCSTR szFuncName);

//辅助
bool JudgeDir(char* szPath);
void print(const char *fmt, ...);

//目录操作
string GetParentPath(const string& strFileName);
bool IsDir(const string& strFileName);
bool DirectoryMake(const string& strDirectory);
bool MiscMyMoveFile(const char* pcszSrcFile,const char* pcszDstFile);
bool MiscMyDeleteFile(const char* pcszFileName);

string GetFileName(const string& strFileName);
string StrReplace(string s, string oldSub, string newSub);
// 删除目录
bool DirectoryDelete(const string& strDirectory);
// 取文件名
string GetFileTitle(const string& strFileName);
bool IsFileExist(const string& strFileName);

// 判断是否可以读该文件
bool FileReadAllow(const string& strFileName);
unsigned __int64 GetFileSize(const string& strFileName);

BOOL MakeDir(const char* pszDir);

bool ChkDir();
bool CheckDat(char* sDatBuff);
bool SafeMove(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
bool isFileExsit(LPTSTR strPath);
//bool DirectoryDelete(char* szDirectory);
bool CreateHashFile(char* szTempPath, vector<UPDATEDATA> &list);
bool CreateHashFile(char* szTempPath, vector<UPDATEDATA> &list, vector<UPDATEDATA> &locallist);
bool CreateHashFile(char* szTempPath, vector<UPDATEDATA> &list, vector<UPDATEDATA> &locallist, vector<string> &vecRedownload, vector<string> &vecCover);

void ANSIToUnicode(char* szBuf, WCHAR *wszBuf);
void UnicodeToANSI(WCHAR *wszBuf, char* szBuf);

DWORD  CalcFileCRC(LPCTSTR lptcsPath);
DWORD  CalcCRC(char* ptr, DWORD Size);

bool CodePageConvert(const string& Source, string& Target, int SourceCodePage, int TargetCodePage);
#endif //_UTIL_H_
