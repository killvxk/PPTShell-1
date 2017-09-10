#include "stdafx.h"
#include "AssetsVideoLayout.h"

CAssetsVideoLayout::CAssetsVideoLayout()
{

}

CAssetsVideoLayout::~CAssetsVideoLayout()
{

}

void CAssetsVideoLayout::SetStream( CStream* pStream , int nType)
{
	pStream->ResetCursor();
	int nCount = pStream->ReadInt();

	for (int i = 0; i < nCount; i++)
	{
		CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(nType);

		if (pItem)
		{
			if(i == 0 || i == 1)
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
			else
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
			pItem->ReadStream(pStream);
			m_pContentLayout->Add(pItem);

		}
	}
}
