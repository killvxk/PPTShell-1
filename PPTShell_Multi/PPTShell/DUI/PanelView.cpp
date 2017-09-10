#include "StdAfx.h"
#include "PanelView.h"
#include "Resource.h"

#define Panel_Default_BkColor 0x01000001

CPanelViewUI::CPanelViewUI(void)
{
	m_ShapeID		= 1;
	m_dwColor		= 0xFF000001;
	m_nWidth		= 4;
	m_bCanDraw		= false;
	m_bMouseDown	= false;
	m_bCanErase		= false;
	m_nCurrentPointer = ePoiner_Pen;
	m_bReferenceBackground	= false;
}

CPanelViewUI::~CPanelViewUI(void)
{
	ClearShapes();
}


UINT CPanelViewUI::GetControlFlags() const
{
	return UIFLAG_SETCURSOR;
}

void CPanelViewUI::Init()
{
	__super::Init();
	this->SetBkColor(Panel_Default_BkColor);

}

void CPanelViewUI::DoPaint( HDC hDC, const RECT& rcPaint )
{
	__super::DoPaint(hDC, rcPaint);

	size_t	nCount = m_ErasePoints.size();
	//abtian Reference bkcolor
	if (nCount > 0 && m_bReferenceBackground)
	{
		m_mapReferencePoints.clear();

		vector<POINT>::iterator ptItor = m_ErasePoints.begin();
		while(ptItor != m_ErasePoints.end())
		{
			COLORREF color	= GetPixel(hDC, ptItor->x, ptItor->y);
			DWORD	dwRgb	= 0;
			dwRgb	= RGB(GetBValue(color), GetGValue(color), GetRValue(color));
			m_mapReferencePoints[MAKELONG(ptItor->x, ptItor->y)] = dwRgb;
			++ptItor;
		}
	}

	bool bUpdate = false;
	vector<IShape*>::iterator itor = m_Shapes.begin();
	while(itor != m_Shapes.end())
	{
		
		IShape* pShape = *itor;
		pShape->DrawShape(hDC);

		if (nCount > 0)
		{
			bool	bFinded= false;

			vector<POINT>::iterator ptItor = m_ErasePoints.begin();
			while(ptItor != m_ErasePoints.end())
			{
				COLORREF color	= GetPixel(hDC, ptItor->x, ptItor->y);
				DWORD	dwRgb	= 0;
				dwRgb	= RGB(GetBValue(color), GetGValue(color), GetRValue(color));

				if (m_bReferenceBackground)
				{
					if (dwRgb != m_mapReferencePoints[MAKELONG(ptItor->x, ptItor->y)])
					{
						ptItor = m_ErasePoints.erase(ptItor);
						bFinded = true;
						continue;
					}
				}
				else
				{
					if (dwRgb != (this->GetBkColor() & 0x00FFFFFF))
					{
						ptItor = m_ErasePoints.erase(ptItor);
						bFinded = true;
						continue;
					}
				}
				
				++ptItor;
			}
			
			if (bFinded)
			{
				itor = m_Shapes.erase(itor);
				bUpdate = true;
				continue;
			}
		}

		++itor;
	}

	m_ErasePoints.clear();
	if (bUpdate)
	{
		Invalidate();
	}

}

void CPanelViewUI::DoEvent( TEventUI& event )
{
	__super::DoEvent(event);

	if (m_nCurrentPointer == ePoiner_Pen)
	{
		OnHandlePenEvent(event);
	}
	else if (m_nCurrentPointer == ePoiner_Erase)
	{
		OnHandleEraseEvent(event);
	}
	else if (m_nCurrentPointer == ePoiner_Arrow)
	{
		OnHandleArrowEvent(event);
		
	}

}

void CPanelViewUI::OnHandlePenEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		m_bMouseDown= true;
		m_bCanDraw	= true;

		m_pCurrentLineShape = new CLineShape;
		m_pCurrentLineShape->SetColor(m_dwColor);
		m_pCurrentLineShape->SetWidth(m_nWidth);
		m_pCurrentLineShape->AddPoint(event.ptMouse);
		m_Shapes.push_back(m_pCurrentLineShape);
	}
	else if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if (m_bCanDraw)
		{
			if (m_pCurrentLineShape)
			{
				m_pCurrentLineShape->AddPoint(event.ptMouse);
			}
		}
	}
	else if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (m_bMouseDown)
		{
			m_bCanDraw = true;

			m_pCurrentLineShape = new CLineShape;
			m_pCurrentLineShape->SetColor(m_dwColor);
			m_pCurrentLineShape->SetWidth(m_nWidth);
			m_pCurrentLineShape->AddPoint(event.ptMouse);
			m_Shapes.push_back(m_pCurrentLineShape);
		}
	}
	else if (event.Type == UIEVENT_MOUSELEAVE)
	{
		m_bCanDraw = false;
	}
	else if (event.Type == UIEVENT_BUTTONUP)
	{
		m_bMouseDown	= false;
		m_bCanDraw		= false;
	}
	else if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
	{
		::SetCursor(::LoadCursor(this->GetManager()->GetInstance(), MAKEINTRESOURCE(IDC_FLOAT_SCREENPEN)));
	}
	else if (event.Type == UIEVENT_RBUTTONDOWN)
	{
		
	}
	else if (event.Type == UIEVENT_RBUTTONUP)
	{
		m_bMouseDown	= false;
		m_bCanDraw		= false;
		//ClearShapes();
	}
	Invalidate();
}

void CPanelViewUI::OnHandleEraseEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		m_bMouseDown= true;
		m_bCanErase	= true;
		AddErasePoint(event.ptMouse);
	}
	else if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if (m_bCanErase)
		{
			AddErasePoint(event.ptMouse);
		}
	}
	else if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (m_bMouseDown)
		{
			m_bCanErase = true;
			AddErasePoint(event.ptMouse);
		}
	}
	else if (event.Type == UIEVENT_MOUSELEAVE)
	{
		m_bCanErase = false;
	}
	else if (event.Type == UIEVENT_BUTTONUP)
	{
		m_bMouseDown	= false;
		m_bCanErase		= false;

		Invalidate();
	}
	else if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
	{
		::SetCursor(::LoadCursor(this->GetManager()->GetInstance(), MAKEINTRESOURCE(IDC_EARSER)));
	}

	
}


void CPanelViewUI::OnHandleArrowEvent( TEventUI& event )
{

}



void CPanelViewUI::SetShapeColor( DWORD dwColor )
{
	m_dwColor = dwColor;
}	

void CPanelViewUI::SetShapeWidth( int nWidth )
{
	m_nWidth = nWidth;
}

void CPanelViewUI::ClearShapes()
{
	for (size_t i = 0; i < m_Shapes.size(); ++i )
	{
		IShape* pShape = m_Shapes.at(i);
		delete pShape;
	}
	m_Shapes.clear();

	Invalidate();
}

void CPanelViewUI::SetPointer( int nPointerType )
{
	if ( nPointerType <=  ePoiner_Unkown
		|| nPointerType >= ePoiner_Total)
	{
		return;
	}

	if (nPointerType == ePoiner_Arrow)
	{
		this->SetBkColor(0x00000000);
	}
	else
	{
		this->SetBkColor(Panel_Default_BkColor);
	}

	m_nCurrentPointer = nPointerType;
}

void CPanelViewUI::AddErasePoint( POINT pt )
{
	for (size_t i = 0; i < m_ErasePoints.size(); ++i)
	{
		if (m_ErasePoints.at(i).x == pt.x
			&& m_ErasePoints.at(i).y == pt.y)
		{
			return ;
		}
	}

	m_ErasePoints.push_back(pt);
}

vector<IShape*>* CPanelViewUI::GetShapes()
{
	return &m_Shapes;
}

void CPanelViewUI::SetShapes( vector<IShape*>* vShapes )
{
	m_Shapes = *vShapes;
}

void CPanelViewUI::SetReferenceBackground( bool bReference )
{
	m_bReferenceBackground = bReference;
}

void CPanelViewUI::AddShapes( IShape* pShape )
{
	m_Shapes.push_back(pShape);
}
