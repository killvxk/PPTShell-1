#include "stdafx.h"
#include "AutoUpdate.h"
#include "Net.h"
#include "UnRarAuto.h"
#include "SuperFastHash.h"
#include "tools.h"
#include "APClientInterface.h"
#include <shlwapi.h>
#include "UpdateLib.h"
//#define LOCAL_DEBUG


BYTE*			g_pUpdateBuff = NULL;

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

#define LONG_PATH 2048

#define GLOBAL_MEMORY_NAME L"Global\\101PPTUpdateSharedMemory"
unsigned int AutoUpdate(HWND hwnd, bool bSelfUpdate, const char* szWorkDirectory)
{
	vector<string> vecDownInfo;
	vector<UPDATEDATA> list;
	vector<UPDATEDATA> locallist;//增加一个本地的list
	string strTmpPath = "";
	E_RET e_ret = e_newest;
	char sUrl[URL_LEN_MAX];
	char sPath[LONG_PATH];
	bool bUrlOk = true;
	bool bPipeOpen = false;
	char szFilePath[LONG_PATH];
	string strWorkDirectory(szWorkDirectory);
	
	UPDATESTATE state;

	CAPClientInterface APClient;

	HANDLE hFileMapping = NULL;
	if(bSelfUpdate)
	{
		hFileMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE,GLOBAL_MEMORY_NAME);
		if( hFileMapping == NULL )
		{
			hFileMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, GLOBAL_MEMORY_NAME);
			if( hFileMapping == NULL )
			{
				goto _Out;
			}
		}

		g_pUpdateBuff = (BYTE*)MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE , 0, 0, 0x1000);
		if(g_pUpdateBuff == NULL)
		{
			goto _Out;
		}

		memset(&APClient.m_updateParam, 0x00, sizeof(APClient.m_updateParam));

		APClient.m_updateParam.updateInfoParam.nType = eUpdateType_Update_Processing;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&APClient.m_updateParam,sizeof(UpdateInfo_Param));
	}
	
	for(int ik = 0;ik< 1;ik++)
	{
		_HasUpdate context;
		memset(&context, 0, sizeof(context));
		
		//GetModuleFileName(NULL, szFilePath, LONG_PATH);

		strcpy(sPath, strWorkDirectory.c_str());
		strcpy(szFilePath, sPath);
		SetCurrentDirectory(strWorkDirectory.c_str());
		strcat(sPath, UPDATE_TEMP_DIR);
		strTmpPath = sPath;
		

// 		if (NULL != pDlg->m_hInPipe && NULL != pDlg->m_hOutPipe)
// 		{
// 			// 管道校验
// 			bPipeOpen = true;
// 		}
		
		// 检测版本号
		char szVersionFile[MAX_PATH];
		strcpy(szVersionFile,szFilePath);
		strcat(szVersionFile,VERSION_DAT);
		char szRes[32] = "0.0.0.0";
		FILE * fp = fopen(szVersionFile,"r");
		if(fp)
		{
			fscanf(fp,"%s\n",szRes);
			fclose(fp);
		}
		
		// 下载版本号文件
		
		::SendMessage(hwnd, WM_MYMESSAGE, e_act_dat, NULL);
		char* sVersionBuff = NULL;
		strcpy(sUrl, UPDATE_URL);
		strcat(sUrl, VERSION_DAT);

		LOG_TRACE("---AutoUpdate url:%s---", sUrl);
		if (!net.DownData(sUrl, "null", "null", &sVersionBuff))
		{
			bUrlOk = false;
			strcpy(sUrl, UPDATE_IP);
			strcat(sUrl, VERSION_DAT);
			LOG_ERROR("---AutoUpdate fail switch to ip:%s---", sUrl);
			if (!net.DownData(sUrl, "null", "null", &sVersionBuff))
			{
				LOG_ERROR("---AutoUpdate update fail ---");
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
	
		char* pp = sIniBuff;
		char* p;
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
		string strICRPlayerPath = szFilePath;
		strICRPlayerPath += _T("\\icrplayer\\ND_ICR_WPF_PLAYER.exe");

		DWORD ret = GetFileAttributes(strICRPlayerPath.c_str());

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

		string strTempFile;
		unsigned int nHash = 0;
		for (ite = list.begin(); ite != list.end();)
		{
			strTempFile = _T("temp\\");
			strTempFile += (*ite).sFile;

			nHash = strtoul((*ite).sHash, NULL ,10);
			if (nHash == SuperFastHashFile((*ite).sFile))
			{
				//增加检测temp目录
				if(isFileExsit((char *)strTempFile.c_str()))
				{
					DeleteFile((char *)strTempFile.c_str());
				}
				//
				ite = list.erase(ite);
			}
			else
			{
				e_ret = e_download_newest;

				if (nHash == SuperFastHashFile((char *)strTempFile.c_str()))
				{
					//增加检测temp目录

					locallist.push_back(*ite);//增加到本地列表
					ite = list.erase(ite);
					//
				}
				else
					++ite;
			}
		}
		
		//增加增量更新部分
		//vector<string> vecDownInfo;

		char szIncrementVerFile[MAX_PATH * 2];
		strcpy(szIncrementVerFile,szFilePath);
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

			if (!APClient.CreateTempFile(szInfofileName))
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

			char* szRead = NULL;
			//if (!net.DownFile(szUrl, (char *)strXmlFileName.c_str()))
			if (!net.DownData(szUrl, "null", "null", &szRead))
			{
				LOG_ERROR("download %s fail", szUrl);
				break;
			}

			HANDLE hFile = CreateFile(strXmlFileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				LOG_ERROR("open file %s fail.error:%d", strXmlFileName.c_str(), GetLastError());
				DeleteFile(szInfofileName);
				break;
			}

			//if (0 == GetFileSize(hFile, 0))
			if (0 == strlen(szRead))
			{
				CloseHandle(hFile);
				DeleteFile(szInfofileName);
				break;
			}

			//char szRead[16];
			DWORD dwRead;

			//ZeroMemory(szRead, sizeof(szRead));
			//ReadFile(hFile, szRead, sizeof(szRead), &dwRead, NULL);
			WriteFile(hFile, szRead, strlen(szRead), &dwRead, NULL);
			if(dwRead != strlen(szRead))
			{
				CloseHandle(hFile);
				DeleteFile(szInfofileName);
				break;
			}
			CloseHandle(hFile);

			if (0 == stricmp(szRead, "NewestVersion"))
			{
				if (szRead)  delete szRead;
				DeleteFile(szInfofileName);
				break;
			}
			else if (strCurVersion == "0")
			{
				if (szRead) delete szRead;
				vecDownInfo.push_back(strXmlFileName);
				break;
			}
			if (szRead)  delete szRead;

			if (!APClient.GetFileVersionFromXml(strCurVersion, strXmlFileName.c_str()))
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
			::SendMessage(hwnd, WM_MYMESSAGE, e_act_show, NULL);//有更新
		
			::SendMessage(hwnd, WM_MYMESSAGE, e_act_down, NULL);// 下载更新

			APClient.m_updateParam.dwDownloadCount = list.size() + locallist.size();
			APClient.m_updateParam.dwDownloadCount += APClient.m_updateParam.dwDownloadCount;//增量
			APClient.m_updateParam.dwDownloadIndex = locallist.size();
			APClient.m_updateParam.updateInfoParam.dwTotalProgress = ((float)APClient.m_updateParam.dwDownloadIndex / APClient.m_updateParam.dwDownloadCount) * 100;
			APClient.m_updateParam.dwPreTotalProgress = APClient.m_updateParam.updateInfoParam.dwTotalProgress;

			::SendMessage(hwnd, WM_MYMESSAGE, e_val_all, APClient.m_updateParam.dwDownloadCount);
		}

		if(!list.empty())//从服务端下载普通更新
		{
			char* szUpdateInfo = NULL;

			bool bCalcHashSuccess = false;
			int nIndex = 0;
			for(int j = 0;j<list.size();j++)
			{

			//for (ite = list.begin(); ite != list.end(); ++ite)
			//{
				bCalcHashSuccess = false;
				if (bUrlOk)
					strcpy(sUrl, UPDATE_URL);
				else
					strcpy(sUrl, UPDATE_IP);
				strcat(sUrl, list[j].sFile);
				string strUrl = sUrl;
				strUrl=replace_all_distinct(strUrl, "\\","//");

				strncpy(APClient.m_updateParam.updateInfoParam.szFileName, list[j].sFile, MAX_PATH - 1);
				APClient.m_updateParam.updateInfoParam.fDownloadSpeed = 0;
				if(g_pUpdateBuff)
					memcpy(g_pUpdateBuff,&APClient.m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));

				for (int i=0; i<3; ++i)
				{
					APClient.m_updateParam.updateInfoParam.dwTotalProgress = APClient.m_updateParam.dwPreTotalProgress;
					if(g_pUpdateBuff)
						memcpy(g_pUpdateBuff,&APClient.m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
					if (net.DownFile((char *)strUrl.c_str(), sPath, list[j].sFile, NULL, &APClient))
					{
						nHash = strtoul(list[j].sHash, NULL ,10);
						string strTmpPath1 = strTmpPath;
						strTmpPath1 += list[j].sFile;
						if (nHash == SuperFastHashFile(strTmpPath1.c_str()))
						{
							bCalcHashSuccess = true;
							list[j].bDown = true;
							break;
						}
					}
				}

				nIndex++;
				
				if(!bCalcHashSuccess)
				{
					LOG_ERROR("---AutoUpdate Download fail %s---", strUrl.c_str());
					break;
				}

				APClient.m_updateParam.dwDownloadIndex++;
				APClient.m_updateParam.updateInfoParam.dwCurrentProgress = 0;
				APClient.m_updateParam.updateInfoParam.dwTotalProgress = ((float)APClient.m_updateParam.dwDownloadIndex / APClient.m_updateParam.dwDownloadCount) * 100;
				APClient.m_updateParam.dwPreTotalProgress = APClient.m_updateParam.updateInfoParam.dwTotalProgress;
				if(g_pUpdateBuff)
					memcpy(g_pUpdateBuff,&APClient.m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
				::SendMessage(hwnd, WM_MYMESSAGE, e_val_down, APClient.m_updateParam.dwDownloadIndex);

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
			if(!APClient.PatchStart(szFilePath, vecDownInfo))
			{
				e_ret = e_f_down;
				break;
			}
			//
		}

		if(!list.empty() || vecDownInfo.size() > 0)
			::SendMessage(hwnd, WM_MYMESSAGE, e_val_down, APClient.m_updateParam.dwDownloadCount);

		string strTmpPath1;
		if(!bSelfUpdate)
		{
			SendMessage(hwnd, WM_MYMESSAGE, e_act_move, NULL);
			// 复制下载的列表
			vector<UPDATEDATA> rarlist;
			for (ite = list.begin(); ite != list.end(); ++ite)
			{
				if((*ite).bDown)
				{
					strTmpPath1 = strTmpPath;
					strTmpPath1 += (*ite).sFile;
					if (!MiscMyMoveFile(strTmpPath1.c_str(), (*ite).sFile))
					{
					}
					if ((*ite).bDecomp)
						rarlist.push_back(*ite);
				}
			}

			for (ite = list.begin(); ite != list.end();)
			{
				nHash = strtoul((*ite).sHash, NULL ,10);
				strTmpPath1 = strTmpPath;
				strTmpPath1 += (*ite).sFile;
				if (nHash == SuperFastHashFile(strTmpPath1.c_str()))
				{
					ite = list.erase(ite);
				}
				else
				{
					LOG_ERROR("---SuperFastHashFile fail %s---", strTmpPath1.c_str());
					++ite;
				}
			}

			if (!list.empty())
			{
				e_ret = e_f_mov;
				break;
			}


			// 复制本地的列表
			for (ite = locallist.begin(); ite != locallist.end(); ++ite)
			{
				strTmpPath1 = strTmpPath;
				strTmpPath1 += (*ite).sFile;
				MiscMyMoveFile(strTmpPath1.c_str(), (*ite).sFile);
			}

			for (ite = locallist.begin(); ite != locallist.end();)
			{
				nHash = strtoul((*ite).sHash, NULL ,10);
				strTmpPath1 = strTmpPath;
				strTmpPath1 += (*ite).sFile;
				if (nHash == SuperFastHashFile(strTmpPath1.c_str()))
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
			string strPhltFile = strTmpPath;
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
						strSrcFile = strTmpPath;
						strDstFile = szFilePath;
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
// 							::MessageBox(hwnd, szUpdateTip, _T("提示"), MB_OK);
// 							::PostMessage(hwnd, WM_CLOSE, 0 ,0);
							LOG_ERROR(szUpdateTip);
							goto _Out;
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
	
_Out:

	if (!bSelfUpdate && (e_ret == e_success || e_ret == e_newest || e_ret == e_download_newest))
	{
		/*
		char sRun[MAX_PATH];
		strcpy(sRun, szFilePath);
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
		*/
	}
	else if( !bSelfUpdate && e_ret > e_success )
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
	else if( bSelfUpdate && e_ret > e_success )
	{
		APClient.m_updateParam.updateInfoParam.nType = eUpdateType_Failed;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&APClient.m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
	}
	else if( bSelfUpdate && e_ret == e_newest )
	{
		APClient.m_updateParam.updateInfoParam.nType = eUpdateType_Newset;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&APClient.m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
	}
	else if( bSelfUpdate && (e_ret == e_download_newest || e_ret == e_download_success ) )
	{
		APClient.m_updateParam.updateInfoParam.nType = eUpdateType_Update_Success;
		if(g_pUpdateBuff)
			memcpy(g_pUpdateBuff,&APClient.m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
	}

	if(hFileMapping)
	{
		CloseHandle(hFileMapping);
	}
	::SendMessage(hwnd, WM_MYMESSAGE, e_act_close, NULL);
	return e_ret;
}
