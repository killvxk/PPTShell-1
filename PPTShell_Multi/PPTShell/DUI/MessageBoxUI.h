#pragma once
#include "DUICommon.h"
#include "DUI/WndShadow.h"
#include "Util/Singleton.h"

#define ID_MSGBOX_BTN  50

class CMessageBoxUI : public WindowImplBase
{
public:
	CMessageBoxUI(void);
	~CMessageBoxUI(void);

	typedef enum _tagMessageBoxType
	{
		enMessageBoxTypeSucceed = 0,
		enMessageBoxTypeInfo,
		enMessageBoxTypeWarn,
		enMessageBoxTypeError,
		enMessageBoxTypeQuestion
	}ENMessageBoxType;

	UINT MessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption,  LPCTSTR lpszBtnType, ENMessageBoxType enIconType , UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150, bool bDoModal=true, LPCTSTR lpszId=NULL);
	UINT MessageBox(HWND hParent, LPCTSTR lpszMainText, DWORD dwMainTextColor, int nMainTextFont,
		LPCTSTR lpszText, DWORD dwTextColor, int nTextFont,
		LPCTSTR lpszCaption, LPCTSTR lpszBtnType,
		CMessageBoxUI::ENMessageBoxType enIconType = CMessageBoxUI::enMessageBoxTypeInfo,
		UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150,bool bDoModal = true ,LPCTSTR lpszId  = NULL );

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void Notify(TNotifyUI& msg);

	void		OnBtnClose(TNotifyUI& msg);
	CControlUI * GetItem(LPCTSTR lpszName);
	void		SetState(ENMessageBoxType eMessageType);

private:
	CVerticalLayoutUI*	m_pVerticalLayout;
	CLabelUI*			m_pMessageTitle;
	CLabelUI*			m_pMessageContent;
	CControlUI*			m_pMessageType;
	CControlUI*			m_pMessageCenterHeight;
	CControlUI*			m_pMessageHeight;
	CControlUI*			m_pMessageWidth;
	int					m_nDefaultButtonIndex;
	CWndShadow			m_WndShadow;
};

typedef Singleton<CMessageBoxUI>	MessageBoxUI;

UINT  UIMessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUI::ENMessageBoxType enIconType = CMessageBoxUI::enMessageBoxTypeInfo, UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 430, int nHeight = 250,bool bDoModal = true ,LPCTSTR lpszId  = NULL );
UINT  UIMessageBox(HWND hParent, LPCTSTR lpszMainText, DWORD dwMainTextColor, int nMainTextFont,
				   LPCTSTR lpszText, DWORD dwTextColor, int nTextFont,
				   LPCTSTR lpszCaption, LPCTSTR lpszBtnType,
				   CMessageBoxUI::ENMessageBoxType enIconType = CMessageBoxUI::enMessageBoxTypeInfo,
				   UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 430, int nHeight = 250,bool bDoModal = true ,LPCTSTR lpszId  = NULL );