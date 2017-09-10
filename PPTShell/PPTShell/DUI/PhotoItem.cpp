#include "StdAfx.h"
#include "PhotoItem.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "Statistics/Statistics.h"
#include "PreviewDialogUI.h"
#include "DragDialogUI.h"
#include "DUI/ItemExplorer.h"


CPhotoItemUI::CPhotoItemUI()
{
	m_dwDownloadId	= 0;
	m_layMaskStyle	= NULL;

}

CPhotoItemUI::~CPhotoItemUI()
{
	NDCloudDownloadCancel(m_dwDownloadId);
}


void CPhotoItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_image.png"));
	SetContentHeight(110);


	if( !m_builder.GetMarkup()->IsValid() ) {
		m_layMaskStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(_T("RightBar\\Item\\ResourceMaskStyle.xml"), (UINT)0, this, this->GetManager()));
	}
	else {
		m_layMaskStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(this, this->GetManager()));
	}
	GetContent()->Add(m_layMaskStyle);


	m_pAnimation= dynamic_cast<CGifAnimUI*>( FindSubControl(_T("loading")));

	SetIcon(_T("RightBar\\Item\\bg_tit_pic.png"));

	DownloadThumbnail();
}

void CPhotoItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	if( nButtonIndex == 0 )
	{
		__super::DownloadResource(nButtonIndex, CloudFileImage, 0);
	}
	else if (1 == nButtonIndex)//预览图片
	{

		CContainerUI* pParaent = (CContainerUI*)this->GetParent();

		CStream stream(1024);
		stream.WriteDWORD((DWORD)pParaent);
		stream.WriteDWORD(pParaent->GetItemIndex(this));
		stream.ResetCursor();

		CRect rect;
		HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
		::GetWindowRect(hwnd,&rect);
		CPreviewDlgUI * pPreviewDlg = new CPreviewDlgUI();
		pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP , 0, 0, 0, 0, 0);
		pPreviewDlg->Init(rect);
		//pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
		//MoveWindow(pPreviewDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
		pPreviewDlg->ShowPreviewWindow(&stream, E_WND_PICTURE);
		pPreviewDlg->ShowModal();
		SetFocus();

		Statistics::GetInstance()->Report(STAT_PREIVIEW_PICUTURE);
	}

}

void CPhotoItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
	if (m_OnItemClick)
	{
		m_OnItemClick(this);
	}
}


void CPhotoItemUI::DownloadThumbnail()
{
	this->StartMask();
	m_dwDownloadId = NDCloudDownloadFile(GetResource(), m_strGuid, GetTitle(), CloudFileImage, 240, MakeHttpDelegate(this, &CPhotoItemUI::OnDownloadThumbnailCompleted));
	if (m_dwDownloadId == 0)
	{
		OnDownloadThumbnailFailed();
		this->StopMask();
		return;
	}
}


bool CPhotoItemUI::OnDownloadThumbnailCompleted( void* pNotify )
{
	this->StopMask();

	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	if (pHttpNotify->strFilePath == _T(""))
	{
		OnDownloadThumbnailFailed();
		return true;
	}

	SetImage(pHttpNotify->strFilePath.c_str());
	return true;

}

void CPhotoItemUI::OnDownloadThumbnailFailed()
{
	SetImage(_T("Rightbar/Item/item_bg_image_none.png"));
	this->EnableToolbar(false);
}

void CPhotoItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	if (nButtonIndex == 0)
	{
		// get image size
		Gdiplus::Image* pImage = new Image(Str2Unicode(lptcsPath).c_str());
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();
		
		delete pImage;

		if( m_ptDragEndPos.x != -1 && m_ptDragEndPos.y != -1 )		
		{
			int x = m_ptDragEndPos.x;
			int y = m_ptDragEndPos.y;

			m_ptDragEndPos.x = -1;
			m_ptDragEndPos.y = -1;

			InsertPictureByThread(lptcsPath, x, y, nWidth, nHeight);
		}
		else
			InsertPictureByThread(lptcsPath, -1, -1, nWidth, nHeight);

		Statistics::GetInstance()->Report(STAT_INSERT_PICTURE);
		
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(lptcsPath);
	}
}

void CPhotoItemUI::OnItemDragFinish()
{
	__super::DownloadResource(0, CloudFileImage, 0);

}

void CPhotoItemUI::StartMask()
{
	m_layMaskStyle->SetVisible(true);
	m_pAnimation->PlayGif();

}

void CPhotoItemUI::StopMask()
{
	m_layMaskStyle->SetVisible(false);
	m_pAnimation->StopGif();

}

CControlUI* CPhotoItemUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return NULL;
}

void CPhotoItemUI::DownloadLocal()
{
	OnDownloadResourceBefore();
	m_nButtonIndex = 0;
	this->ShowProgress(true);
	CStream* pStream = new CStream(1024);
	pStream->WriteString(GetTitle());
	DWORD m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), GetTitle(), CloudFileImage, 240, 
		MakeHttpDelegate(this, &CPhotoItemUI::OnDownloadUrlImageCompelete),MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress),pStream);
	if(m_dwResourceDownloadId==0)
	{
		this->ShowProgress(false);
		tstring strToast=_T("“");
		strToast+=GetTitle();
		strToast+=_T("”");
		strToast+=_T("下载失败");
		CToast::Toast(strToast);
	}
}

bool CPhotoItemUI::OnDownloadUrlImageCompelete(void* pNotify)
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	tstring strTitle = _T("");
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
	if (pHttpNotify->strFilePath != _T(""))
	{
		//广播下载完成
		vector<tstring> arg;
		arg.push_back(strTitle);
		arg.push_back(pHttpNotify->strFilePath);
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)2,(LPARAM)&arg,NULL);
		tstring strToast = _T("“")+ strTitle +_T("”") + _T("下载完成");
		CToast::Toast(strToast);
	}
	else
	{
		tstring strToast = _T("“")+ strTitle +_T("”") + _T("下载失败");
		CToast::Toast(strToast);
	}
	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);
	return true;
}
