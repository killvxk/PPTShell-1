#pragma once
#include "DUICommon.h"
#include "CloudItem.h"
#include "GifAnimUI.h"

class CVideoItemUI : public CCloudItemUI,
	public IDialogBuilderCallback
{
public:
	CVideoItemUI();
	~CVideoItemUI();

	virtual	void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);
	virtual	void		OnItemDragFinish();

	void DownloadLocal();//下载到本地

	virtual	void		ReadStream(CStream* pStream);
	virtual	void		DownloadThumbnail();

	void				StartMask();
	void				StopMask();
protected:
	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );
	bool				OnDownloadThumbnailCompleted(void* pNotify);
private:
	bool OnDownloadResourceCompleted2( void* pNotify );

	CVerticalLayoutUI*	m_layMaskStyle;
	CGifAnimUI*			m_pAnimation;

	tstring				m_strPreviewUrl;

	DWORD				m_dwDownloadId;
};
