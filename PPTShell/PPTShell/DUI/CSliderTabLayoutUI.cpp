#include "stdafx.h"
#include "CSliderTabLayoutUI.h"
#include "EventCenter/EventDefine.h"
CSliderTabLayoutUI::CSliderTabLayoutUI() : 
	m_bIsVertical(false),
	m_iCurFrame( 0 )
{
}

LPCTSTR CSliderTabLayoutUI::GetClass() const
{
	 return _T("SliderTabLayoutUI");
}

LPVOID CSliderTabLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, _T("SliderTabLayoutUI")) == 0 ) 
		return static_cast<CSliderTabLayoutUI*>(this);
	return CTabLayoutUI::GetInterface(pstrName);
}

bool CSliderTabLayoutUI::SelectItem( int iIndex  , bool bAnim , bool bHorizontalAnim)
{
	if(bAnim)
	{
		m_bIsVertical = !bHorizontalAnim;

		if(iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if(iIndex == m_iCurSel ) return true;
		if(iIndex > m_iCurSel ) m_nPositiveDirection = -1;
		if(iIndex < m_iCurSel ) m_nPositiveDirection = 1;

		m_iCurFrame = 0;
		m_rcNextPos = m_rcCurPos = m_rcItem;
		if(!m_bIsVertical )  //横向
		{
			m_rcNextPos.left = m_rcCurPos.left - (m_rcCurPos.right - m_rcCurPos.left) * m_nPositiveDirection;
			m_rcNextPos.right = m_rcCurPos.right - (m_rcCurPos.right - m_rcCurPos.left) * m_nPositiveDirection;
		}
		else
		{
			m_rcNextPos.top = m_rcCurPos.top - (m_rcCurPos.bottom - m_rcCurPos.top) * m_nPositiveDirection;
			m_rcNextPos.bottom = m_rcCurPos.bottom - (m_rcCurPos.bottom - m_rcCurPos.top) * m_nPositiveDirection;
		}

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for(int it = 0; it < m_items.GetSize(); it++ )
		{
			if( it == iIndex ) {
				m_pNextPage = GetItemAt(it);
				m_rc = m_pNextPage->GetPadding();
				m_pNextPage->SetPadding(CDuiRect( (m_rcItem.right - m_rcItem.left)/2,(m_rcItem.bottom - m_rcItem.top)/2,
					(m_rcItem.right - m_rcItem.left)/2,(m_rcItem.bottom - m_rcItem.top)/2));
				
				m_pNextPage->SetVisible(true);
			}
			else if( it == iOldSel )
			{
				m_pCurPage = GetItemAt(it);
				m_pCurPage->SetVisible(true);
			}
			else
				GetItemAt(it)->SetVisible(false);
		}

		m_pManager->SetTimer( this, TIMER_ANIMATION_ID, ANIMATION_ELLAPSE );
		if(m_pManager != NULL ) {
			//m_pManager->SetNextTabControl（);
			m_pManager->SendNotify(this, _T("tab"), m_iCurSel, iOldSel);
		}
	}
	else
	{
		int iCurSel = GetCurSel();
		if(iCurSel != iIndex)
		{
			m_pNextPage = GetItemAt(iIndex);
			//m_pNextPage->SetFocus();
			//m_pManager->SendNotify(m_pNextPage, DUI_MSGTYPE_KILLFOCUS);
			CTabLayoutUI::SelectItem(m_pNextPage);
			
		}
		
	}
	return true;
}

void CSliderTabLayoutUI::DoEvent( TEventUI& event)
{
	if(  event.Type == UIEVENT_TIMER ) 
	{
		 OnTimer( event.wParam );
	}
	else 
		CContainerUI::DoEvent( event);
		//__super::DoEvent（ event ）;
}


void CSliderTabLayoutUI::OnTimer( int nTimerID )
{
	OnSliderStep( );
}

void CSliderTabLayoutUI::OnSliderStep()
{
	int iStepLen = 0;
	if(!m_bIsVertical )  //横向
	{
		iStepLen = ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection / ANIMATION_FRAME_COUNT;
		if( m_iCurFrame != ANIMATION_FRAME_COUNT )
		{
			//当前窗体地位
			m_rcCurPos.left = m_rcCurPos.left + iStepLen;
			m_rcCurPos.right = m_rcCurPos.right +iStepLen;            
			//下一个窗体地位
			m_rcNextPos.left = m_rcNextPos.left + iStepLen;
			m_rcNextPos.right = m_rcNextPos.right +iStepLen;    
			m_pCurPage->SetPos(m_rcCurPos);
			m_pNextPage->SetPos(m_rcNextPos);
		}
		else
		{
			m_pCurPage->SetVisible(false);
			m_pNextPage->SetPos(m_rcItem);
		}
	}
	else //竖向
	{
		iStepLen = (m_rcItem.bottom - m_rcItem.top )  * m_nPositiveDirection / ANIMATION_FRAME_COUNT;
		if( m_iCurFrame != ANIMATION_FRAME_COUNT )
		{
			//当前窗体地位
			m_rcCurPos.top = m_rcCurPos.top + iStepLen;
			m_rcCurPos.bottom = m_rcCurPos.bottom +iStepLen;        
			//下一个窗体地位
			m_rcNextPos.top = m_rcNextPos.top + iStepLen;
			m_rcNextPos.bottom = m_rcNextPos.bottom +iStepLen;        
			m_pCurPage->SetPos(m_rcCurPos);
			 m_pNextPage->SetPos(m_rcNextPos);    
		}
		else
		{
			m_pCurPage->SetVisible(false);
			m_pNextPage->SetPos(m_rcItem);
		}
	}

	//NeedParentUpdate（）;
     
	if(m_iCurFrame == ANIMATION_FRAME_COUNT )
	{
		m_pNextPage->SetPadding(m_rc);
		NeedParentUpdate();
		m_pManager->KillTimer( this, TIMER_ANIMATION_ID );
		BroadcastEvent(EVT_CHAPTER_SWITCH, 0 ,0 ,0);
	}
	m_iCurFrame ++;
}

void CSliderTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("direction")) == 0 && _tcscmp( pstrValue, _T("vertical")) == 0 ) m_bIsVertical = true; // pstrValue = "vertical" or "horizontal"
	return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
}
	