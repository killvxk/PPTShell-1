#pragma once
#include "DUICommon.h"
#include "DUI/WndShadow.h"
#include "Util/Singleton.h"

#define ID_MSGBOX_BTN  50

class CMessageBoxUIEx2 : public WindowImplBase
{
public:
	CMessageBoxUIEx2(void);
	~CMessageBoxUIEx2(void);

	typedef enum _tagMessageBoxType
	{
		enMessageBoxTypeSucceed = 0,
		enMessageBoxTypeInfo,
		enMessageBoxTypeWarn,
		enMessageBoxTypeError,
		enMessageBoxTypeQuestion
	}ENMessageBoxTypeEx2;

	UINT MessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption,  LPCTSTR lpszBtnType, ENMessageBoxTypeEx2 enIconType , UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150, bool bDoModal=true, LPCTSTR lpszId=NULL);
	UINT MessageBox(HWND hParent, LPCTSTR lpszMainText, DWORD dwMainTextColor, int nMainTextFont,
		LPCTSTR lpszText, DWORD dwTextColor, int nTextFont,
		LPCTSTR lpszCaption, LPCTSTR lpszBtnType,
		CMessageBoxUIEx2::ENMessageBoxTypeEx2 enIconType = CMessageBoxUIEx2::enMessageBoxTypeInfo,
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
	void		SetState(ENMessageBoxTypeEx2 eMessageType);

private:
	CVerticalLayoutUI*	m_pVerticalLayout;
	CLabelUI*			m_pMessageTitle;
	CLabelUI*			m_pMessageContent;
	CControlUI*			m_pMessageType;
	int					m_nDefaultButtonIndex;
	CWndShadow			m_WndShadow;
};

typedef Singleton<CMessageBoxUIEx2>	MessageBoxUIEx2;

UINT  UIMessageBoxEx2(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUIEx2::ENMessageBoxTypeEx2 enIconType = CMessageBoxUIEx2::enMessageBoxTypeInfo, UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150,bool bDoModal = true ,LPCTSTR lpszId  = NULL );
UINT  UIMessageBoxEx2(HWND hParent, LPCTSTR lpszMainText, DWORD dwMainTextColor, int nMainTextFont,
				   LPCTSTR lpszText, DWORD dwTextColor, int nTextFont,
				   LPCTSTR lpszCaption, LPCTSTR lpszBtnType,
				   CMessageBoxUIEx2::ENMessageBoxTypeEx2 enIconType = CMessageBoxUIEx2::enMessageBoxTypeInfo,
				   UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150,bool bDoModal = true ,LPCTSTR lpszId  = NULL );