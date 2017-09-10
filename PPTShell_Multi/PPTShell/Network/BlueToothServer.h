//===========================================================================
// FileName:				BlueToothServer.h
// 
// Desc:				
//============================================================================
#ifndef _BLUE_TOOTH_SERVER_H_
#define _BLUE_TOOTH_SERVER_H_

#include "Util/Singleton.h"

class CBlueToothServer : public CBaseServer
{
private:
	CBlueToothServer();
	~CBlueToothServer();

public:
	virtual SOCKET	CreateServerSocket();
	virtual int		BindServerSocket();
	virtual int		SetServerService();
	virtual int 	AcceptConnection();

	DECLARE_SINGLETON_CLASS(CBlueToothServer);

protected:

};

typedef Singleton<CBlueToothServer> BlueToothServer;

#endif