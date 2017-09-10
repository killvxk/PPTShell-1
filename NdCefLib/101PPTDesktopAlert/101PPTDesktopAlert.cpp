// 101PPTDesktopAlert.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "101PPTDesktopAlert.h"
#include "NdUI.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	NdUI::CNdUI nMainUI;
	nMainUI.NdMain(hInstance,nCmdShow);
}