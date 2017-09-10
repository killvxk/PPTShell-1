//===========================================================================
// FileName:				BlueTooth.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BlueTooth.h"
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <BluetoothAPIs.h>
#include "EventCenter/EventDefine.h"

#pragma comment ( lib, "Irprops.lib")
#pragma comment(lib, "Bthprops.lib")

CBlueToothMonitor::CBlueToothMonitor()
{
	m_bScanLocalRadios		= FALSE;
	m_bRunning				= TRUE;
	m_hThread				= NULL;
}

CBlueToothMonitor::~CBlueToothMonitor()
{
	m_bRunning	= FALSE;
	//WaitForSingleObject(m_hThread, 1000);
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
}

BOOL CBlueToothMonitor::Initialize()
{
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorThread, this, 0, NULL);
	return TRUE;
}

BOOL CBlueToothMonitor::Destory()
{
	return TRUE;
}

void CBlueToothMonitor::StartScanLocalRadios()
{
	m_bScanLocalRadios = TRUE;
}

void CBlueToothMonitor::StopScanLocalRadios()
{
	m_bScanLocalRadios = FALSE;
}

DWORD WINAPI CBlueToothMonitor::MonitorThread(LPARAM lParam)
{
	CBlueToothMonitor* pThis = (CBlueToothMonitor*)lParam;
	pThis->MonitorBthDevice();

	return 0;
}

void CBlueToothMonitor::MonitorBthDevice()
{
	while(m_bRunning)
	{
		vector<tstring> vecRadios;
		vecRadios.clear();

		if( m_bScanLocalRadios )
		{
			EnumerateLocalRadios(vecRadios);

			tstring strRadio = _T("");
			if( vecRadios.size() != 0 )
				strRadio = vecRadios[0];

			if( m_strFirstRadioAddress != strRadio )
			{
				m_strFirstRadioAddress = strRadio;
				BroadcastEvent(EVT_BLUETOOTH_CHANGED, 0, 0, NULL);	
			}
		}

		Sleep(1000);
	}
}

//
// Get first bluetooth device address
//
tstring CBlueToothMonitor::GetFirstBthDeviceAddress()
{
	return m_strFirstRadioAddress;
}


//
// Enumerate local radios
//
BOOL CBlueToothMonitor::EnumerateLocalRadios(vector<tstring>& vecRadioAddresses)
{
	HANDLE hRadio = NULL;

	BLUETOOTH_FIND_RADIO_PARAMS params;
	params.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&params, &hRadio);
	if( hFind == NULL )
		return FALSE;

	while( hRadio != NULL )
	{
		BLUETOOTH_RADIO_INFO RadioInfo;
		RadioInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

		DWORD ret = BluetoothGetRadioInfo(hRadio, &RadioInfo);
		if( ret == ERROR_SUCCESS )
		{
			// enable discovery and incoming connections of this radio
			BluetoothEnableDiscovery(hRadio, TRUE);
			BluetoothEnableIncomingConnections(hRadio, TRUE);

			tstring strAddress = FormatBthAddress(RadioInfo.address.rgBytes);
			vecRadioAddresses.push_back(strAddress);
		}
		
		BOOL res = BluetoothFindNextRadio(hFind, &hRadio);
		if( !res )
			break;

	}

	BluetoothFindRadioClose(hFind);
	return TRUE;
}

//
// Format bluetooth address
//
tstring CBlueToothMonitor::FormatBthAddress(BYTE *pBthAddress)
{
	tstring strAddress;

	for(int i = 0; i < 6; i++)
	{
		TCHAR szByte[16];
		_stprintf_s(szByte, _T("%.2x"), pBthAddress[6-i-1]);

		if( strAddress != _T("") )
			strAddress += _T(":");

		strAddress += szByte;
	}

	return strAddress;
}