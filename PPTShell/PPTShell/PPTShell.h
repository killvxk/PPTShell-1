
// PPTShell.h : main header file for the PPTShell application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "Util/Util.h"
#include "Util/Stream.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTInfoSender.h"
#include "PacketProcess/Packet.h"
#include "PacketProcess/PacketProcessor.h"
#include "Network/BaseClient.h"
#include "Network/WANClient.h"
#include "Network/BaseServer.h"
#include "Network/LANServer.h"
#include "Network/BlueToothServer.h"
#include "Network/WANClientManager.h"
#include "UserManager/User.h"
#include "UserManager/UserManager.h"
#include "FileTransfer/FileTransferTask.h"
#include "FileTransfer/FileTransferManager.h"
#include "Update/UpdateOperation.h"
#include "Common.h"


// CPPTShellApp:
// See PPTShell.cpp for the implementation of this class
//

class CPPTShellApp : public CWinApp
{
public:
	CPPTShellApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int	 ExitInstance();
	
	void	SetCurrentPPTFileName(LPCTSTR szFileName)	{ _tcscpy_s(m_szPPTFileName, szFileName);		}
	tstring GetCurrentPPTFileName()						{ return m_szPPTFileName;					}

	void	SetCurrentPPTPath(LPCTSTR szPath)			{ m_strLoadFile = szPath;					}
	tstring GetCurrentPPTPath()							{ return m_strLoadFile;						}
	BOOL	IsFromNetwork()								{ BOOL res = m_bFromNet; m_bFromNet = FALSE; return res; }


	BOOL	SendPacket(SOCKET sock, char* pData, int nDataSize, int nConnectType = 0);
	BOOL	SendStreamPacket(SOCKET sock, CStream* pStream, int nConnectType = 0);

	
	BOOL	SetPacketSentNotifyFunc(SOCKET sock ,int nConnectType, PVOID pPacketSentNotifyFunc);

	BOOL	GeneratePPTID();
	DWORD	GetPPTID();

	// й╣ож
	UINT	m_nAppLook;
	BOOL	m_bHiColorIcons;
	BOOL	m_bFixedIME;
	BOOL	m_bDBClickOpen;

	//virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();

// Implementation
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
protected:
	void RegisterURLProtocol(TCHAR *pszProtocal, TCHAR *pszExePath);
	void RegisterOpenMode();
	void SetNdpxOpenMode();
	BOOL CheckSelfRelation(const TCHAR *strAppKey);
	BOOL CheckFileRelation(const TCHAR *strExt, const TCHAR *strAppKey);
	void RegisterPPTShellUrlProtocol();
	void RegisterFileRelation(TCHAR *strExt, TCHAR *strAppName, 
		TCHAR *strAppKey, TCHAR *strDefaultIcon, TCHAR *strDescribe);

	void ModifyUninstallVersion();

protected:
	CUpdate*					m_pUpdate;
	TCHAR						m_szPPTFileName[MAX_PATH];
	tstring						m_strLoadFile;
	DWORD						m_dwPPTID;
	BOOL						m_bFromNet;

};

extern CPPTShellApp theApp;
