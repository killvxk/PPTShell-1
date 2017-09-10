#include "StdAfx.h"
#include "QuestionPreviewDialogUI.h"
#include "cef\cefobj.h"
#include "Util\Util.h"

#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ThumbnailItem.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/QuestionStyleable.h"

#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/CloudPhotoHandler.h"
#include "DUI/CloudThumbnailHandler.h"
#include "DUI/CloudQuestionHandler.h"
#include "DUI/ITransfer.h"
#include "DUI/ResourceDownloader.h"
#include "BaseParamer.h"
#include "QuestionDownloader.h"
#include "NDCloud/NDCloudQuestion.h"
#include "PPTControl/PPTControllerManager.h"
#include "CloudInteractQuestionHandler.h"
#include "LocalInteractiveQuestionItem.h"
#include "InteractQuestionDownloader.h"
#include "QuestionPreviewFullScreenUI.h"
#include "GUI/MainFrm.h"
#include "Tinyxml/tinyxml.h"
#include "InsertParamer.h"

#define MSG_URLCHANGE  30001	
#define MSG_URLFAILED  30002
#define MSG_PLAYERLOADED 30004

CQuestionPreviewDialogUI::CQuestionPreviewDialogUI(void):m_hWebWindow(NULL),m_pLblTitle(NULL),m_pLblFileSizeDesc(NULL),m_pLblFileSize(NULL),m_pGifLoading(NULL),m_pLayoutQuestion(NULL),m_pLayoutThumbnailList(NULL)
{
	m_sCurrentUrl = _T("");
	m_nCurrentIndex = 0;
	m_phSetPosThread = NULL;
	m_nScrollWidth = 0;
	m_eType = QUESTION_CLOUD;
	m_nInsertCount = 0;
	m_hThread = NULL;
	m_bIsWebShown = false;
}

CQuestionPreviewDialogUI::~CQuestionPreviewDialogUI(void)
{
	if( m_phSetPosThread  != NULL )
	{
		TerminateThread(m_phSetPosThread, 0);
		CloseHandle(m_phSetPosThread);
		m_phSetPosThread = NULL;
	}
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
}

LPCTSTR CQuestionPreviewDialogUI::GetWindowClassName() const
{
	return _T("QuestionPreviewDialog");
}

DuiLib::CDuiString CQuestionPreviewDialogUI::GetSkinFile()
{
	return _T("Preview\\QuestionPreview.xml");
}

DuiLib::CDuiString CQuestionPreviewDialogUI::GetSkinFolder()
{
	return _T("skins");
}

CControlUI* CQuestionPreviewDialogUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return __super::CreateControl(pstrClass);
}

void CQuestionPreviewDialogUI::InitWindow()
{
	m_pLblTitle = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LblTitle")));
	m_pLblFileSizeDesc = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblFileSizeDesc")));
	m_pLblFileSize = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblFileSize")));
	m_pGifLoading = dynamic_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("gifLoading")));
	if(m_pGifLoading)
	{
		m_pGifLoading->PlayGif();
	}
	m_pLayoutQuestion = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutQuestion")));
	m_pLayoutThumbnailList = dynamic_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("layoutThumbnailList")));
	m_pBtnFullScreen = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnFullScreen")));
	m_pBtnInsert = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnInsert")));
	m_pBtnCancelInsert = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnCancelInsert")));
	m_hWebWindow = CreateWindow( _T("CoursePlayWebUIEx"), _T(""), WS_POPUP | WS_VISIBLE | WS_CHILD,  0,0,0,0 , GetHWND() , NULL, AfxGetApp()->m_hInstance, NULL);
}

void CQuestionPreviewDialogUI::InitPosition()
{
	CRect rect;
	HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
	::GetWindowRect(hwnd, &rect);
	if(!GetHWND())
	{
		Create(hwnd, _T("PreviewWindow"), WS_POPUP|WS_VISIBLE , NULL, 0, 0, rect.Width(), rect.Height());
	}
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
	if(m_pLayoutQuestion)
	{
		RECT rectWeb = {0};
		CRect rect;
		HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
		::GetWindowRect(hwnd, &rect);
		rectWeb = m_pLayoutQuestion->GetPos();
		int x = rect.left + (rect.right-rect.left-984)/2+140;
		int y = rect.top + (rect.bottom-rect.top-596)/2+50;
		int width = rectWeb.right - rectWeb.left;
		int height = rectWeb.bottom - rectWeb.top;
		MoveWindow(m_hWebWindow, x, y, width, height, TRUE);
	}
}

QUESTION_TYPE CQuestionPreviewDialogUI::GetQuestionType()
{
	return m_eType;
}

void CQuestionPreviewDialogUI::SetQuestionType( QUESTION_TYPE type )
{
	m_eType = type;
}

void CQuestionPreviewDialogUI::OnShown()
{
	if(m_pLayoutQuestion)
	{
		RECT rectWeb = {0};
		CRect rect;
		HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
		::GetWindowRect(hwnd, &rect);
		rectWeb = m_pLayoutQuestion->GetPos();
		int x = rect.left + (rect.right-rect.left-984)/2+140;
		int y = rect.top + (rect.bottom-rect.top-596)/2+50;
		int width = rectWeb.right - rectWeb.left;
		int height = rectWeb.bottom - rectWeb.top;
		MoveWindow(m_hWebWindow, x, y, width, height, TRUE);
	}
	ShowQuestion(m_sCurrentUrl);
}

LRESULT CQuestionPreviewDialogUI::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( uMsg == WM_KEYDOWN )
	{
		if(wParam == VK_RETURN || wParam == VK_ESCAPE)
		{
			HideWindow();
			return TRUE;
		}
	}
	return __super::OnKeyDown(uMsg, wParam, lParam, bHandled);
}

LRESULT CQuestionPreviewDialogUI::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case SW_HIDE:
		int a = 0;
		break;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CQuestionPreviewDialogUI::InitData( CStream* pStream)
{
	if(pStream)
	{
		m_nInsertCount = 0;
		m_pBtnCancelInsert->SetEnabled(false);
		for (map<CCloudResourceHandler*,CQuestionDownloader*>::iterator itor = m_mDownloaders.begin(); itor != m_mDownloaders.end(); ++itor)
		{
			itor->second->CancelTransfer();
			delete itor->second;
		}
		m_mDownloaders.clear();
		m_pLayoutThumbnailList->RemoveAll();
		pStream->ResetCursor();
		tstring strUrl = pStream->ReadString();
		m_sCurrentUrl = strUrl;
		m_nCurrentIndex = pStream->ReadDWORD();
		int count = pStream->ReadDWORD();
		m_nScrollWidth = 180;
		for (int i = 0; i < count; i++)
		{
			IComponent* pComponent = new CMaskComponent(&CQuestionStyleable::Styleable);
			pComponent = new CItemComponent(pComponent);
			CThumbnailItemUI* pNewItem = new CThumbnailItemUI(pComponent);
			CCloudResourceHandler* pHandler = NULL;
			if(m_eType==QUESTION_CLOUD)
			{
				pHandler= new CCloudQuestionHandler;
			}
			else if(m_eType==QUESTION_INTERACTIVE_CLOUD)
			{
				pHandler= new CCloudInteractQuestionHandler;
			}
			pHandler->OnClick += MakeDelegate(this, &CQuestionPreviewDialogUI::OnThumbnailClick);
			pNewItem->SetHandler(pHandler);
			if (pNewItem)
			{
				pNewItem->SetPadding(CDuiRect(8, 0, 0, 0));
				if (i == 0)
				{
					pNewItem->SetPadding(CDuiRect(0, 0, 0, 0));
				}
				pNewItem->ReadStream(pStream);
				tstring strName = pHandler->GetHandlerId();
				strName += _T("Preview");
				pHandler->SetHandlerId(strName.c_str());
				pNewItem->SetItemHeight(110);
				pNewItem->SetItemWidth(172);
				m_pLayoutThumbnailList->Add(pNewItem);
			}
		}
		if (count > 0)
		{
			CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pLayoutThumbnailList->GetItemAt(m_nCurrentIndex));
			if( pItem)
			{
				pItem->GetOption()->Selected(true);
				m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
			}
		}
		m_pLayoutThumbnailList->GetHorizontalScrollBar()->SetFixedHeight(1);
		m_pLayoutThumbnailList->GetHorizontalScrollBar()->SetScrollPos(0);
		if(count>5)
		{
			int range = m_nScrollWidth * (count-5);
			m_pLayoutThumbnailList->GetHorizontalScrollBar()->SetScrollRange(range);
			m_pLayoutThumbnailList->GetHorizontalScrollBar()->SetVisible(true);
		}
		else
		{
			m_pLayoutThumbnailList->GetHorizontalScrollBar()->SetScrollRange(0);
			m_pLayoutThumbnailList->GetHorizontalScrollBar()->SetVisible(false);
		}
		m_pLayoutThumbnailList->SetVisible(true);
		m_pLayoutThumbnailList->Invalidate();
		m_phSetPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetPosThread, this, 0, NULL);
	}
}

DWORD WINAPI CQuestionPreviewDialogUI::SetPosThread( LPARAM lpParam )
{
	CQuestionPreviewDialogUI* pThis  = (CQuestionPreviewDialogUI*)lpParam;
	while(true)
	{
		Sleep(50);
		RECT rt = pThis->m_pLayoutThumbnailList->GetPos();
		if (!(rt.bottom == 0 
			&& rt.top == 0
			&& rt.left == 0
			&& rt.right == 0))
		{
			SIZE sz = pThis->m_pLayoutThumbnailList->GetScrollPos();
			sz.cx = pThis->m_nScrollWidth * pThis->m_nCurrentIndex;
			pThis->m_pLayoutThumbnailList->SetScrollPos(sz);
			break;
		}
	}
	CloseHandle(pThis->m_phSetPosThread);
	pThis->m_phSetPosThread = NULL;
	return 1;
}

void CQuestionPreviewDialogUI::OnBtnCloseClick( TNotifyUI& msg )
{
	HideWindow();
}

void CQuestionPreviewDialogUI::OnBtnThumbnailPreviewClick( TNotifyUI& msg )
{
	if(m_pLayoutThumbnailList)
	{
		SIZE sz = m_pLayoutThumbnailList->GetScrollPos();
		sz.cx -= m_nScrollWidth;
		m_pLayoutThumbnailList->SetScrollPos(sz);
	}
}

void CQuestionPreviewDialogUI::OnBtnThumbnailNextClick( TNotifyUI& msg )
{
	if(m_pLayoutThumbnailList)
	{
		SIZE sz = m_pLayoutThumbnailList->GetScrollPos();
		sz.cx += m_nScrollWidth;
		SIZE szRange =  m_pLayoutThumbnailList->GetScrollRange();
		m_pLayoutThumbnailList->SetScrollPos(sz);
	}
}

void CQuestionPreviewDialogUI::OnBtnQuestionPreviewClick( TNotifyUI& msg )
{
	if(m_pLayoutThumbnailList)
	{
		if( m_nCurrentIndex > 0 )
		{
			m_nCurrentIndex--;
			if(m_nCurrentIndex>=0)
			{
				HideQuestion();
				//滚动缩略图
				SIZE sz = m_pLayoutThumbnailList->GetScrollPos();
				sz.cx -= m_nScrollWidth;
				m_pLayoutThumbnailList->SetScrollPos(sz);
				//显示上一个内容
				CThumbnailItemUI* pItem = dynamic_cast<CThumbnailItemUI*>(m_pLayoutThumbnailList->GetItemAt(m_nCurrentIndex));
				if( pItem&&pItem->GetOption())
				{
					pItem->GetOption()->Selected(true);
					m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
				}
			}
			else
			{
				m_nCurrentIndex=0;
			}
		}
		else
		{
			m_nCurrentIndex = 0;
			if(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()&&IsWindowVisible(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()))
			{
				CQuestionPreviewFullScreenUI::GetInstance()->ShowQuestion(m_sCurrentUrl);
			}
		}
	}
}

void CQuestionPreviewDialogUI::OnBtnQuestionNextClick( TNotifyUI& msg )
{
	if(m_pLayoutThumbnailList)
	{
		int count = m_pLayoutThumbnailList->GetCount();
		if( m_nCurrentIndex < count )
		{
			m_nCurrentIndex ++;
			if(m_nCurrentIndex!=count)
			{
				HideQuestion();
				//滚动缩略图
				SIZE sz = m_pLayoutThumbnailList->GetScrollPos();
				sz.cx += m_nScrollWidth;
				SIZE szRange =  m_pLayoutThumbnailList->GetScrollRange();
				m_pLayoutThumbnailList->SetScrollPos(sz);
				//显示下一个内容
				CThumbnailItemUI* pItem = dynamic_cast<CThumbnailItemUI*>(m_pLayoutThumbnailList->GetItemAt(m_nCurrentIndex));
				if( pItem&&pItem->GetOption())
				{
					pItem->GetOption()->Selected(true);
					m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
				}
			}
			else
			{
				m_nCurrentIndex --;
			}
		}
		else
		{
			m_nCurrentIndex = count-1;
			if(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()&&IsWindowVisible(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()))
			{
				CQuestionPreviewFullScreenUI::GetInstance()->ShowQuestion(m_sCurrentUrl);
			}
		}
	}
}

void CQuestionPreviewDialogUI::OnBtnFullScreenClick( TNotifyUI& msg )
{
	if(m_pLayoutThumbnailList)
	{
		if(m_nCurrentIndex>=0&&m_nCurrentIndex<m_pLayoutThumbnailList->GetCount())
		{
			CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pLayoutThumbnailList->GetItemAt(m_nCurrentIndex));
			if(pItem)
			{
				CStream stream(1024);
				stream.WriteString(m_sCurrentUrl);
				CQuestionPreviewFullScreenUI* pPreviewUI = CQuestionPreviewFullScreenUI::GetInstance();
				pPreviewUI->SetPreviewDlg(this);
				pPreviewUI->InitData(&stream);
				pPreviewUI->ShowWindow();
			}
		}
	}
}

void CQuestionPreviewDialogUI::OnBtnInsertClick( TNotifyUI& msg )
{
	if(m_pLayoutThumbnailList)
	{
		if(m_nCurrentIndex<m_pLayoutThumbnailList->GetCount())
		{
			CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pLayoutThumbnailList->GetItemAt(m_nCurrentIndex));
			if(pItem)
			{
				CControlUI* pControl = pItem->GetImageCtrl();
				if(pControl)
				{
					tstring imagePath = _T("");
					if (GetFileAttributes(pControl->GetBkImage()) !=  INVALID_FILE_ATTRIBUTES)
					{
						imagePath = pControl->GetBkImage();
					}
					else
					{
						imagePath = CPaintManagerUI::GetResourcePath();
						imagePath += pControl->GetBkImage();
					}
					CCloudResourceHandler* pHandler = NULL;
					tstring strQuestionName = _T("");
					if(m_eType == QUESTION_CLOUD)
					{
						pHandler = dynamic_cast<CCloudQuestionHandler*>(pItem->GetHandler());
						CCloudQuestionHandler* pTmp = dynamic_cast<CCloudQuestionHandler*>(pItem->GetHandler());
						if(pTmp)
						{
							strQuestionName = pTmp->GetQuestionName();
						}
					}
					else if(m_eType == QUESTION_INTERACTIVE_CLOUD)
					{
						pHandler = dynamic_cast<CCloudInteractQuestionHandler*>(pItem->GetHandler());
						CCloudInteractQuestionHandler* pTmp = dynamic_cast<CCloudInteractQuestionHandler*>(pItem->GetHandler());
						if(pTmp)
						{
							strQuestionName = pTmp->GetQuestionName();
						}
					}
					if(pHandler)
					{
						/*m_nInsertCount++;
						m_pBtnCancelInsert->SetEnabled(true);
						pHandler->GetPlaceHolderId(imagePath.c_str(),-1,-1,pItem->GetImageCtrl()->GetImageSize().cx,pItem->GetImageCtrl()->GetImageSize().cy,0);*/

						m_pBtnInsert->SetEnabled(false);
						tstring strTitle = pHandler->GetTitle();
						tstring strGuid = pHandler->GetGuid();
						QUESTION_TYPE type = m_eType;

						CStream* pStream = new CStream(2048);
						pStream->WriteString(strTitle);
						pStream->WriteString(strGuid);
						pStream->WriteString(strQuestionName);
						pStream->WriteDWORD((DWORD)type);
						
						CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
						CInsertParamer* pParamer	= new CInsertParamer();
						pParamer->SetOperationerId(pMainFrame->GetOperationerId());
						pParamer->SetInsertPos(-1, -1);
						pParamer->SetUserData(pStream);
						pParamer->SetCompletedDelegate(MakeDelegate(this, &CQuestionPreviewDialogUI::OnHandleInsert));
						CQuestionDownloader* pQuestionDownloader =NULL;
						if(m_eType==QUESTION_CLOUD)
						{
							pQuestionDownloader = new CQuestionDownloader();
						}
						else if(m_eType==QUESTION_INTERACTIVE_CLOUD)
						{
							pQuestionDownloader = new CInteractQuestionDownloader();
						}
						pQuestionDownloader->SetIsSelfFree(false);
						pQuestionDownloader->SetQuestionGuid(pHandler->GetGuid());
						pQuestionDownloader->SetParamer(pParamer);
						pQuestionDownloader->AddListener(pHandler);//self must be added  at rearmost
						pQuestionDownloader->Transfer();
					}
				}
			}
		}
	}
}

bool CQuestionPreviewDialogUI::OnHandleInsert( void* pObj )
{
	bool doInsert = false;
	CInsertParamer* pParamer = (CInsertParamer*)pObj;
	if(pParamer)
	{
		if (pParamer->GetHttpNotify()&&pParamer->GetHttpNotify()->dwErrorCode == 0)
		{
			if(pParamer->GetUserData()!=NULL)
			{
				CStream* pStream = (CStream*)pParamer->GetUserData();
				if(pStream)
				{
					string strMainXmlPath  = Str2Ansi(pParamer->GetHttpNotify()->strFilePath);
					strMainXmlPath += "\\main.xml";
					bool canInsert = false;
					pStream->ResetCursor();
					string strTitle = Str2Ansi(pStream->ReadString());
					string strGuid = Str2Ansi(pStream->ReadString());
					tstring strQuestionName = pStream->ReadString();
					QUESTION_TYPE type = (QUESTION_TYPE)pStream->ReadDWORD();
					if(type==QUESTION_CLOUD)
					{
						CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
						if (!pMainFrame->IsOperationerChanged(pParamer->GetOperationerId()))
						{
							TiXmlDocument doc;
							bool res = doc.LoadFile(strMainXmlPath.c_str());
							if( res )
							{
								TiXmlElement* pRootElement = doc.FirstChildElement();
								TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
								if( pPagesElement != NULL )
								{
									TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
									while( pPageElement != NULL )
									{
										TCHAR szHref[MAX_PATH];
										_stprintf_s(szHref, _T("pages/%s.xml"), strGuid.c_str());
										tstring strHerf = szHref;
										pPageElement->SetAttribute("id", strGuid.c_str());
										pPageElement->SetAttribute("name", strGuid.c_str());
										pPageElement->SetAttribute("reportable", "true");
										pPageElement->SetAttribute("href", Str2Ansi(strHerf).c_str());
										pPageElement = pPageElement->NextSiblingElement();
									}
									doc.SaveFile(strMainXmlPath.c_str()); 
									// rename file
									string strPageFilePath = strMainXmlPath;
									int pos = strPageFilePath.rfind('\\');
									if( pos != -1 )
										strPageFilePath = strPageFilePath.substr(0, pos);
									string strSearchName = strPageFilePath + "\\pages\\*.xml";
									string strOldName;
									string strNewName;
									WIN32_FIND_DATA FindData;
									HANDLE hFind = FindFirstFile(Ansi2Str(strSearchName).c_str(), &FindData);
									if( hFind != NULL )
									{
										strOldName = strPageFilePath + "\\pages\\";
										strOldName += Str2Ansi(FindData.cFileName);
										strNewName = strPageFilePath + "\\pages\\";
										strNewName += strGuid;
										strNewName += ".xml";
										MoveFile(Ansi2Str(strOldName).c_str(), Ansi2Str(strNewName).c_str());
									}
									// set addonModule.id = BasicQuestionViewer if addonModule.addonId = BasicQuestionViewer
									res = doc.LoadFile(strNewName.c_str());
									if( res )
									{
										TiXmlElement* pRootElement = doc.FirstChildElement();
										TiXmlElement* pAddonModuleElement = GetElementsByTagName(pRootElement, "addonModule");
										if( pAddonModuleElement != NULL )
										{
											const char* pAddonId = pAddonModuleElement->Attribute("addonId");
											if( pAddonId != NULL && !strcmpi(pAddonId, "BasicQuestionViewer"))
												pAddonModuleElement->SetAttribute("id", "BasicQuestionViewer");
											doc.SaveFile(strNewName.c_str());
											// get question type
											tstring strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(strQuestionName.c_str());
											canInsert = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(Ansi2Str(strGuid), strQuestionType, strQuestionName, pParamer->GetHttpNotify()->strFilePath);
										}
									}
								}
							}
						}
					}
					else if(m_eType==QUESTION_INTERACTIVE_CLOUD)
					{
						CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
						if (!pMainFrame->IsOperationerChanged(pParamer->GetOperationerId()))
						{
							TiXmlDocument doc;
							bool res = doc.LoadFile(strMainXmlPath.c_str());
							if( res )
							{
								TiXmlElement* pRootElement = doc.FirstChildElement();
								TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
								if( pPagesElement != NULL )
								{
									TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
									while( pPageElement != NULL )
									{
										pPageElement->SetAttribute("reportable","true");
										pPageElement = pPageElement->NextSiblingElement();
									}
									canInsert = doc.SaveFile(strMainXmlPath.c_str()); 
								}
							}
						}
					}
					if(canInsert)
					{
						doInsert = true;
						InsertQuestionByThread(Ansi2Str(strMainXmlPath).c_str(),Ansi2Str(strTitle).c_str(),Ansi2Str(strGuid).c_str(),0,0,&MakeEventDelegate(this, &CQuestionPreviewDialogUI::OnInsertCompleted));
					}
					delete pStream;
					pParamer->SetUserData(NULL);
				}
			}
		}
		else
		{
			CToast::Toast(_STR_FILE_DWONLOAD_FAILED);
		}

		if (pParamer->GetHttpNotify())
		{
			pParamer->GetHttpNotify()->pUserData = NULL;
		}
		delete pParamer;
	}
	if(!doInsert)
	{
		m_pBtnInsert->SetEnabled(true);
	}
	return true;
}

bool CQuestionPreviewDialogUI::OnInsertCompleted( void* pObj )
{
	TEventNotify* pNotify = (TEventNotify*)pObj;
	if(pNotify)
	{
		CStream* pStream = (CStream*)pNotify->wParam;
		if(pStream)
		{
			pStream->ResetCursor();
			BOOL result= pStream->ReadBOOL();
			if(result==1)
			{
				m_nInsertCount++;
				m_pBtnCancelInsert->SetEnabled(true);
			}
		}
	}
	m_pBtnInsert->SetEnabled(true);
	return true;
}

void CQuestionPreviewDialogUI::OnBtnCancelInsertClick( TNotifyUI& msg )
{
	if (--m_nInsertCount <= 0)
	{
		m_pBtnCancelInsert->SetEnabled(false);
	}
	UnDoByThread();
}

tstring CQuestionPreviewDialogUI::GetUrl()
{
	return m_sCurrentUrl;
}

void CQuestionPreviewDialogUI::AddRetryCount()
{
	m_nRetryCount++;
}

int CQuestionPreviewDialogUI::GetRetryCount()
{
	return m_nRetryCount;
}

void CQuestionPreviewDialogUI::ShowQuestion( tstring url )
{
	m_sCurrentUrl = url;
	CCefObject* pObejct = CefObject::GetInstance();
	if(pObejct&&m_hWebWindow)
	{
		pObejct->Init();
		pObejct->SetQuestionType(m_hWebWindow,0);
		pObejct->CoursePreview(m_hWebWindow,(WCHAR*)Str2Unicode(url).c_str());
		if ( m_hThread != NULL )
		{
			TerminateThread(m_hThread, 0);
			m_hThread = NULL;
		}
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScanThread, this, 0, NULL);
	}
}

bool CQuestionPreviewDialogUI::GetIsWebWindowShown()
{
	return m_bIsWebShown;
}

void CQuestionPreviewDialogUI::ShowWebWindow()
{
	m_bIsWebShown = true;
	if(m_hWebWindow&&IsWindowVisible(this->GetHWND()))
	{
		bool isFullScreenShow = false;
		if(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()&&IsWindowVisible(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()))
		{
			isFullScreenShow = true;
		}
		if(!isFullScreenShow)
		{
			::ShowWindow(m_hWebWindow, SW_SHOW);
			::SetFocus(this->GetHWND());
		}
	}
	if(m_pGifLoading)
	{
		m_pGifLoading->StopGif();
		m_pGifLoading->SetVisible(false);
	}
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
}

void CQuestionPreviewDialogUI::HideQuestion()
{
	if(m_pGifLoading)
	{
		m_pGifLoading->SetVisible(true);
		m_pGifLoading->PlayGif();
	}
	if(m_hWebWindow)
	{
		::ShowWindow(m_hWebWindow, SW_HIDE);
		::SetFocus(this->GetHWND());
	}
	m_bIsWebShown = false;
}

void CQuestionPreviewDialogUI::HideWindow()
{
	m_nInsertCount = 0;
	this->ShowQuestion(_T("about:blank"));
	this->HideQuestion();
	m_pBtnFullScreen->SetEnabled(true);
	m_pBtnInsert->SetEnabled(true);
	m_pBtnCancelInsert->SetEnabled(false);
	this->ShowWindow(false);
	if( m_phSetPosThread  != NULL )
	{
		TerminateThread(m_phSetPosThread, 0);
		CloseHandle(m_phSetPosThread);
		m_phSetPosThread = NULL;
	}
	for (map<CCloudResourceHandler*,CQuestionDownloader*>::iterator itor = m_mDownloaders.begin(); itor != m_mDownloaders.end(); ++itor)
	{
		itor->second->CancelTransfer();
		delete itor->second;
	}
	m_mDownloaders.clear();
	m_pLayoutThumbnailList->RemoveAll();
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
}

bool CQuestionPreviewDialogUI::OnThumbnailClick( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	HideQuestion();
	m_pBtnFullScreen->SetEnabled(false);
	m_pBtnInsert->SetEnabled(false);

	CStyleItemUI* pItem	= (CStyleItemUI*)pNotify->pSender;
	CContainerUI* pContainer = (CContainerUI*)pItem->GetParent();
	if (pContainer)
	{
		m_nCurrentIndex = pContainer->GetItemIndex(pItem);
	}

	CCloudResourceHandler* pHandler = NULL;
	if(m_eType==QUESTION_CLOUD)
	{
		pHandler = (CCloudQuestionHandler*)pItem->GetHandler();
	}
	else if(m_eType==QUESTION_INTERACTIVE_CLOUD)
	{
		pHandler = (CCloudInteractQuestionHandler*)pItem->GetHandler();
	}
	//开启下载
	map<CCloudResourceHandler*,CQuestionDownloader*>::iterator itor = m_mDownloaders.find(pHandler);
	if(itor==m_mDownloaders.end())
	{
		CInvokeParamer* pParamer = new CInvokeParamer();
		pParamer->SetTrigger(pHandler->GetTrigger());

		pParamer->SetCompletedDelegate(MakeDelegate(this, &CQuestionPreviewDialogUI::OnDownloadCompeleted));
		CQuestionDownloader* pQuestionDownloader = NULL;
		if(m_eType==QUESTION_CLOUD)
		{
			pQuestionDownloader = new CQuestionDownloader();
		}
		else if(m_eType==QUESTION_INTERACTIVE_CLOUD)
		{
			pQuestionDownloader = new CInteractQuestionDownloader();
		}
		pQuestionDownloader->SetIsSelfFree(false);
		pQuestionDownloader->SetQuestionGuid(pHandler->GetGuid());
		pQuestionDownloader->SetParamer(pParamer);
		pQuestionDownloader->AddListener(pHandler);//self must be added  at rearmost
		pQuestionDownloader->Transfer();
		m_mDownloaders[pHandler] = pQuestionDownloader;
	}
	else
	{
		itor->second->Transfer();
	}
	return true;
}

bool CQuestionPreviewDialogUI::OnDownloadCompeleted( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;
	if (pParamer&&pParamer->GetTrigger()&&m_pLayoutThumbnailList)
	{
		CThumbnailItemUI* pItem = dynamic_cast<CThumbnailItemUI*>(pParamer->GetTrigger());
		if(pItem)
		{
			int index = m_pLayoutThumbnailList->GetItemIndex(pItem);
			if(index==m_nCurrentIndex)
			{
				if(pParamer->GetHttpNotify()&&pParamer->GetHttpNotify()->dwErrorCode == 0)
				{
					m_pBtnFullScreen->SetEnabled(true);
					m_pBtnInsert->SetEnabled(true);
					tstring strQuestionName = _T("");
					tstring strGuid;
					if(m_eType==QUESTION_CLOUD)
					{
						CCloudQuestionHandler* pHandler = dynamic_cast<CCloudQuestionHandler*>(pItem->GetHandler());
						strGuid = pHandler->GetGuid();
						strQuestionName = pHandler->GetQuestionName();
					}
					else
					{
						CCloudInteractQuestionHandler* pHandler = dynamic_cast<CCloudInteractQuestionHandler*>(pItem->GetHandler());
						strGuid = pHandler->GetGuid();
						strQuestionName = pHandler->GetQuestionName();
					}
					tstring strQuestionDirectoryPath = pParamer->GetHttpNotify()->strFilePath;
					tstring strQuestionPath = strQuestionDirectoryPath +  _T("\\main.xml");
					//显示文本大小
					int fileSize = 0;
					fileSize = GetDirecotySize(strQuestionDirectoryPath.c_str());
					TCHAR szSize[MAX_PATH] = {0};
					_stprintf_s(szSize, _T("%.1fKB"), (float)fileSize/1024.0f);
					m_pLblFileSize->SetText(szSize);

					TCHAR szParam[MAX_PATH*3]={0};
					BOOL res = TRUE;
					if(m_eType==QUESTION_CLOUD)
					{
						tstring strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(strQuestionName.c_str());
						res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(strGuid.c_str(), strQuestionType, strQuestionName, pParamer->GetHttpNotify()->strFilePath);
						if( res )
						{
							// translate slash					
							for(int i = 0; i < (int)strQuestionPath.length(); i++)
							{
								if( strQuestionPath.at(i) == _T('\\') )
									strQuestionPath.replace(i, 1, _T("/"));
							}
							// js player path
							tstring strLocalPath = GetLocalPath();
							// TCHAR szPlayerPath[MAX_PATH*2] = {0};
							tstring strPlayerPath=GetHtmlPlayerPath();
							tstring ntitle = _T("预览");
							_stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell&hidePage=footer %s"), 
								UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
								UrlEncode(Str2Utf8(strQuestionPath)).c_str(),
								ntitle.c_str());
						}
					}
					else
					{
						// translate slash
						for(int i = 0; i < (int)strQuestionPath.length(); i++)
						{
							if( strQuestionPath.at(i) == _T('\\') )
								strQuestionPath.replace(i, 1, _T("/"));
						}
						// js player path
						tstring strLocalPath = GetLocalPath();
						tstring strPlayerPath = GetHtmlPlayerPath();
						_stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell&hidePage=footer"), 
							UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
							UrlEncode(Str2Utf8(strQuestionPath)).c_str());
					}
					if(res)
					{
						tstring strUrl = szParam;
						ShowQuestion(strUrl);
						//
						if(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()&&IsWindowVisible(CQuestionPreviewFullScreenUI::GetInstance()->GetHWND()))
						{
							CQuestionPreviewFullScreenUI::GetInstance()->ShowQuestion(strUrl);
						}
					}
				}
				else
				{
					ShowQuestion(_T("about:blank"));
					HideQuestion();
					if(m_pGifLoading)
					{
						m_pGifLoading->SetVisible(false);
					}
					CToast::Toast(_T("习题下载失败"));
				}
			}
		}
	}
	return true;
}

int CQuestionPreviewDialogUI::GetDirecotySize( CString strDirectoryPath )
{
	int fileSize = 0;
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strDirectoryPath + _T("\\") + _T("*.*"));
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		if(!finder.IsDots())
		{
			CString strFileName = finder.GetFileName();
			CString strTempPath = strDirectoryPath + _T("\\") + strFileName;
			if(finder.IsDirectory())
			{
				fileSize+=GetDirecotySize(strTempPath);
			}
			else
			{
				WIN32_FIND_DATA FindFileData;
				HANDLE hFile = ::FindFirstFile(strTempPath, &FindFileData);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					fileSize+=FindFileData.nFileSizeLow;
				}
			}
		}
	}
	finder.Close();
	return fileSize;
}

bool CQuestionPreviewDialogUI::Initialize( HINSTANCE hInstance )
{
	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS ;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("CoursePlayWebUIEx");
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
	return true;
}

LRESULT CALLBACK CQuestionPreviewDialogUI::WndProc( HWND hwnd, /* handle to window */ UINT uMsg, /* message identifier */ WPARAM wParam, /* first message parameter */ LPARAM lParam /* second message parameter */ )
{
	switch(uMsg)
	{
	case WM_SIZE:
		{
			return false;
		}
		break;
	case WM_COPYDATA:
		{
			COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)lParam;
			switch (pCopyDataStruct->dwData)
			{ 	
			case MSG_CEF_ICRINVOKENATIVE:
				break;
			case MSG_CEFINFO:
				break;
			case MSG_ESC:
				{
					CQuestionPreviewDialogUI * pDlg = QuestionPreviewDialogUI::GetInstance();
					if(pDlg)
					{
						pDlg->HideWindow();
					}
				}
				break;
			case MSG_PLAYERLOADED:
				{
					CQuestionPreviewDialogUI * pDlg = QuestionPreviewDialogUI::GetInstance();
					if(pDlg)
					{
						pDlg->ShowWebWindow();
					}
				}
				break;
			case MSG_URLCHANGE:
				{
					TCHAR * pszUrl = new TCHAR[pCopyDataStruct->cbData + 1];
					pszUrl[pCopyDataStruct->cbData] = 0;
					memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					tstring strUrl = pszUrl;
					delete pszUrl;
					CQuestionPreviewDialogUI * pDlg = QuestionPreviewDialogUI::GetInstance();
					if(pDlg)
					{
						//pDlg->ShowQuestion(AnsiToUnicode(strUrl));
						/*CCefObject* pObejct = CefObject::GetInstance();
						pObejct->Init();
						pObejct->CoursePreview(pDlg->m_hWebWindow,(WCHAR *)strUrl.c_str());*/
					}
				}
				break;
			case MSG_URLFAILED:
				{
					CQuestionPreviewDialogUI* pDlg = QuestionPreviewDialogUI::GetInstance();
					if(pDlg)
					{
						pDlg->AddRetryCount();
						if (pDlg->GetRetryCount()<3){
							tstring strUrl = pDlg->GetUrl();
							if(!strUrl.empty())
							{
								tstring strWorkDirectory = GetLocalPath();
								strWorkDirectory += _T("\\Package\\nodejs");
								tstring strExePath = strWorkDirectory;
								strExePath += _T("\\node.exe");
								ShellExecute(NULL, _T("open"), strExePath.c_str(), _T("app.js"), strWorkDirectory.c_str(), SW_HIDE);
								pDlg->ShowQuestion(strUrl);
								/*CCefObject* pObejct = CefObject::GetInstance();
								pObejct->Init();
								pObejct->CoursePreview(pDlg->m_hWebWindow,(WCHAR *)strUrl.c_str());*/
							}
						}
					}
				}
				break;
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD  WINAPI CQuestionPreviewDialogUI::ScanThread( LPARAM lParam )
{
	Sleep(10*1000);
	CQuestionPreviewDialogUI* pDlg = QuestionPreviewDialogUI::GetInstance();
	if(pDlg)
	{
		pDlg->ShowWebWindow();
	}
	return 1;
}