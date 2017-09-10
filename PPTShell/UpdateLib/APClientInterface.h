#pragma once
#include <map>
#include <string>
#include <vector>
#include "AutoUpdate.h"

using namespace std;

struct ModeInfo 
{
	char szModeName[MAX_PATH];
	string strPathUrl;
	string strCompressPath;
};

typedef std::vector<ModeInfo>				VEC_MODEINFO;
typedef std::map<int, VEC_MODEINFO>			MAP_MODEINFO;
typedef std::map<int, VEC_MODEINFO>::iterator MAP_MODEINFO_IT;

struct State 
{
	bool bEnd;
	UINT64 nComplete;
	UINT64 nTotal;
};

struct PatchType
{
	bool bAddFile;
	UINT unHashContent;
	UINT64 unFileLength;
	UINT unFileNameSize;
};

struct PatchInfo
{
	PatchType pt;
	char      szFileName[MAX_PATH];
};

enum E_ACTION;
#define PATCH_FILE_DIR "temp"

class CAPClientInterface
{
public:
	CAPClientInterface();
	~CAPClientInterface();
public:
	bool CreateTempFile(char* szInfoFileName);

	bool GetFileVersionFromXml(string& strVersion, const char* szXmlName);
	bool UrlRandDownload(const char* szUrl, string strCurVersion, string& strDownFileName);

	bool PatchStart(const char* szApplicationPath, vector<string>& vecXmlName);
	bool DownloadPatch(vector<string>& vecXmlName, vector<string>& vecDownFilePath);
	bool GetDownCountFromXmlFiles(std::vector<string> vecXmls);
	bool XmlParse(const char* szXmlName, MAP_MODEINFO& mapMode);
	bool DownloadFile(const char* szXmlName, string& strDownFilePath, bool bIsRedownload = false);
	bool ParseDecompressTotalSize(std::vector<string>& vecDownFilePath);
	bool DecompressPatch(vector<string>& vecDownFilePath, vector<string>& vecDecompDir);
	bool PatchListParse(const char* szDecompressDir, map<string, PatchType>& mapPatchInfo, string& strPatchPath);

	bool CoverPatch(string strApplicationPath, vector<string>& vecDecompDir, vector<string>& vecRedownload);
	bool RedownloadFileList(string strApplicationPath, 
		map<string, map<string, PatchType>>& mapPatchs,
		vector<string>& vecRedownload);
	bool CopyPatchList(string strApplicationPath, map<string, map<string, PatchType>>& mapPatchs);

	void InsertVecRedownload(string strPath, vector<string>& vecRedownload);
	void RemoveVecRedownload(string strPath, vector<string>& vecRedownload);
public:
	AutoUpdate_Param	m_updateParam;
protected:
	std::map<string, State> m_mapRevolve;
};
