//===========================================================================
// FileName:					Packet.h
//	
// Desc:					Packet defination
//===========================================================================
#ifndef _PACKET_H_
#define _PACKET_H_

#define MAX_PACKET_SIZE		(9*1024)

#define ACTION_DISCONNECT	0xfffa		//断开连接
#define PT_HEARTBEAT_RQST		0xfffb			//心跳包
#define PT_HEARTBEAT_ACK			0xfffc			//心跳包 应答
#define PT_CLOSE_MOBILE		0xfffd
#define PT_REGISTER			0xfffe
#define PT_TRANSFER			0xffff

enum EClientType
{
	CTYPE_NONE	= 0,
	CTYPE_MOBILE,
	CTYPE_PPTSHELL,
};


typedef struct PACKET_HEAD
{
	DWORD		dwSize;
	DWORD		dwMajorType;
	DWORD		dwMinorType;

} PACKET_HEAD, *PPACKET_HEAD;



typedef struct PACKET_REGISTER : PACKET_HEAD
{
	DWORD		dwClientType;				// 0: none 1: mobile / 2: PPTShell
	DWORD		dwMobileID;					// for mobile ID
	DWORD		dwPPTShellID;				// for PPTShell

} PACKET_REGISTER, *PPACKET_REGISTER;

struct WSAOVERLAPPED_EX : WSAOVERLAPPED
{
	char		op;			// 0: send 1:recv
	WSABUF		wsaBuf;
	char*		buffer;
	int			size;
	int			sizeTransferred;
};

void DeleteOverlapEx(WSAOVERLAPPED_EX* pOverlap);

#endif