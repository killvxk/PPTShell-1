#pragma once


#include "DUI/ThumbnailItem.h"
#include "DUI/IDownloadListener.h"
#include "DUI/IFunctionListener.h"
struct THttpNotify;

class CResourceItemExUI : 
	public CThumbnailItemUI,
	public IDownloadListener,
	public IDeleteListener,
	public IRenameListener
{
public:
	CResourceItemExUI(IComponent* pComponent);
	virtual  ~CResourceItemExUI();

protected:
	//override control
	virtual	void	DoInit();
	virtual	void	DoEvent(TEventUI& event);

	//super
	virtual bool	OnControlNotify(void* pNotify);
	virtual	bool	OnControlEvent(void* pObj);
	virtual void	OnGetThumbnailCompleted( LPCTSTR lptcsPath );

	//IDownloadListener
	virtual void	OnDownloadBefore( THttpNotify* pHttpNotify );
	virtual void	OnDownloadProgress( THttpNotify* pHttpNotify );
	virtual void	OnDownloadCompleted( THttpNotify* pHttpNotify );
	virtual void	OnDownloadInterpose(THttpNotify* pHttpNotify);

protected:
	// use to drag
	virtual bool	OnItemEvent(void* param);
	virtual void	OnItemDragFinish(POINT pt);

	//toolbar
	void			OnTimer( UINT_PTR idEvent );
	void			EaseIn();
	void			EaseOut();

	//progress
	virtual void    ShowProgress(bool bVisible);
	void			SetProgress(int nPos);

	//border process
	bool			OnItemSelect(void* pObj);

	virtual void	OnDeleteComplete( LPCTSTR lptcsError );
	virtual void	OnRenameComplete( LPCTSTR lptcsError, LPCTSTR lptcsNewName );

public:
	CLabelUI*		GetTitleCtrl();

	void			SetTitle(LPCTSTR lptcsTitle);
	void			SetHasSubItems(bool bHas);
	bool			HasSubItems();

	void			SildableToolbar(bool bSildable);
	void			EnableToolbar(bool bEnable);
	void			EnableDrag(bool bEnable);

public:
	static CResourceItemExUI*	AllocResourceItem(int nItemType);

protected:
	CProgressUI*			m_proDownload;
	CControlUI*				m_pEmptyCtrl;
private:
	bool					m_bHasSubItems;
	//ui
	CLabelUI*				m_lbTitle;
	CHorizontalLayoutUI*	m_layToolBar;
	CVerticalLayoutUI*		m_layToolBarStyle;
	static CDialogBuilder	m_ItemToolbarBuilder;
	//slide animation
	float					m_fCurrentFrame;
	bool					m_bEnableToolbar;
	bool					m_bSildableToolbar;
	bool					m_bHasSetThumbnail;
	// use to drag 
	bool					m_bEnableDrag;
	CDragDialogUI*			m_pDragDialog;
	DWORD					m_dwDragStartTime;
	POINT					m_ptDragStartPos;

};
