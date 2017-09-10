#include "stdafx.h"
#include "ItemExplorer.h"
#include "CourseSubItem.h"
#include "LocalCourseSubItem.h"
#include "GroupExplorer.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ToolbarComponent.h"
#include "DUI/CloudComponent.h"
#include "DUI/ThumbnailItem.h"
#include "DUI/ResourceItemEx.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/PhotoStyleable.h"


#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/CloudCourseHandler.h"
#include "DUI/CloudCourseSubHandler.h"


CItemExplorerUI* CItemExplorerUI::m_pInstance = NULL;
CItemExplorerUI* CItemExplorerUI::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CItemExplorerUI;
	}
	return m_pInstance;
}


CItemExplorerUI::CItemExplorerUI()
{	
	m_nItemExplorerType = eItemExplorer_Cloud;
	m_pParentItem = NULL;

}
CItemExplorerUI::~CItemExplorerUI()
{

}

LPCTSTR CItemExplorerUI::GetWindowClassName() const
{
	return _T("CItemExplorerUI"); 
}

UINT CItemExplorerUI::GetClassStyle() const
{
	return CS_DBLCLKS;
}

void CItemExplorerUI::InitWindow()
{
	__super::InitWindow();

	this->SetBkColor(0xFF3F3F3F);
	this->SetFixedWidth(250);
	
}


CControlUI* CItemExplorerUI::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("ResourceItem")) == 0 )
	{
		if (m_nItemExplorerType == eItemExplorer_Cloud)
		{
			return new CCourseSubItemUI;
		}
		else if (m_nItemExplorerType == eItemExplorer_Local)
		{
			return new CLocalCourseSubItemUI;
		}
	}
		
	return __super::CreateControl(pstrClass);
}

void CItemExplorerUI::OnBtnClose( TNotifyUI& msg )
{
	//this->ShowWindow(false);	
	if (CPPTController::GetPPTVersion() == PPTVER_2007)		// fix bug:9846 【交互】插入工作表后，关闭BC框，界面异常
	{
		GetPPTController()->ActivePPTWnd();
	}
}


void CItemExplorerUI::ShowResource( int nType, CStream* pStream, void* pParams )
{
	if (nType == eItemExplorer_Cloud)
	{
		m_pList->SetDelayedDestroy(false);
		m_pList->RemoveAll();
		m_pList->Invalidate();
		m_pList->GetVerticalScrollBar()->SetScrollPos(0);

		pStream->ResetCursor();
		int nCount = pStream->ReadInt();
		SetCountText(nCount);
		for (int i = 0; i < nCount; i++)
		{
			IComponent* pCompnent	= new CToolbarComponent(&CPhotoStyleable::Styleable);
			pCompnent				= new CMaskComponent(pCompnent);
			pCompnent				= new CItemComponent(pCompnent);
			pCompnent				= new CCloudComponent(pCompnent);
			CResourceItemExUI*pItem	= new CResourceItemExUI(pCompnent);

			CCloudCourseSubHandler* pHandler = new CCloudCourseSubHandler();
			pHandler->SetContentType(CONTENT_COURSEWARE);
			pItem->SetHandler(pHandler);
			
			pItem->SetPadding(CDuiRect(6, 0, 6, 0));
			pItem->ReadStream(pStream);
			m_pList->Add(pItem);

			TCHAR szText[32] = {0};
			_sntprintf_s(szText, _countof(szText) - 1, _T("%d / %d"), i + 1, nCount);
			pItem->SetTitle(szText);
			pItem->SetItemHeight(150);
		}
	}
	else
	{
		m_pParentItem = static_cast<CResourceItemUI*>(pParams);

		m_nItemExplorerType = nType;

		m_pList->RemoveAll();
		m_pList->Invalidate();
		//clear last border
		m_pList->SetTag(NULL);
		m_pList->GetVerticalScrollBar()->SetScrollPos(0);

		if (!pStream)
		{
			SetCountText(0);
			return;
		}

		pStream->ResetCursor();
		int nCount = pStream->ReadInt();
		SetCountText(nCount);
		for (int i = 0; i < nCount; i++)
		{
			CResourceItemUI * pItem = CreateItem();
			if (pItem)
			{
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
				CSubItemer* pCourseSubItem = dynamic_cast<CSubItemer*>(pItem);
				pCourseSubItem->SetParentItem((CBaseItemUI*)pParams);
				pCourseSubItem->SetIndex(i);
				pItem->ReadStream(pStream);

				m_pList->Add(pItem);

				// 策划要求修改
				TCHAR szText[32] = {0};
				_sntprintf_s(szText, _countof(szText) - 1, _T("%d / %d"), i + 1, nCount);
				pItem->SetTitle(szText);

			}
		}
	}
	if (CGroupExplorerUI::GetInstance()
		&& ::IsWindowVisible(CGroupExplorerUI::GetInstance()->GetHWND()))
	{
		this->ShowWindow(true);
	}
	else
	{
		this->ShowWindow(false);
	}
}

CResourceItemUI* CItemExplorerUI::CreateItem()
{
	CResourceItemUI * pItem = NULL;

	if( !m_builder.GetMarkup()->IsValid() ) 
	{
		pItem = (CResourceItemUI*)(m_builder.Create(_T("RightBar\\Item\\ResourceItem.xml"), (UINT)0, this, &m_PaintManager));
	}
	else
	{
		pItem = (CResourceItemUI*)(m_builder.Create(this, &m_PaintManager));
	}
	if (pItem == NULL)
		return NULL;
	return pItem;
}

void CItemExplorerUI::ResetUI()
{
	m_pList->RemoveAll();
	SetCountText(0);

}

void CItemExplorerUI::SetParentItem( CResourceItemUI* pParentItem )
{
	m_pParentItem = pParentItem;
}
