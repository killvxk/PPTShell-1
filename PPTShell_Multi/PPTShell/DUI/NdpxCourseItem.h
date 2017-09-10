#pragma once

#include "DUICommon.h"
#include "CloudItem.h"

class CNdpxCourseItemUI : public CCloudItemUI
{
public:
	CNdpxCourseItemUI();
	~CNdpxCourseItemUI();

	virtual	void		Init();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual	void		OnButtonClick(int nButtonIndex, TNotifyUI& msg);

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		ReadStream( CStream* pStream );

	virtual void		DownloadResource( int nButtonIndex, int nType, int nThumbnailSize );

	virtual	void		OnItemDragFinish();

	LPCTSTR				GetGuid();
	LPCTSTR				GetTitle();
	LPCTSTR				GetPPTUrl();

private:
	bool				OnDownloadPreviewsNotify( void* pNotify );

	bool				OnGetNdpDownloadUrl(void * pParam);
	bool				OnNdpxCourseDownloaded(void * pParam);
	
protected:
	tstring				m_strPreviewsUrl;
	tstring				m_strTitle;
	DWORD				m_dwPreviewsDownloadId;

	tstring				m_strMd5;
	tstring				m_strDownloadUrl;

};
