#pragma once
#include "DUICommon.h"
#include "ResourceItem.h"
#include "PhotoItem.h"
class CPreviewItemUI : public CPhotoItemUI
{
public:
	CPreviewItemUI();
	~CPreviewItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick(int nButtonIndex, TNotifyUI& msg );
	virtual	void		OnItemClick(TNotifyUI& msg);
	
	virtual void		ReadStream( CStream* pStream );
	void				SetIndex(int nIndex);
	void				SetPreviewDlgPtr(void* pDlg);
	void				SetResourceUrl(LPCTSTR lptcsTitle, LPCTSTR lptcsResourceUrl, tstring strGUID);
	bool				OnDownloadThumbnailCompleted( void* pNotify );
	virtual	void		DownloadThumbnail();
	virtual bool		OnItemEvent(void* param);

	tstring				GetUrl();
	void				SetLocal(bool bLocal);
	bool				IsLocal();

protected:
	DWORD				m_dwDownloadId;
	tstring				m_strPPTGuid;
	void*				m_pPreviewDlg;
	int					m_nIndex;
	bool				m_bItemClick;
	bool				m_bLocal;
};
