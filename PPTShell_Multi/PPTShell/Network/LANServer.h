//===========================================================================
// FileName:				WifiServer.h
// 
// Desc:				
//============================================================================
#ifndef _WIFI_SERVER_H_
#define _WIFI_SERVER_H_

#include "Util/Singleton.h"
#include "BaseServer.h"

class CLANServer : public CBaseServer
{
private:
	CLANServer();
	~CLANServer();

public:
	virtual SOCKET	CreateServerSocket();
	virtual int		BindServerSocket();
	virtual int		SetServerService();
	virtual int 	AcceptConnection();

	DECLARE_SINGLETON_CLASS(CLANServer);

protected:

};

typedef Singleton<CLANServer> LANServer;

#endif