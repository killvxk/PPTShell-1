#include "StdAfx.h"
#include "SubjectToolView.h"
#include "ExecToolItem.h"
#include "SubjectToolItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "NDCloud/NDCloudAPI.h"
#include "ScreenInstrument.h"
#include "Util/Util.h"

DuiLib::CDialogBuilder CSubjectToolViewUI::m_GroupBuilder;
DuiLib::CDialogBuilder CSubjectToolViewUI::m_ItemBuilder;
CSubjectToolViewUI::CSubjectToolViewUI(void)
{
	m_nItemType			= 0;
	m_nContentHeight	= 0;

}

CSubjectToolViewUI::~CSubjectToolViewUI(void)
{
	GetManager()->RemoveNotifier(this);
}


CControlUI* CSubjectToolViewUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("ToolItem"), pstrClass) == 0)
	{
		if (m_nItemType == eTool_Geometry)
		{
			return new CSubjectToolItemUI;
		}
		else if (m_nItemType == eTool_Exec)
		{
			return new CExecToolItemUI;
		}
		else if (m_nItemType == eTool_Empty)
		{
			return new CToolItemUI;
		}
	}

	return NULL;
}


void CSubjectToolViewUI::Init()
{
	__super::Init();

	m_pList = (CContainerUI*)FindSubControl(_T("subjects_tools"));
	m_pMask = (CContainerUI*)FindSubControl(_T("mainWindowLayout"));

	CreateSubjectWithConfig();

	CScreenInstrumentUI::GetMainInstrument()->GetPagger()->AddPageListener(this);
	GetManager()->AddNotifier(this);
}

void CSubjectToolViewUI::CreateGroupFromDirectory( LPCTSTR lptcsGroupDir, LPCTSTR lptcsGroupName )
{

	TCHAR szDir[MAX_PATH * 2] = {0};
	_stprintf_s(szDir, _T("%s%s\\"), GetToolsPath().c_str(), lptcsGroupDir);

	tstring strPath = szDir;
	strPath += _T("*.html");
	WIN32_FIND_DATA FindFileData;  

	HANDLE hFind = ::FindFirstFile(strPath.c_str(), &FindFileData);  
	if(INVALID_HANDLE_VALUE == hFind)
		return;

	CContainerUI*	pGroup		= CreateGroupUI(lptcsGroupName);
	CTileLayoutUI*	pGroupList	= GetGroupList(pGroup);
	if (!pGroup)
	{
		return;
	}

	vector<tstring> vctrFiles;
	do 
	{
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  
		{  
			vctrFiles.push_back(FindFileData.cFileName);
		}

	} while (FindNextFile(hFind,&FindFileData));
	FindClose(hFind);

	int nItems= (int)vctrFiles.size();
	if (nItems <= 0)
	{
		return;
	}

	int nCol	= pGroupList->GetColumns();	
	int nRows	= nItems / nCol + ( nItems % nCol ?  1 : 0);

	for (int i = 0; i < nRows * nCol; i++)
	{
		CToolItemUI* pItem = NULL;
		if (i < nItems)
		{
			pItem = (CToolItemUI*)CreateItemUI(eTool_Geometry);
			LPCTSTR lpFileName = vctrFiles.at(i).c_str();
			tstring strToolPath = szDir;
			strToolPath += lpFileName;
			pItem->SetResource(strToolPath.c_str());

			LPTSTR lpDot = (LPTSTR)_tcsrchr(lpFileName, _T('.'));
			if (lpDot)
			{
				*lpDot = _T('\0');
			}
			pItem->SetTitle(lpFileName);

			tstring strIcon = CPaintManagerUI::GetResourcePath();
			strIcon += _T("ScreenTool\\subjects\\");
			strIcon += lpFileName;
			strIcon += _T(".png");
			pItem->SetIcon(strIcon.c_str());
		}
		else
		{
			pItem = (CToolItemUI*)CreateItemUI(eTool_Empty);
			pItem->SetEmpty(true);
		}

		pItem->SetFixedWidth(70);
		pItem->SetFixedHeight(70);
		pGroupList->Add(pItem);

		//for border
		if ( i < nCol)
		{
			pItem->ShowTopBorder(false);
		}

		if (i % nCol == 0)
		{
			pItem->ShowLeftBorder(false);
		}

	}
	pGroupList->SetFixedHeight(nRows * 70);
	pGroup->SetFixedHeight(nRows * 70 + 40);

	m_pList->Add(pGroup);

}


CTileLayoutUI* CSubjectToolViewUI::GetGroupList( CContainerUI* pGroup )
{
	if (!pGroup)
	{
		return NULL;
	}
	return (CTileLayoutUI*)pGroup->FindSubControl(_T("group_list"));
}

CControlUI* CSubjectToolViewUI::CreateItemUI( int nToolType)
{
	m_nItemType = nToolType;

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

CContainerUI* CSubjectToolViewUI::CreateGroupUI( LPCTSTR lptcsGroup )
{
	CContainerUI* pGroup = NULL;
	if (!m_GroupBuilder.GetMarkup()->IsValid())
	{
		pGroup = (CContainerUI*)m_GroupBuilder.Create(_T("ScreenTool\\SubjectGroupStyle.xml"));
	}
	else
	{
		pGroup = (CContainerUI*)m_GroupBuilder.Create();
	}

	if (!pGroup)
	{
		return NULL;
	}

	CLabelUI* pGroupName = (CLabelUI*)pGroup->FindSubControl(_T("group_name"));
	if (pGroupName && lptcsGroup)
	{
		pGroupName->SetText(lptcsGroup);
	}

	return pGroup;
}

void CSubjectToolViewUI::CreateGroupFromValue( LPCTSTR lptcsGroup, LPVOID lpGroup )
{
	Json::Value* items = (Json::Value*)lpGroup;
	if (!items || items->isNull())
	{
		return;
	}

	int nItems= (int)items->size();
	if (items <= 0)
	{
		return;
	}

	CContainerUI* pGroup		= CreateGroupUI(lptcsGroup);
	CTileLayoutUI* pGroupList	= GetGroupList(pGroup);
	if (!pGroupList)
	{
		return;
	}

	int nCol	= pGroupList->GetColumns();	
	int nRows = nItems / nCol + ( nItems % nCol ?  1 : 0);

	for (int i = 0; i < nRows * nCol; ++i)
	{
		CToolItemUI* pItem = NULL;

		if (i < nItems)
		{
			Json::Value& item = (*items)[i];
			if (item.isNull())
			{
				continue;
			}

			Json::Value& name = item["name"];
			Json::Value& type = item["type"];
			if (name.isNull())
			{
				continue;
			}

			if (type.isNull())
			{
				type = eTool_Geometry;
			}

			if (type.asInt() == eTool_Geometry)
			{
				pItem = (CToolItemUI*)CreateItemUI(type.asInt());
				tstring strName = name.asCString();

				TCHAR szPath[MAX_PATH * 2] = {0};
				_stprintf_s(szPath, _T("%s%s\\%s.html"), GetToolsPath().c_str(),strName.c_str(), strName.c_str());

				pItem->SetResource(szPath);
				pItem->SetTitle(strName.c_str());

				tstring strIcon = CPaintManagerUI::GetResourcePath();
				strIcon += _T("ScreenTool\\subjects\\");
				strIcon += strName;
				strIcon += _T(".png");
				pItem->SetIcon(strIcon.c_str());

			}
			else if (type.asInt() == eTool_Exec)
			{
				pItem = (CToolItemUI*)CreateItemUI(type.asInt());
				Json::Value val = item["value"];
				if (!val.isNull())
				{
					tstring strName = name.asCString();

					pItem->SetResource(val.asCString());
					pItem->SetTitle(strName.c_str());

					tstring strIcon = CPaintManagerUI::GetResourcePath();
					strIcon += _T("ScreenTool\\subjects\\");
					strIcon += strName;
					strIcon += _T(".png");
					pItem->SetIcon(strIcon.c_str());
				}
			}
		}
		else
		{
			pItem = (CToolItemUI*)CreateItemUI(eTool_Empty);
			pItem->SetEmpty(true);
		}


		pItem->SetFixedWidth(70);
		pItem->SetFixedHeight(70);
		pGroupList->Add(pItem);

		//for border
		if ( i < nCol)
		{
			pItem->ShowTopBorder(false);
		}

		if (i % nCol == 0)
		{
			pItem->ShowLeftBorder(false);
		}
	}

	pGroupList->SetFixedHeight(nRows * 70);
	pGroup->SetFixedHeight(nRows * 70 + 40);

	m_pList->Add(pGroup);
}
void CSubjectToolViewUI::OnSubjectItemClick( TNotifyUI& msg )
{
	CSubjectToolItemUI* pItem = dynamic_cast<CSubjectToolItemUI*>(msg.pSender);
	if (!pItem || pItem->IsEmpty())
	{
		return;
	}

	pItem->OnClick(this);
	this->SetVisible(false);
}

void CSubjectToolViewUI::CreateSubjectWithConfig()
{
	tstring strConfigPath = this->GetManager()->GetResourcePath();
	strConfigPath += _T("ScreenTool\\subjects\\Config.txt");

	// read config from file
	FILE* fp = NULL;
	int err = _tfopen_s(&fp, strConfigPath.c_str(), "rb");
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
	bool res = reader.parse(pBuffer, root);

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

		if (type.asInt() == eCreate_FromDirectory)
		{
			CreateGroupFromDirectory(val.asCString(), name.asCString());
		}
		else if (type.asInt() == eCreate_FromValue)
		{
			CreateGroupFromValue(name.asCString(), &val);
		}

	}

	//blank
	if (m_pList)
	{
		CControlUI* pCtrl = new CControlUI;
		pCtrl->SetFixedHeight(40);
		m_pList->Add(pCtrl);
	}
	

}

void CSubjectToolViewUI::OnPageChangeBefore()
{
	
}
