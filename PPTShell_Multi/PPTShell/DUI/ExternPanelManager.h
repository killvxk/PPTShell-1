#pragma once

#include "Util/Singleton.h"

class CExternPanelManager
{
public:
	CExternPanelManager(void);
	~CExternPanelManager(void);



public:
	void	ClearPanel();
	void	ClosePanel();
	void	ShowPanel(DWORD dwColor);
	void	SetPointerType(int nPointerType);

private:
	HWND	m_hPenWnd;
	LPVOID	m_pPanelWnd;
};


typedef Singleton<CExternPanelManager> ExternPanelManager;