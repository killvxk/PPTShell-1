#include "StdAfx.h"
#include "ToolItem.h"
#include "Util/Util.h"
#include "InstrumentView.h"

CToolItemUI::CToolItemUI(void)
{
	m_bEmpty		= false;
}

CToolItemUI::~CToolItemUI(void)
{

}

void CToolItemUI::Init()
{
	__super::Init();

}

void CToolItemUI::SetTitle( LPCTSTR lptcsTitle )
{
	CControlUI*		pToolName	= this->FindSubControl(_T("tool_name"));
	if (pToolName && lptcsTitle)
	{
		pToolName->SetText(lptcsTitle);
		pToolName->SetToolTip(lptcsTitle);
	}
}

void CToolItemUI::SetIcon( LPCTSTR lptcsIcon )
{
	CControlUI*		pToolImg	= this->FindSubControl(_T("tool_image"));
	if (pToolImg && lptcsIcon)
	{
		DWORD dwFileAttr = GetFileAttributes(lptcsIcon);
		if (dwFileAttr != INVALID_FILE_ATTRIBUTES && !(FILE_ATTRIBUTE_DIRECTORY & dwFileAttr))
		{
			pToolImg->SetBkImage(lptcsIcon);
		}
		else
		{
			pToolImg->SetBkImage(_T("ScreenTool\\icons\\icon_common.png"));
		}
	}
}

void CToolItemUI::SetIconRect( int nWidth, int nHeight )
{
	CControlUI*		pToolImg	= this->FindSubControl(_T("tool_image"));
	CControlUI*		pImgCenter	= this->FindSubControl(_T("image_center"));
	if (pToolImg && pImgCenter)
	{
		pImgCenter->SetFixedHeight(nHeight);
		pToolImg->SetFixedHeight(nHeight);
		pToolImg->SetFixedWidth(nWidth);
	}
}

void CToolItemUI::SetResource( LPCTSTR lptcsResource )
{
	if (!lptcsResource)
	{
		return;
	}

	m_strResource = lptcsResource;
}

LPCTSTR CToolItemUI::GetResource()
{
	return m_strResource.c_str();
}

void CToolItemUI::OnClick( CContainerUI* pParent )
{

}

void CToolItemUI::ShowTopBorder( bool bShow )
{
	CControlUI* pCtrl = FindSubControl(_T("top_border"));
	if (pCtrl)
	{
		pCtrl->SetVisible(bShow);
	}
}

void CToolItemUI::ShowLeftBorder( bool bShow )
{

	CControlUI* pCtrl = FindSubControl(_T("left_border"));
	if (pCtrl)
	{
		pCtrl->SetVisible(bShow);
	}
}

void CToolItemUI::SetEmpty( bool bEmpty )
{
	m_bEmpty = bEmpty;
}

bool CToolItemUI::IsEmpty()
{
	return m_bEmpty;
}

CDuiString CToolItemUI::GetTitle()
{
	CControlUI*		pToolName	= this->FindSubControl(_T("tool_name"));
	if (pToolName)
		return pToolName->GetText();
		 
	return _T("");
}

void CToolItemUI::ShowTestIcon( bool bShow )
{
	bool bShowTestIcon = bShow;
	//whether it is empty item
	if (m_bEmpty)
	{
		bShowTestIcon = false;
	}

	CControlUI* pCtrl = FindSubControl(_T("lay_test_icon"));
	if (pCtrl)
	{
		pCtrl->SetVisible(bShowTestIcon);
	}
}
