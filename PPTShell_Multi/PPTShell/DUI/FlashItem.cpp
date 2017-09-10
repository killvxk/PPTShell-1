#include "StdAfx.h"
#include "FlashItem.h"
#include "DUI/ItemExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "Statistics/Statistics.h"


CFlashItemUI::CFlashItemUI()
{
	
}

CFlashItemUI::~CFlashItemUI()
{

}


void CFlashItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_flash.png"));
	SetContentHeight(110);

	SetIcon(_T("RightBar\\Item\\bg_tit_flash.png"));
	
}

void CFlashItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	__super::DownloadResource(nButtonIndex, CloudFileFlash, 0);
}

void CFlashItemUI::OnItemClick( TNotifyUI& msg )
{

	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CFlashItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	if (nButtonIndex == 0)
	{
		tstring strPngPath = _T("");//CreateFlashThumbFile(lptcsPath);
		InsertSwfByThread(lptcsPath,  strPngPath.c_str(), GetTitle());
		

		Statistics::GetInstance()->Report(STAT_INSERT_FLASH);
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(lptcsPath);
		Statistics::GetInstance()->Report(STAT_INSERT_FLASH);
	}
}

void CFlashItemUI::OnItemDragFinish()
{
	__super::DownloadResource(0, CloudFileFlash, 0);
}

void CFlashItemUI::DownloadLocal()
{
	OnDownloadResourceBefore();
	m_nButtonIndex = 0;
	this->ShowProgress(true);
	CStream* pStream = new CStream(1024);
	pStream->WriteString(GetTitle());
	m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), GetTitle(), CloudFileFlash, 0, MakeHttpDelegate(this, &CFlashItemUI::OnDownloadResourceCompleted2), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress),pStream);
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

bool CFlashItemUI::OnDownloadResourceCompleted2( void* pNotify )
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
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)3,(LPARAM)&arg,NULL);
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
