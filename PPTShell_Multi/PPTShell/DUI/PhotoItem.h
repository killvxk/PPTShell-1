#pragma once
#include "DUICommon.h"
#include "CloudItem.h"
#include "GifAnimUI.h"

class CPhotoItemUI : 
	public CCloudItemUI,
	public IDialogBuilderCallback
{
public:
	CPhotoItemUI();
	~CPhotoItemUI();


	virtual	void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );

	void DownloadLocal();//下载到本地
protected:
	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );
	virtual	void		OnItemClick(TNotifyUI& msg);
	virtual	void		OnItemDragFinish();
	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );
	virtual void		OnDownloadThumbnailFailed();

	void				StartMask();
	void				StopMask();

public:
	virtual	void		DownloadThumbnail();
	bool				OnDownloadThumbnailCompleted(void* pNotify);
	

protected:
	DWORD				m_dwDownloadId;

private:
	CVerticalLayoutUI*	m_layMaskStyle;
	CGifAnimUI*			m_pAnimation;

	bool OnDownloadUrlImageCompelete(void* pNotify);


};
