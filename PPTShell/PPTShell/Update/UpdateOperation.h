// Update.h: interface for the CUpdate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPDATE_H__3B438198_4FBF_4557_9900_91288415823F__INCLUDED_)
#define AFX_UPDATE_H__3B438198_4FBF_4557_9900_91288415823F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Http/HttpDelegate.h"
#include "ThirdParty/json/json.h"

#define UPDATE_EXE			_T("Update.exe")


typedef struct _STDIO
{
	HANDLE	hInPipe;
	HANDLE	hOutPipe;

	HANDLE	hProcess;
	void*	pDlg;
	void*	pAboutDlg;
	bool	isAboutDlg;
}STDIO, *PSTDIO;


enum UpdateType
{
	eUpdateType_Has_Update = 1,
	eUpdateType_Newset,
	eUpdateType_Failed,
	eUpdateType_Request_Update,
	eUpdateType_Terminate,
	eUpdateType_Update_Self,
	eUpdateType_Update_Success,
	eUpdateType_Update_Processing,
};

typedef struct _PipeMessageHead
{
	short	wType;
	short	wSize;

	_PipeMessageHead()
	{
		wType		= -1;
		wSize		= sizeof(_PipeMessageHead);
	}

}PipeMessageHead,*PPipeMessageHead;

typedef struct _HasUpdate:PipeMessageHead
{
	bool	bValue;
	char	szVersion[32];
	char	szContent[MAX_PATH * 2];
	_HasUpdate()
	{
		wType		= eUpdateType_Has_Update;
		wSize		= sizeof(_HasUpdate);
	}

}HasUpdate,*PHasUpdate;

typedef struct _ReuqestUpdate:PipeMessageHead
{
	bool	bValue;
	_ReuqestUpdate()
	{
		wType		= eUpdateType_Request_Update;
		wSize		= sizeof(_ReuqestUpdate);
	}

}ReuqestUpdate,*PReuqestUpdate;


typedef struct _VersionStruct
{
	int aVersion[4];
	_VersionStruct()
	{
		for (int i = 0; i < 4; i++)
		{
			aVersion[i] = 0;
		}
	}
	int CompareTo(_VersionStruct* pVersion)
	{
		for (int i = 4 - 1; i >= 0; i--)
		{
			if (this->aVersion[ i ] > pVersion->aVersion[ i ])
			{
				return 1;
			}
			else if (this->aVersion[ i ] == pVersion->aVersion[ i ])
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		return 0;
	}

	bool	InitFromString(LPCTSTR	lpStr)
	{
		if(_stscanf_s(lpStr, _T("%d.%d.%d.%d"), &aVersion[3], &aVersion[2], &aVersion[1], &aVersion[0]) == 4)
			return true;
		return false;
	}

}VersionStruct;

typedef struct _UpdateInfo_Param
{
	int				nType;
	int				nRet;
	DWORD			dwTotalProgress;
	DWORD			dwCurrentProgress;
	char			szFileName[MAX_PATH];
	float			fDownloadSpeed;
}UpdateInfo_Param,*pUpdateInfo_Param;

#define NOT_UPDATE_STATUS 0
#define UPDATE_STATUS 1
class CUpdate  
{
public:
	CUpdate();
	virtual ~CUpdate();

private:
	bool	m_bInit;
	
	HANDLE hThread;
public:
	void    StartUpdate();
	void    StartCover(tstring strPath = _T(""));
	bool	CheckUpdate();
	tstring GetServerVersion();
	tstring GetUpdateLog();
	void	GetUpdateInfo();

	void	CancelUpdate();
	void	BroadcastUpdate(int nType);

	bool	OnGetServerKeyWord(void * pParam);
	bool	UpdateServerKeyWord();

	Json::Value		GetServerKeyWord();

	bool	OnGetNewestVersionUrl(void * pParam);
	bool	OnGetNewestVersionIP(void * pParam);
	bool	OnGetUpdateLog(void * pParam);

	void	SetNeedUpdate(bool b) {m_bIsNeedUpdate = b;}
	bool	GetNeedUpdate() {return m_bIsNeedUpdate; }

	void SetExit();
	
	UpdateInfo_Param* GetUpdateInfoParam() { return &m_UpdateInfo;}
public:
	void* m_pMainDlg;
	void* m_pAboutDlg;

	bool m_bCheckAbout;

	CEventSource m_OnNotify;

	STDIO	m_io;
	
	
private:
	bool	m_bIsNeedUpdate;

	DWORD m_dwUpdateDownloadId;
	DWORD m_dwKeywordDownloadId;
	tstring m_strServerVersion;
	tstring m_strUpdateLog;

	Json::Value		m_serverKeyword;


//	HANDLE			m_hUpdateEvent;
	HANDLE			m_hFileMapping;
	BYTE*			m_pUpdateBuff;

	UpdateInfo_Param m_UpdateInfo;

	HANDLE m_hEventStop;
};

#endif // !defined(AFX_UPDATE_H__3B438198_4FBF_4557_9900_91288415823F__INCLUDED_)
