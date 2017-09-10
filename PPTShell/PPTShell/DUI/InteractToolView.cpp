#include "StdAfx.h"
#include "InteractToolView.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "InteractiveType.h" 
#include "Config.h"
#include "Plugins/Icr/IcrPlayer.h"
#include "BlackBoarder.h"
#include "ScreenInstrument.h"
#include "DUI/ToolItem.h"
#include "MagicPen_Main.h"


DuiLib::CDialogBuilder CInteractToolViewUI::m_GroupBuilder;
DuiLib::CDialogBuilder CInteractToolViewUI::m_ItemBuilder;
CInteractToolViewUI::CInteractToolViewUI(void)
{
	m_pList = NULL; 
}

CInteractToolViewUI::~CInteractToolViewUI(void)
{
	this->GetManager()->RemoveNotifier(this);
	CBlackBoarderUI::GetInstance()->CloseBlackBoardForce();
}

void CInteractToolViewUI::Init()
{
	__super::Init();

	m_pList = (CTileLayoutUI*)FindSubControl(_T("interact_list"));

	CreateInteractionsWithConfig();

	this->GetManager()->AddNotifier(this);

}


void CInteractToolViewUI::OnInteractItemClick( TNotifyUI& msg )
{
	CToolItemUI* pItem = dynamic_cast<CToolItemUI*>(msg.pSender);
	if (!pItem || pItem->IsEmpty())
	{
		return;
	}

	int nType = (int)pItem->GetTag();
	if(nType == Magnifier)
	{ 
		/*CFloat_MagicPen* pMagicPen = new CFloat_MagicPen();
		pMagicPen->Create(IDD_FLOAT_MAGICPEN, CWnd::GetDesktopWindow());
		int cx  = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN); 
		pMagicPen->SetWindowPos(&CWnd::wndTopMost, 0, 0, cx, cy, SWP_SHOWWINDOW| SWP_NOACTIVATE | SWP_NOZORDER);*/
		//CMagicPen_Main* pMagicPen = new CMagicPen_Main();
		//pMagicPen->ShowLocalWindows();

		//m_hMagicPen = pMagicPen->GetHWND();
		MagicPen_MainUI::GetInstance()->ShowLocalWindows();
	}
	else if( nType== BlackBoard )
	{
		if(!CBlackBoarderUI::GetInstance()->GetIsShown())
		{
			CBlackBoarderUI::GetInstance()->ShowBlackBoard();
		}
		else
		{
			::SetFocus(CBlackBoarderUI::GetInstance()->GetHWND());
		}
	}
	else if( nType == Spotlight )
	{
		CSpotLightWindowUI * pSpotLightWindowUI = CScreenInstrumentUI::GetMainInstrument()->GetSpotLightWindow();

		if(pSpotLightWindowUI->GetSpotLightVisible())
			pSpotLightWindowUI->ShowSpotLightWindow(false);
		else
			pSpotLightWindowUI->ShowSpotLightWindow(true);
	}
	//to do test
	else if( nType == Roster )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_ROSTER);
	}
	else if( nType == PushScreen )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_PUSHSCREEN);
	}
	else if( nType == CourseTraining )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_EXAM);
	}
	else if( nType == CourseSummary )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_SUMMARY);
	}
	else if( nType == AssignHomework )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_ASSIGN_HOMEWORK);
	}
	else if( nType == Vote )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_RACE_ANSWER);
	}
	else if( nType == BaiKe )
	{
		IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_BAIKE);
	}

	this->SetVisible(false);

}

void CInteractToolViewUI::OnSelected()
{
	IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_INTERACTIVE_TOOL);
}

void CInteractToolViewUI::OnUnSelected()
{
	//2016.03.01 cws 切换选择工具栏，不进行窗口销毁隐藏
	//if (m_hMagicPen)
	//{

	//	if (::IsWindow(m_hMagicPen))
	//	{
	//		::DestroyWindow(m_hMagicPen);
	//	}
	//	m_hMagicPen = NULL;
	//}
}

CControlUI* CInteractToolViewUI::GetItem( int nIndex )
{
	if( m_pList == NULL ) 
		return NULL;

	CControlUI* pItem = m_pList->GetItemAt(nIndex);
	return pItem;

}

CControlUI* CInteractToolViewUI::GetItem(LPCTSTR szName)
{
	if( m_pList == NULL )
		return NULL;

	for(int i = 0; i < m_pList->GetCount(); i++)
	{
		CToolItemUI* pToolItem = (CToolItemUI*)m_pList->GetItemAt(i);
		if( pToolItem == NULL )
			continue;

		CDuiString strTitle = pToolItem->GetTitle();
		if( strTitle.CompareNoCase(szName) == 0 )
			return pToolItem;
	}

	return NULL;
}

void CInteractToolViewUI::CreateInteractionsWithConfig()
{
	tstring strConfigPath = this->GetManager()->GetResourcePath();
	strConfigPath += _T("ScreenTool\\interacts\\Config.txt");

	// read config from file
	FILE* fp = NULL;
	int err = _tfopen_s(&fp, strConfigPath.c_str(), _T("rb"));
	if( err != 0 )
		return;

	fseek(fp, 0, SEEK_END);
	int nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if( nSize == 0 )
	{
		fclose(fp);
		return;
	}
	//
	char* pBuffer = new char[nSize];
	if( pBuffer == NULL )
	{
		fclose(fp);
		return;
	}

	fread(pBuffer, nSize, 1, fp);
	fclose(fp);

	Json::Value root;
	Json::Reader reader;
	//pBuffer+3 skip utf-8 head
	bool res = reader.parse(pBuffer + 3, root);

	delete pBuffer;
	if( !res )
	{
		return;
	}


	if (root.isNull() || !root.isArray())
	{
		return;
	}

	for (int i = 0; i < (int)root.size(); ++i)
	{
		Json::Value& group = root[i];
		if (group.isNull())
		{
			continue;
		}

		Json::Value& name	= group["name"];
		Json::Value& val	= group["value"];
		Json::Value& type	= group["type"];
		if (name.isNull() || val.isNull())
		{
			continue;
		}

		if (type.isNull())
		{
			type = eCreate_FromDirectory;
		}

		if (type.asInt() == eCreate_FromValue)
		{
			CreateListFromValue(Utf82Str(name.asCString()).c_str(), &val);
		}

	}

	//blank
// 	if (m_pList)
// 	{
// 		CControlUI* pCtrl = new CControlUI;
// 		pCtrl->SetFixedHeight(40);
// 		m_pList->Add(pCtrl);
// 	}
}

CControlUI* CInteractToolViewUI::CreateItemUI( int nToolType )
{
	CControlUI* pItem = NULL;
	if (!m_ItemBuilder.GetMarkup()->IsValid())
	{
		pItem = m_ItemBuilder.Create(_T("ScreenTool\\SubjectItemStyle.xml"), NULL, this);
	}
	else
	{
		pItem = m_ItemBuilder.Create(this);
	}

	return pItem;
}


void CInteractToolViewUI::CreateListFromValue( LPCTSTR lptcsGroup, LPVOID lpGroup )
{
#ifdef DEVELOP_VERSION
	Json::Value* items = (Json::Value*)lpGroup;
	if (!items || items->isNull())
	{
		return;
	}
#else
	Json::Value* items = (Json::Value*)lpGroup;
	if (!items || items->isNull())
	{
		return;
	}

	//filter test attribute item
	Json::Value realitems(Json::arrayValue);
	for (int i = 0; i < (int)items->size(); ++i)
	{
		Json::Value& item = (*items)[i];
		if (item.isNull())
		{
			continue;
		}

		Json::Value& test	= item["test"];
		if (!test.isNull()
			&& test.asBool())
		{
			continue;
		}
		realitems.append(item);
	}
	items = &realitems;
#endif

	int nItems= (int)items->size();
	if (items <= 0)
	{
		return;
	}

	Json::Value ShowItems(Json::arrayValue);
	for (int i = 0; i < nItems; ++i)
	{
		Json::Value& item = (*items)[i];
		if (item.isNull())
		{
			continue;
		}

		Json::Value& type = item["type"];
		if (type.isNull())
		{
			ShowItems.append(item);
			continue;
		}

		int nType = type.asInt();

		if (!g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY)
			&& nType != 0)
		{
			continue;
		}

		ShowItems.append(item);
	}

	nItems= (int)ShowItems.size();
	if (items <= 0)
	{
		return;
	}


// 	DWORD szTypes[]			= {
// 		Magnifier,
// 		Spotlight,
// 		BlackBoard,
// 		Roster,
// 		PushScreen,
// 		CourseTraining,
// 		CourseSummary,
// 		AssignHomework,
// 	};
// 
// 	TCHAR szTypeNames[][32]	= {
// 		_T("放大镜"),
// 		_T("聚光灯"),
// 		_T("黑板"),
// 		_T("花名册"),
// 		_T("推屏"),
// 		_T("发送任务"),
// 		_T("课堂总结"),
// 		_T("布置作业"),
// 	};


	int nCol	= m_pList->GetColumns();
	int nRows	= nItems / nCol + ( nItems % nCol ?  1 : 0);

	if (nRows == 1 && nItems < nCol )
	{
		nCol = nItems;
		m_pList->SetColumns(nCol);
		
	}

	for (int i = 0; i < nRows * nCol; ++i)
	{
		CToolItemUI* pItem = (CToolItemUI*)CreateItemUI(0);

		if (i < nItems)
		{
			Json::Value& item = (*items)[i];
			if (item.isNull())
			{
				continue;
			}

			Json::Value& name = item["name"];
			Json::Value& val = item["value"];
			Json::Value& type = item["type"];
			Json::Value& test = item["test"];
			if (name.isNull() && val.isNull())
			{
				continue;
			}

			int nType = 0;
			if (!type.isNull())
			{
				nType = type.asInt();
			}

			if (test.asBool())
			{
				pItem->ShowTestIcon(true);
			}

			tstring strName = Utf82Str(name.asCString());

			pItem->SetTag((UINT_PTR)val.asInt());
			pItem->SetTitle(strName.c_str());

			tstring strIcon = CPaintManagerUI::GetResourcePath();
			strIcon += _T("ScreenTool\\interacts\\");
			strIcon += strName;
			strIcon += _T(".png");
			pItem->SetIcon(strIcon.c_str());
			pItem->SetIconRect(38,38);
		}
		else
		{
			pItem->SetEmpty(true);
		}

		pItem->SetName(_T("interact_item"));
		pItem->SetFixedWidth(70);
		pItem->SetFixedHeight(70);


		//for border
		if ( i < nCol)
		{
			pItem->ShowTopBorder(false);
		}

		if (i % nCol == 0)
		{
			pItem->ShowLeftBorder(false);
		}
		m_pList->Add(pItem);
	}


	m_pList->SetFixedWidth(70 * nCol);
	m_pList->SetFixedHeight(70* nRows);

}

CControlUI* CInteractToolViewUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("ToolItem"), pstrClass) == 0)
	{
		return new CToolItemUI;
	}

	return NULL;
}
