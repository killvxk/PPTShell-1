#include "StdAfx.h"
#include "LocalItem.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "PreviewDialogUI.h"
#include "ItemExplorer.h"

CLocalItemUI::CLocalItemUI()
{
}

CLocalItemUI::~CLocalItemUI()
{
}


void CLocalItemUI::InitItem()
{
	if (m_nType == RES_TYPE_COURSE)
	{
		CControlUI* pCtrl = FindSubControl(_T("btn2"));
		if (pCtrl)
		{
			pCtrl->SetVisible(false);
		}
	}


	__super::InitItem();
}


void CLocalItemUI::InsertPPTOperate()
{
	
	else if (m_nType == RES_TYPE_VOLUME )
	{
		POINTF pt;
		pt.x = 5.0f;
		pt.y = 5.0f;

		CPPTController* pPttController = GetPPTController();
		POINT ptScreen = pPttController->SlidePosToScreenPixel(pt);
		InsertVideoByThread(m_strUrl.GetData(), ptScreen.x, ptScreen.y, 40, 40);
	}
}

void CLocalItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	if (nButtonIndex == 0)
	{
		InsertPPTOperate();
	}
	else if ( nButtonIndex == 1)
	{
		else if ( m_nType == RES_TYPE_VIDEO || m_nType == RES_TYPE_VOLUME)
		{
			OpenAsDefaultExec(m_strUrl.GetData());
		}
	}
}


void CLocalItemUI::OnItemClick( TNotifyUI& msg )
{
// 	if(m_nType == RES_TYPE_COURSE)
// 		CItemExplorerUI::GetInstance()->ShowResource(0, (CStream*)this->GetTag(), this);
}

void CLocalItemUI::SetResourceUrl( LPCTSTR lptcsResourceUrl )
{
	__super::SetResourceUrl(lptcsResourceUrl);

	if(m_nType == RES_TYPE_VIDEO)
	{
		m_lbImage->SetBkImage("RightBar\\Item\\item_bg_vedio.png");
	}
	else if(m_nType == RES_TYPE_VOLUME)
	{
		m_lbImage->SetBkImage("RightBar\\Item\\item_bg_vedio.png");
	}
	else if (m_nType == RES_TYPE_COURSE)
	{
		m_lbImage->SetBkImage("RightBar\\Item\\item_bg_course.png");
	}
	else if (m_nType == RES_TYPE_PIC)
	{
		int width = GetWidth();
		int height = GetHeight() - m_lbName->GetHeight();

		tstring strImagePath = lptcsResourceUrl;

		Gdiplus::Image* pImage = new Image(AnsiToUnicode(strImagePath).c_str());
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();

		if( nWidth > 1000 || nHeight > 1000 )
		{
			WCHAR wszTempPath[MAX_PATH];
			GetTempPathW(MAX_PATH, wszTempPath);

			WCHAR wszTempFileName[MAX_PATH];
			swprintf(wszTempFileName, L"%s\\%08lX.png", wszTempPath, GetTickCount());

			GUID png = {0x557CF406, 0x1A04, 0x11D3, 0x9A, 0x73, 0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E};

			Gdiplus::Image* pThumbnail = pImage->GetThumbnailImage(width, height);
			pThumbnail->Save(wszTempFileName, &png);

			strImagePath = Un2Str(wszTempFileName);

		}

		delete pImage;

		m_lbImage->SetBkImage(strImagePath.c_str());
	}

	CLabelUI *pLabel = dynamic_cast<CLabelUI*>( FindSubControl(_T("resType")));
	if(pLabel)
	{
		if(m_nType == RES_TYPE_PIC)
			pLabel->SetBkImage("RightBar\\Item\\bg_tit_pic.png");
		else if(m_nType == RES_TYPE_VOLUME)
			pLabel->SetBkImage("RightBar\\Item\\bg_tit_video.png");
		else if(m_nType == RES_TYPE_VIDEO)
			pLabel->SetBkImage("RightBar\\Item\\bg_tit_movie.png");
		else if(m_nType == RES_TYPE_COURSE)
			pLabel->SetBkImage("RightBar\\Item\\bg_tit_class.png");
	}
}

void CLocalItemUI::OnItemDragFinish()
{
	InsertPPTOperate();
}

LPCTSTR CLocalItemUI::GetButtonText( int nIndex )
{
	if (m_nType == RES_TYPE_COURSE)
	{
		if (nIndex == 0)
		{
			return _T("”¶”√");
		}
	}
	else if (m_nType == RES_TYPE_VOLUME)
	{
		if (nIndex == 1)
		{
			return _T(" ‘Ã˝");
		}
	}

	return __super::GetButtonText(nIndex);

}