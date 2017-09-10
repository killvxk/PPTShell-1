#include "stdafx.h"
#include "LoginWindowUI.h"
#include "NDCloud/NDCloudUser.h"
#include <regex>
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"
#include "DUI/ThirdPartyLogin.h"


CLoginWindowUI::CLoginWindowUI()
{
	m_bSetChapter	= false;
	m_bLogining		= false;
	m_pUserNameEdit = NULL;
	m_pPasswordEdit = NULL;
}

CLoginWindowUI::~CLoginWindowUI()
{
	NDCloudUser::GetInstance()->CancelLogin();
}

void CLoginWindowUI::Init(HWND hMainWnd)
{
	m_hMainWnd = hMainWnd;
	m_pUserNameEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("username")));
	m_pPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("password")));
	m_pUserNameEdit->SetHandleSpecialKeydown(true);
	m_pPasswordEdit->SetHandleSpecialKeydown(true);

	m_pLoginTipLayout = dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("LoginTipLayout")));
	m_pLoginTip = dynamic_cast<CLabelUI*>(m_pLoginTipLayout->FindSubControl(_T("LoginTip")));

	m_pSavePasswordUI = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("SavePassword")));
	m_pAutomaticLoginUI = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("AutomaticLogin")));

	m_pErrorTipUserNameLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_username")));
	m_pErrorTipPasswordLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_password")));
	m_pErrorTipNetLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_net")));

	m_pLoginBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("LoginBtn")));

	TCHAR szUserName[MAX_PATH + 1];
	TCHAR szPassword[MAX_PATH + 1];
	if(GetPassword(szUserName, szPassword))
	{
		m_pUserNameEdit->SetText(szUserName);
		m_pUserNameEdit->SetClearBtn();
		m_pPasswordEdit->SetText(szPassword);
		m_pPasswordEdit->SetClearBtn();
		m_pSavePasswordUI->SetCheck(true);
	}

	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Config.ini");
	TCHAR szBuff[MAX_PATH + 1];
	GetPrivateProfileString(_T("config"), _T("AutoLogin"), _T("false"), szBuff, MAX_PATH, strPath.c_str());
	if(_tcsicmp(szBuff, _T("true")) == 0)
	{
		m_pAutomaticLoginUI->SetCheck(true);
	}

	m_pQQLoginBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("qqLoginBtn")));
	m_pWeiboLoginBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("weiboLoginBtn")));
	m_pNdLoginBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("ndLoginBtn")));

 	m_pQQLoginBtn->OnEvent += MakeDelegate(this, &CLoginWindowUI::OnLoginEvent);
 	m_pWeiboLoginBtn->OnEvent += MakeDelegate(this, &CLoginWindowUI::OnLoginEvent);
 	m_pNdLoginBtn->OnEvent += MakeDelegate(this, &CLoginWindowUI::OnLoginEvent);
}

void CLoginWindowUI::MobileLogin(CStream *loginStream)
{
	HideLoginTip();
	SetLoginBtnEnable(false);
	m_bSetChapter = false;
	NDCloudUser::GetInstance()->MobileLogin(loginStream, MakeHttpDelegate(this, &CLoginWindowUI::OnUserLogin));
	return ;
}

void CLoginWindowUI::Login()
{
//#ifdef _DEBUG
//	ShowLoginComplete();
//	return;
//#endif
	HideLoginTip();

	tstring strUserName = m_pUserNameEdit->GetText();
	tstring strPassword = m_pPasswordEdit->GetText();

	if(strUserName.empty() || strUserName == m_pUserNameEdit->GetTipText())
	{
		ShowLoginTip(0, _T("用户名不能为空"));
		return;
	}
	else if(strPassword.empty() || strPassword == m_pPasswordEdit->GetTipText())
	{
		ShowLoginTip(1, _T("密码不能为空"));
		return;
	}

	SetLoginBtnEnable(false);
	m_bSetChapter	= false;
	m_bLogining		= true;
	NDCloudUser::GetInstance()->Login(strUserName.c_str(), strPassword.c_str(), _T("org_esp_app_prod"), MakeHttpDelegate(this, &CLoginWindowUI::OnUserLogin));
}

void CLoginWindowUI::ShowLoginTip( int nType,tstring strTip )
{
	if(nType == 0)
	{
		m_pErrorTipUserNameLabel->SetText(strTip.c_str());
		m_pErrorTipUserNameLabel->SetVisible(true);
	}
	else if(nType == 1)
	{
		m_pErrorTipPasswordLabel->SetText(strTip.c_str());
		m_pErrorTipPasswordLabel->SetVisible(true);
	}
	else if(nType == 2)
	{
		m_pErrorTipNetLabel->SetText(strTip.c_str());
		m_pErrorTipNetLabel->SetVisible(true);
	}
	m_bLogining		= false;
	SetLoginBtnEnable(true);
}

void CLoginWindowUI::HideLoginTip()
{
	if(m_pErrorTipUserNameLabel->IsVisible())
	{
		m_pErrorTipUserNameLabel->SetVisible(false);
	}
	if(m_pErrorTipPasswordLabel->IsVisible())
	{
		m_pErrorTipPasswordLabel->SetVisible(false);
	}
	if(m_pErrorTipNetLabel->IsVisible())
	{
		m_pErrorTipNetLabel->SetVisible(false);
	}
}


void  CLoginWindowUI::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
{
	if (nFontType == UIFONT_GDI)
	{
		HFONT hFont = GetManager()->GetFont(nFontId);
		HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);

		if ((DT_SINGLELINE & format))
		{
			SIZE size = {0};
			::GetTextExtentExPoint(hdc, lpszText, c == -1 ? _tcslen(lpszText) : c, 0, NULL, NULL, &size);
			rc.right = rc.left + size.cx;
			rc.bottom = rc.top + size.cy;
		}
		else
		{
			format &= ~DT_END_ELLIPSIS;
			format &= ~DT_PATH_ELLIPSIS;
			if (!(DT_SINGLELINE & format)) format |= DT_WORDBREAK | DT_EDITCONTROL;
			::DrawText(hdc, lpszText, c, &rc, format | DT_CALCRECT);
		}
		::SelectObject(hdc, hOldFont);
	}

}

void CLoginWindowUI::ShowLoginComplete()
{
	tstring strUserName = NDCloudUser::GetInstance()->GetUserName();
	tstring strPassword = NDCloudUser::GetInstance()->GetPassword();
	if(m_pSavePasswordUI->GetCheck())//选择保存密码
	{
		SavePassword(strUserName, strPassword);
	}
	else
	{
		RemoveSaveAccount();
		m_pUserNameEdit->SetText(m_pUserNameEdit->GetTipText().c_str());
		m_pUserNameEdit->SetClearBtn();
		m_pPasswordEdit->SetText(m_pPasswordEdit->GetTipText().c_str());
		m_pPasswordEdit->SetClearBtn();
	}

	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Config.ini");
	if(m_pAutomaticLoginUI->GetCheck())
		WritePrivateProfileString(_T("config"), _T("AutoLogin"), _T("true"), strPath.c_str());
	else
		WritePrivateProfileString(_T("config"), _T("AutoLogin"), _T("false"), strPath.c_str());

	tstring strRealUserName = NDCloudUser::GetInstance()->GetRealName();
	TCHAR szBuff[MAX_PATH];
	_stprintf(szBuff, _T("{f 23}{c #11B0A7}%s{/c} {c #6F6F6F}老师{/c} {c #FFFFFF}登录成功{/c}{/f}"), strRealUserName.c_str());
	TCHAR szBuff1[MAX_PATH];
	_stprintf(szBuff1, _T("%s 老师 登录成功"), strRealUserName.c_str());
	m_pLoginTip->SetText(szBuff);

	m_bLogining		= false;
	RECT rcCalc		= {0};
	rcCalc.right	= 600;
	rcCalc.top		= 60;
	CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff1, m_pLoginTip->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
	m_pLoginTip->SetFixedWidth(rcCalc.right - rcCalc.left);

	SetLoginBtnEnable(true);
	m_pLoginTipLayout->SetVisible(true);
	SetTimer(m_hMainWnd, WM_LOGIN_COMPLETE, 1000, (TIMERPROC)CGroupExplorerUI::TimerProcComplete);
}

bool CLoginWindowUI::OnUserLogin( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		ShowLoginTip(2, _T("当前网络不太好，无法登录，请检查网络连接"));
		return false;
	}
	CNDCloudUser* pUser = NDCloudUser::GetInstance();
	int nStep = pUser->GetCurStep();

	switch( nStep )
	{
	case UCSTEP_LOGIN:
		{
			if( pUser->IsSuccess() )
			{
				CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();

				TCHAR szUrl[1024];
				_stprintf(szUrl, _T("/101ppt/getChapter.php?account=%d"), NDCloudUser::GetInstance()->GetUserId());
				if(pHttpDownloadManager)
				{
					pHttpDownloadManager->AddTask(_T("p.101.com"), szUrl, _T(""), _T("Get"), "", 80,MakeHttpDelegate(this, &CLoginWindowUI::OnUCLoginSaveStatus) ,MakeHttpDelegate(NULL), MakeHttpDelegate(NULL) );
				}
			}
			else
			{
				tstring strErrorCode = pUser->GetErrorCode();
				tstring strErrorMessage = pUser->GetErrorMessage();
				ShowLoginTip(1, strErrorMessage);
			}
		}
		break;
	}

	return true;
}

void CLoginWindowUI::SetLoginBtnEnable( bool bEnable )
{
	if(bEnable)
	{
		m_pLoginBtn->SetEnabled(true);
		m_pLoginBtn->SetText(_T("登录"));
	}
	else
	{
		m_pLoginBtn->SetEnabled(false);
		m_pLoginBtn->SetText(_T("登录中..."));
	}
}

void CLoginWindowUI::OnEditTabChangeLogin(CControlUI * pControl)
{
	if(m_pUserNameEdit && pControl == m_pUserNameEdit)
	{
		m_pPasswordEdit->SetFocus();
		m_pPasswordEdit->SetSelAll();
	}
	else if(m_pPasswordEdit && pControl == m_pPasswordEdit)
	{
		m_pUserNameEdit->SetFocus();
		m_pUserNameEdit->SetSelAll();
	}
}

void CLoginWindowUI::NotifyTabToEdit()
{
	CControlUI* pControl = GetManager()->FindControl(_T("loginLayout"));
	if (pControl)
	{
		if(m_pUserNameEdit->IsFocused())
		{
			GetManager()->SendNotify(m_pUserNameEdit, DUI_MSGTYPE_TABSWITCH);
		}
		else if(m_pPasswordEdit->IsFocused())
		{
			GetManager()->SendNotify(m_pPasswordEdit, DUI_MSGTYPE_TABSWITCH);
		}
	}
}

bool CLoginWindowUI::OnUCLoginSaveStatus( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode == 0)
	{
		pNotify->pData[pNotify->nDataSize] = '\0';
		string str = pNotify->pData;
		Json::Reader reader;
		Json::Value root;

		tstring strChapter;
		bool res = reader.parse(str, root);
		if( res )
		{
			if( !root["Chapter"].isNull() )
			{
				strChapter = Utf82Str(root["Chapter"].asCString());

				TCHAR szSectionCode[MAX_PATH]		= _T("");
				TCHAR szGradeCode[MAX_PATH]			= _T("");
				TCHAR szCourseCode[MAX_PATH]		= _T("");
				TCHAR szEditionCode[MAX_PATH]		= _T("");
				TCHAR szSubEditionCode[MAX_PATH]	= _T("");
				TCHAR szChapterGuid[MAX_PATH]	= _T("");
				TCHAR szGrade[MAX_PATH]	= _T("");

				if(_stscanf_s(strChapter.c_str(),_T("%[^/]/%[^/]/%[^/]/%[^/]/%[^/]/%[^/]/%[^/]"), 
					szSectionCode, _countof(szSectionCode) - 1,
					szGradeCode, _countof(szGradeCode) - 1,
					szCourseCode, _countof(szCourseCode) - 1,
					szEditionCode, _countof(szEditionCode) - 1,
					szSubEditionCode,_countof(szSubEditionCode) - 1,
					szChapterGuid, _countof(szChapterGuid) - 1,
					szGrade, _countof(szGrade) - 1) == 7)
				{

					m_strSectionCode = szSectionCode;
					m_strGradeCode = szGradeCode;
					m_strCourseCode = szCourseCode;
					m_strEditionCode = szEditionCode;
					m_strSubEditionCode = szSubEditionCode;
					m_strChapterGuid = szChapterGuid;
					m_strGrade = szGrade;

					CCategoryTree * pCategoryTree = NULL;
					NDCloudGetCategoryTree(pCategoryTree);
					if(pCategoryTree)
					{
						CategoryNode * pGradeNode = pCategoryTree->FindNode(m_strSectionCode,m_strGradeCode, _T(""), _T(""));

						if(pGradeNode && pGradeNode->pFirstChild)
						{
							pGradeNode = pCategoryTree->FindNode(m_strSectionCode,m_strGradeCode,m_strCourseCode, _T(""));
							if(pGradeNode && pGradeNode->pFirstChild)
							{
								tstring strUrl = NDCloudComposeUrlBookInfo(
									m_strSectionCode.c_str(),
									m_strGradeCode.c_str(),
									m_strCourseCode.c_str(),
									m_strEditionCode.c_str(),
									m_strSubEditionCode.c_str(),
									0,
									20
									);
								NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetBookGUID));
							}
							else
							{
								tstring strUrl = NDCloudComposeUrlCategory(m_strGradeCode,m_strCourseCode);
								NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetEdition));
								return true;
							}
						}
						else
						{
							tstring strUrl = NDCloudComposeUrlCategory(m_strGradeCode);
							NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetCourse));	
						}
					}

					return true;
				}

			}
		}
	}
	
	ShowLoginComplete();

	return true;
}


bool CLoginWindowUI::OnGetCourse( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode == 0)
	{
		BOOL bRet = NDCloudDecodeCategory(pNotify->pData, pNotify->nDataSize, m_strSectionCode, m_strGradeCode, _T(""), _T(""));
		if(bRet)
		{
			CCategoryTree * pCategoryTree = NULL;
			NDCloudGetCategoryTree(pCategoryTree);

			CategoryNode * pGradeNode = pCategoryTree->FindNode(m_strSectionCode,m_strGradeCode,m_strCourseCode, _T(""));
			if(pGradeNode && pGradeNode->pFirstChild)
			{
				tstring strUrl = NDCloudComposeUrlBookInfo(
					m_strSectionCode.c_str(),
					m_strGradeCode.c_str(),
					m_strCourseCode.c_str(),
					m_strEditionCode.c_str(),
					m_strSubEditionCode.c_str(),
					0,
					20
					);
				NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetBookGUID));
			}
			else
			{
				tstring strUrl = NDCloudComposeUrlCategory(m_strGradeCode,m_strCourseCode);

				NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetEdition));
			}
			
			return true;
		}
	}

	ShowLoginComplete();

	return true;
}

bool CLoginWindowUI::OnGetEdition( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode == 0)
	{
		BOOL bRet = NDCloudDecodeCategory(pNotify->pData, pNotify->nDataSize,m_strSectionCode,m_strGradeCode,m_strCourseCode, _T(""));

		if(bRet)
		{
			tstring strUrl = NDCloudComposeUrlBookInfo(
				m_strSectionCode.c_str(),
				m_strGradeCode.c_str(),
				m_strCourseCode.c_str(),
				m_strEditionCode.c_str(),
				m_strSubEditionCode.c_str(),
				0,
				20
				);

			NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetBookGUID));
			return true;
		}
	}

	ShowLoginComplete();

	return true;
}

bool CLoginWindowUI::OnGetBookGUID( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode == 0)
	{
		tstring strBookGUID;
		BOOL bRet = NDCloudDecodeBookGUID(pNotify->pData, pNotify->nDataSize , strBookGUID);

		if(bRet && strBookGUID.length())
		{
			tstring strUrl = NDCloudComposeUrlChapterInfo(strBookGUID);
			NDCloudDownload(strUrl, MakeHttpDelegate(this, &CLoginWindowUI::OnGetChapter));
			return true;
		}
	}

	ShowLoginComplete();

	return true;
}

bool CLoginWindowUI::OnGetChapter( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode == 0)
	{
		m_pChapterTree = new CChapterTree;
		BOOL bRet = NDCloudDecodeChapterInfo(pNotify->pData, pNotify->nDataSize, m_pChapterTree);

		if(bRet && m_strChapterGuid != NDCloudGetChapterGUID() && m_pChapterTree)
		{
			m_bSetChapter = true;
		}
		else
			delete m_pChapterTree;
	}

	//
	ShowLoginComplete();

	return true;
}

void CLoginWindowUI::SetCheckBoxAutoLogin()
{
	if(m_pAutomaticLoginUI->IsSelected())
	{
		if(!m_pSavePasswordUI->IsSelected())
		{
			m_pSavePasswordUI->Selected(true);
		}
	}
}

void CLoginWindowUI::SetCheckBoxSavePassword()
{
	if(!m_pSavePasswordUI->IsSelected())
	{
		if(m_pAutomaticLoginUI->IsSelected())
		{
			m_pAutomaticLoginUI->Selected(false);
		}
	}
}

bool CLoginWindowUI::OnLoginEvent( void * pParam )
{
	TEventUI* pNotify = (TEventUI*)pParam;
	if (pNotify->Type == UIEVENT_BUTTONDOWN )
	{
		CThirdPartyLoginUI * pThirdPartyLogin = ThirdPartyLoginUI::GetInstance();

		HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
//  	CRect rect;
//  	GetWindowRect(hwnd, &rect);
		if(pThirdPartyLogin->GetHWND() == NULL)
		{
			pThirdPartyLogin->Create(hwnd, _T(""), WS_POPUP, NULL,0,0,0,0);
		}

	//	::MoveWindow(pThirdPartyLogin->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
		pThirdPartyLogin->CenterWindow();
		CButtonUI* pButton = dynamic_cast<CButtonUI*>(pNotify->pSender);
		if(pButton->GetName() == _T("weiboLoginBtn"))
		{
			pThirdPartyLogin->Init(1);
		}
		else if(pButton->GetName() == _T("qqLoginBtn"))
		{
			pThirdPartyLogin->Init(2);
		}
		else if(pButton->GetName() == _T("ndLoginBtn"))
		{
			pThirdPartyLogin->Init(3);
		}
//		MoveWindow(pThirdPartyLogin->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
		
		pThirdPartyLogin->ShowWindow(true);
	}

	return true;
}

bool CLoginWindowUI::GetLoginStatus()
{
	return m_bLogining;
}

void CLoginWindowUI::ClearEditStatus()
{
	TCHAR szUserName[MAX_PATH + 1];
	TCHAR szPassword[MAX_PATH + 1];
	if(GetPassword(szUserName, szPassword))
	{
		m_pUserNameEdit->SetText(szUserName);
		m_pUserNameEdit->SetClearBtn();
		m_pPasswordEdit->SetText(szPassword);
		m_pPasswordEdit->SetClearBtn();
		m_pSavePasswordUI->SetCheck(true);
	}
	else
	{
		m_pUserNameEdit->SetText(m_pUserNameEdit->GetTipText().c_str());
		m_pUserNameEdit->SetClearBtn();
		m_pPasswordEdit->SetText(m_pPasswordEdit->GetTipText().c_str());
		m_pPasswordEdit->SetClearBtn();
		m_pSavePasswordUI->SetCheck(false);
	}

	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Config.ini");
	TCHAR szBuff[MAX_PATH + 1];
	GetPrivateProfileString(_T("config"), _T("AutoLogin"), _T("false"), szBuff, MAX_PATH, strPath.c_str());
	if(_tcsicmp(szBuff, _T("true")) == 0)
		m_pAutomaticLoginUI->SetCheck(true);
	else
		m_pAutomaticLoginUI->SetCheck(false);
	
	m_pErrorTipUserNameLabel->SetVisible(false);
	m_pErrorTipPasswordLabel->SetVisible(false);
	m_pErrorTipNetLabel->SetVisible(false);
}
