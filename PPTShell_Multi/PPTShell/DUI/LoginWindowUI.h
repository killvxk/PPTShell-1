#pragma once

#include "DUICommon.h"
#include "DUI/EditClear.h"
#include "NDCloud/NDCloudAPI.h"
#include "Util/Stream.h"

class CLoginWindowUI : public CContainerUI
{
public:
	CLoginWindowUI();
	~CLoginWindowUI();

private:
	CEditClearUI*		m_pUserNameEdit;
	CEditClearUI*		m_pPasswordEdit;
	CLabelUI*			m_pErrorTipUserNameLabel;
	CLabelUI*			m_pErrorTipPasswordLabel;
	CLabelUI*			m_pErrorTipNetLabel;
	CButtonUI*			m_pLoginBtn;
	
	CLabelUI*			m_pLoginTip;

	CCheckBoxUI*		m_pSavePasswordUI;
	CCheckBoxUI*		m_pAutomaticLoginUI;
	

	HWND				m_hMainWnd;

	CButtonUI*			m_pQQLoginBtn;
	CButtonUI*			m_pWeiboLoginBtn;
	CButtonUI*			m_pNdLoginBtn;

	bool				m_bLogining;
public:

	tstring				m_strSectionCode;
	tstring				m_strGradeCode;
	tstring				m_strCourseCode;
	tstring				m_strEditionCode;
	tstring				m_strSubEditionCode;
	tstring				m_strChapterGuid;
	tstring				m_strGrade;

	CChapterTree*		m_pChapterTree; 

	CVerticalLayoutUI*	m_pLoginTipLayout;

	bool				m_bSetChapter;
private:
	bool				OnUCLoginSaveStatus(void * pParam);
	bool				OnGetCourse(void * pParam);
	bool				OnGetEdition(void * pParam);
	bool				OnGetBookGUID(void * pParam);
	bool				OnGetChapter(void * pParam);

	void				SetLoginBtnEnable( bool bEnable);

	void				HideLoginTip();
	void				ShowLoginTip(int nType,tstring strTip);
	void				ShowLoginComplete();

	bool				OnUserLogin(void * pParam);

	void				CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int  c = -1);

	bool				OnLoginEvent(void * pParam);
	

public:
	void				Init(HWND hMainWnd);
	void				Login();
	void				MobileLogin(CStream *loginStream);
	void				SetCheckBoxAutoLogin();
	void				SetCheckBoxSavePassword();
	void				NotifyTabToEdit();
	void				OnEditTabChangeLogin(CControlUI * pControl);
	bool				GetLoginStatus();
	void				ClearEditStatus();
};