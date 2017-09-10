// CoverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Update.h"
#include "CoverDlg.h"
#include "Util.h"
#include "Tinyxml/tinyxml.h"
#include "Zip/ZipWrapper.h"
#include "tools.h"
#include "UpdateDlg.h"

// CCoverDlg dialog
#define WM_MESSAGE_CHANGE	WM_USER+100

IMPLEMENT_DYNAMIC(CCoverDlg, CDialog)

CCoverDlg::CCoverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCoverDlg::IDD, pParent)
		,m_RctCloseBtn(410, 2, 410+30, 2+30)
		,m_RctSmallBtn(389, 8, 389+21, 8+21)
		,m_RctLoading(170, 185, 170+89, 185+4)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nCloseBtnStatus = 0 ;
	m_pCloseBtn = new CGdiPlusPngResource;
	m_pCloseBtn->Load(IDB_DLG_CLOSE, _T("PNG"));

	m_nSmallBtnStatus = 0 ;
	m_pSmallBtn = new CGdiPlusPngResource;
	m_pSmallBtn->Load(IDB_DLG_SMALL, _T("PNG"));

	m_pLoadPng = new CGdiPlusPngResource;
	m_pLoadPng->Load(IDB_DLG_LOADING, _T("GIF"));

	m_bThread = true;

	m_strMessage = _T("");

	SetRect(&m_RctMessage, 0, 150, 430, 170);
}

CCoverDlg::~CCoverDlg()
{
	m_bThread = false;
}

void CCoverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCoverDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MESSAGE_CHANGE,			&CCoverDlg::OnMessageChange)
END_MESSAGE_MAP()


// CCoverDlg message handlers

void CCoverDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);
		
		CBitmap Bitmap;
		Bitmap.LoadBitmap(IDB_COVERDLG_BMP);
		CBitmap* oldBitmap = MemDC.SelectObject(&Bitmap);

		using namespace Gdiplus;
		USES_CONVERSION;
		Gdiplus::Graphics gx(MemDC.GetSafeHdc());

		if(m_pCloseBtn && m_pCloseBtn->m_pImage)
		{
			if(m_nCloseBtnStatus == 0)//默认
			{
				gx.DrawImage(m_pCloseBtn->m_pImage,RectF((REAL)m_RctCloseBtn.left ,(REAL)m_RctCloseBtn.top , (REAL)30, (REAL)30), (REAL)0, (REAL)0, (REAL)30, (REAL)30,UnitPixel, NULL);
			}
			else if(m_nCloseBtnStatus == 1)//点击
			{
				gx.DrawImage(m_pCloseBtn->m_pImage,RectF((REAL)m_RctCloseBtn.left ,(REAL)m_RctCloseBtn.top , (REAL)30, (REAL)30), (REAL)60, (REAL)0, (REAL)30, (REAL)30,UnitPixel, NULL);
			}
			else if(m_nCloseBtnStatus == 2)//热点
			{
				gx.DrawImage(m_pCloseBtn->m_pImage,RectF((REAL)m_RctCloseBtn.left ,(REAL)m_RctCloseBtn.top , (REAL)30, (REAL)30), (REAL)30, (REAL)0, (REAL)30, (REAL)30,UnitPixel, NULL);
			}
		}
		
		if(m_pSmallBtn && m_pSmallBtn->m_pImage)
		{
			if(m_nSmallBtnStatus == 0)//默认
			{
				gx.DrawImage(m_pSmallBtn->m_pImage,RectF((REAL)m_RctSmallBtn.left ,(REAL)m_RctSmallBtn.top , (REAL)21, (REAL)21), (REAL)0, (REAL)0, (REAL)21, (REAL)21,UnitPixel, NULL);
			}
			else if(m_nSmallBtnStatus == 1)//点击
			{
				gx.DrawImage(m_pSmallBtn->m_pImage,RectF((REAL)m_RctSmallBtn.left ,(REAL)m_RctSmallBtn.top , (REAL)21, (REAL)21), (REAL)42, (REAL)0, (REAL)21, (REAL)21,UnitPixel, NULL);
			}
			else if(m_nSmallBtnStatus == 2)//热点
			{
				gx.DrawImage(m_pSmallBtn->m_pImage,RectF((REAL)m_RctSmallBtn.left ,(REAL)m_RctSmallBtn.top , (REAL)21, (REAL)21), (REAL)21, (REAL)0, (REAL)21, (REAL)21,UnitPixel, NULL);
			}
		}

		if(m_pLoadPng && m_pLoadPng->m_pImage)
		{
			gx.DrawImage(m_pLoadPng->m_pImage,RectF((REAL)m_RctLoading.left, (REAL)m_RctLoading.top ,(REAL)m_pLoadPng->m_pImage->GetWidth(), (REAL)m_pLoadPng->m_pImage->GetHeight()),
				(REAL)0, (REAL)0, (REAL)m_pLoadPng->m_pImage->GetWidth(), (REAL)m_pLoadPng->m_pImage->GetHeight(), UnitPixel, NULL);
		}

	

		//
		dc.BitBlt(0,0,m_RctWnd.Width(),m_RctWnd.Height(),&MemDC,0,0,SRCCOPY);
		Bitmap.DeleteObject();
		MemDC.SelectObject(oldBitmap);
	
		//文本
		if(m_strMessage.length())
		{
			LOGFONT lf;
			CFont font;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = 14;
			strcpy_s(lf.lfFaceName, "宋体");
			CFont* pOldFont = NULL;
			font.CreateFontIndirect(&lf);
			lf.lfWeight = FW_NORMAL;

			pOldFont = dc.SelectObject(&font);
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(RGB(255,255,255));

			dc.DrawText(m_strMessage.c_str(),m_RctMessage,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			dc.SelectObject(&pOldFont);
			font.DeleteObject();
		}
	}
}

BOOL CCoverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	unsigned threadid;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, CoverThread, this, 0, &threadid);
	CloseHandle(hThread);

	GetWindowRect(&m_RctWnd);
	ScreenToClient(&m_RctWnd);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ShowLoadGif, (void *)this, NULL, &threadid);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

HCURSOR CCoverDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CCoverDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return TRUE;
		default:
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CCoverDlg::OnNcHitTest(CPoint point)
{
	return HTCLIENT;
}


void CCoverDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect Temp = m_RctCloseBtn;
	CRect Temp1 = m_RctSmallBtn;
	if(Temp.PtInRect(point) && m_nCloseBtnStatus != 2)
	{
		m_nCloseBtnStatus = 2;
		InvalidateRect(m_RctCloseBtn);
	}
	if(!Temp.PtInRect(point) && m_nCloseBtnStatus != 0)
	{
		m_nCloseBtnStatus = 0;
		InvalidateRect(m_RctCloseBtn);
	}
	if(Temp1.PtInRect(point) && m_nSmallBtnStatus != 2)
	{
		m_nSmallBtnStatus = 2;
		InvalidateRect(m_RctSmallBtn);
	}
	if(!Temp1.PtInRect(point) && m_nSmallBtnStatus != 0)
	{
		m_nSmallBtnStatus = 0;
		InvalidateRect(m_RctSmallBtn);
	}
}

void CCoverDlg::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( m_RctCloseBtn.PtInRect(point))
	{
		m_nCloseBtnStatus = 1;
		InvalidateRect(m_RctCloseBtn);
	}
	else if( m_RctSmallBtn.PtInRect(point))
	{
		m_nSmallBtnStatus = 1;
		InvalidateRect(m_RctSmallBtn);
	}
	CDialog::OnNcLButtonUp(nFlags, point);
}

void CCoverDlg::OnLButtonUp( UINT nFlags, CPoint point )
{
	CRect Temp = m_RctCloseBtn;
	CRect Temp1 = m_RctSmallBtn;
	if( Temp.PtInRect(point))
	{
		ExitProcess(0);
	}
	else if( Temp1.PtInRect(point))
	{
		m_nSmallBtnStatus = 0;
		InvalidateRect(m_RctSmallBtn);
		::PostMessage(GetSafeHwnd(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	CDialog::OnNcLButtonUp(nFlags, point);
}

unsigned __stdcall CCoverDlg::ShowLoadGif( void* pParam )
{
	CCoverDlg * pDlg = (CCoverDlg *)pParam ; 

	if(pDlg->m_pLoadPng->m_pImage == NULL)
		return 0;
	UINT count = 0;
	count = pDlg->m_pLoadPng->m_pImage->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID*)new GUID[count];
	pDlg->m_pLoadPng->m_pImage->GetFrameDimensionsList(pDimensionIDs, count);
	WCHAR strGuid[39];
	StringFromGUID2(pDimensionIDs[0], strGuid, 39);
	UINT frameCount = pDlg->m_pLoadPng->m_pImage->GetFrameCount(&pDimensionIDs[0]);
	delete []pDimensionIDs;   
	if(frameCount > 1)
	{
		UINT   FrameDelayNums=pDlg->m_pLoadPng->m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *  lpPropertyItem=new  PropertyItem[FrameDelayNums];
		pDlg->m_pLoadPng->m_pImage->GetPropertyItem(PropertyTagFrameDelay,FrameDelayNums,lpPropertyItem);
		long lPause = ((long*)lpPropertyItem->value)[0]*10;
		delete lpPropertyItem;
		int    fcount=0;//当前帧数
		//Guid的值在显示GIF为FrameDimensionTime，显示TIF时为FrameDimensionPage
		GUID    Guid = FrameDimensionTime;
		while(pDlg->m_bThread)
		{
			pDlg->m_pLoadPng->m_pImage->SelectActiveFrame(&Guid,fcount++); 
			pDlg->Invalidate(FALSE);
			//重新设置当前的活动数据帧
			if(fcount == frameCount) //frameCount是上面GetFrameCount返回值
				fcount= 0;     //如果到了最后一帧数据又重新开始
			//计算此帧要延迟的时间
			Sleep(lPause * 2);         //这里简单使用了sleep
		}
	} 
	return 0;
}

unsigned __stdcall CCoverDlg::CoverThread( void* pParam )
{
	CCoverDlg * pDlg = (CCoverDlg *)pParam ; 
	DWORD dwTickTime = GetTickCount();

	char szHashFile[MAX_PATH * 2 + 1];
	char szPhltFile[MAX_PATH * 2 + 1];
	char szTempDir[MAX_PATH * 2+ 1];
	char szRootDir[MAX_PATH * 2+ 1];
	char szExePath[MAX_PATH * 2+ 1];
	char szServerExePath[MAX_PATH * 2+ 1];

	GetModuleFileName(NULL, szRootDir, MAX_PATH * 2);
	char * pPos = strrchr(szRootDir, '\\');
	if(pPos)
	{
		*(pPos + 1) = 0;
	}
	SetCurrentDirectory(szRootDir);
	_tcscpy_s(pDlg->m_szRootDir, MAX_PATH * 2, szRootDir);

	strcpy_s(szExePath, MAX_PATH * 2, szRootDir);
	strcat_s(szExePath, MAX_PATH * 2, _T("101PPT.exe"));
	strcpy_s(szTempDir, MAX_PATH * 2, szRootDir);
	strcat_s(szTempDir, MAX_PATH * 2, _T("temp\\"));
	strcpy_s(szHashFile, MAX_PATH * 2, szTempDir);
	strcat_s(szHashFile, MAX_PATH * 2, _T("UpdateHashList.dat"));
	strcpy_s(szPhltFile, MAX_PATH * 2, szTempDir);
	strcat_s(szPhltFile, MAX_PATH * 2, _T("patch.phlt"));

	strcpy_s(szServerExePath, MAX_PATH * 2, szRootDir);
	strcat_s(szServerExePath, MAX_PATH * 2, _T("bin\\CoursePlayer\\101PPTDaemon.exe"));
	OutputDebugString(szServerExePath);
	ShellExecute(NULL,"open",szServerExePath,"-u", "", SW_HIDE );
//	::SendMessage(pDlg->GetSafeHwnd(), WM_MESSAGE_CHANGE, NULL, (LPARAM)_T("正在初始化程序"));
	//获取更新列表
	vector<UPDATEDATA> list;
	vector<UPDATEDATA> listBak;//备份用
	CStdioFile File;
	TCHAR szLine[MAX_PATH*2] ;
	if(File.Open(szHashFile, CFile::modeRead))
	{
		while(File.ReadString(szLine, MAX_PATH * 2 - 1))
		{
			char* pPost = strrchr(szLine, '|');
			if (pPost)
			{
				UPDATEDATA data={0};
				int nFileNameLen = pPost - szLine;
				memcpy(data.sFile, szLine, nFileNameLen);
				data.sFile[nFileNameLen] = '\0';
				strcpy_s(data.sHash, MD5_LEN, pPost + 1);
				list.push_back(data);
				listBak.push_back(data);
			}
		}
		File.Close();
	}

//	::SendMessage(pDlg->GetSafeHwnd(), WM_MESSAGE_CHANGE, NULL, (LPARAM)_T("正在复制文件目录"));
	//
	bool bFlag;
	if(!list.empty())
	{
		TCHAR szSrcFile[MAX_PATH * 2 + 1];
		TCHAR szDstFile[MAX_PATH * 2 + 1];
		vector<UPDATEDATA>::iterator ite;
		for (ite = list.begin(); ite != list.end(); )
		{
			bFlag = false;
			sprintf_s(szSrcFile, _T("%s%s"), szTempDir, (*ite).sFile);
			sprintf_s(szDstFile, _T("%s%s"), szRootDir, (*ite).sFile);
			CString strSrcFile = szSrcFile;
			CString strDstFile = szDstFile;
			strDstFile.Replace("\\\\","\\");
			strSrcFile.Replace("\\\\","\\");
			//先检测文件是否存在
			if(GetFileAttributes(strSrcFile.GetString()) != INVALID_FILE_ATTRIBUTES)
			{
				for(int k = 0; k < 3; k++)
				{
					if(MiscMyMoveFile(strSrcFile.GetString(), strDstFile.GetString()))
					{
						ite = list.erase(ite);
						bFlag = true;
						break;
					}
					Sleep(200);
				}
				if(!bFlag)
					++ite;		
			}
			else//文件已经不存在
			{
				ite = list.erase( ite);
			}
		}
	}

	//获取增量更新列表
	char szUpdateTip[1024];
	if(PathFileExists(szPhltFile))
	{
		FILE* fp = fopen(szPhltFile,"rb");
		if(fp != NULL)
		{
			// 分析配置文件
			vector<PatchInfo> PatchList;
			PatchInfo pi;
			while(!feof(fp))
			{
				memset(&pi,0x00,sizeof(pi));
				if(1 != fread(&pi.pt,sizeof(PatchType),1,fp))
				{
					break;
				}

				if(1 != fread(pi.szFileName,pi.pt.unFileNameSize,1,fp))
				{
					LOG_ERROR("invalid patch list file");
					break;
				}

				PatchList.push_back(pi);
			}
			fclose(fp);

			vector<PatchInfo>::size_type n = PatchList.size();
			string strSrcFile;
			string strDstFile;

			for(int j = 0; j < n; j++)
			{
				PatchInfo& rpi = PatchList[j];
				strSrcFile = szTempDir;
				strDstFile = szRootDir;
				strSrcFile += rpi.szFileName;
				strDstFile += rpi.szFileName;

				// 删除文件
				if(!rpi.pt.bAddFile)
				{
					MiscMyDeleteFile(strDstFile.c_str());
					continue;
				}

				// 源文件不存在
				if(!PathFileExists(strSrcFile.c_str()))
				{
					LOG_ERROR("FILE %s NOT exist, filter", strSrcFile.c_str());
					continue;
				}

				bFlag = false;
				for(int k = 0; k < 3; k++)
				{
					if(MiscMyMoveFile(strSrcFile.c_str(), strDstFile.c_str()))
					{
						bFlag = true;
						break;
					}
					
					Sleep(200);
				}
				if(!bFlag)
				{
					_stprintf_s(szUpdateTip, _T("文件更新失败，文件路径:%s, 错误id:0x%x"), strDstFile.c_str(), GetLastError());
					::MessageBox(pDlg->GetSafeHwnd(), szUpdateTip, _T("提示"), MB_OK);
					::PostMessage(pDlg->GetSafeHwnd(), WM_CLOSE, 0 ,0);
					return 0;
				}
				string strSrc7zFile = strSrcFile + ".7z";
				MiscMyDeleteFile(strSrc7zFile.c_str());
			}

			DeleteFile(szPhltFile);
		}
		else
		{
			LOG_ERROR("open file %s fail",szPhltFile);
		}

	}

	//

	//
	// 解压
	//
	if(pDlg->ExtractFile(listBak) == FALSE)
	{
		::PostMessage(pDlg->GetSafeHwnd(), WM_CLOSE, 0 ,0);
		return 0;
	}


	WCHAR wszCmdBuff[MAX_PATH * 2 + 1];
	if(list.empty())
	{
		wcscpy_s(wszCmdBuff, MAX_PATH * 2, L"success");
		DeleteFile(szHashFile);
	}
	else
	{
		wcscpy_s(wszCmdBuff, MAX_PATH * 2, L"restart");
		CreateHashFile(szTempDir, list);
	}

	

	// sleep
	if(GetTickCount() - dwTickTime <= 2 * 1000)
	{
		Sleep(2 * 1000 - (GetTickCount() - dwTickTime));
	}

	//重启

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	WCHAR wszExePath[MAX_PATH * 2 + 1];
	ANSIToUnicode(szExePath, wszExePath);
	WCHAR wszRootDir[MAX_PATH * 2 + 1];
	ANSIToUnicode(szRootDir, wszRootDir);
	WCHAR wszCMD[MAX_PATH * 2 + 3];
	swprintf(wszCMD,L" \"%s\" %s", g_FilePath, wszCmdBuff);
	if (CreateProcessW(wszExePath,wszCMD,NULL,NULL,TRUE,0,NULL,wszRootDir,&si,&pi))
	{
		CloseHandle(pi.hThread);
	}

	::PostMessage(pDlg->GetSafeHwnd(), WM_CLOSE, 0 ,0);
	/*
	//拷贝完成删除
	
	pDlg->DirectoryDelete(szDirectory);
	//
	*/
	return 0;
}

BOOL CCoverDlg::ExtractFile(vector<UPDATEDATA>& listBak)
{
	BOOL bRet = TRUE;
	
	char szPackagePath[MAX_PATH + 1];
	char szTempPackageDir[MAX_PATH + 1];
	char szSrcPackagePath[MAX_PATH + 1];
	char szUpdateTip[1024];
	char szMessage[1024];

	strcpy_s(szPackagePath, MAX_PATH, m_szRootDir);
	strcat_s(szPackagePath, MAX_PATH, _T("skins\\config\\Package.xml"));

	UnicodeToANSI(g_szNdCloudPath, m_szNdCloudPath);

	TiXmlDocument	docPackage;

	bool bFind = false;
	vector<UPDATEDATA>::iterator ite;
	if(docPackage.LoadFile(szPackagePath))
	{
		TiXmlElement* pRootElement = docPackage.FirstChildElement();
		if( pRootElement)
		{
			TiXmlElement* pListsElement = pRootElement->FirstChildElement();

			TiXmlElement* pChildElement;  
			if( pListsElement )
			{
				for ( pChildElement = pListsElement->FirstChildElement(); pChildElement != 0; pChildElement = pChildElement->NextSiblingElement())   
				{  
					bFind = false;
					TiXmlElement * pSrcPathElement = pChildElement->FirstChildElement();
					TiXmlElement * pDestPathElement = pSrcPathElement->NextSiblingElement();
					const char *szSrcPath = pSrcPathElement->GetText();
					const char *szDestDir = pDestPathElement->GetText();

					
					for (ite = listBak.begin(); ite != listBak.end(); ite++)
					{
						if( _tcsicmp((*ite).sFile, szSrcPath) == 0)
						{
							bFind = true;
							break;
						}
					}

					if(bFind)
					{
						_tcscpy_s(m_szSrcPath, szSrcPath);
						_tcscpy_s(m_szDestDir, szDestDir);

						if(szDestDir[0] == '\\')//当前目录
						{
							_stprintf_s(szTempPackageDir, _T("%stemp%s"), m_szRootDir, szDestDir);
						}
						else//ndCloud目录
						{
							_stprintf_s(szTempPackageDir, _T("%stemp\\%s"), m_szRootDir, szDestDir);
						}
						//
						
						_stprintf_s(szSrcPackagePath, _T("%s%s"), m_szRootDir, szSrcPath);

						if(szTempPackageDir[_tcslen(szTempPackageDir) - 1] == '\\')
						{
							szTempPackageDir[_tcslen(szTempPackageDir) - 1] = 0;
						}

						//先检测文件是否存在
						if(GetFileAttributes(szSrcPackagePath) != INVALID_FILE_ATTRIBUTES)
						{
							_stprintf_s(szMessage, _T("正在解压文件 %s"), szSrcPath);
							::SendMessage(GetSafeHwnd(), WM_MESSAGE_CHANGE, NULL, (LPARAM)szMessage);

							CString strSrcPackagePath = szSrcPackagePath;
							CString strTempPackageDir = szTempPackageDir;
							strSrcPackagePath.Replace("\\\\","\\");
							strTempPackageDir.Replace("\\\\","\\");

							if(CUnZipper::UnZip(strSrcPackagePath.GetString(), strTempPackageDir.GetString() ) == FALSE)
							{
								bRet = FALSE;
								goto _Out;
							}

							_tcscpy_s(m_szTempPackageDir, MAX_PATH, strTempPackageDir.GetString());
							while(TRUE)
							{
								if(!FindFile((char *)strTempPackageDir.GetString()))
								{
									_stprintf_s(szUpdateTip, _T("文件更新失败，文件路径:%s, 错误id:0x%x\r\n请关闭文件后再重试更新"), m_szFailPath, szDestDir, GetLastError());
									int nRet = ::MessageBox(GetSafeHwnd(), szUpdateTip, _T("提示"), MB_RETRYCANCEL);
									if(nRet == IDRETRY)
									{
										
									}
									else
									{
										bRet = FALSE;
										goto _Out;
									}
								}
								else
									break;
							}


							string strConfigFilePath = m_szRootDir;
							strConfigFilePath += "Package\\config.ini";


							DWORD dwNewCrc = CalcFileCRC(szSrcPackagePath);
							// compare crc
							char szName[MAX_PATH] = {0};
							const char * p = strrchr(szSrcPath, '\\');
							if(p)
							{
								strcpy(szName, p+1);
							}
							char szCrc[MAX_PATH] = {0};
							sprintf(szCrc, "%08lX", dwNewCrc);

							WritePrivateProfileStringA("crc", szName, szCrc, strConfigFilePath.c_str());

						}
					}
					
					
				}  
			}
		}
	}

_Out:
	return bRet;
}

BOOL CCoverDlg::FindFile( char* szRootDir )
{
	BOOL bRet = FALSE;
	char szSrcFile[MAX_PATH * 2 + 1];
	char szDstFile[MAX_PATH * 2 + 1];
	char szRelativeFile[MAX_PATH * 2 + 1];

	bool bFlag = false;

	WIN32_FIND_DATA fData;
	char szDir[MAX_PATH * 2 + 1];
	_stprintf(szDir, _T("%s\\*.*"), szRootDir);
	HANDLE hfile = FindFirstFileA(szDir ,&fData);  

	if ( INVALID_HANDLE_VALUE == hfile ) 
	{
		return bFlag;
	}

	if(_tcslen(m_szTempPackageDir) < _tcslen(szRootDir))
		_tcscpy_s(szRelativeFile, MAX_PATH * 2, szRootDir + strlen(m_szTempPackageDir) + 1);
	else
		_tcscpy_s(szRelativeFile, MAX_PATH * 2, _T(""));

	do
	{
		bFlag = false;

		if( _tcscmp(fData.cFileName, ".") == 0 || _tcscmp(fData.cFileName, "..") == 0 )
		{

		}
		else if(fData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			_stprintf(szDir, _T("%s\\%s"), szRootDir, fData.cFileName);
			CString strDir = szDir;
			strDir.Replace("\\\\","\\");
			bRet = FindFile((char *)strDir.GetString());
			if(!bRet)
			{
				goto _Out;
			}
		}
		else
		{
			if(_tcscmp(fData.cFileName, "UpdateHashList.dat") != 0 )
			{
				_stprintf_s(szSrcFile, _T("%s\\%s"), szRootDir, fData.cFileName);

				if(m_szDestDir[0] == '\\')//当前目录
				{
					_stprintf_s(szDstFile, _T("%s%s\\%s\\%s"), m_szRootDir, m_szDestDir, szRelativeFile, fData.cFileName);
				}
				else//ndCloud目录
				{
					_stprintf_s(szDstFile, _T("%s%s\\%s\\%s"), m_szNdCloudPath, m_szDestDir, szRelativeFile, fData.cFileName);
				}
				//

				CString strSrcFile = szSrcFile;
				CString strDstFile = szDstFile;
				strSrcFile.Replace("\\\\","\\");
				strDstFile.Replace("\\\\","\\");
				for(int k = 0; k < 3; k++)
				{
					
					
					if(MiscMyMoveFile(strSrcFile.GetString(), strDstFile.GetString()))
					{
						bFlag = true;
						break;
					}
					Sleep(200);
				}
				if(!bFlag)
				{
					_tcscpy_s(m_szFailPath, MAX_PATH * 2, strDstFile.GetString());
					goto _Out;
				}
			}
		}
	}while ( FindNextFileA(hfile, &fData) != 0 );

	bRet = TRUE;
_Out:
	return bRet;
}

LRESULT CCoverDlg::OnMessageChange( WPARAM wParam, LPARAM lParam )
{
	SetStrMessage((LPCTSTR)lParam);
	Invalidate();
	return 0;
}

void CCoverDlg::SetStrMessage(LPCTSTR pszMessage)
{
	m_strMessage = pszMessage;
}
