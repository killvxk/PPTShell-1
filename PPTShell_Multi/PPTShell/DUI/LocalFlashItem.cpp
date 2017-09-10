#include "StdAfx.h"
#include "LocalFlashItem.h"
#include "Util/Util.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "ItemExplorer.h"


CLocalFlashItemUI::CLocalFlashItemUI()
{

}

CLocalFlashItemUI::~CLocalFlashItemUI()
{

}


void CLocalFlashItemUI::Init()
{
	__super::Init();
	SetImage(_T("RightBar\\Item\\item_bg_flash.png"));
	SetContentHeight(110);

	SetIcon(_T("RightBar\\Item\\bg_tit_flash.png"));

}

void CLocalFlashItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	if( !IsResourceExist() )
	{
		CToast::Toast(_STR_LOCAL_ITEM_TIP_FILE_NO_EXIST, false, 1000);
		return;
	}

	if ( nButtonIndex == 0 )
	{
		tstring strPngPath = _T("");//CreateFlashThumbFile(lptcsPath);
		InsertSwfByThread(GetResource(), strPngPath.c_str(), GetTitle());
	}
	else
	{
		OpenAsDefaultExec(GetResource());
	}

}

void CLocalFlashItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}
