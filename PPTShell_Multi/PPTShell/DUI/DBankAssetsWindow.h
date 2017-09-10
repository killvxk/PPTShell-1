#pragma once
#include "DBankAssetsVideoItem.h"
#include "DBankAssetsImageItem.h"
#include "DBankAssetsFlashItem.h"
#include "DBankAssetsVolumeItem.h"
#include "DBankAssetsAllItem.h"

#include "AssetsAllLayout.h"

class CDBankAssetsWindowUI : public CVerticalLayoutUI
{
public:
	CDBankAssetsWindowUI();
	virtual ~CDBankAssetsWindowUI();

public:
	void				Init();

	void				RefreshData();

protected:
	bool OnItemClick( void* pObj );
	bool OnDoFinish( void* pObj );

private:
	CDBankAssetsVideoItem	*m_pAssetsVideoItem;
	CDBankAssetsImageItem	*m_pAssetsImageItem;
	CDBankAssetsFlashItem	*m_pAssetsFlashItem;
	CDBankAssetsVolumeItem	*m_pAssetsVolumeItem;
	CDBankAssetsAllItem		*m_pAssetsAllItem;

	void*				m_pCurSelected;

	int					m_nDoFinishCount;

	CAssetsAllLayout*	m_pAssetsAllLayout;
};