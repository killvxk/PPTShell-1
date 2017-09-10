#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IDownloaderListener.h"
#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/ResourceDownloaderManager.h"
#include "DUI/DownloadManager.h"

#include "DUI/IComponent.h"
#include "DUI/ProgressComponent.h"
#include "DUI/DownloadComponent.h"
#include "DUI/DownloadToolBarComponent.h"
#include "DUI/StyleItem.h"
#include "DUI/IDownloadListener.h"
#include "DUI/DownloadItem.h"

#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "Util/Stream.h"
#include "DUI/ItemHandler.h"
#include "DUI/DownloadItemHandler.h"

#include "NDCloud/NDCloudAPI.h"
#include "PPTWMUserDef.h"
CDownloadManagerUI::CDownloadManagerUI()
{
	m_tabDownloadMgr		= NULL;
	m_layDownloadingList	= NULL;
	m_layDownloadedList		= NULL;

	m_bIsTipping			= false;
}

CDownloadManagerUI::~CDownloadManagerUI()
{
	CResourceDownloaderManager::GetInstance()->RemoveListener(this);
}

CDownloadManagerUI* CDownloadManagerUI::m_pInstance = NULL;
CDownloadManagerUI* CDownloadManagerUI::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CDownloadManagerUI;
	}

	return m_pInstance;
}


DuiLib::CDuiString CDownloadManagerUI::GetSkinFolder()
{
	return _T("Skins");
}

DuiLib::CDuiString CDownloadManagerUI::GetSkinFile()
{
	return _T("DownloadMgr\\main.xml");
}

LPCTSTR CDownloadManagerUI::GetWindowClassName( void ) const
{
	return _T("CDownloadManagerUI");
}

void CDownloadManagerUI::InitWindow()
{
	__super::InitWindow();

	m_tabDownloadMgr		= (CTabLayoutUI*)m_PaintManager.FindControl(_T("dm_download_tab"));
	m_layDownloadingList	= (CContainerUI*)m_PaintManager.FindControl(_T("dm_downloading_list"));
	m_layDownloadedList		= (CContainerUI*)m_PaintManager.FindControl(_T("dm_downloaded_list"));
	m_btnResumeAll			= (CControlUI*)m_PaintManager.FindControl(_T("dm_start_all"));
	m_btnPauseAll			= (CControlUI*)m_PaintManager.FindControl(_T("dm_pause_all"));
	m_layDownloading		= (CContainerUI*)m_PaintManager.FindControl(_T("dm_downloading_layout"));
	m_layDownloaded			= (CContainerUI*)m_PaintManager.FindControl(_T("dm_downloaded_layout"));
	m_layDownloadingNone	= (CContainerUI*)m_PaintManager.FindControl(_T("dm_downloading_none"));
	m_layDownloadedNone		= (CContainerUI*)m_PaintManager.FindControl(_T("dm_downloaded_none"));

	//register downloader notify
	CResourceDownloaderManager::GetInstance()->AddListener(this);

	//default selected tab
	CContainerUI*	pTabHeaderLayout= (CContainerUI*)m_PaintManager.FindControl(_T("dm_tab_header_layout"));
	COptionUI*		pFirstTabBtn	= (COptionUI*)pTabHeaderLayout->GetItemAt(0);
	if (pFirstTabBtn)
	{
		pFirstTabBtn->Selected(true);
		m_PaintManager.SendNotify(pFirstTabBtn, DUI_MSGTYPE_CLICK, 0, 0, true);
	}

	//default layout
	SwitchDownloadingTabTo(false);
	SwitchDownloadedTabTo(false);

	InitDownloadingList();
}

void CDownloadManagerUI::InitDownloadingList()
{
	//init item
// 	map<CResourceDownloader*, CItemHandler*>* pMapDownloaders = CResourceDownloaderManager::GetInstance()->GetDownloaders();
// 	for (map<CResourceDownloader*, CItemHandler*>::iterator itor = pMapDownloaders->begin(); itor != pMapDownloaders->end(); ++itor)
// 	{
// 		CreateDownloadingItem(itor->first, itor->second);
// 	}
}


LPCTSTR CDownloadManagerUI::GetItemIcon( int nType )
{
	LPCTSTR lptcsIcon = NULL;
	switch(nType)
	{
		case CloudFileCourse:
		case CloudFileNdpCourse:
		case CloudFileNdpxCourse:
			{
				lptcsIcon = _T("Item\\Download\\icon_course.png");
			}
			break;
		case CloudFileImage:
			{
				lptcsIcon = _T("Item\\Download\\icon_image.png");
			}
			break;
		case CloudFileVideo:
			{
				lptcsIcon = _T("Item\\Download\\icon_video.png");
			}
			break;
		case CloudFileVolume:
			{
				lptcsIcon = _T("Item\\Download\\icon_volume.png");
			}
			break;
		case CloudFileFlash:
			{
				lptcsIcon =_T("Item\\Download\\icon_flash.png");
			}
			break;
		case CloudFileQuestion:
			{
				lptcsIcon =_T("Item\\Download\\icon_question.png");
			}
			break;
		case CloudFileCoursewareObjects:
			{
				lptcsIcon =_T("Item\\Download\\icon_interacte_question.png");
			}
			break;
		case CloudFile3DResource:
			{	
				lptcsIcon =_T("Item\\Download\\icon_3d.png");
			}
			break;
		case CloudFileVRResource:
			{
				lptcsIcon =_T("Item\\Download\\icon_vr.png");
			}
			break;
	}

	return lptcsIcon;
}

void CDownloadManagerUI::OnBtnPauseAll( TNotifyUI& msg )
{
	for (int i = 0; i < m_layDownloadingList->GetCount(); ++i)
	{
		CDownloadItemUI* pItem = (CDownloadItemUI*)m_layDownloadingList->GetItemAt(i);
		pItem->Pause();
	}
	m_btnResumeAll->SetVisible(true);
	m_btnPauseAll->SetVisible(false);
}

void CDownloadManagerUI::OnBtnCancelAll( TNotifyUI& msg )
{

	if (!DoTipDialog(_T("确认删除全部下载任务?")))
	{
		return;
	}

	for (int i = 0; i < m_layDownloadingList->GetCount(); ++i)
	{
		CDownloadItemUI* pItem = (CDownloadItemUI*)m_layDownloadingList->GetItemAt(i);
		pItem->Cancel();
	}

	//switch to none layout
	SwitchDownloadingTabTo(false);
}

void CDownloadManagerUI::OnBtnStartAll( TNotifyUI& msg )
{
	for (int i = 0; i < m_layDownloadingList->GetCount(); ++i)
	{
		CDownloadItemUI* pItem = (CDownloadItemUI*)m_layDownloadingList->GetItemAt(i);
		pItem->Resume();
	}
	m_btnPauseAll->SetVisible(true);
	m_btnResumeAll->SetVisible(false);
}

void CDownloadManagerUI::OnBtnCleanAll( TNotifyUI& msg )
{

	if (!DoTipDialog(_T("确认清除所有任务?")))
	{
		return;
	}

	m_layDownloadedList->RemoveAll();

	//switch to none layout
	SwitchDownloadedTabTo(false);
}

void CDownloadManagerUI::OnBtnTabHeader( TNotifyUI& msg )
{
	int nIndex = (int) _ttoi(msg.pSender->GetUserData());

	if (m_tabDownloadMgr)
	{
		m_tabDownloadMgr->SelectItem(nIndex);
	}

}

void CDownloadManagerUI::OnBtnClose( TNotifyUI& msg )
{
	this->ShowWindow(false);

	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DOWNLOAD_MANAGER_CLOSE, 0, 0);
}

void CDownloadManagerUI::OnDownloaderCreate( CResourceDownloader* pDownloader, CItemHandler* pItemHandler )
{
	CDownloadItemUI* pItem = (CDownloadItemUI*)GetDownloadingItem(pItemHandler);
	if (pItem 
		&& !pItem->IsAsycRelease())
	{
		CDownloadItemHandler* pHandler = (CDownloadItemHandler*)pItem->GetHandler();

		pDownloader->AddListener(pHandler);
		//associate downloader
		pHandler->SetDownloader(pDownloader);
		//associate original handler
		pHandler->SetOriginalHandler(pItemHandler);

		return;
	}

	IComponent*	pComponent	= new CDownloadToolBarComponent();
	pComponent				= new CProgressComponent(pComponent);
	pComponent				= new CDownloadComponent(pComponent);
	pItem					= new CDownloadItemUI(pComponent);

	m_layDownloadingList->Add(pItem);

	//set data after add item
	pItem->SetIcon(GetItemIcon(pDownloader->GetResourceType()));
	pItem->SetTitle(pDownloader->GetResourceTitle());
	pItem->SetAttribute(_T("padding"), _T("0,0,12,0"));

	CDownloadItemHandler* pHandler = new CDownloadItemHandler();
	//associate downloader
	pHandler->SetDownloader(pDownloader);
	//assciate oringinal handler
	pHandler->SetOriginalHandler(pItemHandler);

	//set event handler
	pItem->SetHandler(pHandler);

	//switch to list layout
	SwitchDownloadingTabTo(true);

	//update downloading num
	UpdateDownloadCount();
}

void CDownloadManagerUI::OnDownloaderDestroy( CResourceDownloader* pDownloader , bool bCancel)
{
	//update downloading num
	UpdateDownloadCount();

	if (bCancel)
	{
		return;
	}

	if (GetDownloadedItem(pDownloader))
	{
		return;
	}

	IComponent*	pComponent	= NULL;
	CDownloadToolBarComponent*	pToolbarComponent = new CDownloadToolBarComponent();
	pToolbarComponent->SetNeedPauseBtn(false);

	pComponent				= new CDownloadComponent(pToolbarComponent);
	CDownloadItemUI*pItem	= new CDownloadItemUI(pComponent);

	m_layDownloadedList->Add(pItem);
	//set data after add item
	pItem->SetIcon(GetItemIcon(pDownloader->GetResourceType()));
	pItem->SetTitle(pDownloader->GetResourceTitle());
	pItem->SetAttribute(_T("padding"), _T("0,0,12,0"));
	pItem->SetGuid(pDownloader->GetResourceGuid());
	
	CDownloadItemHandler* pHandler = new CDownloadItemHandler();
	pItem->SetHandler(pHandler);

	//switch to list layout
	SwitchDownloadedTabTo(true);

}

void CDownloadManagerUI::SwitchDownloadingTabTo( bool bList )
{
	m_layDownloading->SetVisible(bList);
	m_layDownloadingNone->SetVisible(!bList);
}

void CDownloadManagerUI::SwitchDownloadedTabTo( bool bList )
{
	m_layDownloaded->SetVisible(bList);
	m_layDownloadedNone->SetVisible(!bList);
}

void CDownloadManagerUI::OnDownloadItemRemove( TNotifyUI& msg )
{
	if (msg.pSender == m_layDownloadingList)
	{
		m_layDownloadingList->Remove((CControlUI*)msg.wParam);

		if (m_layDownloadingList->GetCount() <= 0)
		{
			SwitchDownloadingTabTo(false);
		}
	}
	else if (msg.pSender == m_layDownloadedList)
	{
		m_layDownloadedList->Remove((CControlUI*)msg.wParam);

		if (m_layDownloadedList->GetCount() <= 0)
		{
			SwitchDownloadedTabTo(false);
		}
	}
}

CControlUI* CDownloadManagerUI::GetDownloadingItem( CItemHandler* pHandler )
{
	for (int i = 0; i < m_layDownloadingList->GetCount(); ++i)
	{
		CDownloadItemUI* pItem = (CDownloadItemUI*)m_layDownloadingList->GetItemAt(i);
		if (pHandler->GetHolderIndex(pItem) != -1)
		{
			return pItem;
		}
	}

	return NULL;
}

CControlUI* CDownloadManagerUI::GetDownloadedItem( CResourceDownloader* pDownloader )
{
	for (int i = 0; i < m_layDownloadedList->GetCount(); ++i)
	{
		CDownloadItemUI* pItem = (CDownloadItemUI*)m_layDownloadedList->GetItemAt(i);
		if (pItem->GetGuid() != NULL
			&& pDownloader->GetResourceGuid() != NULL
			&& _tcsicmp(pDownloader->GetResourceGuid(), pItem->GetGuid()) == 0)
		{
			return pItem;
		}
	}
	return NULL;
}

void CDownloadManagerUI::ShowManagerUI( bool bShow )
{
	if (!GetHWND())
	{
		Create(AfxGetMainWnd()->GetSafeHwnd(), GetWindowClassName(), WS_POPUP, WS_EX_TOOLWINDOW);
	}

	if (bShow)
	{
		UpdatePos();
	}

	ShowWindow(bShow);
}

void CDownloadManagerUI::UpdatePos()
{
	if (!GetHWND())
	{
		return;
	}

	RECT rtWindow		= {0};
	RECT rtMainWindow	= {0};
	GetWindowRect(GetHWND(), &rtWindow);
	GetWindowRect(AfxGetMainWnd()->GetSafeHwnd(), &rtMainWindow);

	int nX	= rtMainWindow.right - 100 - (rtWindow.right - rtWindow.left);
	int nY	= rtMainWindow.top  + 50;
	SetWindowPos(GetHWND(), NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

}

LRESULT CDownloadManagerUI::OnKillFocus( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (!m_bIsTipping)
	{
		TNotifyUI notify;
		OnBtnClose(notify);
	}
	return __super::OnKillFocus(nMsg, wParam, lParam, bHandled);
}

void CDownloadManagerUI::UpdateDownloadCount()
{
	CContainerUI*	pTabHeaderLayout= (CContainerUI*)m_PaintManager.FindControl(_T("dm_tab_header_layout"));
	if (!pTabHeaderLayout)
	{
		return;
	}

	CLabelUI* lbCount	= (CLabelUI*)pTabHeaderLayout->GetItemAt(0);
	if (!lbCount)
	{
		return;
	}

	TCHAR	szNum[32]	= {0};
	int		nNum		= CResourceDownloaderManager::GetInstance()->GetDownloaders()->size();
	if (nNum > 99)
	{
		_stprintf_s(szNum, _T("正在下载( %d+ )"), nNum);
	}
	else if (nNum >= 1)
	{
		_stprintf_s(szNum, _T("正在下载( %d )"), nNum);
	}
	else
	{
		_stprintf_s(szNum, _T("正在下载"), nNum);
	}

	lbCount->SetText(szNum);
}

bool CDownloadManagerUI::DoTipDialog( LPCTSTR lptcsTip )
{
	m_bIsTipping = true;
	int nRet = UIMessageBox(GetHWND(), lptcsTip, _T(""), _T("是,否"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL, NULL, 310, 180); 
	m_bIsTipping = false;


	if (nRet == ID_MSGBOX_BTN)
	{
		return true;
	}
	return false;
}



