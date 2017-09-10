#pragma once
#include "DUICommon.h"
#include "CloudItem.h"

class CMusicItemUI : public CCloudItemUI
{
public:
	CMusicItemUI();
	~CMusicItemUI();

	virtual	void		Init();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual	void		OnItemDragFinish();

	void DownloadLocal();//下载到本地
protected:
	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );
private:
	bool OnDownloadResourceCompleted2( void* pNotify );
	

};
