#include "stdafx.h"
#include "SpotLightWindow.h"
#include <math.h>
#include "PPTControl/PPTController.h"

const TCHAR* const kSpotLightWindowClassName = _T("SpotLightWindowUI");

CSpotLightWindowUI::CSpotLightWindowUI()
{
	m_dwDirection	= SPOTLIGHT_DIRECT_RIGHTBOTTOM;
	m_dwShape		= SPOTLIGHT_SHAPE_CIRCLE;
	m_dwLightStatus = SPOTLIGHT_STATUS_ON;
	m_bButtonDown	= false;
	m_bDialogDown	= false;
	m_bRgnDown		= false;
	m_bRgnMove		= false;
	m_pSpotLightUI  = new CSpotLightUI;
	m_pCurHotBtn	= NULL;
	m_pCurHotLabel	= NULL;
	m_pCurSpotLightTools	= NULL;
	m_nRectangleWidth = DefaultRadius;
	m_nRectangleHeight = DefaultRadius;
}

CSpotLightWindowUI::~CSpotLightWindowUI()
{

}

LPCTSTR CSpotLightWindowUI::GetClass() const
{
	return kSpotLightWindowClassName;
}

void CSpotLightWindowUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CSpotLightWindowUI::Init()
{
	m_pSpotLightToolsV = dynamic_cast<CListUI *>(FindSubControl(_T("SpotLightToolsV")));
	m_pSpotLightToolsV->OnEvent += MakeDelegate(this, &CSpotLightWindowUI::ToolsBtnEvent);
	m_pSpotLightToolsH = dynamic_cast<CListUI *>(FindSubControl(_T("SpotLightToolsH")));
	m_pSpotLightToolsH->OnEvent += MakeDelegate(this, &CSpotLightWindowUI::ToolsBtnEvent);

	m_pCurSpotLightTools = m_pSpotLightToolsV;
	m_pZoomBtn = dynamic_cast<CButtonUI *>(FindSubControl(_T("zoomBtn")));
	m_pZoomBtn->OnEvent += MakeDelegate(this, &CSpotLightWindowUI::ZoomBtnEvent);

	this->OnEvent += MakeDelegate(this, &CSpotLightWindowUI::BackgroundEvent);

	m_pointCirclePointPos.x = m_rect.left + m_rect.Width()/2;
	m_pointCirclePointPos.y = m_rect.top + m_rect.Height()/2;

	SetCircleRadius(DefaultRadius);

	m_pSpotLightUI->SetShape(m_dwShape);

	m_pSpotLightToolsV->SetVisible(true);

	m_pSpotLightUI->m_pSpotLightWindow = this;
}

void CSpotLightWindowUI::SetRect( CRect &rect )
{
	m_rect = rect;
}

void CSpotLightWindowUI::SetZoomBtnDirect( DWORD dwDirect )
{
	m_dwDirection = dwDirect;

	RECT rect;

	if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
	{
		DWORD dwPos = (DWORD)(sqrt((double)(m_dwCircleRadius * m_dwCircleRadius)/2));
		POINT pointZoomBtnPos;
		switch(m_dwDirection)
		{
		case SPOTLIGHT_DIRECT_RIGHTTOP:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x + dwPos - 3;
				pointZoomBtnPos.y = m_pointCirclePointPos.y - dwPos + 3;
			}
			break;
		case SPOTLIGHT_DIRECT_RIGHTBOTTOM:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x + dwPos - 3;
				pointZoomBtnPos.y = m_pointCirclePointPos.y + dwPos - 3;
			}
			break;
		case SPOTLIGHT_DIRECT_LEFTTOP:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x - dwPos + 3;
				pointZoomBtnPos.y = m_pointCirclePointPos.y - dwPos + 3;
			}
			break;
		case SPOTLIGHT_DIRECT_LEFTBOTTOM:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x - dwPos + 3;
				pointZoomBtnPos.y = m_pointCirclePointPos.y + dwPos - 3;
			}
			break;
		}

		::SetRect(&rect, pointZoomBtnPos.x - 15, pointZoomBtnPos.y - 15,
			pointZoomBtnPos.x + 15, pointZoomBtnPos.y + 15);
	}
	else if(m_dwShape == SPOTLIGHT_SHAPE_RECTANGLE)
	{
		int nRectangleWidth = m_nRectangleWidth;
		int nRectangleHeight = m_nRectangleHeight;

		POINT pointZoomBtnPos;
		switch(m_dwDirection)
		{
		case SPOTLIGHT_DIRECT_RIGHTTOP:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x + m_nRectangleWidth;
				pointZoomBtnPos.y = m_pointCirclePointPos.y - m_nRectangleHeight;
			}
			break;
		case SPOTLIGHT_DIRECT_RIGHTBOTTOM:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x + m_nRectangleWidth;
				pointZoomBtnPos.y = m_pointCirclePointPos.y + m_nRectangleHeight;
			}
			break;
		case SPOTLIGHT_DIRECT_LEFTTOP:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x - m_nRectangleWidth;
				pointZoomBtnPos.y = m_pointCirclePointPos.y - m_nRectangleHeight;
			}
			break;
		case SPOTLIGHT_DIRECT_LEFTBOTTOM:
			{
				pointZoomBtnPos.x = m_pointCirclePointPos.x - m_nRectangleWidth;
				pointZoomBtnPos.y = m_pointCirclePointPos.y + m_nRectangleHeight;
			}
			break;
		}

		::SetRect(&rect, pointZoomBtnPos.x - 15, pointZoomBtnPos.y - 15,
			pointZoomBtnPos.x + 15, pointZoomBtnPos.y + 15);
	}

	m_pZoomBtn->SetPos(rect);
}

void DuiLib::CSpotLightWindowUI::SetToolsPos()
{
		DWORD dwDirection = SPOTLIGHT_TOOLS_DIRECT_RIGHT;

	// 判断工具栏使用那一个布局

	CListUI* pCurSpotLightTools = m_pSpotLightToolsV;

	CRect rect = m_pSpotLightUI->GetPos();

	int nToolWidth	= SpotLightToolsWidth;
	int nToolHeight = SpotLightToolsHeight * 3;
	
// 	if (m_pointCirclePointPos.y + SpotLightToolsMargin + SpotLightToolsHeight * 2 >= m_rect.Height())
// 	{
// 		pCurSpotLightTools	= m_pSpotLightToolsH;
// 		nToolWidth			= SpotLightToolsHeight * 3;
// 		nToolHeight			= SpotLightToolsWidth;
// 	}

	if ( m_pCurSpotLightTools != pCurSpotLightTools )
	{
		m_pCurSpotLightTools->SetVisible(false);
		pCurSpotLightTools->SetVisible(true);
		m_pCurSpotLightTools = pCurSpotLightTools;
	}

	//V布局
	if ( m_pCurSpotLightTools == m_pSpotLightToolsV )
	{
		if(rect.right + SpotLightToolsMargin + SpotLightToolsWidth >= m_rect.Width())
		{
			dwDirection = SPOTLIGHT_TOOLS_DIRECT_LEFT;

			if (m_pointCirclePointPos.y  - SpotLightToolsMargin - SpotLightToolsHeight * 2  <= m_rect.top )
			{
				dwDirection = SPOTLIGHT_TOOLS_DIRECT_LEFTBOTTOM;
			}
			else if (m_pointCirclePointPos.y  + SpotLightToolsMargin + SpotLightToolsHeight * 2  >= m_rect.bottom )
			{
				dwDirection = SPOTLIGHT_TOOLS_DIRECT_LEFTTOP;
			}
		}
		else
		{
			if (m_pointCirclePointPos.y  - SpotLightToolsMargin - SpotLightToolsHeight * 2  <= m_rect.top )
			{
				dwDirection = SPOTLIGHT_TOOLS_DIRECT_RIGHTBOTTOM;
			}
			else if ( m_pointCirclePointPos.y  + SpotLightToolsMargin + SpotLightToolsHeight * 2  >= m_rect.bottom )
			{
				dwDirection = SPOTLIGHT_TOOLS_DIRECT_RIGHTTOP;
			}
		}
	}
	else	//H布局
	{
		dwDirection = SPOTLIGHT_TOOLS_DIRECT_TOP;
		
		if(m_pointCirclePointPos.x + SpotLightToolsMargin + nToolWidth / 2 >= m_rect.Width())
		{
			dwDirection = SPOTLIGHT_TOOLS_DIRECT_TOPLEFT;
		}
		else if ( m_pointCirclePointPos.x - SpotLightToolsMargin - nToolWidth / 2 <= m_rect.left )
		{
			dwDirection = SPOTLIGHT_TOOLS_DIRECT_TOPRIGHT;
		}
	}

	switch(dwDirection)
	{
		case SPOTLIGHT_TOOLS_DIRECT_LEFT:
			{
				rect.right	= rect.left - SpotLightToolsMargin;
				rect.left	= rect.right - nToolWidth;
				rect.top	= m_pointCirclePointPos.y - nToolHeight / 2;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_LEFTTOP:
			{
				rect.right = rect.left - SpotLightToolsMargin;
				rect.left = rect.right - nToolWidth;
				rect.top = m_pointCirclePointPos.y - SpotLightToolsMargin - nToolHeight;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_LEFTBOTTOM:
			{
				rect.right	= rect.left - SpotLightToolsMargin;
				rect.left	= rect.right - nToolWidth;
				rect.bottom = m_pointCirclePointPos.y + SpotLightToolsMargin + nToolHeight;
				rect.top	= rect.bottom - nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_TOP:
			{
				rect.left	= m_pointCirclePointPos.x - nToolWidth / 2;
				rect.right	= rect.left + nToolWidth;
				rect.top	= rect.top - SpotLightToolsMargin - nToolHeight;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_TOPLEFT:
			{
				rect.left	= m_pointCirclePointPos.x - SpotLightToolsMargin - nToolWidth;
				rect.right	= rect.left + nToolWidth;
				rect.top	= rect.top - SpotLightToolsMargin - nToolHeight;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_TOPRIGHT:
			{
				rect.right	= m_pointCirclePointPos.x + SpotLightToolsMargin + nToolWidth;
				rect.left	= rect.right - nToolWidth;
				rect.top	= rect.top - SpotLightToolsMargin - nToolHeight;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_RIGHT:
			{
				rect.left	= rect.right + SpotLightToolsMargin;
				rect.right	= rect.left + nToolWidth;
				rect.top	= m_pointCirclePointPos.y - nToolHeight / 2;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_RIGHTTOP:
			{
				rect.left	= rect.right + SpotLightToolsMargin;
				rect.right	= rect.left + nToolWidth;
				rect.top	= m_pointCirclePointPos.y - SpotLightToolsMargin - nToolHeight;
				rect.bottom = rect.top + nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_RIGHTBOTTOM:
			{
				rect.left	= rect.right + SpotLightToolsMargin;
				rect.right	= rect.left + nToolWidth;
				rect.bottom = m_pointCirclePointPos.y + SpotLightToolsMargin + nToolHeight;
				rect.top	= rect.bottom - nToolHeight;
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
		case SPOTLIGHT_TOOLS_DIRECT_BOTTOM:
			{
				m_pCurSpotLightTools->SetPos(rect);
			}
			break;
	}
}

void CSpotLightWindowUI::MoveSpotLight( int dwOffsetX, int dwOffsetY )
{
	//判断是否超出
	CRect rect;

	if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
	{
		m_pointCirclePointPos.x += dwOffsetX;
		m_pointCirclePointPos.y += dwOffsetY;

		if( m_pointCirclePointPos.x > m_rect.right )
			m_pointCirclePointPos.x = m_rect.right;
		else if (m_pointCirclePointPos.x < m_rect.left )
			m_pointCirclePointPos.x = m_rect.left;

		if (m_pointCirclePointPos.y < m_rect.top )
			m_pointCirclePointPos.y = m_rect.top;
		else if (m_pointCirclePointPos.y > m_rect.bottom )
			m_pointCirclePointPos.y = m_rect.bottom;

		m_dwDirection = SPOTLIGHT_DIRECT_RIGHTBOTTOM;

		if ( m_pointCirclePointPos.x + m_dwCircleRadius >= m_rect.right )
		{
			if (m_pointCirclePointPos.y + m_dwCircleRadius >= m_rect.bottom )
				m_dwDirection = SPOTLIGHT_DIRECT_LEFTTOP;
			else
				m_dwDirection = SPOTLIGHT_DIRECT_LEFTBOTTOM;
		}
		else
		{
			if (m_pointCirclePointPos.y + m_dwCircleRadius >= m_rect.bottom )
				m_dwDirection = SPOTLIGHT_DIRECT_RIGHTTOP;
			else 
				m_dwDirection = SPOTLIGHT_DIRECT_RIGHTBOTTOM;
		}

		::SetRect(&rect, m_pointCirclePointPos.x - m_dwCircleRadius, m_pointCirclePointPos.y - m_dwCircleRadius,
			m_pointCirclePointPos.x + m_dwCircleRadius, m_pointCirclePointPos.y + m_dwCircleRadius);

		DeleteObject(m_SpotLightRgn);
		m_SpotLightRgn = CreateRoundRectRgn(m_pointCirclePointPos.x - m_dwCircleRadius, m_pointCirclePointPos.y - m_dwCircleRadius, 
			m_pointCirclePointPos.x + m_dwCircleRadius, m_pointCirclePointPos.y + m_dwCircleRadius, m_dwCircleRadius * 2, m_dwCircleRadius * 2);
	}
	else
	{
		m_pointCirclePointPos.x += dwOffsetX;
		m_pointCirclePointPos.y += dwOffsetY;

		if( m_pointCirclePointPos.x > m_rect.right )
			m_pointCirclePointPos.x = m_rect.right;
		else if (m_pointCirclePointPos.x < m_rect.left )
			m_pointCirclePointPos.x = m_rect.left;

		if (m_pointCirclePointPos.y < m_rect.top )
			m_pointCirclePointPos.y = m_rect.top;
		else if (m_pointCirclePointPos.y > m_rect.bottom )
			m_pointCirclePointPos.y = m_rect.bottom;

		m_dwDirection = SPOTLIGHT_DIRECT_RIGHTBOTTOM;

		if ( m_pointCirclePointPos.x + m_nRectangleWidth >= m_rect.right )
		{
			if (m_pointCirclePointPos.y + m_nRectangleWidth >= m_rect.bottom )
				m_dwDirection = SPOTLIGHT_DIRECT_LEFTTOP;
			else
				m_dwDirection = SPOTLIGHT_DIRECT_LEFTBOTTOM;
		}
		else
		{
			if (m_pointCirclePointPos.y + m_nRectangleHeight >= m_rect.bottom )
				m_dwDirection = SPOTLIGHT_DIRECT_RIGHTTOP;
			else 
				m_dwDirection = SPOTLIGHT_DIRECT_RIGHTBOTTOM;
		}

		::SetRect(&rect, m_pointCirclePointPos.x - m_nRectangleWidth, m_pointCirclePointPos.y - m_nRectangleHeight,
			m_pointCirclePointPos.x + m_nRectangleWidth, m_pointCirclePointPos.y + m_nRectangleHeight);

		DeleteObject(m_SpotLightRgn);
		m_SpotLightRgn = CreateRectRgnIndirect(rect);
// 		m_SpotLightRgn = CreateRectRgn(m_pointCirclePointPos.x - m_nRectangleWidth, m_pointCirclePointPos.y - m_nRectangleWidth,
// 			m_pointCirclePointPos.x + m_nRectangleWidth, m_pointCirclePointPos.y + m_nRectangleWidth);
	}

	m_pSpotLightUI->SetPos(rect);

	SetZoomBtnDirect(m_dwDirection);

	SetToolsPos();
}

void DuiLib::CSpotLightWindowUI::SetCircleRadius( DWORD dwCircleRadius )
{
	if(dwCircleRadius >= m_rect.Width()/2 || dwCircleRadius >= m_rect.Height()/2)
		return;
	m_dwCircleRadius = dwCircleRadius;

	CRect rect;
	::SetRect(&rect, m_pointCirclePointPos.x - m_dwCircleRadius, m_pointCirclePointPos.y - m_dwCircleRadius,
		m_pointCirclePointPos.x + m_dwCircleRadius, m_pointCirclePointPos.y + m_dwCircleRadius);

	m_pSpotLightUI->SetPos(rect);

	SetZoomBtnDirect(m_dwDirection);

	DeleteObject(m_SpotLightRgn);
	m_SpotLightRgn = CreateRoundRectRgn(m_pointCirclePointPos.x - m_dwCircleRadius, m_pointCirclePointPos.y - m_dwCircleRadius, 
		m_pointCirclePointPos.x + m_dwCircleRadius, m_pointCirclePointPos.y + m_dwCircleRadius, m_dwCircleRadius * 2, m_dwCircleRadius * 2);

	SetToolsPos();
}


void DuiLib::CSpotLightWindowUI::SetRectanglePos( int nRectangleWidth, int nRectangleHeight )
{
	if(nRectangleWidth >= m_rect.Width()/2 - 250)
	{
		nRectangleWidth = m_rect.Width()/2 - 250;
	}
	
	if(nRectangleHeight >= m_rect.Height()/2)
	{
		nRectangleHeight = m_rect.Height()/2;
	}	

	m_nRectangleWidth = nRectangleWidth;
	m_nRectangleHeight = nRectangleHeight;

	CRect rect;
	::SetRect(&rect, m_pointCirclePointPos.x - nRectangleWidth, m_pointCirclePointPos.y - nRectangleHeight,
		m_pointCirclePointPos.x + nRectangleWidth, m_pointCirclePointPos.y + nRectangleHeight);

	m_pSpotLightUI->SetPos(rect);

	SetZoomBtnDirect(m_dwDirection);

	DeleteObject(m_SpotLightRgn);
	m_SpotLightRgn = CreateRectRgnIndirect(rect);

	SetToolsPos();
}


bool CSpotLightWindowUI::ZoomBtnEvent( void *pParam )
{
	TEventUI* pNotify = (TEventUI*)pParam;

	if (pNotify->Type == UIEVENT_BUTTONDOWN )
	{
		m_bButtonDown = true;
		m_bDialogDown = false;
		m_bRgnDown = false;
		m_bRgnMove = false;
	}
	else if (pNotify->Type == UIEVENT_MOUSEMOVE  && m_bButtonDown)
	{
		POINT pointCur ;
		GetCursorPos(&pointCur);

		DWORD dwRadius;
		if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
		{
			DWORD dwOffsetX	= abs(m_pointCirclePointPos.x - pointCur.x);
			DWORD dwOffsetY	= abs(m_pointCirclePointPos.y - pointCur.y);
			dwRadius = (DWORD)sqrt(double((dwOffsetX*dwOffsetX) + (dwOffsetY*dwOffsetY)));

			SetCircleRadius(dwRadius);
		}
		else
		{
			int nRectangleWidth = abs(pointCur.x - m_pointCirclePointPos.x);
			int nRectangleHeight = abs(pointCur.y - m_pointCirclePointPos.y);

			SetRectanglePos(nRectangleWidth, nRectangleHeight);
		}

		
	}
	else if (pNotify->Type == UIEVENT_BUTTONUP )
	{
		m_bButtonDown = false;
		m_bDialogDown = false;
		m_bRgnDown = false;
		m_bRgnMove = false;
	}

	return true;
}

bool CSpotLightWindowUI::BackgroundEvent( void *pParam )
{
	TEventUI* pNotify = (TEventUI*)pParam;

	POINT pointCur ;
	GetCursorPos(&pointCur);

	if(!m_bRgnDown && (PtInRect(&m_pZoomBtn->GetPos(), pointCur) || m_bButtonDown))
	{
		m_pZoomBtn->Event(*pNotify);
		return true;
	}

// 	
// 	TCHAR szBuff[128];
// 	sprintf(szBuff, _T("x:%d, y:%d, isDown:%s"), pointCur.x, pointCur.y, m_bRgnDown ? _T("true") : _T("false"));
// 	OutputDebugString(szBuff);
	if(PtInRegion(m_SpotLightRgn, pointCur.x, pointCur.y) || m_bRgnDown)
	{
		if( pNotify->Type == UIEVENT_BUTTONDOWN )
		{
			m_bRgnDown = true;
			m_bButtonDown = false;
			m_bDialogDown = false;
			m_bRgnMove = false;
		}
		else if( pNotify->Type == UIEVENT_MOUSEMOVE && m_bRgnDown )
		{
// 			POINT pointCur ;
// 			GetCursorPos(&pointCur);

			DWORD dwRadius;
			if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
			{
				DWORD dwOffsetX	= abs(m_pointCirclePointPos.x - pointCur.x);
				DWORD dwOffsetY	= abs(m_pointCirclePointPos.y - pointCur.y);
				dwRadius = (DWORD)sqrt(double((dwOffsetX*dwOffsetX) + (dwOffsetY*dwOffsetY)));

				SetCircleRadius(dwRadius);
			}
			else
			{
				int nRectangleWidth = abs(pointCur.x - m_pointCirclePointPos.x);
				int nRectangleHeight = abs(pointCur.y - m_pointCirclePointPos.y);
				if(!m_bRgnMove)
				{
					if( ((pointCur.x >= m_pointCirclePointPos.x + m_nRectangleWidth - 6) && (pointCur.x <= m_pointCirclePointPos.x + m_nRectangleWidth )) || 
						((pointCur.x <= m_pointCirclePointPos.x - m_nRectangleWidth + 6) && (pointCur.x >= m_pointCirclePointPos.x - m_nRectangleWidth)) )
					{
						m_dwRgnMoveDirect = 0;
					}

					if( ((pointCur.y >= m_pointCirclePointPos.y + m_nRectangleHeight - 6) && (pointCur.y <= m_pointCirclePointPos.y + m_nRectangleHeight )) || 
						((pointCur.y <= m_pointCirclePointPos.y - m_nRectangleHeight + 6) && (pointCur.y >= m_pointCirclePointPos.y - m_nRectangleHeight)) )
					{
						m_dwRgnMoveDirect = 1;

					}

					m_bRgnMove = true;
				}
			
				if(m_bRgnMove)
				{
					if(m_dwRgnMoveDirect == 0)
					{
						nRectangleHeight = m_nRectangleHeight;
					}
					else
					{
						nRectangleWidth = m_nRectangleWidth;
					}
				}
				
				

				SetRectanglePos(nRectangleWidth, nRectangleHeight);
			}
		}
		else if( pNotify->Type == UIEVENT_BUTTONUP )
		{
			m_bButtonDown = false;
			m_bDialogDown = false;
			m_bRgnDown = false;
			m_bRgnMove = false;
		}
		return true;
	}

	if (pNotify->Type == UIEVENT_MOUSEMOVE )
	{
		CButtonUI * pLightShapeBtn = NULL;
		CButtonUI * pLightStatusBtn = NULL;
		CButtonUI * pLightCloseBtn = NULL;

		
		CLabelUI * pLightShapeLabel = NULL;
		CLabelUI * pLightStatusLabel = NULL;
		CLabelUI * pLightCloseLabel = NULL;

		pLightShapeLabel = dynamic_cast<CLabelUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightShapeBackground")));
		pLightStatusLabel = dynamic_cast<CLabelUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightSwitchBackground")));
		pLightCloseLabel = dynamic_cast<CLabelUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightCloseBackground")));

		if(PtInRect(&m_pSpotLightToolsV->GetPos(), pointCur))
		{
			pLightShapeBtn = dynamic_cast<CButtonUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightShapeBtn")));
			pLightStatusBtn = dynamic_cast<CButtonUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightStatusBtn")));
			pLightCloseBtn = dynamic_cast<CButtonUI *>(m_pSpotLightToolsV->FindSubControl(_T("CloseBtn")));
				
			
				
			if(PtInRect(&pLightShapeLabel->GetPos(), pointCur))
			{
				if(m_pCurHotLabel != pLightShapeLabel)
				{
					if ( m_pCurHotLabel != NULL )
					{
						m_pCurHotLabel->SetBkImage(m_strLabelImagePath);
						m_pCurHotBtn->SetNormalImage(m_strBtnImagePath);
						m_pCurHotBtn->SetTextColor(0xFF000001);
						m_pCurHotBtn->Invalidate();
					}

					m_strBtnImagePath = pLightShapeBtn->GetNormalImage();
					m_strLabelImagePath = pLightShapeLabel->GetBkImage();
					
					pLightShapeLabel->SetBkImage(_T("SpotLightWindow\\icon_btn_top_on.png"));

					if ( m_dwShape == SPOTLIGHT_SHAPE_RECTANGLE )
						pLightShapeBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_circle_on.png' dest='19,10,45,36'"));
					else
						pLightShapeBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_rectangle_on.png' dest='19,10,45,36'"));
						

					pLightShapeBtn->SetTextColor(0xFFFFFFFF);
					pLightShapeBtn->Invalidate();

					m_pCurHotLabel	= pLightShapeLabel;
					m_pCurHotBtn	= pLightShapeBtn;
				}
				
			}
			else if(PtInRect(&pLightStatusLabel->GetPos(), pNotify->ptMouse))
			{
				if(m_pCurHotLabel != pLightStatusLabel)
				{
					if ( m_pCurHotLabel != NULL )
					{
						m_pCurHotLabel->SetBkImage(m_strLabelImagePath);
						m_pCurHotBtn->SetNormalImage(m_strBtnImagePath);
						m_pCurHotBtn->SetTextColor(0xFF000001);
						m_pCurHotBtn->Invalidate();
					}

					m_strBtnImagePath = pLightStatusBtn->GetNormalImage();
					m_strLabelImagePath = pLightStatusLabel->GetBkImage();

					pLightStatusLabel->SetBkImage(_T("SpotLightWindow\\icon_btn_mid_on.png"));
				
					if ( m_dwLightStatus == SPOTLIGHT_STATUS_ON )
						pLightStatusBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_closelight_on.png' dest='19,10,45,36'"));
					else
						pLightStatusBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_openlight_on.png' dest='19,10,45,36'"));

					pLightStatusBtn->SetTextColor(0xFFFFFFFF);
					pLightStatusBtn->Invalidate();

					m_pCurHotLabel	= pLightStatusLabel;
					m_pCurHotBtn	= pLightStatusBtn;
				}
				
			}
			else if(PtInRect(&pLightCloseLabel->GetPos(), pointCur))
			{
				if(m_pCurHotLabel != pLightCloseLabel)
				{
					if ( m_pCurHotLabel != NULL )
					{
						m_pCurHotLabel->SetBkImage(m_strLabelImagePath);
						m_pCurHotBtn->SetNormalImage(m_strBtnImagePath);
						m_pCurHotBtn->SetTextColor(0xFF000001);
						m_pCurHotBtn->Invalidate();
					}

					m_strBtnImagePath = pLightCloseBtn->GetNormalImage();
					m_strLabelImagePath = pLightCloseLabel->GetBkImage();

					pLightCloseLabel->SetBkImage(_T("SpotLightWindow\\icon_btn_bottom_on.png"));
					pLightCloseBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_close_on.png' dest='19,10,45,36'"));

					pLightCloseBtn->SetTextColor(0xFFFFFFFF);
					pLightCloseBtn->Invalidate();

					m_pCurHotLabel	= pLightCloseLabel;
					m_pCurHotBtn	= pLightCloseBtn;
				}
				
			}
			return true;

		}
		else if(PtInRect(&m_pSpotLightToolsH->GetPos(), pointCur))
		{
			return true;
		}
		else if( m_pCurHotLabel != NULL )
		{
			m_pCurHotLabel->SetBkImage(m_strLabelImagePath);
			m_pCurHotBtn->SetNormalImage(m_strBtnImagePath);
			m_pCurHotBtn->SetTextColor(0xFF000001);
			m_pCurHotBtn->Invalidate();
			m_pCurHotLabel = NULL;
		}
	}

	if( pNotify->Type == UIEVENT_BUTTONDOWN )
	{
		if(PtInRect(&m_pSpotLightToolsV->GetPos(), pointCur))
		{
			CButtonUI * pLightShapeBtn = NULL;
			CButtonUI * pLightStatusBtn = NULL;
			CButtonUI * pLightCloseBtn = NULL;

			CLabelUI * pLightShapeLabel = NULL;
			CLabelUI * pLightStatusLabel = NULL;
			CLabelUI * pLightCloseLabel = NULL;

			pLightShapeLabel = dynamic_cast<CLabelUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightShapeBackground")));
			pLightStatusLabel = dynamic_cast<CLabelUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightSwitchBackground")));
			pLightCloseLabel = dynamic_cast<CLabelUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightCloseBackground")));

			pLightShapeBtn = dynamic_cast<CButtonUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightShapeBtn")));
			pLightStatusBtn = dynamic_cast<CButtonUI *>(m_pSpotLightToolsV->FindSubControl(_T("LightStatusBtn")));
			pLightCloseBtn = dynamic_cast<CButtonUI *>(m_pSpotLightToolsV->FindSubControl(_T("CloseBtn")));

			if(PtInRect(&pLightShapeLabel->GetPos(), pointCur))
			{
				switch(m_dwShape)
				{
				case SPOTLIGHT_SHAPE_CIRCLE:
					{
						m_dwShape = SPOTLIGHT_SHAPE_RECTANGLE;
						m_strBtnImagePath = _T("file='SpotLightWindow\\icon_circle.png' dest='19,10,45,36'");
						pLightShapeBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_circle_on.png' dest='19,10,45,36'"));
						
						pLightShapeBtn->SetText(_T("圆形"));

						SetRectanglePos(m_nRectangleWidth, m_nRectangleHeight);
					}
					break;
				case SPOTLIGHT_SHAPE_RECTANGLE:
					{
						m_dwShape = SPOTLIGHT_SHAPE_CIRCLE;
						m_strBtnImagePath = _T("file='SpotLightWindow\\icon_rectangle.png' dest='19,10,45,36'");
						pLightShapeBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_rectangle_on.png' dest='19,10,45,36'"));

						pLightShapeBtn->SetText(_T("矩形"));

						SetCircleRadius(m_dwCircleRadius);
					}
					break;
				}

				m_pSpotLightUI->SetShape(m_dwShape);
				//SetZoomBtnDirect(m_dwDirection);
			}

			if(PtInRect(&pLightStatusLabel->GetPos(), pointCur))
			{
				switch(m_dwLightStatus)
				{
				case SPOTLIGHT_STATUS_CLOSE:
					{
						m_dwLightStatus = SPOTLIGHT_STATUS_ON;
						m_strBtnImagePath = _T("file='SpotLightWindow\\icon_closelight.png' dest='19,10,45,36'");
						pLightStatusBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_closelight_on.png' dest='19,10,45,36'"));

						pLightStatusBtn->SetText(_T("关灯"));

						SetBkColor(OpenLightColor);
						m_pSpotLightUI->SetTransColor(OpenLightColor);
					}
					break;
				case SPOTLIGHT_STATUS_ON:
					{
						m_dwLightStatus = SPOTLIGHT_STATUS_CLOSE;
						m_strBtnImagePath = _T("file='SpotLightWindow\\icon_openlight.png' dest='19,10,45,36'");
						pLightStatusBtn->SetNormalImage(_T("file='SpotLightWindow\\icon_openlight_on.png' dest='19,10,45,36'"));
						
						pLightStatusBtn->SetText(_T("开灯"));
						
						SetBkColor(CloseLightColor);
						m_pSpotLightUI->SetTransColor(CloseLightColor);
					}
					break;
				}
			}

			if(PtInRect(&pLightCloseLabel->GetPos(), pointCur))
			{
				SetVisible(false);


				::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
				::SetFocus(CPPTController::GetSlideShowViewHwnd());
			}

			return true;
		}
	}

	if (pNotify->Type == UIEVENT_BUTTONDOWN )
	{
		GetCursorPos(&m_pointStart);
		m_bDialogDown = true;
	}
	else if (pNotify->Type == UIEVENT_MOUSEMOVE  && m_bDialogDown)
	{
		if(m_bDialogDown)
		{
			POINT pointCur ;
			GetCursorPos(&pointCur);


			MoveSpotLight(pointCur.x - m_pointStart.x, pointCur.y - m_pointStart.y);

			m_pointStart = pointCur;
		}
	}
	else if (pNotify->Type == UIEVENT_BUTTONUP )
	{
		m_bButtonDown = false;
		m_bDialogDown = false;
		m_bRgnDown = false;
		m_bRgnMove = false;
	}

	return true;
}

CSpotLightUI* DuiLib::CSpotLightWindowUI::GetSpotLightUI()
{
	return m_pSpotLightUI;
}

bool DuiLib::CSpotLightWindowUI::GetSpotLightVisible()
{
	return IsVisible();
}

bool DuiLib::CSpotLightWindowUI::ToolsBtnEvent( void *pParam )
{
	return true;
}

void DuiLib::CSpotLightWindowUI::SetGesture( DWORD dwFlag, ULONGLONG ullArguments )
{
	switch(dwFlag)
	{
	case GID_BEGIN:
		m_ullArguments = 0;
		break;
	case GID_END:
		m_ullArguments = 0;
		break;
	case GID_ZOOM:
		{
			if(m_ullArguments != 0)
			{
				int nTempArguments = ullArguments - m_ullArguments;

				m_dwLightStatus += nTempArguments;
// 				DWORD dwRadius;
// 				if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
// 				{
// 					DWORD dwOffsetX	= abs(m_pointCirclePointPos.x + nTempArguments);
// 					DWORD dwOffsetY	= abs(m_pointCirclePointPos.y + nTempArguments);
// 					dwRadius = (DWORD)sqrt(double((dwOffsetX*dwOffsetX) + (dwOffsetY*dwOffsetY)));
// 				}
// 				else
// 				{
// 					dwRadius = abs(m_pointCirclePointPos.x + nTempArguments);
// 				}
				
				SetCircleRadius(m_dwLightStatus);

				m_ullArguments = ullArguments;

			}
			else
			{
				m_ullArguments = ullArguments;
			}
		}
		break;
	}
}

void DuiLib::CSpotLightWindowUI::ShowSpotLightWindow(bool bShow)
{
	if(bShow)
	{
		SetVisible(true);
	}
	else
	{
		SetVisible(false);


		::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
		::SetFocus(CPPTController::GetSlideShowViewHwnd());

	}
}
