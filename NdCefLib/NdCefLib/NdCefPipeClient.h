#pragma once

#include <string>

class CNdCefPipeClient
{
public:
	CNdCefPipeClient();
	~CNdCefPipeClient();

	BOOL Connect(std::string requestId);
	BOOL Send(const CHAR* pszRequest, DWORD cbRequestBytes);
	BOOL Recv(std::string &reply);
	BOOL IsConnected(){ return connected; }
	BOOL Disconnect();

private:
	BOOL connected;
	HANDLE hPipe;
};

