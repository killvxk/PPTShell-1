#include "StdAfx.h"
#include "AutoResizeOption.h"

namespace DuiLib
{

	// MenuUI
	const TCHAR* const kAutoResizeOptionUIClassName = _T("AutoResizeOptionUI");
	const TCHAR* const kAutoResizeOptionUIInterfaceName = _T("AutoResizeOption");

	CAutoResizeOptionUI::CAutoResizeOptionUI() : m_nDefaultWidth(80) , m_pCategoryNode(NULL)
	{
		m_szNodeCode = _T("");
	}

	CAutoResizeOptionUI::~CAutoResizeOptionUI()
	{}

	LPCTSTR CAutoResizeOptionUI::GetClass() const
	{
		return kAutoResizeOptionUIClassName;
	}

	LPVOID CAutoResizeOptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kAutoResizeOptionUIInterfaceName) == 0 ) return static_cast<CAutoResizeOptionUI*>(this);
		return COptionUI::GetInterface(pstrName);
	}

	void CAutoResizeOptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		
	}

	void CAutoResizeOptionUI::SetText(LPCTSTR pstrText)
	{
		int nCount = _tcslen(pstrText);
		int nWidth = nCount * 10;
		if(nWidth < m_nDefaultWidth)
			nWidth = m_nDefaultWidth;
		SetFixedWidth(nWidth);
		COptionUI::SetText(pstrText);
	}


	void CAutoResizeOptionUI::SetNodeCode(LPCTSTR pstrNodeCode)
	{
		m_szNodeCode = pstrNodeCode;
	}

	tstring CAutoResizeOptionUI::GetNodeCode()
	{
		return m_szNodeCode;
	}


	// MenuUI
	const TCHAR* const kAutoHideOptionUIClassName = _T("AutoHideOptionUI");
	const TCHAR* const kAutoHideOptionUIInterfaceName = _T("AutoHideOption");

	CAutoHideOptionUI::CAutoHideOptionUI() 
	{
	}

	CAutoHideOptionUI::~CAutoHideOptionUI()
	{}

	LPCTSTR CAutoHideOptionUI::GetClass() const
	{
		return kAutoHideOptionUIClassName;
	}

	LPVOID CAutoHideOptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kAutoHideOptionUIInterfaceName) == 0 ) return static_cast<CAutoHideOptionUI*>(this);
		return COptionUI::GetInterface(pstrName);
	}

	void CAutoHideOptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		COptionUI::SetAttribute(pstrName,pstrValue);
	}

	void CAutoHideOptionUI::SetText(LPCTSTR pstrText)
	{
		TCHAR szText[MAX_PATH] = {0};
		_tcscpy(szText,pstrText);
		int nCount = _tcslen(szText);
		if(nCount > MAX_OPTION_LENGTH)
		{
			szText[MAX_OPTION_LENGTH] = 0;
			_tcscat(szText,_T("..."));
		}
		SetAttribute(_T("align"), _T("left"));
		COptionUI::SetText(szText);
	}


	void CAutoHideOptionUI::SetGuid(tstring& strGuid)
	{
		m_szGuid = strGuid;
	}

	tstring CAutoHideOptionUI::GetGuid()
	{
		return m_szGuid;
	}
}

