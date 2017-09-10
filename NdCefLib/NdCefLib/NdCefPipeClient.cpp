#include "stdafx.h"
#include "NdCefPipeClient.h"

#define BUFSIZE							16384

CNdCefPipeClient::CNdCefPipeClient()
{
	hPipe = INVALID_HANDLE_VALUE;
}


CNdCefPipeClient::~CNdCefPipeClient()
{

}

BOOL CNdCefPipeClient::Connect(std::string requestId)
{
	//LPTSTR lpvMessage = TEXT("Default message from client.");
	BOOL   fSuccess = FALSE;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\ndcef");

	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			return -1;
		}

		// All pipe instances are busy, so wait for 2 seconds. 

		if (!WaitNamedPipe(lpszPipename, 2000))
		{
			//printf("Could not open pipe: 2 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		//_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return FALSE;
	}
	if (Send(requestId.c_str(), requestId.length())){
		std::string resp;
		if (Recv(resp) && resp == "OK"){
			connected = TRUE;
			fSuccess = TRUE;
		}

	}
	return fSuccess;
}

BOOL CNdCefPipeClient::Send(const CHAR* pszRequest, DWORD cbRequestBytes)
{
	DWORD cbWritten;
	BOOL fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		pszRequest,             // message 
		cbRequestBytes,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess || cbRequestBytes != cbWritten)
	{
		//_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		Disconnect();
		connected = FALSE;
		return -1;
	}
	return fSuccess;
}

BOOL CNdCefPipeClient::Recv(std::string &reply)
{
	BOOL fSuccess;
	CHAR  chBuf[BUFSIZE + 1];
	DWORD  cbRead;
	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE*sizeof(CHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;
		chBuf[cbRead] = '\0';
		reply.append(chBuf);
		//_tprintf(TEXT("\"%s\"\n"), chBuf);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		//_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		Disconnect();
		connected = FALSE;
		return -1;
	}
	return fSuccess;
}

BOOL CNdCefPipeClient::Disconnect()
{
	if (hPipe != INVALID_HANDLE_VALUE)
		CloseHandle(hPipe);
	hPipe = INVALID_HANDLE_VALUE;
	connected = false;
	return TRUE;
}

