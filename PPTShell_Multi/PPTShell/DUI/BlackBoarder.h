#pragma once

class CBlackBoarderUI:public WindowImplBase
{
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnPageAppend"),OnBtnPageAppendClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnPageRemove"),OnBtnPageRemoveClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnPagePrevious"),OnBtnPagPreviousClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnPageNext"),OnBtnPageNextClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnClose"),OnBtnCloseClick);
	UIEND_MSG_MAP

public:
	void ShowBlackBoard();
	bool GetIsShown();
	void CloseBlackBoard();
	void CloseBlackBoardForce();
	void SetPenColor(DWORD dwColor);
	DWORD GetPenColor();
	void SetPenType(int type);
	void ClearPanel();
	static	CBlackBoarderUI* GetInstance();
private:
	CBlackBoarderUI(void);
	~CBlackBoarderUI(void);

	void OnBtnCloseClick(TNotifyUI& msg);
	void OnBtnPageAppendClick(TNotifyUI& msg);
	void OnBtnPageRemoveClick(TNotifyUI& msg);
	void OnBtnPagPreviousClick(TNotifyUI& msg);
	void OnBtnPageNextClick(TNotifyUI& msg);
	void FreshBtnPageActionState();
	void SetBtnPagePreviousEnable(bool enable);
	void SetBtnPageNextEnable(bool enable);
	void SetCurrentIndex(int index);
	void SetTotalCount(int count);
	void LoadBlackboard();
	void SaveBlackboard();

	//if true return ,we should  release the pStream.
	bool ReadFile(LPCTSTR lptcsFilePath, CStream** pStream);
	bool WriteFile(LPCTSTR lptcsFilePath, CStream* pStream);

	void ConvertPointToPercent(int x,int y,float& xPercent,float& yPercent);
	void ConvertPercentToPoint(float xPercent,float yPercent,int& x,int& y);
	
	bool isShown;
	DWORD dwCurrentColor;
	int m_nCurrentPenViewType;
	int m_nScreenWidth;
	int m_nScreenHeight;
	CTabLayoutUI* m_pTabPageContainer;
	CLabelUI* m_pPageTotalCount;
	CLabelUI* m_pPageCurrentIndex;
	CButtonUI* m_pBtnPagePrevious;
	CButtonUI* m_pBtnPageNext;
	CVerticalLayoutUI* m_pLayoutBG;
	static CBlackBoarderUI* m_pInstance;

protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName( void ) const;
	virtual	CControlUI*	CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage( HWND hWnd );
	virtual LRESULT OnKeyDown( UINT , WPARAM , LPARAM , BOOL& bHandled );
	virtual void InitWindow();
	static void ReleaseInstance();
};
