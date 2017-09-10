#include "StdAfx.h"
#include "VideoItem.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "Statistics/Statistics.h"
#include "DUI/ItemExplorer.h"

CVideoItemUI::CVideoItemUI()
{

}

CVideoItemUI::~CVideoItemUI()
{
	NDCloudDownloadCancel(m_dwDownloadId);
}

CControlUI* CVideoItemUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return NULL;
}

void CVideoItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_video.png"));
	SetContentHeight(110);

	if( !m_builder.GetMarkup()->IsValid() ) {
		m_layMaskStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(_T("RightBar\\Item\\ResourceMaskStyle.xml"), (UINT)0, this, this->GetManager()));
	}
	else {
		m_layMaskStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(this, this->GetManager()));
	}
	GetContent()->Add(m_layMaskStyle);


	m_pAnimation= dynamic_cast<CGifAnimUI*>( FindSubControl(_T("loading")));

	SetIcon(_T("RightBar\\Item\\bg_tit_video.png"));

	DownloadThumbnail();
}

void CVideoItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	__super::DownloadResource(nButtonIndex, CloudFileVideo, 0);
}

void CVideoItemUI::OnItemClick( TNotifyUI& msg )
{

	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CVideoItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{

	if (nButtonIndex == 0)
	{

		tstring strTempPath = lptcsPath;
		tstring strFLASH	= _T(".swf");
		
		if (strTempPath.empty())
			return;

		strTempPath			= strTempPath.substr(strTempPath.length() - strFLASH.length());
		if (_tcsicmp(strTempPath.c_str(), strFLASH.c_str()) == 0)
		{
			tstring strPngPath = _T("");//CreateFlashThumbFile(lptcsPath);
			InsertSwfByThread(lptcsPath,  strPngPath.c_str(), GetTitle());
		}
		else
		{
			InsertVideoByThread(lptcsPath);
		}

		Statistics::GetInstance()->Report(STAT_INSERT_VIDEO);
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(lptcsPath);
		Statistics::GetInstance()->Report(STAT_PREVIEW_VIDEO);
	}
}

void CVideoItemUI::OnItemDragFinish()
{
	__super::DownloadResource(0, CloudFileVideo, 0);
}

void CVideoItemUI::DownloadLocal()
{
	OnDownloadResourceBefore();
	m_nButtonIndex = 0;
	this->ShowProgress(true);
	CStream* pStream = new CStream(1024);
	pStream->WriteString(GetTitle());
	m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), GetTitle(), CloudFileVideo, 0, 
		MakeHttpDelegate(this, &CVideoItemUI::OnDownloadResourceCompleted2), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress),pStream);
	if (m_dwResourceDownloadId == 0)
	{
		this->ShowProgress(false);
		string strToast="“";
		strToast+=GetTitle();
		strToast+="”";
		strToast+="下载失败";
		CToast::Toast(strToast);
	}
}

bool CVideoItemUI::OnDownloadResourceCompleted2( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	tstring strTitle = "";
	if(pHttpNotify->pUserData)
	{
		CStream* pStream = (CStream*)pHttpNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			strTitle = pStream->ReadString();
			delete pStream;
		}
	}
	if(pHttpNotify->strFilePath != _T(""))
	{
		//广播下载完成
		vector<tstring> arg;
		arg.push_back(strTitle);
		arg.push_back(pHttpNotify->strFilePath);
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)4,(LPARAM)&arg,NULL);
		string strToast = "“"+ strTitle +"”" + "下载完成";
		CToast::Toast(strToast);
	}
	else
	{
		string strToast = "“"+ strTitle +"”" + "下载失败";
		CToast::Toast(strToast);
	}
	CControlUI* pCtrl	= FindSubControl(_T("empty"));
	if(pCtrl)
		pCtrl->SetToolTip("");
	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);
	return true;
}

void CVideoItemUI::ReadStream(CStream* pStream)
{
	m_strGuid = pStream->ReadString();

	SetTitle(pStream->ReadString().c_str());
	SetResource(pStream->ReadString().c_str());
	SetGroup(_T("123"));

	// json information which used to modify title
	
	m_strPreviewUrl = pStream->ReadString();
	m_strJsonInfo = pStream->ReadString();
}

void CVideoItemUI::DownloadThumbnail()
{
	this->StartMask();
	m_dwDownloadId = NDCloudDownloadFile(m_strPreviewUrl.c_str(), m_strGuid, GetTitle(), CloudFileImage, 240, MakeHttpDelegate(this, &CVideoItemUI::OnDownloadThumbnailCompleted));
 	if (m_dwDownloadId == 0)
 	{
// 		SetImage(_T("Rightbar/Item/item_bg_image_none.png"));
// 		this->EnableToolbar(false);
		this->StopMask();
 		return;
 	}
}

void CVideoItemUI::StartMask()
{
	m_layMaskStyle->SetVisible(true);
	m_pAnimation->PlayGif();
}

void CVideoItemUI::StopMask()
{
	m_layMaskStyle->SetVisible(false);
	m_pAnimation->StopGif();
}

bool CVideoItemUI::OnDownloadThumbnailCompleted( void* pNotify )
{
	this->StopMask();

	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	if (pHttpNotify->strFilePath == _T(""))
	{
// 		SetImage(_T("Rightbar/Item/item_bg_image_none.png"));
//		this->EnableToolbar(false);
		return true;
	}

	SetImage(pHttpNotify->strFilePath.c_str());
	return true;
}
