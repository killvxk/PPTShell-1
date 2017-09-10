//===========================================================================
// FileName:				BlueToothClient.h
// 
// Desc:				
//============================================================================
#ifndef _WIFI_CLIENT_H_
#define _WIFI_CLIENT_H_

class CBlueToothClient : public CBaseClient
{
public:
	CBlueToothClient();
	~CBlueToothClient();

	virtual BOOL  Initialize(SOCKET clientSocket, SOCKADDR* clientAddr);

protected:
	SOCKADDR_BTH				m_ClientAdress;
};

#endif