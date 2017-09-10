// NdCefClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "NdCefClient.h"
#include "include/cef_app.h"
#include "include/base/cef_scoped_ptr.h"
#include "NdDebug.h"
#include "NdCefMain.h"

using namespace NdCef;

scoped_ptr<CNdCefMain> nCefMain;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!nCefMain.get()){
		nCefMain.reset(new CNdCefMain());
		nCefMain.get()->RunMain(false);
		//CefRefPtr<CNdCefHandler> g_hNdCefHandle;
		//nCefMain.get()->ShowMain(NULL);
		//nCefMain.get()->MsgRun();

	};
	return 0;
}
