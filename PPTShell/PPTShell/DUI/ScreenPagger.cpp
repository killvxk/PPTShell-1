#include "StdAfx.h"
#include "ScreenPagger.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "DUI/InstrumentItem.h"
#include "DUI/ScreenThumbItem.h"
#include "GUI/MainFrm.h"

#define ScreenThumbPage_Visibles	5
#define Vaild_ScroolPos_TimerId		3000
#define DelayLoad_Thumbnail_TimerId	3001

DuiLib::CDialogBuilder CScreenPaggerUI::m_ItemBuilder;
CScreenPaggerUI::CScreenPaggerUI(void)
{
	m_pItemList				= NULL;
	m_nTotalLeafCount		= 0;
	m_nCurrentPPTPage = 0;
	m_dwLastClickTickCount	= GetTickCount();
}

CScreenPaggerUI::~CScreenPaggerUI(void)
{
	this->GetManager()->RemoveNotifier(this);
}

CControlUI* CScreenPaggerUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("ResourceItem"), pstrClass) == 0)
	{
		return new CScreenThumbItemUI;
	}

	return NULL;
}

void CScreenPaggerUI::Init()
{
	__super::Init();

	m_pItemList = (	CContainerUI*)FindSubControl(_T("thumbnail_list"));
	m_pItemList->GetHorizontalScrollBar()->SetFixedHeight(1);
	CreateThumbnailList();


	m_pItemList->OnEvent += MakeDelegate(this, &CScreenPaggerUI::OnListEvent);
	this->GetManager()->SetTimer(m_pItemList, Vaild_ScroolPos_TimerId, 100);
	this->GetManager()->AddNotifier(this);

	CControlUI* pCtrl = FindSubControl(_T("thumbnail_pickup"));
	if(pCtrl)
	{
		this->GetManager()->SendNotify(pCtrl, DUI_MSGTYPE_CLICK, 0, 0, true);
	}
	

}

void CScreenPaggerUI::CreateThumbnailList()
{
	m_pItemList->RemoveAll();
	m_pItemList->SetTag((UINT_PTR)NULL);
	
	TCHAR szIndex[MAX_PATH * 2]	= {0};
	RECT lRect;
	TCHAR szText[MAX_PATH] = {0};

	for (int i = 0; i < m_nTotalLeafCount; ++i)
	{ 
		_stprintf_s(szText, _T("%d"), i+1);
		CScreenThumbItemUI* pItem = NULL;
		if (!m_ItemBuilder.GetMarkup()->IsValid())
		{
			pItem = (CScreenThumbItemUI*)m_ItemBuilder.Create(_T("RightBar\\Item\\ResourceItem.xml"), NULL, this);
		}
		else
		{
			pItem = (CScreenThumbItemUI*)m_ItemBuilder.Create(this);
		}
		pItem->SetIndex(i);
		pItem->SetGroup(_T("ScreenThumbs")); 
		pItem->SetOnItemClickDelegate(MakeDelegate(this, &CScreenPaggerUI::OnItemClick)); 
		pItem->SetItemLabel(szText, true);
		m_pItemList->Add(pItem);
	} 

	bool bShowSwitchBtn = true;
	if (m_nTotalLeafCount <= 5)
	{
		bShowSwitchBtn = false;
	}

	CContainerUI* pListLayout = (CContainerUI*)FindSubControl(_T("thumbnail_layout"));
	CControlUI* pCtrl = pListLayout->FindSubControl(_T("pre_page"));
	if (pCtrl)
	{
		pCtrl->SetVisible(bShowSwitchBtn);
	}

	pCtrl = pListLayout->FindSubControl(_T("next_page"));
	if (pCtrl)
	{
		pCtrl->SetVisible(bShowSwitchBtn);
	}

	if (m_nTotalLeafCount > 0)
	{
		CScreenThumbItemUI* pItem  =  dynamic_cast<CScreenThumbItemUI*>(m_pItemList->GetItemAt(0)); 		 
		if (pItem)
		{
			pItem->GetOption()->Selected(true);
		}		 
	}
}

void CScreenPaggerUI::OnBtnExpand( TNotifyUI& msg )
{
	CControlUI* pOtherBtn		= this->FindSubControl(_T("thumbnail_pickup"));
	CControlUI* pThubmnailLayout= this->FindSubControl(_T("thumbnail_layout"));
	pThubmnailLayout->SetVisible(true);
	pOtherBtn->SetVisible(true);
	msg.pSender->SetVisible(false); 

	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
	::SetFocus(CPPTController::GetSlideShowViewHwnd());
}

void CScreenPaggerUI::OnBtnPickup( TNotifyUI& msg )
{
	CControlUI* pOtherBtn		= this->FindSubControl(_T("thumbnail_expand"));
	CControlUI* pThubmnailLayout= this->FindSubControl(_T("thumbnail_layout"));
	pThubmnailLayout->SetVisible(false);
	pOtherBtn->SetVisible(true);
	msg.pSender->SetVisible(false);

	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
	::SetFocus(CPPTController::GetSlideShowViewHwnd());
}


void CScreenPaggerUI::OnBtnPreAction( TNotifyUI& msg )
{
	if (m_nCurrentPPTPage>0){
		m_nCurrentPPTPage--;
	}
	OnPageChangeBefore();
	PreviousSlideByThread();
	KillExeCheckParentPid(COURSE_PLAYER_EXE_NAME,true);
}

void CScreenPaggerUI::OnBtnNextAction( TNotifyUI& msg )
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL nFind = pMainFrame->CloseVRMediaPlayer();

	int nViewIndex = CPPTController::GetShowViewIndex();
	int ViewCount = CPPTController::GetShowViewCount();
	if (nViewIndex!=m_nCurrentPPTPage){
		m_nCurrentPPTPage = nViewIndex;
	}else if (nViewIndex==ViewCount){
		m_nCurrentPPTPage++;
	}else{
		m_nCurrentPPTPage = nViewIndex;
	}
	if (m_nCurrentPPTPage>ViewCount){
		m_nCurrentPPTPage = nViewIndex;
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_TITLEBUTTON_OPERATION, MSG_PPT_ENDSTOP, 0);
	}else{
		OnPageChangeBefore();
		NextSlideByThread();

		KillExeCheckParentPid(COURSE_PLAYER_EXE_NAME,true);
	}
}

void CScreenPaggerUI::OnBtnPrePage( TNotifyUI& msg )
{
// 	if (GetTickCount() - m_dwLastClickTickCount  < 200)
// 	{
// 		return;
// 	}
// 	m_dwLastClickTickCount = GetTickCount();
// 
// 	OnPageChangeBefore();
// 
// 	int nPage = m_nCurrentPage - 1;
// 	if (nPage <= 0)
// 	{
// 		return;
// 	}
// 
// 	CScreenThumbItemUI* pItem = dynamic_cast<CScreenThumbItemUI*>(m_pItemList->GetItemAt(nPage - 1));
// 	if (pItem)
// 	{
// 		this->GetManager()->SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK);
// 	}
	
	m_nCurrentPage --;
	m_nCurrentPPTPage = m_nCurrentPage;
	if (m_nCurrentPage < 0)
	{
		m_nCurrentPage = 0;
		return;
	}
	SwitchPage(m_nCurrentPage);
}

void CScreenPaggerUI::OnBtnNextPage( TNotifyUI& msg )
{
	m_nCurrentPage ++;
	m_nCurrentPPTPage = m_nCurrentPage;
	if (m_nCurrentPage >= m_nTotalPageCount)
	{
		m_nCurrentPage = m_nTotalPageCount - 1;
		return;
	}
	SwitchPage(m_nCurrentPage);
}

void CScreenPaggerUI::SwitchPage( int nPage )
{
	if (!m_pItemList)
		return;

	SIZE sz = m_pItemList->GetScrollPos();
	sz.cx = (ScreenThumbItem_Width + m_pItemList->GetChildPadding()) * nPage * ScreenThumbPage_Visibles;
	if (sz.cx < 0)
	{
		sz.cx = 0;
	}
	m_pItemList->SetScrollPos(sz);

	if (GetTickCount() - m_dwLastClickTickCount  < 200)
	{
		return;
	}
	
	this->GetManager()->KillTimer(m_pItemList, DelayLoad_Thumbnail_TimerId);
	this->GetManager()->SetTimer(m_pItemList, DelayLoad_Thumbnail_TimerId, 300);

	ShowSelectIndex();//更新索引值
}

void CScreenPaggerUI::SetPageCount( int nCount )
{
	m_nTotalLeafCount	= nCount;
	m_nTotalPageCount	= nCount / ScreenThumbPage_Visibles;
	if (nCount % ScreenThumbPage_Visibles != 0)
	{
		m_nTotalPageCount += 1;
	}

	GetScanner()->Init(nCount);

}

void CScreenPaggerUI::UpdateItemListScrollPos()
{
	if (!m_pItemList)
		return;

	SIZE sz = m_pItemList->GetScrollPos();
	sz.cx = (ScreenThumbItem_Width + m_pItemList->GetChildPadding()) *((m_nCurrentLeaf  + ScreenThumbPage_Visibles - 1)/ ScreenThumbPage_Visibles  -  1) * ScreenThumbPage_Visibles;
	if (sz.cx < 0)
	{
		sz.cx = 0;
	}
	m_pItemList->SetScrollPos(sz);

	int i = 0;
	int nCount = 0;
	CScreenThumbItemUI* pItem = NULL;
	CVerticalLayoutUI* pVerLayout = NULL;

	// 后几页缩略图生成
	for (i=m_nCurrentLeaf; nCount<ScreenThumbPage_Visibles&&i<=m_nTotalLeafCount; i++, nCount++)
	{ 
		CScreenThumbItemUI* pItem  =  dynamic_cast<CScreenThumbItemUI*>(m_pItemList->GetItemAt(i-1)); 
		if (pItem)
		{
			pItem->DownloadThumbnailEx();
		}
		 
	}

	nCount = 0;
	// 前几页缩略图生成
	for (i=m_nCurrentLeaf; nCount<ScreenThumbPage_Visibles&&i>0; i--, nCount++)
	{
		CScreenThumbItemUI* pItem  =  dynamic_cast<CScreenThumbItemUI*>(m_pItemList->GetItemAt(i-1)); 
		if (pItem)
		{
			pItem->DownloadThumbnailEx();
		}
	}
}

void CScreenPaggerUI::SetCurrentPage( int nCurrentPage )
{
	m_nCurrentLeaf = nCurrentPage;

	if (m_pItemList)
	{ 
		CScreenThumbItemUI* pItem  =  dynamic_cast<CScreenThumbItemUI*>(m_pItemList->GetItemAt(nCurrentPage-1)); 
		if (pItem)
		{
			pItem->GetOption()->Selected(true);
		}	 
	}

	m_nCurrentPage = (m_nCurrentLeaf + ScreenThumbPage_Visibles - 1) / ScreenThumbPage_Visibles - 1;

	UpdateItemListScrollPos();
	
	OnPageChanged(nCurrentPage);

	GetScanner()->SetCurrentPage(nCurrentPage);
	if (GetScanner()->HasScannedAt(nCurrentPage))
	{
		OnPageScanneded();
	}
	else
	{
		GetScanner()->ScanActiveXAt(nCurrentPage);
	}
	ShowSelectIndex();
}

CPageActiveXScanner* CScreenPaggerUI::GetScanner()
{
	return &m_PageScaner;
}

int CScreenPaggerUI::GetPageCount()
{
	return m_nTotalLeafCount;
}

void CScreenPaggerUI::SetPageActiveX( CStream* pStream )
{
	GetScanner()->SetActiveX(pStream);
	OnPageScanneded();
}

void CScreenPaggerUI::SetInsideMode( bool bInsideMode )
{
	m_bInsideMode = bInsideMode;
}

bool CScreenPaggerUI::OnListEvent( void* pObj )
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == Vaild_ScroolPos_TimerId)
		{
			if (m_pItemList)
			{
				RECT rt = m_pItemList->GetPos();
				if (!(rt.bottom == 0 
					&& rt.top == 0
					&& rt.left == 0
					&& rt.right == 0))
				{
					SIZE sz = m_pItemList->GetScrollPos();
					sz.cx = (ScreenThumbItem_Width + m_pItemList->GetChildPadding()) *((m_nCurrentLeaf  + ScreenThumbPage_Visibles - 1)/ ScreenThumbPage_Visibles  -  1) * ScreenThumbPage_Visibles;
					if (sz.cx < 0)
					{
						sz.cx = 0;
					}
					m_pItemList->SetScrollPos(sz);

					GetManager()->KillTimer(m_pItemList, Vaild_ScroolPos_TimerId);
				}
			}
		}
		else if (pEvent->wParam == DelayLoad_Thumbnail_TimerId)
		{
			CVerticalLayoutUI* pVerLayout = NULL;
			CScreenThumbItemUI* pItem	= NULL;
			int			nCount			= ScreenThumbPage_Visibles;
			int			nLeafIndex		= m_nCurrentPage * ScreenThumbPage_Visibles + (ScreenThumbPage_Visibles - 1);
			if (nLeafIndex > m_nTotalLeafCount)
			{
				nLeafIndex = m_nTotalLeafCount - 1;
			}

			nLeafIndex -= (ScreenThumbPage_Visibles - 1);

			while(nLeafIndex < m_nTotalLeafCount
				&& nLeafIndex > 0
				&& nCount > 0)
			{ 
				pItem  =  dynamic_cast<CScreenThumbItemUI*>(m_pItemList->GetItemAt(nLeafIndex)); 
				if (pItem)
				{
					pItem->DownloadThumbnailEx();
				} 
				
				nCount--;
				nLeafIndex ++;
			}

			this->GetManager()->KillTimer(m_pItemList, DelayLoad_Thumbnail_TimerId);
		}
	}
	return true;
}

void CScreenPaggerUI::AddPageListener( IPageListener* pPageListener )
{
	if (pPageListener)
	{
		m_vctrPageListener.push_back(pPageListener);
	}
	
}

void CScreenPaggerUI::RemovePageListener( IPageListener* pPageListener )
{
	if (pPageListener)
	{
		for (vector<IPageListener*>::iterator itor = m_vctrPageListener.begin(); itor != m_vctrPageListener.end(); ++itor)
		{
			IPageListener* pListener = (IPageListener*)*itor;
			if (pListener == pPageListener)
			{
				m_vctrPageListener.erase(itor);
				break;
			}
		}
	}
}

void CScreenPaggerUI::OnPageChangeBefore()
{
	for (vector<IPageListener*>::iterator itor = m_vctrPageListener.begin(); itor != m_vctrPageListener.end(); ++itor)
	{
		IPageListener* pListener = (IPageListener*)*itor;
		if (pListener)
		{
			pListener->OnPageChangeBefore();
		}
	}
}

void CScreenPaggerUI::OnPageChanged( int nPageIndex )
{
	for (vector<IPageListener*>::iterator itor = m_vctrPageListener.begin(); itor != m_vctrPageListener.end(); ++itor)
	{
		IPageListener* pListener = (IPageListener*)*itor;
		if (pListener)
		{
			pListener->OnPageChanged(nPageIndex);
		}
	}
}

void CScreenPaggerUI::OnPageScanneded()
{
	for (vector<IPageListener*>::iterator itor = m_vctrPageListener.begin(); itor != m_vctrPageListener.end(); ++itor)
	{
		IPageListener* pListener = (IPageListener*)*itor;
		if (pListener)
		{
			pListener->OnPageScanneded();
		}
	}
}

bool CScreenPaggerUI::OnItemClick( void* pObj )
{
	OnPageChangeBefore();
	ShowSelectIndex();
	return true;
}	

void CScreenPaggerUI::ShowSelectIndex()
{
	CControlUI* pOtherBtn		= this->FindSubControl(_T("thumbnail_pickup"));  
	TCHAR szPageForamt[]	= _T("{f 160100}{c #f97821}%d{/c}{/f} {f 140100}{c #ffffff}/%d{/c}{/f}"); 
	TCHAR szText[256]	= {0};
	_stprintf_s(szText, szPageForamt, m_nCurrentLeaf, m_nTotalLeafCount);
	((CButtonUI*)pOtherBtn)->SetShowHtml();
	pOtherBtn->SetText(szText); 

	//收起也要显示页码 2016.03.09 cws
	pOtherBtn	= this->FindSubControl(_T("thumbnail_expand"));   
	((CButtonUI*)pOtherBtn)->SetShowHtml();
	pOtherBtn->SetText(szText); 
}
