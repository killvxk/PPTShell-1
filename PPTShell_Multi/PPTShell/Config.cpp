#include "StdAfx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	m_strVerion	= _T("未知版本");
	m_dwCheckUpdateStatus = eUpdateType_Newset;
	m_dwUpdatedStatus = eUpdateType_Newset;
	Init();
}

CConfig::~CConfig(void)
{
}

BOOL CConfig::Init()
{
	tstring strPath		= GetLocalPath();
	m_strConfigPath		= strPath + _T("\\") + CONFIG_FOLDER_NAME;
	m_strConfigName		= m_strConfigPath + _T("\\") + INI_CONFIG_FILE_NAME;
	m_strLocalFilePath	= m_strConfigPath + _T("\\") + JSON_LOCAL_FILE_NAME;
	m_strVersionPath	= strPath + _T("\\Version.dat");

	tstring m_strModuleVisibelXmlPath = strPath + _T("\\Skins\\Config\\ModuleVisible.xml");
	m_bDocModuleVisible = m_docModuleVisible.LoadFile(m_strModuleVisibelXmlPath.c_str());
	CreateDirectory(m_strConfigPath.c_str(), NULL);

	return TRUE;
}

BOOL CConfig::SaveString( tstring strSection, tstring strKey, tstring strContent )
{
	return WritePrivateProfileString(strSection.c_str(), strKey.c_str(), strContent.c_str(), m_strConfigName.c_str());
}

tstring CConfig::GetString( tstring strSection, tstring strKey )
{
	TCHAR szContent[MAX_PATH] = {0};
	DWORD dwSize = 0;
	GetPrivateProfileString(strSection.c_str(), strKey.c_str(), _T(""), szContent, dwSize, m_strConfigName.c_str());

	return szContent;
}

BOOL CConfig::SaveInt( tstring strSection, tstring strKey, int nValue )
{
	TCHAR szValue[MAX_PATH] = {0};
	wsprintf(szValue, _T("%d"), nValue);
	return WritePrivateProfileString(strSection.c_str(), strKey.c_str(), szValue, m_strConfigName.c_str());
}

int CConfig::GetInt( tstring strSection, tstring strKey )
{
	return GetPrivateProfileInt(strSection.c_str(), strKey.c_str(), 0, m_strConfigName.c_str());
}

BOOL CConfig::GetLocalFileJosn( map<tstring, list<LOCAL_RES_INFO>>& mapLocalFile )
{
	FILE* pf = _tfopen(m_strLocalFilePath.c_str(), _T("r"));

	if ( pf == NULL )
		return FALSE;

	fseek(pf, 0,  SEEK_END);
	int nSize = ftell(pf);
	fseek(pf, 0, SEEK_SET);

	DWORD dwSize = 0;
	char* pBuff = new char[nSize + 1];
	memset(pBuff, 0, nSize + 1);
	fread(pBuff, nSize, 1, pf);
	fclose(pf);

	//
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = mapLocalFile.begin();
	for (iter; iter != mapLocalFile.end(); iter++)
	{
		list<LOCAL_RES_INFO> lstFiles = iter->second;
		lstFiles.clear();
	}
	mapLocalFile.clear();

	Json::Reader	jsonReader;

	Json::Value		rootItem;
	Json::Value		folderItems(Json::arrayValue);
	Json::Value		folderItem;
	Json::Value		fileItems(Json::arrayValue);

	int nCount		= 0;
	if ( jsonReader.parse(pBuff, rootItem) )
	{
		folderItems = rootItem["folder"];
		for (int m = 0; m < folderItems.size(); m++)
		{
			tstring strFolder	= folderItems[m]["name"].asString();
			fileItems			= folderItems[m]["file"];
			list<LOCAL_RES_INFO> lstFiles;
			LOCAL_RES_INFO resInfo;

			if (strFolder.compare(_T("其他")) == 0)
				continue;

			for ( int i = 0; i <  fileItems.size(); i++)
			{
				Json::Value fileItem		= fileItems[i]["name"];
				Json::Value fileChapterGuid	= fileItems[i]["guid"];
				Json::Value fileChapter		= fileItems[i]["chapter"];
				Json::Value fileTitle		= fileItems[i]["title"];
				Json::Value fileType		= fileItems[i]["type"];

				resInfo.strChapterGuid		= fileChapterGuid.asString();
				resInfo.strChapter			= fileChapter.asString();
				resInfo.strResPath			= fileItem.asString();
				resInfo.strTitle			= fileTitle.asString();
				resInfo.nType				= fileType.asInt();

				lstFiles.push_back(resInfo);
			}

			mapLocalFile[strFolder] = lstFiles;
		}
	}

	delete pBuff;

	return TRUE;
}

BOOL CConfig::WriteLocalFileJosn( map<tstring, list<LOCAL_RES_INFO>>& mapLocalFile )
{
	Json::Value		rootItem;
	Json::Value		folderItems(Json::arrayValue);
	Json::Value		folderItem;
	Json::Value		fileItems(Json::arrayValue);
 
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = mapLocalFile.begin();
	for(iter; iter != mapLocalFile.end(); iter++)
	{
		folderItem["name"] = iter->first;
		
		list<LOCAL_RES_INFO>::iterator	lstIter = iter->second.begin();
		
		fileItems.clear();
		for (lstIter; lstIter != iter->second.end(); lstIter++)
		{
			Json::Value fileItem;
			LOCAL_RES_INFO resInfo = *lstIter;
			
			fileItem["guid"]	= resInfo.strChapterGuid;
			fileItem["chapter"]	= resInfo.strChapter;
			fileItem["name"]	= resInfo.strResPath;
			fileItem["title"]	= resInfo.strTitle;
			fileItem["type"]	= resInfo.nType;

			fileItems.append(fileItem);
		}
		
		folderItem["file"] = fileItems;
		folderItems.append(folderItem);
	}
	
	rootItem["folder"] = folderItems;

	string strFolders = rootItem.toStyledString();

	FILE* pf = _tfopen(m_strLocalFilePath.c_str(), _T("w+"));

	if ( pf != NULL )
	{
		fwrite(strFolders.c_str(), strFolders.length(), 1, pf);
		fclose(pf);
	}

	return TRUE;
}

tstring CConfig::GetVersion()
{
	FILE* pf = _tfopen(m_strVersionPath.c_str(), _T("r"));

	if ( pf != NULL )
	{
		TCHAR szVersion[52] = {0};
		fread(szVersion, sizeof(szVersion)-1, 1, pf);
		fclose(pf);
		m_strVerion = szVersion;
	}

	return m_strVerion;
}


BOOL CConfig::GetModuleVisible(LPCTSTR szModule)
{
	if(m_bDocModuleVisible)
	{
		TiXmlElement* pRootElement = m_docModuleVisible.FirstChildElement();
		if( pRootElement)
		{
			TiXmlElement* pModuleListsElement = GetElementsByTagName(pRootElement, "ModuleLists");
			if(pModuleListsElement)
			{
				TiXmlElement* pModuleElement = GetElementsByTagName(pModuleListsElement, szModule);
				if(pModuleElement)
				{
					TiXmlElement* pVisibleElement = GetElementsByTagName(pModuleElement, _T("Visible"));
					const char *szText = pVisibleElement->GetText();
					if(szText && _tcsicmp(szText, _T("true")) == 0)
						return TRUE;
				}
			}
		}
	}
	if( _tcsicmp(szModule,MODULE_ND_ICRPLAY) == 0)
		return FALSE;
	else
		return TRUE;
}

void CConfig::SetCheckUpdateStatus( int nUpdate )
{
	m_dwCheckUpdateStatus = nUpdate;
}

DWORD CConfig::GetCheckUpdateStatus()
{
	return m_dwCheckUpdateStatus;
}

void CConfig::SetCheckUpdateCount()
{
	m_dwCheckUpdateCount = GetTickCount();
}

void CConfig::SetStartAppCount()
{
	m_dwStartAppCount = GetTickCount();
}

DWORD CConfig::GetCheckUpdateCount()
{
	return m_dwCheckUpdateCount;
}

DWORD CConfig::GetStartAppCount()
{
	return m_dwStartAppCount;
}

void CConfig::SetUpdatedStatus( int nUpdate )
{
	m_dwUpdatedStatus = nUpdate;
}

DWORD CConfig::GetUpdatedStatus()
{
	return m_dwUpdatedStatus;
}
