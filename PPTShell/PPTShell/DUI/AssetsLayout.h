#pragma once
#include "Util/Stream.h"
#include "NDCloud/NDCloudAPI.h"
#include "DragDialogUI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "ResourceItemEx.h"
#include "AssetsLayoutComponent.h"

class CAssetsLayout : public CContainerUI, 
					public CAssetsLayoutComponent,
					public IDialogBuilderCallback
{
public:
	CAssetsLayout();
	virtual ~CAssetsLayout();

	virtual	void		Init();

	void				SetColumns(int nCount);
	virtual void		SetStream(CStream* pStream, int nType);

	CTileLayoutUI*		GetContentLayout();

	void				StartLoading();
	void				StopLoading();
	void				ShowNetLess(bool bShow = true);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
	
};