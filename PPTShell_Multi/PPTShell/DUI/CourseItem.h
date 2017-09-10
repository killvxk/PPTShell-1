#pragma once

#include "DUICommon.h"
#include "PhotoItem.h"
#include "Util/Stream.h"

class CCourseItemUI : public CPhotoItemUI
{
public:
	CCourseItemUI();
	~CCourseItemUI();

	void DownloadLocal();//下载到本地
protected:
	virtual	void		Init();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual	void		OnButtonClick(int nButtonIndex, TNotifyUI& msg);

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		ReadStream( CStream* pStream );

	virtual void		DownloadResource( int nButtonIndex, int nType, int nThumbnailSize );

	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );

	virtual	void		DownloadThumbnail();

	virtual bool		OnDownloadThumbnailCompleted(void* pNotify);

	virtual	void		OnItemDragFinish();

	virtual bool		HasSubitems();

	bool OnDownloadResourceCompleted2( void* pNotify );
public:
	LPCTSTR				GetGuid();

	LPCTSTR				GetTitle();

	LPCTSTR				GetPPTUrl();

	CStream*			GetPreviews();

private:
	bool				OnDownloadPreviewsNotify( void* pNotify );

protected:
	tstring				m_strPreviewUrl;
	CStream*			m_pPreviewsStream;

};
