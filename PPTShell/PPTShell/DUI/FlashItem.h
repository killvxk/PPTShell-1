#pragma once
#include "DUICommon.h"
#include "CloudItem.h"

class CFlashItemUI : public CCloudItemUI
{
public:
	CFlashItemUI();
	~CFlashItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick(int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );

	virtual void		OnItemDragFinish();

	void DownloadLocal();//下载到本地
private:
	bool OnDownloadResourceCompleted2( void* pNotify );
};
