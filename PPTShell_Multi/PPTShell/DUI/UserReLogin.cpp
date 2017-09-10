#include "StdAfx.h" 
#include "UserReLogin.h" 
#include "NDCloud\NDCloudUser.h" 
#include "EventCenter\EventDefine.h"
#include "GroupExplorer.h"
#include "LoginToolTip.h"

CUserReLogin::CUserReLogin(void)
{
	m_bInit = false;
}

CUserReLogin::~CUserReLogin(void)
{
	m_PaintManager.KillTimer(m_pUserDept, DESKTOP_TIMER_ID);
}

LPCTSTR CUserReLogin::GetWindowClassName() const
{
	return _T("UserReLogin");
}

DuiLib::CDuiString CUserReLogin::GetSkinFile()
{
	return _T("UserRelogin\\UserReLogin.xml");
}

DuiLib::CDuiString CUserReLogin::GetSkinFolder()
{
	return _T("skins");
}

void CUserReLogin::ShowLocalWindows( )
{ 
	this->ShowWindow(SW_SHOWNORMAL);

	m_PaintManager.SetTimer(m_pUserImage, DESKTOP_TIMER_ID, 500);
	m_pUserImage->OnEvent += MakeDelegate(this, &CUserReLogin::OnCloseWndEvent);
}


bool CUserReLogin::OnCloseWndEvent(void* pObj)
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == DESKTOP_TIMER_ID)
		{
			POINT pt; 
			GetCursorPos(&pt); 
			CRect lRect;
			GetWindowRect(this->GetHWND(), &lRect);
			char szPos[128];
			sprintf(szPos, "curpos:%d,%d,pos:%d,%d, %d,%d\n", pt.x, pt.y, m_ptPos.x, m_ptPos.y, lRect.Width(), lRect.Height());
			OutputDebugString(_T(szPos));
			if((pt.x > m_ptPos.x) && (pt.x < m_ptPos.x + lRect.Width())  && ( pt.y > m_ptPos.y) && (pt.y < m_ptPos.y + lRect.Height()) )
			{
				//在区域范围内的不处理
			}
			else
			{
				m_PaintManager.KillTimer(m_pUserDept, DESKTOP_TIMER_ID);				 
				if (this->GetHWND())
				{
					Close();
				}
			}
		} 
	}
	return true;
}

void CUserReLogin::InitWindows( HWND hWndParent, POINT ptPos , tstring pUserPhoto, tstring pNewUserName, int nLoginType)
{
	m_bInit = false;
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	m_hWndParent = hWndParent;
	m_ptPos = ptPos;   
	m_pLogOutBtn		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("LogOutBtn")));
	m_pModifyPwdBtn		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("ModifyPwdBtn")));
	m_pChangeUserBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("ChangeUserBtn")));
	m_pUserImage		= static_cast<CControlUI*>(m_PaintManager.FindControl(_T("UserPhoto")));
	m_pUserName			= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("UserName")));
	m_pUserDept			= static_cast<CComboUI*>(m_PaintManager.FindControl(_T("UserDept")));
	m_pFreeTeachCtn		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("FreeTeachSelectCtn"))); 
	SetUserName(pNewUserName);
	m_sTeacherName = pNewUserName.c_str();

	SetUserDept();
	

	if (pUserPhoto != _T(""))
	{
		m_pUserImage->SetBkImage(pUserPhoto.c_str());
	}
	m_pModifyPwdBtn->SetVisible(false);//控制第三方登陆时修改密码功能屏蔽
	if (nLoginType == 0)
	{
		m_pModifyPwdBtn->SetVisible(true);
	}
	else
	{
		NDCloudUser::GetInstance()->SetFullName(_T(""));
	}

}

void CUserReLogin::OnLogOutBtn( TNotifyUI& msg )
{
	//发送退出消息
	NDCloudUser::GetInstance()->SetLoginType(-1);
	
	NDCloudUser::GetInstance()->CancelLogin();
	NDCloudUser::GetInstance()->LogOut(); 

	BroadcastEvent(EVT_LOGOUT, 0 ,0 ,0);

	m_bInit = false;
	Close();
}

void CUserReLogin::OnChangeUserBtn( TNotifyUI& msg )
{
	m_bInit = false;
	Close();
	//发送重新登录
	BroadcastEvent(EVT_RELOGIN, 0 ,0 ,0); 
	CGroupExplorerUI::GetInstance()->ShowWindow(true); 
	CGroupExplorerUI::GetInstance()->ShowLoginUI();	
}

void CUserReLogin::SetUserName(tstring pNewUserName)
{
	m_pUserName->SetFont(200100);
	if (pNewUserName.length() > 10)//手机号注册的名称字体换小的 2016.01.19
	{
		m_pUserName->SetFont(160100);
	}
	m_pUserName->SetText(pNewUserName.c_str());
}

void CUserReLogin::SetUserDept()
{	
	vector<Json::Value> lGradeList = NDCloudUser::GetInstance()->GetGradeList();
	if (lGradeList.size() == 0)//没有配置班级信息，则隐藏控件2016.02.16 cws
	{
		m_pUserDept->SetVisible(false);//接口获取的数据不全，先暂时屏蔽班级信息2016.01.05
		return;
	}
	if (m_bInit)
	{
		return;
	}
	m_pUserDept->RemoveAll(); 
	Json::Value root;
	Json::Reader reader; 
	tstring sGradeName;
	tstring sClassID;

	for (int i = 0; i < lGradeList.size(); i++)
	{
		CListLabelElementUI* pItem = new CListLabelElementUI;
		string str = lGradeList[i].toStyledString();
		 
		bool res = reader.parse(str, root);
		if (!res)
		{
			pItem->SetText(_T(""));
		}
		else
		{
			sGradeName = root["gradeinfo"]["gradename"].asCString();
			sGradeName+= root["gradeinfo"]["classname"].asCString(); 
			pItem->SetText(sGradeName.c_str());
		} 
		m_pUserDept->Add(pItem);
	}
	CListLabelElementUI* pItem = new CListLabelElementUI; 
	pItem->SetText(_T("自由授课"));		 
	m_pUserDept->Add(pItem);	
	m_pUserDept->SetVisible(true);	 
	int nIndex = NDCloudUser::GetInstance()->GetCurGradeIndex();
	m_pUserDept->SelectItem(nIndex);
}

//LRESULT CUserReLogin::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
//{
//	if (this->GetHWND())
//	{
//		Close();
//	}
//
//	return __super::OnKillFocus(uMsg, wParam, lParam, bHandled); 
//}

void CUserReLogin::OnModifyPwdBtn( TNotifyUI& msg )
{
	if (this->GetHWND())
	{
		Close();
	}

	CGroupExplorerUI::GetInstance()->ShowWindow(true); 
	CGroupExplorerUI::GetInstance()->ShowChangePasswordUI();	
}

void CUserReLogin::OnSelectEvent(TNotifyUI& msg)
{ 
	if (!m_bInit)
	{
		m_bInit = true;
		return;
	}
	int nSel = m_pUserDept->GetCurSel();
	int nCout = m_pUserDept->GetCount(); 
	if ((nCout - 1) == nSel)
	{
		//自由授课
		NDCloudUser::GetInstance()->SetFreeMode(1);
		NDCloudUser::GetInstance()->SetCurGradeIndex(nSel);
	}
	else
	{ 
		vector<Json::Value> lGradeList = NDCloudUser::GetInstance()->GetGradeList(); 
		Json::Value root;
		Json::Reader reader; 
		tstring sGradeName;
		tstring sClassID;		 
		string str = lGradeList[nSel].toStyledString();

		bool res = reader.parse(str, root); 
		sClassID = root["gradeinfo"]["classid"].asCString(); 
		sGradeName = root["gradeinfo"]["gradename"].asCString();
		sGradeName+= root["gradeinfo"]["classname"].asCString(); 
		 
		NDCloudUser::GetInstance()->SetFreeMode(0); 
		NDCloudUser::GetInstance()->SetCurGradeIndex(nSel);
		NDCloudUser::GetInstance()->ReLoadTeachRoomInfo(sClassID);  
	}
	  
}

