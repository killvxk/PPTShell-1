#pragma once
//
// ListItem.h
// 
#include "AssetsItemNotify.h"
#include "AssetsItemHandler.h"
#include "AssetsItemComponent.h"

class CAssetsItem : public CVerticalLayoutUI,
	public IAssetsItemNotify,
	public IAssetsItemHandler,
	public CAssetsItemComponent

{
public:
	CAssetsItem();
	virtual ~CAssetsItem();


	void			SetJsonUrl(tstring strJsonUrl);					//设置json获取地址
	tstring			GetJsonUrl();

	int				GetItemType();									//获取item类型

	virtual void	DoClick(TEventUI* pEvent) ;						//左键点击
	virtual void	DoRClick(TEventUI* pEvent) ;					//右键点击

	virtual bool	OnControlNotify(void* pEvent);					//Notify事件

	virtual	void	DoInit();

	virtual	void	SetDoClickCallBack(CDelegateBase& delegate);	//设置点击回调
	virtual	void	SetDoRClickCallBack(CDelegateBase& delegate);	//设置鼠标右键点击事件
	virtual	void	SetDoFinishCallBack(CDelegateBase& delegate);							//设置数据完成回调

	virtual	void	RefreshData(bool bForce = false);

	virtual bool	OnChapterChanged( void* pObj )				;	//章节切换事件
	virtual bool	OnRefreshGroupExplorer( void* pObj )		;	//GroupExplorer刷新按钮事件
	virtual bool	OnEventLoginComplete( void* pObj )			;	//登录成功

	void			SetLayout(CAssetsLayout* pLayout);
	CAssetsLayout*	GetLayout();

	void			Select(bool bSelected);
	bool			GetSelected();

	void			SetCountText( int nCount );

	virtual	CStream*GetStream();									//获取流数据
	bool			GetNetStatus();
protected:

	void			CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

	DWORD			m_dwDownloadId;

	CStream*		m_pStream;
private:

	bool			m_bSelected;
	tstring			m_strJsonUrl;


};