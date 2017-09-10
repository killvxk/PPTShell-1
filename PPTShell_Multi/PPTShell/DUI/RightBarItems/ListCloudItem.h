#pragma once
//
// ListItem.h
// 
#include "ListBarItem.h"
#include "ListCloudItemComponent.h"

class CListCloudItem :public CListBarItem,
					public CListCloudItemComponent
{
public:
	CListCloudItem();
	virtual ~CListCloudItem();

	virtual	void			DoInit();

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
	virtual bool			OnRefreshGroupExplorer( void* pObj )		;	//GroupExplorer刷新按钮事件

	virtual void			GetTotalCountInterface()					;	//获取总量
	virtual void			GetCurCountInterface(tstring strGuid)		;	//获取当前章节的数量

	virtual bool			OnGetTotalCount(void * pParam)				;	//获取总量回调
	virtual bool			OnGetCurCount(void * pParam)				;	//获取当前章节的数量回调

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击
	virtual void			DoRClick(TNotifyUI*		pNotify) ;				//右键点击


	void					SetCurCountUrl(tstring strUrl);				//设置当前章节总量获取地址
	tstring					GetCurCountUrl();	

	void					SetTotalCountUrl(tstring strUrl);			//设置总量获取地址
	tstring					GetTotalCountUrl();							

	bool					Select(bool bSelect = true);			

protected:
	void					CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

protected:
	DWORD					m_dwCurCountDownId;
	DWORD					m_dwTotalCountDownId;

	tstring					m_strCurCountUrl;
	tstring					m_strTotalCountUrl;

	
};