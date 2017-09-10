//===========================================================================
// FileName:				BlueTooth.h
// 
// Desc:				
//============================================================================
#ifndef _BLUE_TOOTH_MONITOR
#define _BLUE_TOOTH_MONITOR

#include "Util/Singleton.h"

class CBlueToothMonitor
{
private:
	CBlueToothMonitor();
	~CBlueToothMonitor();

public:
	BOOL			Initialize();
	BOOL			Destory();

	void			StartScanLocalRadios();
	void			StopScanLocalRadios();

	tstring			GetFirstBthDeviceAddress();
	BOOL			ScanNearbyBthDevice();
	BOOL			RequestAuthenticateDevice();
	
	DECLARE_SINGLETON_CLASS(CBlueToothMonitor);

protected:
	static DWORD WINAPI MonitorThread(LPARAM lParam);

	BOOL			EnumerateLocalRadios(vector<tstring>& vecRadioAddresses);
	void			MonitorBthDevice();
	tstring			FormatBthAddress(BYTE* pBthAddress);

protected:
	BOOL			m_bRunning;
	HANDLE			m_hThread;
	tstring			m_strFirstRadioAddress;
	BOOL			m_bScanLocalRadios;

};

typedef Singleton<CBlueToothMonitor> BlueToothMonitor;

#endif