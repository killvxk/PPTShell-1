#pragma once
//
// ListItem.h
// 
#include "ListBarItem.h"
#include "ListCloudItemComponent.h"

class CListLocalItem :public CListBarItem,
	public CListCloudItemComponent
{
public:
	CListLocalItem();
	virtual ~CListLocalItem();

	virtual	void			DoInit();

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
	virtual bool			OnRefreshGroupExplorer( void* pObj )		;	//GroupExplorer刷新按钮事件

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击
	virtual void			DoRClick(TNotifyUI*		pNotify) ;				//右键点击
	virtual void			DoSelect(TNotifyUI*		pNotify) ;				//左键选择
				
	virtual void			SetFileCount(int nCount);

	virtual tstring			GetFolderName();
	virtual int				GetFileCount();

	bool					Select(bool bSelect = true);	
private:
	void					CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

protected:
	tstring					m_strFolderName;
	int						m_nCount;

	tstring					m_strChapterGUID;
	tstring					m_strChapter;

};