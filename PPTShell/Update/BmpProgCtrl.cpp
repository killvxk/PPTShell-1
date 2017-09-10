// BmpProgCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BmpProgCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpProgCtrl


CBmpProgCtrl::CBmpProgCtrl() : m_nLower(1), m_nUpper(100), m_nPos(0), m_pDC(NULL)
{
	m_BkBmp.LoadBitmap(IDB_PERBK_BMP);
	m_PerBmp.LoadBitmap(IDB_PER_BMP);
}

CBmpProgCtrl::~CBmpProgCtrl()
{
	if (NULL != m_pDC) delete m_pDC;
	m_BkBmp.DeleteObject();
	m_PerBmp.DeleteObject();
}

BEGIN_MESSAGE_MAP(CBmpProgCtrl, CStatic)
	//{{AFX_MSG_MAP(CBmpProgCtrl2)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpProgCtrl2 message handlers

void CBmpProgCtrl::SetRange(int nLower, int nUpper)
{
	if(nUpper > nLower)
	{
		m_nLower = nLower;
		m_nUpper = nUpper;
	}
	else
	{
		m_nLower = nUpper;
		m_nUpper = nLower;
	}
}

void CBmpProgCtrl::GetRange(int &lower, int &upper)
{
	lower = m_nLower;
	upper = m_nUpper;
}

int CBmpProgCtrl::SetPos(int nPos)
{
	if(m_nLower <= nPos && m_nUpper >= nPos)
	{
		if(m_nPos != nPos)
		{
			m_nPos = nPos;
			CRect Rect;
			GetClientRect(&Rect);
			InvalidateRect(&Rect);
		}
		return 1;
	}
	
	return 0;
}

int CBmpProgCtrl::GetPos()
{
	return m_nPos;
}

void CBmpProgCtrl::OnPaint() 
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);

	if (NULL == m_pDC)
	{
		m_pDC = new CDC;
		m_pDC->CreateCompatibleDC(&dc);
		m_pDC->SelectObject(&m_PerBmp);
	}

	CDC MemDC; 
	MemDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBit = MemDC.SelectObject(&m_BkBmp);

	double Fraction = (double)(m_nPos - m_nLower) / (double)(m_nUpper - m_nLower);
	int nBarWidth = (int)(Fraction * rect.Width());
	if( nBarWidth > 0 && nBarWidth <= rect.Width())
	{
		MemDC.BitBlt(0, 0, nBarWidth, rect.Height(), m_pDC, 0, 0, SRCCOPY);
	}
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();
}
