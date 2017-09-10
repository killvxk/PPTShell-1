#pragma once
#include "DUICommon.h"


class CSpotLightUI : public CControlUI
{
public:
	CSpotLightUI();
	~CSpotLightUI();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

public:
	void	SetTransColor(DWORD dwTransColor);
	void	SetTransColor1(DWORD dwTransColor);
	DWORD	GetTransColor();
	DWORD	GetTransColor1();

	void	SetShape(DWORD dwShape);
	DWORD	GetShape();

	void*	m_pSpotLightWindow;
private:  
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void DoInit();
	
private:
	DWORD	m_dwTransColor;
	DWORD	m_dwTransColor1;
	DWORD	m_dwShape;
};