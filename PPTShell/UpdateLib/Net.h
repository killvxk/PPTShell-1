// Net.h: interface for the CNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NET_H__2560346E_FA81_4A77_9540_1E1F8C901276__INCLUDED_)
#define AFX_NET_H__2560346E_FA81_4A77_9540_1E1F8C901276__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"

class CNet  
{
public:
	CNet();
	virtual ~CNet();

public:
	BOOL DownFile(char* szUrl, char* szPath, char* szFile, char** ppBuff=NULL, void* p=NULL);
	BOOL DownFile(char* szUrl, char* szPath, void* p=NULL);
	BOOL DownData(char* szUrl, char* szPath, char* szFile, char** ppBuff=NULL, void* p=NULL);
	LONG GetNetFileSize(char* szUrl);

	void SetDownloadSize(HANDLE hFile, LONGLONG& lSize, LONGLONG& lDownloadSize);

public:
	bool m_bInit;
	HANDLE m_hTempFile;
	HANDLE m_hConfFile;
};

#endif // !defined(AFX_NET_H__2560346E_FA81_4A77_9540_1E1F8C901276__INCLUDED_)
