#include "StdAfx.h"
#include "UploadItem.h"
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudResModel.h"
#include "NDCloud/NDCloudUpload.h"
#include "json/json.h"
#include "Util/FileTypeFilter.h"

#define NDCloudServerUploadAssets					_T("/v0.6/assets/none/uploadurl?uid=%u&renew=false")
#define NDCloudServerUploadCoursewares				_T("/v0.6/coursewares/none/uploadurl?uid=%u&renew=false")

CUploadItemUI::CUploadItemUI()
{
	m_edtRename = NULL;
	m_dwDownloadServerPathId	= 0;
	m_dwUploadId				= 0;
	m_dwCommitId				= 0;
	m_nType						= 0;

	SetState(eReady);

}


CUploadItemUI::~CUploadItemUI()
{
	NDCloudDownloadCancel(m_dwDownloadServerPathId);
	NDCloudDownloadCancel(m_dwCommitId);
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadId);
}

void CUploadItemUI::Init()
{
	__super::Init();


	CContainerUI*pEmpty= dynamic_cast<CContainerUI*>(FindSubControl(_T("empty")));
	pEmpty->OnEvent	+= MakeDelegate(this, &CUploadItemUI::OnEmptyControlEvent);

	m_edtRename		= dynamic_cast<CEditUI*>(FindSubControl(_T("rename")));
	m_edtRename->OnEvent += MakeDelegate(this, &CUploadItemUI::OnRenameEvent);

	CButtonUI* pBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("delete")));
	assert(pBtn);
	pBtn->OnNotify	+= MakeDelegate(this, &CUploadItemUI::OnBtnNotify);

	assert(m_optBorder);
	m_optBorder->OnNotify	+= MakeDelegate(this, &CUploadItemUI::OnBtnNotify);
	m_optBorder->OnEvent	+= MakeDelegate(this, &CUploadItemUI::OnItemEvent);

	m_proDownload = dynamic_cast<CProgressUI*>( FindSubControl(_T("progress")));

}

bool CUploadItemUI::OnBtnNotify( void* pNotify )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

	if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("item")) == 0)
	{
		if (pNotifyUI->sType == _T("click"))
		{
// 			CControlUI *pControl = this->GetParent();
// 			if (this->GetParent()->GetTag() != NULL)
// 			{
// 				CControlUI* pLastToolBar = (CControlUI*)this->GetParent()->GetTag();
// 				pLastToolBar->SetAttribute(_T("inset"), _T("0,0,0,0"));
// 			}
// 			m_layToolBar->SetAttribute(_T("inset"), _T("2,0,2,2"));
// 			this->GetParent()->SetTag((UINT_PTR) m_layToolBar);			
		}

	}

	if (pNotifyUI->sType == _T("click"))
	{
		if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("delete")) == 0)
		{
			OnButtonDelete(*pNotifyUI);
		}
		else if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("item")) == 0
			&& pNotifyUI->wParam == 0)
		{
			OnItemClick(*pNotifyUI);
		}
	}
	return true;
}

void CUploadItemUI::OnButtonDelete( TNotifyUI& msg )
{
	this->SetVisible(false);
	this->GetManager()->SendNotify(this->GetParent(), _T("itemremove"), (WPARAM)this, 0, true);
}

void CUploadItemUI::OnItemClick( TNotifyUI& msg )
{
	::SetFocus(CGroupExplorerUI::GetInstance()->GetHWND());
}

void CUploadItemUI::SetTypeTitle( LPCTSTR lptcsType )
{
	CControlUI* pCtrl = (FindSubControl(_T("type")));
	if (pCtrl)
	{
		pCtrl->SetText(lptcsType);
	}
}

void CUploadItemUI::SetType( int nType )
{
	m_nType = nType;
}

bool CUploadItemUI::OnRenameEvent( void* pObj )
{
	TEventUI* pEvent  = (TEventUI*)pObj;

	if (pEvent->Type == UIEVENT_KILLFOCUS
		&& pEvent->pSender)
	{
		OnRenameEnd();
	}

	if (pEvent->Type == UIEVENT_KEYDOWN)
	{
		if (pEvent->chKey == VK_RETURN)
		{
			OnRenameEnd();
		}

	}

	return true;

}

bool CUploadItemUI::OnItemEvent( void* pEvent )
{
	TEventUI event = *(TEventUI*)pEvent;

	if (event.Type == UIEVENT_KEYDOWN)
	{
		if (event.chKey == VK_F2)
		{	
			OnRenameStart();
		}

	}

	return true;
}

void CUploadItemUI::OnRenameStart()
{
	m_lbName->SetVisible(false);
	m_edtRename->SetText(m_lbName->GetText());
	m_edtRename->SetVisible(true);
	m_edtRename->SetFocus();
	m_edtRename->SetSelAll();

}

void CUploadItemUI::OnRenameEnd()
{
	m_lbName->SetVisible(true);
	m_edtRename->SetVisible(false);
	tstring strText = m_edtRename->GetText();

	if (!trim(strText).empty())
	{
		this->SetTitle(m_edtRename->GetText());
	}
	
}

bool CUploadItemUI::OnEmptyControlEvent( void* pEvent )
{
	TEventUI event = *(TEventUI*)pEvent;
	m_optBorder->Event(event);

	return true;
}

DWORD CUploadItemUI::UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath)
{
	TCHAR szHost[128];
	TCHAR szUrlPath[1024];
	if(_stscanf_s(lptcsUrl, _T("http://%[^/]%s"), szHost, _countof(szHost) - 1, szUrlPath , _countof(szUrlPath) - 1) != 2)
	{
		return 0;
	}
	tstring strHost = szHost;
	tstring strUrlPath = szUrlPath;
	strUrlPath += _T("?session=");
	strUrlPath += lptcsSessionId;

	return HttpUploadManager::GetInstance()->AddUploadTask(strHost.c_str(), 
		strUrlPath.c_str(),
		GetResource(),
		lptcsServerFilePath,
		_T("POST"), 
		80,
		MakeHttpDelegate(this, &CUploadItemUI::OnUploadCompleted),
		MakeHttpDelegate(this, &CUploadItemUI::OnUploadProgress));

}

bool CUploadItemUI::OnGetServerPath( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode == 0)
	{
		pNotify->pData[pNotify->nDataSize] = '\0';
		tstring str = Ansi2Str(pNotify->pData);
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(pNotify->pData, root);
		if( res )
		{
			tstring strUrl;
			tstring strSessionId;
			do 
			{
				if( !root["access_url"].isNull() )
				{
					strUrl = Ansi2Str(root["access_url"].asCString());
				}
				else
					break;

				if( !root["uuid"].isNull() )
				{
					m_strUploadGuid = Ansi2Str(root["uuid"].asCString());
				}
				else
					break;

				if( !root["session_id"].isNull() )
				{
					strSessionId = Ansi2Str(root["session_id"].asCString());
				}
				else
					break;

				if( !root["dist_path"].isNull() )
				{
					m_strServerPath = Ansi2Str(root["dist_path"].asCString());
				}
				else
					break;

				this->ShowProgress(true);
				m_dwUploadId = UploadFile(strUrl.c_str(), strSessionId.c_str(), m_strUploadGuid.c_str(), m_strServerPath.c_str());
				if(m_dwUploadId == 0)
				{
					break;
				}
				return true;

			} while (FALSE);

		}

	}

	if(pNotify->dwErrorCode == 0)
	{
		pNotify->dwErrorCode = 1;
	}
	OnUploadCompleted(pNotify);

	return true;
}

bool CUploadItemUI::OnUploadProgress( void * pParam )
{

	THttpNotify* pHttpNotify = (THttpNotify*)pParam;
	int nPos = (int)(pHttpNotify->fPercent * m_proDownload->GetMaxValue());
	this->SetProgress(nPos);

	CControlUI* pCtrl	= FindSubControl(_T("empty"));
	TCHAR szTip[128]	= {0};
	TCHAR szSpeed[16]	= {0};
	TCHAR szRemain[32]	= {0};
	TCHAR szElapse[32]	= {0};
	TCHAR szSize[32]	= {0};
	int		nTemp		= 0;

	_stprintf_s(szSpeed, _T("%.1f %s"), 
		pHttpNotify->nSpeed < 1000 ? pHttpNotify->nSpeed : (pHttpNotify->nSpeed  * 1.0f / 1024),
		pHttpNotify->nSpeed < 1000 ? _T("KB/S") : _T("MB/S"));

	nTemp = pHttpNotify->nElapseTime;
	_stprintf_s(szElapse, _T("%02d:%02d:%02d"), 
		pHttpNotify->nElapseTime / 3600,
		(nTemp %= 3600, nTemp / 60),
		pHttpNotify->nElapseTime % 60);

	nTemp = pHttpNotify->nRemainTime;
	_stprintf_s(szRemain, _T("%02d:%02d:%02d"), 
		pHttpNotify->nRemainTime / 3600,
		(nTemp %= 3600, nTemp / 60),
		pHttpNotify->nRemainTime % 60);


	if (pHttpNotify->nTotalSize < 1000 )
	{
		_stprintf_s(szSize, _T("%.2f B"), 
			(pHttpNotify->nTotalSize  * 1.0f));
	}
	else if (pHttpNotify->nTotalSize < 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f KB"), 
			(pHttpNotify->nTotalSize  * 1.0f / 1024));
	}
	else if (pHttpNotify->nTotalSize < 1000 * 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f MB"), 
			(pHttpNotify->nTotalSize  * 1.0f / (1024 * 1024)));
	}


	_stprintf_s(szTip, _T("上传速度：%s<n>上传用时：%s<n>剩余时间：%s<n>文件大小：%s<n>"), 
		szSpeed,
		szElapse,
		szRemain,
		szSize);

	pCtrl->SetToolTip(szTip);

	return true;
}

bool CUploadItemUI::OnUploadCompleted( void * pParam )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pParam;

	tstring strFullFileServerPath;
	if (pHttpNotify->dwErrorCode==0)
	{
		pHttpNotify->pData[pHttpNotify->nDataSize] = '\0';
		string str = pHttpNotify->pData;
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if(!res||root.get("dentry_id", Json::Value()).isNull())
		{
			pHttpNotify->dwErrorCode=5;
		}

		if(res && !root.get("path", Json::Value()).isNull())
		{
			strFullFileServerPath= Ansi2Str(root["path"].asCString());
		}
	}
	if (pHttpNotify->dwErrorCode != 0)
	{
		this->ShowProgress(false);
		this->SetProgress(0);
		SetState(eFail);
		return true;
	}

	this->SetProgress(m_proDownload->GetMaxValue() - 1);

	CControlUI* pCtrl = FindSubControl(_T("empty"));
	pCtrl->SetToolTip(_T(""));

	CNDCloudResourceModel ResModel;
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

	tstring strChapterGuid = NDCloudGetChapterGUID();

	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	ResModel.SetGuid(m_strUploadGuid.c_str());
	tstring strTmp = GetTitle();
	if(strTmp.length()>200)
	{
		strTmp = strTmp.substr(0,200);
	}
	ResModel.SetBasicInfo(strTmp);
	ResModel.SetPreview();
	ResModel.SetTechInfo(GetResource(), m_strServerPath.c_str(), strFullFileServerPath);
	ResModel.SetLifeCycleInfo();
	ResModel.AddCoverage(_T("User"), szUserId, _T(""), _T("OWNER") );
	ResModel.AddRelation(strChapterGuid);

	CFileTypeFilter filter;
	int nType = filter.GetFileType(GetResource());
	if( nType == FILE_FILTER_PPT )
	{
		ResModel.AddRequirement(_T("SOFTWARE"), _T("editor"), _T("2007"), _T("2013"), _T("microsoft office 2007+"));
		ResModel.AddRequirement(_T("SOFTWARE"), _T("player"), _T("2007"), _T("2013"), _T("microsoft office 2007+"));
	}


	CCategoryTree* pCategory;
	NDCloudGetCategoryTree(pCategory);
	ResModel.SetCategoryInfo(pCategory);


	// Commit to database
	m_dwCommitId = ResModel.CommitResourceModel(MakeHttpDelegate(this, &CUploadItemUI::OnCommitCompleted));

	return true;
}

bool CUploadItemUI::OnCommitCompleted( void * pParam )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pParam;

	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);

	if (pHttpNotify->dwErrorCode != 0)
	{
		// fail
		SetState(eFail);
		return true;
	}

	Json::Reader	reader;
	Json::Value		result;
	if (!reader.parse(pHttpNotify->pData, pHttpNotify->pData + pHttpNotify->nDataSize, result))
	{
		// fail
		SetState(eFail);
		return true;
	}

	if (!result.get("code", Json::Value()).isNull())
	{
		// fail
		SetState(eFail);
		return true;
	}

	SetState(eSuccess);

// 	CControlUI* pCtrl = FindSubControl(_T("delete"));
// 	if (pCtrl)
// 	{
// 		pCtrl->SetVisible(false);
// 	}

	//notify dbank item refresh count
	CGroupExplorerUI::GetInstance()->AddDBankItemCount(m_nType);

	return true;
}


void CUploadItemUI::StartUpload()
{

	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return;


	CFileTypeFilter filter;
	int nType = filter.GetFileType(GetResource());
	if ( nType == FILE_FILTER_OTHER)
	{
		return;
	}

	TCHAR szPost[1024] = {0};
	if (nType == FILE_FILTER_PPT)
	{
		_stprintf_s(szPost, NDCloudServerUploadCoursewares, NDCloudUser::GetInstance()->GetUserId());
	}
	else
	{
		_stprintf_s(szPost, NDCloudServerUploadAssets, NDCloudUser::GetInstance()->GetUserId());
	}

	SetState(eUploading);
	m_dwDownloadServerPathId = pHttpManager->AddTask(_T("esp-lifecycle.web.sdp.101.com"), szPost, _T(""), _T("GET"), "", 80, 
		MakeHttpDelegate(this, &CUploadItemUI::OnGetServerPath),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL), 
		TRUE);

	if (!m_dwDownloadServerPathId)
	{
		SetState(eFail);
	}

}

void CUploadItemUI::ShowProgress( bool bVisible )
{
	m_proDownload->SetVisible(bVisible);
	CContainerUI* pContainer = dynamic_cast<CContainerUI*>(FindSubControl(_T("bottom")));
	for (int i = 0; i < pContainer->GetCount(); ++i)
	{
		CControlUI* pCtrl  = pContainer->GetItemAt(i);
		if (pCtrl)
		{
			pCtrl->SetEnabled(!bVisible);
		}
	}

}

void CUploadItemUI::SetProgress( int nPos )
{
	m_proDownload->SetValue(nPos);

	TCHAR szText[32] = {0};
	_stprintf_s(szText, _T("%d%%"), nPos);

	m_proDownload->SetText(szText);
}

void CUploadItemUI::SetState( int nState )
{
	if(m_OnUploadStateCallBack)
		m_OnUploadStateCallBack(this);

	m_nState = nState;
	if (m_nState == eSuccess)
	{
		CControlUI* pCtrl = FindSubControl(_T("success"));
		pCtrl->SetVisible(true);

		return;
	}

	CControlUI* pCtrl = FindSubControl(_T("fail"));
	if (m_nState == eFail)
	{
		if (pCtrl)
		{
			pCtrl->SetVisible(true);
		}
		
	}
	else
	{
		if (pCtrl)
		{
			pCtrl->SetVisible(false);
		}
	}


}

bool CUploadItemUI::IsUploading()
{
	if (m_nState == eUploading)
	{
		return true;
	}
	return false;
}

bool CUploadItemUI::IsUploaded()
{
	if (m_nState == eSuccess)
	{
		return true;
	}
	return false;
}

void CUploadItemUI::SetUploadFinishCallBack( CDelegateBase &delegate )
{
	m_OnUploadStateCallBack.clear();
	m_OnUploadStateCallBack += delegate;
}
