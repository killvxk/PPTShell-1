// CrashCollector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "process.h"
#include "CrashCollector.h"
#include "7z.h"
#include "Util.h"
#include "HttpClient.h"
#include "afxinet.h"
#include "resource.h"

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"" ) // 设置入口地址 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;


unsigned __stdcall UploadThread(void* pParam);
tstring GetLocalPath();
//BOOL UploadFile(LPCTSTR strURL, LPCTSTR strLocalFileName);
//BOOL UploadGzFile(LPCTSTR strURL, LPCTSTR strLocalFileName);
CString MakeRequestHeaders(CString &strBoundary);//包头
CString MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID);
CString MakePostFileData(CString &strBoundary);//发送请求包
BOOL SendTrack(TCHAR* szDestFile);//发送数据

HANDLE hUploadEvent;
	
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		hUploadEvent = CreateEvent(NULL, FALSE, FALSE, _T("UploadEvent"));
		unsigned threadid;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, UploadThread, argv, 0, &threadid);
		CloseHandle(hThread);

		int result = MessageBox(NULL, _T("抱歉，系统检测到您有异常退出,是否尝试修复？"), _T("101教育PPT - 修复提示"), MB_YESNO | MB_ICONQUESTION);
		if (result == 6)	//yes
		{
			
			tstring strUpdatePath = GetLocalPath();
			strUpdatePath += _T("\\Update.exe");

			tstring strVerinfoPath = GetLocalPath();
			strVerinfoPath += _T("\\version.dat");
			DeleteFile(strVerinfoPath.c_str());

			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			si.wShowWindow = SW_SHOW;

			if (CreateProcess(strUpdatePath.c_str(),NULL,NULL,NULL,FALSE,0,NULL,GetLocalPath().c_str(),&si,&pi))
			{
				CloseHandle(pi.hThread);
			}
		}  

		WaitForSingleObject(hUploadEvent, INFINITE);

	}

	return nRetCode;
}


unsigned __stdcall UploadThread(void* pParam)
{
	try
	{

		TCHAR* (*argv)[] = (TCHAR* (*)[])pParam;
		TCHAR* pFileName = (*argv)[0];
		//OutputDebugString(_T("dump文件名"));
		//OutputDebugString(pFileName);

		//版本号
		TCHAR szPath[MAX_PATH];
		_stprintf_s(szPath, _T("%s\\version.dat"), GetLocalPath().c_str());

		FILE* fp = NULL;
		errno_t err = _tfopen_s(&fp, szPath, _T("rb"));
		if( err != 0 )
		{
			SetEvent(hUploadEvent);
			return FALSE;
		}

		char szVersionCode[MAX_PATH] = {0};
		fread(szVersionCode, 1, MAX_PATH, fp);
		fclose(fp);

		//用户ID
		TCHAR* pUserName = (*argv)[1];
		//OutputDebugString(_T("用户名"));
		//OutputDebugString(pUserName);

		//ip地址及mac地址
		char   ipAddress[200] = {0};
		char   mac[200] = {0};
		getIP(ipAddress);
		getMAC(mac);

		//时间
		SYSTEMTIME st;
		GetLocalTime(&st); 

		TCHAR Systime[100];
		wsprintf(Systime, _T("%d-%d-%d-%d-%d-%d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		tstring pUploadFileName = GetLocalPath();
		pUploadFileName += _T("\\Dump\\");
		pUploadFileName += Systime;
		pUploadFileName += _T(".txt");
		//创建文件
		HANDLE hFile = CreateFile(pUploadFileName.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(INVALID_HANDLE_VALUE == hFile)
		{
			SetEvent(hUploadEvent);
			return -1;
		}


		DWORD dwSize = 0;
		WriteFile(hFile, "软件版本号：  ", strlen("软件版本号：  "), &dwSize, NULL);
		WriteFile(hFile, szVersionCode, strlen(szVersionCode), &dwSize, NULL);
		WriteFile(hFile, "\r\n", strlen("\r\n"), &dwSize, NULL);

		WriteFile(hFile, "用户ID：  ", strlen("用户ID：  "), &dwSize, NULL);
		if (pUserName != NULL)
		{
			char pUserNameA[100] = {0};
			WideCharToMultiByte (CP_OEMCP,NULL,pUserName,-1,pUserNameA,100,NULL,FALSE);
			WriteFile(hFile, pUserNameA, strlen(pUserNameA), &dwSize, NULL);
			//OutputDebugStringA("用户名为：");
			//OutputDebugStringA(pUserNameA);
			//WriteFile(hFile, pUserName, wcslen(pUserName), &dwSize, NULL);
		}
		WriteFile(hFile, "\r\n", strlen("\r\n"), &dwSize, NULL);

		WriteFile(hFile, "IP地址：  ", strlen("IP地址：  "), &dwSize, NULL);
		WriteFile(hFile, ipAddress, strlen(ipAddress), &dwSize, NULL);
		WriteFile(hFile, "   ", strlen("   "), &dwSize, NULL);
		WriteFile(hFile, "MAC地址：   ", strlen("MAC地址：   "), &dwSize, NULL);
		WriteFile(hFile, mac, strlen(mac), &dwSize, NULL);
		WriteFile(hFile, "\r\n", strlen("\r\n"), &dwSize, NULL);

		WriteFile(hFile, "系统时间：  ", strlen("系统时间：  "), &dwSize, NULL);
		char SystimeA[100] = {0};
		WideCharToMultiByte (CP_OEMCP,NULL,Systime,-1,SystimeA,100,NULL,FALSE);
		WriteFile(hFile, SystimeA, strlen(SystimeA), &dwSize, NULL);
		//WriteFile(hFile, Systime, wcslen(Systime)*2, &dwSize, NULL);
		//TCHAR szFileName[MAX_PATH];
		//wsprintf(szFileName, pFileName);

		CloseHandle(hFile);

		tstring basePah = GetLocalPath();
		basePah += _T("\\Dump\\");

		char basePahA[MAX_PATH] = {0};
		WideCharToMultiByte (CP_OEMCP,NULL,basePah.c_str(),-1,basePahA,MAX_PATH,NULL,FALSE);

		char szDestFile[MAX_PATH] = {0};
		sprintf(szDestFile, "%s%s%s", basePahA, SystimeA, ".7z");

		char pUploadFileNameA[MAX_PATH] = {0};
		sprintf(pUploadFileNameA, "%s%s", SystimeA, ".txt");

		char* pcszSrcFileOrDirectory[2] = {0};
		pcszSrcFileOrDirectory[0] = pUploadFileNameA;

		if (pFileName != NULL)
		{
			char pFileNameA[MAX_PATH*2];
			WideCharToMultiByte (CP_OEMCP,NULL,pFileName,-1,pFileNameA,MAX_PATH*2,NULL,FALSE);
			pcszSrcFileOrDirectory[1] = pFileNameA;
			//OutputDebugStringA("dmp文件名");
			//OutputDebugStringA(pFileNameA);
		}
		//pcszSrcFileOrDirectory[1] = "2016-24-1959-37-Beta201602031819.dmp";
		//OutputDebugStringA("压缩文件名");
		//OutputDebugStringA(szDestFile);
		//OutputDebugStringA("将要压缩文件1");
		//OutputDebugStringA(pcszSrcFileOrDirectory[0]);
		//OutputDebugStringA("将要压缩文件2");
		//OutputDebugStringA(pcszSrcFileOrDirectory[1]);
		SevenZipCompressItems(szDestFile, basePahA, pcszSrcFileOrDirectory, 2, NULL);

		TCHAR szDestFileW[MAX_PATH*2] = {0};
		MultiByteToWideChar(CP_ACP, 0, szDestFile, -1, szDestFileW, MAX_PATH * 2 * sizeof(wchar_t));
		//OutputDebugStringA("上传宽字符压缩文件名");
		//OutputDebugString(szDestFileW);
		SendTrack(szDestFileW);

		DeleteFile(szDestFileW);		//删除.7z文件
		DeleteFile(pUploadFileName.c_str());	//删除.txt文件
		
		SetEvent(hUploadEvent);
		return 1;
	}
	catch (CException* e)
	{
		SetEvent(hUploadEvent);
	}

	return 1;
}


CString MakeRequestHeaders(CString &strBoundary)//包头
{
	CString strFormat;
	CString strData;

	strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");//二进制文件传送Content-Type类型为: multipart/form-data

	strData.Format(strFormat, strBoundary);
	return strData;
}

CString MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID)
{
	/**/////////////////////////////////////////////////////////////////////////////////
	//Content-Type:
	//JPG image/pjpeg
	//PNG image/x-png
	//BMP image/bmp
	//TIF image/tiff
	//GIF image/gif
	CString strFormat;
	CString strData;

	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"file\"; filename=\"%s\"");//文件地址信息
	strFormat += _T("\r\n");
	strFormat += _T("Content-Type: application/octet-stream");

	strFormat += _T("\r\n\r\n");
	strData.Format(strFormat, strBoundary, strFileName);//

	return strData;
}

CString MakePostFileData(CString &strBoundary)//发送请求包
{
	CString strFormat;
	CString strData;

	strFormat = _T("\r\n");	
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"submitted\"");
	strFormat += _T("\r\n\r\n");
	strFormat += _T("hello");
	strFormat += _T("\r\n");
	strFormat += _T("--%s--");
	strFormat += _T("\r\n");


	strData.Format(strFormat, strBoundary, strBoundary);

	return strData;
}

BOOL SendTrack(TCHAR* szDestFile)//发送数据
{
	CString _mFilePath;

	//_mFilePath = _T("E:\\PPTShell正式版\\bin\\Debug开发版\\Dump\\2016-2-16-15-22-24.7z");//要传的本地文件地址
	_mFilePath = szDestFile;//要传的本地文件地址
	int startp = _mFilePath.ReverseFind('\\');
	int namelen = _mFilePath.GetLength()-startp-1;

	CString pcmname = _mFilePath.Mid(startp+1,namelen);

	CString defServerName =_T("p.101.com");//服务器名http://p.101.com/101ppt/errorLogPC.php 
	CString defObjectName =_T("/101ppt/errorLogPC.php");//保存的地址


	USES_CONVERSION;
	CInternetSession Session;
	CHttpConnection *pHttpConnection = NULL;
	INTERNET_PORT   nPort = 80;
	CFile fTrack;
	CHttpFile* pHTTP;
	CString strHTTPBoundary;
	CString strPreFileData;
	CString strPostFileData;
	DWORD dwTotalRequestLength;
	DWORD dwChunkLength;
	DWORD dwReadLength;
	DWORD dwResponseLength;
	TCHAR szError[MAX_PATH];
	void* pBuffer;
	LPSTR szResponse;
	CString strResponse;
	BOOL bSuccess = TRUE;

	CString strDebugMessage;

	if (FALSE == fTrack.Open(_mFilePath, CFile::modeRead | CFile::shareDenyWrite))//读出文件 
	{
		//AfxMessageBox(_T("Unable to open the file."));
		_tprintf(_T("Unable to open the file."));
		return FALSE;
	}

	int iRecordID = 1;
	strHTTPBoundary = _T("---------------------------7b4a6d158c9");//定义边界值
	strPreFileData = MakePreFileData(strHTTPBoundary, pcmname, iRecordID);
	strPostFileData = MakePostFileData(strHTTPBoundary);

	dwTotalRequestLength = strPreFileData.GetLength() + strPostFileData.GetLength() + fTrack.GetLength();//计算整个包的总长度

	dwChunkLength = 64 * 1024;

	pBuffer = malloc(dwChunkLength);

	if (NULL == pBuffer)
	{
		return FALSE;
	}

	try
	{
		pHttpConnection = Session.GetHttpConnection(defServerName,nPort);
		pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, defObjectName);
		pHTTP->AddRequestHeaders(MakeRequestHeaders(strHTTPBoundary));//发送包头请求
		pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);

#ifdef _UNICODE
		pHTTP->Write(W2A(strPreFileData), strPreFileData.GetLength());
#else
		pHTTP->Write((LPSTR)(LPCSTR)strPreFileData, strPreFileData.GetLength());//写入服务器所需信息
#endif

		dwReadLength = -1;
		while (0 != dwReadLength)
		{
			strDebugMessage.Format(_T("%u / %u\n"), fTrack.GetPosition(), fTrack.GetLength());
			TRACE(strDebugMessage);
			dwReadLength = fTrack.Read(pBuffer, dwChunkLength);//文件内容
			if (0 != dwReadLength)
			{
				pHTTP->Write(pBuffer, dwReadLength);//写入服务器本地文件，用二进制进行传送
			}
		}

#ifdef _UNICODE
		pHTTP->Write(W2A(strPostFileData), strPostFileData.GetLength());
#else
		pHTTP->Write((LPSTR)(LPCSTR)strPostFileData, strPostFileData.GetLength());
#endif

		pHTTP->EndRequest(HSR_SYNC);

		dwResponseLength = pHTTP->GetLength();
		while (0 != dwResponseLength)
		{
			szResponse = (LPSTR)malloc(dwResponseLength + 1);
			szResponse[dwResponseLength] = '\0';
			pHTTP->Read(szResponse, dwResponseLength);
			strResponse += szResponse;
			free(szResponse);
			dwResponseLength = pHTTP->GetLength();
		}



	} 
	catch (CException* e)
	{
		e->GetErrorMessage(szError, MAX_PATH);
		e->Delete();
		//AfxMessageBox(szError);
		//printf(_T("%s"), szError);
		_tprintf(szError);
		bSuccess = FALSE;
	}

	pHTTP->Close();
	delete pHTTP;

	fTrack.Close();

	if (NULL != pBuffer)
	{
		free(pBuffer);
	}
	return bSuccess;
}


tstring GetLocalPath()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	TCHAR* p = _tcsrchr(szPath, '\\');
	*p = '\0';
	p = _tcsrchr(szPath, '\\');
	*p = '\0';
	
	return szPath;
}

