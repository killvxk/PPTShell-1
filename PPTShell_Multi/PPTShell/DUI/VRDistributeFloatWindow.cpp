#include "StdAfx.h"
#include "VRDistributeFloatWindow.h"
#include "Util/Util.h"

CVRDistributeFloatWindow::CVRDistributeFloatWindow(void)
{
	m_strApkPath		= _T("");
	m_strApkPackageName = _T("");
	m_strApkMainActivity= _T("");
	m_strPackageVer		= _T("");
}

CVRDistributeFloatWindow::~CVRDistributeFloatWindow(void)
{
}

LPCTSTR CVRDistributeFloatWindow::GetWindowClassName() const
{
	return _T("VRDistribute");
}

CDuiString CVRDistributeFloatWindow::GetSkinFile()
{
	return _T("VRDistribute\\VRDistributeFloatWindow.xml");
}

CDuiString CVRDistributeFloatWindow::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CVRDistributeFloatWindow::InitWindow()
{

}

bool CVRDistributeFloatWindow::GetVRPackageInfo( tstring strPath )
{
	vector<tstring>	vecInfo;
	vecInfo = SplitString(strPath, strPath.length(), _T('|'), true);

	if (vecInfo.size() > 4 )
	{
		m_strApkPath		= vecInfo[0];
		m_strApkPackageName = vecInfo[1];
		m_strApkMainActivity= vecInfo[2];
		m_strPackageVer		= vecInfo[3];

		int nPos		= m_strApkPath.rfind(_T('\\'));
		m_strApkPath	= m_strApkPath.substr(0, nPos+1);
		tstring strSearchPath = m_strApkPath;

		strSearchPath += _T("*.apk");
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
			m_strApkPath += FindFileData.cFileName;
		else
			m_strApkPath = _T("");

		return true;
	}

	m_strApkPath = _T("");

	return false;
}

void CVRDistributeFloatWindow::OnVRDistributeBtn( TNotifyUI& msg )
{
	if ( m_strApkPath.empty() ) 
	{
		CToast::Toast(_STR_VR_PACKAGE_NOT_FIND, true);
	}
	else
	{
		tstring strExePath = GetLocalPath();

		tstring strWorkDir = strExePath + _T("\\bin\\plugins\\icr\\");

		strExePath += _T("\\bin\\plugins\\icr\\DispatchAssist.exe");

		TCHAR szCmd[MAX_PATH*2] = {0};
		wsprintf(szCmd, _T("--pkgname=%s --pkgver=%s --apkpath=\"%s\""), m_strApkPackageName.c_str(), m_strPackageVer.c_str(), m_strApkPath.c_str());

		ShellExecute(NULL, _T("open"), strExePath.c_str(), szCmd, strWorkDir.c_str(), SW_SHOW);

	}
}
