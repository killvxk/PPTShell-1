#include "StdAfx.h"
#include "DBankUploadUI.h"
#include "UploadItem.h"
#include "ForAppendItem.h"
#include "Util/FileTypeFilter.h"
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"


#define TimerId_ScrollToBottom		10
#define TimerId_InvalidateProgress	11

CDBankUploadUI::CDBankUploadUI(void)
{
}

CDBankUploadUI::~CDBankUploadUI(void)
{
	this->GetManager()->RemoveNotifier(this);
}


void CDBankUploadUI::Init()
{
	__super::Init();

	m_comboUploadDest	= dynamic_cast<CComboUI*>(FindSubControl(_T("upload_dest")));
	m_layList			= dynamic_cast<CTileLayoutUI*>(FindSubControl(_T("list")));
	m_laySelect			= dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("select_layout")));
	m_layContent		= dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("content_layout")));
	m_lbFileDetail		= dynamic_cast<CLabelUI*>(FindSubControl(_T("detail")));

	m_layList->OnEvent += MakeDelegate(this, &CDBankUploadUI::OnListEvent);

	ShowListUI(false);
	this->GetManager()->AddNotifier(this);

	CGroupExplorerUI::GetInstance()->SetDlgAcceptFiles(true); 
}



CControlUI* CDBankUploadUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("UploadItem"), pstrClass) == 0)
	{
		return new CUploadItemUI;
	}
	else if (_tcsicmp(_T("ForAppendItem"), pstrClass) == 0)
	{
		return new CForAppendItemUI;
	}
	return NULL;
}


void CDBankUploadUI::OnBtnChoose( TNotifyUI& msg )
{
	OnBtnAppend(msg);

	if (m_layList->GetCount() > 1)
	{
		ShowListUI(true);
	}	
}

void CDBankUploadUI::OnBtnStartUpload( TNotifyUI& msg )
{
	for (int i = 0; i < m_layList->GetCount(); ++i)
	{
		CUploadItemUI* pItem = dynamic_cast<CUploadItemUI*>(m_layList->GetItemAt(i));

		if (pItem)
		{
			if (!pItem->IsUploaded() 
				&& !pItem->IsUploading())
			{
				pItem->StartUpload();
			}
		}
		
	}
}

void CDBankUploadUI::OnBtnAppend( TNotifyUI& msg )
{

	vector<tstring> vctrFileList;
	BOOL bRet = FileSelectDialog(_T("选择文件"), FILTER_ALL,
			OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST| OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
			vctrFileList, AfxGetMainWnd()->m_hWnd);

	if ( -1 == bRet )
		CToast::Toast(_STR_SELECT_FILE_BUFFER_SMALL, true);

	if ( FALSE == bRet || -1 == bRet )
		return;

	CreateList(vctrFileList, true);
}

void CDBankUploadUI::ShowListUI( bool bShow )
{
	m_layContent->SetVisible(bShow);
	m_laySelect->SetVisible(!bShow);
}

void CDBankUploadUI::CreateList( vector<tstring>& vctrFileList, bool bAppend )
{

	if (!bAppend)
	{
		m_layList->RemoveAll();
	}

	if (m_layList->GetCount() > 0)
	{
		m_layList->RemoveAt( m_layList->GetCount() - 1 );
	}

	CFileTypeFilter fileTypeFilter;
	for (int i = 0; i < (int)vctrFileList.size(); ++i)
	{
		const TCHAR* lptcsPath	= vctrFileList.at(i).c_str();
		int nLocalItemType = fileTypeFilter.GetFileType(lptcsPath);
		if (nLocalItemType == FILE_FILTER_OTHER)
		{
			continue;
		}

		//get file name

		TCHAR szDrive[8]				= {0};
		TCHAR szDir[MAX_PATH * 2]		= {0};
		TCHAR szFileName[MAX_PATH * 2]	= {0};
		TCHAR szExt[MAX_PATH * 2]		= {0};
		if (_tsplitpath_s(lptcsPath, szDrive, szDir, szFileName, szExt))
		{
			continue;
		}

		CUploadItemUI* pItem = NULL;
		for (int j = 0; j < m_layList->GetCount(); ++j)
		{
			pItem = (CUploadItemUI*)m_layList->GetItemAt(j);
			if (_tcsicmp(pItem->GetResource(), lptcsPath) == 0)
			{
				break;
			}
			pItem = NULL;
		}

		//had already exists
		if (pItem)
		{
			continue;
		}

		if (!m_builder.GetMarkup()->IsValid())
		{
			pItem = dynamic_cast<CUploadItemUI*>(m_builder.Create(_T("Upload\\UploadItem.xml"), NULL, this));
		}
		else
		{
			pItem = dynamic_cast<CUploadItemUI*>(m_builder.Create(this));
		}
	
		LPCTSTR	lptcsType		= NULL;
		LPCTSTR	lptcsBkImage	= NULL;
		switch(nLocalItemType)
		{
			case FILE_FILTER_PPT:
				{
					lptcsType		= _T("课件");
					lptcsBkImage	= _T("RightBar\\Item\\item_bg_course.png");
				}
				break;
			case FILE_FILTER_VIDEO:
				{
					lptcsType = _T("视频");
					lptcsBkImage	= _T("RightBar\\Item\\item_bg_video.png");
				}
				break;
			case FILE_FILTER_PIC:
				{
					lptcsType = _T("图片");
					lptcsBkImage	= lptcsPath;
				}
				break;
			case FILE_FILTER_VOLUME:
				{
					lptcsType = _T("音频");
					lptcsBkImage	= _T("RightBar\\Item\\item_bg_volume.png");
				}
				break;
			case FILE_FILTER_FLASH:
				{
					lptcsType = _T("动画");
					lptcsBkImage	= _T("RightBar\\Item\\item_bg_flash.png");
				}
				break;
			case FILE_FILTER_BASIC_EXERCISES:
			case FILE_FILTER_INTERACTIVE_EXERCISES:
				{
					lptcsType = _T("习题");
					lptcsBkImage	= _T("RightBar\\Item\\item_bg_course.png");
				}
				break;
		}

		pItem->SetGroup(_T("upload"));
		pItem->SetTypeTitle(lptcsType);
		pItem->SetType(nLocalItemType);

		_tcsncpy_s(szFileName, szFileName, 100);
		pItem->SetTitle(szFileName);

		pItem->SetImage(lptcsBkImage);
		pItem->SetResource(lptcsPath);

		m_layList->Add(pItem);
		
	}


	//add for append item
	CForAppendItemUI* pAppendItem = NULL;
	if (!m_AppendItemBuilder.GetMarkup()->IsValid())
	{
		pAppendItem = dynamic_cast<CForAppendItemUI*>(m_AppendItemBuilder.Create(_T("Upload\\ForAppendItem.xml"), NULL, this));
	}
	else
	{
		pAppendItem = dynamic_cast<CForAppendItemUI*>(m_AppendItemBuilder.Create(this));
	}
	pAppendItem->SetClickDelegate(MakeDelegate(this, &CDBankUploadUI::OnAppendItem));

	m_layList->Add(pAppendItem);

	//show detail
	ShowListDetail();


	this->GetManager()->KillTimer(m_layList, TimerId_ScrollToBottom);
	this->GetManager()->SetTimer(m_layList, TimerId_ScrollToBottom, 100);


}

void CDBankUploadUI::ShowListDetail()
{
	DWORD dwTotalSize = 0;
	int nCount = m_layList->GetCount();

	//except append item
	if (nCount > 1)
	{
		nCount = nCount - 1;
	}
	else
	{
		nCount = 0;
	}


	for (int i = 0; i < nCount; ++i)
	{
		CUploadItemUI* pItem = (CUploadItemUI*)m_layList->GetItemAt(i);

		FILE* fp = NULL;
		_tfopen_s(&fp, pItem->GetResource(), _T("r"));
		if (!fp)
		{
			continue;
		}

		fseek(fp, 0, SEEK_END);
		dwTotalSize += ftell(fp);
		fseek(fp, 0, SEEK_SET);

		fclose(fp);

	}

	TCHAR szFileSize[32]	= {0};
	TCHAR szFileFormat[]	= _T("%.1f %s");
	if (dwTotalSize < 1000 )
	{
		_stprintf_s(szFileSize, szFileFormat, dwTotalSize * 1.0f, _T("B"));
	}
	else if (dwTotalSize < 1000 * 1000 )
	{
		_stprintf_s(szFileSize, szFileFormat, dwTotalSize / 1024.0f, _T("KB"));
	}
	else if (dwTotalSize < 1000 * 1000 * 1000 )
	{
		_stprintf_s(szFileSize, szFileFormat, dwTotalSize / (1024.0f * 1024), _T("MB"));
	}
	else if (dwTotalSize < 1000 * 1000 * 1000 * 1000.0f )
	{
		_stprintf_s(szFileSize, szFileFormat, dwTotalSize / (1024.0f * 1024 * 1024), _T("MB"));
	}

	TCHAR szDetail[MAX_PATH] = {0};
	TCHAR szFormat[] = _T("{f 14}共{c #FF6600} %d {/c}个文件，%s{/f}");

	_stprintf_s(szDetail, szFormat, nCount, szFileSize);

	m_lbFileDetail->SetText(szDetail);

}

void CDBankUploadUI::OnItemRemove( TNotifyUI& msg )
{
	m_layList->Remove((CControlUI*)msg.wParam);
	ShowListDetail();
}

bool CDBankUploadUI::OnAppendItem( void*pObj )
{
	TNotifyUI msg = *(TNotifyUI*)pObj;
	OnBtnAppend(msg);

	return true;
}

bool CDBankUploadUI::OnListEvent( void*pObj )
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == TimerId_ScrollToBottom)
		{
			SIZE size = m_layList->GetScrollRange();
			if (size.cy > 0)
			{
				m_layList->SetScrollPos(size);
				this->GetManager()->KillTimer(m_layList, TimerId_ScrollToBottom);
			}
		}
		else if (pEvent->wParam == TimerId_InvalidateProgress)
		{
			
		}
	}
	return true;
}

void CDBankUploadUI::OnDragFiles( WPARAM wParam, LPARAM lParam )
{
	HDROP hDropInfo = (HDROP)wParam;
	UINT nCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	TCHAR szPath[MAX_PATH * 2];
	vector<tstring> vecFiles;
	if (nCount)
	{
		for (UINT nIndex = 0; nIndex < nCount; ++nIndex)
		{
			DragQueryFile(hDropInfo, nIndex, szPath, _countof(szPath));
			vecFiles.push_back(szPath);
		}
	}

	CreateList(vecFiles, true);
	if (m_layList->GetCount() > 1)
	{
		ShowListUI(true);
	}
	DragFinish(hDropInfo);

	
}
