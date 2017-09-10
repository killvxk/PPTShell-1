#pragma once
#include "DUICommon.h"
#include "Util/Singleton.h"

class CImageCtrlDialogUI : public WindowImplBase
{
private:
	CImageCtrlDialogUI(void);
	~CImageCtrlDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btnClose"),OnBtnCloseClick);
	UIEND_MSG_MAP

	void OnBtnCloseClick(TNotifyUI& msg);

public:
	void OpenDialog(LPCTSTR strImagePath);
	void CloseDialog();
	void ZoomIn(float scale);
	void ZoomOut(float scale);
	void Normal();

	bool OnImageControl(void* param);

	// 设置窗口已关闭的标志
	bool IsWindowClosed();

	DECLARE_SINGLETON_CLASS(CImageCtrlDialogUI);

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();
	virtual LRESULT OnKeyDown(UINT , WPARAM , LPARAM , BOOL& bHandled);

	// 获取窗口是否已关闭的标志
	void	SetWindowClosed(bool bClosed = true);
	// 计算窗口大小，并移动窗口
	void	ResizeWindow(int nWidth, int nHeight);
protected:
	CDuiString			m_strImagePath;
	RECT				m_rtOriginal;

	bool				m_bClosed;		// 窗口是否已关闭
};

typedef Singleton<CImageCtrlDialogUI>	ImageCtrlDialogUI;