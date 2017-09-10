#include "StdAfx.h"
#include "DUI/Shape.h"
#include "Util/Stream.h"

CDrawAttribute::CDrawAttribute()
{
	m_nWidth	= 2;
	m_dwColor	= 0;

}

CDrawAttribute::~CDrawAttribute()
{

}

int CDrawAttribute::GetWidth()
{
	return m_nWidth;
}

void CDrawAttribute::SetWidth( int nWidth )
{
	m_nWidth = nWidth;
}	

DWORD CDrawAttribute::GetColor()
{
	return m_dwColor;
}

void CDrawAttribute::SetColor( DWORD dwColor )
{
	m_dwColor	= dwColor;
}


//CLineShape
void CLineShape::AddPoint( POINT pt )
{
	m_vctrLinePoint.push_back(pt);
}

void CLineShape::DrawShape( HDC hDC )
{
	if (m_vctrLinePoint.size() <= 0)
	{
		return;
	}

	if (m_vctrLinePoint.size() == 1)
	{
		vector<POINT>::iterator cur = m_vctrLinePoint.begin();
		RECT rt = {0};
		rt.left		= cur->x;
		rt.top		= cur->y;
		rt.right	= cur->x;
		rt.bottom	= cur->y;

		CRenderEngine::DrawLine(hDC, rt, GetWidth(), GetColor());
		return;
	}

	for (vector<POINT>::iterator cur = m_vctrLinePoint.begin(); cur != m_vctrLinePoint.end(); ++cur)
	{
		vector<POINT>::iterator next = cur;
		++next;
		if (next != m_vctrLinePoint.end())
		{
			RECT rt = {0};
			rt.left		= cur->x;
			rt.top		= cur->y;
			rt.right	= next->x;
			rt.bottom	= next->y;
			CRenderEngine::DrawLine(hDC, rt, GetWidth(), GetColor());
		}	
	}
}

int CLineShape::GetShapeType()
{
	return eShape_Line;
}

void CLineShape::WriteTo( CStream* pStream, int nScreenWidth, int nScreenHeight )
{
	pStream->WriteInt(GetWidth());
	pStream->WriteInt(GetColor());

	size_t nCount = m_vctrLinePoint.size();
	pStream->WriteDWORD(nCount);
	for (size_t i = 0; i < nCount; ++i)
	{
		POINT	pt			= m_vctrLinePoint.at(i);
		float	fPercentX	= (float)(pt.x * 1.0 / nScreenWidth);
		float	fPercentY	= (float)(pt.y * 1.0 / nScreenHeight);

		pStream->WriteFloat(fPercentX);
		pStream->WriteFloat(fPercentY);
	}
}

void CLineShape::ReadFrom( CStream* pStream, int nScreenWidth, int nScreenHeight )
{
	m_vctrLinePoint.clear();

	SetWidth(pStream->ReadInt());
	SetColor(pStream->ReadInt());

	size_t nCount = pStream->ReadDWORD();
	for (size_t i = 0; i < nCount; ++i)
	{
		float	fPercentX	= pStream->ReadFloat();
		float	fPercentY	= pStream->ReadFloat();
		
		POINT	pt = {0};
		pt.x	= (DWORD)(fPercentX * nScreenWidth);
		pt.y	= (DWORD)(fPercentY * nScreenHeight);

		m_vctrLinePoint.push_back(pt);

	}
}

//CArcShape

CArcShape::CArcShape()
{
	memset(&m_rtRetangle, 0, sizeof(m_rtRetangle));
}

CArcShape::~CArcShape()
{

}

void CArcShape::SetRetangle( RECT& rt )
{
	m_rtRetangle = rt;
}

void CArcShape::SetRetangle( int left, int top, int right, int bottom )
{
	m_rtRetangle.left	= left;
	m_rtRetangle.top	= top;
	m_rtRetangle.right	= right;
	m_rtRetangle.bottom	= bottom;
}

void CArcShape::SetStartAngle( int nAngle )
{
	m_nStartAngle = nAngle;
}

void CArcShape::SetSweepAngle( int nAngle )
{
	m_nSweepAngle = nAngle;
}

void CArcShape::DrawShape( HDC hDC )
{
	Gdiplus::Graphics	graphics(hDC);
	Gdiplus::Color		color(GetColor());
	Gdiplus::Pen		pen(color, (REAL)GetWidth());

	RectF retangle;
	retangle.X		= (REAL)m_rtRetangle.left;
	retangle.Y		= (REAL)m_rtRetangle.top;
	retangle.Width	= (REAL)(m_rtRetangle.right - m_rtRetangle.left);
	retangle.Height	= (REAL)(m_rtRetangle.bottom - m_rtRetangle.top);
	graphics.DrawArc(&pen, retangle, (REAL)m_nStartAngle, (REAL)m_nSweepAngle);
}

int CArcShape::GetShapeType()
{
	return eShape_Arc;
}

void CArcShape::WriteTo( CStream* pStream, int nScreenWidth, int nScreenHeight )
{
// 	pStream->WriteInt(GetWidth());
// 	pStream->WriteInt(GetColor());
// 	pStream->WriteInt(GetColor());
// 	pStream->WriteInt(GetColor());
// 	pStream->WriteInt(GetColor());
// 
// 	size_t nCount = m_vctrLinePoint.size();
// 	pStream->WriteDWORD(nCount);
// 	for (size_t i = 0; i < nCount; ++i)
// 	{
// 		POINT	pt			= m_vctrLinePoint.at(i);
// 		float	fPercentX	= (float)(pt.x * 1.0 / nScreenWidth);
// 		float	fPercentY	= (float)(pt.y * 1.0 / nScreenHeight);
// 
// 		pStream->WriteFloat(fPercentX);
// 		pStream->WriteFloat(fPercentY);
// 	}
}

void CArcShape::ReadFrom( CStream* pStream, int nScreenWidth, int nScreenHeight )
{
// 	m_vctrLinePoint.clear();
// 
// 	SetWidth(pStream->ReadInt);
// 	SetColor(pStream->ReadInt);
// 
// 	size_t nCount = pStream->ReadDWORD();
// 	for (size_t i = 0; i < nCount; ++i)
// 	{
// 		float	fPercentX	= pStream->ReadFloat();
// 		float	fPercentY	= pStream->ReadFloat();
// 
// 		POINT	pt = {0};
// 		pt.x	= (DWORD)(fPercentX * nScreenWidth);
// 		pt.y	= (DWORD)(fPercentY * nScreenHeight);
// 
// 		m_vctrLinePoint.push_back(pt);
// 
// 	}
}