#include "StdAfx.h"
#include "LocalItem.h"
#include "ItemExplorer.h"
#include "GroupExplorer.h"
#include "NDCloud/NDCloudUser.h"
#include "Http/HttpDownloadManager.h"
#include "NDCloud/CategoryTree.h"
#include "NDCloud/NDCloudResModel.h"
#include "Http/HttpUploadManager.h"
#include "NDCloud/NDCloudAPI.h"

DuiLib::CDialogBuilder CLocalItemUI::m_ChapterStyleBuilder;
CLocalItemUI::CLocalItemUI()
{
	m_bEnableChapter = true;
	CResourceItemUI::m_bNeedTypeIcon = true;
	m_dwUploadServerPathTaskId = 0;
	m_dwUploadNetdiscTaskId = 0;
	m_dwUplpadCommitTaskId = 0;
}


CLocalItemUI::~CLocalItemUI()
{
	
}

void CLocalItemUI::OnItemDragFinish()
{
	TNotifyUI msg;
	OnButtonClick(0, msg);
}

void CLocalItemUI::Init()
{
	__super::Init();

	if (m_bEnableChapter)
	{
		CHorizontalLayoutUI* pChapterLayout = NULL;
		if (!m_ChapterStyleBuilder.GetMarkup()->IsValid())
		{
			pChapterLayout = dynamic_cast<CHorizontalLayoutUI*>(m_ChapterStyleBuilder.Create(_T("RightBar\\Item\\LocalChapterStyle.xml"), (UINT)0, NULL, this->GetManager()));

		}
		else
		{
			pChapterLayout = dynamic_cast<CHorizontalLayoutUI*>(m_ChapterStyleBuilder.Create(NULL, this->GetManager()));
		}
		this->Add(pChapterLayout);

// 		 this->SetFixedHeight(160);
// 
// 		 this->SetAttribute(_T("childpadding"), _T("5"));

		SetChapter(m_strChapter.c_str());
	}

}

bool CLocalItemUI::OnEmptyControlEvent( void* pEvent )
{
	__super::OnEmptyControlEvent(pEvent);
	TEventUI event = *(TEventUI*)pEvent;
	if (event.Type == UIEVENT_RBUTTONDOWN)
	{
		this->GetManager()->SendNotify(this->GetOption(), DUI_MSGTYPE_CLICK, (WPARAM)TRUE);
		this->GetOption()->Selected(true);
	}

	return true;
}

void CLocalItemUI::SetChapter( LPCTSTR lptcsChapter )
{
	if (!lptcsChapter
		|| _tcsicmp(lptcsChapter, _T("")) == 0)
	{
		return;
	}

	m_strChapter = lptcsChapter;

	CControlUI* pCtrl = FindSubControl(_T("chapter"));
	if (pCtrl)
	{
		pCtrl->SetText(lptcsChapter);
		pCtrl->SetToolTip(lptcsChapter);
	}
}

void CLocalItemUI::SetContentHeight( int nHeight )
{
	CControlUI* pCtrl = GetContent();
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(nHeight);
		if (m_bEnableChapter)
		{
			this->SetFixedHeight(nHeight + 20 + 15);
		}
		else
		{
			this->SetFixedHeight(nHeight + 20);
		}
		
	}
}

void CLocalItemUI::EnableChapter( bool bEnable )
{
	m_bEnableChapter = bEnable;
}

void CLocalItemUI::UploadNetdisc(int currentModeType)
{
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if (dwUserID != 0)
	{
		string strResource  = GetResource();
		DWORD dwAttri = ::GetFileAttributes(strResource.c_str());
		if ((dwAttri != -1) && !(dwAttri&FILE_ATTRIBUTE_DIRECTORY))
		{
			string strTitle = GetTitle();
			string strToast = "“"+ strTitle +"”" + "开始上传...";
			CToast::Toast(strToast);
			CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
			if( pHttpManager != NULL )
			{
				CStream* pStream=new CStream(1024);
				pStream->WriteString(strTitle);
				pStream->WriteString(strResource);
				pStream->WriteInt(currentModeType);
				TCHAR szPost[1024];
				_stprintf_s(szPost, "/v0.6/coursewares/none/uploadurl?uid=%u&renew=false", dwUserID);
				m_dwUploadServerPathTaskId = pHttpManager->AddTask(_T("esp-lifecycle.web.sdp.101.com"), szPost, _T(""), _T("GET"), _T(""), 80, 
					MakeHttpDelegate(this, &CLocalItemUI::OnGetServerPath),
					MakeHttpDelegate(NULL),
					MakeHttpDelegate(NULL), 
					TRUE,FALSE,0,pStream);
				if(m_dwUploadServerPathTaskId==0)
				{
					strToast = "“"+ strTitle +"”" + "上传失败";
					CToast::Toast(strToast);
				}
			}
		}
		else
		{
			CToast::Toast("文件不存在");
		}
	}
}

bool CLocalItemUI::OnGetServerPath( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);
	string strTitle = "";
	string strResource = "";
	int currentModeType = 0;
	if(pNotify->pUserData)
	{
		CStream* pStream = (CStream*)pNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			strTitle = pStream->ReadString();
			strResource = pStream->ReadString();
			currentModeType = pStream->ReadInt();
			delete pStream;
		}
	}
	bool isSuccess=false;
	if(pNotify->dwErrorCode == 0)
	{
		if(pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			tstring str = pNotify->pData;
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(str, root);
			if( res )
			{
				tstring strUrl="";
				tstring strSessionId="";
				tstring strServerPath = "";
				tstring strUploadGuid = "";
				if( !root["access_url"].isNull() )
				{
					strUrl = root["access_url"].asCString();
				}
				if( !root["uuid"].isNull() )
				{
					strUploadGuid = root["uuid"].asCString();
				}
				if( !root["session_id"].isNull() )
				{
					strSessionId = root["session_id"].asCString();
				}
				if( !root["dist_path"].isNull() )
				{
					strServerPath = root["dist_path"].asCString();
				}
				if(!strUrl.empty()&&!strUploadGuid.empty()&&!strSessionId.empty()&&!strServerPath.empty())
				{
					isSuccess=true;
					UploadFile(strUrl.c_str(), strSessionId.c_str(), strUploadGuid.c_str(), strServerPath.c_str(),strTitle.c_str(),strResource.c_str(),currentModeType);
				}
			}
		}
	}
	if(!isSuccess)
	{
		string strToast = "“"+ strTitle +"”" + "上传失败";
		CToast::Toast(strToast);
	}
	return true;
}

void CLocalItemUI::UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath,LPCTSTR strTitle,LPCTSTR strResource,int currentModeType)
{
	TCHAR szHost[128];
	TCHAR szUrlPath[1024];
	if(sscanf_s(lptcsUrl, _T("http://%[^/]%s"), szHost, sizeof(szHost) - 1, szUrlPath , sizeof(szUrlPath) - 1) == 2)
	{
		tstring strHost = szHost;
		tstring strUrlPath = szUrlPath;
		strUrlPath += _T("?session=");
		strUrlPath += lptcsSessionId;

		CStream* pStream = new CStream(1024);
		pStream->WriteString(lptcsUploadGuid);
		pStream->WriteString(lptcsServerFilePath);
		pStream->WriteString(strTitle);
		pStream->WriteString(strResource);
		pStream->WriteInt(currentModeType);
		m_dwUploadNetdiscTaskId = HttpUploadManager::GetInstance()->AddUploadTask(strHost.c_str(), 
			strUrlPath.c_str(),
			strResource,
			lptcsServerFilePath,
			_T("POST"), 
			80,
			MakeHttpDelegate(this, &CLocalItemUI::OnUploadCompleted),
			MakeHttpDelegate(NULL),FALSE,FALSE,
			pStream);
		if(m_dwUploadNetdiscTaskId==0)
		{
			string strToast = strTitle;
			strToast = "“"+strToast+"”" + "上传失败";
			CToast::Toast(strToast);
		}
	}
}

bool CLocalItemUI::OnUploadCompleted( void * pParam )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pParam;
	string strServerPath = "";
	string strUploadGuid = "";
	string strTitle = "";
	string strResource = "";
	int currentModeType=0;
	CStream* pStream=NULL;
	tstring strFullFileServerPath;
	if(pHttpNotify->pUserData)
	{
		pStream = (CStream*)pHttpNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			strUploadGuid = pStream->ReadString();
			strServerPath = pStream->ReadString();
			strTitle = pStream->ReadString();
			strResource = pStream->ReadString();
			currentModeType = pStream->ReadInt();
		}
	}
	if (pHttpNotify->dwErrorCode==0)
	{
		if(pHttpNotify->nDataSize>0)
		{
			pHttpNotify->pData[pHttpNotify->nDataSize] = '\0';
			tstring str = pHttpNotify->pData;
			Json::Reader reader;
			Json::Value root;

			bool res = reader.parse(str, root);
			if(!res||root.get("dentry_id", Json::Value()).isNull())
			{
				pHttpNotify->dwErrorCode=5;	
			}

			if(res && !root.get("path", Json::Value()).isNull())
			{
				strFullFileServerPath= Ansi2Str(root["path"].asCString());
			}
		}
	}
	if (pHttpNotify->dwErrorCode==0)
	{
		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		if(dwUserId!=0)
		{
			tstring strChapterGuid = NDCloudGetChapterGUID();
			TCHAR szUserId[MAX_PATH];
			_stprintf_s(szUserId, _T("%d"), dwUserId);

			CNDCloudResourceModel ResModel;

			ResModel.SetGuid(strUploadGuid.c_str());
			string strTmp = strTitle;
			int length = strTmp.length();
			if(length>200)
			{
				strTmp = strTmp.substr(0,200);
			}
			ResModel.SetBasicInfo(strTmp);
			ResModel.SetPreview();
			ResModel.SetTechInfo(strResource, strServerPath.c_str(), strFullFileServerPath);
			ResModel.SetLifeCycleInfo();
			ResModel.AddCoverage(_T("User"), szUserId, _T(""), _T("OWNER") );
			ResModel.AddRelation(strChapterGuid);

			CCategoryTree* pCategory;
			NDCloudGetCategoryTree(pCategory);
			ResModel.SetCategoryInfo(pCategory);
			m_dwUplpadCommitTaskId = ResModel.CommitResourceModel(MakeHttpDelegate(this, &CLocalItemUI::OnUploadCommitCompleted),pStream);
			if(m_dwUplpadCommitTaskId==0)
			{
				string strToast = "“"+ strTitle +"”" + "上传失败";
				CToast::Toast(strToast);
			}
		}
	}
	else
	{
		string strToast = "“"+ strTitle +"”" + "上传失败";
		CToast::Toast(strToast);
	}
	return true;
}

bool CLocalItemUI::OnUploadCommitCompleted( void * pParam )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pParam;
	string strServerPath = "";
	string strUploadGuid = "";
	string strTitle = "";
	string strResource = "";
	int currentModeType = 0;
	if(pHttpNotify->pUserData)
	{
		CStream* pStream = (CStream*)pHttpNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			strUploadGuid = pStream->ReadString();
			strServerPath = pStream->ReadString();
			strTitle = pStream->ReadString();
			strResource = pStream->ReadString();
			currentModeType = pStream->ReadInt();
			delete pStream;
		}
	}
	bool success = false;
	if (pHttpNotify->dwErrorCode==0)
	{
		if(pHttpNotify->nDataSize>0)
		{
			pHttpNotify->pData[pHttpNotify->nDataSize] = '\0';
			string strContent = Utf8ToAnsi(pHttpNotify->pData);
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strContent, root);
			if(res)
			{
				if( root["code"].isNull() )
				{
					success = true;
					string strToast = "“"+ strTitle +"”" + "上传成功";
					CToast::Toast(strToast);
				}
			}
		}
	}
	if(!success)
	{
		string strToast = "“"+ strTitle +"”" + "上传失败";
		CToast::Toast(strToast);
	}
	BroadcastEvent(EVT_MENU_UPLOADNETDISC_COMPELETE,(WPARAM)pHttpNotify->dwErrorCode,(LPARAM)currentModeType);
	return true;
}