#include "stdafx.h"
#include "AssetsAllLayout.h"

CAssetsAllLayout::CAssetsAllLayout()
{
	
}

CAssetsAllLayout::~CAssetsAllLayout()
{

}

void CAssetsAllLayout::SetStream( CStream* pStream, int nType )
{
	pStream->ResetCursor();
	int nCount = pStream->ReadInt();

	int n = m_pContentLayout->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(nType);

		if (pItem)
		{
			if(n != 0 && n != 1)
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
			else
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
			pItem->ReadStream(pStream);
			m_pContentLayout->Add(pItem);

		}
	}

}
