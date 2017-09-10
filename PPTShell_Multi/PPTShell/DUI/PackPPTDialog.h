#pragma once

#include "DUICommon.h"
#include "DUI/WndShadow.h"
#include "Util/Singleton.h"

#define _STR_PACK_FAIL_SAVE_KNOWN		_T("非常抱歉，暂时无法保存")
#define _STR_PACK_FAIL_SAVE_UNKNOWN		_T("哎呀，出错了！")
#define _STR_PACK_FAIL_UNKNOWN			_T("可能原因:程序员哥哥被外星人绑架了；\r\n程序员哥哥想引起你的注意力；\r\n客户端傲娇了；")
#define _STR_PACK_FAIL_KNOWN			_T("请关闭以下文件并重试：")

class CPackPPTDialogUI : public WindowImplBase
{
public:
	CPackPPTDialogUI();
	~CPackPPTDialogUI();

public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

//	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
//	bool	OnWindowDestroy(void * Param);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void	Pack( tstring strFile );
	void	UnPack( tstring strFile );

	void	Init(CRect rect);
	static void  TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

	tstring GetPath() {return m_strFilePath ;}
private:
//	CWndShadow					m_WndShadow;
	CRect						m_rcRect;

	CProgressUI*				m_pProgress;
	CLabelUI*					m_pLabel;

	int							m_nProgressCount;

	DWORD						m_dwTaskId;
private:
	bool OnPackProgress(void * pParam);
	bool OnPackCompleted(void * pParam);
	bool OnUnPackCompleted(void * pParam);
	bool OnSetValue(void * pParam);
	bool OnSaveToDBankCompleted(void* param);

	int	 m_nTotal;
	tstring m_strFilePath;	
	tstring m_strFailPath;	
	DWORD	m_dwRet;
};

typedef Singleton<CPackPPTDialogUI> PackPPTDialogUI;