#pragma once

class CDBankUploadUI:
	public CVerticalLayoutUI,
	public IDialogBuilderCallback,
	public INotifyUI

{
public:
	CDBankUploadUI(void);
	virtual ~CDBankUploadUI(void);

private:
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,		_T("choose"),		OnBtnChoose);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,		_T("start"),		OnBtnStartUpload);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,		_T("append"),		OnBtnAppend);
		EVENT_HANDLER(_T("itemremove"),								OnItemRemove);
	UIEND_MSG_MAP

public:
	void					OnDragFiles(WPARAM wParam, LPARAM lParam);
private:
	void					OnBtnChoose(TNotifyUI& msg);
	void					OnBtnStartUpload(TNotifyUI& msg);
	void					OnBtnAppend(TNotifyUI& msg);
	void					OnItemRemove(TNotifyUI& msg);
	bool					OnAppendItem(void*pObj);
	bool					OnListEvent(void*pObj);


protected:
	virtual void			Init();
	void					ShowListUI(bool bShow);
	void					ShowListDetail();
	void					CreateList(vector<tstring>& vctrFileList, bool bAppend);
	virtual CControlUI*		CreateControl( LPCTSTR pstrClass );


private:
	CComboUI*				m_comboUploadDest;
	CLabelUI*				m_lbFileDetail;
	CTileLayoutUI*			m_layList;
	CVerticalLayoutUI*		m_laySelect;
	CVerticalLayoutUI*		m_layContent;
	CDialogBuilder			m_builder;
	CDialogBuilder			m_AppendItemBuilder;

};
