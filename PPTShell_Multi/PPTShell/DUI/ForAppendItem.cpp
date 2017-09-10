#include "StdAfx.h"
#include "ForAppendItem.h"

CForAppendItemUI::CForAppendItemUI()
{
	
}


CForAppendItemUI::~CForAppendItemUI()
{
	
}

void CForAppendItemUI::Init()
{
	__super::Init();

	CButtonUI* pBtn = dynamic_cast<CButtonUI* >(FindSubControl(_T("item")));
	pBtn->OnNotify += MakeDelegate(this, &CForAppendItemUI::OnItemClick);


}

bool CForAppendItemUI::OnItemClick( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;

	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		m_OnClick(pNotify);
	}

	return true;
}

void CForAppendItemUI::SetClickDelegate( CDelegateBase& OnClick )
{
	m_OnClick += OnClick;
}

