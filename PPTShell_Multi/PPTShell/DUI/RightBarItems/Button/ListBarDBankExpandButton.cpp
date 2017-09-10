#include "stdafx.h"
#include "ListBarDBankExpandButton.h"
#include "NDCloud/NDCloudUser.h"
#include "DUI/GroupExplorer.h"

CListBarDBankExpandButton::CListBarDBankExpandButton()
{

}

CListBarDBankExpandButton::~CListBarDBankExpandButton()
{

}

void CListBarDBankExpandButton::DoInit()
{
	__super::DoInit();
	m_nType = -1;
}

void CListBarDBankExpandButton::Expand( bool bExpand )
{
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	if(dwUserId <= 0)
	{
		for(int i = 0 ; i < m_vecControls.size() ; i++)
		{
			if(_tcsicmp( m_vecControls[i]->GetName(), _T("ndbanklogin")) == 0)
			{
				m_vecControls[i]->SetVisible(bExpand);
			}
			else
			{
				m_vecControls[i]->SetVisible(false);
			}
		}
	}
	else
	{
		for(int i = 0 ; i < m_vecControls.size() ; i++)
		{
			if(_tcsicmp( m_vecControls[i]->GetName(), _T("ndbanklogin")) != 0)
			{
				if(_tcsicmp( m_vecControls[i]->GetClass(), _T("ListUI")) == 0)
				{
					CListUI* pList = dynamic_cast<CListUI*>(m_vecControls[i]);
					pList->SelectItem(-1);
				}

				m_vecControls[i]->SetVisible(bExpand);
			}
			else
			{
				m_vecControls[i]->SetVisible(false);
			}
		}
	}

	__super::Expand(bExpand);
}

void CListBarDBankExpandButton::DoClick( TNotifyUI* pNotify )
{
	Expand(!m_bExpand);

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}
