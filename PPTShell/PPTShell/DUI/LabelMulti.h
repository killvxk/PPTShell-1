#pragma once
#include "DUICommon.h"

class CLabelMultiUI : public CLabelUI
{
public:
	CLabelMultiUI();
	~CLabelMultiUI();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);

	virtual void PaintText(HDC hDC);  

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
private:  
	Color _MakeRGB(int a, Color cl);  
	Color _MakeRGB(int r, int g, int b);  
};