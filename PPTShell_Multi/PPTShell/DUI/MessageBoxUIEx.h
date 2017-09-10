#pragma once
#include "DUICommon.h"
#include "DUI/WndShadow.h"

#define ID_MSGBOX_BTN  50

class CMessageBoxUIEx : public WindowImplBase
{
public:
	CMessageBoxUIEx(void);
	~CMessageBoxUIEx(void);

	typedef enum _tagMessageBoxType
	{
		enMessageBoxTypeSucceed = 0,
		enMessageBoxTypeInfo,
		enMessageBoxTypeWarn,
		enMessageBoxTypeError,
		enMessageBoxTypeQuestion,
		enMessageBoxTypeCenterPic//2015.11.03 Í¼Ïñ¾ÓÖÐ CWS
	}ENMessageBoxTypeEx;

	UINT MessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption,  LPCTSTR lpszBtnType, ENMessageBoxTypeEx enIconType , UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150, bool bDoModal=true, LPCTSTR lpszId=NULL);

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void Notify(TNotifyUI& msg);

	void		OnBtnClose(TNotifyUI& msg);
	CControlUI * GetItem(LPCTSTR lpszName);
	void		SetState(ENMessageBoxTypeEx eMessageType); 
private:
	CVerticalLayoutUI*	m_pVerticalLayout;
	CVerticalLayoutUI*	m_pLayoutHeight;//2015.11.04 cws
	CVerticalLayoutUI*	m_pLayoutCenterHeight;//2015.11.04 cws
	CLabelUI*			m_pMessageTitle;
	CLabelUI*			m_pMessageContent;
	CControlUI*			m_pMessageType;
	int					m_nDefaultButtonIndex;
	CWndShadow			m_WndShadow;
};

UINT  UIMessageBoxEx(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUIEx::ENMessageBoxTypeEx enIconType = CMessageBoxUIEx::enMessageBoxTypeInfo, UINT nDefaultButtonIndex = 0, LPCTSTR lpszStyleId = NULL, int nWidth = 340, int nHeight = 150,bool bDoModal = true ,LPCTSTR lpszId  = NULL );

