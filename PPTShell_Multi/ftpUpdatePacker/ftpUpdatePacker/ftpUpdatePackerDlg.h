
// ftpUpdatePackerDlg.h : 头文件
//

#pragma once

#include <WinInet.h>
#include "afxcmn.h"



typedef struct _VersionStruct
{
	int aVersion[4];
	_VersionStruct()
	{
		for (int i = 0; i < 4; i++)
		{
			aVersion[i] = 0;
		}
	}
	int CompareTo(_VersionStruct* pVersion)
	{
		for (int i = 4 - 1; i >= 0; i--)
		{
			if (this->aVersion[ i ] > pVersion->aVersion[ i ])
			{
				return 1;
			}
			else if (this->aVersion[ i ] == pVersion->aVersion[ i ])
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		return 0;
	}

	void	InitFromString(LPCTSTR	lpStr)
	{
		sscanf_s(lpStr, "%d.%d.%d.%d", &aVersion[3], &aVersion[2], &aVersion[1], &aVersion[0]);
	}

}VersionStruct;

class classcomp           // define compare class, as method 1
{
public:
	bool operator () (CString  obj1, CString  obj2) const   // the class must has a operator function.
	{
		VersionStruct a;
		VersionStruct b;

		a.InitFromString(obj1.GetString());
		b.InitFromString(obj2.GetString());

		return a.CompareTo(&b) >= 0 ? false : true;

		bool bret=false;
		CString str1;
		CString str2;
		CString str3;
		CString str4;

		AfxExtractSubString(str1,obj1,0,'.');
		AfxExtractSubString(str2,obj1,1,'.');
		AfxExtractSubString(str3,obj1,2,'.'); 
		AfxExtractSubString(str4,obj1,3,'.');

		CString str11;
		CString str22;
		CString str33;
		CString str44;

		AfxExtractSubString(str11,obj2,0,'.');
		AfxExtractSubString(str22,obj2,1,'.');
		AfxExtractSubString(str33,obj2,2,'.'); 
		AfxExtractSubString(str44,obj2,3,'.');

		do 
		{
			if(atoi(str1)>atoi(str11))
			{
				break;
			}
			if(atoi(str2)>atoi(str22))
			{
				break;
			}
			if(atoi(str3)>atoi(str33))
			{
				break;
			}
			if(atoi(str4)>atoi(str44))
			{
				break;
			}
			bret=true;

		} while (false);

		return bret;// obj2>obj1;
	}
};



typedef struct _CollectStruct
{
	CString			strVersion;
	VersionStruct	stVersion;
	list<CString>	listFile;
	CString			strPackPath;

}CollectStruct;


// CftpUpdatePackerDlg 对话框
class CftpUpdatePackerDlg : public CDialog
{
// 构造
public:
	CftpUpdatePackerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FTPUPDATEPACKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	CString			GetReleativePath(CString* pStrSrc);

	CString			m_strVerRootDir;
	CString			m_strPackageDir;
	CString			m_strConfigDir;
	CString         m_strHistroyDir;
	CString			m_strExecDir;
	CString			m_strUpdateTextPath;
	CString			m_strGetUpdateFilePath;


	DWORD	GetCrc(LPCSTR lpBuffer, DWORD dwSize);
	int		CompareFile(CString& pathA, CString& pathB);
	void	VertifyDir(CString& strDir);
	void	VertifyLinuxFtpDir(CString& strDir);
	void	CollectDir();
	void	CompareCollect();
	void	CompareCollect(CString& strCompare,  CollectStruct& maxCollect, CollectStruct& collect);
	void	MakePackage();
	void	ArchiveDirPackage(CString& strDir, CString& strPath);
	bool	ReleaseVersion();
	bool	SaveVersionDat();
	bool	CreateVersionDat();
	bool	IsPortableExecutable(CString& pathA);
	int		CompareCodeSection(CString& pathA, CString& pathB);
	DWORD	rva2Offset(DWORD Rva, DWORD dwSectionCount, PIMAGE_SECTION_HEADER pSections);
	PIMAGE_SECTION_HEADER findCodeSection(PIMAGE_SECTION_HEADER pFirstSection, int nSectionCount, DWORD dwAddressOfEntry);

	bool    MakeFileCrcList();
	bool    AutoSignLastVersion();
	bool	AutoSignFile(LPTSTR szSignFile);

	map<CString, CollectStruct, classcomp>	m_mapCollect;
	map<CString, CollectStruct, classcomp>	m_mapBak;
	CollectStruct				m_maxCollect;
	CListBox *					m_pList;
	CString m_strHttpHost;
	
	CString m_strAccount;
	CString m_strPassword;
	CString m_strFtpHost;

	HINTERNET myGetFtpConnection(HINTERNET & hInet);
	CString m_strFtpPath;
	CString m_strContext;
	CString	m_strVersionPath;
	CString	m_strNewPackageName;
	TCHAR m_strAppPath[MAX_PATH];

	static DWORD WINAPI OpThread(LPARAM lParam);
	void GetAllHash();
	bool m_bHashThreadResult;
	bool m_bProxy;
// 实现
protected:
	HICON m_hIcon;
	//配置管理
	void SaveConfig();
	void ReadConfig();

	static	void	GenerateThread(void* pParam);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	CProgressCtrl m_strProCtrl;
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnEnChangeEditUser();
	afx_msg void OnBnClickedButtonOpenConf();
	afx_msg void OnBnClickedButtonOpenVer();
	afx_msg void OnBnClickedButtonOpenGen();
	afx_msg void OnBnClickedButtonShowLog();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnStnClickedStaticVersion();
};
