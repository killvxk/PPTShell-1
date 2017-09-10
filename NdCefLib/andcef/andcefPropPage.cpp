// andcefPropPage.cpp : Implementation of the CandcefPropPage property page class.

#include "stdafx.h"
#include "andcef.h"
#include "andcefPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CandcefPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CandcefPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CandcefPropPage, "ANDCEF.andcefPropPage.1",
	0xa58e8887, 0xec, 0x4b8b, 0xbc, 0xf8, 0xf1, 0xe4, 0xae, 0xe2, 0x22, 0x31)



// CandcefPropPage::CandcefPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CandcefPropPage

BOOL CandcefPropPage::CandcefPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ANDCEF_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CandcefPropPage::CandcefPropPage - Constructor

CandcefPropPage::CandcefPropPage() :
	COlePropertyPage(IDD, IDS_ANDCEF_PPG_CAPTION)
{
}



// CandcefPropPage::DoDataExchange - Moves data between page and properties

void CandcefPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CandcefPropPage message handlers
