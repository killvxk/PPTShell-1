#include "StdAfx.h"
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

#include "DUI/BaseParamer.h"
#include "DUI/InsertParamer.h"


#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include <Windef.h>
#include "DUI/PreviewDialogUI.h"
#include "Statistics/Statistics.h"

#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "DUI/CourseDownloader.h"
#include "DUI/IButtonTag.h"
#include "DUI/ItemExplorer.h"


CCloudCourseSubHandler::CCloudCourseSubHandler()
{

}


CCloudCourseSubHandler::~CCloudCourseSubHandler()
{

}


void CCloudCourseSubHandler::InitHandlerId()
{
	m_strHandlerId = Ansi2Str(typeid(this).name());
	m_strHandlerId += m_strPreviewUrl;
}

void CCloudCourseSubHandler::ReadFrom( CStream* pStream )
{
	m_nSlideIndex	= pStream->ReadInt();
	m_strTitle		= pStream->ReadString();
	m_strGuid		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	m_strPreviewUrl = pStream->ReadString();
	InitHandlerId();

}

void CCloudCourseSubHandler::WriteTo( CStream* pStream )
{
	pStream->WriteString(m_strTitle);
	pStream->WriteString(m_strGuid);
	pStream->WriteString(m_strUrl);
	pStream->WriteString(m_strPreviewUrl);
}

void CCloudCourseSubHandler::DoClick( TNotifyUI* pNotify )
{
	CItemHandler::DoClick(pNotify);

}

void CCloudCourseSubHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Insert)
	{
		//insert
		DoDropDown(pNotify);
	}
	else if (pNotify->wParam == eClickFor_Preview)
	{
		//preview
 		CContainerUI*	pParaent	= (CContainerUI*)GetTrigger()->GetParent();
		int				nCount		= pParaent->GetCount();
 		CStream stream(1024);
 		stream.WriteDWORD(pParaent->GetItemIndex(GetTrigger()));
		stream.WriteDWORD(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			IHandlerVisitor* pHandlerVisitor	= dynamic_cast<IHandlerVisitor*>(pParaent->GetItemAt(i));
			if (pHandlerVisitor)
			{
				CCloudCourseSubHandler* pHandler	= (CCloudCourseSubHandler*)pHandlerVisitor->GetHandler();
				pHandler->WriteTo(&stream);
			}
		}	
 		stream.ResetCursor();
 
 		CRect rect;
 		HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
 		::GetWindowRect(hwnd, &rect);
 		CPreviewDlgUI * pPreviewDlg = new CPreviewDlgUI();
 		pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP , 0, 0, 0, 0, 0);
 		pPreviewDlg->Init(rect);
 		pPreviewDlg->ShowPreviewWindow(&stream, E_WND_COURSE);
 		pPreviewDlg->ShowModal();
 		GetTrigger()->SetFocus();
 
 		Statistics::GetInstance()->Report(STAT_PREVIEW_THUMBNAIL);
	}

}

void CCloudCourseSubHandler::InsertPPT( void* pObj )
{
	CInsertParamer* pParamer = (CInsertParamer*)pObj;
	InsertPPTByThread(
		pParamer->GetHttpNotify()->strFilePath.c_str(),
		m_nSlideIndex,
		m_nSlideIndex,
		0,
		pParamer->GetSlideId(),
		pParamer->GetPlaceHolderId());

	Statistics::GetInstance()->Report(STAT_INSERT_PPT);
}
