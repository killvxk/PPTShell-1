
// ftpUpdatePackerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ftpUpdatePacker.h"
#include "ftpUpdatePackerDlg.h"

#include "coresrc\\FileOperateion.h"

#pragma  comment(lib, "Wininet.lib")
#include "coresrc\\zip.h"
#include "coresrc\\http.h"
#include "coresrc\\SuperFastHash.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CftpUpdatePackerDlg 对话框




CftpUpdatePackerDlg::CftpUpdatePackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CftpUpdatePackerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//获取路径
	GetModuleFileName(0,m_strAppPath, sizeof(m_strAppPath));
	LPTSTR pAppPath=_tcsrchr(m_strAppPath, TEXT('\\'));
	if(pAppPath)
	{
		*(pAppPath+1)=TEXT('\0');
	}
	m_bProxy=false;
}

void CftpUpdatePackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_strProCtrl);
}

BEGIN_MESSAGE_MAP(CftpUpdatePackerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CftpUpdatePackerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CftpUpdatePackerDlg::OnBnClickedButtonTest)
	ON_EN_CHANGE(IDC_EDIT_USER, &CftpUpdatePackerDlg::OnEnChangeEditUser)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CONF, &CftpUpdatePackerDlg::OnBnClickedButtonOpenConf)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_VER, &CftpUpdatePackerDlg::OnBnClickedButtonOpenVer)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_GEN, &CftpUpdatePackerDlg::OnBnClickedButtonOpenGen)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_LOG, &CftpUpdatePackerDlg::OnBnClickedButtonShowLog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CftpUpdatePackerDlg::OnBnClickedButtonSave)
	ON_STN_CLICKED(IDC_STATIC_VERSION, &CftpUpdatePackerDlg::OnStnClickedStaticVersion)
END_MESSAGE_MAP()


// CftpUpdatePackerDlg 消息处理程序

BOOL CftpUpdatePackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pList=(CListBox *)GetDlgItem(IDC_LIST1);

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标



	TCHAR szPath[MAX_PATH]	= {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);

	TCHAR* pFinded	= _tcsrchr(szPath, _T('\\'));
	*pFinded		= 0x0;

	m_strExecDir = szPath;

	m_strVerRootDir = m_strExecDir + _T("\\VerRoot");
	m_strPackageDir	= m_strExecDir + _T("\\Generate");
	m_strConfigDir = m_strExecDir + _T("\\Config");

	m_strHistroyDir = m_strExecDir + _T("\\历史版本");

	CFileOperateion::CreateDirectory(m_strVerRootDir);
	CFileOperateion::CreateDirectory(m_strPackageDir);
	CFileOperateion::CreateDirectory(m_strConfigDir);
	CFileOperateion::CreateDirectory(m_strHistroyDir);

	VertifyDir(m_strConfigDir);
	VertifyDir(m_strVerRootDir);
	VertifyDir(m_strPackageDir);
	VertifyDir(m_strExecDir);
	VertifyDir(m_strHistroyDir);

	m_strUpdateTextPath		= m_strConfigDir + _T("update.dat");
	m_strGetUpdateFilePath	= m_strConfigDir + _T("update.txt");
	m_strVersionPath		= m_strConfigDir + _T("version.dat");

	//要再这里获取
	ReadConfig();

	GetDlgItemText(IDC_EDIT_FTP_DOMAIN, m_strHttpHost);
	if(m_strHttpHost=="")
	{
		GetDlgItemText(IDC_EDIT_FTP_IP, m_strHttpHost);
	}
	GetDlgItemText(IDC_EDIT_FTP_DIR, m_strFtpPath);


	//获取上次更新记录
	try
	{
		CHttpClient   conn;  
		CString   value;
		value=conn.doGet("http://"+m_strHttpHost+m_strFtpPath+_T("/update.dat"));
		value+="\r";
		value+="（这是上个版本的升级日志，请填写最新版本更新内容！！！）";
		SetDlgItemText(IDC_EDIT_UPDATETXT,value);

		value=conn.doGet("http://"+m_strHttpHost+m_strFtpPath+_T("/version.dat"));		
		SetDlgItemText(IDC_STATIC_VERSION,value);
	}
	catch (...)
	{
	}

	if(m_bProxy)
	{
		AfxMessageBox("代理模式！");
	}


	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CftpUpdatePackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CftpUpdatePackerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CftpUpdatePackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
	{

		TCHAR szPath[MAX_PATH]	= {0};
		GetModuleFileName(NULL, szPath, MAX_PATH);

		TCHAR* pFinded	= _tcsrchr(szPath, _T('\\'));
		*pFinded		= 0x0;

		SendMessage(hwnd, BFFM_SETSELECTION,
			TRUE,(LPARAM)szPath);
	}
	return 0;
}

void CftpUpdatePackerDlg::GenerateThread( void* pParam )
{
	CftpUpdatePackerDlg* pThis = (CftpUpdatePackerDlg*)pParam;


	CListBox * pList=(CListBox *)pThis->GetDlgItem(IDC_LIST1);
	pList->ResetContent();
	pList->AddString("开始制作版本...");
	pThis->GetDlgItemText(IDC_EDIT_UPDATETXT, pThis->m_strContext);
	pThis->m_strContext.Trim();
	if(pThis->m_strContext  == "")
	{
		pThis->SetDlgItemText(IDC_PRO_TEXT, _T("context must not null!"));
		pThis->GetDlgItem(IDC_EDIT_UPDATETXT)->SetFocus();
		return;
	}

	int nLower, nUpper;
	pThis->m_strProCtrl.GetRange(nLower, nUpper);
	pThis->m_strProCtrl.SetStep((nUpper-nLower)/5); 

	pThis->SetDlgItemText(IDC_PRO_TEXT, _T("collect dir..."));
	pThis->CollectDir();
	pThis->m_strProCtrl.StepIt();

	if(pThis->m_mapCollect.empty())
	{
		pThis->SetDlgItemText(IDC_PRO_TEXT, _T("versions must not null!"));
		pThis->m_strProCtrl.SetPos(0);
		pList->AddString("没有发现新版本文件");
		return;
	}

	pThis->SaveConfig();

	//对排序过的只留3个最新的
	//移动到历史版本
	pThis->m_mapBak.clear();
	map<CString, CollectStruct, classcomp> mapCollectTemp;
	mapCollectTemp.swap(pThis->m_mapCollect);
	unsigned int iIndex=0;
	int iTemppos=0;
	for (map<CString, CollectStruct, classcomp>::iterator i=mapCollectTemp.begin(); i!=mapCollectTemp.end(); /*i++*/)  
	{  
		if(iIndex>=mapCollectTemp.size()-MAX_MAKE_VERSION_COUNT)
		{
			pThis->m_mapCollect[i->first]=i->second;
			char szTemp[MAX_PATH]={0};
			if(iTemppos==0)
				sprintf(szTemp, "发现版本：%s 【当前版本】", i->first);
			else
				sprintf(szTemp, "发现版本：%s 【最新版本】", i->first);
			pList->AddString(szTemp);
			iTemppos++;
		}else
		{
			pThis->m_mapBak[i->first]=i->second;
		}
		i++;
		iIndex++;
	}  




	pThis->SetDlgItemText(IDC_PRO_TEXT, _T("compare files..."));
	pThis->CompareCollect();

	pThis->m_strProCtrl.StepIt();

	if(pThis->CreateVersionDat())
	{
		pList->AddString("创建vesion.dat成功！");
	}

	//签名最新版
	if(!pThis->AutoSignLastVersion())
	{
		pList->AddString("签名失败!!!");
	}

	char szDuiDllFile[MAX_PATH]={0};
	sprintf(szDuiDllFile, "%s%s\\%s",pThis->m_strVerRootDir, pThis->m_maxCollect.strVersion, DUILIB_DLL);
	//签名dll
	if(!pThis->AutoSignFile(szDuiDllFile))
	{
		pList->AddString("Duilib.dll签名失败!!!");
	}


	pThis->SetDlgItemText(IDC_PRO_TEXT, _T("make package..."));

	pThis->MakePackage();

	pThis->m_strProCtrl.StepIt();

	pThis->SaveVersionDat();

	if(pThis->MakeFileCrcList())
	{
		pList->AddString("创建CRC列表成功！");
	}else
	{
		pList->AddString("创建CRC列表失败！");
	}

	pThis->m_strProCtrl.StepIt();

	//release 
	pThis->SetDlgItemText(IDC_PRO_TEXT, _T("上传中.."));
	if (pThis->ReleaseVersion())
	{
		pThis->m_strProCtrl.StepIt();
		pThis->SetDlgItemText(IDC_PRO_TEXT, _T("release comlete！"));
		pList->AddString("恭喜，更新包已制作成功，请进行测试和确认！");
	}
	else
	{
		pThis->m_strProCtrl.SetPos(0);
		pList->AddString("一键升级制作失败！！！");
	}


	//移动其他版本到历史 m_strHistroyDir

	int iMoved=0;
	for (map<CString, CollectStruct, classcomp>::iterator iBak=pThis->m_mapBak.begin(); iBak!=pThis->m_mapBak.end(); /*i++*/)  
	{  
		CString str=iBak->second.strVersion;
		pThis->VertifyDir(pThis->m_strVerRootDir);
		pThis->VertifyDir(pThis->m_strHistroyDir);
		if(MoveFile(pThis->m_strVerRootDir+str,pThis->m_strHistroyDir+str)) iMoved++;
		iBak++;
	} 

	if(pThis->m_mapBak.size()>0)
	{
		char szTemp[100]={0};
		sprintf(szTemp, "发现%d个历史版本，成功移动：%d 个!", pThis->m_mapBak.size(),iMoved);
		pList->SetTopIndex(pList->AddString(szTemp));
	}

	pList->SetTopIndex(pList->GetCount() - 1);



}
void CftpUpdatePackerDlg::OnBnClickedButtonStart()
{

	_beginthread(GenerateThread, 0, this);
}



void CftpUpdatePackerDlg::CollectDir()
{

	m_mapCollect.clear();
	CString strTemp = m_strVerRootDir + _T("\\*.*");
	WIN32_FIND_DATA fd;

	HANDLE hFindFile = FindFirstFile(strTemp, &fd);
	if (INVALID_HANDLE_VALUE != hFindFile)
	{
		do
		{
			bool bDir = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

			if (bDir)
			{
				if (_tcscmp(fd.cFileName, _T("."))==0 || _tcscmp(fd.cFileName, _T(".."))==0)
				{
					continue;
				}            
				else
				{
					CollectStruct collect;

					collect.strVersion = fd.cFileName;
					collect.stVersion.InitFromString(collect.strVersion);

					m_mapCollect[collect.strVersion] =  collect;

				}
			}

		}while(FindNextFile(hFindFile, &fd));
	}
}

void CftpUpdatePackerDlg::CompareCollect()
{
	map<CString, CollectStruct , classcomp>::iterator itorEnd = m_mapCollect.end();
	itorEnd--;
	m_maxCollect = itorEnd->second;
	m_mapCollect.erase(itorEnd);

	CString strTemp = m_strVerRootDir + m_maxCollect.strVersion;
	VertifyDir(strTemp);
	for (map<CString, CollectStruct, classcomp>::iterator itor = m_mapCollect.begin(); itor != m_mapCollect.end(); ++itor)
	{
		CompareCollect(strTemp, m_maxCollect, itor->second);
	}
}

void CftpUpdatePackerDlg::CompareCollect( CString& strCompare, CollectStruct& maxCollect, CollectStruct& collect )
{
	WIN32_FIND_DATA fd;
	CString strCompareTemp = strCompare + _T("*.*");
	HANDLE hFindFile = FindFirstFile(strCompareTemp, &fd);
	if (INVALID_HANDLE_VALUE != hFindFile)
	{
		do
		{
			bool	bDir	= ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			CString strTemp = strCompare + fd.cFileName;
			if (bDir)
			{
				if (_tcscmp(fd.cFileName, _T("."))==0 || _tcscmp(fd.cFileName, _T(".."))==0)
				{
					continue;
				}            
				else
				{
					
					VertifyDir(strTemp);
					CompareCollect(strTemp, maxCollect, collect);

				}
			}
			else
			{
				CString strAnother = strCompare + fd.cFileName;
				strAnother.Replace(maxCollect.strVersion, collect.strVersion);

				/*if (IsPortableExecutable(strTemp))
				{
					if (CompareCodeSection(strTemp, strAnother))
					{
						collect.listFile.push_back(strTemp);
					}
				}
				else*/
				{
					if (CompareFile(strTemp, strAnother))
					{
						collect.listFile.push_back(strTemp);
					}
				}
				
			}

		}while(FindNextFile(hFindFile, &fd));
	}
}

void CftpUpdatePackerDlg::VertifyDir( CString& strDir )
{
	if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
	{
		strDir += _T('\\');
	}
}

void CftpUpdatePackerDlg::VertifyLinuxFtpDir( CString& strDir )
{
	strDir.Replace('\\','/');
}

int CftpUpdatePackerDlg::CompareFile( CString& pathA, CString& pathB )
{
	HANDLE hFileA = CreateFile(pathA, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileA == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	HANDLE hFileB = CreateFile(pathB, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileB == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFileA);
		return 1;
	}
	DWORD dwFileSizeA = GetFileSize(hFileA, NULL);
	DWORD dwFileSizeB = GetFileSize(hFileB, NULL);

	if(dwFileSizeA > dwFileSizeB)
	{
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return 1;
	}
	else if(dwFileSizeA < dwFileSizeB)
	{
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return -1;
	}

	DWORD	dwRead = 0;
	CHAR*	pszBufferA	= new CHAR[dwFileSizeA];
	CHAR*	pszBufferB	= new CHAR[dwFileSizeB];
	ReadFile(hFileA, pszBufferA, dwFileSizeA, &dwRead, NULL);
	ReadFile(hFileB, pszBufferB, dwFileSizeB, &dwRead, NULL);

	DWORD dwCrcA = GetCrc(pszBufferA, dwFileSizeA);
	DWORD dwCrcB = GetCrc(pszBufferB, dwFileSizeB);
	if(dwCrcA > dwCrcB)
	{
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		delete pszBufferA;
		delete pszBufferB;
		return 1;
	}
	else if (dwCrcA < dwCrcB)
	{
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		delete pszBufferA;
		delete pszBufferB;
		return 1;
	}
	CloseHandle(hFileA);
	CloseHandle(hFileB);
	delete pszBufferA;
	delete pszBufferB;
	return 0;
}

DWORD CftpUpdatePackerDlg::GetCrc( LPCSTR lpBuffer, DWORD dwSize )
{
	static DWORD crcTable[256] = {0}, crcTmp1;
	BYTE*	ptr	= (BYTE*)lpBuffer;
	DWORD	Size= dwSize;
	if (crcTable[0] == 0)
	{
		//动态生成CRC-32表
		for (int i=0; i<256; i++)
		{
			crcTmp1 = i;
			for (int j=8; j>0; j--)
			{
				if (crcTmp1&1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
				else crcTmp1 >>= 1;
			}
			crcTable[i] = crcTmp1;
		}
	}

	DWORD dwRet = 0;
	//计算CRC32值
	DWORD crcTmp2 = 0xFFFFFFFF;
	while(Size--)
	{
		crcTmp2 = ((crcTmp2>>8) & 0x00FFFFFF) ^ crcTable[ (crcTmp2^(*ptr)) & 0xFF ];
		ptr++;
	}

	dwRet=(crcTmp2^0xFFFFFFFF);//异或0x2e3f851c
	return dwRet;
}


BOOL VerifyEmbeddedSignature(LPCWSTR pwszSourceFile)
{
	BOOL bSigned=FALSE;
    LONG lStatus;
    DWORD dwLastError;

    // Initialize the WINTRUST_FILE_INFO structure.

    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = pwszSourceFile;
    FileData.hFile = NULL;
    FileData.pgKnownSubject = NULL;

    /*
    WVTPolicyGUID specifies the policy to apply on the file
    WINTRUST_ACTION_GENERIC_VERIFY_V2 policy checks:

    1) The certificate used to sign the file chains up to a root 
    certificate located in the trusted root certificate store. This 
    implies that the identity of the publisher has been verified by 
    a certification authority.

    2) In cases where user interface is displayed (which this example
    does not do), WinVerifyTrust will check for whether the 
    end entity certificate is stored in the trusted publisher store, 
    implying that the user trusts content from this publisher.

    3) The end entity certificate has sufficient permission to sign 
    code, as indicated by the presence of a code signing EKU or no 
    EKU.
    */

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA WinTrustData;

    // Initialize the WinVerifyTrust input data structure.

    // Default all fields to 0.
    memset(&WinTrustData, 0, sizeof(WinTrustData));

    WinTrustData.cbStruct = sizeof(WinTrustData);

    // Use default code signing EKU.
    WinTrustData.pPolicyCallbackData = NULL;

    // No data to pass to SIP.
    WinTrustData.pSIPClientData = NULL;

    // Disable WVT UI.
    WinTrustData.dwUIChoice = WTD_UI_NONE;

    // No revocation checking.
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 

    // Verify an embedded signature on a file.
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;

    // Default verification.
    WinTrustData.dwStateAction = 0;

    // Not applicable for default verification of embedded signature.
    WinTrustData.hWVTStateData = NULL;

    // Not used.
    WinTrustData.pwszURLReference = NULL;

    // Default.
    WinTrustData.dwProvFlags = WTD_SAFER_FLAG;

    // This is not applicable if there is no UI because it changes 
    // the UI to accommodate running applications instead of 
    // installing applications.
    WinTrustData.dwUIContext = 0;

    // Set pFile.
    WinTrustData.pFile = &FileData;

    // WinVerifyTrust verifies signatures as specified by the GUID 
    // and Wintrust_Data.
    lStatus = WinVerifyTrust(
        NULL,
        &WVTPolicyGUID,
        &WinTrustData);

    switch (lStatus) 
    {
    case ERROR_SUCCESS:
        /*
        Signed file:
        - Hash that represents the subject is trusted.

        - Trusted publisher without any verification errors.

        - UI was disabled in dwUIChoice. No publisher or 
        time stamp chain errors.

        - UI was enabled in dwUIChoice and the user clicked 
        "Yes" when asked to install and run the signed 
        subject.
        */
        wprintf_s(L"The file \"%s\" is signed and the signature "
            L"was verified.\n",
            pwszSourceFile);
		bSigned=TRUE;
        break;

    case TRUST_E_NOSIGNATURE:
        // The file was not signed or had a signature 
        // that was not valid.

        // Get the reason for no signature.
        dwLastError = GetLastError();
        if (TRUST_E_NOSIGNATURE == dwLastError ||
            TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError ||
            TRUST_E_PROVIDER_UNKNOWN == dwLastError) 
        {
            // The file was not signed.
            wprintf_s(L"The file \"%s\" is not signed.\n",
                pwszSourceFile);
        } 
        else 
        {
            // The signature was not valid or there was an error 
            // opening the file.
            wprintf_s(L"An unknown error occurred trying to "
                L"verify the signature of the \"%s\" file.\n",
                pwszSourceFile);
        }

        break;

    case TRUST_E_EXPLICIT_DISTRUST:
        // The hash that represents the subject or the publisher 
        // is not allowed by the admin or user.
        wprintf_s(L"The signature is present, but specifically "
            L"disallowed.\n");
        break;

    case TRUST_E_SUBJECT_NOT_TRUSTED:
        // The user clicked "No" when asked to install and run.
        wprintf_s(L"The signature is present, but not "
            L"trusted.\n");
        break;

    case CRYPT_E_SECURITY_SETTINGS:
        /*
        The hash that represents the subject or the publisher 
        was not explicitly trusted by the admin and the 
        admin policy has disabled user trust. No signature, 
        publisher or time stamp errors.
        */
        wprintf_s(L"CRYPT_E_SECURITY_SETTINGS - The hash "
            L"representing the subject or the publisher wasn't "
            L"explicitly trusted by the admin and admin policy "
            L"has disabled user trust. No signature, publisher "
            L"or timestamp errors.\n");
        break;

    default:
        // The UI was disabled in dwUIChoice or the admin policy 
        // has disabled user trust. lStatus contains the 
        // publisher or time stamp chain error.
        wprintf_s(L"Error is: 0x%x.\n",
            lStatus);
        break;
    }

    return bSigned;
}

wstring ANSIToUnicode( const string& str )
{
	int len = 0;
	len = str.length();
	int unicodeLen = ::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0 ); 
	wchar_t * pUnicode; 
	pUnicode = new wchar_t [unicodeLen+1]; 
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 
	::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen ); 
	wstring rt; 
	rt = ( wchar_t* )pUnicode;
	delete pUnicode; 
	return rt; 
}

bool CftpUpdatePackerDlg::AutoSignFile(LPTSTR szSignFile)
{
	if(VerifyEmbeddedSignature(ANSIToUnicode(szSignFile).c_str()))
	{
		m_pList->SetTopIndex(m_pList->AddString("检测文件已签名，跳过签名步骤！"));
		return true;
	}

	bool bret=false;
	HINTERNET	hInet = InternetOpen(NULL,
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,
		NULL,
		0);
	if (!hInet)
	{
		return false;
	}

	CString strAccount="administrator";
	CString strPassword="1q2w3e4r..";
	CString strFtpHost="192.168.59.9";
	SetDlgItemText(IDC_PRO_TEXT, "FTP connecting...");
	HINTERNET	hFtp;

	CButton    *pButton    =    (CButton    *)GetDlgItem(IDC_CHECK_FTPMODE); 
	if (pButton->GetCheck()!=1)
	{
		hFtp = InternetConnect(hInet,
			strFtpHost,
			INTERNET_DEFAULT_FTP_PORT,
			strAccount,
			strPassword,
			INTERNET_SERVICE_FTP,
			NULL,
			NULL);
	}else
	{
		hFtp= InternetConnect(hInet,
			strFtpHost,
			INTERNET_DEFAULT_FTP_PORT,
			strAccount,
			strPassword,
			INTERNET_SERVICE_FTP,
			INTERNET_FLAG_PASSIVE,
			NULL);
	}


	if (!hFtp)
	{
		InternetCloseHandle(hInet);
		SetDlgItemText(IDC_PRO_TEXT, "FTP connect fail!");
		return false;
	}

		

	char szSignFileServer[MAX_PATH]={0};
	sprintf(szSignFileServer, "\\signtool\\Signbin\\temp\\sign.exe");

	if (!FtpPutFile(hFtp, szSignFile, szSignFileServer, FTP_TRANSFER_TYPE_BINARY, NULL))
	{
		DWORD dw=GetLastError();
		//strText = itor->second.strPackPath + " puts fail！ stop release";
		//SetDlgItemText(IDC_PRO_TEXT, strText);
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		return false;
	}else
	{
		CString strTips=szSignFile;
		strTips=strTips+" 上传成功!";
		m_pList->SetTopIndex(m_pList->AddString(strTips));
		Sleep(500);
	}


	CHttpClient   conn;  
	CString   value="http://192.168.59.9/signtool/autosign/start";  
	value=conn.doGet(value); 
	int iPos=value.Find("OK");
	if (iPos>=0)
	{
		m_pList->SetTopIndex(m_pList->AddString("签名中，请稍后......"));
	}

	char szSignedFileServer[MAX_PATH]={0};
	sprintf(szSignedFileServer, "\\signtool\\Signbin\\temp\\signed.exe");


	bool bSinded=false;
	int iWaitSec=5;
	while(1)
	{
		iWaitSec--;
		if(iWaitSec<0)break;
		Sleep(1000);
		CString  value=conn.doGet("http://192.168.59.9/signtool/autosign/feedback");  
		int iFound=value.Find("status\":\"1\"");
		if (iFound>0)
		{
			bSinded=true;
			break;
		}
	}
	if(bSinded)
	{
		m_pList->SetTopIndex(m_pList->AddString("签名成功！"));
	}



	char szSignedFile[MAX_PATH]={0};
	sprintf(szSignedFile, "%s%s\\signed_%s",m_strVerRootDir, m_maxCollect.strVersion, MAIN_EXE);


	if(bSinded)
	{
		if(!FtpGetFile(hFtp,szSignedFileServer,szSignedFile,FALSE,FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_RELOAD,NULL))
		{
			m_pList->SetTopIndex(m_pList->AddString("下载签名文件失败！"));
		}else
		{
			m_pList->SetTopIndex(m_pList->AddString("下载签名文件成功！"));
			
		}

	}

 
	HANDLE hFileA= CreateFile(szSignedFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileA != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFileA);
		if(MoveFileEx(szSignedFile,szSignFile, MOVEFILE_REPLACE_EXISTING))
		{
			bret=true;
		}
		DWORD dwe=GetLastError();
	
	} 

	InternetCloseHandle(hFtp);
	InternetCloseHandle(hInet);

	return bret;
}

//自动签名
bool CftpUpdatePackerDlg::AutoSignLastVersion()
{
	char szSignFile[MAX_PATH]={0};
	sprintf(szSignFile, "%s%s\\%s",m_strVerRootDir, m_maxCollect.strVersion, MAIN_EXE);
	if(VerifyEmbeddedSignature(ANSIToUnicode(szSignFile).c_str()))
	{
		m_pList->SetTopIndex(m_pList->AddString("检测到主程序已签名，跳过签名步骤！"));
		return true;
	}


	bool bret=false;
	HINTERNET	hInet = InternetOpen(NULL,
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,
		NULL,
		0);
	if (!hInet)
	{
		return false;
	}

	CString strAccount="administrator";
	CString strPassword="1q2w3e4r..";
	CString strFtpHost="192.168.59.9";
	SetDlgItemText(IDC_PRO_TEXT, "FTP connecting...");
	HINTERNET	hFtp;

	CButton    *pButton    =    (CButton    *)GetDlgItem(IDC_CHECK_FTPMODE); 
	if (pButton->GetCheck()!=1)
	{
		hFtp = InternetConnect(hInet,
			strFtpHost,
			INTERNET_DEFAULT_FTP_PORT,
			strAccount,
			strPassword,
			INTERNET_SERVICE_FTP,
			NULL,
			NULL);
	}else
	{
		hFtp= InternetConnect(hInet,
			strFtpHost,
			INTERNET_DEFAULT_FTP_PORT,
			strAccount,
			strPassword,
			INTERNET_SERVICE_FTP,
			INTERNET_FLAG_PASSIVE,
			NULL);
	}


	if (!hFtp)
	{
		InternetCloseHandle(hInet);
		SetDlgItemText(IDC_PRO_TEXT, "FTP connect fail!");
		return false;
	}

	

	

	char szSignFileServer[MAX_PATH]={0};
	sprintf(szSignFileServer, "\\signtool\\Signbin\\temp\\sign.exe");

	if (!FtpPutFile(hFtp, szSignFile, szSignFileServer, FTP_TRANSFER_TYPE_BINARY, NULL))
	{
		DWORD dw=GetLastError();
		//strText = itor->second.strPackPath + " puts fail！ stop release";
		//SetDlgItemText(IDC_PRO_TEXT, strText);
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		return false;
	}else
	{
		m_pList->SetTopIndex(m_pList->AddString("上传成功!"));
		Sleep(500);
	}


	CHttpClient   conn;  
	CString   value="http://192.168.59.9/signtool/autosign/start";  
	value=conn.doGet(value); 
	int iPos=value.Find("OK");
	if (iPos>=0)
	{
		m_pList->SetTopIndex(m_pList->AddString("签名中，请稍后......"));
	}

	char szSignedFileServer[MAX_PATH]={0};
	sprintf(szSignedFileServer, "\\signtool\\Signbin\\temp\\signed.exe");


	bool bSinded=false;
	int iWaitSec=5;
	while(1)
	{
		iWaitSec--;
		if(iWaitSec<0)break;
		Sleep(1000);
		CString  value=conn.doGet("http://192.168.59.9/signtool/autosign/feedback");  
		int iFound=value.Find("status\":\"1\"");
		if (iFound>0)
		{
			bSinded=true;
			break;
		}
	}
	if(bSinded)
	{
		m_pList->SetTopIndex(m_pList->AddString("签名成功！"));
	}



	char szSignedFile[MAX_PATH]={0};
	sprintf(szSignedFile, "%s%s\\signed_%s",m_strVerRootDir, m_maxCollect.strVersion, MAIN_EXE);


	if(bSinded)
	{
		if(!FtpGetFile(hFtp,szSignedFileServer,szSignedFile,FALSE,FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_RELOAD,NULL))
		{
			m_pList->SetTopIndex(m_pList->AddString("下载签名文件失败！"));
		}else
		{
			m_pList->SetTopIndex(m_pList->AddString("下载签名文件成功！"));
			
		}

	}

 
	HANDLE hFileA= CreateFile(szSignedFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileA != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFileA);
		if(MoveFileEx(szSignedFile,szSignFile, MOVEFILE_REPLACE_EXISTING))
		{
			bret=true;
		}
		DWORD dwe=GetLastError();
	
	} 

	InternetCloseHandle(hFtp);
	InternetCloseHandle(hInet);

	return bret;
}



//////////////////////////////////////////////////////////////////////////

typedef struct _PARAM_RES
{
	char szFileName[MAX_PATH];
	char szPath[MAX_PATH];
}PARAM_RES,*PPARAM_RES;

void Traversefolder(vector<PARAM_RES> &vecList,char * szPath,char * szExt)//遍历目录获取ppt文件
{
	char szFolderPath[MAX_PATH] = {0};
	strcpy(szFolderPath,szPath);
	sprintf(szFolderPath,"%s\\*.%s",szPath,szExt);
	WIN32_FIND_DATA wfd;
	HANDLE hFile = FindFirstFile(szFolderPath, &wfd);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (wfd.cFileName[0] != '.' && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				PARAM_RES ppt;
				CString str;
				str.Format("%s%s",szPath,wfd.cFileName);
				str.Replace("\\\\","\\");
				//sprintf(ppt.szPath,"%s%s",szPath,wfd.cFileName);
				strcpy(ppt.szPath,(char *)str.GetString());
				strcpy(ppt.szFileName,wfd.cFileName);
				vecList.push_back(ppt);
			}
			if(wfd.cFileName[0] != '.' && wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char szTempPath[MAX_PATH] = {0};
				sprintf(szTempPath,"%s\\%s\\",szPath,wfd.cFileName);
				Traversefolder(vecList,szTempPath,szExt);
			}
		} while (FindNextFile(hFile, &wfd));
		FindClose(hFile);
	}
};


void CftpUpdatePackerDlg::GetAllHash()
{
	map<CString, CollectStruct, classcomp>::iterator itor = m_mapCollect.begin();
	if(itor!=m_mapCollect.end())
	{
		vector<PARAM_RES> vecList;
		char szExt[] = "*";

		char szHashDir[MAX_PATH]={0};
		sprintf(szHashDir, "%s%s\\", m_strVerRootDir, m_maxCollect.strVersion);

		Traversefolder(vecList, szHashDir, szExt);

		char szUpdatePath[MAX_PATH]={0};
		sprintf(szUpdatePath, "%supdate.txt", itor->second.strPackPath);
		HANDLE m_hFile = CreateFile(szUpdatePath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			AfxMessageBox("文件打开失败");
			return;
		}

		char * szFilePath = szHashDir;//strFilePath.GetBuffer(strFilePath.GetLength());

		int nPos = 0;
		unsigned int nHash = 0;

		char szBuff[256] = {0};

		char * pos = NULL;
		for(unsigned int i= 0;i< vecList.size();i++)
		{
			nPos = (i+1)/vecList.size() * 100;

			nHash = SuperFastHashFile(vecList[i].szPath);

			pos = strstr(vecList[i].szPath,szFilePath);
			if(pos)
			{
				char * FilePath = pos+ strlen(szFilePath);
				if(stricmp(FilePath,"update.txt") == 0 || stricmp(FilePath,"CreateData.exe") == 0)
					continue;
				sprintf(szBuff,"%s|%u\r\n",pos+ strlen(szFilePath),nHash);
				DWORD dwSize = 0;
				WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
			}
		}
		CloseHandle(m_hFile);
		if(CopyFile(szUpdatePath, m_strGetUpdateFilePath,FALSE))
			m_bHashThreadResult=true;
	}
}


DWORD CftpUpdatePackerDlg::OpThread(LPARAM lParam)
{
	CftpUpdatePackerDlg* pThis = (CftpUpdatePackerDlg*)lParam;
	pThis->GetAllHash();
	return 0;
}


bool CftpUpdatePackerDlg::MakeFileCrcList()
{

	m_bHashThreadResult=false;
	HANDLE hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OpThread, this, 0, NULL);
	WaitForSingleObject(hThread, 5000);
	CloseHandle(hThread);
	return m_bHashThreadResult;

	//bool bret=false;
	//map<CString, CollectStruct, classcomp>::iterator itor = m_mapCollect.begin();


	//if(itor!=m_mapCollect.end())
	//{
	//	char szExe[MAX_PATH]={0};
	//	sprintf(szExe, "%sCreateData.exe", m_strExecDir);

	//	char szCommand[MAX_PATH*2]={0};
	//	sprintf(szCommand, " %s%s\\ %s", m_strVerRootDir, m_maxCollect.strVersion, itor->second.strPackPath);

	//	STARTUPINFO si; //一些必备参数设置
	//	memset(&si,0,sizeof(STARTUPINFO));
	//	si.cb = sizeof(STARTUPINFO);
	//	GetStartupInfo(&si);
	//	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//	si.wShowWindow = SW_HIDE;//
	//	PROCESS_INFORMATION pi; //必备参数设置结束
	//	try
	//	{	
	//		char szCrcFile[MAX_PATH]={0};
	//		sprintf(szCrcFile,"%supdate.txt",  itor->second.strPackPath);
	//		DeleteFile(szCrcFile);
	//		CreateProcess(szExe, szCommand, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	//		HANDLE hFileA=INVALID_HANDLE_VALUE;
	//		int iWaitSecon=5;
	//		while (TRUE)
	//		{
	//			 Sleep(1000);
	//			 iWaitSecon--;
	//			 if(iWaitSecon<0)  break;
	//			 hFileA= CreateFile(szCrcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//			if (hFileA == INVALID_HANDLE_VALUE)
	//			{
	//				continue;
	//			}else
	//			{
	//				CopyFile(szCrcFile, m_strGetUpdateFilePath,FALSE);
	//				CloseHandle(hFileA);
	//				bret=true;
	//				break;
	//			}
	//			
	//		}
	//		
	//	}
	//	catch (...)
	//	{
	//	}
	//}
	//return bret;
}

void CftpUpdatePackerDlg::MakePackage()
{
	CFileOperateion::DeleteDirectory(m_strPackageDir);
	CString strPackageName;
	CString strPackageDir;
	for (map<CString, CollectStruct, classcomp>::iterator itor = m_mapCollect.begin(); itor != m_mapCollect.end(); ++itor)
	{
		strPackageName= itor->second.strVersion;
		strPackageName+= _T('_');
		strPackageName+=m_maxCollect.strVersion;

		strPackageDir	= m_strPackageDir + strPackageName;
		VertifyDir(strPackageDir);
		CFileOperateion::CreateDirectory(strPackageDir);

		list<CString>* pList = &itor->second.listFile;

		for (list<CString>::iterator itorChild = pList->begin(); itorChild != pList->end(); ++itorChild)
		{
			CString strNewPath = *itorChild;
			strNewPath.Replace(m_maxCollect.strVersion, strPackageName);
			strNewPath.Replace(m_strVerRootDir, m_strPackageDir);

			int nPos = strNewPath.ReverseFind(_T('\\'));
			CString strNewDir = strNewPath.Left(nPos);
			CFileOperateion::CreateDirectory(strNewDir);

			CopyFile(*itorChild, strNewPath, FALSE);


		}
		//放入最新Version
		int nPos				= m_strVersionPath.ReverseFind(_T('\\'));
		CString strVersionName	= m_strVersionPath.Right(m_strVersionPath.GetLength() - nPos - 1);

		CString strToPath = strPackageDir + strVersionName;
		CopyFile(m_strVersionPath, strToPath, FALSE);

		//放入到最新版本的一个
		CopyFile(m_strVersionPath, m_strVerRootDir+m_maxCollect.strVersion+"\\"+strVersionName, FALSE);
		itor->second.strPackPath = strPackageDir;
	}
}

void CftpUpdatePackerDlg::ArchiveDirPackage( CString& strDir, CString& strPath )
{

	CString strText = "acrhive：";
	strText += strPath;
	SetDlgItemText(IDC_PRO_TEXT, strText);
	HANDLE hSendReadPipe, hSendWritePipe, hRecvReadPipe, hRecvWritePipe;

	SECURITY_ATTRIBUTES stAttr;

	stAttr.nLength				= sizeof(SECURITY_ATTRIBUTES);
	stAttr.bInheritHandle		= TRUE;
	stAttr.lpSecurityDescriptor	= NULL;

	if (!CreatePipe(&hSendReadPipe, &hSendWritePipe, &stAttr, 0) || !CreatePipe(&hRecvReadPipe, &hRecvWritePipe, &stAttr, 0))
	{
		return;
	}

	STARTUPINFO				si;
	PROCESS_INFORMATION		pi;
	ZeroMemory(&si, sizeof(si));
	si.cb			= sizeof(STARTUPINFO);
	si.hStdInput	= hSendReadPipe;
	si.hStdOutput	= hRecvWritePipe;
	si.hStdError	= hRecvWritePipe;
	si.wShowWindow	= SW_HIDE;
	si.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	ZeroMemory(&pi, sizeof(pi));

	CString strCmd;

	if (CreateProcess("C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		//CloseHandle(pi.hThread);
		//CloseHandle(pi.hProcess);
	}
	
 	DWORD dwReal;
 	TCHAR szBuff[MAX_PATH] =  {0};

// 	strCmd	= strDir.Left(2);
// 	strCmd	+= "\r\n";
// 	WriteFile(hSendWritePipe, strCmd, strCmd.GetLength(), &dwReal, NULL);
// 
// 	strCmd	= "cd ";
// 	strCmd	+= strDir;
// 	strCmd	+= "\r\n";
// 
// 	WriteFile(hSendWritePipe, strCmd, strCmd.GetLength(), &dwReal, NULL);

	strCmd	= "\"C:\\Program Files\\WinRAR\\RAR.exe\"";
	strCmd	+= " a -k -r -s -m1 -ep1 ";
	strCmd	+= strPath;
	strCmd	+= " ";
	strCmd	+= strDir;
	strCmd	+= "";
	strCmd	+= "\r\n";

 	WriteFile(hSendWritePipe, strCmd, strCmd.GetLength(), &dwReal, NULL);
	Sleep(100);
	DWORD dwSize;
	while(dwSize = GetFileSize(hRecvReadPipe, NULL))
	{
		TCHAR* pszBuffer = new TCHAR[dwSize];
		ReadFile(hRecvReadPipe, pszBuffer, dwSize, &dwReal, NULL);
		if (_tcsstr(pszBuffer, "完成") != NULL)
		{
			delete pszBuffer;
			break;
		}
		delete pszBuffer;
		Sleep(300);
	}
	WriteFile(hSendWritePipe, _T("exit\r\n"), _tcslen(_T("exit\r\n")), &dwReal, NULL);


	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

}



HINTERNET CftpUpdatePackerDlg::myGetFtpConnection(HINTERNET & hInet)
{
	GetDlgItemText(IDC_EDIT_FTP_DOMAIN,m_strFtpHost);
	GetDlgItemText(IDC_EDIT_USER, m_strAccount);
	GetDlgItemText(IDC_EDIT_PASS, m_strPassword);
	GetDlgItemText(IDC_EDIT_FTP_DIR, m_strFtpPath);
	GetDlgItemText(IDC_EDIT_UPDATETXT, m_strContext);

	HINTERNET hFtp =NULL;

	if(m_bProxy)
	{
		hInet = InternetOpen(NULL,
			INTERNET_OPEN_TYPE_PROXY,
			"HTTP=HTTP://192.168.59.9:808",
			NULL,
			0);
	}else{
		hInet= InternetOpen(NULL,
			INTERNET_OPEN_TYPE_DIRECT,
			NULL,
			NULL,
			0);
	}

	if (!hInet)
	{
		SetDlgItemText(IDC_PRO_TEXT, "net error！");
		return hFtp;
	}

	CButton    *pButton    =    (CButton    *)GetDlgItem(IDC_CHECK_FTPMODE); 
	DWORD dwFlag=NULL;
	if (pButton->GetCheck()==1)
	{	
		dwFlag=INTERNET_FLAG_PASSIVE;
	}

	SetDlgItemText(IDC_PRO_TEXT, "FTP connecting...");
	if(m_bProxy)
	{
		hFtp= InternetConnect(hInet,
			m_strFtpHost,
			INTERNET_DEFAULT_FTP_PORT,
			m_strAccount,
			m_strPassword,
			INTERNET_SERVICE_FTP,
			dwFlag ,
			NULL);

	}else{
		hFtp = InternetConnect(hInet,
			m_strFtpHost,
			INTERNET_DEFAULT_FTP_PORT,
			m_strAccount,
			m_strPassword,
			INTERNET_SERVICE_FTP,
			dwFlag ,
			NULL);
	}
	if (!hFtp)
	{
		DWORD dw =GetLastError();
		InternetCloseHandle(hInet);
		SetDlgItemText(IDC_PRO_TEXT, "FTP connect fail!");
	}
	return hFtp;
}




bool CftpUpdatePackerDlg::ReleaseVersion()
{
	CListBox * pList=(CListBox *)GetDlgItem(IDC_LIST1);

	HINTERNET	hInet;
	HINTERNET	hFtp =NULL;
	hFtp=myGetFtpConnection(hInet);
	if (!hInet)
	{
		return false;
	}
	if (!hFtp)
	{
		InternetCloseHandle(hInet);
		SetDlgItemText(IDC_PRO_TEXT, "FTP connect fail!");
		return false;
	}


	CString strFtpDir = m_strFtpPath;
	//int nSucc = FtpRemoveDirectory(hFtp, strFtpDir);
	VertifyLinuxFtpDir(strFtpDir);
	FtpCreateDirectory(hFtp, strFtpDir);


	//versiondat
	CString  strFtpVersionDatFile= m_strFtpPath + "/";
	strFtpVersionDatFile += m_strVersionPath.Right(m_strVersionPath.GetLength() -  m_strVersionPath.ReverseFind('\\') - 1);

	FtpDeleteFile(hFtp, strFtpVersionDatFile);

	CString strFtpFile;
	CString strText;
	CString strCrcFile;
	char szTempStr[MAX_PATH]={0};

	for (map<CString, CollectStruct,classcomp>::iterator itor = m_mapCollect.begin(); itor != m_mapCollect.end(); ++itor)
	{
		strFtpFile = strFtpDir + "/";
		int nPos = itor->second.strPackPath.ReverseFind('\\');
		strFtpFile += itor->second.strPackPath.Right(itor->second.strPackPath.GetLength() - nPos - 1);

		strText = "putting " + itor->second.strPackPath;
		SetDlgItemText(IDC_PRO_TEXT, strText);


		sprintf(szTempStr, "发现差异文件：%d 个！",itor->second.listFile.size());
		pList->AddString(szTempStr );


		for (list<CString>::iterator iFile = itor->second.listFile.begin(); iFile !=  itor->second.listFile.end(); ++iFile)
		{
			try
			{
				CString strFile =  *iFile;
				strFile.Replace(m_strVerRootDir+m_maxCollect.strVersion+"\\", "");
				CString strPutFile= itor->second.strPackPath+strFile;
				CString strDir=m_strFtpPath+"/"+strFile;

				CString strFileTemp=strFile;
				char* pFindDir=NULL;
				pFindDir=(char *)_tcschr(strFileTemp,'\\');
				CString strCreatedDir="";
				while (pFindDir)
				{
					*(pFindDir)='\0';
					CString strNewDir=m_strFtpPath+"/";
					strNewDir=strNewDir+strCreatedDir;
					strNewDir=strNewDir+strFileTemp;
					VertifyLinuxFtpDir(strNewDir);
					BOOL bCreated=FtpCreateDirectory(hFtp,strNewDir );

					strCreatedDir+=strFileTemp;
					strCreatedDir+="/";
					strFileTemp=pFindDir+sizeof(TCHAR);
					pFindDir=(char *)_tcschr(strFileTemp,'\\');
					if(bCreated)
						pList->SetTopIndex(pList->AddString("创建目录:"+strNewDir));
				}



				VertifyLinuxFtpDir(strDir);
				if (!FtpPutFile(hFtp, strPutFile, strDir, FTP_TRANSFER_TYPE_BINARY, NULL))
				{
					DWORD dw=GetLastError();
					strText = itor->second.strPackPath + " puts fail！ stop release";
					SetDlgItemText(IDC_PRO_TEXT, strText);
					InternetCloseHandle(hFtp);
					InternetCloseHandle(hInet);
					return false;
				}else
				{
					pList->SetTopIndex(pList->AddString("上传成功:"+strDir));
				}
			}
			catch(...)
			{
				pList->SetTopIndex(pList->AddString("上传失败，发生异常!"));
			}

		}

	}

	int nPos;

	strFtpFile = m_strFtpPath + "/";
	nPos = m_strUpdateTextPath.ReverseFind('\\');
	strFtpFile += m_strUpdateTextPath.Right(m_strUpdateTextPath.GetLength() - nPos - 1);

	strText = "putting " + m_strUpdateTextPath;
	SetDlgItemText(IDC_PRO_TEXT, strText);

	if (!FtpPutFile(hFtp, m_strUpdateTextPath, strFtpFile, FTP_TRANSFER_TYPE_BINARY, NULL))
	{
		strText = m_strUpdateTextPath + " puts fail！ stop release";
		SetDlgItemText(IDC_PRO_TEXT, strText);
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		return false;
	}else
	{
		pList->AddString("更新update.dat成功！");
	}

	strFtpFile = m_strFtpPath + "/";
	nPos = m_strGetUpdateFilePath.ReverseFind('\\');
	strFtpFile += m_strGetUpdateFilePath.Right(m_strGetUpdateFilePath.GetLength() - nPos - 1);

	strText = "putting " + m_strGetUpdateFilePath;
	SetDlgItemText(IDC_PRO_TEXT, strText);

	if (!FtpPutFile(hFtp, m_strGetUpdateFilePath, strFtpFile, FTP_TRANSFER_TYPE_BINARY, NULL))
	{
		strText = m_strGetUpdateFilePath + " puts fail！ stop release";
		SetDlgItemText(IDC_PRO_TEXT, strText);
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		return false;
	}else
	{
		pList->AddString("更新update.txt成功！");
	}


	strText = "putting " + m_strVersionPath;
	SetDlgItemText(IDC_PRO_TEXT, strText);

	if (!FtpPutFile(hFtp, m_strVersionPath, strFtpVersionDatFile, FTP_TRANSFER_TYPE_BINARY, NULL))
	{
		strText = m_strVersionPath + " puts fail！ stop release";
		SetDlgItemText(IDC_PRO_TEXT, strText);
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		return false;
	}else
	{
		pList->AddString("更新version.dat成功！");
	}


	//提交更新日志
	CString strLogPath=m_strFtpPath;
	nPos = strLogPath.ReverseFind('/');
	strLogPath= strLogPath.Mid(0,nPos);
	CString strLogFile="";
	CTime time = CTime::GetCurrentTime();
	CString szTime = time.Format("%Y-%m-%d");
	strLogFile.Format("%s/versionlog/%s-%s.txt",strLogPath, szTime, m_maxCollect.strVersion);

	if (!FtpPutFile(hFtp, m_strUpdateTextPath, strLogFile, FTP_TRANSFER_TYPE_BINARY, NULL))
	{
		strText = m_strUpdateTextPath + " puts fail！ stop release";
		SetDlgItemText(IDC_PRO_TEXT, strText);
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		pList->AddString("提交更新日志失败！"+strLogFile);
		
	}else
	{
		pList->AddString("提交更新日志成功！"+strLogFile);
	}
	


	if(hFtp!=NULL)
	{
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
	}

	return true;
}



bool CftpUpdatePackerDlg::SaveVersionDat()
{
	HANDLE hFileUpdate = CreateFile(m_strUpdateTextPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileUpdate == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwWrite;
	if (!WriteFile(hFileUpdate, m_strContext.GetBuffer(), m_strContext.GetLength(), &dwWrite, NULL))
	{
		m_strContext.ReleaseBuffer();
		CloseHandle(hFileUpdate);
		return false;
	}
	m_strContext.ReleaseBuffer();
	CloseHandle(hFileUpdate);


	return true;
}

bool CftpUpdatePackerDlg::CreateVersionDat()
{
	HANDLE hFile = CreateFile(m_strVersionPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	DWORD dwWrite;
	if (!WriteFile(hFile, m_maxCollect.strVersion.GetBuffer(), m_maxCollect.strVersion.GetLength(), &dwWrite, NULL))
	{
		m_maxCollect.strVersion.ReleaseBuffer();
		CloseHandle(hFile);
		return false;
	}
	m_maxCollect.strVersion.ReleaseBuffer();
	CloseHandle(hFile);
	return true;
}

int CftpUpdatePackerDlg::CompareCodeSection( CString& pathA, CString& pathB )
{
	HANDLE hFileA = CreateFile(pathA, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileA == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	HANDLE hFileB = CreateFile(pathB, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileB == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFileA);
		return 1;
	}
	DWORD dwFileSizeA = GetFileSize(hFileA, NULL);
	DWORD dwFileSizeB = GetFileSize(hFileB, NULL);

	DWORD	dwRead = 0;
	CHAR*	pszBufferA	= new CHAR[dwFileSizeA];
	CHAR*	pszBufferB	= new CHAR[dwFileSizeB];
	ReadFile(hFileA, pszBufferA, dwFileSizeA, &dwRead, NULL);
	ReadFile(hFileB, pszBufferB, dwFileSizeB, &dwRead, NULL);

	PIMAGE_DOS_HEADER pHeadA = (PIMAGE_DOS_HEADER)pszBufferA;
	PIMAGE_DOS_HEADER pHeadB = (PIMAGE_DOS_HEADER)pszBufferB;
	if(pHeadA->e_magic != IMAGE_DOS_SIGNATURE)
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return -1;
	}
	if(pHeadB->e_magic != IMAGE_DOS_SIGNATURE)
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return 1;
	}
	PIMAGE_NT_HEADERS pNtA = (PIMAGE_NT_HEADERS)((DWORD)pszBufferA + pHeadA->e_lfanew);
	PIMAGE_NT_HEADERS pNtB = (PIMAGE_NT_HEADERS)((DWORD)pszBufferB + pHeadB->e_lfanew);
	if (pNtA->Signature != IMAGE_NT_SIGNATURE)
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return -1;
	}
	if (pNtB->Signature != IMAGE_NT_SIGNATURE)
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return 1;
	}
	PIMAGE_SECTION_HEADER	pFisrstSectionA = (PIMAGE_SECTION_HEADER)((BYTE*)pNtA + sizeof(IMAGE_NT_HEADERS32));

	DWORD dwBaseOfCodeA = pNtA->OptionalHeader.BaseOfCode; 
	PIMAGE_SECTION_HEADER pCodeSectionA = findCodeSection(pFisrstSectionA, pNtA->FileHeader.NumberOfSections, dwBaseOfCodeA);
	if( pCodeSectionA == NULL )
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return -1;
	}

	DWORD	dwMappedCodeBaseA = (DWORD)pszBufferA + rva2Offset(pCodeSectionA->VirtualAddress, pNtA->FileHeader.NumberOfSections, pFisrstSectionA);
	DWORD	dwMappedCodeSizeA = pCodeSectionA->Misc.VirtualSize < pCodeSectionA->SizeOfRawData ? pCodeSectionA->Misc.VirtualSize : pCodeSectionA->Misc.VirtualSize;



	PIMAGE_SECTION_HEADER	pFisrstSectionB = (PIMAGE_SECTION_HEADER)((BYTE*)pNtB + sizeof(IMAGE_NT_HEADERS32));

	DWORD dwBaseOfCodeB = pNtB->OptionalHeader.BaseOfCode; 
	PIMAGE_SECTION_HEADER pCodeSectionB = findCodeSection(pFisrstSectionB, pNtB->FileHeader.NumberOfSections, dwBaseOfCodeB);
	if( pCodeSectionB == NULL )
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return 1;
	}

	DWORD	dwMappedCodeBaseB = (DWORD)pszBufferA + rva2Offset(pCodeSectionB->VirtualAddress, pNtB->FileHeader.NumberOfSections, pFisrstSectionB);
	DWORD	dwMappedCodeSizeB = pCodeSectionB->Misc.VirtualSize < pCodeSectionB->SizeOfRawData ? pCodeSectionB->Misc.VirtualSize : pCodeSectionB->Misc.VirtualSize;

	DWORD dwCrcA = GetCrc((LPCSTR)dwMappedCodeBaseA, dwMappedCodeSizeA);
	DWORD dwCrcB = GetCrc((LPCSTR)dwMappedCodeBaseB, dwMappedCodeSizeB);

	if (dwCrcA > dwCrcB)
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return 1;
	}
	else if (dwCrcA < dwCrcB)
	{
		delete pszBufferA;
		delete pszBufferB;
		CloseHandle(hFileA);
		CloseHandle(hFileB);
		return -1;
	}

	delete pszBufferA;
	delete pszBufferB;
	CloseHandle(hFileA);
	CloseHandle(hFileB);

	return 0;

}

bool CftpUpdatePackerDlg::IsPortableExecutable( CString& pathA )
{
	//
	HANDLE hFileA = CreateFile(pathA, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileA == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	DWORD dwFileSizeA = GetFileSize(hFileA, NULL);

	DWORD	dwRead = 0;
	CHAR*	pszBufferA	= new CHAR[dwFileSizeA];
	ReadFile(hFileA, pszBufferA, dwFileSizeA, &dwRead, NULL);

	PIMAGE_DOS_HEADER pHeadA = (PIMAGE_DOS_HEADER)pszBufferA;
	if(pHeadA->e_magic != IMAGE_DOS_SIGNATURE)
	{
		delete pszBufferA;
		CloseHandle(hFileA);
		return false;
	}
	PIMAGE_NT_HEADERS pNtA = (PIMAGE_NT_HEADERS)((DWORD)pszBufferA + pHeadA->e_lfanew);
	if (pNtA->Signature != IMAGE_NT_SIGNATURE)
	{
		delete pszBufferA;
		CloseHandle(hFileA);
		return false;
	}
	delete pszBufferA;
	CloseHandle(hFileA);
	return true;
}

DWORD CftpUpdatePackerDlg::rva2Offset( DWORD Rva, DWORD dwSectionCount, PIMAGE_SECTION_HEADER pSections )
{
	//Translate the Relative Virtual Address to Raw File Offset  
	ULONG ulDiffer = 0;
	for (DWORD i = 0; i < dwSectionCount; i++ )  
	{
		if( (Rva >= pSections[i].VirtualAddress) && (Rva < (pSections[i].VirtualAddress + pSections[i].Misc.VirtualSize)) )      
		{
			// indicate in the this section. 
			ulDiffer = pSections[i].VirtualAddress - pSections[i].PointerToRawData;
			break;
		}
	}
	return (Rva - ulDiffer);
}

PIMAGE_SECTION_HEADER CftpUpdatePackerDlg::findCodeSection( PIMAGE_SECTION_HEADER pFirstSection, int nSectionCount, DWORD dwAddressOfEntry )
{

	PIMAGE_SECTION_HEADER pSectionHeader = pFirstSection;
	PIMAGE_SECTION_HEADER pCodeSection = NULL;

	for(int i = 0; i < nSectionCount; i++)
	{
		if( dwAddressOfEntry >= pSectionHeader->VirtualAddress && 
			dwAddressOfEntry < (pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize) )
		{
			pCodeSection = pSectionHeader;
			break;
		}

		pSectionHeader ++;
	}
	return pCodeSection;
}
void CftpUpdatePackerDlg::OnBnClickedButtonTest()
{
	UpdateData(TRUE);
	SetDlgItemText(IDC_PRO_TEXT, "FTP testing...");
	HINTERNET	hInet;
	HINTERNET	hFtp =NULL;
	hFtp=myGetFtpConnection(hInet);
	if(hFtp!=NULL)
	{
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInet);
		SetDlgItemText(IDC_PRO_TEXT, "FTP connect success!");
	}	
}

void CftpUpdatePackerDlg::OnEnChangeEditUser()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CftpUpdatePackerDlg::SaveConfig()
{

	TCHAR iniFile[MAX_PATH]={0};
	TCHAR szWriteBuff[MAX_PATH]={0};
	BOOL bGet;
	CString csApp="APP";

	wsprintf(iniFile, _T("%s%s"),m_strAppPath, CONFIG_FILE);


	bGet=IsDlgButtonChecked(IDC_CHECK_FTPMODE);
	wsprintf(szWriteBuff, _T("%d"), bGet);
	::WritePrivateProfileString(csApp, _T("ftpmode"), szWriteBuff, iniFile);


	CString strGet;
	GetDlgItemText(IDC_EDIT_USER, strGet);
	::WritePrivateProfileString(csApp, _T("user"), strGet, iniFile);


	GetDlgItemText(IDC_EDIT_PASS, strGet);
	::WritePrivateProfileString(csApp, _T("pass"), strGet, iniFile);

	GetDlgItemText(IDC_EDIT_FTP_DIR, strGet);
	::WritePrivateProfileString(csApp, _T("ftpdir"), strGet, iniFile);


	GetDlgItemText(IDC_EDIT_FTP_DOMAIN, strGet);
	::WritePrivateProfileString(csApp, _T("ftpdomain"), strGet, iniFile);

	GetDlgItemText(IDC_EDIT_FTP_IP, strGet);
	::WritePrivateProfileString(csApp, _T("ftpip"), strGet, iniFile);

}

// iniConfig read
void CftpUpdatePackerDlg::ReadConfig()
{
	TCHAR iniFile[MAX_PATH]={0};
	TCHAR szWriteBuff[MAX_PATH]={0};
	CString csApp="APP";
	wsprintf(iniFile, _T("%s%s"),m_strAppPath, CONFIG_FILE);

	int iGet;


	iGet=GetPrivateProfileInt(csApp, _T("ftpmode"), 1, iniFile);
	::SendMessage(GetDlgItem(IDC_CHECK_FTPMODE)->GetSafeHwnd(),BM_SETCHECK,iGet,0);

	 
	 GetPrivateProfileString(csApp, _T("user"), "update", szWriteBuff, MAX_PATH, iniFile );
	SetDlgItemText(IDC_EDIT_USER, szWriteBuff); 

	GetPrivateProfileString(csApp, _T("pass"), "nd123456", szWriteBuff, MAX_PATH, iniFile );
	SetDlgItemText(IDC_EDIT_PASS, szWriteBuff); 

	GetPrivateProfileString(csApp, _T("ftpdir"), "/newpptshell/update", szWriteBuff, MAX_PATH, iniFile );
	SetDlgItemText(IDC_EDIT_FTP_DIR, szWriteBuff); 


	GetPrivateProfileString(csApp, _T("ftpdomain"), "p.101.com", szWriteBuff, MAX_PATH, iniFile );
	SetDlgItemText(IDC_EDIT_FTP_DOMAIN, szWriteBuff); 
	
	GetPrivateProfileString(csApp, _T("ftpip"), "61.160.40.166", szWriteBuff, MAX_PATH, iniFile );
	SetDlgItemText(IDC_EDIT_FTP_IP, szWriteBuff); 

}
void CftpUpdatePackerDlg::OnBnClickedButtonOpenConf()
{
	CString strFile;
	strFile.Format(" %s",m_strConfigDir);
	ShellExecute(NULL,"open","explorer.exe",(LPCTSTR)strFile,NULL,true);
}

void CftpUpdatePackerDlg::OnBnClickedButtonOpenVer()
{
	CString strFile;
	strFile.Format(" %s",m_strVerRootDir);
	ShellExecute(NULL,"open","explorer.exe",(LPCTSTR)strFile,NULL,true);
}

void CftpUpdatePackerDlg::OnBnClickedButtonOpenGen()
{
	CString strFile;
	strFile.Format(" %s",m_strPackageDir);
	ShellExecute(NULL,"open","explorer.exe",(LPCTSTR)strFile,NULL,true);
}


void CftpUpdatePackerDlg::OnBnClickedButtonShowLog()
{
	ShellExecute(NULL,"open","http://p.101.com/101PPT/version.php",NULL,NULL,true);
}

void CftpUpdatePackerDlg::OnBnClickedButtonSave()
{
	SaveConfig();
}

void CftpUpdatePackerDlg::OnStnClickedStaticVersion()
{
	// TODO: Add your control notification handler code here
}
