#pragma once
//
// ListItem.h
// 
#include "../ListCloudItem.h"

class CListCloudAssetsItem :public CListCloudItem
{
public:
	CListCloudAssetsItem();
	virtual ~CListCloudAssetsItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击

	virtual void			GetCurCountInterface(tstring strGuid)		;	//获取当前章节的数量

	virtual void			GetTotalCountInterface()					;	//获取总量
protected:

	DWORD					m_dwCurImageDownloadThreadId;
	DWORD					m_dwCurFlashDownloadThreadId;
	DWORD					m_dwCurVideoDownloadThreadId;
	DWORD					m_dwCurVolumeDownloadThreadId;

	DWORD					m_dwTotalImageDownloadThreadId;
	DWORD					m_dwTotalFlashDownloadThreadId;
	DWORD					m_dwTotalVideoDownloadThreadId;
	DWORD					m_dwTotalVolumeDownloadThreadId;

	bool					OnGetAssetsCurCount(void * pParam)				;	//获取当前章节的数量回调
	bool					OnGetAssetsTotalCount(void * pParam)				;	//获取当前章节的数量回调

	DWORD					m_nGetCurAssetsCount;
	DWORD					m_dwCurThreadFinishCount;								//线程完成计数
	DWORD					m_dwCurRequestSuccess	;								//请求并返回数据计数

	DWORD					m_nGetTotalAssetsCount;
	DWORD					m_dwTotalThreadFinishCount;								//线程完成计数
	DWORD					m_dwTotalRequestSuccess	;								//请求并返回数据计数
};