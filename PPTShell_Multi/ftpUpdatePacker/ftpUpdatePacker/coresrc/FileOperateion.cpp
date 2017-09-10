#include "stdafx.h"
#include "FileOperateion.h"
#include <string>
using std::string;
using std::wstring;
#include <direct.h>


#ifdef _UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif
CFileOperateion::CFileOperateion()
{

}

CFileOperateion::~CFileOperateion()
{

}

bool CFileOperateion::IsDirectoryExist( LPCTSTR lpPath )
{
	DWORD dwAttri = ::GetFileAttributes(lpPath);
	if ((dwAttri != -1) && (dwAttri & FILE_ATTRIBUTE_DIRECTORY))
		return true;
	return false;
}

bool CFileOperateion::IsFileExist( LPCTSTR lpPath )
{
	DWORD dwAttri = ::GetFileAttributes(lpPath);
	if ((dwAttri != -1) && !(dwAttri & FILE_ATTRIBUTE_DIRECTORY))
		return true;
	return false;
}

bool CFileOperateion::CreateDirectory( LPCTSTR lpPath )
{
	int nLen = _tcslen(lpPath);
	if ( nLen <= 3)
	{
		return false;
	}
	if (IsDirectoryExist(lpPath))
	{
		return true;
	}
	LPTSTR	pDot		= (LPTSTR)_tcsrchr(lpPath, _T('.'));
	LPTSTR	pBackslash	= (LPTSTR)_tcsrchr(lpPath, _T('\\'));
	if (!pBackslash)
	{
		return false;
	}
	int nPos				= pBackslash - lpPath;
	TCHAR szDir[MAX_PATH]	= { 0 };
	if (pDot && (pDot -pBackslash) > 0)
	{
		nPos = pBackslash - lpPath;
		_tcsncpy_s(szDir, lpPath, nPos);
		_tcscat_s(szDir, _T("\\"));
		goto __create;
	}
	else
	{
		if (nPos == nLen - 1)
		{
			_tcscpy_s(szDir, lpPath);
			goto __create;
		}
	}
	_tcscpy_s(szDir, lpPath);
	_tcscat_s(szDir, _T("\\"));
__create:
	{
		TCHAR	chLast;	
		LPTSTR	pCurrent	= szDir;
		LPTSTR	pHead		= NULL;
		LPTSTR	pBackSlant	= _tcschr(pCurrent, _T('\\'));

		pCurrent = pBackSlant + 1;
		pHead = szDir;
		while(pCurrent)
		{
			pBackSlant	= _tcschr(pCurrent, _T('\\'));
			if (pBackSlant == NULL)
			{
				break;
			}
			chLast = *pBackSlant;
			*pBackSlant = _T('\0');
			if (!CFileOperateion::IsDirectoryExist(pHead))
			{
				if (_tmkdir(pHead))
				{
					return false;
				}
			}
			
			pCurrent = pBackSlant + 1;
			*pBackSlant = chLast;

		}
	}
	return true;
}

bool CFileOperateion::DeleteDirectory( LPCTSTR lpPath )
{
	if (!IsDirectoryExist(lpPath))
	{
		return true;
	}

	tstring strPath = lpPath;
	tstring strFind = _T("");
	tstring	strDir	= _T("");
	int		nLen	= strPath.length();

	if (strPath.at(nLen - 1) == _T('\\'))
	{
		strFind = strPath + _T("*");
		strDir	= strPath;
	}
	else
	{
		strFind = strPath + _T("\\*");
		strDir	= strPath + _T("\\");
	}

	WIN32_FIND_DATA fd;

	HANDLE hFindFile = FindFirstFile(strFind.c_str(), &fd);
	if (INVALID_HANDLE_VALUE != hFindFile)
	{
		bool bFinish = false;
		while (!bFinish)
		{
			bool bDir = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

			tstring strFullPath = strDir + fd.cFileName;
			if (bDir)
			{
				if (_tcscmp(fd.cFileName, _T("."))==0 || _tcscmp(fd.cFileName, _T(".."))==0)
				{
					bFinish  = (FindNextFile(hFindFile, &fd) == false);
					continue;
				}            
				else
				{
					DeleteDirectory(strFullPath.c_str());
				}
			}
			else
				::DeleteFile(strFullPath.c_str());

			bFinish  = (FindNextFile(hFindFile, &fd) == false);
		}
	}

	FindClose(hFindFile);

	if (!::RemoveDirectory(strDir.c_str()))
		return false;

	return true;
}

bool CFileOperateion::CreatePath( LPCTSTR lpPath )
{
// 	int nLen = _tcslen(lpPath);
// 	if ( nLen <= 3)
// 	{
// 		return false;
// 	}
// 	if (IsFileExist(lpPath))
// 	{
// 		return true;
// 	}
// 	CreateDirectory(lpPath);
	return false;

}
