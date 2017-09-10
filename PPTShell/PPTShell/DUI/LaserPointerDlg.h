//===========================================================================
// FileName:				LaserPointerDlg.h
//	
// Desc:					 
//============================================================================
#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"


class CLaserPointerUI : public WindowImplBase
{
public:
	CLaserPointerUI();
	~CLaserPointerUI();

public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);


	void Show(int nPosX, int nPoxY);
	void Hide();

private:
	int m_nScreenLeft;
	int m_nScreenTop;

	int m_nImageWidth;
	int m_nImageHeight;
};

typedef Singleton<CLaserPointerUI>	LaserPointerDlg;
