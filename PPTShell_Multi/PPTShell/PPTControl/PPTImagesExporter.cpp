#include "stdafx.h"
#include "PPTImagesExporter.h"
#include "PPTController.h"
#include "PPTControllerManager.h"
#include "Util/Util.h"
#include <algorithm>
using std::sort;

#define CurrentPPT		((LPCTSTR)_T("CurrentPPT"))
PPTImagesExporter*		PPTImagesExporter::m_pInstance = NULL;
deque<ExportTask*>		PPTImagesExporter::m_deqTasks;
map<DWORD,ExportTask*>	PPTImagesExporter::m_mapTasks;
CRITICAL_SECTION		PPTImagesExporter::m_lock;
HANDLE					PPTImagesExporter::m_hExportThread;
HWND					PPTImagesExporter::m_hOwner	= NULL;
PPTImagesExporter::PPTImagesExporter()
{

}

PPTImagesExporter::~PPTImagesExporter()
{

}

PPTImagesExporter * PPTImagesExporter::GetInstance()
{
	if (m_pInstance == NULL)
	{
		InitializeCriticalSection(&m_lock);
		m_pInstance = new PPTImagesExporter();
	}

	return m_pInstance;
}

DWORD PPTImagesExporter::ExportCurrentPPTToImages( CDelegateBase& OnCompleted, int nStartIndex /*= -1*/, int nEndIndex /*= -1*/, LPCTSTR lptcsExt /* =_T("jpg")*/ )
{
	return ExportImages(CurrentPPT, 0, 0, OnCompleted, nStartIndex, nEndIndex, false, lptcsExt);
}

DWORD PPTImagesExporter::ExportCurrentPPTTo3DImages( LPCTSTR lptcsDir, CDelegateBase& OnCompleted, int nStartIndex /*= -1*/, int nEndIndex /*= -1*/, LPCTSTR lptcsExt /* =_T("jpg")*/ )
{
	return ExportImages(lptcsDir, -1, -1, OnCompleted, nStartIndex, nEndIndex, false, lptcsExt);
}


DWORD PPTImagesExporter::ExportImages( LPCTSTR lptcsPath, int nWidth, int nHeight, CDelegateBase& OnCompleted, int nStartIndex /*= -1*/, int nEndIndex /*= -1*/, bool bUseExists /*= true*/, LPCTSTR lptcsExt /* =_T("jpg")*/ )
{
	ExportTask* pTask = new ExportTask;

	pTask->strPath		= lptcsPath;
	pTask->OnCompleted	+= OnCompleted;
	pTask->dwTaskId		= (DWORD)GetCycleCount();
	pTask->nWidth		= nWidth;
	pTask->nHeight		= nHeight;
	pTask->bShouldCancel= false;
	pTask->bUseExists	= bUseExists;
	pTask->nStartIndex	= nStartIndex;
	pTask->nEndIndex	= nEndIndex;
	pTask->strExt		= lptcsExt;

	EnterCriticalSection(&m_lock);
	if (!m_hExportThread)
	{
		m_hExportThread = (HANDLE)_beginthread(ExportThread, 0, this);
	}
	m_deqTasks.push_back(pTask);
	m_mapTasks.insert(make_pair(pTask->dwTaskId, pTask));
	LeaveCriticalSection(&m_lock);

	return pTask->dwTaskId;
}

DWORD PPTImagesExporter::ExportImages( LPCTSTR lptcsPath, CDelegateBase& OnCompleted, bool bUseExists /*= true*/ )
{
	return ExportImages(lptcsPath, 0, 0, OnCompleted, -1, -1, bUseExists);
}


void PPTImagesExporter::CancelExport( DWORD dwExportId )
{

	if (!dwExportId)
	{
		return;
	}

	EnterCriticalSection(&m_lock);
	map<DWORD,ExportTask*>::iterator itor = m_mapTasks.find(dwExportId);
	if (itor != m_mapTasks.end())
	{
		itor->second->OnCompleted.clear();
		itor->second->bShouldCancel = true;
	}
	LeaveCriticalSection(&m_lock);
}

void PPTImagesExporter::ExportThread( void* )
{
	CPPTController* pControl	= GetPPTController();
	ExportTask* pTask			= NULL;
	while(true)
	{	
		EnterCriticalSection(&m_lock);
		if (pTask)
		{
			map<DWORD,ExportTask*>::iterator itor = m_mapTasks.find(pTask->dwTaskId);
			if (itor != m_mapTasks.end())
			{
				m_mapTasks.erase(itor);
			}
			delete pTask;
			pTask = NULL;
		}
		if (m_deqTasks.empty())
		{
			LeaveCriticalSection(&m_lock);
			break;
		}
		pTask = m_deqTasks.front();
		m_deqTasks.pop_front();
		LeaveCriticalSection(&m_lock);

		ExportNotify notify;
		notify.dwErrorCode	= 0;
		notify.strDir		= _T("");
		notify.strPath		= pTask->strPath;
		notify.lptcsExt		= pTask->strExt.c_str();
		notify.dwExportId	= pTask->dwTaskId;
		notify.nCount		= 0;

		if (pTask->bShouldCancel)
		{
			continue;
		}


		if (pTask->strPath == _T("")
			&& pTask->strPath != CurrentPPT)
		{
			notify.dwErrorCode = 1;
			::SendMessage(m_hOwner, WM_USER_PPT_EXPOERT_IMAGES, (WPARAM)&pTask->OnCompleted, (LPARAM)&notify);
			continue;
		}

		if (!GenarateThumbnailDir(pTask->strPath.c_str(), notify.strDir))
		{
			notify.dwErrorCode = 2;
			::SendMessage(m_hOwner, WM_USER_PPT_EXPOERT_IMAGES, (WPARAM)&pTask->OnCompleted, (LPARAM)&notify);
			continue;
		}

		if(pTask->strPath == CurrentPPT)
		{
			CreateDirectory(notify.strDir.c_str(), NULL);
			if (!pControl->ExportToImages((LPTSTR)notify.strDir.c_str(), (LPTSTR)notify.lptcsExt, pTask->nStartIndex, pTask->nEndIndex, pTask->nWidth, pTask->nHeight,  &pTask->bShouldCancel))
			{
				notify.dwErrorCode = 3;
				::SendMessage(m_hOwner, WM_USER_PPT_EXPOERT_IMAGES, (WPARAM)&pTask->OnCompleted, (LPARAM)&notify);
				continue;
			}
			notify.nStartIndex = pTask->nStartIndex;
			notify.nCount = pTask->nEndIndex - pTask->nStartIndex + 1;

		}
		else
		{

			if (pTask->bUseExists)
			{
				int nCountInFile = CountSlides(notify.strDir.c_str(), notify.lptcsExt);
				CreateDirectory(notify.strDir.c_str(), NULL);
				if (!pControl->ExportToImages((LPTSTR)pTask->strPath.c_str(), (LPTSTR)notify.strDir.c_str(), (LPTSTR)notify.lptcsExt, &pTask->bShouldCancel, &notify.nCount, nCountInFile, -1, nCountInFile, pTask->nWidth, pTask->nHeight))
				{
					notify.dwErrorCode = 3;
					::SendMessage(m_hOwner, WM_USER_PPT_EXPOERT_IMAGES, (WPARAM)&pTask->OnCompleted, (LPARAM)&notify);
					continue;
				}
			}
			else
			{
				CreateDirectory(notify.strDir.c_str(), NULL);
				if (!pControl->ExportToImages((LPTSTR)pTask->strPath.c_str(), (LPTSTR)notify.strDir.c_str(), (LPTSTR)notify.lptcsExt, &pTask->bShouldCancel, &notify.nCount, -1, -1, 0, pTask->nWidth, pTask->nHeight))
				{
					notify.dwErrorCode = 3;
					::SendMessage(m_hOwner, WM_USER_PPT_EXPOERT_IMAGES, (WPARAM)&pTask->OnCompleted, (LPARAM)&notify);
					continue;
				}
			}
		}

		::SendMessage(m_hOwner, WM_USER_PPT_EXPOERT_IMAGES, (WPARAM)&pTask->OnCompleted, (LPARAM)&notify);

	}
	EnterCriticalSection(&m_lock);
	m_hExportThread = NULL;
	LeaveCriticalSection(&m_lock);

}

void PPTImagesExporter::SetOwner( HWND hWnd )
{
	m_hOwner = hWnd;
}

bool PPTImagesExporter::GenarateThumbnailDir( LPCTSTR lpctsPath, tstring& strDir )
{

	tstring strPath = lpctsPath;
	if (strPath == CurrentPPT)
	{
		strDir = GetLocalPath();
		strDir += _T("\\Cache\\PPTSlides\\");
		strDir += CurrentPPT;

		return true;
	}

	DWORD dwCrc = CalcFileCRC(strPath.c_str());
	if (!dwCrc)
	{
		return false;
	}

	TCHAR szFolderPath[MAX_PATH]= {0};
	TCHAR szDrive[8]			= {0};
	TCHAR szDir[MAX_PATH]		= {0};
	TCHAR szFileName[MAX_PATH]	= {0};
	TCHAR szExt[8]				= {0};
	if (_tsplitpath_s(strPath.c_str(), szDrive, szDir, szFileName, szExt))
	{
		return false;
	}

	int nLen = _tcslen(szFileName);
	if ( nLen > MAX_PATH/2 )
		szFileName[MAX_PATH/2] = _T('\0');

	_stprintf_s(szFolderPath, _T("%s\\Cache\\PPTSlides\\%s_%08x"), GetLocalPath().c_str(), szFileName, dwCrc);

	strDir = szFolderPath;
	return true;
}

bool compare_name(const tstring& pfirst,const tstring& psecond)
{             
	TCHAR szExt[8] = {0};
	int		nIndex1 = 0;
	int		nIndex2 = 0;

	_stscanf_s(pfirst.c_str(), _T("Slide_%d.%s"), &nIndex1, szExt, _countof(szExt));
	_stscanf_s(psecond.c_str(), _T("Slide_%d.%s"), &nIndex2, szExt, _countof(szExt));

	return nIndex1 >= nIndex2 ? false : true;
}

int PPTImagesExporter::CountSlides( LPCTSTR lptcsPath, LPCTSTR lptcsExt )
{
	tstring strPath = lptcsPath;
	strPath += _T("\\*.");
	strPath += lptcsExt;

	CFileFind finder;
	BOOL working = finder.FindFile(strPath.c_str());

	vector<tstring> vctrFileName;
	while (working)
	{
		Sleep(1);
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			continue;
		}

		TCHAR szExt[8] = {0};
		int		nIndex = 0;
		if (_stscanf_s(finder.GetFileName(), _T("Slide_%d.%s"), &nIndex, szExt, _countof(szExt)) != 2)
		{
			continue;
		}

		if (_tcsicmp(szExt, lptcsExt) != 0)
		{
			continue;
		}

		vctrFileName.push_back(finder.GetFileName().GetString());
	}

	std::sort(vctrFileName.begin(), vctrFileName.end(), compare_name);


	int count = 0;
	for (int i = 0; i < (int)vctrFileName.size(); ++i)
	{
		TCHAR szExt[8] = {0};
		int		nIndex = 0;
		if (_stscanf_s(vctrFileName.at(i).c_str(), _T("Slide_%d.%s"), &nIndex, szExt, _countof(szExt)) != 2)
		{
			continue;
		}

		if (count + 1 != nIndex)
		{
			break;
		}
		count++;
	}

	return count;
}

