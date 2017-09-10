#include "StdAfx.h"
#include "LocalMusicItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "ItemExplorer.h"

CLocalMusicItemUI::CLocalMusicItemUI()
{

}

CLocalMusicItemUI::~CLocalMusicItemUI()
{
	
}


void CLocalMusicItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_volume.png"));
	SetContentHeight(110);
	SetIcon(_T("RightBar\\Item\\bg_tit_volume.png"));
}

LPCTSTR CLocalMusicItemUI::GetButtonText( int nIndex )
{
	if (nIndex == 1)
	{
		return _T("ÊÔÌý");
	}
	return __super::GetButtonText(nIndex);

}

void CLocalMusicItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	if( !IsResourceExist() )
	{
		CToast::Toast(_STR_LOCAL_ITEM_TIP_FILE_NO_EXIST, false, 1000);
		return;
	}


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

			InsertVideoByThread(GetResource(), x, y, 200, 120);
		}
		else
		{
			CPPTController* pController = GetPPTController();
			POINT ptScreen = pController->SlidePosToScreenPixel(pt);
			InsertVideoByThread(GetResource(), -1, -1, 200, 120);
		}

		Statistics::GetInstance()->Report(STAT_INSERT_VOLUME);
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(GetResource());
	}
}

void CLocalMusicItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}
