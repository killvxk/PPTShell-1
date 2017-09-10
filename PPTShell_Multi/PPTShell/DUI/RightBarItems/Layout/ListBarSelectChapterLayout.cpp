#include "stdafx.h"
#include "ListBarSelectChapterLayout.h"

CListBarSelectChapterLayout::CListBarSelectChapterLayout()
{

}

CListBarSelectChapterLayout::~CListBarSelectChapterLayout()
{

}

void CListBarSelectChapterLayout::DoInit()
{
	__super::DoInit();

	m_nType = -1;

	m_pSelectChapterBtn = dynamic_cast<CButtonUI *>(FindSubControl(_T("btnSelectChapter")));
	m_pSelectChapterBtn->OnNotify += MakeDelegate(this, &CListBarSelectChapterLayout::OnBtnSelectChapter);
}

void CListBarSelectChapterLayout::DoClick( TNotifyUI* pNotify )
{

}

void CListBarSelectChapterLayout::DoRClick( TNotifyUI* pNotify )
{

}

bool CListBarSelectChapterLayout::OnBtnSelectChapter( void* pObj )
{
	TNotifyUI* Notify = (TNotifyUI *)pObj;
	if(Notify->sType == DUI_MSGTYPE_CLICK)
	{
		if(m_OnDoClickCallBack)
			m_OnDoClickCallBack(Notify);
	}

	return true;
}

