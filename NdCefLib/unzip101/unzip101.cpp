// unzip101.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_DEPRECATE 
#include "stdafx.h"
#include <windows.h>
#include <shellapi.h>
#include <string>
#include "Zip/ZipWrapper.h"
#include "NdCefCmdLine.h"
#include "cefobj.h"

#pragma comment(lib, "shell32.lib")

std::string UnicodeToAnsi( std::wstring unicode )
{
	// unicode --> ansi
	int ansiLen = WideCharToMultiByte(CP_ACP, NULL, unicode.c_str(), unicode.length(), NULL, 0, NULL, NULL);
	char* szAnsi = new char[ansiLen + 1];

	WideCharToMultiByte(CP_ACP, NULL, unicode.c_str(), unicode.length(), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	std::string str = szAnsi;
	delete szAnsi;

	return str;
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
bool FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	bool bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	bool RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}
BOOL DeleteDirectory(LPCTSTR lpszDir)
{
	if (NULL == lpszDir || L'\0' == lpszDir[0])
	{
		return FALSE;
	}

	if (!FindFirstFileExists(lpszDir,false))
	{
		return FALSE;
	}

	WIN32_FIND_DATA wfd = {0};
	TCHAR szFile[MAX_PATH] = {0};
	TCHAR szDelDir[MAX_PATH] = {0};

	lstrcpy(szDelDir, lpszDir);
	if (lpszDir[lstrlen(lpszDir) - 1] != L'\\')
	{
		_snwprintf(szDelDir, _countof(szDelDir) - 1, TEXT("%s\\"), lpszDir);
	}
	else
	{
		wcsncpy(szDelDir, lpszDir, _countof(szDelDir) - 1);
	}

	_snwprintf(szFile, _countof(szFile) - 1, TEXT("%s*.*"), szDelDir);
	HANDLE hFindFile = FindFirstFile(szFile, &wfd);
	if (INVALID_HANDLE_VALUE == hFindFile)
	{
		return FALSE;
	}

	do
	{
		if (lstrcmpi(wfd.cFileName, TEXT(".")) == 0 || lstrcmpi(wfd.cFileName, TEXT("..")) == 0)
		{
			continue;
		}

		_snwprintf(szFile, _countof(szFile) - 1, TEXT("%s%s"), szDelDir, wfd.cFileName);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			DeleteDirectory(szFile);
		}
		else
		{
			DeleteFile(szFile);
		}

	} while (FindNextFile(hFindFile, &wfd));

	FindClose(hFindFile);
	RemoveDirectory(szDelDir);
	return TRUE;
}
int _tmain(int argc, _TCHAR* argv[])
{
	NdCef::CNdCefCmdLine<wstring> argumentList;
	argumentList.parseCmd(argc,argv);
	if (argumentList.size()==0){
		return false;
	}else {
		std::wstring strType = argumentList[0];
		if( strType == _T("unzip")){
			// unzip d:\player.zip d:\player
			if (argumentList.size()==3){
				std::wstring strZipFilePath = argumentList[1];
				std::wstring strUnZipFolder = argumentList[2];
				std::string strZip = UnicodeToAnsi(strZipFilePath.c_str());
				std::string strFolder = UnicodeToAnsi(strUnZipFolder.c_str());

				int iPos = strZip.rfind('\\');
				std::string strKey=strZip.substr(iPos+1);

				// check whether we have unzip before
				std::string strConfigFilePath = strFolder;
				strConfigFilePath += "\\config.ini";

				// compare crc
				char szCrc[MAX_PATH] = {0};
				GetPrivateProfileStringA("crc", strKey.c_str(), "", szCrc, MAX_PATH, strConfigFilePath.c_str());

				DWORD dwSavedCrc = strtoul(szCrc, NULL, 16);
				DWORD dwNewCrc = CalcFileCRC(strZipFilePath.c_str());

				// unzip
				if( dwSavedCrc != dwNewCrc )
				{
					std::wstring strnode_modulesPath = strUnZipFolder;
					strnode_modulesPath += L"\\nodejs\\node_modules";
					DeleteDirectory(strnode_modulesPath.c_str());

					CCefObject nCefObj;
					nCefObj.UnZip(strZip.c_str(), strFolder.c_str());
					//CUnZipper unZipper;
					//unZipper.UnZip(strZip.c_str(), strFolder.c_str());

					sprintf_s(szCrc, "%08lX", dwNewCrc);
					WritePrivateProfileStringA("crc", strKey.c_str(), szCrc, strConfigFilePath.c_str());
				}
				return 0;
			}
		}
	}
	return 0;
}
