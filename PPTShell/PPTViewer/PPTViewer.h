
// PPTViewer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CPPTViewerApp:
// 有关此类的实现，请参阅 PPTViewer.cpp
//

class CPPTViewerApp : public CWinAppEx
{
public:
	CPPTViewerApp();

// 重写
	public:
	virtual BOOL InitInstance();

	BOOL ExtractResource(char* szPath, int nResourceID, char* szResourceType);
	void ExtractDlls(char* szDestDir);
	void ExtractMainfests(char* szDestDir);
	void ExtractPptFile(char* szDestDir);
	void WritePlayListFile(char* szDestDir);
	void RunPPTViewer(char* szDestDir);

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CPPTViewerApp theApp;