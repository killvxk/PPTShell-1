#pragma once

#include <map>
#include <GdiPlus.h>
using namespace Gdiplus;


class CSkinManager
{
private:
	CSkinManager();
	~CSkinManager();
public:
    static CSkinManager*		GetInstance();
	void						SetSkinPath(LPCTSTR lpszSkinPath);
	CString						GetSkinPath();
	Image*						GetSkinItem(LPCTSTR lpszFileName);
	static void					UnLoadSkin(); 
private:
	std::map<CString,Image*>	m_mapSkinItems;
	static CSkinManager*		m_Instance;

	CString						m_strSkinPath;
	
	ULONG_PTR					gdiplusToken;

};
