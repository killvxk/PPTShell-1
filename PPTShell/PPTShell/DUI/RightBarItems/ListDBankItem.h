#pragma once
//
// ListItem.h
// 
#include "ListBarItem.h"
#include "ListDBankItemComponent.h"

class CListDBankItem :public CListBarItem,
				public CListDBankItemComponent
{
public:
	CListDBankItem();
	virtual ~CListDBankItem();


	virtual	void			DoInit();

	virtual bool			OnRefreshGroupExplorer( void* pObj )		;	//GroupExplorer刷新按钮事件

	virtual bool			OnEventLoginComplete( void* pObj )		= 0	;	//登录完成事件

	virtual void			GetTotalCountInterface()					;	//获取总量

	virtual bool			OnChapterChanged( void* pObj )				;

	virtual bool			OnGetTotalCount(void * pParam)				;	//获取总量回调

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击
	virtual void			DoRClick(TNotifyUI*		pNotify) ;				//右键点击

	void					SetTotalCountUrl(tstring strUrl);			//设置总量获取地址
	tstring					GetTotalCountUrl();		

	bool					Select(bool bSelect = true);

	vector<int>*			GetSupportType();
protected:
	void					CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

protected:
	DWORD					m_dwTotalCountDownId;

	tstring					m_strTotalCountUrl;
	vector<int>				m_vecSupportType;
};
