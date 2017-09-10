#pragma once
#include "DUICommon.h"
#include "BaseItem.h"
#include "Util/Stream.h"
#include "DragDialogUI.h"
#include "SubItemer.h"

class CResourceItemUI : 
	public CBaseItemUI,
	public IItemContainer
{
public:
	CResourceItemUI();
	virtual  ~CResourceItemUI();

	void			SildableToolbar(bool bSildable);
	void			EnableToolbar(bool bEnable);
	void			EnableDrag(bool bEnable);
	void			NeedTypeIcon(bool bNeed);
	void			SetIcon(LPCTSTR lptcsIcon);
	virtual	void	ReadStream(CStream* pStream);
	void			SetOnItemClickDelegate(CDelegateBase& OnItemClick);
	bool			IsResourceExist();

	virtual void UploadNetdisc(int currentModeType);//ÉÏ´«µ½ÍøÅÌ
	virtual void Delete(int currentModeType);
	virtual void Rename(string strNewName);
	virtual void EditExercises();
private:
	CVerticalLayoutUI*		m_layToolBarStyle;
	bool					m_bAnimation;
	float					m_fCurrentFrame;
	bool					m_bEnableToolbar;
	bool					m_bSildableToolbar;
	static CDialogBuilder	m_builder;

	bool			OnToolbarLayoutEvent(void* pEvent);
	bool			OnToolbarEvent(void* pObj);
	void			OnTimer( UINT_PTR idEvent );
	virtual	void	DoEvent(TEventUI& event);
protected:
	int						m_nButtonIndex;
	CHorizontalLayoutUI*	m_layToolBar;
	bool					m_bNeedTypeIcon;
	bool					m_bEnableDrag;

	CEventSource			m_OnItemClick;			
	// use to drag 
	CDragDialogUI*			m_pDragDialog;
	DWORD					m_dwDragStartTime;
	POINT					m_ptDragStartPos;
	POINT					m_ptDragEndPos;
	//int						m_nItemType;

	void			EaseIn();
	void			EaseOut();
	virtual	void	Init();
	virtual	LPCTSTR GetButtonText(int nIndex);
	virtual	void	OnButtonClick(int nButtonIndex, TNotifyUI& msg);
	virtual bool	OnEmptyControlEvent(void* pEvent);
	virtual bool	OnBtnNotify(void* pNotify);
	virtual bool	HasSubitems();

	// use to drag
	virtual bool	OnItemEvent(void* param);
	virtual void	OnItemDragFinish();
};
