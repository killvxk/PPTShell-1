#include "stdafx.h"
#include "CNdCefPipeServer.h"
#include <vector>
using namespace std;


CNdCefPipeServer::CNdCefPipeServer(LPCSTR szPipeName, INetworkListener *pNetworkListener)
{
	this->m_szPipeName = szPipeName;
	this->m_pNetworkListener = pNetworkListener;
}


CNdCefPipeServer::~CNdCefPipeServer()
{
}

BOOL CNdCefPipeServer::Create()
{
	m_hPipe = CreateNamedPipeA(
		m_szPipeName,             // pipe name 
		PIPE_ACCESS_DUPLEX,       // read/write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_READMODE_MESSAGE |   // message-read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		MAX_BUFFER_SIZE,                  // output buffer size 
		MAX_BUFFER_SIZE,                  // input buffer size 
		0,                        // client time-out 
		NULL);                    // default security attribute 

	BOOL fConnected = ConnectNamedPipe(m_hPipe, NULL) ?
		TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	return fConnected;
}



int CNdCefPipeServer::Receive(char* buffer)
{
	DWORD cbBytesRead = 0;
	BOOL fSuccess = ReadFile(
		m_hPipe,        // handle to pipe 
		buffer,    // buffer to receive data 
		MAX_BUFFER_SIZE, // size of buffer 
		&cbBytesRead, // number of bytes read 
		NULL);        // not overlapped I/O 
	if (!fSuccess || cbBytesRead == 0)
		return -1;

	return (int)cbBytesRead;
}

void CNdCefPipeServer::Release()
{
	FlushFileBuffers(m_hPipe);
	DisconnectNamedPipe(m_hPipe);
	CloseHandle(m_hPipe);
}

BOOL CNdCefPipeServer::Run()
{
	int             iCxnCount = 0;
	UINT            iLengthReceived = 0;
	BOOL			result;
	vector< CHAR > vecChar;

	if (m_pNetworkListener == NULL)
		return FALSE;

	//PRINTF("Creating...\n");
	result = Create();
	if (!result){		
		return FALSE;
	}

	isConnected = TRUE;

	//
	// Read data from the incoming stream
	//
	BOOL bContinue = TRUE;
	HANDLE hHeap = GetProcessHeap();
	CHAR* tempBuffer = (CHAR*)HeapAlloc(hHeap, 0, MAX_BUFFER_SIZE * sizeof(CHAR));
	if (tempBuffer == NULL){
		return FALSE;
	}
	//CHAR tempBuffer[MAX_BUFFER_SIZE];
	// 要处理粘包和拆包, 开头 #!， 结尾!#

	while (!isStopped && bContinue) {
		memset(tempBuffer, 0, MAX_BUFFER_SIZE);
		iLengthReceived = Receive(tempBuffer);
		//printf("\nReceived: %d bytes)\n", iLengthReceived);
		switch (iLengthReceived) {
		case 0: 
		case -1:
			bContinue = FALSE;
			result = FALSE;
			break;

		default:
				if (vecChar.size() >= 2 && vecChar[0] == '#' && vecChar[1] == '+'){
					if (iLengthReceived >= 2 && tempBuffer[0] == '#' && tempBuffer[1] == '+'){
						vecChar.clear();
					}
				}

				for (size_t i = 0; i < iLengthReceived; i++)
				{
					vecChar.push_back(tempBuffer[i]);
				}
				if (vecChar.size() > 4)
				{						
					int a = vecChar[2] & 0xFF;
					int b = vecChar[3] & 0xFF;
					int  dataLen =  a << 8;
					dataLen |= b;

					while (vecChar.size() >= (dataLen + 4))
					{
						for (size_t i = 0; i < dataLen; i++)
						{
							tempBuffer[i] = vecChar[i + 4];
						}
						vecChar.erase(vecChar.begin(), vecChar.begin() + dataLen + 4);
						m_pNetworkListener->onReceive(tempBuffer, dataLen);
						if (vecChar.size() > 4)
						{								
							a = vecChar[2] & 0xFF;
							b = vecChar[3] & 0xFF;
							dataLen = a << 8;
							dataLen |= b;
						}
					}
				}
			//}
			break;
		}

		if (!result) {
			break;
		}
	}

	Release();

	return result;
}

void CNdCefPipeServer::Stop(){
	isStopped = TRUE;
}