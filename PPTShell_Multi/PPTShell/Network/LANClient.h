//===========================================================================
// FileName:				WifiClient.h
// 
// Desc:				
//============================================================================
#ifndef _WIFI_CLIENT_H_
#define _WIFI_CLIENT_H_

class CLANClient : public CBaseClient
{
public:
	CLANClient();
	~CLANClient();

	virtual BOOL  Initialize(SOCKET clientSocket, SOCKADDR* clientAddr);

protected:
	SOCKADDR_IN				m_ClientAdress;
};

#endif