#include "stdafx.h"
#include "DUICommon.h"
#include "UISelectStepBar.h"

namespace DuiLib
{
	const TCHAR* const kSelectStepBarUIClassName = _T("SelectStepBarUI");
	const TCHAR* const kSelectStepBarUIInterfaceName = _T("SelectStepBar");

	CSelectStepBarUI::CSelectStepBarUI()
	{
		m_nStepCount	= 0;
		m_nCurStep		= 0;
		m_nOrientation	= 0;
	}

	CSelectStepBarUI::~CSelectStepBarUI()
	{

	}

	LPCTSTR CSelectStepBarUI::GetClass() const
	{
		return kSelectStepBarUIClassName;
	}

	LPVOID CSelectStepBarUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, kSelectStepBarUIInterfaceName) == 0 ) 
			return static_cast<CSelectStepBarUI*>(this);

		return CControlUI::GetInterface(pstrName);
	}

	void CSelectStepBarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, "NodeNotReached") == 0 )
		{
			SetNodeNotReachedImageImage(pstrValue);
		}
		else if( _tcscmp(pstrName, "NodeReached") == 0 )
		{
			SetNodeReachedImageImage(pstrValue);
		}
		else if( _tcscmp(pstrName, "NodePasted") == 0 )
		{
			SetNodePastedImageImage(pstrValue);
		}
		else if( _tcscmp(pstrName, "LineReached") == 0 )
		{
			SetLineReachedImage(pstrValue);
		}
		else if( _tcscmp(pstrName, "LineNotReached") == 0 )
		{
			SetLineNotReachedImage(pstrValue);
		}
		else if( _tcscmp(pstrName, "NodeSize") == 0 )
		{
			SIZE size = { 0 };
			LPTSTR pstr = NULL;
			size.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			size.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     

			SetNodeSize(size);
		}
		else if( _tcscmp(pstrName, "LineSize") == 0 )
		{
			SIZE size = { 0 };
			LPTSTR pstr = NULL;
			size.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			size.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     

			SetLineSize(size);
		}
		else if( _tcscmp(pstrName, "StepCount") == 0 )
		{
			m_nStepCount = 	_tcstol(pstrValue, NULL, 10);
		}

		CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CSelectStepBarUI::SetNodeNotReachedImageImage(LPCTSTR pStrImage)
	{
		if( m_diNodeNotReached.sDrawString == pStrImage && m_diNodeNotReached.pImageInfo != NULL ) 
			return;

		m_diNodeNotReached.Clear();
		m_diNodeNotReached.sDrawString = pStrImage;
		Invalidate();
	}

	void CSelectStepBarUI::SetNodeReachedImageImage(LPCTSTR pStrImage)
	{
		if( m_diNodeReached.sDrawString == pStrImage && m_diNodeReached.pImageInfo != NULL ) 
			return;

		m_diNodeReached.Clear();
		m_diNodeReached.sDrawString = pStrImage;
		Invalidate();
	}

	void CSelectStepBarUI::SetNodePastedImageImage(LPCTSTR pStrImage)
	{
		if( m_diNodePasted.sDrawString == pStrImage && m_diNodePasted.pImageInfo != NULL ) 
			return;

		m_diNodePasted.Clear();
		m_diNodePasted.sDrawString = pStrImage;
		Invalidate();
	}

	void CSelectStepBarUI::SetLineReachedImage(LPCTSTR pStrImage)
	{
		if( m_diLineReached.sDrawString == pStrImage && m_diLineReached.pImageInfo != NULL ) 
			return;

		m_diLineReached.Clear();
		m_diLineReached.sDrawString = pStrImage;
		Invalidate();
	}

	void CSelectStepBarUI::SetLineNotReachedImage(LPCTSTR pStrImage)
	{
		if( m_diLineNotReached.sDrawString == pStrImage && m_diLineNotReached.pImageInfo != NULL ) 
			return;

		m_diLineNotReached.Clear();
		m_diLineNotReached.sDrawString = pStrImage;
		Invalidate();
	}

	void CSelectStepBarUI::SetNodeSize(SIZE size)
	{
		m_NodeSize = size;
	}

	void CSelectStepBarUI::SetLineSize(SIZE size)
	{
		m_LineSize = size;
	}

	void CSelectStepBarUI::PrevStep()
	{
		m_nCurStep --;
		if( m_nCurStep == 0 )
			m_nCurStep = 0;
	}

	void CSelectStepBarUI::NextStep()
	{
		m_nCurStep ++;
		if( m_nCurStep = m_nStepCount )
			m_nCurStep = m_nStepCount - 1;
	}

	void CSelectStepBarUI::PaintStatusImage(HDC hDC)
	{
		if( m_nStepCount == 0 )
			return;

		// calculate interval
		int nInterval = 0;
		nInterval = (m_cxyFixed.cy - m_NodeSize.cy) / m_nStepCount;

		// node images
		for(int i = 0; i < m_nStepCount; i++)
		{
			if( i < m_nCurStep )
			{
				m_diNodePasted.rcDestOffset.left		= m_cXY.cx;
				m_diNodePasted.rcDestOffset.top			= m_cXY.cy + nInterval * i;
				m_diNodePasted.rcDestOffset.right		= m_cXY.cx + m_NodeSize.cx;
				m_diNodePasted.rcDestOffset.bottom		= m_diNodePasted.rcDestOffset.top + m_NodeSize.cy;

				DrawImage(hDC, m_diNodePasted);
			}
			else if( i == m_nCurStep )
			{
				m_diNodeReached.rcDestOffset.left		= m_cXY.cx;
				m_diNodeReached.rcDestOffset.top		= m_cXY.cy + nInterval * i;
				m_diNodeReached.rcDestOffset.right		= m_cXY.cx + m_NodeSize.cx;
				m_diNodeReached.rcDestOffset.bottom		= m_diNodeReached.rcDestOffset.top + m_NodeSize.cy;

				DrawImage(hDC, m_diNodeReached);
			}
			else if( i > m_nCurStep )
			{
				m_diNodeNotReached.rcDestOffset.left	= m_cXY.cx;
				m_diNodeNotReached.rcDestOffset.top		= m_cXY.cy + nInterval * i;
				m_diNodeNotReached.rcDestOffset.right	= m_cXY.cx + m_NodeSize.cx;
				m_diNodeNotReached.rcDestOffset.bottom  = m_diNodeNotReached.rcDestOffset.top + m_NodeSize.cy;
				
				DrawImage(hDC, m_diNodeNotReached);
			}
		 
		}

		// lines
		//for(int i = 0; i < m_nStepCount-1; i++)
		//{
		//	if( i < m_nCurStep )
		//	{
		//		m_diLineReached.rcDestOffset.left		= m_cXY.cx + m_NodeSize.cx/2;
		//		m_diLineReached.rcDestOffset.top		= m_cXY.cy + m_NodeSize.cy + nInterval * i;
		//		m_diLineReached.rcDestOffset.right		= m_diLineReached.rcDestOffset.left + m_LineSize.cx;
		//		m_diLineReached.rcDestOffset.bottom		= m_diLineReached.rcDestOffset.top + m_LineSize.cy;

		//		DrawImage(hDC, m_diLineReached);
		//	}
		//	else if( i == m_nCurStep )
		//	{
		//		m_diLineReached.rcDestOffset.left		= m_cXY.cx + m_NodeSize.cx/2;
		//		m_diLineReached.rcDestOffset.top		= m_cXY.cy + m_NodeSize.cy + nInterval * i;
		//		m_diLineReached.rcDestOffset.right		= m_diLineReached.rcDestOffset.left + m_LineSize.cx;
		//		m_diLineReached.rcDestOffset.bottom		= m_diLineReached.rcDestOffset.top + m_LineSize.cy;

		//		DrawImage(hDC, m_diLineReached);

		//	}
		//	else if( i > m_nCurStep )
		//	{
		//		m_diLineNotReached.rcDestOffset.left		= m_cXY.cx + m_NodeSize.cx/2;
		//		m_diLineNotReached.rcDestOffset.top			= m_cXY.cy + m_NodeSize.cy + nInterval * i;
		//		m_diLineNotReached.rcDestOffset.right		= m_diLineNotReached.rcDestOffset.left + m_LineSize.cx;
		//		m_diLineNotReached.rcDestOffset.bottom		= m_diLineNotReached.rcDestOffset.top + m_LineSize.cy;

		//		DrawImage(hDC, m_diLineNotReached);
		//	}	
		//}
	}
	

}