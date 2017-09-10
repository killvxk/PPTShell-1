#include "stdafx.h"
#include "AutoUpdate.h"
#include "Net.h"
#include "UnRarAuto.h"
#include "UpdateDlg.h"
#include "SuperFastHash.h"
#include "tools.h"

//#define LOCAL_DEBUG


#ifdef LOCAL_DEBUG


#else
	static const char UPDATE_DAT[]	= "update.txt";
	static const char RUN_APP[]		= "101PPT.exe";
	static const char UPDATE_INFO[]	= "Update.dat";

	static const char APPNAME[]		= "101ppt";
	static const char INCREMENTWEB[]	= "Autopatcher.php";
#ifdef DEVELOP_VERSION
	static const char ICRPLAYER_UPDATE_URL[]	= "http://p.101.com/newpptshell/update/icrplayer/";
	static const char ICRPLAYER_UPDATE_IP[]	= "http://61.160.40.166/newpptshell/update/icrplayer/";
	static const char UPDATE_URL[]	= "http://p.101.com/newpptshell/update/";
	static const char UPDATE_IP[]	= "http://61.160.40.166/newpptshell/update/";
	static const char INCREMENT_URL[]	= "http://p.101.com/newpptshell/incremental/";
//	static const char INCREMENT_URL[]	= "http://127.0.0.1/";
	static const char INCREMENT_IP[]	= "http://61.160.40.166/newpptshell/incremental/";
#else
	static const char ICRPLAYER_UPDATE_URL[]	= "http://p.101.com/final/update/icrplayer/";
	static const char ICRPLAYER_UPDATE_IP[]	= "http://61.160.40.166/final/update/icrplayer/";
	static const char UPDATE_URL[]	= "http://p.101.com/final/update/";
	static const char UPDATE_IP[]	= "http://61.160.40.166/final/update/";
	static const char INCREMENT_URL[]	= "http://p.101.com/final/incremental/";
	static const char INCREMENT_IP[]	= "http://61.160.40.166/final/incremental/";
#endif
	static const char VERSION_DAT[]	= "Version.dat";
	static const char INCREMENTVER_DAT[]	= "Increment_Ver.dat";
#endif

CNet net;

unsigned __stdcall AutoUpdate(void* pParam)
{
	E_RET e_ret = e_newest;
	char sUrl[URL_LEN_MAX];
	char sPath[MAX_PATH];
	bool bUrlOk = true;
	bool bPipeOpen = false;
	CUpdateDlg* pDlg = (CUpdateDlg*)pParam;
	CString strFilePath;
	
	UPDATESTATE state;

	if(pDlg->m_bSelfUpdate)
	{
		WCHAR szMemory[] = L"101PPTUpdateSharedMemory";

		HANDLE hFileMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE,szMemory);
		if( hFileMapping == NULL )
		{
			return 0;
		}

		g_pUpdateBuff = (BYTE*)MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE , 0, 0, 0x1000);
		if(g_pUpdateBuff == NULL)
		{
			return 0;
		}

		memset(&pDlg->m_updateParam, 0x00, sizeof(pDlg->m_updateParam));

		pDlg->m_updateParam.updateInfoParam.nType = eUpdateType_Update_Processing;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
	}
	
	for(int ik = 0;ik< 1;ik++)
	{
		_HasUpdate context;
		memset(&context, 0, sizeof(context));
		
		GetModuleFileName(NULL, strFilePath.GetBuffer(MAX_PATH), MAX_PATH);
		strFilePath.ReleaseBuffer();
		strFilePath = strFilePath.Left(strFilePath.ReverseFind('\\') + 1);
		strcpy(sPath, strFilePath);
		strcat(sPath, UPDATE_TEMP_DIR);
		CString strTmpPath = sPath;
		SetCurrentDirectory(strFilePath);

// 		if (NULL != pDlg->m_hInPipe && NULL != pDlg->m_hOutPipe)
// 		{
// 			// 管道校验
// 			bPipeOpen = true;
// 		}
		
		// 检测版本号
		char szVersionFile[MAX_PATH];
		strcpy(szVersionFile,strFilePath);
		strcat(szVersionFile,VERSION_DAT);
		char szRes[32] = "0.0.0.0";
		FILE * fp = fopen(szVersionFile,"r");
		if(fp)
		{
			fscanf(fp,"%s\n",szRes);
			fclose(fp);
		}
		
		// 下载版本号文件
		pDlg->SendMessage(WM_MYMESSAGE, e_act_dat, NULL);
		char* sVersionBuff = NULL;
		strcpy(sUrl, UPDATE_URL);
		strcat(sUrl, VERSION_DAT);

		if (!net.DownData(sUrl, "null", "null", &sVersionBuff))
		{
			bUrlOk = false;
			strcpy(sUrl, UPDATE_IP);
			strcat(sUrl, VERSION_DAT);
			if (!net.DownData(sUrl, "null", "null", &sVersionBuff))
			{
				e_ret = e_f_dat;
				break;
			}
		}

		_VersionStruct versionStruct;
		_VersionStruct serverVersionStruct;
		if(!versionStruct.InitFromString(szRes))
		{
			delete sVersionBuff;
			e_ret = e_f_dat;
			break;
		}
		if(!serverVersionStruct.InitFromString(sVersionBuff))
		{
			delete sVersionBuff;
			e_ret = e_f_dat;
			break;
		}
		if(versionStruct.CompareTo(&serverVersionStruct) >= 0)
		{
			delete sVersionBuff;
			break;
		}

		strncpy(context.szVersion, sVersionBuff, sizeof(context.szVersion)-1);
		delete sVersionBuff;
		//

		// 下载配置文件

		char* sIniBuff = NULL;
		if(bUrlOk)
		{
			strcpy(sUrl, UPDATE_URL);
			strcat(sUrl, UPDATE_DAT);
		}
		else
		{
			strcpy(sUrl, UPDATE_IP);
			strcat(sUrl, UPDATE_DAT);
		}

		if (!net.DownData(sUrl, "null", "null", &sIniBuff))
		{
			e_ret = e_f_dat;
			break;
		}

		// 解析
		vector<UPDATEDATA> list;
		vector<UPDATEDATA> locallist;//增加一个本地的list
		char* p;
		char* pp = sIniBuff;
		while (NULL != (p = strstr(pp, "\r\n")))
		{
			int n = FindChar_f(pp, "|");
			if (-1 == n)
			{
				e_ret = e_f_dat;
				break;
			}

			UPDATEDATA data={0};
			memcpy(data.sFile, pp, n);
			data.sFile[n] = '\0';
			memcpy(data.sHash, &pp[n+1], p - (pp + n + 1));
			data.sHash[MD5_LEN] = '\0';
			if ('*' == data.sFile[n-1])
			{
				data.sFile[n-1] = '\0';
				data.bDecomp = true;
			}
			else
				data.bDecomp = false;
			list.push_back(data);
			pp = p + 2;
		}
		if(pp && strlen(pp) != 0)
		{
			int n = FindChar_f(pp, "|");
			if (-1 != n)
			{
				UPDATEDATA data={0};
				memcpy(data.sFile, pp, n);
				data.sFile[n] = '\0';
				memcpy(data.sHash, &pp[n+1], p - (pp + n + 1));
				data.sHash[MD5_LEN] = '\0';
				if ('*' == data.sFile[n-1])
				{
					data.sFile[n-1] = '\0';
					data.bDecomp = true;
				}
				else
					data.bDecomp = false;
				list.push_back(data);
			}
		}

		delete[] sIniBuff;
		sIniBuff = NULL;

		//增加互动课堂判断
		CString strICRPlayerPath = strFilePath;
		strICRPlayerPath += _T("\\icrplayer\\ND_ICR_WPF_PLAYER.exe");

		DWORD ret = GetFileAttributes(strICRPlayerPath);

		if( ret != INVALID_FILE_ATTRIBUTES )
		{
			if(bUrlOk)
			{
				strcpy(sUrl, ICRPLAYER_UPDATE_URL);
				strcat(sUrl, UPDATE_DAT);
			}
			else
			{
				strcpy(sUrl, ICRPLAYER_UPDATE_IP);
				strcat(sUrl, UPDATE_DAT);
			}

			if (!net.DownData(sUrl, "null", "null", &sIniBuff))
			{
				e_ret = e_f_dat;
				break;
			}

			// 解析
			char* p;
			char* pp = sIniBuff;
			while (NULL != (p = strstr(pp, "\r\n")))
			{
				int n = FindChar_f(pp, "|");
				if (-1 == n)
				{
					e_ret = e_f_dat;
					break;
				}

				UPDATEDATA data={0};
				memcpy(data.sFile, pp, n);
				data.sFile[n] = '\0';
				memcpy(data.sHash, &pp[n+1], p - (pp + n + 1));
				data.sHash[MD5_LEN] = '\0';
				if ('*' == data.sFile[n-1])
				{
					data.sFile[n-1] = '\0';
					data.bDecomp = true;
				}
				else
					data.bDecomp = false;
				list.push_back(data);
				pp = p + 2;
			}
			if(pp && strlen(pp) != 0)
			{
				int n = FindChar_f(pp, "|");
				if (-1 != n)
				{
					UPDATEDATA data={0};
					memcpy(data.sFile, pp, n);
					data.sFile[n] = '\0';
					memcpy(data.sHash, &pp[n+1], strlen(pp) - n - 1);
					data.sHash[MD5_LEN] = '\0';
					if ('*' == data.sFile[n-1])
					{
						data.sFile[n-1] = '\0';
						data.bDecomp = true;
					}
					else
						data.bDecomp = false;
					list.push_back(data);
				}
			}

			delete[] sIniBuff;
		}
		//
		if (list.empty())
		{
			e_ret = e_f_dat;
			break;
		}
		
		// 校验本地文件
		vector<UPDATEDATA>::iterator ite;

		CString strTempFile;
		unsigned int nHash = 0;
		for (ite = list.begin(); ite != list.end();)
		{
			strTempFile = _T("temp\\");
			strTempFile += (*ite).sFile;

			nHash = strtoul((*ite).sHash, NULL ,10);
			if (nHash == SuperFastHashFile((*ite).sFile))
			{
				//增加检测temp目录
				char* pszPath = strTempFile.GetBuffer(strTempFile.GetLength());
				if(isFileExsit(pszPath))
				{
					DeleteFile(pszPath);
				}
				strTempFile.ReleaseBuffer();
				//
				ite = list.erase(ite);
			}
			else
			{
				e_ret = e_download_newest;

				char* pszPath = strTempFile.GetBuffer(strTempFile.GetLength());
				if (nHash == SuperFastHashFile(pszPath))
				{
					//增加检测temp目录

					locallist.push_back(*ite);//增加到本地列表
					ite = list.erase(ite);
					//
				}
				else
					++ite;

				strTempFile.ReleaseBuffer();
			}
		}
		
		//增加增量更新部分
		vector<string> vecDownInfo;

		char szIncrementVerFile[MAX_PATH * 2];
		strcpy(szIncrementVerFile,strFilePath);
		strcat(szIncrementVerFile,INCREMENTVER_DAT);
		fp = fopen(szIncrementVerFile,"r");
		_tcscpy(szRes, "0\0");
		if(fp)
		{
			fscanf(fp,"%s\n",szRes);
			fclose(fp);
		}
		string strCurVersion = szRes;

		char* szUrl = (char*)malloc(1024);


		
		while(true)
		{
			char szInfofileName[MAX_PATH * 2];

			if (!CreateTempFile(szInfofileName))
			{
				LOG_ERROR("create temp file fail");
				break;
			}

			LOG_TRACE("create temp file:%s", szInfofileName);
			string strXmlFileName = szInfofileName;

			_snprintf(szUrl,1024,"%s%s?app=%s&version=%s",
				INCREMENT_URL,
				INCREMENTWEB,
				APPNAME,
				strCurVersion.c_str());

			if (!UrlRandDownload(szUrl, strCurVersion, strXmlFileName))
			{
				LOG_ERROR("download %s fail", strXmlFileName.c_str());
				break;
			}

			HANDLE hFile = CreateFile(strXmlFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				LOG_ERROR("open file %s fail.error:%d", strXmlFileName.c_str(), GetLastError());
				DeleteFile(szInfofileName);
				break;
			}

			if (0 == GetFileSize(hFile, 0))
			{
				CloseHandle(hFile);
				DeleteFile(szInfofileName);
				break;
			}

			char szRead[16];
			DWORD dwRead;

			ZeroMemory(szRead, sizeof(szRead));
			ReadFile(hFile, szRead, sizeof(szRead), &dwRead, NULL);
			CloseHandle(hFile);

			if (0 == stricmp(szRead, "NewestVersion"))
			{
				DeleteFile(szInfofileName);
				break;
			}
			else if (strCurVersion == "0")
			{
				vecDownInfo.push_back(strXmlFileName);
				break;
			}

			if (!GetFileVersionFromXml(strCurVersion, strXmlFileName.c_str()))
			{
				LOG_ERROR("GetFileVersionFromXml fail:%s", strXmlFileName.c_str());
				break;
			}

			vecDownInfo.push_back(strXmlFileName);

			// 解决会多下载一个0_0.xml的问题
			if (strCurVersion == "0")
			{
				break;
			}
		}
		//
		delete szUrl;

		if (list.empty() && locallist.empty() && vecDownInfo.empty())//3个列表都为空
		{
			break;
		}

		if (!ChkDir())
		{
			e_ret = e_f_dir;
			break;
		}

		if(!list.empty() || vecDownInfo.size() > 0)
		{
			pDlg->SendMessage(WM_MYMESSAGE, e_act_show, NULL);//有更新
		
			pDlg->SendMessage(WM_MYMESSAGE, e_act_down, NULL);// 下载更新

			pDlg->m_updateParam.dwDownloadCount = list.size() + locallist.size();
			pDlg->m_updateParam.dwDownloadCount += pDlg->m_updateParam.dwDownloadCount;//增量
			pDlg->m_updateParam.dwDownloadIndex = locallist.size();
			pDlg->m_updateParam.updateInfoParam.dwTotalProgress = ((float)pDlg->m_updateParam.dwDownloadIndex / pDlg->m_updateParam.dwDownloadCount) * 100;
			pDlg->m_updateParam.dwPreTotalProgress = pDlg->m_updateParam.updateInfoParam.dwTotalProgress;

			pDlg->SendMessage(WM_MYMESSAGE, e_val_all, pDlg->m_updateParam.dwDownloadCount);
		}

		if(!list.empty())//从服务端下载普通更新
		{
			char* szUpdateInfo = NULL;

			bool bCalcHashSuccess = false;
			int nIndex = 0;
			for (ite = list.begin(); ite != list.end(); ++ite)
			{
				bCalcHashSuccess = false;
				if (bUrlOk)
					strcpy(sUrl, UPDATE_URL);
				else
					strcpy(sUrl, UPDATE_IP);
				strcat(sUrl, (*ite).sFile);
				CString strUrl;
				strUrl.Format("%s",sUrl);
				strUrl.Replace('\\','//');

				strncpy(pDlg->m_updateParam.updateInfoParam.szFileName, (*ite).sFile, MAX_PATH - 1);
				pDlg->m_updateParam.updateInfoParam.fDownloadSpeed = 0;
				if(g_pUpdateBuff)
					memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));

				for (int i=0; i<3; ++i)
				{
					pDlg->m_updateParam.updateInfoParam.dwTotalProgress = pDlg->m_updateParam.dwPreTotalProgress;
					if(g_pUpdateBuff)
						memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
					if (net.DownFile((char *)strUrl.GetString(), sPath, (*ite).sFile, NULL, pDlg))
					{
						nHash = strtoul((*ite).sHash, NULL ,10);
						if (nHash == SuperFastHashFile(strTmpPath + (*ite).sFile))
						{
							bCalcHashSuccess = true;
							(*ite).bDown = true;
							break;
						}
					}
				}

				nIndex++;
				
				if(!bCalcHashSuccess)
				{
					break;
				}

				pDlg->m_updateParam.dwDownloadIndex++;
				pDlg->m_updateParam.updateInfoParam.dwCurrentProgress = 0;
				pDlg->m_updateParam.updateInfoParam.dwTotalProgress = ((float)pDlg->m_updateParam.dwDownloadIndex / pDlg->m_updateParam.dwDownloadCount) * 100;
				pDlg->m_updateParam.dwPreTotalProgress = pDlg->m_updateParam.updateInfoParam.dwTotalProgress;
				if(g_pUpdateBuff)
					memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
				pDlg->SendMessage(WM_MYMESSAGE, e_val_down, pDlg->m_updateParam.dwDownloadIndex);

			}

			if(!bCalcHashSuccess)
			{
				e_ret = e_f_down;
				break;
			}
		}

		if(vecDownInfo.size() > 0)
		{
			//下载增量更新
			if(!pDlg->PatchStart(strFilePath.GetString(), vecDownInfo))
			{
				e_ret = e_f_down;
				break;
			}
			//
		}

		if(!list.empty() || vecDownInfo.size() > 0)
			pDlg->SendMessage(WM_MYMESSAGE, e_val_down, pDlg->m_updateParam.dwDownloadCount);

		if(!pDlg->m_bSelfUpdate)
		{
			pDlg->SendMessage(WM_MYMESSAGE, e_act_move, NULL);
			// 复制下载的列表
			vector<UPDATEDATA> rarlist;
			for (ite = list.begin(); ite != list.end(); ++ite)
			{
				if((*ite).bDown)
				{
					if (!MiscMyMoveFile(strTmpPath+(*ite).sFile, (*ite).sFile))
					{
					}
					if ((*ite).bDecomp)
						rarlist.push_back(*ite);
				}
			}

			for (ite = list.begin(); ite != list.end();)
			{
				nHash = strtoul((*ite).sHash, NULL ,10);
				if (nHash == SuperFastHashFile(strFilePath + (*ite).sFile))
				{
					ite = list.erase(ite);
				}
				else
					++ite;
			}

			if (!list.empty())
			{
				e_ret = e_f_mov;
				break;
			}


			// 复制本地的列表
			for (ite = locallist.begin(); ite != locallist.end(); ++ite)
			{
				MiscMyMoveFile(strTmpPath+(*ite).sFile, (*ite).sFile);
			}

			for (ite = locallist.begin(); ite != locallist.end();)
			{
				nHash = strtoul((*ite).sHash, NULL ,10);
				if (nHash == SuperFastHashFile(strFilePath + (*ite).sFile))
				{
					ite = locallist.erase(ite);
				}
				else
					++ite;
			}

			if (!locallist.empty())
			{
				e_ret = e_f_mov;
				break;
			}

			//获取增量更新列表
			char szUpdateTip[1024];
			string strPhltFile = strTmpPath.GetString();
			strPhltFile += "\\patch.phlt";
			if(PathFileExists(strPhltFile.c_str()))
			{
				FILE* fp = fopen(strPhltFile.c_str(),"rb");
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
						strSrcFile = strTmpPath.GetString();
						strDstFile = strFilePath.GetString();
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

						bool bFlag = false;
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

					DeleteFile(strPhltFile.c_str());
				}
				else
				{
					LOG_ERROR("open file %s fail",strPhltFile.c_str());
				}
			}

			e_ret = e_success;
		}
		else
		{
			
			//保存到本地列表
			CreateHashFile(sPath, list, locallist);
			//
			e_ret = e_download_success;
		}

	}
	
	if (!pDlg->m_bSelfUpdate && (e_ret == e_success || e_ret == e_newest || e_ret == e_download_newest))
	{
		char sRun[MAX_PATH];
		strcpy(sRun, strFilePath);
		strcat(sRun, RUN_APP);
		char sAppend[MAX_PATH] = "";

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		si.wShowWindow = SW_SHOW;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		if (CreateProcess(sRun,sAppend,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
	else if( !pDlg->m_bSelfUpdate && e_ret > e_success )
	{
		switch (e_ret)
		{
		case e_f_dir:
			OutputDebugString("dir fail");
			break;
		case e_f_dat:
			OutputDebugString("dat fail");
			break;
		case e_f_down:
			OutputDebugString("down fail");
			break;
		case e_f_mov:
			OutputDebugString("mov fail");
			break;
		default:
			break;
		}
	}
	else if( pDlg->m_bSelfUpdate && e_ret > e_success )
	{
		pDlg->m_updateParam.updateInfoParam.nType = eUpdateType_Failed;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
	}
	else if( pDlg->m_bSelfUpdate && e_ret == e_newest )
	{
		pDlg->m_updateParam.updateInfoParam.nType = eUpdateType_Newset;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));

	}
	else if( pDlg->m_bSelfUpdate && (e_ret == e_download_newest || e_ret == e_download_success ) )
	{
		pDlg->m_updateParam.updateInfoParam.nType = eUpdateType_Update_Success;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
	}

	pDlg->SendMessage(WM_MYMESSAGE, e_act_close, NULL);
	return 0;
}
