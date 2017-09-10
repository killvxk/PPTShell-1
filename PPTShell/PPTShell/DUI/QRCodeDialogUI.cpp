#include "StdAfx.h"
#include "QRCodeDialogUI.h"
#include "QRCode/QRCodeImageGenerator.h"
#include "EventCenter/EventDefine.h"
#include "BlueTooth/BlueTooth.h"

CQRCodeDialogUI::CQRCodeDialogUI(void)
{
	::OnEvent(EVT_BLUETOOTH_CHANGED, MakeEventDelegate(this, &CQRCodeDialogUI::OnBlueToothChanged));
}

CQRCodeDialogUI::~CQRCodeDialogUI(void)
{
	
}

LPCTSTR CQRCodeDialogUI::GetWindowClassName( void ) const
{
	return _T("QRCodeDialog");
}

DuiLib::CDuiString CQRCodeDialogUI::GetSkinFile()
{
	return _T("QRCode\\QRCode.xml");
}

DuiLib::CDuiString CQRCodeDialogUI::GetSkinFolder()
{
	return _T("skins");
}


void CQRCodeDialogUI::InitWindow()
{
	// start scan local blue tooth radios
	BlueToothMonitor::GetInstance()->StartScanLocalRadios();

	// generate qrcode image
	TCHAR szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	TCHAR szTempFileName[MAX_PATH];
	_stprintf_s(szTempFileName, _T("%s\\PPTShellQRCode_%08lX.png"), szTempPath, GetTickCount());
	
	CQRCodeImageGenerator generator;
	generator.GenQRCodeImageToFile(szTempFileName);

	CControlUI* pImageLabel	= static_cast<CControlUI*>(m_PaintManager.FindControl(_T("QRCodeImage")));
	pImageLabel->SetBkImage(szTempFileName);



}

void CQRCodeDialogUI::OnBtnClose(TNotifyUI& msg)
{
	Close();
}

bool CQRCodeDialogUI::OnBlueToothChanged(void* param)
{
	TEventNotify* pNotify = (TEventNotify*)param;

	// generate qrcode image
	TCHAR szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	TCHAR szTempFileName[MAX_PATH];
	_stprintf_s(szTempFileName, _T("%s\\PPTShellQRCode_%08lX.png"), szTempPath, GetTickCount());

	CQRCodeImageGenerator generator;
	generator.GenQRCodeImageToFile(szTempFileName);

	CControlUI* pImageLabel	= static_cast<CControlUI*>(m_PaintManager.FindControl(_T("QRCodeImage")));
	pImageLabel->SetBkImage(szTempFileName);

	return true;
}

void CQRCodeDialogUI::OnFinalMessage( HWND hWnd )
{
	// start scan local blue tooth radios
	BlueToothMonitor::GetInstance()->StopScanLocalRadios();

	::CancelEvent(EVT_BLUETOOTH_CHANGED, MakeEventDelegate(this, &CQRCodeDialogUI::OnBlueToothChanged));
}