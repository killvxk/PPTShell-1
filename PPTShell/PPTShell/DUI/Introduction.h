#pragma once

#include "DUICommon.h"
#include "DUI/WndShadow.h"
#include "Util/Singleton.h"
#include "DUI/CSliderTabLayoutUI.h"

#define START_ANIM	100
#define STOP_ANIM	101

class CIntroductionDialogUI : public WindowImplBase
{
public:
	CIntroductionDialogUI();
	~CIntroductionDialogUI();

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("closeBtn"),		OnCloseBtn);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("ExperienceBtn"),		OnCloseBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void	Init(CRect& rect);

	void	LoadImage(tstring strImagePath);
	void	LoadImages(vector<tstring>& vecList);

	void				StartAnim(int nIndex = 0);
public:
	CSliderTabLayoutUI*			m_pSliderTabLayout;
	CHorizontalLayoutUI*			m_pSliderBtnLayout;

	int					m_nIndex;
	int					m_nImagesCount;
private:
	void				OnCloseBtn(TNotifyUI& msg);
	
	void				StopAnim();

	bool				OnBtnPageChange(void* pNotify);
	static void  TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

private:
//	CWndShadow					m_WndShadow;

	map<int,CButtonUI*>			m_mapSliderButton;
};

typedef Singleton<CIntroductionDialogUI>		IntroductionDialogUI;