#include "stdafx.h"
#include "tools.h"
#include "Tinyxml/tinyxml.h"
#include "Util.h"
#include "Net.h"
#include "7z.h"
#include "XdeltaWrapper.h"
#include <algorithm>
#include "SuperFastHash.h"
#include "APClientInterface.h"

CAPClientInterface::CAPClientInterface()
{

}

CAPClientInterface::~CAPClientInterface()
{

}

bool CAPClientInterface::CreateTempFile(char* szInfoFileName)
{
	DWORD dwRetVal = 0;

	LOG_TRACE("---CreateTempFile start---");

	dwRetVal = GetTempPath(MAX_PATH * 2, szInfoFileName);
	if (dwRetVal > MAX_PATH * 2 || (dwRetVal == 0))
	{
		LOG_ERROR("GetTempPath failed with error:%d", GetLastError());
		return false;
	}

	dwRetVal = GetTempFileName(szInfoFileName, "", 0, szInfoFileName);
	if (dwRetVal == 0)
	{
		LOG_ERROR("Create temp File failed with error:%d", GetLastError());
		return false;
	}

	LOG_TRACE("---CreateTempFile end---");
	return true;
}


bool CAPClientInterface::GetFileVersionFromXml(string& strVersion, const char* szXmlName)
{
	LOG_TRACE("---GetFileVersionFromXml start---");
	LOG_TRACE("File:%s", szXmlName);

	if (!szXmlName)
	{
		LOG_ERROR("GetFileVersionFromXml: open xml file fail:%s", szXmlName);
		return false;
	}

	TiXmlDocument doc;
	bool result = doc.LoadFile(szXmlName);

	if (!result)
	{
		LOG_ERROR("GetFileVersionFromXml: open xml file fail:%s", szXmlName);
		return false;
	}

	TiXmlNode* node = doc.FirstChild("info");
	if (!node)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}
	TiXmlNode* node2 = node->FirstChild("mode");
	if (!node2)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}
	TiXmlElement* element = node2->ToElement();
	if (!element)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}
	string str = element->Attribute("patchurl");

	size_t nFirst = str.rfind("_");
	if (nFirst != string::npos)
	{
		size_t nEnd = str.find(".", nFirst);
		if (nEnd != string::npos)
		{
			strVersion = str.substr(nFirst+1, nEnd-nFirst-1);
			LOG_TRACE("---GetFileVersionFromXml end ---");
			return true;
		}
	}

	LOG_TRACE("Parse version error");
	LOG_TRACE("---GetFileVersionFromXml end ---");
	return false;
}

bool CAPClientInterface::PatchStart( const char* szApplicationPath, vector<string>& vecXmlName)
{
	LOG_TRACE("---PatchStart start---");
	vector<string> vecDownFilePath;
	vector<string> vecDecompDir;
	vector<string> vecRedownload;

	if (!DownloadPatch(vecXmlName, vecDownFilePath))
	{
		LOG_ERROR("Download patch fail.");
		return false;
	}

	if (!DecompressPatch(vecDownFilePath, vecDecompDir))//解压patch_
	{
		LOG_ERROR("Decompress Patch Fail");
		return false;
	}

	if (!CoverPatch(szApplicationPath, vecDecompDir, vecRedownload))//对phlt操作
	{
		LOG_ERROR("Xdelta and Check Patch Fail.");
		return false;
	}

	vector<string>::iterator it = vecDecompDir.begin();

	for (; it != vecDecompDir.end(); it++)
	{
		DirectoryDelete(*it);
	}

	LOG_TRACE("---PatchStart end---");

	return true;
}


bool CAPClientInterface::GetDownCountFromXmlFiles(std::vector<string> vecXmls)
{
	LOG_TRACE("---GetDownCountFromXmlFiles---");

	for (UINT i=0; i<vecXmls.size(); ++i)
	{
		MAP_MODEINFO mapModeInfo;
		LONGLONG lFileSize = 0;
		int nRnd = -1;
		int nPos = 0;
		LONG lStop = 0;

		XmlParse(vecXmls[i].c_str(), mapModeInfo);

		MAP_MODEINFO_IT it = mapModeInfo.begin();

		while (it != mapModeInfo.end())
		{
			if (nPos == nRnd)
			{
				nRnd = -1;
				it++;
				continue;
			}
			if (-1 == nRnd)
			{
				nRnd = rand() % (int)it->second.size();
				nPos = nRnd;
			}
			else if (nRnd == it->second.size())
			{
				nRnd = 0;
				continue;
			}

			lFileSize = GetNetFileSize((char*)it->second[nRnd].strPathUrl.c_str(), &lStop);
			if (lFileSize>0)
			{
				break;
			}

			nRnd++;
		}

		// 一个有效的xml文件对应一个状态

		if (lFileSize <= 0)
		{
			return false;
		}
		else
		{
			State fs;

			fs.bEnd = false;
			fs.nComplete = 0;
			fs.nTotal = (UINT64)lFileSize;
			m_mapRevolve[vecXmls[i]] = fs;
		}
	}

	LOG_TRACE("---GetDownCountFromXmlFiles end---");
	return true;
}

bool CAPClientInterface::DownloadPatch(vector<string>& vecXmlName, vector<string>& vecDownFilePath)
{
	LOG_TRACE("---DownloadPatch start---");

	if (!GetDownCountFromXmlFiles(vecXmlName))//解析xml
	{
		LOG_ERROR("can't get all download file property");
		return false;
	}

	for (UINT i=0; i<vecXmlName.size(); ++i)
	{
		string strDownFilePath;

		if (!DownloadFile((char *)vecXmlName[i].c_str(), strDownFilePath))
		{
			LOG_ERROR("down patch from %s fail.", strDownFilePath.c_str());
			return false;
		}

		vecDownFilePath.push_back(strDownFilePath);
	}

	LOG_TRACE("---DownloadPatch end---");
	return true;
}

bool CAPClientInterface::XmlParse(const char* szXmlName, MAP_MODEINFO& mapMode)
{
	LOG_TRACE("---XmlParse start---");
	LOG_TRACE("XmlFile:%s", szXmlName);

	if (!szXmlName)
	{
		return false;
	}

	mapMode.clear();

	TiXmlDocument doc;
	bool result = doc.LoadFile(szXmlName);

	if (!result)
	{
		LOG_ERROR("XmlParse: open xml file fail:%s", szXmlName);
		return false;
	}

	TiXmlNode* node = doc.FirstChild("info");
	if (!node)
	{
		LOG_ERROR("parse xml fail.%s", szXmlName);
		return false;	
	}

	TiXmlNode* node2 = node->FirstChild("mode");
	ModeInfo modeInfo;

	while (node2)
	{
		TiXmlElement* element = node2->ToElement();
		if (!element)
		{
			LOG_ERROR("parse xml fail.%s", szXmlName);
			return false;	
		}

		ZeroMemory(&modeInfo, sizeof(ModeInfo));
		string strName = element->Attribute("name");
		_snprintf(modeInfo.szModeName, sizeof(modeInfo.szModeName)-1, "%s", strName.c_str());

		modeInfo.strPathUrl = element->Attribute("patchurl");
		modeInfo.strCompressPath = element->Attribute("compresspath");

		string strPri = element->Attribute("pri");
		mapMode[atoi(strPri.c_str())].push_back(modeInfo);

		node2 = node2->NextSibling();
	}

	return true;
}

MAP_MODEINFO g_mapMode;

bool CAPClientInterface::DownloadFile( const char* szXmlName, string& strDownFilePath, bool bIsRedownload /*= false*/)
{	
	if (!bIsRedownload && szXmlName && !XmlParse(szXmlName, g_mapMode))
	{
		return false;
	}

	MAP_MODEINFO_IT it = g_mapMode.begin();
	bool bDownSucc = false;
	int nRnd = -1;
	int nPos = 0;

	CNet net;

	while (it != g_mapMode.end())
	{
		if (nPos == nRnd)
		{
			nRnd = -1;
			it++;
			continue;
		}
		if (-1 == nRnd)
		{
			nRnd = rand() % (int)it->second.size();
			nPos = nRnd;
		}
		else if (nRnd == it->second.size())
		{
			nRnd = 0;
			continue;
		}

		char szDownPath[MAX_PATH * 2];
		ZeroMemory(szDownPath, sizeof(szDownPath));


		if (!bIsRedownload)
		{
			char szName[MAX_PATH * 2];
			char* p = _tcsrchr((char*)it->second[nRnd].strPathUrl.c_str(), _T('/'));
			if(p)
			{
				_tcscpy(szName, p+1);
			}
			strncpy(m_updateParam.updateInfoParam.szFileName, szName, MAX_PATH - 1);

			_snprintf(szDownPath, sizeof(szDownPath)-1, _T("temp\\%s"), szName);

			LOG_INFO("Down url %s",  (char*)it->second[nRnd].strPathUrl.c_str());
			int nRet = net.DownFile((char*)it->second[nRnd].strPathUrl.c_str(), szDownPath, this);
			if (0 == nRet)
			{
				LOG_ERROR("ERROR:download %s fail.", strDownFilePath.c_str());

				return false;
			}

			strDownFilePath = szDownPath;
			//			strDownFilePath += GetFileName(it->second[nRnd].strPathUrl);
		}
		else
		{
			string strUrl = it->second[nRnd].strCompressPath;
			if (!strUrl.empty() && strUrl[strUrl.length()-1] != '\\' && strUrl[strUrl.length()-1] != '/')
			{
				strUrl += "\\";
			}
			strUrl += strDownFilePath;
			strUrl = StrReplace(strUrl, "\\\\", "\\");
			strUrl = StrReplace(strUrl, "\\", "/");

			strncpy(m_updateParam.updateInfoParam.szFileName, strDownFilePath.c_str(), MAX_PATH - 1);
			_snprintf(szDownPath, sizeof(szDownPath)-1, "temp\\%s",  strDownFilePath.c_str());
			strDownFilePath = szDownPath;

			LOG_INFO("ReDownload %s", (char*)strUrl.c_str());
			int nRet = net.DownFile((char*)strUrl.c_str(), szDownPath, this);
			if (nRet > 0)
			{
				//				strDownFilePath += GetFileName(strUrl);
			}
			else
			{
				LOG_ERROR("ERROR:download %s fail.", strUrl.c_str());
				//				strDownFilePath += GetFileName(strUrl);
				return false;
			}
		}

		nRnd++;
	}

	return true;
}

// 分析解压文件大小，大小以KB为单位，一个文件对应一个大小
bool CAPClientInterface::ParseDecompressTotalSize(std::vector<string>& vecDownFilePath)
{
	State ste;

	ste.bEnd = false;
	ste.nComplete = 0;

	m_mapRevolve.clear();

	for (UINT i=0; i<vecDownFilePath.size(); ++i)
	{
		if (SZIP_OK != SevenZipGetDecompressSize(vecDownFilePath[i].c_str(), &ste.nTotal))
		{
			return false;
		}
		ste.nTotal /= 1024;
		m_mapRevolve[vecDownFilePath[i]] = ste;
	}

	return true;
}

bool CAPClientInterface::DecompressPatch(vector<string>& vecDownFilePath, vector<string>& vecDecompDir)
{
	LOG_TRACE("---DecompressPatch start---");

	if (!ParseDecompressTotalSize(vecDownFilePath))
	{
		LOG_ERROR("can't get all decompress file property");
		return false;
	}

	for (UINT i=0; i < vecDownFilePath.size(); ++i)
	{
		string strDecompressDir;

		if (i == 0)
		{
			strDecompressDir = _T("temp\\patch\\");
		}
		else
		{
			strDecompressDir = _T("temp\\patch_\\");
			strDecompressDir += GetFileTitle(vecDownFilePath[i]) + "\\";
		}
		DirectoryDelete(strDecompressDir);
		DirectoryMake(strDecompressDir);


		LOG_TRACE("DECOMPRESS:%s", vecDownFilePath[i].c_str());

		SZIPRES res = SZIP_OK;

		for(int j =0 ; j < 3; j++)
		{
			if (SZIP_OK == (res = SevenZipDecompress(strDecompressDir.c_str(), vecDownFilePath[i].c_str(), NULL)))
			{
				break;
			}
			Sleep(1000);
		}
		if(res != SZIP_OK)
		{
			LOG_ERROR("[解压]:SevenZipDecompress file %s to %s error code:%d", vecDownFilePath[i].c_str(), strDecompressDir.c_str(), res);
			DeleteFile(vecDownFilePath[i].c_str());
			return false;
		}

		DeleteFile(vecDownFilePath[i].c_str());


		string strPhltPath;

		strPhltPath = strDecompressDir;
		strPhltPath += "*.phlt";

		WIN32_FIND_DATA fd;
		HANDLE hFindFile = FindFirstFile(strPhltPath.c_str(), &fd);
		if (INVALID_HANDLE_VALUE == hFindFile)
		{
			LOG_ERROR("[解压]:can't not find phlt file");
			DeleteFile(vecDownFilePath[i].c_str());
			return false;
		}
		if (fd.nFileSizeHigh == 0 && fd.nFileSizeLow == 0)
		{
			LOG_ERROR("[解压]:phlt file is empty");
			DeleteFile(vecDownFilePath[i].c_str());
			FindClose(hFindFile);
			return false;
		}
		FindClose(hFindFile);

		vecDecompDir.push_back(strDecompressDir);
	}

	LOG_TRACE("---DecompressPatch end---");

	return true;
}

void CAPClientInterface::InsertVecRedownload(string strPath, vector<string>& vecRedownload)
{
	vector<string>::iterator result = find( vecRedownload.begin( ), vecRedownload.end( ), strPath ); //查找
	if ( result == vecRedownload.end( ) ) //没找到
	{
		vecRedownload.push_back(strPath);
	}
}

void CAPClientInterface::RemoveVecRedownload(string strPath, vector<string>& vecRedownload)
{
	vector<string>::iterator result = find( vecRedownload.begin( ), vecRedownload.end( ), strPath ); //查找
	if ( result != vecRedownload.end( ) ) //找到
	{
		vecRedownload.erase(result);
	}
}

bool CAPClientInterface::CoverPatch(string strApplicationPath, vector<string>& vecDecompDir, vector<string>& vecRedownload)//对phlt操作
{
	LOG_TRACE("---CoverPatch start---");

	string strPatchFile;
	string strSrcFile;
	string strDestFile;

	map<string, map<string, PatchType>> mapPatchs;
	map<string, map<string, PatchType>>::iterator itPatchs;
	UINT index = 0;
	UINT nCheckFileCount = 0;

	vector<string> vecPatchPath;
	vector<string>::iterator itPatchPath;

	for (UINT i=0; i<vecDecompDir.size(); ++i)
	{
		string strPatchPath;
		map<string, PatchType> mapInfo;

		if (!PatchListParse(vecDecompDir[i].c_str(), mapInfo, strPatchPath))
		{
			LOG_ERROR("分析校验文件失败");
			return false;
		}

		mapPatchs[strPatchPath] = mapInfo;
		nCheckFileCount += (UINT)mapInfo.size();

		// 增加一个vector的主要目的是按顺序校验
		bool bFound = false;

		for (itPatchPath = vecPatchPath.begin(); itPatchPath != vecPatchPath.end(); itPatchPath++)
		{
			if (*itPatchPath == strPatchPath)
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			vecPatchPath.push_back(strPatchPath);
		}
	}

	if (!IsDir(strApplicationPath + PATCH_FILE_DIR "\\"))
	{
		DirectoryMake(strApplicationPath + PATCH_FILE_DIR "\\");
	}

	for (itPatchPath = vecPatchPath.begin(); itPatchPath != vecPatchPath.end(); itPatchPath++)
	{
		itPatchs = mapPatchs.find(*itPatchPath);
		if (itPatchs == mapPatchs.end())
		{
			continue;
		}

		LOG_TRACE("Check: %s", itPatchs->first.c_str());

		string strPatchPath = itPatchs->first;
		map<string, PatchType>& mapPatchInfo = itPatchs->second;
		map<string, PatchType>::iterator it = mapPatchInfo.begin();

		for (; it != mapPatchInfo.end(); it++)
		{
			++index;
			// delete file
			if (!it->second.bAddFile)
			{
				LOG_TRACE("File %s DEL", (*it).first.c_str());
				RemoveVecRedownload((*it).first,vecRedownload);
				// 删除由覆盖程序完成
				continue;
			}
			else
			{
				LOG_TRACE("File %s ADD", (*it).first.c_str());
			}
			// add or modify file

			// 循环更新，先检查PATCH_FILE_DIR目录下的文件是否存在
			strDestFile = strApplicationPath + PATCH_FILE_DIR "\\" + (*it).first;

			if (IsFileExist(strDestFile))
			{
				strSrcFile = strDestFile;
			}
			else
			{
				strSrcFile = strApplicationPath + (*it).first;//多版本更新，叠加
			}

			if (!IsDir(GetParentPath(strDestFile)))
			{
				DirectoryMake(GetParentPath(strDestFile));//创建文件夹目录
			}

			strPatchFile = strPatchPath + (*it).first + ".s";

			if (IsFileExist(strPatchFile))
			{
				if (!CopyFile(strPatchFile.c_str(), strDestFile.c_str(), FALSE))
				{
					LOG_ERROR("copy file:%s to %s fail.", strPatchFile.c_str(), strDestFile.c_str());
					return false;
				}
				else
				{
					RemoveVecRedownload((*it).first,vecRedownload);
					LOG_INFO("copy .sfile:%s to %s success.", strPatchFile.c_str(), strDestFile.c_str());
					//InsertVecCover((*it).first,vecCover);
				}
				continue; // next file
			}

			// 补丁文件
			strPatchFile = strPatchPath + (*it).first + ".patch";
			if (!IsFileExist(strSrcFile))
			{
				LOG_INFO("file:%s not exists.must redownload.", strSrcFile.c_str());
				InsertVecRedownload((*it).first,vecRedownload);
				continue;
			}

			if (!IsFileExist(strPatchFile))
			{
				if ((*it).second.unHashContent != SuperFastHashFile(strSrcFile.c_str()))
				{
					LOG_INFO("file:%s not exists must redownload.", strPatchFile.c_str());
					InsertVecRedownload((*it).first,vecRedownload);
				}
				continue;
			}

			while (!FileReadAllow(strSrcFile))
			{
				{
					char szMsg[1024];
					ZeroMemory(szMsg, sizeof(szMsg));
					_snprintf(szMsg, sizeof(szMsg)-1, "文件%s被占用,\n请关闭应用程序后再试", strSrcFile.c_str());
					if (IDCANCEL == ::MessageBox(NULL, szMsg, "警告", MB_ICONWARNING | MB_RETRYCANCEL))
					{
						if (IDYES == ::MessageBox(NULL, "确定退出更新程序？", "提示", MB_ICONINFORMATION | MB_YESNO))
						{
							exit(0);
							return false;
						}
					}
				}
			}

			UINT unFileHash = 0;
			DWORD64 unFileSize = 0;

			unFileHash = SuperFastHashFile(strSrcFile.c_str());
			unFileSize = GetFileSize(strSrcFile);
			if (unFileHash == (*it).second.unHashContent && (*it).second.unFileLength == unFileSize)
			{
				if (strSrcFile != strDestFile)
				{
					if (!CopyFile(strSrcFile.c_str(), strDestFile.c_str(), FALSE))
					{
						LOG_TRACE("Hash and Size is equal, but copy fail. file:%s  redownload", strDestFile.c_str());
						InsertVecRedownload((*it).first,vecRedownload);
					}
					else
					{
						LOG_INFO("SrcFile:%s exists and hash is equal.success!", strPatchFile.c_str());
						RemoveVecRedownload((*it).first,vecRedownload);
					}
				}
				else
				{
					LOG_INFO("DestFile:%s exists and hash is equal.success!", strPatchFile.c_str());
					RemoveVecRedownload((*it).first,vecRedownload);
				}

				continue;
			}

			LOG_INFO("[合并]xdelta file: %s .........", strDestFile.c_str());
			if (!CXdeltaWrapper::ApplyPatch(strSrcFile.c_str(), strPatchFile.c_str(), strDestFile.c_str()))
			{
				{
					LOG_INFO("[合并]xdelta file %s fail", strDestFile.c_str());

					InsertVecRedownload((*it).first,vecRedownload);

				}
				continue;
			}
			else
			{
				LOG_INFO("[合并]xdelta file: %s Success!",strDestFile.c_str());
			}

			unFileHash = SuperFastHashFile(strDestFile.c_str());
			unFileSize = GetFileSize(strDestFile);
			if (unFileHash != (*it).second.unHashContent || unFileSize != (*it).second.unFileLength)
			{
				LOG_INFO("incorrect of file %s hash or length, redownload.", strDestFile.c_str());

				InsertVecRedownload((*it).first,vecRedownload);
				continue;
			}

			RemoveVecRedownload((*it).first,vecRedownload);
		}
	}

	if (!RedownloadFileList(strApplicationPath, mapPatchs, vecRedownload))
	{
		LOG_ERROR("RedownloadFileList error!");
		return false;
	}

	if (!CopyPatchList(strApplicationPath, mapPatchs))
	{
		LOG_ERROR("copy patchlist file error!");
		return false;
	}

	LOG_TRACE("---CoverPatch end---");

	return true;
}

bool CAPClientInterface::PatchListParse(const char* szDecompressDir, map<string, PatchType>& mapPatchInfo, string& strPatchPath)
{
	if (!szDecompressDir)
	{
		return false;
	}

	LOG_TRACE("---PatchListParse start---");

	string strPhltPath;

	strPhltPath = szDecompressDir;
	strPhltPath += "*.phlt";

	WIN32_FIND_DATA fd;
	HANDLE hFindFile = FindFirstFile(strPhltPath.c_str(), &fd);
	if (INVALID_HANDLE_VALUE == hFindFile)
	{
		LOG_ERROR("can't not find phlt file");
		return false;
	}
	FindClose(hFindFile);

	mapPatchInfo.clear();
	strPhltPath = szDecompressDir;
	strPhltPath += fd.cFileName;

	HANDLE hFile = CreateFile(strPhltPath.c_str(), 
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		LOG_ERROR("CreateFile %s fail with eror %d", strPhltPath.c_str(), GetLastError());
		return false;
	}

	DWORD dwTypeLen = sizeof(PatchType);
	DWORD dwRead = 0;
	PatchType patchType;
	char szFileName[MAX_PATH];

	while (true)
	{
		ReadFile(hFile, &patchType, dwTypeLen, &dwRead, NULL);

		if (dwTypeLen != dwRead)
		{
			break;
		}

		ReadFile(hFile, szFileName, patchType.unFileNameSize, &dwRead, NULL);

		if (patchType.unFileNameSize != dwRead)
		{
			LOG_ERROR("invalid phlt file:%s", strPhltPath.c_str());
			CloseHandle(hFile);
			return false;
		}

		string strFileName;

		CodePageConvert(szFileName, strFileName, 936, CP_ACP);
		mapPatchInfo.insert(std::pair<string, PatchType>(strFileName.c_str(), patchType));
	}
	CloseHandle(hFile);

	unsigned nHead = 0;
	unsigned nEnd = 0;

	sscanf(fd.cFileName, "%u_%u.phlt", &nHead, &nEnd);

	char szPatchPath[MAX_PATH];

	ZeroMemory(szPatchPath, sizeof(szPatchPath));
	_snprintf(szPatchPath, sizeof(szPatchPath)-1, "%s%u_%u_patch\\", szDecompressDir, nHead, nEnd);
	strPatchPath = szPatchPath;

	LOG_TRACE("---PatchListParse end---");

	return true;
}

bool CAPClientInterface::RedownloadFileList(string strApplicationPath, 
									map<string, map<string, PatchType>>& mapPatchs,
									vector<string>& vecRedownload)
{
	LOG_TRACE("---RedownloadFileList start---");
	string strSrcFile;
	string strDestFile;

	map<string, map<string, PatchType>>::reverse_iterator ritPh;
	map<string, PatchType>::iterator it;

	CNet net;
	for (UINT i=0; i<vecRedownload.size(); i++)
	{
		{
			strSrcFile = vecRedownload[i] + ".7z";

			bool bWideChar = false;

			if (!DownloadFile(NULL, strSrcFile, true))
			{
				LOG_ERROR("RedownloadFile From %s fail.", strSrcFile.c_str());
				return false;
			}

			//////////////////////////////////////////////////////////////////////////
			DirectoryMake(strApplicationPath+PATCH_FILE_DIR"\\");

			string strDecomTargetPath = strApplicationPath+PATCH_FILE_DIR;


			// 			if (SZIP_OK != SevenZipDecompress(strDecomTargetPath.c_str(), strSrcFile.c_str(), NULL))
			// 			{
			// 				LOG_ERROR("Extract fail, redownload!%s, %s", strDecomTargetPath.c_str(), strSrcFile.c_str());
			// 				return false;
			// 			}

			SZIPRES res = SZIP_OK;
			for(int j =0 ; j < 3; j++)
			{
				if (SZIP_OK == (res = SevenZipDecompress(strDecomTargetPath.c_str(), strSrcFile.c_str(), NULL)) )
				{
					break;
				}
				Sleep(1000);
			}
			if(res != SZIP_OK)
			{
				LOG_ERROR("Extract fail, redownload!%s, %s", strDecomTargetPath.c_str(), strSrcFile.c_str());
				return false;
			}

			strDestFile = strApplicationPath + PATCH_FILE_DIR"\\" + vecRedownload[i];

			UINT unFileHash = SuperFastHashFile(strDestFile.c_str());
			DWORD64 unFileSize = GetFileSize(strDestFile);

			ritPh = mapPatchs.rbegin();
			while(ritPh != mapPatchs.rend())
			{
				it = ritPh->second.find(vecRedownload[i]);
				if (it != ritPh->second.end())
				{
					break;
				}
				ritPh++;
			}

			if (ritPh == mapPatchs.rend())
			{
				LOG_ERROR("an badly error, where's log of file");
				return false;
			}

			if ((unFileHash != it->second.unHashContent) || (it->second.unFileLength != unFileSize))
			{
				LOG_ERROR("the file %s content error, redownload again!", vecRedownload[i].c_str());
				DeleteFile(strSrcFile.c_str());
				return false;
			}
		}
	}

	LOG_TRACE("---RedownloadFileList end---");
	return true;
}

bool CAPClientInterface::CopyPatchList(string strApplicationPath, map<string, map<string, PatchType>>& mapPatchs)
{
	LOG_TRACE("---CopyPatchList start---");

	map<string, PatchType> mapPick;
	map<string, map<string, PatchType>>::reverse_iterator ritPatchs;

	for (ritPatchs = mapPatchs.rbegin(); ritPatchs != mapPatchs.rend(); ritPatchs++)
	{
		if (!ritPatchs->second.empty())
		{
			mapPick.insert(ritPatchs->second.begin(), ritPatchs->second.end()--);
		}
	}

	string strFileName;

	strFileName = strApplicationPath + PATCH_FILE_DIR "\\patch.phlt";

	HANDLE hFile = CreateFile(strFileName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		LOG_ERROR("CreateFile:%s fail with error code:%d", strFileName.c_str(), GetLastError());
		return false;
	}

	map<string, PatchType>::iterator itInfo;
	PatchType patchType;

	for (itInfo = mapPick.begin(); itInfo != mapPick.end(); itInfo++)
	{
		patchType.bAddFile = itInfo->second.bAddFile;
		patchType.unHashContent = itInfo->second.unHashContent;
		patchType.unFileLength = itInfo->second.unFileLength;
		patchType.unFileNameSize = itInfo->second.unFileNameSize;

		DWORD dwWrite = 0;
		string strCurFileName = (*itInfo).first;

		LOG_TRACE("ADD FILE:%d HASH:%u LEN:%I64d NAME:%s", patchType.bAddFile, patchType.unHashContent, patchType.unFileLength, strCurFileName.c_str());

		if (!WriteFile(hFile, &patchType, sizeof(PatchType), &dwWrite, NULL))
		{
			CloseHandle(hFile);
			LOG_ERROR("写文件%s失败1", strFileName.c_str());
			return false;
		}

		if (!WriteFile(hFile, itInfo->first.c_str(), patchType.unFileNameSize, &dwWrite, NULL))
		{
			CloseHandle(hFile);
			LOG_ERROR("写文件%s失败2", strFileName.c_str());
			return false;
		}
	}

	CloseHandle(hFile);
	LOG_TRACE("---CopyPatchList end---");
	return true;
}