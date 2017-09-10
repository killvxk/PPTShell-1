#include "StdAfx.h"
#include "Language.h"

namespace DuiLib
{
	CLanguage* CLanguage::m_pInstance = NULL;
	
	CLanguage::CLanguage(void)
	{
		m_pLanguageMap		= new CLanguageMap;
		m_strLanguageDir	= _T("");
		m_strLangCode		= LANG_CODE_ZHCN;
		LoadLanguageFile();
	}

	CLanguage::~CLanguage(void)
	{
		if ( m_pLanguageMap != NULL )
			delete m_pLanguageMap;

		if ( m_pInstance != NULL )
			delete m_pInstance;
	}

	CLanguage* CLanguage::GetInstance()
	{
		if ( m_pInstance == NULL )
			m_pInstance = new CLanguage;

		return m_pInstance;
	}

	bool CLanguage::SetLangCode( CDuiString strCode )
	{
		if (strCode == m_strLangCode)
		{
			return true;
		}

		m_strLangCode = strCode;
		return LoadLanguageFile();
	}

	DuiLib::CDuiString CLanguage::GetLanguageDir()
	{
		if ( !m_strLanguageDir.IsEmpty() )
			return m_strLanguageDir;

		TCHAR szModulePath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szModulePath, MAX_PATH);

		TCHAR* pPos = _tcsrchr(szModulePath, '\\');
		if ( pPos != NULL )
			*pPos='\0';

		m_strLanguageDir = szModulePath;

		m_strLanguageDir += _T("\\Language\\");

		return m_strLanguageDir;

	}

	bool CLanguage::LoadLanguageFile()
	{
		m_pLanguageMap->m_mapLanguage.clear();

		CDuiString strLangPath = GetLanguageDir() + m_strLangCode;

		 if( !m_xml.LoadFromFile(strLangPath) ) 
			 return false;

		 CMarkupNode root = m_xml.GetRoot();
		 if( !root.IsValid() ) 
			 return false;

		 LPCTSTR pstrClass	= NULL;
		 int nAttributes	= 0;
		 LPCTSTR pstrName	= NULL;
		 LPCTSTR pstrValue	= NULL;
		 LPTSTR pstr		= NULL;
		 for( CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling() ) 
		 {
			 pstrClass = node.GetName();
			
			if ( _tcsicmp(pstrClass, _T("skin")) == 0 )
			{
				 for( CMarkupNode chlidnode = node.GetChild(); chlidnode.IsValid(); chlidnode = chlidnode.GetSibling() )
				 {
					 pstrClass = chlidnode.GetName();

					 if ( _tcsicmp(pstrClass, _T("text")) == 0 )
					 {
						 nAttributes		= chlidnode.GetAttributeCount();
						 DWORD dwId			= 0;
						 LPCTSTR pTextId	= NULL;
						 LPCTSTR pTextValue	= NULL;
						 for( int i = 0; i < nAttributes; i++ ) 
						 {
							 pstrName	= chlidnode.GetAttributeName(i);
							 pstrValue	= chlidnode.GetAttributeValue(i);
							 if( _tcsicmp(pstrName, _T("id")) == 0 ) 
							 {
								 //dwId =  _ttol(pstrValue);
								 pTextId = pstrValue;
							 }
							 else if( _tcsicmp(pstrName, _T("value")) == 0 ) 
							 {
								 pTextValue = pstrValue;
							 }
						 }

						 m_pLanguageMap->m_mapLanguage[pTextId] = pTextValue;
					 }
				 }
			}
			
		 }
		
		return true;
	}

	DuiLib::CDuiString CLanguage::GetString( DWORD dwID )
	{
		TCHAR szID[MAX_PATH] = {0};
		wsprintf(szID, _T("%d"), dwID);
		return GetString(szID);
	}

	DuiLib::CDuiString CLanguage::GetString( LPCTSTR strID )
	{
		map<CDuiString, CDuiString>::iterator iter = m_pLanguageMap->m_mapLanguage.find(strID);
		if ( iter != m_pLanguageMap->m_mapLanguage.end() )
			return iter->second;

		return _T("");
	}

}
