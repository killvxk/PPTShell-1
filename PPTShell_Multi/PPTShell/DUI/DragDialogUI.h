#pragma once
#include "DUICommon.h"
#include "Util/Singleton.h"

class CDragDialogUI : public WindowImplBase
{
private:
	CDragDialogUI(void);
	~CDragDialogUI(void);

public:
	void		SetBkImage(LPCTSTR strBkImage);
	void		SetUserData(CDragDialogUI** pDragDialog );


	DECLARE_SINGLETON_CLASS(CDragDialogUI);

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();
	virtual LRESULT OnKillFocus(UINT , WPARAM , LPARAM , BOOL& bHandled);

	

protected:
	CDuiString			m_strBkImage;
	CDragDialogUI**		m_ppDragDialogUI;
};

typedef Singleton<CDragDialogUI>	DragDialogUI;