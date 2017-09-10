#pragma once
#include "DUICommon.h"

enum EPLUS_WND_TYPE
{
	E_WND_FLASH,		//FLASH
	E_WND_COURSE,		//¿Î¼þ
	E_WND_PICTURE,		//Í¼Æ¬
};

#define PIC_PREVIEW_WIDTH 800
#define PIC_PREVIEW_HEIGHT 560



class CPreviewFullUI : public WindowImplBase
{
public:
	CPreviewFullUI(void);
	~CPreviewFullUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("CloseBtn"),			OnBtnClose);
	UIEND_MSG_MAP
public:
	void			InitFullWnd(LPCTSTR lpszImgFile, EPLUS_WND_TYPE eWndType);
	static void ShowPreviewFull(HWND hwnd, EPLUS_WND_TYPE eWndType, LPCTSTR lpszImgFile);
	static CPreviewFullUI* GetInstance(HWND hwnd, EPLUS_WND_TYPE eWndType, LPCTSTR lpszImgFile);
	static void ReleaseInstance();
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);



protected:
	virtual CDuiString	 GetSkinFolder();
	virtual CDuiString	 GetSkinFile();
	virtual LPCTSTR		 GetWindowClassName(void) const;
	virtual void	     InitWindow();

	void				 OnBtnClose(TNotifyUI& msg);

	SIZE			     GetImageFileSize(LPCTSTR lpszFile);
private:
	CVerticalLayoutUI*		m_pVerticalLayout;
	CControlUI*				m_pPreviewBig;
	CControlUI*				m_pPreviewError;
	int						m_nHeight;
	int						m_nWidth;
	static CPreviewFullUI*	m_pInstance;
	bool					m_bHaveWindow;
	EPLUS_WND_TYPE			m_eCurrentType;
	SIZE					m_rImgeSize;

};



