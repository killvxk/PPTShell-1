//====================================================================
// FileName:			HWID.cpp
//
// Desc:
//===================================================================
#include "stdafx.h"

#include <Windows.h>
#include <stdio.h>
#include <iphlpapi.h>
#include <string>
#include "HWID.h"
#include "Base64.h"



#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"advapi32.lib")

cHWID::cHWID()
{

}

cHWID::~cHWID()
{

}

//
// get hardware id string
//
bool cHWID::getHardwareIDString(char* pOutBuf, DWORD dwHWType)
{
	int sizeOfCpuID		= 0;
	int sizeOfHDSerial  = 0;

	BYTE cpuid[16];
	memset(cpuid, 0, 16);

	BYTE hdSerialNo[MAX_PATH] = {0};
	
	if( pOutBuf == NULL )
		return false;

	// cpuid
	if( dwHWType & eHWType_CPUID )
		sizeOfCpuID = obtainCPUID(cpuid);
	

	// harddisk serial no
	if( dwHWType & eHWType_HardDrive )
		sizeOfHDSerial = obtainHardDiskID(hdSerialNo);


	// base64
	BYTE temp[1024];
	int total = 0;

	if(sizeOfCpuID )
	{
		memcpy(temp, cpuid, sizeOfCpuID);
		total += sizeOfCpuID;
	}
	
	if( sizeOfHDSerial )
	{
		memcpy(temp+total, hdSerialNo, sizeOfHDSerial);
		total += sizeOfHDSerial;
	}

	string strOut;
	CBase64::Encode(temp, total, strOut);
	
	strcpy(pOutBuf, strOut.c_str());
	return true;

}

//
// get system id string
//
bool cHWID::getSystemIDString(char* pOutBuf, DWORD dwSystemInfoType)
{
	char szComputerName[MAX_COMPUTERNAME_LENGTH+1]		= {0};
	char szCDiskSerial[MAX_PATH]		= {0};
	char szBiosVersion[MAX_PATH]		= {0};
	char szProductId[MAX_PATH]			= {0};
	char szMac[MAX_PATH]				= {0};
 
	int sizeOfComputerName  = 0;
	int sizeOfCDiskSerial   = 0;
	int sizeOfBiosVersion	= 0;
	int sizeOfProductId		= 0;
	int sizeOfMAC		    = 0;

	if( pOutBuf == NULL )
		return false;


	//暂时取消
	// computer name
	//if( dwSystemInfoType & eSystemType_ComputerName )
	//	sizeOfComputerName = obtainComputerName((BYTE*)szComputerName);

	//暂时取消
	// C Disk serial
	if( dwSystemInfoType & eSystemType_CDiskSerial )
		sizeOfCDiskSerial = obtainCDiskSerial((BYTE*)szCDiskSerial);

	// Bios version
	//if( dwSystemInfoType & eSystemType_BiosVersion )
	//	sizeOfBiosVersion = obtainBiosVersion((BYTE*)szBiosVersion);

	// Product id
	if( dwSystemInfoType & eSystemType_ProductID )
		sizeOfProductId = obtainProductID((BYTE*)szProductId);

	//暂时取消
	// mac
	//if( dwSystemInfoType & eSystemType_MAC )
	//	sizeOfMAC = obtainMAC((BYTE*)szMac);


	BYTE temp[1024] = {0};
	int total = 0;

	if( sizeOfComputerName )
	{
		memcpy(temp, szComputerName, sizeOfComputerName);
		total += sizeOfComputerName;
	}

	if( sizeOfCDiskSerial )
	{
		memcpy(temp+total, szCDiskSerial, sizeOfCDiskSerial);
		total += sizeOfCDiskSerial;
	}

	if( sizeOfBiosVersion )
	{
		memcpy(temp+total, szBiosVersion, sizeOfBiosVersion);
		total += sizeOfBiosVersion;
	}

	if( sizeOfProductId )
	{
		memcpy(temp+total, szProductId, sizeOfProductId);
		total += sizeOfProductId;
	}


	if( sizeOfMAC )
	{
		memcpy(temp+total, szMac, sizeOfMAC); 
		total += sizeOfMAC;
	}

	// base64
	string strOut;
	CBase64::Encode(temp, total, strOut);

	strcpy(pOutBuf, strOut.c_str());
	return true;
}


//----------------------------------------------------------------------------------------------------
// obtain CPUID
//
int cHWID::obtainCPUID(BYTE* pOutBuf)
{
	DWORD dwEax;
	DWORD dwEbx;
	DWORD dwEcx;
	DWORD dwEdx;

	BOOL bException = FALSE;

	__try 
	{
		_asm
		{
			mov eax, 1
			cpuid
			mov dwEax, eax
			mov dwEbx, ebx
			mov dwEcx, ecx
			mov dwEdx, edx
		}

		dwEbx &= 0x000FFFFF;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bException = TRUE;
	}


	if( !bException )
	{
		if( pOutBuf )
		{
			memcpy(pOutBuf, &dwEax, 4);
			memcpy(pOutBuf+4, &dwEbx, 4);
			memcpy(pOutBuf+8, &dwEcx, 4);
			memcpy(pOutBuf+12, &dwEdx, 4);
		}

		return 16;
	}
	
	return 0;
	
}

//----------------------------------------------------------------------------------------------------
// obtain physic network adapter MAC
//

bool cHWID::IsPhysicalLocalMac( char* szDesc )
{
	HKEY hKEY;

	//"\\Connection"
	char szTempConn[] = {'\\','\\','C','o','n','n','e','c','t','i','o','n','\0'};
	//"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\";
	char szTempKey[] = {'S','Y','S','T','E','M','\\','\\','C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\','\\','C','o','n','t','r','o','l','\\','\\','N','e','t','w','o','r','k','\\','\\','{','4','D','3','6','E','9','7','2','-','E','3','2','5','-','1','1','C','E','-','B','F','C','1','-','0','8','0','0','2','B','E','1','0','3','1','8','}','\\','\\','\0'};
	
	std::string strKey;
	strKey = szTempKey;
	strKey += std::string(szDesc);
	strKey += szTempConn;
	LONG nRet = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey.c_str(), 0, KEY_READ|KEY_WOW64_64KEY, &hKEY );

	if ( nRet != ERROR_SUCCESS )
		return false;
	

	TCHAR szData[256] = { 0 };
	DWORD dwType = REG_SZ;
	DWORD dwDataSize = 256;

	char szTempPn[]="PnpInstanceID";
	nRet = ::RegQueryValueEx( hKEY, szTempPn, NULL, &dwType, (LPBYTE)szData, &dwDataSize );
	::RegCloseKey(hKEY);
	if ( nRet != ERROR_SUCCESS )          
		return false;
 
	char szTmpPCI[]="PCI";
	if ( !strnicmp(szData, szTmpPCI, 3) )         
		return true;

	else
		return false;
	
}


int cHWID::obtainMAC(BYTE* pOutBuf)
{
	DWORD Err;

	PIP_ADAPTER_INFO pAdapterInfo, pAdapt;
	DWORD AdapterInfoSize;
//	PIP_ADDR_STRING pAddrStr;

	AdapterInfoSize = 0;
	if ((Err = GetAdaptersInfo(NULL, &AdapterInfoSize)) != 0)
	{
		if (Err != ERROR_BUFFER_OVERFLOW)
			return 0;

	}

	// Allocate memory from sizing information
	if ((pAdapterInfo = (PIP_ADAPTER_INFO) GlobalAlloc(GPTR, AdapterInfoSize)) == NULL)
		return 0; 


	// Get actual adapter information
	if ((Err = GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) != 0)
		return 0;


	pAdapt = pAdapterInfo;	 

	int totalBytes = 0;
	while (pAdapt)
	{
		if(  IsPhysicalLocalMac( pAdapt->AdapterName ) )
		 {
			 for(UINT i = 0; i < pAdapt->AddressLength; i++)
			 {
				 if( pOutBuf )
					 pOutBuf[i] = pAdapt->Address[i];

				 totalBytes ++;
			 }
		 }

		 pAdapt = pAdapt->Next;
	}

	GlobalFree(pAdapterInfo);
	return totalBytes;
}

//----------------------------------------------------------------------------------------------------
// obtain hard disk serial no
//
int cHWID::obtainHardDiskID(BYTE* pOutBuf)
{
	UINT SerialLen = 0;

	OSVERSIONINFO ovi = { 0 };
	ovi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &ovi );

	if( ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
	{
		// Only Windows 2000, Windows XP, Windows Server 2003...
		return 0;
	}
	else
	{
		if( !WinNTHDSerialNumAsPhysicalRead( pOutBuf, &SerialLen, 1024 ) )
		{
			WinNTHDSerialNumAsScsiRead( pOutBuf, &SerialLen, 1024 );
		}
		
	}

	return SerialLen;
}


BOOL cHWID::WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{

	BOOL bInfoLoaded = FALSE;

	for( UINT uDrive = 0; uDrive < 4; ++ uDrive )
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char szDriveName [256]={0};
		char szTempPhy[]="\\\\.\\PhysicalDrive%d";
		sprintf( szDriveName, szTempPhy, uDrive );

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile( szDriveName, GENERIC_READ | GENERIC_WRITE, 
										FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
										OPEN_EXISTING, 0, NULL);

		if( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
		{
			GETVERSIONOUTPARAMS VersionParams = { 0 };
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			if( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
								NULL, 
								0,
								&VersionParams,
								sizeof( GETVERSIONOUTPARAMS ),
								&cbBytesReturned, NULL ) )
			{
				// If there is a IDE device at number "i" issue commands
				// to the device
				if( VersionParams.bIDEDeviceMap != 0 )
				{
					BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
					SENDCMDINPARAMS  scip = { 0 };

					// Now, get the ID sector for all IDE devices in the system.
					// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
					// otherwise use the IDE_ATA_IDENTIFY command
					bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
					BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };

					if( DoIdentify( hPhysicalDriveIOCTL, 
						&scip, 
						( PSENDCMDOUTPARAMS )&IdOutCmd, 
						( BYTE )bIDCmd,
						( BYTE )uDrive,
						&cbBytesReturned ) )
					{
						if( *puSerialLen + 20 <= uMaxSerialLen )
						{
							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 );  // 序列号

							// Cut off the trailing blanks
							UINT i = 20;
							for( i = 20; i != 0 && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
							*puSerialLen += i;

							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // 型号

							// Cut off the trailing blanks
							for( i = 40; i != 0 && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
							*puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							CloseHandle( hPhysicalDriveIOCTL );
							return bInfoLoaded;
						}
					}
				}
			}

			CloseHandle( hPhysicalDriveIOCTL );
		}
	}

	return bInfoLoaded;
}


BOOL cHWID::WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
	BOOL bInfoLoaded = FALSE;

	for( int iController = 0; iController < 4; ++ iController )
	{
		HANDLE hScsiDriveIOCTL = 0;
		char   szDriveName[256];

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char szTempSci[]="\\\\.\\Scsi%d:";
		sprintf( szDriveName, szTempSci, iController );

		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFile( szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);


		if( hScsiDriveIOCTL != INVALID_HANDLE_VALUE )
		{
			int iDrive = 0;
			for( iDrive = 0; iDrive < 16; ++ iDrive )
			{
				char szBuffer[sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH] = { 0 };

				SRB_IO_CONTROL* p = ( SRB_IO_CONTROL* )szBuffer;
				SENDCMDINPARAMS* pin = ( SENDCMDINPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
				DWORD dwResult;

				p->HeaderLength = sizeof( SRB_IO_CONTROL );
				p->Timeout = 10000;
				p->Length = SENDIDLENGTH;
				p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				char szTmpSCSI[]="SCSIDISK";
				strncpy( ( char* )p->Signature, szTmpSCSI, 8 );

				pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin->bDriveNumber = iDrive;

				if( DeviceIoControl( hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					szBuffer,
					sizeof( SRB_IO_CONTROL ) + sizeof( SENDCMDINPARAMS ) - 1,
					szBuffer,
					sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH,
					&dwResult, NULL ) )
				{
					SENDCMDOUTPARAMS* pOut = ( SENDCMDOUTPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
					IDSECTOR* pId = ( IDSECTOR* )( pOut->bBuffer );
					if( pId->sModelNumber[0] )
					{
						if( * puSerialLen + 20U <= uMaxSerialLen )
						{
							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 10, 20 );		// 序列号

							// Cut off the trailing blanks
							UINT i = 20;
							for( i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i ) {}
							*puSerialLen += i;

							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 27, 40 );		// 型号

							// Cut off the trailing blanks
							for(i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )	{}
							*puSerialLen += i;

							bInfoLoaded = TRUE;
						} 
						else
						{
							::CloseHandle( hScsiDriveIOCTL );
							return bInfoLoaded;
						}
					}
				}
			}
			::CloseHandle( hScsiDriveIOCTL );
		}
	}
	return bInfoLoaded;
}

BOOL cHWID::DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				PDWORD lpcbBytesReturned )
{
	// Set up data structures for IDENTIFY command.
	pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg     = 0;
	pSCIP->irDriveRegs.bSectorCountReg  = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg       = 0;
	pSCIP->irDriveRegs.bCylHighReg      = 0;

	// calc the drive number.
	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );

	// The command can either be IDE identify or ATAPI identify.
	pSCIP->irDriveRegs.bCommandReg = bIDCmd;
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		( LPVOID ) pSCIP,
		sizeof( SENDCMDINPARAMS ) - 1,
		( LPVOID ) pSCOP,
		sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL );
}

int cHWID::obtainComputerName2(char cKey, BYTE* pOutBuf)
{
	char name[MAX_COMPUTERNAME_LENGTH+1] = {0};
	DWORD size = MAX_COMPUTERNAME_LENGTH+1;
	GetComputerName(name, &size);
	strcpy((char*)pOutBuf, name);


	for(int i=0;i<MAX_COMPUTERNAME_LENGTH;i++)
	{
		*(pOutBuf+i)=*(pOutBuf+i)^(i+1)^0xF^cKey;
	}

	return size;
}

//----------------------------------------------------------------------------------------------------
// obtain computer name
// 
int cHWID::obtainComputerName(BYTE* pOutBuf)
{
	char name[MAX_COMPUTERNAME_LENGTH+1] = {0};
	DWORD size = MAX_COMPUTERNAME_LENGTH+1;
	GetComputerName(name, &size);
	strcpy((char*)pOutBuf, name);

	return size;
}


// 
// obtain c drive serial
//
int cHWID::obtainCDiskSerial(BYTE* pOutBuf)
{
	//加vm会奔溃
	DWORD SerialNumber = 0;
	BOOL ret = GetVolumeInformationW(L"C:\\", NULL, 0, (LPDWORD)pOutBuf, 0, NULL, NULL, 0);

	if( !ret )
		return 0;

	return 4;
}

DWORD cHWID::getHdiskNum(DWORD dwXorKey)
{
	DWORD dwRetNum=0;
	DWORD SerialNumberC = 0x123456;
	DWORD SerialNumberD =0x123457;
	BOOL bRet=GetVolumeInformationW(L"C:\\", NULL, 0, &SerialNumberC, 0, NULL, NULL, 0);
	bRet=GetVolumeInformationW(L"D:\\", NULL, 0, &SerialNumberD, 0, NULL, NULL, 0);
	dwRetNum=SerialNumberC^SerialNumberD^dwXorKey^HWID_XOR_KEY;

	return dwRetNum;

}

//
// obtain bios version
//
int cHWID::obtainBiosVersion(BYTE* pOutBuf)
{
	HKEY hKey=0;
	char szTemsy[]="\0\0HARDWARE\\DESCRIPTION\\System";

	DWORD wd =KEY_READ|KEY_WOW64_64KEY;
	LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTemsy+2, 0, wd, &hKey);
	if( ret != ERROR_SUCCESS )
		return 0;

	DWORD dwSize = 0;
	char szSysB[]="SystemBiosVersion";
	ret = RegQueryValueEx(hKey, szSysB, 0, 0, 0, &dwSize);
	if( ret != ERROR_SUCCESS )
		return 0;

	BYTE* pData = new BYTE[dwSize+1];
	if( pData == NULL )
		return 0;

	ret = RegQueryValueEx(hKey, szSysB, 0, 0, pData, &dwSize);
	if( ret != ERROR_SUCCESS )
	{
		delete pData;
		return 0;
	}

	memcpy(pOutBuf, pData, dwSize);
	delete pData;
	RegCloseKey(hKey);
	return dwSize;
}

//
// obtain product id
//
int cHWID::obtainProductID(BYTE* pOutBuf)
{
	HKEY hKey;
	BOOL ret = 0;
	bool bVistaWin7 = isVistaWin7OS();

	char szT1[]="\0\0SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
	char szT2[]="\0\0SOFTWARE\\Microsoft\\Windows\\CurrentVersion";
	if( bVistaWin7 )
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szT1+2, 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY/*0x20019*/, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szT2+2, 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY/*0x20019*/, &hKey);

	if( ret != ERROR_SUCCESS )
		return 0;


	char szTp[]="ProductId";
	DWORD dwSize = 0;
	ret = RegQueryValueEx(hKey, szTp, 0, 0, NULL, &dwSize);
	if( ret != ERROR_SUCCESS )
		return 0;

	BYTE* pData = new BYTE[dwSize+1];
	if( pData == NULL )
		return 0;

	ret = RegQueryValueEx(hKey, szTp, 0, 0, pData, &dwSize);
	if( ret != ERROR_SUCCESS )
	{
		delete pData;
		return 0;
	}
	 
	RegCloseKey(hKey);

	memcpy(pOutBuf, pData, dwSize);
	delete pData;
	return dwSize;
}

//
// check current system version
//
bool cHWID::isVistaWin7OS()
{
	OSVERSIONINFOEX version;
	memset(&version, 0, sizeof(OSVERSIONINFOEX));

	version.dwOSVersionInfoSize		= sizeof(OSVERSIONINFOEX);
	version.dwMajorVersion			= 6;
	version.dwMinorVersion			= 0;

	DWORDLONG dwlConditionMask = 0;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

	if( VerifyVersionInfo(&version, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask) )
		return true;

	return false;
}

