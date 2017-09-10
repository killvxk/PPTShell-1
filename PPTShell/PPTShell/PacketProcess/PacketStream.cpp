//===========================================================================
// FileName:				PacketStream.cpp
//	
// Desc:					 
//============================================================================
#include "stdafx.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "Packet.h"
#include "PacketStream.h"

CPacketStream::CPacketStream(char* pBuffer /*= NULL*/, int nBufferSize /*= 0*/)
{
	m_pBuffer			= pBuffer;
	m_nBufferSize		= nBufferSize;
	m_nDataSize			= 0;

	memset(m_StreamBuffer, 0, sizeof(m_StreamBuffer));

	if( pBuffer == NULL )
	{
		m_pBuffer = m_StreamBuffer;
		m_nBufferSize = STREAM_BUFFER_SIZE;
	}

}

CPacketStream::~CPacketStream()
{

}

void CPacketStream::PackCommand(DWORD dwMajorType, DWORD dwMinorType)
{
	WriteHeader(dwMajorType, dwMinorType);
	CorrectSize();
}

//-----------------------------------------------------------------
// User actions
//

//
// unpack logon stream
//
void CPacketStream::UnPackUserLogon(DWORD &dwUID, tstring &strName, BOOL &bMobileLogin)
{
	// skip header
	SkipHeader();
	
	// UID | Name 
	dwUID = ReadDWORD();
	string str = ReadCString();
	strName = Utf82Str(str);
	bMobileLogin = ReadBOOL();
}

//
// unpack modify name
//
void CPacketStream::UnPackModifyName(DWORD& dwUID, tstring& strName)
{
	// skip header
	SkipHeader();

	// UID | Name 
	dwUID = ReadDWORD();
	string strTmp = ReadCString();
	strName = Utf82Str(strTmp);
}

//
// unpack modify permission
//
void CPacketStream::UnPackModifyPermission(DWORD& dwUID, DWORD& dwPermission)
{
	// skip header
	SkipHeader();

	// UID | Permission
	dwUID = ReadDWORD();
	dwPermission = ReadDWORD();
}

//
// pack user logon response
//
void CPacketStream::PackUserLogonResponse(BOOL bLeader, DWORD dwPermission)
{
	WriteHeader(ACTION_USER, USER_LOGON);

	// Leader | Permission
	WriteDWORD(bLeader);
	WriteDWORD(dwPermission);

	CorrectSize();
	
}

//
// pack guest logon response
//
void CPacketStream::PackGuestLogonResponse(DWORD dwUID, DWORD dwPermission, tstring strName)
{
	string strTmp = Str2Utf8(strName);

	WriteHeader(ACTION_USER, USER_GUEST_LOGON);

	// UID | Permission | Name
	WriteDWORD(dwUID);
	WriteDWORD(dwPermission);
	WriteString((char*)strTmp.c_str());

	CorrectSize();
}

//
// pack guest modify name 
//
void CPacketStream::PackGuestModifyName(DWORD dwUID, tstring strName)
{
	string strTmp = Str2Utf8(strName);

	WriteHeader(ACTION_USER, USER_GUEST_LOGON);

	//
	WriteDWORD(dwUID);
	WriteString((char*)strTmp.c_str());

	CorrectSize();
}


//
// pack guest logoff response
//
void CPacketStream::PackGuestLogoffResponse(DWORD dwUID)
{
	WriteHeader(ACTION_USER, USER_GUEST_LOGOFF);
	WriteDWORD(dwUID);
	CorrectSize();
}

//-----------------------------------------------------------------
// Control actions
//
void CPacketStream::UnPackPPTControl(DWORD& dwControlCommand)
{
	SkipHeader();
	dwControlCommand = ReadDWORD();
}

void CPacketStream::UnPackImageControl(DWORD& dwControlCommand)
{
	SkipHeader();
	dwControlCommand = ReadDWORD();
}

void CPacketStream::UnPack3DControl(DWORD& dwControlCommand)
{
	SkipHeader();
	dwControlCommand = ReadDWORD();
}

void CPacketStream::PackControlResponse(DWORD dwControlCommand, BOOL bSuccess, tstring strErrorDesc)
{
	// ControlCommand | Success | ErrorDesc
	WriteHeader(ACTION_CONTROL, CONTROL_P2M_RESPONSE);
	WriteDWORD(dwControlCommand);
	WriteDWORD(bSuccess);

	if( strErrorDesc != _T("") )
	{
		//strErrorDesc = AnsiToUtf8(strErrorDesc);
		//WriteString(strErrorDesc);
		string strUtf8 = Str2Utf8(strErrorDesc);
		WriteString((char*)strUtf8.c_str());
	}

	CorrectSize();
}

//-----------------------------------------------------------------
// Push actions
//
void CPacketStream::PackPushPPTBasicInfo(tstring strPPTName, DWORD dwPageCount)
{
	// PageCount | PPTName
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_PPTINFO);
	WriteDWORD(dwPageCount);
	
	if( strPPTName != _T("") )
	{
		string strTmp = Str2Utf8(strPPTName);
		WriteString((char*)strTmp.c_str());
	}

	CorrectSize();
}

void CPacketStream::PackPushOutlineInfo(vector<tstring> vecOutline, int nStartIdx, int& nPackedCount)
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_OUTLINE);

	int nTotalSize = 0;
	nPackedCount = 0;

	for(int i = nStartIdx; i < (int)vecOutline.size(); i++)
	{
		tstring str = vecOutline[i];
		string strUtf8 = Str2Utf8(str);

		if( strUtf8 != "" )
		{
			int nLen = strUtf8.length();

			// not enough buffer
			nTotalSize += nLen;
			if( nTotalSize > m_nBufferSize )
				return;

			// Len1|String1|Len2|String2|Len3|String3
			WriteDWORD(nLen);
			Write((char*)strUtf8.c_str(), nLen);

			nPackedCount ++;
		}
	}

	CorrectSize();
}

void CPacketStream::PackPushPCInfo(tstring strComputerName, int nScreenWidth, int nScreenHeight)
{
	// ScreenWidth | nScreenHeight | ComputerName
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_PCINFO);
	WriteDWORD(nScreenWidth);
	WriteDWORD(nScreenHeight);
	
	if(strComputerName != _T("") )
	{
		string strUtf8 = Str2Utf8(strComputerName);
		WriteString((char*)strUtf8.c_str());
	}

	CorrectSize();
}

void CPacketStream::PackPushPlayBegin()
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_PLAY_BEGIN);
	CorrectSize();
}

void CPacketStream::PackPushPlaySlide(int nSlideIdx)
{
	// nSlideIdx
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_PLAY_SLIDE);
	WriteDWORD(nSlideIdx);

	CorrectSize();
}

void CPacketStream::PackPushPlayEnd()
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_PLAY_END);
	CorrectSize();
}

void CPacketStream::PackPushVideoList(int nSlideIdx, std::vector<float> vecVideoList)
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_VIDEO_LIST);
	WriteDWORD(nSlideIdx);

	for(int i = 0; i < (int)vecVideoList.size(); i++)
		WriteFloat(vecVideoList[i]);

	CorrectSize();
}

void CPacketStream::PackPushSlideUpdateStart()
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_UPDATE_PPTIMAGE);
	WriteDWORD(UPDATE_START);
	CorrectSize();
}

void CPacketStream::PackPushSlideUpdatePageCnt(int iPageCnt)
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_UPDATE_PPTIMAGE);
	WriteDWORD(UPDATE_PAGECNT);
	WriteInt(iPageCnt);
	CorrectSize();
}

void CPacketStream::PackPushSlideUpdateNew(int iNewID, int iOldID)
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_UPDATE_PPTIMAGE);
	WriteDWORD(UPDATE_NEW);
	WriteInt(iNewID);
	WriteInt(iOldID);
	CorrectSize();
}

void CPacketStream::PackPushSlideUpdateEnd()
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_UPDATE_PPTIMAGE);
	WriteDWORD(UPDATE_END);
	CorrectSize();
}

void CPacketStream::PackPushPreviewSlideIdx(int iSlideIdx)
{
	WriteHeader(ACTION_PUSH_DATA, PUSH_P2M_PREVIEW_INDEX);
	WriteDWORD(iSlideIdx);
	CorrectSize();
}

void CPacketStream::PackSyncDataPPTChanged()
{
	WriteHeader(ACTION_SYNC_DATA, SYNC_PPT_HAVE_CHANGE);
	CorrectSize();
}

//-----------------------------------------------------------------
// File transfer actions
//
void CPacketStream::UnPackFileHead(tstring& strFileName, DWORD& dwFileSize, DWORD& dwFileCrc, DWORD& dwFileType)
{
	// skip header
	SkipHeader();

	// FileSize | FileCrc | FileType
	dwFileSize  = ReadDWORD();
	dwFileCrc	= ReadDWORD();
	dwFileType  = ReadDWORD();

	strFileName = Utf82Str(ReadCString());

}

void CPacketStream::UnPackFileData(DWORD& dwFileCrc, char* pOutBuffer, DWORD& dwDataSize)
{
	//
	DWORD dwMajorType, dwMinorType, dwSize;
	ReadHeader(dwMajorType, dwMinorType, dwSize);

	dwDataSize	= dwSize - sizeof(PACKET_HEAD) - sizeof(DWORD);

	// FileCrc | FileData
	dwFileCrc = ReadDWORD();
	Read(pOutBuffer, dwDataSize);
}

void CPacketStream::UnPackFileEnd(DWORD& dwFileCrc)
{
	// skip header
	SkipHeader();

	// FileCrc
	dwFileCrc = ReadDWORD();
}

void CPacketStream::UnPackFileCancel(DWORD& dwFileCrc)
{
	// skip header
	SkipHeader();

	// FileCrc
	dwFileCrc = ReadDWORD();
}

void CPacketStream::PackFileHead(TCHAR* szFileName, DWORD dwFileSize, DWORD dwFileCrc, DWORD dwFileType, DWORD dwFileID)
{
	// FileSize | FileCrc | FileType | FileName
	WriteHeader(ACTION_TRANSFER_FILE, TRANSFER_FILE_START);
	WriteDWORD(dwFileSize);
	WriteDWORD(dwFileCrc);
	WriteDWORD(dwFileType);
	WriteDWORD(dwFileID);

	if( szFileName[0] != '\0')
	{
		string strFile = Str2Utf8(szFileName);
		WriteString((char*)strFile.c_str());
	}

	CorrectSize();
}

void CPacketStream::PackFileData(DWORD dwFileCrc, char* pFileData, DWORD dwDataSize)
{
	if( (int)dwDataSize > m_nBufferSize )
		return;

	// FileCrc | FileData
	WriteHeader(ACTION_TRANSFER_FILE, TRANSFER_FILE_DATA);
	WriteDWORD(dwFileCrc);
	Write(pFileData, dwDataSize);

	CorrectSize();
}

void CPacketStream::PackFileEnd(DWORD dwFileCrc, DWORD dwFileID)
{
	// FileCrc
	WriteHeader(ACTION_TRANSFER_FILE, TRANSFER_FILE_END);
	WriteDWORD(dwFileCrc);
	WriteDWORD(dwFileID);

	CorrectSize();
}


//
// correct header size
//
void CPacketStream::CorrectSize()
{
	*(DWORD*)m_pBuffer = m_nCursor;
}

//
// read header
//
void CPacketStream::SkipHeader()
{
	m_nCursor += sizeof(PACKET_HEAD);
}

void CPacketStream::ReadHeader(DWORD& dwMajorType, DWORD& dwMinorType, DWORD& dwSize)
{
	dwSize		= ReadDWORD();
	dwMajorType	= ReadDWORD();
	dwMinorType	= ReadDWORD();
}

//
// write header
//
void CPacketStream::WriteHeader(DWORD dwMajorType, DWORD dwMinorType)
{
	WriteDWORD(0);				// size
	WriteDWORD(dwMajorType);	// major type
	WriteDWORD(dwMinorType);	// minor type
}

// 函数：	PackFileCheckResult
// 参数：	strFileName		- 文件名
//			bCheckCRCResult	- 文件CRC检查结果。0=本地文件不存在；1=本地文件已存在
// 返回：
// 描述：	填写文件CRC检查结果帧，kyp20160125
void CPacketStream::PackFileCheckResult(tstring& strFileName, BOOL bCheckCRCResult)
{
	WriteHeader(ACTION_TRANSFER_FILE, TRANSFER_FILE_CHECK_RESULT);
	WriteBOOL(bCheckCRCResult);

	if(strFileName != _T("") )
	{
		string strUtf8 = Str2Utf8(strFileName);
		WriteString((char*)strUtf8.c_str());
	}

	CorrectSize();
}
// kyp20160125
// 函数：	UnPackFileCRC
// 参数：	strFileName		- 文件名
//			dwFileCRC		- 文件CRC
// 返回：
// 描述：	截取文件CRC检查帧，kyp20160125
void CPacketStream::UnPackFileCRC(tstring& strFileName, DWORD& dwFileCRC)
{
	SkipHeader();

	dwFileCRC = ReadDWORD();
	strFileName = Utf82Str(ReadCString());
}
// 读取放大镜通信的帧长和命令号
void CPacketStream::UnPackMagnifier(DWORD& dwSize, DWORD& dwControlCommand)
{
	DWORD dwMajor, dwMinor;
	ReadHeader(dwMajor, dwMinor, dwSize);

	dwControlCommand = ReadDWORD();
}
// 写入一级、二级、三级命令
void CPacketStream::PackThreeCommand(DWORD dwMajorType, DWORD dwMinorType, DWORD dwThirdCmd)
{
	WriteHeader(dwMajorType, dwMinorType);
	WriteDWORD(dwThirdCmd);

	CorrectSize();
}
// 写入放大镜放大区域百分比
void CPacketStream::PackPushMagnifierRect(float fPerLeft, float fPerTop, float fPerRight, float fPerBottom)
{
	PackThreeCommand(ACTION_PUSH_DATA, PUSH_P2M_MAGNIFIER, MAGNIFIER_P2M_RECT);
	WriteFloat(fPerLeft);
	WriteFloat(fPerTop);
	WriteFloat(fPerRight);
	WriteFloat(fPerBottom);
	CorrectSize();
}

