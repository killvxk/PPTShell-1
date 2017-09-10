#pragma once
//
// ListItem.h
// 
#include "../ListBarItem.h"

class CListBarDBankUploadItem : public CListBarItem
{
public:
	CListBarDBankUploadItem();
	~CListBarDBankUploadItem();

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
	virtual bool			OnRefreshGroupExplorer( void* pObj )		;	//GroupExplorer刷新按钮事件
	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据	

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击

	bool					OnBtnUpload(void * pObj);
protected:
	//override control
	virtual	void			DoInit();

private:
	CButtonUI*				m_pUploadBtn;

};