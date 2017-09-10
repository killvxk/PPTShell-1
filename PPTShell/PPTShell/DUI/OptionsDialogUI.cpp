#include "StdAfx.h"
#include "OptionsDialogUI.h"
#include "Util/Util.h"
#include "Config.h"
#include "ClearMask.h"
#include "PasswordLoginDialogUI.h"
#include "ModifyPasswordDialogUI.h"
#include "Encode.h"
#include "NDCloud\NDCloudUser.h" 
#include "..\Plugins\Icr\IcrPlayer.h"
#include <ctype.h>

#define  PPT_REGISTER_KEY_COUNT		3
#define  PPT_RESET_KEY_COUNT		2

BOOL COptionsDialogUI::isICROpen = FALSE;		   //互动课堂是否已经初始化

COptionsDialogUI::COptionsDialogUI(void)
{
}

COptionsDialogUI::~COptionsDialogUI(void)
{
}

LPCTSTR COptionsDialogUI::GetWindowClassName( void ) const
{
	return _T("OptionsDialog");
}

DuiLib::CDuiString COptionsDialogUI::GetSkinFile()
{
	return _T("Options\\Options.xml");
}

DuiLib::CDuiString COptionsDialogUI::GetSkinFolder()
{
	return _T("skins");
}

void COptionsDialogUI::InitWindow()
{
	COptionUI* pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("setting_Advanced")));
	CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("setting_Advanced_Dot")));

	pOption->SetFont(120100);
	pOption->SetTextColor(0xFF11b0b6);
	pControl->SetBkImage(_T("Options\\dot2.png"));

	m_strOptionSelected = pOption->GetName();
	m_strDotSelected = pControl->GetName();


	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");
	
	//高级设置
	TCHAR szOpenMode[MAX_PATH + 1];
	CCheckBoxUI* pCheckBoxOpenMode = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkOpenMode")));
	GetPrivateProfileString(_T("Option"), _T("openMode"), _T("101ppt"), szOpenMode, MAX_PATH, strConfinFile.c_str());
	if (_tcscmp(_T("101ppt"), szOpenMode) == 0)
	{
		pCheckBoxOpenMode->SetCheck(TRUE);
	}
	else
	{
		pCheckBoxOpenMode->SetCheck(FALSE);
	}

	CCheckBoxUI* pCheckBoxIcrPlay = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkIcrplay")));
	if( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY))
	{
		pCheckBoxIcrPlay->SetCheck(TRUE);
	}
	else
	{
		pCheckBoxIcrPlay->SetCheck(FALSE);
	}

	//保存位置管理
	TCHAR szSavePath[MAX_PATH + 1];
	GetPrivateProfileString(_T("config"), _T("NDCloudPath"), _T(""), szSavePath, MAX_PATH, strConfinFile.c_str());
	CRichEditUI* pEditSavePath = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editPath")));
	if (_tcscmp(szSavePath, _T("")) != 0)
	{
		m_SavePath = szSavePath;
		_tcscat_s(szSavePath, _T("\\NDCloud"));	
		pEditSavePath->SetText(szSavePath);
	}
	else
	{
		tstring strSavePath = GetDefaultDirectory();
		m_SavePath = strSavePath;
		strSavePath += _T("\\NDCloud");
		pEditSavePath->SetText(strSavePath.c_str());
	}



	//常规设置
	CString password = GetPassword();
	if (password.IsEmpty())
	{
		CCheckBoxUI* pCheckBoxMakePassword = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkMakePassword")));
		pCheckBoxMakePassword->SetCheck(false);
		SwitchView(1);
	}
	else
	{
		CCheckBoxUI* pCheckBoxMakePassword = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkMakePassword")));
		pCheckBoxMakePassword->SetCheck(true);
		SwitchView(4);
	}

	TCHAR szAutorun[MAX_PATH + 1];
	GetPrivateProfileString(_T("Option"), _T("autoRun"), _T(""), szAutorun, MAX_PATH, strConfinFile.c_str());
	if (_tcscmp(szAutorun, _T("true")) == 0)
	{
		CCheckBoxUI* pCheckBoxAutoRun = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkAutoRun")));
		pCheckBoxAutoRun->SetCheck(true);
	}
	else
	{
		CCheckBoxUI* pCheckBoxAutoRun = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkAutoRun")));
		pCheckBoxAutoRun->SetCheck(false);
	}

}

void COptionsDialogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	Close();
}

void COptionsDialogUI::OnBtnOKClick(TNotifyUI& msg)
{
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	CCheckBoxUI* pCheckBoxOpenMode = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkOpenMode")));
	if (pCheckBoxOpenMode->GetCheck())
	{
		RegisterOpenMode();
		WritePrivateProfileString(_T("Option"), _T("openMode"), _T("101ppt"), strConfinFile.c_str());
	}
	else
	{
		ResetOpenMode();
		WritePrivateProfileString(_T("Option"), _T("openMode"), _T("MSppt"), strConfinFile.c_str());
	}

	CCheckBoxUI* pCheckBoxIcrPlay = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkIcrplay")));
	if (pCheckBoxIcrPlay->GetCheck())
	{
		CheckXmlExist(); //判断ModuleVisible.xml是否存在，如果不存在则创建
		g_Config::GetInstance()->SetModuleVisible(MODULE_ND_ICRPLAY, true);
		if (!isICROpen)
		{
			//IcrPlayer::GetInstance()->Initialize(); //互动课堂立即生效
			isICROpen = TRUE;
		}

	}
	else
	{
		CheckXmlExist(); //判断ModuleVisible.xml是否存在，如果不存在则创建
		g_Config::GetInstance()->SetModuleVisible(MODULE_ND_ICRPLAY, false);
	}

	CRichEditUI* pEditSavePath = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editPath")));
	CString savePath = pEditSavePath->GetText().GetData();
	if (_tcscmp(savePath, _T("")) != 0)
	{
		CString savePathcfg = savePath.Left(savePath.GetLength() - 8);
		WritePrivateProfileString(_T("config"), _T("NDCloudPath"), savePathcfg.GetBuffer(), strConfinFile.c_str());
	}
	else
	{
		WritePrivateProfileString(_T("config"), _T("NDCloudPath"), savePath.GetBuffer(), strConfinFile.c_str());
	}
	

	CCheckBoxUI* pCheckBoxAutoRun = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkAutoRun")));

	//HKEY hRoot = HKEY_LOCAL_MACHINE;
	//TCHAR szSubKey[100] = _T("Software//Microsoft//Windows//CurrentVersion//Run");
	HKEY hKey;// 打开指定子键
	//DWORD dwDisposition = REG_OPENED_EXISTING_KEY;
	//LONG lRet =  RegCreateKeyEx(hRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_READ|KEY_SET_VALUE, &hKey);

	if(lRet != ERROR_SUCCESS)
	{
		//OutputDebugString(_T("打开键值失败"));
	}

	if (pCheckBoxAutoRun->GetCheck())
	{
		// 得到当前执行文件的文件名（包含路径）
		TCHAR szModule[MAX_PATH] ;
		GetModuleFileName (NULL, szModule, MAX_PATH);
		// 创建一个新的键值，设置键值数据为文件名
		lRet = RegSetValueEx(hKey,_T("pptShell"),0, REG_SZ, (BYTE*)szModule, _tcslen(szModule));
		if(lRet != ERROR_SUCCESS)
		{
			//OutputDebugString(_T("设置键值失败"));
		}
		WritePrivateProfileString(_T("Option"), _T("autoRun"), _T("true"), strConfinFile.c_str());
	}
	else
	{
		lRet = RegDeleteValue(hKey, _T("pptShell"));
		if(lRet != ERROR_SUCCESS)
		{
			//OutputDebugString(_T("删除键值失败"));
		}
		WritePrivateProfileString(_T("Option"), _T("autoRun"), _T("false"), strConfinFile.c_str());
	}
	// 关闭子键句柄
	RegCloseKey(hKey);

	Close();
}

void COptionsDialogUI::OnOptionSelectChanged(TNotifyUI& msg)
{
	CDuiString    strName  = msg.pSender->GetName();
	CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("OptionTab")));

	OptionColorChanged(strName);

	if(strName == _T("setting_Advanced"))
		pControl->SelectItem(0);
	else if(strName == _T("setting_SavePath"))
		pControl->SelectItem(1);
	else if(strName == _T("setting_Individuation"))
		pControl->SelectItem(2);
	else if (strName == _T("setting_Convention"))
		pControl->SelectItem(3);
}

void COptionsDialogUI::OptionColorChanged(CDuiString strName)
{
	if (m_strOptionSelected == strName)
	{
		return;
	}
	else
	{
		COptionUI* pOption;
		CControlUI* pControl;
		if (strName == _T("setting_Advanced"))
		{
			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("setting_Advanced")));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("setting_Advanced_Dot")));

			pOption->SetFont(120100);
			pOption->SetTextColor(0xFF11b0b6);
			pControl->SetBkImage(_T("Options\\dot2.png"));

			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(m_strOptionSelected));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(m_strDotSelected));
			pOption->SetFont(120000);
			pOption->SetTextColor(0xFF333333);
			pControl->SetBkImage(_T("Options\\dot.png"));
			
			m_strOptionSelected = _T("setting_Advanced");
			m_strDotSelected = _T("setting_Advanced_Dot");
		}
		else if (strName == _T("setting_SavePath"))
		{
			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("setting_SavePath")));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("setting_SavePath_Dot")));

			pOption->SetFont(120100);
			pOption->SetTextColor(0xFF11b0b6);
			pControl->SetBkImage(_T("Options\\dot2.png"));

			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(m_strOptionSelected));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(m_strDotSelected));
			pOption->SetFont(120000);
			pOption->SetTextColor(0xFF333333);
			pControl->SetBkImage(_T("Options\\dot.png"));

			m_strOptionSelected = _T("setting_SavePath");
			m_strDotSelected = _T("setting_SavePath_Dot");
		}
		else if (strName == _T("setting_Individuation"))
		{
			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("setting_Individuation")));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("setting_Individuation_Dot")));

			pOption->SetFont(120100);
			pOption->SetTextColor(0xFF11b0b6);
			pControl->SetBkImage(_T("Options\\dot2.png"));

			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(m_strOptionSelected));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(m_strDotSelected));
			pOption->SetFont(120000);
			pOption->SetTextColor(0xFF333333);
			pControl->SetBkImage(_T("Options\\dot.png"));

			m_strOptionSelected = _T("setting_Individuation");
			m_strDotSelected = _T("setting_Individuation_Dot");
		}
		else if (strName == _T("setting_Convention"))
		{
			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("setting_Convention")));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("setting_Convention_Dot")));

			pOption->SetFont(120100);
			pOption->SetTextColor(0xFF11b0b6);
			pControl->SetBkImage(_T("Options\\dot2.png"));

			pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(m_strOptionSelected));
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(m_strDotSelected));
			pOption->SetFont(120000);
			pOption->SetTextColor(0xFF333333);
			pControl->SetBkImage(_T("Options\\dot.png"));

			m_strOptionSelected = _T("setting_Convention");
			m_strDotSelected = _T("setting_Convention_Dot");
		}
	}
	
}

void COptionsDialogUI::RegisterOpenMode()
{
	//使用这两个函数进行关联的示例代码

	//BOOL bUsed = GetPrivateProfileInt("App", "Reg", 0, m_strAppPath+INI_NAME);

	//if ( bUsed )
	//	return;

	try
	{
		OSVERSIONINFOEX osvi;  
		BOOL bOsVersionInfoEx;  

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));  
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);  
		bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);  

		// win7的系统版本为NT6.1   win8需要盾牌 ，正式版需要打开检测
		//if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId &&    
		//	osvi.dwMajorVersion >= 6 &&   
		//	osvi.dwMinorVersion >= 2 )  
		//{  

		tstring strExt[PPT_REGISTER_KEY_COUNT];
		strExt[0] = _T(".ppt");
		strExt[1] = _T(".pptx");
		strExt[2] = _T(".ndpx");

		TCHAR strAppKey[20] = _T("PPTShell"); 


		// 以下2段修改ppt关联的会导致子ppt无法播放

		for (int i = 0; i < PPT_REGISTER_KEY_COUNT; i++ )
		{
			BOOL relationExists = CheckFileRelation(strExt[i].c_str(), strAppKey);
			if(!relationExists)
			{
				TCHAR szExePath[MAX_PATH] = {0};
				GetModuleFileName(NULL, szExePath, MAX_PATH);
				TCHAR strDefaultIcon[MAX_PATH + 1] = {0};       
				wsprintf(strDefaultIcon, _T("%s,0"), szExePath);
				TCHAR strDescribe[100] = _T("101教育PPT课件");
				RegisterFileRelation((TCHAR*)strExt[i].c_str(), szExePath, strAppKey, strDefaultIcon, strDescribe);
			}
		}

		//修复win8 关联 //write root 打开方式
		TCHAR strShellAppKey[] = _T("PPTShell\\Shell\\Open\\Command");        
		bool bWrite = CheckSelfRelation(strShellAppKey);
		if(!bWrite)
		{
			TCHAR szExePath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szExePath, MAX_PATH);
			TCHAR strDefaultIcon[MAX_PATH + 1] = {0};       
			wsprintf(strDefaultIcon, _T("%s,0"), szExePath);
			TCHAR strDescribe[100] = _T("101教育PPT课件");
			RegisterFileRelation(strAppKey, szExePath, strAppKey, strDefaultIcon, strDescribe);
		}
		//}

		//写入URL Protocol
		tstring strWrite=_T("URL Protocol");
		HKEY hKeyTemp = NULL;
		//写入root
		if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, strAppKey, 0, KEY_READ|KEY_SET_VALUE, &hKeyTemp))
		{
			DWORD dwReserved = 0;
			RegSetValueEx(hKeyTemp, strWrite.c_str(), dwReserved, REG_SZ, NULL, 0);
			RegCloseKey(hKeyTemp);
		}	

		//写生字卡免提示选项 outlookfeeds
		TCHAR strUrlAppKey[] = _T("outlookfeeds\\Shell\\Open\\Command");        
		if(!CheckSelfRelation(strUrlAppKey))
		{	
			TCHAR szExePath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szExePath, MAX_PATH);
			RegisterURLProtocol(_T("outlookfeeds"), szExePath);
		}




	}
	catch (...)
	{
	}

	tstring strKey[PPT_REGISTER_KEY_COUNT];

	tstring strOpenWithList[PPT_REGISTER_KEY_COUNT];
	tstring strOpenWithProgids[PPT_REGISTER_KEY_COUNT];

	tstring strUserChoice[PPT_REGISTER_KEY_COUNT];
	tstring strUserChoiceKey[PPT_REGISTER_KEY_COUNT];

	strKey[0] = (_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.pptx\\"));
	strKey[1] = (_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.ppt\\"));
	strKey[2] = (_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.ndpx\\"));

	for ( int i = 0; i < PPT_REGISTER_KEY_COUNT ; i++)
	{
		strOpenWithList[i]		= _T("OpenWithList");
		strOpenWithProgids[i]	= _T("OpenWithProgids");

		strUserChoice[i]		= _T("UserChoice");
		strUserChoiceKey[i]		= _T("Progid");
	}


	tstring strPPTShell = _T("Software\\Classes\\Applications\\PPTShell\\shell\\open\\command");

	TCHAR szExePath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szExePath, MAX_PATH);

	tstring strUserChoiceValue = _T("Applications\\PPTShell");

	tstring strCommand = _T("\"");
	strCommand += szExePath;
	strCommand += _T("\" \"%1\"");

	HKEY hKey = NULL;
	TCHAR szValue[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	DWORD dwType = REG_SZ;
	//注册PPTSHELL路径
	DWORD dwReserved = 0;
	//注册DLL
	if ( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strPPTShell.c_str(), 0, KEY_READ|KEY_SET_VALUE, &hKey))
	{
		RegCreateKey(HKEY_CURRENT_USER, strPPTShell.c_str(), &hKey);
		RegSetValueEx(hKey, NULL, dwReserved, REG_SZ, (BYTE*)strCommand.c_str(), _tcslen(strCommand.c_str()));
	}
	else
	{
		RegQueryValueEx(hKey, NULL, NULL, &dwType, (BYTE*)szValue, &dwSize);
		if ( (_tcsicmp(strCommand.c_str(), szValue) != 0) || (_tcslen(szValue) == 0) )
		{
			RegSetValueEx(hKey, NULL, dwReserved, REG_SZ, (BYTE*)strCommand.c_str(), _tcslen(strCommand.c_str()));
		}
	}

	//注册PPT打开方式
	for(int i = 0; i < 2; i++)
	{
		DWORD dwReserved = 0;
		tstring strDelKey =	strKey[i] + strOpenWithList[i];
		RegDeleteKey(HKEY_CURRENT_USER, strDelKey.c_str()); //直接删掉重新写入。 使用系统选择打开方式后，不能改值，只能删掉重写
		strDelKey =	strKey[i] + strOpenWithProgids[i];
		RegDeleteKey(HKEY_CURRENT_USER, strDelKey.c_str()); 
		strDelKey =	strKey[i] + strUserChoice[i];	//关联只需要写入这个值就可以了
		RegDeleteKey(HKEY_CURRENT_USER, strDelKey.c_str()); 
		//注册DLL
		if ( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strDelKey.c_str(), 0, KEY_READ|KEY_SET_VALUE, &hKey))  
		{
			RegCreateKey(HKEY_CURRENT_USER, strDelKey.c_str(), &hKey);
			RegSetValueEx(hKey, _T("Progid"), dwReserved, REG_SZ, (BYTE*)strUserChoiceValue.c_str() , _tcslen(strUserChoiceValue.c_str()));
		}
		else
		{
			RegQueryValueEx(hKey, strUserChoiceKey[i].c_str(), NULL, &dwType, (BYTE*)szValue, &dwSize);
			if ( (_tcsicmp(strUserChoiceValue.c_str(), szValue) != 0) || (_tcslen(szValue) == 0) )
			{
				RegSetValueEx(hKey, _T("Progid"), dwReserved, REG_SZ, (BYTE*)strUserChoiceValue.c_str(), _tcslen(strUserChoiceValue.c_str()));
			}
		}

		RegCloseKey(hKey);
	}

	//WritePrivateProfileString("APP", "Reg", "1", m_strAppPath+INI_NAME);
}


BOOL COptionsDialogUI::CheckSelfRelation(const TCHAR *strAppKey)
{
	int nRet = FALSE;
	HKEY hExtKey;
	TCHAR szPath[_MAX_PATH] = {0};
	TCHAR szSelfPath[_MAX_PATH] = {0};
	GetModuleFileName(NULL, szSelfPath, MAX_PATH);
	DWORD dwSize = sizeof(szPath);
	if(RegOpenKey(HKEY_CLASSES_ROOT, strAppKey, &hExtKey) == ERROR_SUCCESS)
	{

		RegQueryValueEx(hExtKey, NULL, NULL, NULL, (LPBYTE)szPath, &dwSize);


		if(_tcsstr(szPath , szSelfPath) != NULL)
		{
			nRet=TRUE;
		}

		RegCloseKey(hExtKey);
		return nRet;
	}
	return nRet;
}

BOOL COptionsDialogUI::CheckFileRelation(const TCHAR *strExt, const TCHAR *strAppKey)
{
	int nRet = FALSE;
	HKEY hExtKey;
	TCHAR szPath[_MAX_PATH] = {0};
	DWORD dwSize = sizeof(szPath);

	if(RegOpenKey(HKEY_CLASSES_ROOT, strExt, &hExtKey) == ERROR_SUCCESS)
	{

		RegQueryValueEx(hExtKey, NULL, NULL, NULL, (LPBYTE)szPath, &dwSize);
		

		if(_tcsicmp(szPath, strAppKey) == 0)
		{
			nRet=TRUE;
		}

		RegCloseKey(hExtKey);
		return nRet;
	}
	return nRet;
}

void COptionsDialogUI::RegisterURLProtocol(TCHAR *pszProtocal, TCHAR *pszExePath)
{
	TCHAR strTemp[_MAX_PATH]={0};
	HKEY hKey;
	RegCreateKey(HKEY_CLASSES_ROOT,pszProtocal,&hKey);
	RegSetValueEx(hKey,_T("URL Protocol"), 0, REG_SZ, NULL, 0);
	RegCloseKey(hKey);   
	wsprintf(strTemp, _T("%s\\Shell"), pszProtocal);
	RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);
	RegSetValue(hKey,_T(""),REG_SZ, _T("Open"),_tcslen(_T("Open"))+1);
	RegCloseKey(hKey);
	wsprintf(strTemp,_T("%s\\Shell\\Open\\Command"), pszProtocal);

	RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);
	wsprintf(strTemp,_T("\"%s\" \"%%1\""), pszExePath);
	RegSetValue(hKey,_T(""), REG_SZ, strTemp, _tcslen(strTemp)+1);

	RegCloseKey(hKey);
}


void COptionsDialogUI::RegisterFileRelation(TCHAR *strExt, TCHAR *strAppName, TCHAR *strAppKey, TCHAR *strDefaultIcon, TCHAR *strDescribe)
{
	TCHAR strTemp[_MAX_PATH];

	HKEY hKey;

	RegCreateKey(HKEY_CLASSES_ROOT,strExt,&hKey);

	RegSetValue(hKey, _T(""), REG_SZ, strAppKey, _tcslen(strAppKey)+1);

	RegCloseKey(hKey);   

	RegCreateKey(HKEY_CLASSES_ROOT, strAppKey, &hKey);

	RegSetValue(hKey, _T(""), REG_SZ, strDescribe,_tcslen(strDescribe)+1);

	RegCloseKey(hKey);

	wsprintf(strTemp, _T("%s\\DefaultIcon"), strAppKey);

	RegCreateKey(HKEY_CLASSES_ROOT, strTemp, &hKey);

	RegSetValue(hKey, _T(""), REG_SZ, strDefaultIcon, _tcslen(strDefaultIcon)+1);

	RegCloseKey(hKey);

	wsprintf(strTemp, _T("%s\\Shell"), strAppKey);

	RegCreateKey(HKEY_CLASSES_ROOT, strTemp, &hKey);

	RegSetValue(hKey, _T(""), REG_SZ, _T("Open"), _tcslen(_T("Open"))+1);

	RegCloseKey(hKey);

	wsprintf(strTemp, _T("%s\\Shell\\Open\\Command"), strAppKey);

	RegCreateKey(HKEY_CLASSES_ROOT, strTemp, &hKey);

	wsprintf(strTemp, _T("\"%s\" \"%%1\""), strAppName);

	RegSetValue(hKey, _T(""), REG_SZ, strTemp, _tcslen(strTemp)+1);

	RegCloseKey(hKey);

}


void COptionsDialogUI::ResetOpenMode()
{
	HKEY hKey;

	RegCreateKey(HKEY_CLASSES_ROOT, _T(".pptx"), &hKey);
	RegSetValue(hKey, _T(""), REG_SZ, _T("POWERPNT.EXE"), _tcslen(_T("POWERPNT.EXE"))+1);
	RegCloseKey(hKey);

	RegCreateKey(HKEY_CLASSES_ROOT, _T(".ppt"), &hKey);
	RegSetValue(hKey, _T(""), REG_SZ, _T("POWERPNT.EXE"),  _tcslen(_T("POWERPNT.EXE"))+1);
	RegCloseKey(hKey);  

	tstring strKey[PPT_RESET_KEY_COUNT];

	tstring strOpenWithList[PPT_RESET_KEY_COUNT];
	tstring strOpenWithProgids[PPT_RESET_KEY_COUNT];

	tstring strUserChoice[PPT_RESET_KEY_COUNT];
	tstring strUserChoiceKey[PPT_RESET_KEY_COUNT];

	strKey[0] = (_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.pptx\\"));
	strKey[1] = (_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.ppt\\"));

	for ( int i = 0; i < PPT_RESET_KEY_COUNT ; i++)
	{
		strOpenWithList[i]		= _T("OpenWithList");
		strOpenWithProgids[i]	= _T("OpenWithProgids");

		strUserChoice[i]		= _T("UserChoice");
		strUserChoiceKey[i]		= _T("Progid");
	}

	tstring strUserChoiceValue = _T("Applications\\POWERPNT.EXE");

	TCHAR szValue[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	DWORD dwType = REG_SZ;

	//注册PPT打开方式
	for(int i = 0; i < 2; i++)
	{
		DWORD dwReserved = 0;
		tstring strDelKey =	strKey[i] + strOpenWithList[i];
		RegDeleteKey(HKEY_CURRENT_USER, strDelKey.c_str()); //直接删掉重新写入。 使用系统选择打开方式后，不能改值，只能删掉重写
		strDelKey =	strKey[i] + strOpenWithProgids[i];
		RegDeleteKey(HKEY_CURRENT_USER, strDelKey.c_str()); 
		strDelKey =	strKey[i] + strUserChoice[i];	//关联只需要写入这个值就可以了
		RegDeleteKey(HKEY_CURRENT_USER, strDelKey.c_str()); 
		//注册DLL
		if ( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strDelKey.c_str(), 0, KEY_READ|KEY_SET_VALUE, &hKey))  
		{
			RegCreateKey(HKEY_CURRENT_USER, strDelKey.c_str(), &hKey);
			RegSetValueEx(hKey, _T("Progid"), dwReserved, REG_SZ, (BYTE*)strUserChoiceValue.c_str(), _tcslen(strUserChoiceValue.c_str()));
		}
		else
		{
			RegQueryValueEx(hKey, strUserChoiceKey[i].c_str(), NULL, &dwType, (BYTE*)szValue, &dwSize);
			if ( (_tcsicmp(strUserChoiceValue.c_str(), szValue) != 0) || (_tcslen(szValue) == 0) )
			{
				RegSetValueEx(hKey, _T("Progid"), dwReserved, REG_SZ, (BYTE*)strUserChoiceValue.c_str(), _tcslen(strUserChoiceValue.c_str()));
			}
		}

		RegCloseKey(hKey);
	}
}


void COptionsDialogUI::OnBtnChangePathClick(TNotifyUI& msg)
{
	TCHAR pszPath[MAX_PATH];  
	BROWSEINFO bi;   
	bi.hwndOwner      = this->GetHWND();  
	bi.pidlRoot       = NULL;  
	bi.pszDisplayName = NULL;   
	bi.lpszTitle      = TEXT("请选择下载文件夹");   
	bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;  
	bi.lpfn           = NULL;   
	bi.lParam         = 0;  
	bi.iImage         = 0;   

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);  
	if (pidl == NULL)  
	{  
		return;  
	}  

	if (SHGetPathFromIDList(pidl, pszPath))  
	{  
		CRichEditUI* pEditSavePath = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editPath")));
		CString strPath = pszPath;
		if (CheckDirectory(strPath))
		{
			int size =  _tcslen(pszPath);
			if (_tcscmp(&pszPath[size - 1], _T("\\")) == 0)
			{
				pszPath[size - 1] = _T('\0');
			}
			
			m_SavePath = pszPath;
			_tcscat(pszPath, _T("\\NDCloud")); 
			pEditSavePath->SetText(pszPath);
		}
		else
		{
			CToast::GetInstance(this->GetHWND())->Show(_T("下载目录不合法"));
		}

	}
}

void COptionsDialogUI::OnBtnOpenFolderClick(TNotifyUI& msg)
{
	CRichEditUI* pEditSavePath = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editPath")));
	CDuiString savePath = pEditSavePath->GetText();

	BOOL ret = IsDirectoryExists(savePath.GetData());
	if (ret)
	{
		ShellExecute(NULL,NULL,_T("explorer"),savePath,NULL,SW_SHOW);
	}
	else
	{
		ShellExecute(NULL,NULL,_T("explorer"),m_SavePath.c_str(),NULL,SW_SHOW);
	}

}

void COptionsDialogUI::OnEditPathKillfocus(TNotifyUI& msg)
{
	CRichEditUI* pEditSavePath = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editPath")));
	CString editPath = pEditSavePath->GetText().GetData();
	if (editPath.Right(8) == "\\NDCloud")
	{
		//int lastCh = editPath.ReverseFind(_T('\\'));
		//editPath = editPath.Left(lastCh);
		if(!CheckDirectory(editPath))
		{
			CToast::GetInstance(this->GetHWND())->Show(_T("下载目录不合法"));
			pEditSavePath->SetText(_T(""));
		}
	}
	else
	{
		if(!CheckDirectory(editPath))
		{
			CToast::GetInstance(this->GetHWND())->Show(_T("下载目录不合法"));
			pEditSavePath->SetText(_T(""));
		}
		else
		{
			//int lastCh = editPath.ReverseFind(_T('\\'));
			//if (lastCh > 0)
			//{
			//	editPath = editPath.Left(lastCh);
			//}
			if (editPath.Right(1) == "\\")
			{
				editPath += _T("NDCloud");
			}
			else
			{
				editPath += _T("\\NDCloud");
			}
			pEditSavePath->SetText(editPath);
		}
	}

}

bool COptionsDialogUI::CheckDirectory(CString& path)
{
	////  /,\,\\,// 全都转化为 \ 标识符
	path.Replace(_T("\/"), _T("\\"));
	int ret;
	do 
	{
		ret = path.Replace(_T("\\\\"), _T("\\"));
	} while (ret != 0);
	

	//路劲要 字母：开头
	if (!isalpha(path.GetAt(0)) || path.GetAt(1) != _T(':') || path.GetAt(2) != _T('\\'))
	{
		return FALSE;
	}

	// 盘符必需为固定磁盘
	if( GetDriveType(path.Left(2)) != DRIVE_FIXED )
	{
		return FALSE;
	}

	CString strRight = path.Right(path.GetLength() - 3);
	if (strRight.Find(':') != -1 || strRight.Find('*') != -1 ||
		strRight.Find('?') != -1 || strRight.Find('<') != -1 || 
		strRight.Find('>') != -1 || strRight.Find('|') != -1 ||
		strRight.Find('"') != -1)
	{
		return FALSE;
	}

	return TRUE;
	//if (IsDirectoryExists(path))
	//{
	//	CLabelUI* pErrorMsg = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg")));
	//	pErrorMsg->SetVisible(FALSE);
	//	CButtonUI* pBtnOK = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOK")));
	//	pBtnOK->SetEnabled(TRUE);

	//	return TRUE;
	//}
	//else
	//{
	//	CLabelUI* pErrorMsg = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg")));
	//	pErrorMsg->SetVisible(TRUE);
	//	CButtonUI* pBtnOK = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOK")));
	//	pBtnOK->SetEnabled(FALSE);

	//	return FALSE;
	//}
}

bool COptionsDialogUI::IsDirectoryExists(CString const& path)
{ 
	//判断是否存在
	if(!PathFileExists(path))
		return false;

	//判断是否为目录
	DWORD attributes = ::GetFileAttributes(path);  
	attributes &= FILE_ATTRIBUTE_DIRECTORY;
	return attributes == FILE_ATTRIBUTE_DIRECTORY;
}

void COptionsDialogUI::OnBtnClearTempClick(TNotifyUI& msg)
{
	CRect rect;
	::GetWindowRect(this->GetHWND(), &rect);
	CClearMask* pClearMask = new CClearMask();
	pClearMask->Create(this->GetHWND(), _T("ClearMask"), WS_POPUP, 0, 0, 0, 0, 0);
	pClearMask->CenterWindow();
	pClearMask->ShowWindow();

	tstring localPath = GetLocalPath();
	tstring folderCache = localPath + _T("\\Cache");
	tstring folderlog = localPath + _T("\\log");
	DeleteDirectory((LPTSTR)folderCache.c_str());
	DeleteDirectory((LPTSTR)folderlog.c_str());

	pClearMask->CloseMask();
	CToast::GetInstance(this->GetHWND())->Show(_T("已成功清除缓存"));
}

BOOL COptionsDialogUI::DeleteDirectory(TCHAR* psDirName)   
{   
	CFileFind tempFind;   
	TCHAR sTempFileFind[ _MAX_PATH ] = { 0 };   
	wsprintf(sTempFileFind, _T("%s//*.*"), psDirName);   
	BOOL IsFinded = tempFind.FindFile(sTempFileFind);   
	while (IsFinded)   
	{   
		IsFinded = tempFind.FindNextFile();   
		if (!tempFind.IsDots())  
		{   
			TCHAR sFoundFileName[ _MAX_PATH ] = { 0 };   
			_tcscpy(sFoundFileName, tempFind.GetFileName().GetBuffer(200));   
			if (tempFind.IsDirectory())   
			{   
				TCHAR sTempDir[ _MAX_PATH ] = { 0 };   
				wsprintf(sTempDir, _T("%s//%s"), psDirName, sFoundFileName);   
				DeleteDirectory(sTempDir);   
			}   
			else   
			{   
				TCHAR sTempFileName[ _MAX_PATH ] = { 0 };  
				wsprintf(sTempFileName, _T("%s//%s"), psDirName, sFoundFileName);   
				DeleteFile(sTempFileName);   
			}   
		}   
	}   
	tempFind.Close();   
/*	if(!RemoveDirectory(psDirName))   
	{   
		return FALSE;   
	} */  
	return TRUE;   
}  

void COptionsDialogUI::OnEditPasswordSetfocus( TNotifyUI& msg )
{
	CButtonUI* pBtnPasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordTip")));
	pBtnPasswordTip->SetVisible(false);
}

void COptionsDialogUI::OnEditPasswordKillfocus(TNotifyUI& msg)
{
	CDuiString editName = msg.pSender->GetName();
	CRichEditUI* editPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(editName.GetData()));

	if (_tcscmp(editPassword->GetText().GetData(), _T("")) == 0)
	{
		CButtonUI* pBtnPasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordTip")));
		pBtnPasswordTip->SetVisible(true);
	}
	else
	{
	    CButtonUI* pBtnPasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordTip")));
		pBtnPasswordTip->SetVisible(false);
	}
}

void COptionsDialogUI::OnEditRetypePasswordSetfocus(TNotifyUI& msg)
{
	CButtonUI* pBtnRetypePasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("retypePasswordTip")));
	pBtnRetypePasswordTip->SetVisible(false);
}

void COptionsDialogUI::OnEditRetypePasswordKillfocus( TNotifyUI& msg )
{
	CDuiString editName = msg.pSender->GetName();
	CRichEditUI* editPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(editName.GetData()));

	if (_tcscmp(editPassword->GetText().GetData(), _T("")) == 0)
	{
		CButtonUI* pBtnRetypePasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("retypePasswordTip")));
		pBtnRetypePasswordTip->SetVisible(true);
	}
	else
	{
		CButtonUI* pBtnRetypePasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("retypePasswordTip")));
		pBtnRetypePasswordTip->SetVisible(false);
	}
}

void COptionsDialogUI::OnBtnPasswordOkClick(TNotifyUI& msg)
{
	CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
	CRichEditUI* pEditRetypePassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("retypePassword")));
	if (ChkPasswordFormat((LPTSTR)pEditPassword->GetText().GetData()))
	{
		if (pEditPassword->GetText() == pEditRetypePassword->GetText())
		{
			tstring strConfinFile = GetLocalPath();
			strConfinFile += _T("\\Setting\\Config.ini");
			tstring password = pEditPassword->GetText().GetData();
			password = trim(password);
			//密码
			//WritePrivateProfileString(_T("Option"), _T("password"), password.c_str(), strConfinFile.c_str());
			SetPassword((LPTSTR)password.c_str());
			SwitchView(4);
			CToast::GetInstance(this->GetHWND())->Show(_T("设置密码成功"));
		}
		else
		{
			SwitchView(3, _T("两次密码不一致"));
		}
	}
	else
	{
		SwitchView(3, _T("密码只能包含字母和数字"));
	}

}

void COptionsDialogUI::OnChkMakePasswordChanged(TNotifyUI& msg)
{
	CString password = GetPassword();
	CCheckBoxUI* pChkMakePassword = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("chkMakePassword")));
	if (pChkMakePassword->IsSelected())
	{
		if (password.IsEmpty())
		{
			SwitchView(2);
		}
		else
		{
			SwitchView(4);
		}

	}
	else
	{

		if (!password.IsEmpty())
		{
			CPasswordLoginDialogUI* pPasswordLoginDialog = new CPasswordLoginDialogUI();
			pPasswordLoginDialog->SetTitleWords(_T("请输入密码完成取消密码设置"));
			pPasswordLoginDialog->Create(this->GetHWND(), _T("PasswordLoginDialog"), WS_POPUP, 0, 0, 0, 0, 0);
			pPasswordLoginDialog->CenterWindow();
			UINT ret = pPasswordLoginDialog->ShowModal();
			if (ret == IDCANCEL)
			{
				pChkMakePassword->SetCheck(true);
				return;
			}
		}
		SwitchView(1);
		SetPassword(_T(""));
	}
}

void COptionsDialogUI::SwitchView(int status, LPTSTR tip)
{
	CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
	CRichEditUI* pEditRetypePassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("retypePassword")));
	CHorizontalLayoutUI* pEditPasswordLayout = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("passwordLayout")));
	CHorizontalLayoutUI* pEditRetypePasswordLayout = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("retypePasswordLayout")));
	CButtonUI* pBtnPasswordOk = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordOk")));
	CButtonUI* pBtnpasswordModify = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordModify")));
	CLabelUI* pError = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg")));

	if (status == 1)	//checkbox未设置密码
	{
		pEditPassword->SetVisible(false);
		pEditPasswordLayout->SetVisible(false);
		pEditRetypePassword->SetVisible(false);
		pEditRetypePasswordLayout->SetVisible(false);
		pBtnPasswordOk->SetVisible(false);
		pBtnpasswordModify->SetVisible(false);
		pError->SetVisible(false);

		CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
		CRichEditUI* pRetypePassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("retypePassword")));
		pEditPassword->SetText(_T(""));
		pRetypePassword->SetText(_T(""));
	}
	else if (status == 2) //checkbox设置密码
	{
		pEditPassword->SetVisible(true);
		pEditPassword->SetEnabled(true);
		pEditPasswordLayout->SetVisible(true);
		pEditPassword->SetFocus();
		pEditRetypePassword->SetVisible(true);
		pEditRetypePasswordLayout->SetVisible(true);
		pBtnPasswordOk->SetVisible(true);
		pBtnpasswordModify->SetVisible(false);
		pError->SetVisible(false);
	}
	else if(status == 3) //两次输入密码不一致
	{
		pError->SetText(tip);
		pError->SetVisible(true);
	}
	else if (status == 4) //设置密码成功
	{
		pEditPassword->SetVisible(true);
		pEditPassword->SetEnabled(false);
		pEditPasswordLayout->SetVisible(true);
		pEditRetypePassword->SetVisible(false);
		pEditRetypePasswordLayout->SetVisible(false);
		pBtnPasswordOk->SetVisible(false);
		pBtnpasswordModify->SetVisible(true);
		pError->SetVisible(false);

		CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
		CRichEditUI* pRetypePassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("retypePassword")));
		pEditPassword->SetText(GetPassword());
		pRetypePassword->SetText(GetPassword());

		CButtonUI* pBtnRetypePasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("retypePasswordTip")));
		pBtnRetypePasswordTip->SetVisible(false);
		CButtonUI* pBtnPasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordTip")));
		pBtnPasswordTip->SetVisible(false);
	}
}

void COptionsDialogUI::OnBtnPasswordModifyClick(TNotifyUI& msg)
{
	CModifyPasswordDialogUI* pModifyPasswordDialog = new CModifyPasswordDialogUI();
	pModifyPasswordDialog->Create(this->GetHWND(), _T("ModifyPasswordDialog"), WS_POPUP, 0, 0, 0, 0, 0);
	pModifyPasswordDialog->CenterWindow();
	pModifyPasswordDialog->ShowModal();

	//tstring strConfinFile = GetLocalPath();
	//strConfinFile += _T("\\Setting\\Config.ini");
	//TCHAR password[MAX_PATH + 1] = {0};
	//GetPrivateProfileString(_T("Option"), _T("password"), _T(""), password, MAX_PATH, strConfinFile.c_str());

	CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
	pEditPassword->SetText(GetPassword());
}

CString COptionsDialogUI::GetPassword()
{
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");
	TCHAR password[MAX_PATH + 1] = {0};
	GetPrivateProfileString(_T("Option"), _T("password"), _T(""), password, MAX_PATH, strConfinFile.c_str());

	CString strPassword = Decrypt(password, (WORD)100);

	return strPassword;
}

void COptionsDialogUI::SetPassword(LPTSTR password)
{
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");
	
	tstring strPassword = Encrypt(password, (WORD)100);

	WritePrivateProfileString(_T("Option"), _T("password"), strPassword.c_str(), strConfinFile.c_str());
	
	return;
}

void COptionsDialogUI::OnBtnPasswordTipClick( TNotifyUI& msg )
{
	CButtonUI* pBtnPasswordtip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("passwordTip")));
	pBtnPasswordtip->SetVisible(false);

	CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
	pEditPassword->SetFocus();
}

void COptionsDialogUI::OnBtnRetypePasswordTipClick( TNotifyUI& msg )
{
	CButtonUI* pBtnRetypePasswordTip = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("retypePasswordTip")));
	pBtnRetypePasswordTip->SetVisible(false);

	CRichEditUI* pRetypePassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("retypePassword")));
	pRetypePassword->SetFocus();
}

bool COptionsDialogUI::ChkPasswordFormat(LPTSTR password)
{
	int size = _tcslen(password);
	for(int pos = 0; pos < size; pos++)
	{
		if ((password[pos] <= _T('Z') && password[pos] >= _T('A')) || (password[pos] <= _T('z') && password[pos] >= _T('a')) || (password[pos] <= _T('9') && password[pos] >= _T('0')))
		{
			continue;
		}
		else
		{
			return false;
		}
		
	}

	return true;
}

void COptionsDialogUI::CheckXmlExist()
{
	TiXmlDocument docModuleVisible;
	tstring strPath	= GetLocalPath();
	tstring m_strModuleVisibelXmlPath = strPath + _T("\\Skins\\Config\\ModuleVisible.xml");
	bool bDocModuleExist = docModuleVisible.LoadFile(Str2Ansi(m_strModuleVisibelXmlPath).c_str());
	if (!bDocModuleExist)
	{
		TiXmlDeclaration *dec = new TiXmlDeclaration("1.0","utf-8","yes");
		TiXmlElement *config = new TiXmlElement("Config");  
		docModuleVisible.LinkEndChild(dec);  
		docModuleVisible.LinkEndChild(config);  

		TiXmlElement *moduleLists = new TiXmlElement("ModuleLists");  
		config->LinkEndChild(moduleLists);  
		
		TiXmlElement *module_ND_ICRPlay = new TiXmlElement("Module_ND_ICRPlay");  
		moduleLists->LinkEndChild(module_ND_ICRPlay);

		TiXmlElement *visible = new TiXmlElement("Visible");  
		module_ND_ICRPlay->LinkEndChild(visible);

		tstring strVisible = _T("false");  
		TiXmlText *pStrVisible=new TiXmlText(Str2Ansi(strVisible).c_str());  
		visible->LinkEndChild(pStrVisible);  

		docModuleVisible.SaveFile(Str2Ansi(m_strModuleVisibelXmlPath).c_str());  
	}
}

#define MAX_HARD_DISK_COUNT		24
static char g_HardDiskLetters[MAX_HARD_DISK_COUNT][4] = 
{
	"c:\\",	"d:\\",	"e:\\",	"f:\\",	"g:\\",	"h:\\",
	"i:\\",	"j:\\",	"k:\\",	"l:\\",	"m:\\",	"n:\\",
	"o:\\",	"p:\\",	"q:\\",	"r:\\",	"s:\\",	"t:\\",
	"u:\\",	"v:\\",	"w:\\",	"x:\\",	"y:\\",	"z:\\"
};

tstring COptionsDialogUI::GetDefaultDirectory() //得到下载地址的默认目录
{
	tstring strLocalPath = GetLocalPath();
	strLocalPath += _T("\\Setting\\Config.ini");
	
	tstring strNDCloudDrive;

	// os drive
	char szSystemPath[MAX_PATH];
	GetSystemDirectoryA(szSystemPath, MAX_PATH);
	_strlwr_s(szSystemPath, MAX_PATH);

	//
	ULARGE_INTEGER MaxFreeSize;
	MaxFreeSize.QuadPart = 0;


	ULARGE_INTEGER AvailableToCaller, Disk, Free;
	char szTemp[MAX_PATH] = {0};

	for( int i = 0; i < MAX_HARD_DISK_COUNT; i++ )
	{
		if ( GetDriveTypeA(g_HardDiskLetters[i]) == DRIVE_FIXED )
		{
			if (GetDiskFreeSpaceExA(g_HardDiskLetters[i], &AvailableToCaller, &Disk, &Free))
			{
				if( Free.QuadPart > MaxFreeSize.QuadPart )
				{
					if( strstr(szSystemPath, g_HardDiskLetters[i]) == NULL )
					{
						MaxFreeSize = Free;
						strNDCloudDrive = Ansi2Str(g_HardDiskLetters[i]);
						strNDCloudDrive = strNDCloudDrive.substr(0, 2);
						strNDCloudDrive[0] = toupper(strNDCloudDrive[0]);
					}
				}
			}

		}
	}

	//
	if( strNDCloudDrive == _T("") )
		strNDCloudDrive = GetLocalPath();

	WritePrivateProfileString(_T("Config"), _T("NDCloudPath"), strNDCloudDrive.c_str(), strLocalPath.c_str());
	return strNDCloudDrive;


}