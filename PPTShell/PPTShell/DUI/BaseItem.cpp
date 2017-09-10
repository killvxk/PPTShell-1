#include "StdAfx.h"
#include "BaseItem.h"

CBaseItemUI::CBaseItemUI()
{
	m_cntContent= NULL;
	m_lbName	= NULL;
	m_lbImage	= NULL;
	m_optBorder = NULL;
	m_lbItemLabel = NULL;
	m_bShowItemLabel = false;
}


CBaseItemUI::~CBaseItemUI()
{

}

void CBaseItemUI::Init()
{
	m_cntContent= dynamic_cast<CContainerUI*>( FindSubControl(_T("content")));
	m_lbName	= dynamic_cast<CLabelUI*>( FindSubControl(_T("name")));
	m_lbImage	= dynamic_cast<CLabelUI*>( FindSubControl(_T("image")));
	m_optBorder = dynamic_cast<COptionUI*>( FindSubControl(_T("item")));
	m_lbItemLabel	= dynamic_cast<CLabelUI*>( FindSubControl(_T("itemlabel"))); 

	SetTitle(m_strTitle.c_str());
	SetGroup(m_strGroup.c_str());
	SetImage(m_strImage.c_str());
	SetItemLabel(m_strItemLabel.c_str(), m_bShowItemLabel);
}

void CBaseItemUI::SetResource( LPCTSTR lptcsResource )
{
	m_strResource = lptcsResource;
}

LPCTSTR CBaseItemUI::GetResource()
{
	return m_strResource.c_str();
}

void CBaseItemUI::SetTitle( LPCTSTR lptcsName )
{
	m_strTitle = lptcsName;
	if (m_lbName)
	{
		m_lbName->SetText(m_strTitle.c_str());
		m_lbName->SetToolTip(m_strTitle.c_str());

		m_optBorder->SetToolTip(m_strTitle.c_str());
		this->Invalidate();
	}
}


void CBaseItemUI::SetImage( LPCTSTR lptcsImage )
{
	if (!lptcsImage
		|| _tcsicmp(lptcsImage, _T("")) == 0)
	{
		return;
	}

	m_strImage = lptcsImage;
	if (m_lbImage)
	{
		m_lbImage->SetBkImage(lptcsImage);
	}
}


LPCTSTR CBaseItemUI::GetTitle()
{
	return m_strTitle.c_str();
}

void CBaseItemUI::SetGroup( LPCTSTR lptcsGroup )
{
	m_strGroup = lptcsGroup;
	if (m_optBorder)
	{
		m_optBorder->SetAttribute(_T("group"), lptcsGroup);
	}
}

void CBaseItemUI::OnItemClick( TNotifyUI& msg )
{
	
}

void CBaseItemUI::SetContentHeight( int nHeight )
{
	CControlUI* pCtrl = GetContent();
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(nHeight);
		this->SetFixedHeight(nHeight + 20);
	}
	
}

void CBaseItemUI::SetContentWidth(int nWidth)
{
	CControlUI* pCtrl = GetContent();
	if (pCtrl)
	{
		pCtrl->SetFixedWidth(nWidth);
	}
}


bool CBaseItemUI::OpenAsDefaultExec( LPCTSTR lptcsPath )
{
	int nRetn = (int)ShellExecute(NULL, _T("open"), lptcsPath, NULL, NULL, SW_SHOWNORMAL);
	if (nRetn <= 32)
	{
		nRetn = (int)ShellExecute(NULL, _T("openas"), lptcsPath, NULL, NULL, SW_SHOWNORMAL);
		if (nRetn <= 32)
		{
			tstring strCmd = _T("shell32.dll,OpenAs_RunDLL ");
			strCmd += lptcsPath;
			nRetn = (int)ShellExecute(NULL,
				_T("open"),
				_T("rundll32.exe"),
				strCmd.c_str(),
				NULL, 
				SW_SHOWNORMAL);
			if (nRetn <= 32)
			{
				//_T("无法打开该文件")
				return false;
			}
		}
	}
	return true;
}

COptionUI* CBaseItemUI::GetOption()
{
	return m_optBorder;
}

CContainerUI* CBaseItemUI::GetContent()
{
	return m_cntContent;
}

bool CBaseItemUI::IsSelected()
{
	if (GetOption())
	{
		return GetOption()->IsSelected();
	}
	return false;
}

DuiLib::CLabelUI* CBaseItemUI::GetTitleControl()
{
	return m_lbName;
}

CLabelUI* CBaseItemUI::GetItemLabelControl()
{
	return m_lbItemLabel;
}

void CBaseItemUI::SetItemLabel( LPCTSTR lptcsLabel , bool bShow)
{
	m_strItemLabel = lptcsLabel; 
	m_bShowItemLabel = bShow;
	if (m_lbItemLabel)
	{
		m_lbItemLabel->SetVisible(bShow);
		m_lbItemLabel->SetText(m_strItemLabel.c_str()); 
		this->Invalidate();
	}
}
