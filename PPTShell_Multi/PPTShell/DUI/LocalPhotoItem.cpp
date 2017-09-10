#include "StdAfx.h"
#include "LocalPhotoItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "PreviewDialogUI.h"
#include "Util/Util.h"
#include "Statistics/Statistics.h"
#include "ItemExplorer.h"

CLocalPhotoItemUI::CLocalPhotoItemUI()
{

}

CLocalPhotoItemUI::~CLocalPhotoItemUI()
{

}


void CLocalPhotoItemUI::Init()
{
	__super::Init();
	SetContentHeight(110);
	if (_tcsicmp(GetResource(), _T("")) != 0)
	{
		Gdiplus::Image* pImage = new Image(AnsiToUnicode(GetResource()).c_str());
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();

		if( nWidth > 1000 || nHeight > 1000 )
		{
			WCHAR wszTempPath[MAX_PATH];
			GetTempPathW(MAX_PATH, wszTempPath);

			DWORD dwCrc = CalcFileCRC(GetResource());

			WCHAR wszTempFileName[MAX_PATH];
			swprintf_s(wszTempFileName, L"%s\\thumbnail_%x.jpg", wszTempPath, dwCrc);

			DWORD dwRet = GetFileAttributesW(wszTempFileName);

			BOOL bSucess = TRUE;
			if (dwRet == INVALID_FILE_ATTRIBUTES)
			{
				bSucess = CompressImagePixel(Str2Unicode(GetResource()).c_str(), wszTempFileName, 300, 200);
			}

			if ( bSucess )
				SetImage(Un2Str(wszTempFileName).c_str());
		}
		else
		{
			SetImage(GetResource());
		}

		delete pImage;
		
	}
	else
	{
		SetImage(_T("RightBar\\Item\\item_bg_image.png"));
	}	

	SetIcon(_T("RightBar\\Item\\bg_tit_pic.png"));
}

void CLocalPhotoItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	if( !IsResourceExist() )
	{
		CToast::Toast(_STR_LOCAL_ITEM_TIP_FILE_NO_EXIST, false, 1000);
		return;
	}

	if( nButtonIndex == 0 )
	{
		wstring str = Str2Unicode(GetResource());
		Gdiplus::Image* pImage = new Image(str.c_str());
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();

		delete pImage;

		if( m_ptDragEndPos.x != -1 && m_ptDragEndPos.y != -1 )		
		{
			int x = m_ptDragEndPos.x ;
			int y = m_ptDragEndPos.y ;

			InsertPictureByThread(GetResource(), x, y, nWidth, nHeight);
		}
		else
			InsertPictureByThread(GetResource(), -1, -1, nWidth, nHeight);

		Statistics::GetInstance()->Report(STAT_INSERT_PICTURE);
	}	
	else if (1 == nButtonIndex)//Ô¤ÀÀÍ¼Æ¬
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
		pPreviewDlg->ShowPreviewWindow(&stream, E_WND_PICTURE_LOCAL);
		pPreviewDlg->ShowModal();
		SetFocus();

		Statistics::GetInstance()->Report(STAT_PREIVIEW_PICUTURE);


	}

}

void CLocalPhotoItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
	if (m_OnItemClick)
	{
		m_OnItemClick(this);
	}
}
