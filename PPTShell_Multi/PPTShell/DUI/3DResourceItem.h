#pragma once

#include "DUICommon.h"
#include "CloudItem.h"
#include "GifAnimUI.h"

class C3DResourceItemUI : public CCloudItemUI,
	public IDialogBuilderCallback
{
public:
	C3DResourceItemUI();
	~C3DResourceItemUI();

	virtual	void		Init();

	virtual	void		OnButtonClick(int nButtonIndex, TNotifyUI& msg);

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		ReadStream( CStream* pStream );

	virtual void		DownloadResource( int nButtonIndex, int nType, int nThumbnailSize );

	virtual	void		DownloadThumbnail();

	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );

	virtual CControlUI* CreateControl( LPCTSTR pstrClass );

	void				StartMask();
	void				StopMask();
private:

	bool				On3DResourceDownloaded(void * pParam);
	bool				OnDownloadThumbnailCompleted(void* pNotify);
protected:

	CVerticalLayoutUI*	m_layMaskStyle;
	CGifAnimUI*			m_pAnimation;

	tstring				m_strPreviewUrl;
	tstring				m_strTitle;
	tstring				m_strThumbnailPath;
	DWORD				m_dwDownloadId;

};
