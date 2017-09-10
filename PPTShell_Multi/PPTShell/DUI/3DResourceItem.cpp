#include "StdAfx.h"
#include "3DResourceItem.h"
#include "ItemExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "ThirdParty/zip/ZipWrapper.h"
#include "Util/Util.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"

C3DResourceItemUI::C3DResourceItemUI()
{
	m_dwDownloadId	= 0;
	m_eContentType			= CONTENT_ASSETS;
}

C3DResourceItemUI::~C3DResourceItemUI()
{
	CStream* pStream = (CStream*)this->GetTag();
	if (pStream)
	{
		delete pStream;
		this->SetTag(NULL);
	}
	NDCloudDownloadCancel(m_dwDownloadId);
}

void C3DResourceItemUI::Init()
{
	__super::Init();

	CControlUI* pCtrl = FindSubControl(_T("toolbar"));
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(30);
	}

	//SetImage(_T("RightBar\\Item\\item_bg_course.png"));

	if( !m_builder.GetMarkup()->IsValid() ) {
		m_layMaskStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(_T("RightBar\\Item\\ResourceMaskStyle.xml"), (UINT)0, this, this->GetManager()));
	}
	else {
		m_layMaskStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(this, this->GetManager()));
	}
	GetContent()->Add(m_layMaskStyle);


	m_pAnimation= dynamic_cast<CGifAnimUI*>( FindSubControl(_T("loading")));


	//SetIcon(_T("RightBar\\Item\\bg_tit_class.png"));
	SetContentHeight(110);

	DownloadThumbnail();
}


void C3DResourceItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	DownloadResource(nButtonIndex, CloudFile3DResource, 0);
}

void C3DResourceItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void C3DResourceItemUI::ReadStream( CStream* pStream )
{
	
	m_strGuid		= pStream->ReadString().c_str();

	m_strTitle		= pStream->ReadString();

	SetTitle(m_strTitle.c_str());
	SetToolTip(m_strTitle.c_str());
	SetResource(pStream->ReadString().c_str());

	SetGroup(_T("123"));

	// json information which used to modify title

	m_strPreviewUrl = pStream->ReadString();

	m_strJsonInfo = pStream->ReadString();

	SetGroup(_T("CourseItem"));
}

void C3DResourceItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	CCloudItemUI::OnDownloadResourceBefore();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);

	if(_tcsicmp(GetResource(), _T("")) == 0)
	{
		CToast::Toast(_STR_PREVIEW_DIALOG_NOSUPPORT);
		return;
	}

	m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), _T(""), CloudFile3DResource, 0, MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceCompleted), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));	

	if(m_dwResourceDownloadId==0)
	{
		this->ShowProgress(false);
		string strToast="¡°";
		strToast+=GetTitle();
		strToast+="¡±";
		strToast+="ÏÂÔØÊ§°Ü";
		CToast::Toast(strToast);
	}
}

void C3DResourceItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	
	// folder
	tstring strFolder = lptcsPath;
	int pos = strFolder.rfind('.');
	if( pos != -1 )
		strFolder = strFolder.substr(0, pos);

	tstring strAppPath		= strFolder;
	tstring strSearchPath	= strFolder;
	strSearchPath += _T("\\*.exe");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		CUnZipper UnZipper;
		UnZipper.UnZip(Str2Ansi(lptcsPath).c_str(), Str2Ansi(strFolder).c_str());

		hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
	}

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		strAppPath += _T("\\");
		strAppPath += FindFileData.cFileName;

		if (nButtonIndex == 0)
		{
			Insert3DByThread(strAppPath.c_str(), m_strThumbnailPath.c_str(), 0, 0);
			Statistics::GetInstance()->Report(STAT_INSERT_3D);
		}
		else if (nButtonIndex == 1)
		{
			ShellExecute(NULL, "open", strAppPath.c_str(), _T(""), strFolder.c_str(), SW_SHOWNORMAL);
		}
	}
}

//bool C3DResourceItemUI::On3DResourceDownloaded( void * pParam )
//{
//	THttpNotify* pNotify = (THttpNotify*)pParam;
//
//	// unzip
//	if( pNotify->strFilePath != _T("") )
//	{
//		// folder
//		tstring strFolder = pNotify->strFilePath;
//		int pos = strFolder.rfind('.');
//		if( pos != -1 )
//			strFolder = strFolder.substr(0, pos);
//
//		CUnZipper UnZipper;
//		UnZipper.UnZip(Str2Ansi(pNotify->strFilePath).c_str(), Str2Ansi(strFolder).c_str());
//
//		tstring strAppPath = strFolder;
//		tstring strSearchPath = strFolder;
//		strSearchPath += _T("\\*.exe");
//		WIN32_FIND_DATA FindFileData;
//		HANDLE hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
//		if (hFind != INVALID_HANDLE_VALUE)
//		{
//			strAppPath += _T("\\");
//			strAppPath += FindFileData.cFileName;
//
//			ShellExecute(NULL, "open", strAppPath.c_str(), _T(""), strFolder.c_str(), SW_SHOWNORMAL);
//		}
//	}
//
//	__super::OnDownloadResourceCompleted(pNotify);
//
//	return true;
//}

void C3DResourceItemUI::DownloadThumbnail()
{
	this->StartMask();
	m_dwDownloadId = NDCloudDownloadFile(m_strPreviewUrl.c_str(), m_strGuid, GetTitle(), CloudFileImage, 240, MakeHttpDelegate(this, &C3DResourceItemUI::OnDownloadThumbnailCompleted));
	if (m_dwDownloadId == 0)
	{
		this->StopMask();
		return;
	}
}

void C3DResourceItemUI::StartMask()
{
	m_layMaskStyle->SetVisible(true);
	m_pAnimation->PlayGif();
}

void C3DResourceItemUI::StopMask()
{
	m_layMaskStyle->SetVisible(false);
	m_pAnimation->StopGif();
}

bool C3DResourceItemUI::OnDownloadThumbnailCompleted( void* pNotify )
{
	this->StopMask();

	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	if (pHttpNotify->strFilePath == _T(""))
	{
		return true;
	}

	m_strThumbnailPath = pHttpNotify->strFilePath;
	SetImage(pHttpNotify->strFilePath.c_str());

	return true;
}

CControlUI* C3DResourceItemUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return NULL;
}