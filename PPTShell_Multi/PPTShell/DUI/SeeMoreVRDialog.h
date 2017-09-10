#pragma once

class CStream;
class DuiLib::CGifAnimUI;

class CSeeMoreVRDialogUI : 
	public WindowImplBase
{
public:
	CSeeMoreVRDialogUI(void);
	~CSeeMoreVRDialogUI(void); 
	

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btn_search_vr"),		OnBtnSearch);
		EVENT_BUTTON_CLICK(_T("btn_close"),			OnBtnClose);
		EVENT_BUTTON_CLICK(_T("btn_expand"),		OnBtnExpand);
		EVENT_BUTTON_CLICK(_T("btn_collapse"),		OnBtnCollapse);
		EVENT_BUTTON_CLICK(_T("item_classify"),		OnBtnClassifyItem);
		EVENT_ID_HANDLER(DUI_MSGTYPE_RETURN ,_T("edit_search_vr"),		OnBtnSearch);
	UIEND_MSG_MAP

//override
protected:
	virtual void			InitWindow();
	virtual CDuiString		GetSkinFolder();
	virtual CDuiString		GetSkinFile();
	virtual LPCTSTR			GetWindowClassName( void ) const;
	virtual CControlUI*		CreateControl(LPCTSTR pstrClass);

//notify
private:
	void	OnBtnSearch(TNotifyUI& msg);
	void	OnBtnClose(TNotifyUI& msg);
	void	OnBtnExpand(TNotifyUI& msg);
	void	OnBtnCollapse(TNotifyUI& msg);
	void	OnBtnClassifyItem(TNotifyUI& msg);

	bool	OnClassfyItemSelect(void* pObj);

	//download
	bool	OnGetClassifyInfoProgress(void* pObj);
	bool	OnGetClassifyInfoCompleted(void* pObj);

	bool	OnGetClassifyResultCompleted(void* pObj);

	bool	OnGetSearchResultProgress(void* pObj);
	bool	OnGetSearchResultCompleted(void* pObj);

protected:
	void	GetClassifyInfo();
	void	CreateClassifyList(CStream* pStream);
	void	CreateVRList(CStream* pStream);


public:
	void	ShowDialog();

private:
	CContainerUI*			m_pVRListLay;
	CTileLayoutUI*			m_pVRList;
	CContainerUI*			m_pVRListMask;
	DuiLib::CGifAnimUI*		m_pVRListLoding;
	CContainerUI*			m_pClassifyList;
	CContainerUI*			m_pClassifyListMask;
	DuiLib::CGifAnimUI*		m_pClassifyListLoding;
	CContainerUI*			m_pExpandLay;
	CContainerUI*			m_pCollapseLay;
	CContainerUI*			m_pExpandedListLay;
	CContainerUI*			m_pExpandedList;
	CContainerUI*			m_pClassifyListLay;
	CControlUI*				m_pCurrentSelect;
	CRichEditUI*			m_pSearchCtrl;

};
