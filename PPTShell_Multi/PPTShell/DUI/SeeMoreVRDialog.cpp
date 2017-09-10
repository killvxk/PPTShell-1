#include "StdAfx.h"
#include "DUI/GifAnimUI.h"
#include "DUI/SeeMoreVRDialog.h"

//VRItemEx
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "DUI/DragDialogUI.h"
#include "DUI/ResourceItemEx.h"
#include "DUI/VRItemEx.h"

//IComponent
#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/CloudComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ToolbarComponent.h"

//Styleable
#include "DUI/ResourceStyleable.h"
#include "DUI/VRResourceStyleable.h"

//handler
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
#include "DUI/CloudVRResourceHandler.h"


#define ClassifyLayout_Height 38
CSeeMoreVRDialogUI::CSeeMoreVRDialogUI(void)
{
	m_pCurrentSelect = NULL;

}

CSeeMoreVRDialogUI::~CSeeMoreVRDialogUI(void)
{
	//release stream
	for (int i = 0; i < m_pClassifyList->GetCount(); ++i)
	{
		CControlUI* pItem	= m_pClassifyList->GetItemAt(i);
		CStream*	pStream	= (CStream*)pItem->GetTag();
		if (pStream)
		{
			delete pStream;
			pItem->SetTag(NULL);
		}
	}

	for (int i = 0; i < m_pExpandedList->GetCount(); ++i)
	{
		CContainerUI* pContainer =(CContainerUI*)m_pExpandedList->GetItemAt(i);
		for (int i = 0; i < pContainer->GetCount(); ++i)
		{
			CControlUI* pItem	= pContainer->GetItemAt(i);
			CStream*	pStream	= (CStream*)pItem->GetTag();
			if (pStream)
			{
				delete pStream;
				pItem->SetTag(NULL);
			}
		}
	}
}

CControlUI* CSeeMoreVRDialogUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("GifAnim"), pstrClass) == 0)
	{
		return new DuiLib::CGifAnimUI;
	}
	return NULL;
}

DuiLib::CDuiString CSeeMoreVRDialogUI::GetSkinFolder()
{
	return _T("skins");
}

DuiLib::CDuiString CSeeMoreVRDialogUI::GetSkinFile()
{
	return _T("Preview\\SeeMoreVR.xml");
}

LPCTSTR CSeeMoreVRDialogUI::GetWindowClassName( void ) const
{
	return _T("CSeeMoreVRDialogUI");
}

void CSeeMoreVRDialogUI::InitWindow()
{
	__super::InitWindow();

	RECT rtParaent = {0};
	GetWindowRect(GetWindowOwner(GetHWND()), &rtParaent);

	MoveWindow(GetHWND(), rtParaent.left, rtParaent.top, rtParaent.right - rtParaent.left, rtParaent.bottom - rtParaent.top, TRUE);

	m_pVRListLay			= (CContainerUI*)m_PaintManager.FindControl(_T("lay_vr_list"));
	m_pVRList				= (CTileLayoutUI*)m_PaintManager.FindControl(_T("vr_list"));
	m_pVRListMask			= (CContainerUI*)m_PaintManager.FindControl(_T("vr_list_mask"));
	m_pVRListLoding			= (DuiLib::CGifAnimUI*)m_pVRListMask->FindSubControl(_T("loading"));
	m_pClassifyList			= (CContainerUI*)m_PaintManager.FindControl(_T("classify_list"));
	m_pClassifyListMask		= (CContainerUI*)m_PaintManager.FindControl(_T("classify_list_mask"));
	m_pClassifyListLoding	= (DuiLib::CGifAnimUI*)m_pVRListMask->FindSubControl(_T("loading"));
	m_pExpandLay			= (CContainerUI*)m_PaintManager.FindControl(_T("lay_expand"));
	m_pCollapseLay			= (CContainerUI*)m_PaintManager.FindControl(_T("lay_collapse"));
	m_pExpandedListLay		= (CContainerUI*)m_PaintManager.FindControl(_T("lay_classification_expanded"));
	m_pExpandedList			= (CContainerUI*)m_PaintManager.FindControl(_T("classify_list_more"));
	m_pClassifyListLay		= (CContainerUI*)m_PaintManager.FindControl(_T("lay_classification"));
	m_pSearchCtrl			= (CRichEditUI*)m_PaintManager.FindControl(_T("edit_search_vr"));
	m_pSearchCtrl->SetWantReturn(false);
	//get classify data
	GetClassifyInfo();

}

void CSeeMoreVRDialogUI::ShowDialog()
{
	if (!GetHWND())
	{
		Create(AfxGetMainWnd()->GetSafeHwnd(), GetWindowClassName(), WS_OVERLAPPED, 0);
	}

	ShowModal();

}

void CSeeMoreVRDialogUI::OnBtnSearch( TNotifyUI& msg )
{
	tstring strSearchText = m_pSearchCtrl->GetText().GetData();
	if (trim(strSearchText).empty())
	{
		return;
	}

	tstring strCode;
	if (m_pCurrentSelect)
	{
		strCode = m_pCurrentSelect->GetUserData().GetData();
	}

	tstring strUrl = NDCloudComposeUrlVRResourceInfo(_T(""), strSearchText.c_str(), strCode, 0, 100);

	NDCloudDownload(strUrl,
		MakeHttpDelegate(this, &CSeeMoreVRDialogUI::OnGetSearchResultCompleted),
		MakeHttpDelegate(this, &CSeeMoreVRDialogUI::OnGetSearchResultProgress));

}

void CSeeMoreVRDialogUI::OnBtnClose( TNotifyUI& msg )
{
	Close();
}

void CSeeMoreVRDialogUI::OnBtnExpand( TNotifyUI& msg )
{
	m_pExpandLay->SetVisible(false);
	m_pCollapseLay->SetVisible(true);

	m_pExpandedListLay->SetVisible(true);
	m_pClassifyListLay->SetFixedHeight(ClassifyLayout_Height + m_pExpandedListLay->GetFixedHeight());

}

void CSeeMoreVRDialogUI::OnBtnCollapse( TNotifyUI& msg )
{
	m_pExpandLay->SetVisible(true);
	m_pCollapseLay->SetVisible(false);

	m_pExpandedListLay->SetVisible(false);
	m_pClassifyListLay->SetFixedHeight(ClassifyLayout_Height);
}

void CSeeMoreVRDialogUI::OnBtnClassifyItem( TNotifyUI& msg )
{
	CControlUI* pItem	= msg.pSender;
	m_pCurrentSelect	= pItem;
	CStream*	pStream = (CStream*)pItem->GetTag();
	if (pStream)
	{
		CreateVRList(pStream);
	}
	else
	{
		tstring strUrl = NDCloudComposeUrlVRResourceInfo(_T(""), _T(""), pItem->GetUserData().GetData(), 0, 100);

		NDCloudDownload(strUrl,
			MakeHttpDelegate(this, &CSeeMoreVRDialogUI::OnGetClassifyResultCompleted),
			MakeHttpDelegate(this, &CSeeMoreVRDialogUI::OnGetSearchResultProgress),
			pItem);
	}
}

bool CSeeMoreVRDialogUI::OnClassfyItemSelect( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	COptionUI* pOption = (COptionUI*)pNotify->pSender;
	if (pOption->IsSelected())
	{
		pOption->SetTextColor(0xFFFF2524);
	}
	else
	{
		pOption->SetTextColor(0xFF0FB0B6);
	}

	return true;
}

void CSeeMoreVRDialogUI::GetClassifyInfo()
{
	tstring strUrl = NDCloudComposeUrlCagegoryInfo(_T(""), 0, 100);

	NDCloudDownload(strUrl,
		MakeHttpDelegate(this, &CSeeMoreVRDialogUI::OnGetClassifyInfoCompleted),
		MakeHttpDelegate(this, &CSeeMoreVRDialogUI::OnGetClassifyInfoProgress));
}

bool CSeeMoreVRDialogUI::OnGetClassifyInfoProgress( void* pObj )
{
	m_pClassifyListMask->SetVisible(true);
	m_pClassifyListLoding->PlayGif();
	return true;
}

bool CSeeMoreVRDialogUI::OnGetClassifyInfoCompleted( void* pObj )
{
	m_pClassifyListMask->SetVisible(false);
	m_pClassifyListLoding->StopGif();

	THttpNotify* pHttpNotify = (THttpNotify*)pObj;
	if (pHttpNotify->dwErrorCode != 0)
	{
		return true;
	}

	CStream cStream(1024);
	if (!NDCloudDecodeCategoryList(pHttpNotify->pData, pHttpNotify->nDataSize, &cStream))
	{
		return true;
	}

	CreateClassifyList(&cStream);
	return true;
}

void CSeeMoreVRDialogUI::CreateClassifyList( CStream* pStream )
{
	m_pClassifyList->RemoveAll();
	pStream->ResetCursor();

	int nCount = pStream->ReadInt();
	if (nCount <= 0)
	{
		return;
	}

	CItemOptionUI* pItemAll = new CItemOptionUI;
	pItemAll->SetName(_T("item_classify"));
	pItemAll->SetGroup(_T("item_group"));
	pItemAll->SetTextColor(0xFF0FB0B6);
	pItemAll->SetText(_T("全部"));
	pItemAll->OnSelect += MakeDelegate(this, &CSeeMoreVRDialogUI::OnClassfyItemSelect);
	SIZE textSize = CRenderEngine::GetTextSize(m_PaintManager.GetPaintDC(), &m_PaintManager, pItemAll->GetText(), pItemAll->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
	pItemAll->SetFixedWidth(textSize.cx);
	m_pClassifyList->Add(pItemAll);


	bool	bFirstRow	= true;
	int		nWidthCount = textSize.cx;
	CHorizontalLayoutUI* pRowLay = NULL;
	for (int i = 0; i < nCount; ++i)
	{
		CItemOptionUI* pItem = new CItemOptionUI;
		pItem->SetName(_T("item_classify"));
		pItem->SetGroup(_T("item_group"));
		pItem->SetTextColor(0xFF0FB0B6);
		pItem->OnSelect += MakeDelegate(this, &CSeeMoreVRDialogUI::OnClassfyItemSelect);

		//skip guid
		pStream->ReadString();

		pItem->SetText(pStream->ReadString().c_str());
		pItem->SetUserData(pStream->ReadString().c_str());

		//calc text size

		SIZE textSize = CRenderEngine::GetTextSize(m_PaintManager.GetPaintDC(), &m_PaintManager, pItem->GetText(), pItem->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
		pItem->SetFixedWidth(textSize.cx);

		if (bFirstRow)
		{
			nWidthCount += m_pClassifyList->GetChildPadding();
			nWidthCount += textSize.cx;
			if (nWidthCount < m_pClassifyList->GetFixedWidth())
			{
				m_pClassifyList->Add(pItem);
			}
			else
			{
				pRowLay = new CHorizontalLayoutUI;
				pRowLay->SetAttribute(_T("inset"), _T("30,0,0,0"));
				pRowLay->SetChildPadding(m_pClassifyList->GetChildPadding());
				pRowLay->SetFixedWidth(m_pClassifyList->GetFixedWidth());
				pRowLay->SetFixedHeight(ClassifyLayout_Height);
				m_pExpandedList->Add(pRowLay);

				m_pExpandedListLay->SetFixedHeight(m_pExpandedListLay->GetFixedHeight() + ClassifyLayout_Height);
				
				pRowLay->Add(pItem);
				bFirstRow = false;
				nWidthCount = 30;
				nWidthCount += textSize.cx;
			}
		}
		else
		{
			nWidthCount += pRowLay->GetChildPadding();
			nWidthCount += textSize.cx;

			if (nWidthCount >= pRowLay->GetFixedWidth())
			{
				pRowLay = new CHorizontalLayoutUI;
				pRowLay->SetAttribute(_T("inset"), _T("30,0,0,0"));
				pRowLay->SetChildPadding(m_pClassifyList->GetChildPadding());
				pRowLay->SetFixedWidth(m_pClassifyList->GetFixedWidth());
				pRowLay->SetFixedHeight(ClassifyLayout_Height);
				m_pExpandedList->Add(pRowLay);

				m_pExpandedListLay->SetFixedHeight(m_pExpandedListLay->GetFixedHeight() + ClassifyLayout_Height);

				nWidthCount = 30;
				nWidthCount += textSize.cx;
			}

			pRowLay->Add(pItem);
		}
		
	}

	//notify default select
	pItemAll->Selected(true);
	m_PaintManager.SendNotify(pItemAll, DUI_MSGTYPE_CLICK, 0, 0, true);
}	

bool CSeeMoreVRDialogUI::OnGetSearchResultProgress( void* pObj )
{
	m_pVRListMask->SetVisible(true);
	m_pVRListLoding->PlayGif();
	return true;
}

bool CSeeMoreVRDialogUI::OnGetSearchResultCompleted( void* pObj )
{
	m_pVRListMask->SetVisible(false);
	m_pVRListLoding->StopGif();

	THttpNotify* pHttpNotify = (THttpNotify*)pObj;
	if (pHttpNotify->dwErrorCode != 0)
	{
		return true;
	}

	CStream cStream(1024);
	if (!NDCloudDecodeVRResourceList(pHttpNotify->pData, pHttpNotify->nDataSize, &cStream))
	{
		CToast::Toast(_T("搜索失败,可能含有敏感关键字"));
		return true;
	}

	CreateVRList(&cStream);
	return true;
}

bool CSeeMoreVRDialogUI::OnGetClassifyResultCompleted( void* pObj )
{
	m_pVRListMask->SetVisible(false);
	m_pVRListLoding->StopGif();

	THttpNotify* pHttpNotify = (THttpNotify*)pObj;
	if (pHttpNotify->dwErrorCode != 0)
	{
		return true;
	}

	CStream* pStream = new CStream(1024);
	if (!NDCloudDecodeVRResourceList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
	{
		return true;
	}

	CControlUI* pItem = (CControlUI*)pHttpNotify->pUserData;
	pItem->SetTag((UINT_PTR)pStream);
	CreateVRList(pStream);

	return true;
}

void CSeeMoreVRDialogUI::CreateVRList( CStream* pStream )
{
	//clear list
	m_pVRList->RemoveAll();
	pStream->ResetCursor();

	int nCount = pStream->ReadInt();
	if (nCount <= 0)
	{
		return;
	}

	for (int i = 0; i < nCount; ++i)
	{
		IComponent* pCompnent	= NULL;
		CToolbarComponent* pToolbarCompnent	= new CToolbarComponent(&CVRResourceStyleable::Styleable);
		pToolbarCompnent->SetNeedInterposeBtn(true);
		pCompnent				= new CMaskComponent(pToolbarCompnent);
		pCompnent				= new CItemComponent(pCompnent);
		pCompnent				= new CCloudComponent(pCompnent);
		CVRItemExUI*pItem		= new CVRItemExUI(pCompnent);
		pItem->SetHandler(new CCloudVRResourceHandler());
		pItem->ReadStream(pStream);

		m_pVRList->Add(pItem);
		pItem->SetItemHeight(135);
		pItem->SetItemWidth(175);
		pItem->SetTitleColor(0xFF323232);

	}

	int nCol	= m_pVRList->GetColumns();
	int nRow	= nCount / nCol;
	if (nCount % nCol != 0)
	{
		nRow++;
	}

	//update lay height for scroll
	m_pVRList->SetFixedHeight(m_pVRList->GetFixedHeight() + (nRow - 3) * (135 + 22));
	m_pVRListLay->SetFixedHeight(m_pVRList->GetFixedHeight());
}
