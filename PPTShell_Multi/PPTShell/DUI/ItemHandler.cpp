#include "StdAfx.h"

#include "Util/Stream.h"
#include "DUI/ItemHandler.h"

CItemHandler::CItemHandler()
{
	m_pHolderHandlerVisitor = NULL;
	m_pTrigger				= NULL;
}	


CItemHandler::~CItemHandler()
{

}

void CItemHandler::DoClick(TNotifyUI*	pNotify)
{
	if (OnClick)
	{
		OnClick(pNotify);
	}
}

void CItemHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	if (OnButtonClick)
	{
		OnButtonClick(pNotify);
	}
}

void CItemHandler::DoMenuClick(TNotifyUI*	pNotify)
{
	if (OnMenuClick)
	{
		OnMenuClick(pNotify);
	}
}

void CItemHandler::DoRClick( TNotifyUI* pNotify )
{
	if (OnRClick)
	{
		OnRClick(pNotify);
	}
}

void CItemHandler::DoKeyDown( TEventUI* pNotify )
{
	if (OnKeyDown)
	{
		OnKeyDown(pNotify);
	}
}

IHandlerVisitor* CItemHandler::GetHolderHandlerVisitor()
{
	return m_pHolderHandlerVisitor;
}

map<DWORD, CItemHandler*> CItemHandler::m_mapHandlers;
CItemHandler* CItemHandler::GetExistedHandler( LPCTSTR lptcsId )
{
	DWORD dwHash = DuiLib::crc_32((unsigned char*)lptcsId, _tcslen(lptcsId) * sizeof(TCHAR));
	map<DWORD, CItemHandler*>::iterator itor = m_mapHandlers.find(dwHash);
	if (itor != m_mapHandlers.end())
	{
		return itor->second;
	}

	return NULL;

}

void CItemHandler::AppendHandler( LPCTSTR lptcsId, CItemHandler* pHandler )
{
	DWORD dwHash = DuiLib::crc_32((unsigned char*)lptcsId, _tcslen(lptcsId) * sizeof(TCHAR));
	m_mapHandlers[dwHash] = pHandler;
}

void CItemHandler::RemoveHandler( LPCTSTR lptcsId )
{
	DWORD dwHash = DuiLib::crc_32((unsigned char*)lptcsId, _tcslen(lptcsId) * sizeof(TCHAR));
	m_mapHandlers.erase(dwHash);
}

bool CItemHandler::OpenAsDefaultExec( LPCTSTR lptcsPath )
{
	int nRetn = (int)ShellExecute(NULL, _T("open"), lptcsPath, NULL, NULL, SW_SHOWNORMAL);
	if (nRetn <= 32)
	{
		nRetn = (int)ShellExecute(NULL, _T("openas"), lptcsPath, NULL, NULL, SW_SHOWNORMAL);
		if (nRetn <= 32)
		{
			tstring strCmd = _T("shell32.dll,OpenAs_RunDLL ");
			strCmd += lptcsPath;
			nRetn = (int)ShellExecute(NULL,
				_T("open"),
				_T("rundll32.exe"),
				strCmd.c_str(),
				NULL, 
				SW_SHOWNORMAL);
			if (nRetn <= 32)
			{
				//_T("无法打开该文件")
				return false;
			}
		}
	}
	return true;
}

CControlUI* CItemHandler::GetTrigger()
{
	return m_pTrigger;
}

void CItemHandler::SetTrigger( CControlUI* pTrigger )
{
	m_pTrigger = pTrigger;
}

int CItemHandler::GetHolderCount()
{
	return m_vctrHolder.size();
}

void CItemHandler::AddHolder( CControlUI* pHolder )
{
	if (GetHolderIndex(pHolder) != -1)
	{
		return;
	}

	m_vctrHolder.push_back(pHolder);

}

void CItemHandler::RemoveHolder( CControlUI* pHolder )
{
	int nIndex = GetHolderIndex(pHolder);
	if (nIndex == -1)
	{
		return;
	}

	std::vector<CControlUI*>::iterator itor = m_vctrHolder.begin() + nIndex;
	m_vctrHolder.erase(itor);

	if (GetTrigger() == pHolder)
	{
		SetTrigger(NULL);
	}
}

int CItemHandler::GetHolderIndex( CControlUI* pHolder )
{
	for (size_t i = 0; i< m_vctrHolder.size(); ++i)
	{
		if (m_vctrHolder.at(i) == pHolder)
		{
			return (int)i;
		}
	}

	return -1;
}

CControlUI* CItemHandler::GetHolderAt( int nIndex )
{
	if (nIndex < 0
		|| nIndex >= GetHolderCount())
	{
		return NULL;
	}

	return m_vctrHolder.at(nIndex);
}
