//===========================================================================
// FileName:				Statistics.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"
#include "Util/Util.h"
#include "ThirdParty/json/json.h"
#include "Statistics.h"


CStatistics::CStatistics()
{

}

CStatistics::~CStatistics()
{

}

BOOL CStatistics::Initialize()
{
	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, "%s\\version.dat", GetLocalPath().c_str());

	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, szPath, "rb");
	if( err != 0 )
		return FALSE;

	char szVersionCode[MAX_PATH] = {0};
	fread(szVersionCode, 1, MAX_PATH, fp);
	fclose(fp);

	m_strVersionCode = szVersionCode;
	m_dwUserId = 0;
	return TRUE;
}

BOOL CStatistics::Destory()
{
	return TRUE;
}

void CStatistics::Report(int nStatType, DWORD dwUserId, CStream* pParamStream/* = NULL*/)
{
	CHttpDownloadManager* pDownloadManager = HttpDownloadManager::GetInstance();
	if( pDownloadManager == NULL )
		return;
 
	time_t t = time(NULL);

	Json::Value root;
	Json::Value params(Json::arrayValue);

	if(dwUserId == 0)
	{
		dwUserId=m_dwUserId;
	}

	root["userId"]		= (int)dwUserId;
	root["eventId"]		= nStatType;
	root["versionCode"] = m_strVersionCode;
	root["params"]		= params;
#ifdef DEVELOP_VERSION
	root["ver"]			= 1;
#else
	root["ver"]			= 0;
#endif

	Json::FastWriter writter; 
	std::string str = writter.write(root);
 
	string strPost = "data=" + str;

	DWORD dwTaskId = pDownloadManager->AddTask(_T("p.101.com"), _T("/101ppt/pptEvent.php"), _T(""), _T("POST"), strPost.c_str(), 80, 
												MakeHttpDelegate(this, &CStatistics::OnStatisticsReported), MakeHttpDelegate(NULL), MakeHttpDelegate(NULL));

	if( dwTaskId != 0 )
		pDownloadManager->SetTaskPriority(dwTaskId, LOWEST_DOWNLOAD_PRIORITY);
}

bool CStatistics::OnStatisticsReported(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	return true;
}