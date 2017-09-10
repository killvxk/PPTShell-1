#pragma once

#include <map>
#include "Utils.h"
#include "../Core/UIMarkup.h"

using namespace std;


#define LANG_SIGN				_T("@string/")

//定于语言名字
#define LANG_CODE_ZHCN          _T("zh-cn.xml")
#define LANG_CODE_ZANG          _T("zang.xml")

namespace DuiLib
{
	class CLanguageMap
	{
	public:
		map<CDuiString, CDuiString> m_mapLanguage;
	};

	
	class CLanguageMap;
	
	class UILIB_API CLanguage
	{
	public:

	public:
		static CLanguage* GetInstance();

	public:
		bool		SetLangCode(CDuiString strCode);
		CDuiString	GetString(DWORD dwID);
		CDuiString	GetString(LPCTSTR strID);

	private:
		CLanguage(void);
		~CLanguage(void);

		CDuiString	GetLanguageDir();
		bool		LoadLanguageFile();

	private:
		CDuiString			m_strLanguageDir;
		CDuiString			m_strLangCode;
		
		CMarkup				m_xml;

		CLanguageMap*		m_pLanguageMap;

		static CLanguage*	m_pInstance;

	};

	//获取字符串串宏，简化调用代码  ID -- 支持string 和 Dword
	#define GETLANGSTRING(ID) CLanguage::GetInstance()->GetString(ID);

}
