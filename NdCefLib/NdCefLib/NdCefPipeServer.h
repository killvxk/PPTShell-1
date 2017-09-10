#pragma once
#include "stdafx.h"
//#include "Utils.h"

#define MAX_BUFFER_SIZE							16384

class INetworkListener 
{
public:
	virtual void onReceive(CHAR* data, INT len){};
};

class CNdCefPipeServer
{
public:
	CNdCefPipeServer(LPCSTR szPipeName, INetworkListener *pNetworkListener);
	~CNdCefPipeServer();


public:
	BOOL Run();
	void Stop();
	virtual void Release();
	BOOL connected(){ return isConnected; }

protected:
	virtual BOOL Create();

	virtual int Receive(char* buffer);

	BOOL			isStopped = FALSE;
	BOOL			isConnected = FALSE;


private:
	LPCSTR m_szPipeName;
	HANDLE m_hPipe;
	INetworkListener *m_pNetworkListener;
};

