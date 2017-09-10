#include "StdAfx.h"
#include "MusicItem.h"
#include <NDCloud/NDCloudAPI.h>
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "DUI/ItemExplorer.h"

CMusicItemUI::CMusicItemUI()
{

}

CMusicItemUI::~CMusicItemUI()
{
	
}


void CMusicItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_volume.png"));
	SetContentHeight(110);

	SetIcon(_T("RightBar\\Item\\bg_tit_volume.png"));
}

LPCTSTR CMusicItemUI::GetButtonText( int nIndex )
{
	if (nIndex == 1)
	{
		return _T("试听");
	}
	return __super::GetButtonText(nIndex);

}

void CMusicItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	__super::DownloadResource(nButtonIndex, CloudFileVolume, 0);
}

void CMusicItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CMusicItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	if (nButtonIndex == 0)
	{
		POINTF pt;
		pt.x = 5.0f;
		pt.y = 5.0f;

		if( m_ptDragEndPos.x != -1 && m_ptDragEndPos.y != -1 )		
		{
			int x = m_ptDragEndPos.x;
			int y = m_ptDragEndPos.y;

			m_ptDragEndPos.x = -1;
			m_ptDragEndPos.y = -1;

			InsertVideoByThread(lptcsPath, x, y, 200, 120);
		}
		else
		{
			/*CPPTController* pController = GetPPTController();
			POINT ptScreen = pController->SlidePosToScreenPixel(pt);
			ptScreen.x += 70; */
			InsertVideoByThread(lptcsPath, -1, -1, 200, 120);
		}

		Statistics::GetInstance()->Report(STAT_INSERT_VOLUME);
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(lptcsPath);
	}
}

void CMusicItemUI::OnItemDragFinish()
{
	__super::DownloadResource(0, CloudFileVolume, 0);
}

void CMusicItemUI::DownloadLocal()
{
	OnDownloadResourceBefore();
	m_nButtonIndex = 0;
	this->ShowProgress(true);
	CStream* pStream = new CStream(1024);
	pStream->WriteString(GetTitle());
	m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), GetTitle(), CloudFileVolume, 0, MakeHttpDelegate(this, &CMusicItemUI::OnDownloadResourceCompleted2), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress),pStream);
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

bool CMusicItemUI::OnDownloadResourceCompleted2( void* pNotify )
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
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)5,(LPARAM)&arg,NULL);
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