#pragma once
#include "PhotoItem.h"

#define ScreenThumbItem_Height	85
#define ScreenThumbItem_Width	138

class CScreenThumbItemUI : public CPhotoItemUI
{
public:
	CScreenThumbItemUI();
	~CScreenThumbItemUI();

protected:
	virtual	void		Init();
	virtual	void		OnItemClick(TNotifyUI& msg);
	virtual	void		DownloadThumbnail();
	bool				OnExportCompleted(void* pObj);


public:
	bool				OnLabelEvent(void* pObj);
	void				SetIndex(int nIndex); 
	void				DownloadThumbnailEx();


private:
	int				m_nIndex;
	DWORD			m_dwExportId;
	bool			m_bEverDownloadImage;


};
