//====================================================================
// FileName:			HWID.h
//
// Desc:
//===================================================================

#include "WinIoCtl.h"
enum
{
	// 0 - 15
	eHWType_CPUID				= 1 << 0,
	eHWType_HardDrive			= 1 << 1,
	 

	
	// 16 - 31
	eSystemType_ComputerName	= 1 << 16,
	eSystemType_ProductID		= 1 << 17,
	eSystemType_CDiskSerial		= 1 << 18,
	eSystemType_BiosVersion		= 1 << 19,
	eSystemType_MAC				= 1 << 20,
	
};

#define		HWID_XOR_KEY				0xE900FF15

#define		DFP_GET_VERSION				0x00074080
#define		FILE_DEVICE_SCSI			0x0000001b
#define		IOCTL_SCSI_MINIPORT			0x0004D008  //  see NTDDSCSI.H for definition

#define		IDENTIFY_BUFFER_SIZE		512
#define		IDE_ATAPI_IDENTIFY			0xA1  //  Returns ID sector for ATAPI.
#define		IDE_ATA_IDENTIFY			0xEC  //  Returns ID sector for ATA.
#define		DFP_RECEIVE_DRIVE_DATA		0x0007c088

#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ( ( FILE_DEVICE_SCSI << 16 ) + 0x0501 )
#define  SENDIDLENGTH  ( sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE )


typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;


typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

//////////////////////////////////////////////////////////////////////

#ifndef _NATIVE_API_H_
typedef struct _UNICODE_STRING 
{ 
	USHORT  Length;  
	USHORT  MaximumLength; 
	PWSTR  Buffer; 
} UNICODE_STRING,*PUNICODE_STRING;


typedef struct _OBJECT_ATTRIBUTES 
{ 
	ULONG Length; 
	HANDLE RootDirectory; 
	PUNICODE_STRING ObjectName; 
	ULONG Attributes; 
	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR£¬0 
	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE£¬0 
} OBJECT_ATTRIBUTES; 
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES; 
#endif
 
typedef DWORD  (__stdcall *ZWOS )( PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES); 
typedef DWORD  (__stdcall *ZWMV )( HANDLE,HANDLE,PVOID,ULONG,ULONG,PLARGE_INTEGER,PSIZE_T,DWORD,ULONG,ULONG); 
typedef DWORD  (__stdcall *ZWUMV )( HANDLE,PVOID); 

//=========================================================================================================
//
//
class cHWID
{
public:
	cHWID();
	~cHWID(); 

	bool getHardwareIDString(char* pOutBuf, DWORD dwHWType = 0xffffffff );
	bool getSystemIDString(char* pOutBuf, DWORD dwSystemInfoType = 0xffffffff );

	DWORD getPcInstallTime(DWORD dwXorKey);
	DWORD getHdiskNum(DWORD dwXorKey);
	// system os relative infos
	int obtainComputerName(BYTE* pOutBuf);
	int obtainComputerName2(char cKey, BYTE* pOutBuf);

protected:
	// hardware relative infos
	int obtainCPUID(BYTE* pOutBuf); 
	int obtainHardDiskID(BYTE* pOutBuf);
	

	// system os relative infos

	int obtainCDiskSerial(BYTE* pOutBuf);
	int obtainBiosVersion(BYTE* pOutBuf);
	int obtainProductID(BYTE* pOutBuf);
	int obtainMAC(BYTE* pOutBuf);

	// helper functions
	bool IsPhysicalLocalMac(char* szDesc);
	bool isVistaWin7OS();
	BOOL WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen );
	BOOL WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen );
	BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned );
	

};