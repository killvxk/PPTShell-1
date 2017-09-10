#pragma once

// andcefPropPage.h : Declaration of the CandcefPropPage property page class.


// CandcefPropPage : See andcefPropPage.cpp for implementation.

class CandcefPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CandcefPropPage)
	DECLARE_OLECREATE_EX(CandcefPropPage)

// Constructor
public:
	CandcefPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_ANDCEF };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

