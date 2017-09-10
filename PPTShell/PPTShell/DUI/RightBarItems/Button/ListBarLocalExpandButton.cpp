#include "stdafx.h"
#include "ListBarLocalExpandButton.h"
#include "DUI/GroupExplorer.h"

CListBarLocalExpandButton::CListBarLocalExpandButton()
{

}

CListBarLocalExpandButton::~CListBarLocalExpandButton()
{

}

void CListBarLocalExpandButton::DoInit()
{
	__super::DoInit();
	m_nType = -1;
}

void CListBarLocalExpandButton::Expand( bool bExpand )
{
	for(int i = 0 ; i < m_vecControls.size() ; i++)
	{
		if(_tcsicmp( m_vecControls[i]->GetClass(), _T("ListUI")) == 0)
		{
			CListUI* pList = dynamic_cast<CListUI*>(m_vecControls[i]);
			pList->SelectItem(-1);
		}

		m_vecControls[i]->SetVisible(bExpand);
	}
	__super::Expand(bExpand);
}

void CListBarLocalExpandButton::DoClick( TNotifyUI* pNotify )
{
	Expand(!m_bExpand);

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);

}


