#include "StdAfx.h"
#include "IWantMoreUI.h"
#include "NDCloud\NDCloudUser.h"
#include "Http\HttpDownloadManager.h"
#include "Config.h"
#include "Effect\Utility.h"

CIWantMoreUI::CIWantMoreUI(void):m_pEditContent(NULL)
{
	m_dwPostIWantTaskId = 0;
}

CIWantMoreUI::~CIWantMoreUI(void)
{
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if(pHttpDownloadManager)
	{
		pHttpDownloadManager->CancelTask(m_dwPostIWantTaskId);
	}
}

DuiLib::CDuiString CIWantMoreUI::GetSkinFolder()
{
	return _T("skins");
}

DuiLib::CDuiString CIWantMoreUI::GetSkinFile()
{
	return _T("UpdateLog\\Suggest.xml");
}

LPCTSTR CIWantMoreUI::GetWindowClassName( void ) const
{
	return _T("SuggesstDlg");
}

void CIWantMoreUI::InitWindow()
{
	m_pEditContent = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editContent")));
	m_pBtnSubmit = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSubmit")));
}

void CIWantMoreUI::Init( CRect rect )
{
	int width = 561;
	int height = 619;
	int x = rect.left + (rect.right - rect.left - width)/2;
	int y = rect.top + (rect.bottom - rect.top - height)/2;
	MoveWindow(GetHWND(), x, y, width, height, TRUE);//设置窗体位置大小
}

void CIWantMoreUI::OnBtnCloseClick( TNotifyUI& msg )
{
	this->Close();
}

void CIWantMoreUI::OnEditContentTextChanged( TNotifyUI& msg )
{
	if(m_pEditContent&&m_pBtnSubmit)
	{
		tstring strContent = m_pEditContent->GetText();
		if(strContent.empty())
		{
			m_pBtnSubmit->SetEnabled(false);
			m_pBtnSubmit->SetNormalImage(_T(""));
			m_pBtnSubmit->SetHotImage(_T(""));
			m_pBtnSubmit->SetPushedImage(_T(""));
			m_pBtnSubmit->SetTextColor(0xFFFFFFFF);
		}
		else
		{
			m_pBtnSubmit->SetEnabled(true);
			m_pBtnSubmit->SetNormalImage(_T("file='UpdateLog\\submit_normal.png'"));
			m_pBtnSubmit->SetHotImage(_T("file='UpdateLog\\submit_hover.png'"));
			m_pBtnSubmit->SetPushedImage(_T("file='UpdateLog\\submit_press.png'"));
			m_pBtnSubmit->SetTextColor(0xFFFFFFFF);
		}
	}
}

void CIWantMoreUI::OnBtnSubmitClick( TNotifyUI& msg )
{
	if(m_pEditContent)
	{
		tstring strContent = m_pEditContent->GetText();
		if(!strContent.empty())
		{
			tstring strContentEnterTrim = strContent;
			bool canContinue = true;
			//判断全为空格和换行
			vector<tstring> strings = SplitString(strContentEnterTrim,strContentEnterTrim.length(),_T('\r'),false);
			if(strings.size()>0)
			{
				bool isNotChar  = true;
				for(int i=0;i<strings.size();i++)
				{
					tstring strTmp = strings[i];
					strTmp = trim(strTmp);
					if(!strTmp.empty())
					{
						isNotChar = false;
						break;
					}
				}
				if(isNotChar)
				{
					canContinue = false;
					CToast::Toast(_T("内容不能全为空格或回车"));
				}
			}
			if(canContinue)
			{
				strContentEnterTrim = trim(strContentEnterTrim);
				if(!strContentEnterTrim.empty())
				{
					tstring strContentTrim = trim(strContent);
					CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
					if(pHttpDownloadManager)
					{
						Json::Value jData(Json::objectValue);
						jData["text"]=Str2Utf8(strContentTrim);
						DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
						TCHAR szUserId[MAX_PATH];
						_stprintf_s(szUserId, _T("%d"), dwUserId);
						jData["user"]=szUserId;
						jData["ver"]=Str2Utf8(g_Config::GetInstance()->GetVersion());
						string strResult = jData.toStyledString();
						string strResultTmp = Str2Utf8(UrlEncode(strResult));
						string strPost = "data=" + strResultTmp;
						m_dwPostIWantTaskId = pHttpDownloadManager->AddTask(IWant_host,IWant_PostUrl, _T(""), _T("POST"),strPost.c_str(),80,MakeHttpDelegate(this, &CIWantMoreUI::OnPostIWantCompeleted) ,MakeHttpDelegate(NULL), MakeHttpDelegate(NULL) );
					}
				}
				else
				{
					CToast::Toast(_T("内容不能全为空格"));
				}
			}
		}
		else
		{
			CToast::Toast(_T("请输入您想要的内容信息"));
		}
	}
}

bool CIWantMoreUI::OnPostIWantCompeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0)
		{
			CToast::Toast(_T("投递成功"));
			this->Close();
		}
		else
		{
			CToast::Toast(_T("投递失败"));
		}
	}
	return true;
}
