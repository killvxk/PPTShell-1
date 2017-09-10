// Update.cpp: implementation of the CUpdate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpdateOperation.h"
#include "EventCenter/EventDefine.h"
#include <process.h>
#include "Util/Util.h"
#include "NDCloud/NDCloudAPI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
bool g_bRecvUpdateMsg=false;
unsigned __stdcall WaitForUpdateProcess(void* pParam)
{

	PSTDIO io = (PSTDIO)pParam;
	if(io->hProcess==INVALID_HANDLE_VALUE) return 0;
	g_bRecvUpdateMsg=false;
	while(WaitForSingleObject(io->hProcess,INFINITE)==WAIT_OBJECT_0)
	{
		//if(g_bRecvUpdateMsg==false && io->isAboutDlg)		//zcs 2015-08-17
		//{
		//	CWnd * dlg = (CWnd *)io->pAboutDlg;
		//	::PostMessage(dlg->GetSafeHwnd(),WM_USER_NOUPDATE,0,0);
		//}
		break;
	}

	CloseHandle(io->hProcess);
	io->hProcess=INVALID_HANDLE_VALUE;
	return 0;
}
*/
unsigned __stdcall GetUpdateInfoThread(void* pParam)
{
	CUpdate* pUpdate = (CUpdate *)pParam;
	pUpdate->GetUpdateInfo();
//	PSTDIO io = &pUpdate->m_io;
//	HasUpdate	hasUpdate;
// 	DWORD dwReal;
// 	while (ReadFile(io->hInPipe, &hasUpdate, sizeof(HasUpdate), &dwReal, NULL))
// 	{
// 		switch(hasUpdate.wType)
// 		{
// 		case eUpdateType_Update_Success:
// 			{
// 				pUpdate->BroadcastUpdate(eUpdateType_Has_Update);
// 				pUpdate->SetNeedUpdate(true);
// 				goto _Out;
// 			}
// 			break;
// 		case eUpdateType_Newset:
// 		case eUpdateType_Failed:
// 			{
// 				goto _Out;
// 			}
// 			break;
// 		}
// 		Sleep(200);
// 	}
// _Out:
// 	CloseHandle(io->hInPipe);
// 	CloseHandle(io->hOutPipe);
	
	
	return 0;
}

CUpdate::CUpdate() : m_bInit(false)
{
	m_bCheckAbout=false;
	m_dwUpdateDownloadId = 0;
	m_dwKeywordDownloadId = 0;
	m_bIsNeedUpdate = false;
}

CUpdate::~CUpdate()
{
	CancelUpdate();
}

void CUpdate::StartUpdate()
{

	WCHAR szMemory[] = L"101PPTUpdateSharedMemory";
	
	m_hFileMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE,szMemory);
	if( m_hFileMapping == NULL )
	{
		m_hFileMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, szMemory);
		if( m_hFileMapping == NULL )
		{
			BroadcastUpdate(eUpdateType_Failed);
			return ;
		}

		m_pUpdateBuff = (BYTE*)MapViewOfFile(m_hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE , 0, 0, 0x1000);
		if(m_pUpdateBuff == NULL)
		{
			BroadcastUpdate(eUpdateType_Failed);
			return ;
		}

		tstring strUpdatePath = GetLocalPath();
		strUpdatePath += _T("\\update.exe");

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		si.wShowWindow = SW_SHOW;

		tstring strCmd = _T(" selfupdate");

		if (CreateProcess(strUpdatePath.c_str(),(LPTSTR)strCmd.c_str(),NULL,NULL,FALSE,0,NULL,GetLocalPath().c_str(),&si,&pi))
		{
			CloseHandle(pi.hThread);
		}
	}
	else
	{
		m_pUpdateBuff = (BYTE*)MapViewOfFile(m_hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE , 0, 0, 0x1000);
		if(m_pUpdateBuff == NULL)
		{
			BroadcastUpdate(eUpdateType_Failed);
			return ;
		}
	}


	unsigned threadid;
	m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	hThread = (HANDLE)_beginthreadex(NULL, 0, GetUpdateInfoThread, this, 0, &threadid);

}
void CUpdate::SetExit(){
	SetEvent(m_hEventStop);
}
void CUpdate::GetUpdateInfo()
{
	while(true)
	{

		DWORD dwRet = WaitForSingleObject(m_hEventStop, 1000);
		switch(dwRet)
		{
		case WAIT_FAILED:
		case WAIT_OBJECT_0:
			return ;
		case WAIT_TIMEOUT:
			memcpy(&m_UpdateInfo,m_pUpdateBuff,sizeof(UpdateInfo_Param));
			switch(m_UpdateInfo.nType)
			{
			case eUpdateType_Update_Processing:
				{
					BroadcastUpdate(eUpdateType_Update_Processing);
				}
				break;
			case eUpdateType_Update_Success:
				{
					BroadcastUpdate(eUpdateType_Has_Update);
					SetNeedUpdate(true);
					goto _Out;
				}
				break;
			case eUpdateType_Failed:
				{
					BroadcastUpdate(eUpdateType_Failed);
					goto _Out;
				}
			case eUpdateType_Newset:
				{
					goto _Out;
				}
				break;
			}
			break;
		}		
	}
_Out:
	return ;
}

void CUpdate::CancelUpdate()
{
	if( m_dwUpdateDownloadId != 0 )
		NDCloudDownloadCancel(m_dwUpdateDownloadId);
	if( m_dwKeywordDownloadId != 0 )
		NDCloudDownloadCancel(m_dwKeywordDownloadId);
}

bool CUpdate::CheckUpdate()
{
	m_strServerVersion = _T("");

	tstring strPath = GetLocalPath();
	strPath += _T("\\version.dat");

	char szRes[32] = _T("0.0.0.0");
	FILE * fp = fopen(strPath.c_str(),_T("r"));
	if(fp)
	{
		fscanf(fp, _T("%s\n"),szRes);
		fclose(fp);
	}

	TCHAR szUrl[MAX_URL_LEN];
	#ifdef DEVELOP_VERSION
	_stprintf(szUrl, _T("/newpptshell/update/version.dat?id=%08lX"), GetTickCount());
	#else
	_stprintf(szUrl, _T("/final/update/version.dat?id=%08lX"), GetTickCount());
	#endif
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if( pHttpDownloadManager == NULL )
		return false;

	m_dwUpdateDownloadId = pHttpDownloadManager->AddTask(_T("p.101.com"), 
		szUrl, 
		_T(""), 
		_T("GET"), 
		_T(""), 
		80, 
		MakeHttpDelegate(this, &CUpdate::OnGetNewestVersionUrl),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));

	return true;
}

bool CUpdate::OnGetNewestVersionUrl(void * pParam)
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	if (m_dwUpdateDownloadId != pNotify->dwTaskId)
	{
		return false;
	}

	if(pNotify->dwErrorCode != 0)
	{
		
#ifdef DEVELOP_VERSION
		tstring strUrl = _T("/newpptshell/update/version.dat");
#else
		tstring strUrl = _T("/final/update/version.dat");
#endif

		CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
		if( pHttpDownloadManager == NULL )
			return false;

		m_dwUpdateDownloadId = pHttpDownloadManager->AddTask(_T("61.160.40.166"), 
			strUrl.c_str(),
			_T(""), 
			_T("GET"), 
			_T(""), 
			80, 
			MakeHttpDelegate(this, &CUpdate::OnGetNewestVersionIP),
			MakeHttpDelegate(NULL),
			MakeHttpDelegate(NULL));
		return false;
	}

	pNotify->pData[pNotify->nDataSize] = 0;
	m_strServerVersion = pNotify->pData;

	tstring strPath = GetLocalPath();
	strPath += _T("\\version.dat");

	TCHAR szLocalVersion[32] = _T("0.0.0.0");
	FILE * fp = fopen(strPath.c_str(),_T("r"));
	if(fp)
	{
		fscanf(fp, _T("%s\n"),szLocalVersion);
		fclose(fp);
	}

	_VersionStruct versionStruct;
	_VersionStruct serverVersionStruct;
	if(!versionStruct.InitFromString(szLocalVersion))
	{
// 		BroadcastUpdate(UPDATE_STATUS);
// 		return false;
	}
	if(!serverVersionStruct.InitFromString(m_strServerVersion.c_str()))
	{
		BroadcastUpdate(eUpdateType_Failed);
		return false;
	}
	if(versionStruct.CompareTo(&serverVersionStruct) == 0)
	{
		BroadcastUpdate(eUpdateType_Newset);
		return false;
	}

	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if( pHttpDownloadManager == NULL )
		return false;

	TCHAR szUrl[MAX_URL_LEN];
#ifdef DEVELOP_VERSION
	_stprintf(szUrl, _T("/newpptshell/update/update.dat?id=%08lX"), GetTickCount());
#else
	_stprintf(szUrl, _T("/final/update/update.dat?id=%08lX"), GetTickCount());
#endif
	
	m_dwUpdateDownloadId = pHttpDownloadManager->AddTask(_T("p.101.com"), 
		szUrl, 
		_T(""), 
		_T("GET"), 
		_T(""), 
		80, 
		MakeHttpDelegate(this, &CUpdate::OnGetUpdateLog),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));

	return true;
}

bool CUpdate::OnGetNewestVersionIP(void * pParam)
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	if (m_dwUpdateDownloadId != pNotify->dwTaskId)
	{
		return true;
	}else
	{
		//AfxMessageBox("CreateProcess fail");
	}

	if(pNotify->dwErrorCode != 0)
	{
		BroadcastUpdate(eUpdateType_Failed);
		return false;
	}

	pNotify->pData[pNotify->nDataSize] = 0;
	m_strServerVersion = pNotify->pData;

	tstring strPath = GetLocalPath();
	strPath += _T("\\version.dat");

	TCHAR szLocalVersion[32] = _T("0.0.0.0");
	FILE * fp = fopen(strPath.c_str(),_T("r"));
	if(fp)
	{
		fscanf(fp, _T("%s\n"),szLocalVersion);
		fclose(fp);
	}

	_VersionStruct versionStruct;
	_VersionStruct serverVersionStruct;
	if(!versionStruct.InitFromString(szLocalVersion))
	{
// 		BroadcastUpdate(UPDATE_STATUS);
// 		return false;
	}
	if(!serverVersionStruct.InitFromString(m_strServerVersion.c_str()))
	{
		BroadcastUpdate(eUpdateType_Failed);
		return false;
	}
	if(versionStruct.CompareTo(&serverVersionStruct) == 0)
	{
		BroadcastUpdate(eUpdateType_Newset);
		return false;
	}

	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if( pHttpDownloadManager == NULL )
		return false;

	TCHAR szUrl[MAX_URL_LEN];
#ifdef DEVELOP_VERSION
	_stprintf(szUrl, _T("/newpptshell/update/update.dat?id=%08lX"), GetTickCount());
#else
	_stprintf(szUrl, _T("/final/update/update.dat?id=%08lX"), GetTickCount());
#endif
	
	m_dwUpdateDownloadId = pHttpDownloadManager->AddTask(_T("61.160.40.166"), 
		szUrl, 
		_T(""), 
		_T("GET"), 
		_T(""), 
		80, 
		MakeHttpDelegate(this, &CUpdate::OnGetUpdateLog),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));

	return true;
}

bool CUpdate::OnGetUpdateLog(void * pParam)
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	if (m_dwUpdateDownloadId != pNotify->dwTaskId)
	{
		return true;
	}

	if(pNotify->dwErrorCode != 0)
	{
		BroadcastUpdate(eUpdateType_Failed);
		return false;
	}

	pNotify->pData[pNotify->nDataSize] = 0;
	m_strUpdateLog = pNotify->pData;

	StartUpdate();
//	
	return true;
}

tstring CUpdate::GetServerVersion()
{
	return m_strServerVersion;
}

void CUpdate::BroadcastUpdate(int nType)
{
	m_dwUpdateDownloadId = 0;

	if(m_OnNotify)
	{
		TEventNotify Msg;
		Msg.nEventType = nType;
		m_OnNotify(&Msg);
	}
}

tstring CUpdate::GetUpdateLog()
{
	return m_strUpdateLog;
}

bool CUpdate::UpdateServerKeyWord()
{
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if( pHttpDownloadManager == NULL )
		return false;

	tstring strUrl = _T("/101ppt/getKeywords.php");
	m_dwKeywordDownloadId = pHttpDownloadManager->AddTask(_T("p.101.com"), 
		strUrl.c_str(), 
		_T(""), 
		_T("GET"), 
		_T(""), 
		80, 
		MakeHttpDelegate(this, &CUpdate::OnGetServerKeyWord),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));

	return true;
}

bool CUpdate::OnGetServerKeyWord( void * pParam )
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	if (m_dwKeywordDownloadId != pNotify->dwTaskId)
	{
		return true;
	}

	if(pNotify->dwErrorCode != 0)
	{
		return false;
	}

	pNotify->pData[pNotify->nDataSize] = 0;

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	bool res = reader.parse(str.c_str(), m_serverKeyword);
	if( !res )
	{
		return false;
	}

	return true;
}

Json::Value CUpdate::GetServerKeyWord()
{
	return m_serverKeyword;
}

void CUpdate::StartCover(tstring strPath)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow=SW_SHOW;
	ZeroMemory(&pi, sizeof(pi));

	tstring strUpdatePath = GetLocalPath();
	strUpdatePath += _T("\\update.exe");

	tstring strCmd = _T(" cover");
	if(!strPath.empty() && GetFileAttributes(strPath.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		strCmd += _T(" \"");
		strCmd += strPath;
		strCmd += _T("\"");
	}
	else
	{
		strCmd += _T(" \"\"");
	}

	strCmd += _T(" \"");
	strCmd += NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
	strCmd += _T("\"");
	CreateProcess(strUpdatePath.c_str(),(LPTSTR)strCmd.c_str(),NULL,NULL,FALSE,0,NULL,GetLocalPath().c_str(),&si,&pi);
	
}
