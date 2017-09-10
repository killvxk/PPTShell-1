#pragma once
//
// ListItem.h
// 
#include "../ListDBankItem.h"

class CListDBankAssetsItem :public CListDBankItem
{
public:
	CListDBankAssetsItem();
	virtual ~CListDBankAssetsItem();

	virtual	void			DoInit();		

	virtual bool			OnEventLoginComplete( void* pObj )		;

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据	

	virtual void			GetTotalCountInterface()					;	//获取总量
protected:
	DWORD					m_dwImageDownloadThreadId;
	DWORD					m_dwFlashDownloadThreadId;
	DWORD					m_dwVideoDownloadThreadId;
	DWORD					m_dwVolumeDownloadThreadId;

	bool					OnGetAssetsCurCount(void * pParam)				;	//获取当前章节的数量回调

	DWORD					m_nGetCurAssetsCount;
	DWORD					m_dwThreadFinishCount;								//线程完成计数
	DWORD					m_dwRequestSuccess	;								//请求并返回数据计数
};