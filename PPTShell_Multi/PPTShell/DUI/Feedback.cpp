#include "stdafx.h"
#include "Feedback.h"
#include "DUI/EditClear.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"
#include "Util/Util.h"
#include "NDCloud/NDCloudUser.h"
#include "Config.h"
#include "Http/HttpUploadManager.h"
#include "EventCenter/EventDefine.h"
#include <regex>
#include <array>

CFeedbackDialogUI::CFeedbackDialogUI():m_pContainerSubmit(NULL),m_pChkResource(NULL),m_pChkOperation(NULL),m_pEditOperation(NULL),m_pChkStyle(NULL),m_pChkFunction(NULL),m_pChkFluency(NULL),
					m_pChkOther(NULL),m_pLblInputLeftCount(NULL),m_pEditDescription(NULL),m_pBtnAddPic(NULL),m_pCtrPic1(NULL),m_pBtnDeletePic1(NULL),m_pCtrPic2(NULL),m_pBtnDeletePic2(NULL),m_pCtrPic3(NULL),
					m_pBtnDeletePic3(NULL),m_pCtrPic4(NULL),m_pBtnDeletePic4(NULL),m_pCtrPic5(NULL),m_pBtnDeletePic5(NULL),m_pEditContactMode(NULL),m_pBtnSubmit(NULL),m_pLayoutLoading(NULL),
					m_pLayoutSuccess(NULL),m_pLayoutFail(NULL),m_pBtnBackFeedback(NULL),m_pCtrDeleteFrame1(NULL),m_pCtrDeleteFrame2(NULL),m_pCtrDeleteFrame3(NULL),m_pCtrDeleteFrame4(NULL),m_pCtrDeleteFrame5(NULL),
					m_pEditStyle(NULL),m_pEditFunction(NULL),m_pLayoutUserContactMode(NULL),m_pLblUserContactMode(NULL)
{
	m_strPic1Path="";
	m_strPic2Path="";
	m_strPic3Path="";
	m_strPic4Path="";
	m_strPic5Path="";
	m_dwUploadPic1TaskId = 0;
	m_dwUploadPic2TaskId = 0;
	m_dwUploadPic3TaskId = 0;
	m_dwUploadPic4TaskId = 0;
	m_dwUploadPic5TaskId = 0;
	m_dwPostFeedbackTaskId = 0;
	m_nLoginType = -1;//未登陆，类型设置-1；0为正常登陆，1为新浪微博登陆，2为QQ登陆，3为99通行证登陆
	isContactModeModify  = false;
	isDescriptionModify = false;
	isOperationModify = false;
	isStyleModify = false;
	isFunctionModify = false;
	OnEvent(EVT_LOGIN,MakeEventDelegate(this, &CFeedbackDialogUI::OnLoginComplete));
	OnEvent(EVT_LOGOUT,MakeEventDelegate(this, &CFeedbackDialogUI::OnLogoutComplete));
}


CFeedbackDialogUI::~CFeedbackDialogUI()
{
	CancelEvent(EVT_LOGIN,MakeEventDelegate(this, &CFeedbackDialogUI::OnLoginComplete));
	CancelEvent(EVT_LOGOUT,MakeEventDelegate(this, &CFeedbackDialogUI::OnLogoutComplete));
	StopUploadPic1Task();
	StopUploadPic2Task();
	StopUploadPic3Task();
	StopUploadPic4Task();
	StopUploadPic5Task();
	StopPostFeedbackTask();
}

LPCTSTR CFeedbackDialogUI::GetWindowClassName() const
{
	return _T("FeedbackDialog");
}

CDuiString CFeedbackDialogUI::GetSkinFile()
{
	return _T("Feedback\\Feedback.xml");
}

CDuiString CFeedbackDialogUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

void CFeedbackDialogUI::InitWindow()
{
	m_pContainerSubmit = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerSubmit")));
	m_pChkResource = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkResource")));
	m_pChkOperation = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkOperation")));
	m_pEditOperation = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editOperation")));
	m_pChkStyle = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkStyle")));
	m_pEditStyle = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editStyle")));
	m_pChkFunction = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkFunction")));
	m_pEditFunction = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editFunction")));
	m_pChkFluency = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkFluency")));
	m_pChkOther = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkOther")));
	m_pLblInputLeftCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblInputLeftCount")));
	m_pEditDescription = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editDescription")));
	m_pBtnAddPic = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnAddPic")));
	m_pContainerPic1 = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerPic1")));
	m_pCtrPic1 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrPic1")));
	m_pCtrDeleteFrame1 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btnDeleteFrame1")));
	m_pBtnDeletePic1 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDeletePic1")));
	m_pContainerPic2 = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerPic2")));
	m_pCtrPic2 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrPic2")));
	m_pCtrDeleteFrame2 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btnDeleteFrame2")));
	m_pBtnDeletePic2 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDeletePic2")));
	m_pContainerPic3 = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerPic3")));
	m_pCtrPic3 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrPic3")));
	m_pCtrDeleteFrame3 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btnDeleteFrame3")));
	m_pBtnDeletePic3 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDeletePic3")));
	m_pContainerPic4 = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerPic4")));
	m_pCtrPic4 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrPic4")));
	m_pCtrDeleteFrame4 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btnDeleteFrame4")));
	m_pBtnDeletePic4 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDeletePic4")));
	m_pContainerPic5 = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerPic5")));
	m_pCtrPic5 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrPic5")));
	m_pCtrDeleteFrame5 = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btnDeleteFrame5")));
	m_pBtnDeletePic5 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDeletePic5")));
	m_pEditContactMode = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editContactMode")));
	m_pLayoutUserContactMode = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("layoutUserContactMode")));
	m_pLblUserContactMode = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblUserContactMode")));
	m_pBtnSubmit = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSubmit")));
	m_pLayoutLoading = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutLoading")));
	m_pGifLoading = static_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("gifLoading")));
	m_pLayoutSuccess = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutSuccess")));
	m_pLayoutFail = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutFail")));
	m_pBtnBackFeedback = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnBackFeedback")));
	//设置联系方式
	bool showContactMode = true;
	m_nLoginType = NDCloudUser::GetInstance()->GetLoginType();
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if (dwUserID != 0&&m_nLoginType==0)
	{
		showContactMode = false;
	}
	if(showContactMode)
	{
		ShowContactMode();
	}
	else
	{
		string strUserContact = NDCloudUser::GetInstance()->GetUserName();
		ShowUserContactMode(strUserContact);
	}
}

void CFeedbackDialogUI::Init(CRect rect)
{
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);//设置窗体位置大小
}

CControlUI* CFeedbackDialogUI::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("EditClear")) == 0 )
		return new CEditClearUI;
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return NULL;
}


void CFeedbackDialogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	StopUploadPic1Task();
	StopUploadPic2Task();
	StopUploadPic3Task();
	StopUploadPic4Task();
	StopUploadPic5Task();
	StopPostFeedbackTask();
	Close();
}

void CFeedbackDialogUI::ShowLoading()
{
	if(m_pLayoutLoading)
	{
		if(m_pGifLoading)
		{
			m_pGifLoading->PlayGif();
		}
		m_pLayoutLoading->SetVisible(true);
	}
}

void CFeedbackDialogUI::HideLoading()
{
	if(m_pLayoutLoading)
	{
		if(m_pGifLoading)
		{
			m_pGifLoading->StopGif();
		}
		m_pLayoutLoading->SetVisible(false);
	}
}

void CFeedbackDialogUI::ShowSubmitContent()
{
	HideLoading();
	HideFeedbackSuccess();
	HideFeedbackFail();
	if(m_pContainerSubmit)
	{
		m_pContainerSubmit->SetVisible(true);
	}
}

void CFeedbackDialogUI::HideSubmitContent()
{
	if(m_pContainerSubmit)
	{
		m_pContainerSubmit->SetVisible(false);
	}
}

void CFeedbackDialogUI::ShowFeedbackSuccess()
{
	HideSubmitContent();
	HideLoading();
	HideFeedbackFail();
	if(m_pLayoutSuccess)
	{
		m_pLayoutSuccess->SetVisible(true);
	}
	OutputDebugString(_T("EndSubmit Feedback Success\r\n"));
	//开始计时关闭窗体
	//SetTimer(m_hWnd, 100, 1000, (TIMERPROC)TimerProcComplete);
}

void CFeedbackDialogUI::HideFeedbackSuccess()
{
	if(m_pLayoutSuccess)
	{
		m_pLayoutSuccess->SetVisible(false);
	}
}

void CFeedbackDialogUI::ShowFeedbackFail()
{
	HideSubmitContent();
	HideLoading();
	HideFeedbackSuccess();
	if(m_pLayoutFail)
	{
		m_pLayoutFail->SetVisible(true);
	}
	OutputDebugString(_T("EndSubmit Feedback Fail\r\n"));
}

void CFeedbackDialogUI::HideFeedbackFail()
{
	if(m_pLayoutFail)
	{
		m_pLayoutFail->SetVisible(false);
	}
}

void CFeedbackDialogUI::OnEditDescriptionTextChanged( TNotifyUI& msg )
{
	if(m_pEditDescription)
	{
		if(m_pLblInputLeftCount)
		{
			long textCount = m_pEditDescription->GetTextLength();
			long textLeftCount = 800 - textCount;
			char cNum[20];
			itoa(textLeftCount,cNum,10);
			m_pLblInputLeftCount->SetText(cNum);
		}
		string strDescription = m_pEditDescription->GetText();
		if(strDescription.empty())
		{
			isDescriptionModify = false;
		}
		else
		{
			isDescriptionModify = true;
		}
	}
}

void CFeedbackDialogUI::OnEditOperationTextChanged( TNotifyUI& msg )
{
	if(m_pEditOperation)
	{
		string strOperation = m_pEditOperation->GetText();
		if(strOperation.empty())
		{
			isOperationModify = false;
		}
		else
		{
			isOperationModify = true;
		}
	}
}

void CFeedbackDialogUI::OnEditStyleTextChanged( TNotifyUI& msg )
{
	if(m_pEditStyle)
	{
		string strStyle = m_pEditStyle->GetText();
		if(strStyle.empty())
		{
			isStyleModify = false;
		}
		else
		{
			isStyleModify = true;
		}
	}
}

void CFeedbackDialogUI::OnEditFunctionTextChanged( TNotifyUI& msg )
{
	if(m_pEditFunction)
	{
		string strFunction = m_pEditFunction->GetText();
		if(strFunction.empty())
		{
			isFunctionModify = false;
		}
		else
		{
			isFunctionModify = true;
		}
	}
}

#pragma region 图片选择相关
void CFeedbackDialogUI::OnBtnAddPicClick( TNotifyUI& msg )
{
	vector<string> vFiles;
	BOOL bReadOnly		= true;
	BOOL bRet = FileSelectDialog(_T("打开图片"), FILTER_PICTURE, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST| OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
		vFiles, AfxGetMainWnd()->GetSafeHwnd());
	if (bRet)
	{
		if(vFiles.size()>0)
		{
			if(vFiles.size()>5)
			{
				CToast::GetInstance(this->GetHWND())->Show("最多只能选择5张图片");
			}
			else
			{
				int count=0;
				if(!m_strPic1Path.empty())
				{
					count++;
				}
				if(!m_strPic2Path.empty())
				{
					count++;
				}
				if(!m_strPic3Path.empty())
				{
					count++;
				}
				if(!m_strPic4Path.empty())
				{
					count++;
				}
				if(!m_strPic5Path.empty())
				{
					count++;
				}
				int leftCount = 5 - count;
				if(vFiles.size()>leftCount)
				{
					char c[MAX_PATH];
					itoa(leftCount,c,10);
					string strLeftCount = c;
					string strLeftTip = "只能再选择";
					strLeftTip+=strLeftCount;
					strLeftTip+="张图片";
					CToast::GetInstance(this->GetHWND())->Show(strLeftTip);
				}
				else
				{
					AddPictures(vFiles);
				}
			}
		}
	}
}

void CFeedbackDialogUI::AddPictures( vector<string> files )
{
	if(files.size()>0)
	{
		bool needNotify = false;//是否提示选择的图片中已经存在
		bool sizeNotify = false;//是否提示大小限制
		for (int i=0;i<files.size();i++)
		{
			//判断文件大小
			WIN32_FIND_DATA FindFileData;
			HANDLE hFile = ::FindFirstFile(files[i].c_str(), &FindFileData);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				if(FindFileData.nFileSizeLow>5*1024*1024)
				{
					sizeNotify = true;
					continue;
				}
				else
				{
					if(m_strPic1Path.empty())
					{
						m_strPic1Path = files[i];
					}
					else if(m_strPic1Path.compare(files[i].c_str())==0)
					{
						needNotify = true;
						continue;
					}
					else if(m_strPic2Path.empty())
					{
						m_strPic2Path = files[i];
					}
					else if(m_strPic2Path.compare(files[i].c_str())==0)
					{
						needNotify = true;
						continue;
					}
					else if(m_strPic3Path.empty())
					{
						m_strPic3Path = files[i];
					}
					else if(m_strPic3Path.compare(files[i].c_str())==0)
					{
						needNotify = true;
						continue;
					}
					else if(m_strPic4Path.empty())
					{
						m_strPic4Path = files[i];
					}
					else if(m_strPic4Path.compare(files[i].c_str())==0)
					{
						needNotify = true;
						continue;
					}
					else if(m_strPic5Path.empty())
					{
						m_strPic5Path = files[i];
					}
					else if(m_strPic5Path.compare(files[i].c_str())==0)
					{
						needNotify = true;
						continue;
					}
				}
			}
		}
		if(sizeNotify)
		{
			CToast::GetInstance(this->GetHWND())->Show("图片大小不能大于5M");
		}
		else if(needNotify)
		{
			CToast::GetInstance(this->GetHWND())->Show("相同图片不能再次添加");
		}
		RefreshPictureList();
	}
}

void CFeedbackDialogUI::OnLayoutDelete1MouseEnter( TNotifyUI& msg )
{
	if(m_pBtnDeletePic1)
	{
		m_pBtnDeletePic1->SetVisible(true);
	}
	if(m_pCtrDeleteFrame1)
	{
		m_pCtrDeleteFrame1->SetVisible(true);
	}
}

void CFeedbackDialogUI::OnLayoutDelete1MouseLeave( TNotifyUI& msg )
{
	if(m_pBtnDeletePic1)
	{
		m_pBtnDeletePic1->SetVisible(false);
	}
	if(m_pCtrDeleteFrame1)
	{
		m_pCtrDeleteFrame1->SetVisible(false);
	}
}

void CFeedbackDialogUI::OnBtnDeletePic1Click( TNotifyUI& msg )
{
	if(m_pBtnAddPic)
	{
		m_pBtnAddPic->SetVisible(true);
	}
	m_strPic1Path = m_strPic2Path;
	m_strPic2Path = m_strPic3Path;
	m_strPic3Path = m_strPic4Path;
	m_strPic4Path = m_strPic5Path;
	m_strPic5Path = "";
	RefreshPictureList();
}

void CFeedbackDialogUI::RefreshPictureList()
{
	if(m_pContainerPic1&&m_pCtrPic1)
	{
		if(!m_strPic1Path.empty())
		{
			m_pCtrPic1->SetBkImage(m_strPic1Path.c_str());
			m_pContainerPic1->SetVisible(true);
		}
		else
		{
			m_pContainerPic1->SetVisible(false);
			if(m_pBtnDeletePic1)
			{
				m_pBtnDeletePic1->SetVisible(false);
			}
			if(m_pCtrDeleteFrame1)
			{
				m_pCtrDeleteFrame1->SetVisible(false);
			}
		}
	}
	if(m_pContainerPic2&&m_pCtrPic2)
	{
		if(!m_strPic2Path.empty())
		{
			m_pCtrPic2->SetBkImage(m_strPic2Path.c_str());
			m_pContainerPic2->SetVisible(true);
		}
		else
		{
			m_pContainerPic2->SetVisible(false);
			if(m_pBtnDeletePic2)
			{
				m_pBtnDeletePic2->SetVisible(false);
			}
			if(m_pCtrDeleteFrame2)
			{
				m_pCtrDeleteFrame2->SetVisible(false);
			}
		}			
	}
	if(m_pContainerPic3&&m_pCtrPic3)
	{
		if(!m_strPic3Path.empty())
		{
			m_pCtrPic3->SetBkImage(m_strPic3Path.c_str());
			m_pContainerPic3->SetVisible(true);
		}
		else{
			m_pContainerPic3->SetVisible(false);
			if(m_pBtnDeletePic3)
			{
				m_pBtnDeletePic3->SetVisible(false);
			}
			if(m_pCtrDeleteFrame3)
			{
				m_pCtrDeleteFrame3->SetVisible(false);
			}
		}
	}
	if(m_pContainerPic4&&m_pCtrPic4)
	{
		if(!m_strPic4Path.empty())
		{
			m_pCtrPic4->SetBkImage(m_strPic4Path.c_str());
			m_pContainerPic4->SetVisible(true);
		}
		else
		{
			m_pContainerPic4->SetVisible(false);
			if(m_pBtnDeletePic4)
			{
				m_pBtnDeletePic4->SetVisible(false);
			}
			if(m_pCtrDeleteFrame4)
			{
				m_pCtrDeleteFrame4->SetVisible(false);
			}
		}
	}
	if(m_pContainerPic5&&m_pCtrPic5)
	{
		if(!m_strPic5Path.empty())
		{
			m_pCtrPic5->SetBkImage(m_strPic5Path.c_str());
			m_pContainerPic5->SetVisible(true);
			m_pBtnAddPic->SetVisible(false);
		}
		else
		{
			m_pContainerPic5->SetVisible(false);
			if(m_pBtnDeletePic5)
			{
				m_pBtnDeletePic5->SetVisible(false);
			}
			if(m_pCtrDeleteFrame5)
			{
				m_pCtrDeleteFrame5->SetVisible(false);
			}
		}
	}
}

void CFeedbackDialogUI::OnLayoutDelete2MouseEnter(TNotifyUI& msg)
{
	if(m_pBtnDeletePic2)
	{
		m_pBtnDeletePic2->SetVisible(true);
	}
	if(m_pCtrDeleteFrame2)
	{
		m_pCtrDeleteFrame2->SetVisible(true);
	}
}
void CFeedbackDialogUI::OnLayoutDelete2MouseLeave(TNotifyUI& msg)
{
	if(m_pBtnDeletePic2)
	{
		m_pBtnDeletePic2->SetVisible(false);
	}
	if(m_pCtrDeleteFrame2)
	{
		m_pCtrDeleteFrame2->SetVisible(false);
	}
}
void CFeedbackDialogUI::OnBtnDeletePic2Click(TNotifyUI& msg)
{
	if(m_pBtnAddPic)
	{
		m_pBtnAddPic->SetVisible(true);
	}
	m_strPic2Path = m_strPic3Path;
	m_strPic3Path = m_strPic4Path;
	m_strPic4Path = m_strPic5Path;
	m_strPic5Path = "";
	RefreshPictureList();
}
void CFeedbackDialogUI::OnLayoutDelete3MouseEnter(TNotifyUI& msg)
{
	if(m_pBtnDeletePic3)
	{
		m_pBtnDeletePic3->SetVisible(true);
	}
	if(m_pCtrDeleteFrame3)
	{
		m_pCtrDeleteFrame3->SetVisible(true);
	}
}
void CFeedbackDialogUI::OnLayoutDelete3MouseLeave(TNotifyUI& msg)
{
	if(m_pBtnDeletePic3)
	{
		m_pBtnDeletePic3->SetVisible(false);
	}
	if(m_pCtrDeleteFrame3)
	{
		m_pCtrDeleteFrame3->SetVisible(false);
	}
}
void CFeedbackDialogUI::OnBtnDeletePic3Click(TNotifyUI& msg)
{
	if(m_pBtnAddPic)
	{
		m_pBtnAddPic->SetVisible(true);
	}
	m_strPic3Path = m_strPic4Path;
	m_strPic4Path = m_strPic5Path;
	m_strPic5Path = "";
	RefreshPictureList();
}
void CFeedbackDialogUI::OnLayoutDelete4MouseEnter(TNotifyUI& msg)
{
	if(m_pBtnDeletePic4)
	{
		m_pBtnDeletePic4->SetVisible(true);
	}
	if(m_pCtrDeleteFrame4)
	{
		m_pCtrDeleteFrame4->SetVisible(true);
	}
}
void CFeedbackDialogUI::OnLayoutDelete4MouseLeave(TNotifyUI& msg)
{
	if(m_pBtnDeletePic4)
	{
		m_pBtnDeletePic4->SetVisible(false);
	}
	if(m_pCtrDeleteFrame4)
	{
		m_pCtrDeleteFrame4->SetVisible(false);
	}
}
void CFeedbackDialogUI::OnBtnDeletePic4Click(TNotifyUI& msg)
{
	if(m_pBtnAddPic)
	{
		m_pBtnAddPic->SetVisible(true);
	}
	m_strPic4Path = m_strPic5Path;
	m_strPic5Path = "";
	RefreshPictureList();
}
void CFeedbackDialogUI::OnLayoutDelete5MouseEnter(TNotifyUI& msg)
{
	if(m_pBtnDeletePic5)
	{
		m_pBtnDeletePic5->SetVisible(true);
	}
	if(m_pCtrDeleteFrame5)
	{
		m_pCtrDeleteFrame5->SetVisible(true);
	}
}
void CFeedbackDialogUI::OnLayoutDelete5MouseLeave(TNotifyUI& msg)
{
	if(m_pBtnDeletePic5)
	{
		m_pBtnDeletePic5->SetVisible(false);
	}
	if(m_pCtrDeleteFrame5)
	{
		m_pCtrDeleteFrame5->SetVisible(false);
	}
}
void CFeedbackDialogUI::OnBtnDeletePic5Click(TNotifyUI& msg)
{
	if(m_pBtnAddPic)
	{
		m_pBtnAddPic->SetVisible(true);
	}
	m_strPic5Path = "";
	RefreshPictureList();
}
#pragma endregion
void CFeedbackDialogUI::OnBtnBackFeedbackClick( TNotifyUI& msg )
{
	ShowSubmitContent();
}

void CFeedbackDialogUI::OnCheckBoxSelectChanged( TNotifyUI& msg )
{
	if (m_pChkOperation->GetCheck())
	{
		m_pEditOperation->SetVisible(true);
	}
	else
	{
		m_pEditOperation->SetVisible(false);
	}
	if (m_pChkStyle->GetCheck())
	{
		m_pEditStyle->SetVisible(true);
	}
	else
	{
		m_pEditStyle->SetVisible(false);
	}
	if (m_pChkFunction->GetCheck())
	{
		m_pEditFunction->SetVisible(true);
	}
	else
	{
		m_pEditFunction->SetVisible(false);
	}
}

#pragma region 提交相关
void CFeedbackDialogUI::OnBtnSubmitClick( TNotifyUI& msg )
{
	if(m_pChkFluency->GetCheck()||m_pChkFunction->GetCheck()||m_pChkOperation->GetCheck()||m_pChkOther->GetCheck()
		||m_pChkResource->GetCheck()||m_pChkStyle->GetCheck())
	{
		bool canSubmit = true;
		string strContactMode = m_pEditContactMode->GetText();
		if(isContactModeModify&&!strContactMode.empty())
		{
			if(!IsMailString(strContactMode)&&!IsPhoneString(strContactMode))
			{
				canSubmit = false;
				CToast::Toast("请输入正确的邮箱或手机号码");
			}
		}
		if(canSubmit)
		{
			m_vRemotePictures.clear();
			ShowLoading();
			OutputDebugString(_T("BeginSubmit Feedback\r\n"));
			bool needUploadFile=false;
			if(!m_strPic1Path.empty())
			{
				needUploadFile=true;
				StartUploadPic1Task();
			}
			if(!needUploadFile)
			{
				StartPostFeedbackTask();
			}
		}
	}
	else
	{
		CToast::GetInstance(this->GetHWND())->Show("请先选择您想要反馈的问题");
	}
}

void CFeedbackDialogUI::StartUploadPic1Task()
{
	m_dwUploadPic1TaskId = HttpUploadManager::GetInstance()->AddUploadTask(Feedback_host, Feedback_UploadPicUrl, 
		m_strPic1Path.c_str(), "", _T("POST"), 80, MakeHttpDelegate(this, &CFeedbackDialogUI::OnUploadPic1Compeleted), MakeHttpDelegate(NULL));
	if(m_dwUploadPic1TaskId==0)
	{
		ShowFeedbackFail();
	}
}

void CFeedbackDialogUI::StopUploadPic1Task()
{
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadPic1TaskId);
}

bool CFeedbackDialogUI::OnUploadPic1Compeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0&&pNotify->nDataSize>0)
		{
			try
			{
				pNotify->pData[pNotify->nDataSize] = '\0';
				string strResult = pNotify->pData;
				Json::Reader reader;
				Json::Value root;
				bool res = reader.parse(strResult, root);
				if(res)
				{
					string strPic = root["pic"].asString();
					m_vRemotePictures.push_back(strPic);
					if(!m_strPic2Path.empty())
					{
						StartUploadPic2Task();
					}
					else
					{
						StartPostFeedbackTask();
					}
				}
			}
			catch (...)
			{
				ShowFeedbackFail();
			}
		}
		else
		{
			ShowFeedbackFail();
		}
	}
	return true;
}

void CFeedbackDialogUI::StartUploadPic2Task()
{
	m_dwUploadPic2TaskId = HttpUploadManager::GetInstance()->AddUploadTask(Feedback_host, Feedback_UploadPicUrl, 
		m_strPic2Path.c_str(), "", _T("POST"), 80, MakeHttpDelegate(this, &CFeedbackDialogUI::OnUploadPic2Compeleted), MakeHttpDelegate(NULL));
	if(m_dwUploadPic2TaskId==0)
	{
		ShowFeedbackFail();
	}
}

void CFeedbackDialogUI::StopUploadPic2Task()
{
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadPic2TaskId);
}

bool CFeedbackDialogUI::OnUploadPic2Compeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0&&pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			string strResult = pNotify->pData;
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strResult, root);
			if(res)
			{
				string strPic = root["pic"].asString();
				m_vRemotePictures.push_back(strPic);
				if(!m_strPic3Path.empty())
				{
					StartUploadPic3Task();
				}
				else
				{
					StartPostFeedbackTask();
				}
			}
		}
		else
		{
			ShowFeedbackFail();
		}
	}
	return true;
}

void CFeedbackDialogUI::StartUploadPic3Task()
{
	m_dwUploadPic3TaskId = HttpUploadManager::GetInstance()->AddUploadTask(Feedback_host, Feedback_UploadPicUrl, 
		m_strPic3Path.c_str(), "", _T("POST"), 80, MakeHttpDelegate(this, &CFeedbackDialogUI::OnUploadPic3Compeleted), MakeHttpDelegate(NULL));
	if(m_dwUploadPic3TaskId==0)
	{
		ShowFeedbackFail();
	}
}

void CFeedbackDialogUI::StopUploadPic3Task()
{
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadPic3TaskId);
}

bool CFeedbackDialogUI::OnUploadPic3Compeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0&&pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			string strResult = pNotify->pData;
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strResult, root);
			if(res)
			{
				string strPic = root["pic"].asString();
				m_vRemotePictures.push_back(strPic);
				if(!m_strPic4Path.empty())
				{
					StartUploadPic4Task();
				}
				else
				{
					StartPostFeedbackTask();
				}
			}
		}
		else
		{
			ShowFeedbackFail();
		}
	}
	return true;
}

void CFeedbackDialogUI::StartUploadPic4Task()
{
	m_dwUploadPic4TaskId = HttpUploadManager::GetInstance()->AddUploadTask(Feedback_host, Feedback_UploadPicUrl, 
		m_strPic4Path.c_str(), "", _T("POST"), 80, MakeHttpDelegate(this, &CFeedbackDialogUI::OnUploadPic4Compeleted), MakeHttpDelegate(NULL));
	if(m_dwUploadPic4TaskId==0)
	{
		ShowFeedbackFail();
	}
}

void CFeedbackDialogUI::StopUploadPic4Task()
{
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadPic4TaskId);
}

bool CFeedbackDialogUI::OnUploadPic4Compeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0&&pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			string strResult = pNotify->pData;
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strResult, root);
			if(res)
			{
				string strPic = root["pic"].asString();
				m_vRemotePictures.push_back(strPic);
				if(!m_strPic5Path.empty())
				{
					StartUploadPic5Task();
				}
				else
				{
					StartPostFeedbackTask();
				}
			}
		}
		else
		{
			ShowFeedbackFail();
		}
	}
	return true;
}

void CFeedbackDialogUI::StartUploadPic5Task()
{
	m_dwUploadPic5TaskId = HttpUploadManager::GetInstance()->AddUploadTask(Feedback_host, Feedback_UploadPicUrl, 
		m_strPic5Path.c_str(), "", _T("POST"), 80, MakeHttpDelegate(this, &CFeedbackDialogUI::OnUploadPic5Compeleted), MakeHttpDelegate(NULL));
	if(m_dwUploadPic5TaskId==0)
	{
		ShowFeedbackFail();
	}
}

void CFeedbackDialogUI::StopUploadPic5Task()
{
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadPic5TaskId);
}

bool CFeedbackDialogUI::OnUploadPic5Compeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0&&pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			string strResult = pNotify->pData;
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strResult, root);
			if(res)
			{
				string strPic = root["pic"].asString();
				m_vRemotePictures.push_back(strPic);
				StartPostFeedbackTask();
			}
		}
		else
		{
			ShowFeedbackFail();
		}
	}
	return true;
}

void CFeedbackDialogUI::StartPostFeedbackTask()
{
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if(pHttpDownloadManager)
	{
		//提交文本内容
		Json::Value jData(Json::objectValue);
		string strPrbType = "";
		if(m_pChkResource->GetCheck())
		{
			strPrbType="A";
		}
		if(m_pChkOperation->GetCheck())
		{
			if(strPrbType.empty())
			{
				strPrbType="B";
			}
			else
			{
				strPrbType+="|B";
			}
		}
		if(m_pChkStyle->GetCheck())
		{
			if(strPrbType.empty())
			{
				strPrbType="C";
			}
			else
			{
				strPrbType+="|C";
			}
		}
		if(m_pChkFunction->GetCheck())
		{
			if(strPrbType.empty())
			{
				strPrbType="D";
			}
			else
			{
				strPrbType+="|D";
			}
		}
		if(m_pChkFluency->GetCheck())
		{
			if(strPrbType.empty())
			{
				strPrbType="E";
			}
			else
			{
				strPrbType+="|E";
			}
		}
		if(m_pChkOther->GetCheck())
		{
			if(strPrbType.empty())
			{
				strPrbType="F";
			}
			else
			{
				strPrbType+="|F";
			}
		}
		jData["prbType"]=strPrbType;
		//图片
		string strPic="";
		for (int i=0;i<m_vRemotePictures.size();i++)
		{
			if(i==0)
			{
				strPic = m_vRemotePictures[i];
			}
			else
			{
				strPic +="|"+ m_vRemotePictures[i];
			}
		}
		jData["pic"]=strPic;
		//联系方式
		string strContent="";
		if(m_pEditContactMode&&m_pEditContactMode->IsVisible()&&isContactModeModify)
		{
			strContent = m_pEditContactMode->GetText();
		}
		if(m_pLblUserContactMode&&m_pLblUserContactMode->IsVisible())
		{
			strContent = m_pLblUserContactMode->GetText();
		}
		jData["contacContent"]=strContent;
		//联系类型
		if(!strContent.empty())
		{
			int contactType = 0;//1邮箱，2电话号码
			if(IsMailString(strContent))
			{
				contactType = 1;
			}
			else if(IsPhoneString(strContent))
			{
				contactType = 2;
			}
			jData["contactType"] = contactType;
		}
		//op
		Json::Value jop(Json::objectValue);
		if(m_pEditOperation&&m_pEditOperation->IsVisible()&&isOperationModify)
		{
			string strContent= m_pEditOperation->GetText();
			jop["B"] = strContent;
		}
		if(m_pEditStyle&&m_pEditStyle->IsVisible()&&isStyleModify)
		{
			string strContent= m_pEditStyle->GetText();
			jop["C"] = strContent;
		}
		if(m_pEditFunction&&m_pEditFunction->IsVisible()&&isFunctionModify)
		{
			string strContent= m_pEditFunction->GetText();
			jop["D"] = strContent;
		}
		jData["op"]=jop;
		//问题描述
		if(m_pEditDescription&&isDescriptionModify)
		{
			string strDescript = m_pEditDescription->GetText();
			jData["prbDescript"]=strDescript;
		}
		//用户信息
		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		TCHAR szUserId[MAX_PATH];
		_stprintf_s(szUserId, _T("%d"), dwUserId);
		jData["userId"]=szUserId;
		string strUserName = NDCloudUser::GetInstance()->GetUserName();
		jData["userName"]=strUserName;
		jData["versionCode"]=g_Config::GetInstance()->GetVersion();
		jData["fro"]=1;
		string strResult = jData.toStyledString();
		string strResultTmp = UrlEncodeEx(Str2Utf8(strResult));
		string strPost = "data=" + strResultTmp;
		m_dwPostFeedbackTaskId = pHttpDownloadManager->AddTask(Feedback_host,Feedback_PostFeedbackUrl, _T(""), _T("POST"),strPost.c_str(),80,MakeHttpDelegate(this, &CFeedbackDialogUI::OnPostFeedbackCompeleted) ,MakeHttpDelegate(NULL), MakeHttpDelegate(NULL) );
		if(m_dwPostFeedbackTaskId==0)
		{
			ShowFeedbackFail();
		}
	}
}

void CFeedbackDialogUI::StopPostFeedbackTask()
{
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if(pHttpDownloadManager)
	{
		pHttpDownloadManager->CancelTask(m_dwPostFeedbackTaskId);
	}
}

bool CFeedbackDialogUI::OnPostFeedbackCompeleted( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify)
	{
		if(pNotify->dwErrorCode==0)
		{
			ShowFeedbackSuccess();
		}
		else
		{
			ShowFeedbackFail();
		}
	}
	return true;
}
#pragma endregion

#pragma region 联系方式相关
void CFeedbackDialogUI::OnBtnCloseUserContactModeClick( TNotifyUI& msg )
{
	ShowContactMode();
}

bool CFeedbackDialogUI::OnLoginComplete( void* pObj )
{
	TEventNotify* pNotify = (TEventNotify*)pObj;
	m_nLoginType = pNotify->wParam;
	bool showContactMode = true;
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if (dwUserID != 0&&m_nLoginType==0)
	{
		showContactMode = false;
	}
	if(showContactMode)
	{
		ShowContactMode();
	}
	else
	{
		string strUserContact = NDCloudUser::GetInstance()->GetUserName();
		ShowUserContactMode(strUserContact);
	}
	return true;
}

bool CFeedbackDialogUI::OnLogoutComplete( void* pObj )
{
	m_nLoginType = -1;
	ShowContactMode();
	return true;
}

void CFeedbackDialogUI::ShowUserContactMode(string strUserContact)
{
	if(m_pLayoutUserContactMode)
	{
		if(m_pLblUserContactMode)
		{
			m_pLblUserContactMode->SetText(strUserContact.c_str());
		}
		m_pLayoutUserContactMode->SetVisible(true);
	}
	if(m_pEditContactMode)
	{
		m_pEditContactMode->SetVisible(false);
	}
}

void CFeedbackDialogUI::ShowContactMode()
{
	if(m_pLayoutUserContactMode)
	{
		m_pLayoutUserContactMode->SetVisible(false);
	}
	if(m_pEditContactMode)
	{
		m_pEditContactMode->SetVisible(true);
	}
}

bool CFeedbackDialogUI::IsMailString( string strMail)
{
	bool result = false;
	std::tr1::regex regexMail("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
	std::tr1::smatch sResult;
	result = std::tr1::regex_search(strMail, sResult, regexMail);
	return result;
}

bool CFeedbackDialogUI::IsPhoneString( string strPhone )
{
	bool result = false;
	std::tr1::regex regexPhone("^1[34578]\\d{9}$");
	std::tr1::smatch sResult;
	result = std::tr1::regex_search(strPhone, sResult, regexPhone);
	return result;
}

void CFeedbackDialogUI::OnEditContactModeTextChanged( TNotifyUI& msg )
{
	string strContactMode = m_pEditContactMode->GetText();
	if(strContactMode.empty())
	{
		isContactModeModify = false;
	}
	else 
	{
		isContactModeModify = true;
	}
}
#pragma endregion