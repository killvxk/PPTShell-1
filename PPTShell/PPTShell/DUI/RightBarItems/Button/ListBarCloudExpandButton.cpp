#include "stdafx.h"
#include "ListBarCloudExpandButton.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"

CListBarCloudExpandButton::CListBarCloudExpandButton()
{

}

CListBarCloudExpandButton::~CListBarCloudExpandButton()
{

}

void CListBarCloudExpandButton::DoInit()
{
	__super::DoInit();
	m_nType = -1;
}


void CListBarCloudExpandButton::Expand(bool bExpand)
{
	tstring strGuid = NDCloudGetChapterGUID();

	if(strGuid.length() == 0)
	{
		for(int i = 0 ; i < m_vecControls.size() ; i++)
		{
			if(_tcsicmp( m_vecControls[i]->GetName(), _T("ndclondselectchapter")) == 0)
			{
				m_vecControls[i]->SetVisible(bExpand);
				break;
			}
		}
	}
	else
	{
		for(int i = 0 ; i < m_vecControls.size() ; i++)
		{
			if(_tcsicmp( m_vecControls[i]->GetName(), _T("ndclondselectchapter")) != 0)
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

void CListBarCloudExpandButton::DoClick( TNotifyUI* pNotify )
{
	Expand(!m_bExpand);

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}