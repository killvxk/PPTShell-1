#include "StdAfx.h"
#include "LocalVideoItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "ItemExplorer.h"

CLocalVideoItemUI::CLocalVideoItemUI()
{

}

CLocalVideoItemUI::~CLocalVideoItemUI()
{

}


void CLocalVideoItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_video.png"));
	SetContentHeight(110);
	SetIcon(_T("RightBar\\Item\\bg_tit_video.png"));
}

void CLocalVideoItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	if( !IsResourceExist() )
	{
		CToast::Toast(_STR_LOCAL_ITEM_TIP_FILE_NO_EXIST, false, 1000);
		return;
	}


	if (nButtonIndex == 0)
	{
		InsertVideoByThread(GetResource());
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(GetResource());
	}

	Statistics::GetInstance()->Report(STAT_INSERT_VIDEO);
}

void CLocalVideoItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}
