#include "stdafx.h"
#include "AssetsLayout.h"

CAssetsLayout::CAssetsLayout()
{

}

CAssetsLayout::~CAssetsLayout()
{

}

CControlUI* CAssetsLayout::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return NULL;
}

void CAssetsLayout::Init()
{
	CDialogBuilder m_builder;

	m_pContainerLayout = dynamic_cast<CContainerUI*>(m_builder.Create(_T("RightBar\\AssetsLayout.xml"), (UINT)0, this, this->GetManager()));

	this->Add(m_pContainerLayout);

	m_pContentLayout = dynamic_cast<CTileLayoutUI*>(m_pContainerLayout->FindSubControl(_T("content")));
	m_pContentLayout->SetDelayedDestroy(false);
	m_pLoadingLayout = dynamic_cast<CVerticalLayoutUI*>(m_pContainerLayout->FindSubControl(_T("loading")));
	m_pNetlessLayout = dynamic_cast<CVerticalLayoutUI*>(m_pContainerLayout->FindSubControl(_T("netless")));
	m_pLoadingGif	 = dynamic_cast<CGifAnimUI*>(m_pContainerLayout->FindSubControl(_T("loadinggif")));
	m_pContentLayout->SetColumns(2);
}

void CAssetsLayout::SetColumns( int nCount )
{
	m_pContentLayout->SetColumns(nCount);
}

void CAssetsLayout::SetStream( CStream* pStream, int nType )
{

}

CTileLayoutUI* CAssetsLayout::GetContentLayout()
{
	return m_pContentLayout;
}

void CAssetsLayout::StartLoading()
{
	m_pLoadingLayout->SetVisible(true);
	m_pLoadingGif->PlayGif();
	m_pNetlessLayout->SetVisible(false);
}

void CAssetsLayout::StopLoading()
{
	m_pLoadingLayout->SetVisible(false);
	m_pLoadingGif->StopGif();
}

void CAssetsLayout::ShowNetLess(bool bShow)
{
	m_pNetlessLayout->SetVisible(bShow);
}
