#pragma once

#include "DUICommon.h"
#include "CloudItem.h"

class CNdpCourseItemUI : public CCloudItemUI
{
public:
	CNdpCourseItemUI();
	~CNdpCourseItemUI();

	virtual	void		Init();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual	void		OnButtonClick(int nButtonIndex, TNotifyUI& msg);

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		ReadStream( CStream* pStream );

	virtual void		DownloadResource( int nButtonIndex, int nType, int nThumbnailSize );

	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );
	virtual	void		OnItemDragFinish();

	LPCTSTR				GetGuid();
	LPCTSTR				GetTitle();
	LPCTSTR				GetPPTUrl();

private:
	bool				OnDownloadPreviewsNotify( void* pNotify );

	bool				OnGetNdpDownloadUrl(void * pParam);
	bool				OnNdpCourseDownloaded(void * pParam);
	
protected:
	tstring				m_strPreviewsUrl;
	tstring				m_strTitle;
	DWORD				m_dwPreviewsDownloadId;

	tstring				m_strMd5;
	tstring				m_strDownloadUrl;

};
