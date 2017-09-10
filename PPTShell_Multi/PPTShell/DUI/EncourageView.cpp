#include "StdAfx.h"
#include "EncourageView.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "Util/Util.h"
#include "DUI/InstrumentItem.h"

CEncourageViewUI::CEncourageViewUI(void)
{
	m_pList = NULL;
}

CEncourageViewUI::~CEncourageViewUI(void)
{
	this->GetManager()->RemoveNotifier(this);
}

void CEncourageViewUI::Init()
{
	__super::Init();

	m_pList = (CListUI*)FindSubControl(_T("encourages"));

	CreateColors();

	this->GetManager()->AddNotifier(this);

}



void CEncourageViewUI::CreateColors()
{
	TCHAR	szIconDir[]		= _T("ScreenTool\\encourages\\");
	TCHAR	szIconPath[128]	= {0};
	LPCTSTR szEncourages[]	= {
		_T("4"),
		_T("5"),
		_T("6"),
	};

	LPCTSTR szEncouragesText[]	= {
		_T("鼓掌"),
		_T("彩带"),
		_T("彩屑"),
	};

	for (int i = 0; i < _countof(szEncourages); i++)
	{
		memset(szIconPath, 0, sizeof(szIconPath));
		_stprintf_s(szIconPath, _T("%s\\normal (%d).png"), szIconDir, i + 1);

		CContainerUI* pItem = NULL;
		if (!m_ColorBuilder.GetMarkup()->IsValid())
		{
			pItem = (CContainerUI*)m_ColorBuilder.Create(_T("ScreenTool\\ImageItemStyle.xml"));
		}
		else
		{
			pItem = (CContainerUI*)m_ColorBuilder.Create();

		}

		if (pItem)
		{
			CControlUI* pImage = pItem->FindSubControl(_T("item_image"));
			if (pImage)
			{
				pImage->SetBkImage(szIconPath);
				
			}

			CControlUI* pText = pItem->FindSubControl(_T("item_name"));
			if (pText)
			{
				pText->SetText(szEncouragesText[i]);
			}	

			pItem->SetUserData(szEncourages[i]);
			pItem->SetFixedHeight(64);
		}
		m_pList->Add(pItem);
		m_pList->SetFixedHeight(_countof(szEncourages) * 64);

	}
}

void CEncourageViewUI::OnColorItemChanged( TNotifyUI& msg )
{
	CListUI*	pList = dynamic_cast<CListUI*>(msg.pSender);
	if (!pList)
	{
		return;
	}

	CContainerUI* pCurrentItem = (CContainerUI*)pList->GetItemAt(pList->GetCurSel());
	if (!pCurrentItem)
	{
		return;
	}

	LPCTSTR szType = pCurrentItem->GetUserData().GetData();
	char szImage[MAX_PATH]; 
	char szImageDir[MAX_PATH];// 运行目录不指定，声音不会播放 2015.12.18

	sprintf_s(szImage, "%s\\bin\\GifPlayer.exe %s", GetLocalPath().c_str(), szType);
	sprintf_s(szImageDir, "%s\\bin", GetLocalPath().c_str());

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb = sizeof(STARTUPINFO); 

	if ( !CreateProcess(NULL, szImage, NULL, NULL, FALSE, 0, NULL, szImageDir, &si, &pi ) )


	if (_tcsicmp(_T("4"), szType) == 0)
	{
		Statistics::GetInstance()->Report(STAT_EFFECT_HANDCLAP);
	}
	else if (_tcsicmp(_T("5"), szType) == 0)
	{
		Statistics::GetInstance()->Report(STAT_EFFECT_CAIDAI);
	}
	else if (_tcsicmp(_T("6"), szType) == 0)
	{
		Statistics::GetInstance()->Report(STAT_EFFECT_CAIXIE);
	}


	m_pList->SelectItem( - 1);
	this->SetVisible(false);


	
}
