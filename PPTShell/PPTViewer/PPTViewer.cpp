
// PPTViewer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "PPTViewer.h"
#include "PPTViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPPTViewerApp

BEGIN_MESSAGE_MAP(CPPTViewerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPPTViewerApp 构造

CPPTViewerApp::CPPTViewerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPPTViewerApp 对象

CPPTViewerApp theApp;


// CPPTViewerApp 初始化

BOOL CPPTViewerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	//
	char szDestPath[MAX_PATH];
	char szTempPath[MAX_PATH];

	GetTempPath(MAX_PATH, szTempPath);
	sprintf(szDestPath, "%s\\ppt", szTempPath);

	CreateDirectory(szDestPath, NULL);

	ExtractDlls(szDestPath);
	ExtractMainfests(szDestPath);
	ExtractPptFile(szDestPath);
	WritePlayListFile(szDestPath);
	RunPPTViewer(szDestPath);

	ExitProcess(0);

	CPPTViewerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CPPTViewerApp::ExtractResource(char* szPath, int nResourceID, char* szResourceType)
{
	HRSRC hRsrc = FindResourceA(NULL, MAKEINTRESOURCE(nResourceID), szResourceType);
	if( hRsrc == NULL )
		return FALSE;

	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
	if( hGlobal == NULL )
		return FALSE;

	DWORD dwSize = SizeofResource(NULL, hRsrc);
	if( dwSize == 0 )
		return FALSE;

	char* pBuffer = (char*)LockResource(hGlobal);
	if( pBuffer == NULL )
		return FALSE;

	FILE* fp = fopen(szPath, "wb");
	if( fp == NULL )
		return FALSE;

	fwrite(pBuffer, dwSize, 1, fp);
	fclose(fp);

	UnlockResource(hGlobal);
	return TRUE;
}


void CPPTViewerApp::ExtractDlls(char* szDestDir)
{
	char* szNames[] = 
	{
		"PPTVIEW.EXE",
		"INTLDATE.DLL",
		"msvcm80.dll",
		"msvcp80.dll",
		"MSVCR80.dll",
		"OGL.DLL",
		"PPVWINTL.DLL",
		"SAEXT.DLL",
	};

	for(int i = IDR_DLL_PPTVIEW; i <= IDR_DLL_SAEXT; i++)
	{
		char szPath[MAX_PATH];
		sprintf(szPath, "%s\\%s", szDestDir, szNames[i-IDR_DLL_PPTVIEW]);

		 ExtractResource(szPath, i, "DLL");
	}

}

void CPPTViewerApp::ExtractMainfests(char* szDestDir)
{
	char* szNames[] = 
	{
		"microsoft.vc80.crt.manifest",
		"pptview.exe.manifest",
	};

	for(int i = IDR_TXT_MICRO; i <= IDR_TXT_PPTVIEW; i++)
	{
		char szPath[MAX_PATH];
		sprintf(szPath, "%s\\%s", szDestDir, szNames[i-IDR_TXT_MICRO]);

		ExtractResource(szPath, i, "TXT");
	}

}

void CPPTViewerApp::ExtractPptFile(char* szDestDir)
{
	// extract ppt file
	char szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	
	FILE* fp = fopen(szPath, "rb");
	if( fp == NULL )
		return;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pBuffer = new char[size];
	if( pBuffer == NULL )
		return;

	fread(pBuffer, size, 1, fp);
	fclose(fp);

	DWORD dwPptSize = *(DWORD*)(pBuffer+size-4);
	DWORD dwOffset = size - dwPptSize - 4;

	char szPptFilePath[MAX_PATH];
	sprintf(szPptFilePath, "%s\\test.ppt", szDestDir);

	fp = fopen(szPptFilePath, "wb");
	if( fp == NULL )
		return;

	fwrite(pBuffer+dwOffset, dwPptSize, 1, fp);
	fclose(fp);

}

void CPPTViewerApp::WritePlayListFile(char* szDestDir)
{	
	char szPath[MAX_PATH];
	sprintf(szPath, "%s\\playlist.txt", szDestDir);
	
	FILE* fp = fopen(szPath, "wb");
	if( fp == NULL )
		return;

	char* name = "test.ppt";

	fwrite(name, strlen(name), 1, fp);
	fclose(fp);


}

void CPPTViewerApp::RunPPTViewer(char* szDestDir)
{
	char szPath[MAX_PATH];
	sprintf(szPath, "%s\\PPTVIEW.EXE", szDestDir);

	ShellExecute(NULL, "open", szPath, "/L \"playlist.txt\"", szDestDir, SW_SHOW);
}