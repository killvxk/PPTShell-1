#include "stdafx.h"
#include "HttpFileEx.h"


CHttpFileEx::CHttpFileEx()
{
	m_lpUTF8 = NULL;		//UTF8编码缓存指针
	m_lpMultiByte = NULL;		//
	m_wpWideChar = NULL;		//Widechar编码缓存指针
	//服务器默认值
	m_strServerName = "";
	m_strObjectName = "";
	m_nPort = INTERNET_DEFAULT_HTTP_PORT;
	m_strBoundary = _T("-------------2389jhrwfjsjd9823");
}

CHttpFileEx::~CHttpFileEx()
{
	//如果原先的数据没有删除
	if (m_lpUTF8)
	{
		delete [] m_lpUTF8;
		m_lpUTF8 = NULL;
	}
	//如果原先的数据没有删除
	if (m_lpMultiByte)
	{
		delete [] m_lpMultiByte;
		m_lpMultiByte = NULL;
	}
	//宽字节代码指针
	if (m_wpWideChar)
	{
		delete []m_wpWideChar;
		m_wpWideChar = NULL;
	}
}

//设置服务器参数
void CHttpFileEx::SetServerParam(CString strServerName, CString strObjectName, WORD Port)
{
	m_strServerName = strServerName;
	m_strObjectName = strObjectName;
	m_nPort = Port;
}

//发送数据
//编码参数：1表示GB2312，2表示UTF8
//返回错误代码列表：
//100：正常成功
//101：服务器无法连接
//102：提交页面无法打开
//103：数据发送失败
//104：服务器处理失败
//500：异常错误
DWORD CHttpFileEx::PostDataMethod(LPCTSTR strDataSend, int DataSize, LPTSTR *pRecvData, DWORD &RecvSize,int SendCode,int RecvCode)	
{
	CInternetSession	InternetSession;
	CHttpConnection		*pHttp = NULL;
	CHttpFile			*pFile = NULL;
	DWORD				nRetCode	= 100;	//返回值
	TCHAR				tzErrMsg[MAX_PATH] = {0};	//	捕捉异常
	CString				strResponse;	//接收的数据

	try
	{
		pHttp = InternetSession.GetHttpConnection(m_strServerName,m_nPort);
		if (pHttp == NULL)
		{
			return 101;
		}
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_POST,m_strObjectName,NULL);
		if (pFile == NULL)
		{
			return 102;
		}
		CString	strHeader = _T("Content-Type: application/x-www-form-urlencoded");
		//////////////////////////////////////////////////////////////////////////
		//发送时采用GB2312
		if (SendCode == 1)
		{
#ifdef UNICODE
			WideToMulti((LPCWSTR)(LPCTSTR)strDataSend,DataSize);
#else
			MultiToMulti((LPCSTR)(LPCTSTR)strDataSend,DataSize);				
#endif
			//发送
			if (pFile->SendRequest(strHeader,m_lpMultiByte,m_nMultiByteLen) == FALSE)
			{
				return 103;
			}
		}
		//发送时选择UTF8
		else if (SendCode == 2)
		{
#ifdef UNICODE
			WideCharToUTF8((LPCWSTR)(LPCTSTR)strDataSend,DataSize);
#else
			MultiByteToUTF8((LPCSTR)(LPCTSTR)strDataSend,DataSize);
#endif
			//发送
			if (pFile->SendRequest(strHeader,m_lpUTF8,m_nUTF8Len) == FALSE)
			{
				return 103;
			}
		}

		//查询状态
		DWORD	dwStatusCode = 0;
		pFile->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode != 200)
		{
			return 104;
		}

		//////////////////////////////////////////////////////////////////////////
		//接收数据
		char	szRead[1024] = {0};
		while((pFile->Read(szRead,1023)) > 0)
		{
			strResponse += szRead;
		}
		//转换接收的数据
		//接收采用GB2312编码
		if (RecvCode == 1)
		{
#ifdef UNICODE
			MultiToWide(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_wpWideChar;
			RecvSize = m_nWideCharLen;
#else
			MultiToMulti(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_lpMultiByte;
			RecvSize = m_nMultiByteLen;
#endif
		}
		//接收采用UTF8编码
		else if (RecvCode == 2)
		{
#ifdef UNICODE
			UTF8ToWideChar(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_wpWideChar;
			RecvSize = m_nWideCharLen;
#else
			UTF8ToMultiByte(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_lpMultiByte;
			RecvSize = m_nMultiByteLen;
#endif
		}
		//返回值
		nRetCode = 100;
	}
	//捕获异常
	catch (CInternetException* e)
	{
		e->GetErrorMessage(tzErrMsg,sizeof(tzErrMsg));
		e->Delete();
		nRetCode = 500;
	}

	//////////////////////////////////////////////////////////////////////////
	//关闭
	pFile->Close();
	delete	pFile;
	pHttp->Close();
	delete pHttp;
	InternetSession.Close();

	return nRetCode;
}
//提交图片，自己构造数据包
//99：图片不存在
//100：正常成功
//101：服务器无法连接
//102：提交页面无法打开
//103：数据发送失败
//104：服务器处理失败
//500：异常错误
DWORD CHttpFileEx::PostPicDataMethod(CString strLocalPath,LPCTSTR strFormName,LPTSTR *pRecvData, DWORD &RecvSize,int SendCode ,int RecvCode)
{
	DWORD	dwRetCode = 100;
	DWORD	dwPostSize = 0;		//要传送的数据大小
	DWORD	dwPicSize	= 0;	//图片大小
	DWORD	dwPostEndSize = 0;	//协议尾部数据长度

	CInternetSession	InternetSession;
	CHttpConnection		*pHttp = NULL;
	CHttpFile			*pFile = NULL;
	TCHAR				tzErrMsg[MAX_PATH] = {0};	//	捕捉异常
	CString				strResponse;	//接收的数据

	//获取图片大小
	dwPicSize = GetExistPic(strLocalPath);

	//构造数据包
	MakeHeaderData();
	MakePostPicData(strFormName,strLocalPath);	//在原有数据包后面加上图片前缀包
	MakeEndData();
	//数据包构造完毕

	try
	{
		pHttp = InternetSession.GetHttpConnection(m_strServerName,m_nPort);
		if (pHttp == NULL)
		{
			return 101;
		}
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_POST,m_strObjectName,NULL);
		if (pFile == NULL)
		{
			return 102;
		}
		pFile->AddRequestHeaders(m_strHeaderData);
		//////////////////////////////////////////////////////////////////////////
		//发送时采用GB2312
		if (SendCode == 1)
		{
#ifdef UNICODE
			WideToMulti((LPCWSTR)(LPCTSTR)m_strPostData,m_strPostData.GetLength());
			//计算尾部数据长度
			dwPostEndSize = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength(),NULL,0,NULL,NULL);
#else
			MultiToMulti((LPCSTR)(LPCTSTR)m_strPostData,m_strPostData.GetLength());		
			dwPostEndSize = m_strEndData.GetLength();
#endif
			//由于协议尾部只有英文数据，在unicode、UTF8、ASCII编码下，
			//英文数据转换后长度不变，因此可直接加上长度。
			//获取尾部数据转为UTF8后的长度
			dwPostSize = dwPicSize + m_nMultiByteLen + dwPostEndSize;
			//发送，告知数据总长度
			pFile->SendRequestEx(dwPostSize);
			pFile->Write(m_lpMultiByte,m_nMultiByteLen);
		}
		//发送时选择UTF8
		else if (SendCode == 2)
		{
#ifdef UNICODE
			WideCharToUTF8((LPCWSTR)(LPCTSTR)m_strPostData,m_strPostData.GetLength());
			dwPostEndSize = WideCharToMultiByte(CP_UTF8,0,(LPCWSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength(),NULL,0,NULL,NULL);
#else
			//这个麻烦，要转两道，直接用函数转算了。
			MultiByteToUTF8((LPCSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength());
			dwPostEndSize = m_nUTF8Len;

			MultiByteToUTF8((LPCSTR)(LPCTSTR)m_strPostData,m_strPostData.GetLength());
#endif
			dwPostSize = dwPicSize + m_nUTF8Len + dwPostEndSize;
			//发送
			pFile->SendRequestEx(dwPostSize);
			pFile->Write(m_lpUTF8,m_nUTF8Len);
		}
		//////////////////////////////////////////////////////////////////////////
		//发送图片
		//缓存大小
		int	BuffSize = 8 * 1024;
		if (dwPicSize != 0)
		{
			char *pbuff = new char[BuffSize];
			CFile	cFile;
			UINT	ReadLength = -1;

			if (pbuff != NULL)
			{
				if (cFile.Open(strLocalPath,CFile::modeRead|CFile::shareDenyWrite,NULL))
				{
					while(ReadLength !=0)
					{
						ReadLength = cFile.Read(pbuff,BuffSize);
						if (ReadLength != 0)
						{
							pFile->Write(pbuff,ReadLength);
						}
					}

					cFile.Close();
				}
				//删除堆
				delete []pbuff;
				pbuff = NULL;
			}

		}
		//////////////////////////////////////////////////////////////////////////
		//发送协议尾部
		//发送时采用GB2312
		if (SendCode == 1)
		{
#ifdef UNICODE
			WideToMulti((LPCWSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength());
#else
			MultiToMulti((LPCSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength());				
#endif
			pFile->Write(m_lpMultiByte,m_nMultiByteLen);
		}
		//发送时选择UTF8
		else if (SendCode == 2)
		{
#ifdef UNICODE
			WideCharToUTF8((LPCWSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength());
#else
			MultiByteToUTF8((LPCSTR)(LPCTSTR)m_strEndData,m_strEndData.GetLength());
#endif
			pFile->Write(m_lpUTF8,m_nUTF8Len);
		}


		//发送完毕
		pFile->EndRequest(HSR_SYNC);
		//查询状态
		DWORD	dwStatusCode = 0;
		pFile->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode != 200)
		{
			return 104;
		}

		//////////////////////////////////////////////////////////////////////////
		//接收数据
		char	szRead[1024] = {0};
		while((pFile->Read(szRead,1023)) > 0)
		{
			strResponse += szRead;
		}
		//转换接收的数据
		//接收采用GB2312编码
		if (RecvCode == 1)
		{
#ifdef UNICODE
			MultiToWide(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_wpWideChar;
			RecvSize = m_nWideCharLen;
#else
			MultiToMulti(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_lpMultiByte;
			RecvSize = m_nMultiByteLen;
#endif
		}
		//接收采用UTF8编码
		else if (RecvCode == 2)
		{
#ifdef UNICODE
			UTF8ToWideChar(strResponse.c_str(),strResponse.length());
			*pRecvData = (LPTSTR)m_wpWideChar;
			RecvSize = m_nWideCharLen;
#else
			UTF8ToMultiByte(strResponse,strResponse.GetLength());
			*pRecvData = (LPTSTR)m_lpMultiByte;
			RecvSize = m_nMultiByteLen;
#endif
		}
		//返回值
		dwRetCode = 100;
	}
	//捕获异常
	catch (CInternetException* e)
	{
		e->GetErrorMessage(tzErrMsg,sizeof(tzErrMsg));
		e->Delete();
		dwRetCode = 500;
	}


	//////////////////////////////////////////////////////////////////////////
	//关闭
	pFile->Close();
	delete	pFile;
	pHttp->Close();
	delete pHttp;
	InternetSession.Close();
	return dwRetCode;
}
//返回值为图片大小，返回值为0表示不存在。
DWORD CHttpFileEx::GetExistPic(CString strPicPath)
{
	DWORD	dwRetCode = 0;
	WIN32_FIND_DATA	FindData;
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	hFile = FindFirstFile(strPicPath,&FindData);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		dwRetCode = 0;
	}
	else
	{
		dwRetCode = FindData.nFileSizeLow;
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return dwRetCode;
}
//UTF-8转为宽字符
void CHttpFileEx::UTF8ToWideChar(LPCSTR pUTF8,int UTF8Len)
{
	if (m_wpWideChar)
	{
		delete [] m_wpWideChar;
		m_wpWideChar = NULL;
	}
	
	m_nWideCharLen = MultiByteToWideChar(CP_UTF8,0,pUTF8,UTF8Len,NULL,0);
	
	m_wpWideChar = new WCHAR[m_nWideCharLen + 1];
	m_wpWideChar[m_nWideCharLen] = 0;
	
	MultiByteToWideChar(CP_UTF8,0,pUTF8,UTF8Len,m_wpWideChar,m_nWideCharLen);
	
}
//将UTF8转为多字节
void CHttpFileEx::UTF8ToMultiByte(LPCSTR pUTF8, int UTF8Len)
{
	//这个转换也要分两步，先将UTF8转为宽字符，
	//再将宽字符转为多字节。
	if (m_lpMultiByte)
	{
		delete [] m_lpMultiByte;
		m_lpMultiByte = NULL;
	}
	//UTF8转宽字符
	UTF8ToWideChar(pUTF8,UTF8Len);
	//宽字符转为多字节
	m_nMultiByteLen = WideCharToMultiByte(CP_ACP,0,m_wpWideChar,m_nWideCharLen,NULL,0,NULL,NULL);
	m_lpMultiByte = new char[m_nMultiByteLen + 1];
	m_lpMultiByte[m_nMultiByteLen] = 0;
	
	WideCharToMultiByte(CP_UTF8,0,m_wpWideChar,m_nWideCharLen,m_lpMultiByte,m_nMultiByteLen,NULL,NULL);
}
//将宽字符转为UTF-8
void CHttpFileEx::WideCharToUTF8(LPCWSTR wpWideData, int WideLen)
{
	//如果原先的数据没有删除
	if (m_lpUTF8)
	{
		delete [] m_lpUTF8;
		m_lpUTF8 = NULL;
	}
	
	m_nUTF8Len = WideCharToMultiByte(CP_UTF8,0,wpWideData,WideLen,NULL,0,NULL,NULL);
	
	m_lpUTF8 = new char[m_nUTF8Len + 1];
	m_lpUTF8[m_nUTF8Len] = 0;
	
	WideCharToMultiByte(CP_UTF8,0,wpWideData,WideLen,m_lpUTF8,m_nUTF8Len,NULL,NULL);
}


//将多字节转为UTF8
void CHttpFileEx::MultiByteToUTF8(LPCSTR lpMultiData, int MultiLen)
{
	//这个转换要麻烦点，要先将多字节转为宽字符，再将宽字符转为UTF8
	//其实如果全部是英文的话，是没有必要进行转换了，如果有中文，就必须转换
	//如果原先的数据没有删除
	if (m_lpUTF8)
	{
		delete [] m_lpUTF8;
		m_lpUTF8 = NULL;
	}
	if (m_wpWideChar)
	{
		delete [] m_wpWideChar;
		m_wpWideChar = NULL;
	}
	//先转为宽字符	
	m_nWideCharLen = MultiByteToWideChar(CP_ACP,0,lpMultiData,MultiLen,NULL,0);
	
	m_wpWideChar = new WCHAR[m_nWideCharLen + 1];
	m_wpWideChar[m_nWideCharLen] = 0;
	
	MultiByteToWideChar(CP_ACP,0,lpMultiData,MultiLen,m_wpWideChar,m_nWideCharLen);
	//再将宽字符转为UTF8
	WideCharToUTF8(m_wpWideChar,m_nWideCharLen);
}

void CHttpFileEx::WideToMulti(LPCWSTR wpWideData, int WideLen)
{
	if (m_lpMultiByte)
	{
		delete [] m_lpMultiByte;
		m_lpMultiByte = NULL;
	}

	m_nMultiByteLen = WideCharToMultiByte(CP_ACP,0,wpWideData,WideLen,NULL,0,NULL,NULL);
	m_lpMultiByte = new char[m_nMultiByteLen + 1];
	m_lpMultiByte[m_nMultiByteLen] = '\0';

	WideCharToMultiByte(CP_ACP,0,wpWideData,WideLen,m_lpMultiByte,m_nMultiByteLen,NULL,NULL);
}

void CHttpFileEx::MultiToMulti(LPCSTR strDataSend, int DataSize)
{
	if (m_lpMultiByte)
	{
		delete [] m_lpMultiByte;
		m_lpMultiByte = NULL;
	}
	
	m_nMultiByteLen = DataSize;
	m_lpMultiByte = new	char[m_nMultiByteLen + 1];
	m_lpMultiByte[m_nMultiByteLen] = '\0';

	memcpy(m_lpMultiByte,(LPCSTR)strDataSend,m_nMultiByteLen);
}

void CHttpFileEx::MultiToWide(LPCSTR strDataSend, int DataSize)
{
	if (m_wpWideChar)
	{
		delete [] m_wpWideChar;
		m_wpWideChar = NULL;
	}

	m_nWideCharLen = MultiByteToWideChar(CP_ACP,0,strDataSend,DataSize,NULL,0);

	m_wpWideChar = new wchar_t[m_nWideCharLen + 1];
	m_wpWideChar[m_nWideCharLen] = 0;

	MultiByteToWideChar(CP_ACP,0,strDataSend,DataSize,m_wpWideChar,m_nWideCharLen);
}

//构造头数据
void CHttpFileEx::MakeHeaderData()
{
	 CString strTemp = /*_T("POST /adminlilu/Include/UpFileSave.asp HTTP/1.1\r\n")*/
		_T("Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/xaml+xml,*/*\r\n")
		_T("Accept-Encoding: gzip, deflate\r\n")
		_T("Accept-Language: zh-cn\r\n")
		_T("Content-Type: multipart/form-data; boundary=%s\r\n")
		_T("Cache-Control: no-cache\r\n\r\n");

		/*_T("Host: %s:%d\r\n")*/
	 m_strHeaderData.Format(strTemp,m_strBoundary);
}
//构造发送的数据包
BOOL CHttpFileEx::MakePostData(LPCTSTR strFormName, LPCTSTR pPostValue)
{
	if (strFormName == NULL)
	{
		return FALSE;
	}

	CString	strTemp;
	strTemp.Format(_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n"),
		m_strBoundary,strFormName,pPostValue);
	//增加一个新的数据包
	m_strPostData += strTemp;
	
	return TRUE;
}
//构造协议数据内容
BOOL CHttpFileEx::MakePostData(LPCTSTR strFormName, int iPostValue)
{
	if (strFormName == NULL)
	{
		return FALSE;
	}

	CString	strTemp;
	strTemp.Format(_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%d\r\n"),
		m_strBoundary,strFormName,iPostValue);
	//增加一个新的数据包
	m_strPostData += strTemp;

	return TRUE;
}
//构造图片数据前缀包
void CHttpFileEx::MakePostPicData(LPCTSTR strFormName, LPCTSTR pPostValue)
{
	if (strFormName == NULL)
	{
		return ;
	}

	CString	strTemp;
	CString	strPicPath(pPostValue);	//
	CString	strExtName;	//扩展名
	strTemp.Format(_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"),
		m_strBoundary,strFormName,pPostValue);
	//根据扩展名设定content-type
	strPicPath.MakeUpper();
	strExtName = strPicPath.Right(strPicPath.GetLength() - strPicPath.ReverseFind(_T('.')) - 1);

	if (strExtName == _T("JPG") || strExtName == _T("JPEG"))
	{
		strTemp += _T("Content-Type: image/pjpeg\r\n\r\n");
	}
	else if (strExtName == _T("GIF"))
	{
		strTemp += _T("Content-Type: image/gif\r\n\r\n");
	}
	else if (strExtName == _T("PNG"))
	{
		strTemp += _T("Content-Type: image/x-png\r\n\r\n");
	}
	//二进制数据流
	else
	{
		strTemp += _T("Content-Type: application/octet-stream\r\n\r\n");
	}
	//增加一个新的数据包
	m_strPostData += strTemp;
	
	return;
}
//构造结束数据
void CHttpFileEx::MakeEndData()
{
	CString strFormat;

	strFormat += _T("\r\n--%s\r\nContent-Disposition: form-data; name=\"Submit\"\r\n\r\nsubmit\r\n--%s--");
	
	m_strEndData.Format(strFormat,m_strBoundary, m_strBoundary);
}

//下载HTTP文件，并保存成本地文件
BOOL CHttpFileEx::DownLoadFile(CString strServerPath,CString strLocalFilePath)
{
	if(strServerPath.IsEmpty())return FALSE;

	CString FilePath = strServerPath;
	DWORD dwSize = NULL;

	CFile* httpFile = NULL;
	char * pData = NULL;
	UINT   uCurrent = 0,uTotal=0;
	CFile  writeFile;
	CInternetSession session("HttpClient");
	try
	{
		//创建本地文件
		if(!writeFile.Open(strLocalFilePath,CFile::modeCreate | CFile::modeReadWrite))
			return FALSE;

		httpFile = (CHttpFile*)session.OpenURL(strServerPath);
		//如果失败，则返回FALSE
		if(httpFile == INVALID_HANDLE_VALUE)
		{
			writeFile.Close();
			return FALSE;
		}
        //获取文件字节数
		dwSize = (DWORD)httpFile->GetLength();

		pData = (char *)malloc(8 * 1024);
		memset(pData,0,8 * 1024);
		//读数据
		while(1)
		{
			//读去数据
			uCurrent = httpFile->Read(pData,8 * 1024);
			//写本地文件
			writeFile.Write(pData,uCurrent);
			//计算已经写入的数据大小
			uTotal = uTotal + uCurrent;
			if(uTotal >= dwSize) break;
		}
		free(pData);pData = NULL;
		writeFile.Close();
		//删除文件
		httpFile->Close();
		delete httpFile;

		return TRUE;
	}
	catch(CInternetException *e)
	{
		if(httpFile!=NULL)
		{
			httpFile->Close();
			delete httpFile;
		}
		e->Delete();
		return FALSE;
	}
}




