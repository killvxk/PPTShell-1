#include "StdAfx.h"
#include "ScreenThumbItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "PPTControl/PPTImagesExporter.h"
#include "PreviewDialogUI.h"
#include "Util/Util.h"
#include "Statistics/Statistics.h"
#include "ItemExplorer.h"

CScreenThumbItemUI::CScreenThumbItemUI()
{
	m_nIndex = 0;
	m_dwExportId = 0;
	m_bEverDownloadImage = false;
}

CScreenThumbItemUI::~CScreenThumbItemUI()
{
	PPTImagesExporter::GetInstance()->CancelExport(m_dwExportId);
}


void CScreenThumbItemUI::Init()
{
	CResourceItemUI::EnableToolbar(false);
	CResourceItemUI::EnableDrag(false);

	__super::Init();

	COptionUI* pOption = GetOption();
	if (pOption)
	{
		pOption->SetAttribute(_T("bkimage"), _T("file='ScreenTool\\thumbnail\\item_bk_normal.png' corner='20,20,20,20'"));
		pOption->SetAttribute(_T("selectedimage"), _T("file='ScreenTool\\thumbnail\\item_bk_pushed.png' corner='20,20,20,20'"));
	}

	CControlUI* pTitle = GetTitleControl();
	if (pTitle)
	{
		pTitle->SetVisible(false);
	} 
	this->SetContentHeight(ScreenThumbItem_Height);
	this->SetFixedWidth(ScreenThumbItem_Width);
	
}

void CScreenThumbItemUI::OnItemClick( TNotifyUI& msg )
{
	if (m_OnItemClick)
	{
		m_OnItemClick(this);
	}
	GotoSlideByThread(m_nIndex + 1);
}

void CScreenThumbItemUI::DownloadThumbnail()
{
// 	StartMask();
// 	m_dwExportId = PPTImagesExporter::GetInstance()->ExportCurrentPPTToImages(MakeDelegate(this, &CScreenThumbItemUI::OnExportCompleted), m_nIndex + 1, m_nIndex + 1);
// 	if (!m_dwExportId)
// 	{
// 		StopMask();
// 	}

}

void CScreenThumbItemUI::DownloadThumbnailEx()
{
	if ( m_bEverDownloadImage )
		return;

	m_bEverDownloadImage = true;

	StartMask();
	m_dwExportId = PPTImagesExporter::GetInstance()->ExportCurrentPPTToImages(MakeDelegate(this, &CScreenThumbItemUI::OnExportCompleted), m_nIndex + 1, m_nIndex + 1);
	if (!m_dwExportId)
	{
		StopMask();
	}
}

bool CScreenThumbItemUI::OnExportCompleted( void* pObj )
{
	ExportNotify* pNotify = (ExportNotify*)pObj;
	StopMask();
	if (pNotify->dwErrorCode != 0)
	{
		return true;
	}

	TCHAR szFileName[MAX_PATH]		= {0};
	TCHAR szFilePath[MAX_PATH * 2]	= {0};

	_stprintf_s(szFileName, "Slide_%d.%s", pNotify->nStartIndex, pNotify->lptcsExt);
	_stprintf_s(szFilePath, "%s\\%s", pNotify->strDir.c_str(), szFileName);

	SetImage(szFilePath); 
 
	return true;
}

void CScreenThumbItemUI::SetIndex( int nIndex )
{
	m_nIndex = nIndex;
} 


