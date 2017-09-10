#include "StdAfx.h"
#include "UpdateLogUI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"
#include "Util/Util.h"
#include "Config.h"
#include <algorithm>
#include "IWantMoreUI.h"

int	CompareVersion(tstring strVersion1, tstring strVersion2);

CUpdateLogUI::CUpdateLogUI(void)
{
	m_pLayoutUpdateLog = NULL;
	m_pLayoutLoading = NULL;
	m_pGifLoading = NULL;
	m_pLayoutLoadingFail=NULL;
	m_pGifLoadingFail=NULL;
	m_pLayoutPraise = NULL;
	m_pLayoutLogList = NULL;
	m_dwLogTaskId=0;
	m_dwPraiseTaskId=0;
	m_strVersion="";
	m_strLogFilePath = GetLocalPath();
	m_strLogFilePath = m_strLogFilePath + _T("\\Setting\\VersionLog.dat");
	m_strPraiseFilePath = GetLocalPath();
	m_strPraiseFilePath = m_strPraiseFilePath + _T("\\Setting\\Praise.dat");
	m_strPraiseDateFilePath = GetLocalPath();
	m_strPraiseDateFilePath = m_strPraiseDateFilePath + _T("\\Setting\\PraiseDate.dat");
	m_bLogLoadCompeleted = false;
	m_bLogLoadSuccess = false;
	m_bPraiseLoadCompeleted = false;
	m_bPraiseLoadSuccess = false;
	m_bHasShowCache = false;
	//InitializeCriticalSection(&m_Lock);
}

CUpdateLogUI::~CUpdateLogUI(void)
{
	StopGetInfoTask();
	//DeleteCriticalSection(&m_Lock);
}

LPCTSTR CUpdateLogUI::GetWindowClassName( void ) const
{
	return _T("UpdateLogDlg");
}

DuiLib::CDuiString CUpdateLogUI::GetSkinFile()
{
	return _T("UpdateLog\\UpdateLog.xml");
}

DuiLib::CDuiString CUpdateLogUI::GetSkinFolder()
{
	return _T("skins");
}

void CUpdateLogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	StopGetInfoTask();
	//DeleteCriticalSection(&m_Lock);
	Close();
}

CControlUI* CUpdateLogUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return NULL;
}

void CUpdateLogUI::InitWindow()
{
	m_pLayoutUpdateLog = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutUpdateLog")));
	m_pLayoutLoading = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutLoading")));
	m_pGifLoading = static_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("gifLoading")));
	m_pLayoutLoadingFail = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutLoadingFail")));
	m_pGifLoadingFail = static_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("gifLoadingFail")));
	m_pLayoutPraise = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutPraise")));
	m_pLayoutLogList = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl("layoutLogList"));

	//读取点赞时间缓存
	ReadPraiseDateFile();

	WIN32_FIND_DATA FindFileData;
	HANDLE hLogFind = ::FindFirstFile(m_strLogFilePath.c_str(), &FindFileData);
	HANDLE hPraiseFind = ::FindFirstFile(m_strPraiseFilePath.c_str(), &FindFileData);
	if (hLogFind != INVALID_HANDLE_VALUE&&hPraiseFind!=INVALID_HANDLE_VALUE)
	{
		ReadVersionLogFile();
		ReadPraiseFile();
		ShowContent();
		m_bHasShowCache = true;
	}
	else
	{
		ShowLoading();
	}
	
	//开启获取信息任务
	StarGetInfoTask();
}

bool ItemCompare(const Json::Value& item1,const Json::Value& item2)  
{      
	tstring strVersion1 = item1["versionCode"].asCString();
	tstring strVersion2 = item2["versionCode"].asCString();

	int ret = CompareVersion(strVersion1, strVersion2);
	if( ret <= 0 )
		return false; 
	return true;
}

void CUpdateLogUI::ShowLoading()
{
	HideLoadingFail();
	//HideContent();
	if(m_pLayoutLoading)
	{
		if(m_pGifLoading)
		{
			m_pGifLoading->PlayGif();
		}
		m_pLayoutLoading->SetVisible(true);
	}
}

void CUpdateLogUI::HideLoading()
{
	if(m_pLayoutLoading)
	{
		if(m_pGifLoading)
		{
			m_pGifLoading->StopGif();
		}
		m_pLayoutLoading->SetVisible(false);
	}
}

void CUpdateLogUI::ShowLoadingFail()
{
	HideLoading();
	//HideContent();
	if(m_pLayoutLoadingFail)
	{
		if(m_pGifLoadingFail)
		{
			m_pGifLoadingFail->PlayGif();
		}
		m_pLayoutLoadingFail->SetVisible(true);
	}
}

void CUpdateLogUI::HideLoadingFail()
{
	if(m_pLayoutLoadingFail)
	{
		if(m_pGifLoadingFail)
		{
			m_pGifLoadingFail->StopGif();
		}
		m_pLayoutLoadingFail->SetVisible(false);
	}
}

#pragma region 内容列表显示相关
void CUpdateLogUI::ShowContent()
{
	HideLoadingFail();
	HideLoading();
	if(m_pLayoutUpdateLog)
	{
		//加载点赞列表
		m_pLayoutPraise->RemoveAll();
		if(m_vPraises.size()>0&&m_pLayoutPraise)
		{
			for (int i=0;i<m_vPraises.size();i++)
			{
				CPraiseInfo info = m_vPraises[i];
				CHorizontalLayoutUI* pItem=NULL;
				if( !m_builderPraise.GetMarkup()->IsValid() )
				{
					pItem = dynamic_cast<CHorizontalLayoutUI*>(m_builderPraise.Create(_T("UpdateLog\\PraiseItemStyle.xml"), (UINT)0, this, &m_PaintManager));
				}
				else 
				{
					pItem = dynamic_cast<CHorizontalLayoutUI*>(m_builderPraise.Create(this, &m_PaintManager));
				}
				if(pItem)
				{
					CLabelUI* lblText = static_cast<CLabelUI*>(pItem->FindSubControl(_T("lblText")));
					if(lblText)
					{
						lblText->SetText(info.GetText().c_str());
						lblText->SetFont(120000);
						lblText->SetTextColor(0xFF000001);
					}
					CButtonUI* btnPraise = static_cast<CButtonUI*>(pItem->FindSubControl(_T("btnPraise")));
					if(btnPraise)
					{
						btnPraise->OnNotify+=MakeDelegate(this,&CUpdateLogUI::OnPraiseItemClick);
						CPraiseInfo* infoPtr = &(m_vPraises[i]);
						btnPraise->SetTag((UINT_PTR)infoPtr);
						if(info.GetIsParised())
						{
							btnPraise->SetBkImage("UpdateLog\\praise.png");
						}
						else
						{
							btnPraise->SetBkImage("UpdateLog\\unpraise.png");
						}
					}
					CLabelUI* lblNum = static_cast<CLabelUI*>(pItem->FindSubControl(_T("lblNum")));
					if(lblNum)
					{
						char cNum[20];
						itoa(info.GetNum(),cNum,10);
						lblNum->SetText(cNum);
						if(info.GetIsParised())
						{
							lblNum->SetTextColor(0xFFEF8307);
						}
						else
						{
							lblNum->SetTextColor(0xFF888687);
						}
					}
					m_pLayoutPraise->Add(pItem);
				}
			}
		}
		//加载日志列表
		ShowUpdateLog();
		m_pLayoutUpdateLog->SetVisible(true);
	}
}

void CUpdateLogUI::ShowUpdateLog()
{
	if(m_pLayoutLogList)
	{
		m_pLayoutLogList->RemoveAll();
		tstring strExeVersion = g_Config::GetInstance()->GetVersion();
		tstring strLastYear = "";
		Json::Value& items = m_LogRootItem["items"];
		if( items.size() == 0 )
			return;

		// sort items
		vector<Json::Value> vecItems;
		for(int i = items.size()-1; i >= 0; i--)
		{
			Json::Value item = items[i];
			vecItems.push_back(item);
		}

		sort(vecItems.begin(), vecItems.end(), ItemCompare);

		for(int i = 0; i < vecItems.size(); i++)
		{
			Json::Value& item = vecItems[i];

			tstring strDate = item[_T("date")].asCString();
			tstring strVersion = item["versionCode"].asCString();
			tstring strContent = item["content"].asCString();

			tstring strYear = strDate.substr(0, 4);	 

			int st=strDate.find_first_of('-');
			tstring strMonthDay=_T("");
			if (st>0)
				strMonthDay=strDate.substr(st+1);
			else
				strMonthDay = strDate.substr(6);

			// compare version
			if( CompareVersion(strVersion, strExeVersion) > 0 )
				continue;
			bool isCurrent = false;
			if(i==0)
			{
				isCurrent=true;
			}
			if( strYear != strLastYear )
			{
				CreatYearDot(m_pLayoutLogList, strYear.c_str(),isCurrent);
				strLastYear = strYear;
			}
			CreateItemSytle(m_pLayoutLogList, strMonthDay.c_str(), strVersion.c_str(), strContent.c_str(),isCurrent);
		}
		//m_pLayoutLogList->SetFixedHeight(m_nLogListHeight);
		m_pLayoutLogList->SetVisible(true);
	}
}

void CUpdateLogUI::CreatYearDot(CContainerUI* pParent, const char* lpszYear,bool isCurrent)
{
	if(pParent)
	{
		CHorizontalLayoutUI*  layoutYear= NULL;
		if( !m_builderYear.GetMarkup()->IsValid() ) 
		{
			layoutYear = dynamic_cast<CHorizontalLayoutUI*>(m_builderYear.Create(_T("UpdateLog\\LogYearStyle.xml"), (UINT)0, this, &m_PaintManager));
		}
		else 
		{
			layoutYear = dynamic_cast<CHorizontalLayoutUI*>(m_builderYear.Create(this, &m_PaintManager));
		}
		if(layoutYear)
		{
			CLabelUI* lblYear = dynamic_cast<CLabelUI*>(layoutYear->FindSubControl("lblYear"));
			if(lblYear)
			{
				lblYear->SetText(lpszYear);
			}
			if(isCurrent)
			{
				CControlUI* ctrLineTop = dynamic_cast<CControlUI*>(layoutYear->FindSubControl("ctrLineTop"));
				if(ctrLineTop)
				{
					ctrLineTop->SetVisible(false);
				}
			}
			int height = layoutYear->GetFixedHeight();
			pParent->Add(layoutYear);
		}
	}
}

void CUpdateLogUI::CreateItemSytle(CContainerUI* pParent, const char* lpszMothDate, const char* lpszTitle, const char* lpszLogContent,bool isCurrent)
{
	if(pParent)
	{
		CHorizontalLayoutUI * pItem = NULL;
		if( !m_builderLogItem.GetMarkup()->IsValid() )
		{
			pItem = dynamic_cast<CHorizontalLayoutUI*>(m_builderLogItem.Create(_T("UpdateLog\\LogItemStyle.xml"), (UINT)0, this, &m_PaintManager));
		}
		else 
		{
			pItem = dynamic_cast<CHorizontalLayoutUI*>(m_builderLogItem.Create(this, &m_PaintManager));
		}
		if(pItem)
		{
			int heigth = 0;//Item的高度
			CLabelUI* lblDate = dynamic_cast<CLabelUI*>(pItem->FindSubControl("lblDate"));//日期
			if(lblDate)
			{
				lblDate->SetText(lpszMothDate);
				if(isCurrent)
				{
					lblDate->SetTextColor(0xFF333333);
				}
				else
				{
					lblDate->SetTextColor(0xFFA5A5A5);
				}
			}
			CLabelUI* lblVersonTitle = dynamic_cast<CLabelUI*>(pItem->FindSubControl("lblVersonTitle"));//版本号
			if(lblVersonTitle)
			{
				string strTmp = lpszTitle;
				string strVerTitle = "版本：";
				strVerTitle+=strTmp;
				lblVersonTitle->SetText(strVerTitle.c_str());
				if(isCurrent)
				{
					lblVersonTitle->SetTextColor(0xFF333333);
				}
				else
				{
					lblVersonTitle->SetTextColor(0xFFa7a7a7);
				}
				heigth+=lblVersonTitle->GetFixedHeight();
			}
			CVerticalLayoutUI* layoutContent = dynamic_cast<CVerticalLayoutUI*>(pItem->FindSubControl("layoutContent"));//更新内容
			if(layoutContent)
			{
				RECT rcCalc = {0};
				int nContainerWidth = 500;
				rcCalc.right = nContainerWidth;
				
				CLabelUI* pLabel = new CLabelUIEx;
				pLabel->SetText(lpszLogContent);
				pLabel->SetFont(120000);
				if(isCurrent)
				{
					pLabel->SetTextColor(0xFF888888);
				}
				else
				{
					pLabel->SetTextColor(0xFFc9c9c9);
				}
				//计算文本高度
				this->CalcText(m_PaintManager.GetPaintDC(), rcCalc, lpszLogContent, pLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
				pLabel->SetFixedWidth(nContainerWidth);
				pLabel->SetFixedHeight(rcCalc.bottom);
				layoutContent->SetFixedHeight(rcCalc.bottom);
				heigth+=rcCalc.bottom;
				layoutContent->Add(pLabel);
			}
			CContainerUI* containerContent = dynamic_cast<CContainerUI*>(pItem->FindSubControl("containerContent"));
			if(containerContent)
			{
				RECT rectInset= containerContent->GetInset();
				heigth+=rectInset.top;
				heigth+=rectInset.bottom;
				heigth+=16;//版本与内容的间距
				containerContent->SetFixedHeight(heigth);
				if(isCurrent)
				{
					containerContent->SetBkImage("file='UpdateLog\\log_item_now_bk.png' corner='11,32,10,3'");
				}
				else
				{
					containerContent->SetBkImage("file='UpdateLog\\log_item_pass_bk.png' corner='11,32,10,3'");
				}
			}
			heigth+=8;//占位符高度
			pItem->SetFixedHeight(heigth);
			pParent->Add(pItem);
		}
	}
}

void  CUpdateLogUI::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
{
	if (nFontType == UIFONT_GDI)
	{
		HFONT hFont = m_PaintManager.GetFont(nFontId);
		HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
		if ((DT_SINGLELINE & format))
		{
			SIZE size = {0};
			::GetTextExtentExPoint(hdc, lpszText, c == -1 ? _tcslen(lpszText) : c, 0, NULL, NULL, &size);
			rc.right = rc.left + size.cx;
			rc.bottom = rc.top + size.cy;
		}
		else
		{
			format &= ~DT_END_ELLIPSIS;
			format &= ~DT_PATH_ELLIPSIS;
			if (!(DT_SINGLELINE & format)) format |= DT_WORDBREAK | DT_EDITCONTROL;
			::DrawText(hdc, lpszText, c, &rc, format | DT_CALCRECT);
		}
		::SelectObject(hdc, hOldFont);
	}
}

void CUpdateLogUI::HideContent()
{
	if(m_pLayoutUpdateLog)
	{
		m_pLayoutUpdateLog->SetVisible(false);
	}
}
#pragma endregion

#pragma region 获取日志及点赞内容相关
void CUpdateLogUI::StarGetInfoTask()
{
	m_bLogLoadCompeleted = false;
	m_bLogLoadSuccess = false;
	m_bPraiseLoadCompeleted = false;
	m_bPraiseLoadSuccess = false;
	StopGetInfoTask();
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if( pHttpDownloadManager != NULL )
	{
		//开启获取日志任务
		tstring strLogUrl;

		strLogUrl = _T("/101ppt/versionLog.php?version=") + m_strVersion;
#ifndef DEVELOP_VERSION
		strLogUrl += "&build=final";
#endif
		m_dwLogTaskId = pHttpDownloadManager->AddTask(_T("p.101.com"),strLogUrl.c_str(),_T(""),_T("GET"),_T(""),80, 
			MakeHttpDelegate(this, &CUpdateLogUI::OnUpdateLogCompeleted),
			MakeHttpDelegate(NULL),
			MakeHttpDelegate(NULL));
		//开启获取点赞列表任务
		string strPraiseUrl="/101ppt/getLikes.php";
		m_dwPraiseTaskId = pHttpDownloadManager->AddTask(_T("p.101.com"),strPraiseUrl.c_str(),_T(""),_T("GET"),_T(""),80, 
			MakeHttpDelegate(this, &CUpdateLogUI::OnPraiseCompeleted),
			MakeHttpDelegate(NULL),
			MakeHttpDelegate(NULL));
	}
}

void CUpdateLogUI::StopGetInfoTask()
{
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
	if( pHttpDownloadManager != NULL )
	{
		pHttpDownloadManager->CancelTask(m_dwLogTaskId);
		pHttpDownloadManager->CancelTask(m_dwPraiseTaskId);
	}
}

bool CUpdateLogUI::OnPraiseCompeleted( void* param )
{
	//EnterCriticalSection(&m_Lock);
	m_bPraiseLoadCompeleted = true;
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify->dwErrorCode==0)
	{
		m_bPraiseLoadSuccess = true;
		pNotify->pData[pNotify->nDataSize] = '\0';
		WritePraiseFile(pNotify->pData);
	}
	else
	{
		m_bPraiseLoadSuccess = false;
	}
	//
	if(m_bLogLoadCompeleted&&m_bPraiseLoadCompeleted)
	{
		if(m_bLogLoadSuccess&&m_bPraiseLoadSuccess)
		{
			ShowContent();
		}
		else if(!m_bHasShowCache)
		{
			ShowLoadingFail();
		}
	}
	//LeaveCriticalSection(&m_Lock);
	return true;
}

bool CUpdateLogUI::OnUpdateLogCompeleted(void *param)
{
	//EnterCriticalSection(&m_Lock);
	m_bLogLoadCompeleted = true;
	THttpNotify* pNotify = (THttpNotify*)param;
	if(pNotify->dwErrorCode==0)
	{
		m_bLogLoadSuccess = true;
		pNotify->pData[pNotify->nDataSize] = '\0';
		WriteVersionLogFile(pNotify->pData);
	}
	else
	{
		m_bLogLoadSuccess = false;
	}
	//
	if(m_bLogLoadCompeleted&&m_bPraiseLoadCompeleted)
	{
		if(m_bLogLoadSuccess&&m_bPraiseLoadSuccess)
		{
			ShowContent();
		}
		else if(!m_bHasShowCache)
		{
			ShowLoadingFail();
		}
	}
	//LeaveCriticalSection(&m_Lock);
	return true;
}

bool CUpdateLogUI::ReadVersionLogFile()
{
	// read json from file
	FILE* fp = NULL;
	int err = _tfopen_s(&fp, m_strLogFilePath.c_str(), "rb");
	if( err != 0 )
		return false;

	fseek(fp, 0, SEEK_END);
	int nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if( nSize == 0 )
	{
		fclose(fp);
		return false;
	}
	//
	char* pBuffer = new char[nSize];
	if( pBuffer == NULL )
	{
		fclose(fp);
		return false;
	}

	fread(pBuffer, nSize, 1, fp);
	fclose(fp);


	Json::Reader reader;
	bool res = reader.parse(pBuffer, m_LogRootItem);
	if( !res )
	{
		delete pBuffer;
		return false;
	}

	if( !m_LogRootItem["versionCode"].isNull() )
		m_strVersion = m_LogRootItem["versionCode"].asCString();

	delete pBuffer;
	return true;
}

bool CUpdateLogUI::WriteVersionLogFile(char* strJson)
{
	Json::Value newitems;

	tstring strAnsi = Utf8ToAnsi(strJson);

	Json::Reader reader;
	bool res = reader.parse(strAnsi, newitems);
	if( !res )
		return false;

	if( newitems.size() == 0 )
		return true;

	Json::Value items(Json::arrayValue);

	if( !m_LogRootItem["items"].isNull() )
		items = m_LogRootItem["items"];


	Json::Value lastItem;
	for(int i = 0; i < newitems.size(); i++)
	{
		Json::Value& item = newitems[i];
		items.append(item);

		if( i == newitems.size() -1 )
			lastItem = newitems[i];
	}

	m_LogRootItem["items"] = items;
	m_LogRootItem["versionCode"] = lastItem["versionCode"].asCString();

	tstring str = m_LogRootItem.toStyledString();

	FILE* fp = NULL;
	int err = _tfopen_s(&fp, m_strLogFilePath.c_str(), "wb");
	if( err != 0 )
		return false;

	fwrite(str.c_str(), str.length(), 1, fp);
	fclose(fp);
	return false;
}

void CUpdateLogUI::ReadPraiseFile()
{
	FILE* fp = NULL;
	try
	{
		int err = _tfopen_s(&fp, m_strPraiseFilePath.c_str(), "rb");
		if( err == 0 )
		{
			fseek(fp, 0, SEEK_END);
			int nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			if(nSize>0)
			{
				char* pBuffer = new char[nSize];
				if( pBuffer != NULL )
				{
					fread(pBuffer, nSize, 1, fp);
					fclose(fp);
					Json::Reader reader;
					Json::Value jRootItem;
					bool res = reader.parse(pBuffer, jRootItem);
					if(res)
					{
						if(jRootItem.isArray()&&jRootItem.size()>0)
						{
							m_vPraises.clear();
							for (int i=0;i<jRootItem.size();i++)
							{
								Json::Value item = jRootItem[i];
								CPraiseInfo info;
								Json::Value jId	= item["id"];
								if(jId.isInt())
								{
									info.SetId(jId.asInt());
								}
								Json::Value jNum = item["num"];
								if(jNum.isInt())
								{
									info.SetNum(jNum.asInt());
								}
								Json::Value jText = item["text"];
								if(jText.isString())
								{
									//添加序号
									char cNum[20];
									itoa(i+1,cNum,10);
									string text = cNum;
									text+="."+jText.asString();
									info.SetText(text);
								}
								//根据点赞列表设置是否已经点赞过
								for (map<int,int>::iterator itor=m_mapPraiseDate.begin();itor!=m_mapPraiseDate.end();itor++)
								{
									if(itor->first==info.GetId())
									{
										int date = itor->second;
										SYSTEMTIME st = {0};
										GetLocalTime(&st);
										int dateNow = st.wYear*10000+st.wMonth*100+st.wDay;
										if(date<dateNow)
										{
											info.SetIsParised(false);
										}
										else
										{
											info.SetIsParised(true);
										}
										break;
									}
								}
								m_vPraises.push_back(info);
							}
						}
					}
					delete pBuffer;
				}
			}
		}
	}
	catch (...)
	{
	}
	fclose(fp);
}

void CUpdateLogUI::WritePraiseFile(char* strJson)
{
	try
	{
		Json::Value items;
		string strAnsi = Utf8ToAnsi(strJson);
		Json::Reader reader;
		bool res = reader.parse(strAnsi, items);
		if(res&&items.size()>0)
		{
			//覆盖缓存文件
			string strResult = items["result"].toStyledString();
			FILE* fp = NULL;
			int err = _tfopen_s(&fp, m_strPraiseFilePath.c_str(), "wb");
			if( err == 0 )
			{
				fwrite(strResult.c_str(), strResult.length(), 1, fp);
			}
			fclose(fp);
			//
			if(items["result"].size()>0)
			{
				m_vPraises.clear();
				for (int i=0;i<items["result"].size();i++)
				{
					Json::Value item = items["result"][i];
					CPraiseInfo info;
					Json::Value jId	= item["id"];
					if(jId.isInt())
					{
						info.SetId(jId.asInt());
					}
					Json::Value jNum = item["num"];
					if(jNum.isInt())
					{
						info.SetNum(jNum.asInt());
					}
					Json::Value jText = item["text"];
					if(jText.isString())
					{
						//添加序号
						char cNum[20];
						itoa(i+1,cNum,10);
						string text = cNum;
						text+="."+jText.asString();
						info.SetText(text);
					}
					//根据点赞列表设置是否已经点赞过
					for (map<int,int>::iterator itor=m_mapPraiseDate.begin();itor!=m_mapPraiseDate.end();itor++)
					{
						if(itor->first==info.GetId())
						{
							int date = itor->second;
							SYSTEMTIME st = {0};
							GetLocalTime(&st);
							int dateNow = st.wYear*10000+st.wMonth*100+st.wDay;
							if(date<dateNow)
							{
								info.SetIsParised(false);
							}
							else
							{
								info.SetIsParised(true);
							}
							break;
						}
					}
					m_vPraises.push_back(info);
				}
			}
		}
	}
	catch (...)
	{
	}
}
#pragma endregion

bool CUpdateLogUI::OnPraiseItemClick( void* pNotify )
{
	TNotifyUI* msg = (TNotifyUI*)pNotify;
	if (msg&&msg->sType == _T("click"))
	{
		if(msg->pSender!=NULL && msg->pSender->GetTag()!=NULL)
		{
			CPraiseInfo* info = (CPraiseInfo*)(msg->pSender->GetTag());
			if(info&&!info->GetIsParised())
			{
				try
				{
					//设置数据
					int num = info->GetNum();
					num++;
					info->SetNum(num);
					info->SetIsParised(true);
					//设置控件显示
					msg->pSender->SetBkImage("UpdateLog\\praise.png");
					CHorizontalLayoutUI* ctrParent = static_cast<CHorizontalLayoutUI*>(msg->pSender->GetParent());
					if(ctrParent)
					{
						CLabelUI* lblNum=static_cast<CLabelUI*>(ctrParent->FindSubControl("lblNum"));
						if(lblNum)
						{
							char cNum[20];
							itoa(num,cNum,10);
							lblNum->SetText(cNum);
							lblNum->SetTextColor(0xFFEF8307);
						}
					}
					//开启提交点赞任务
					CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();
					if( pHttpDownloadManager != NULL )
					{
						vector<pair<string, string>> vecIpMacs;
						string strMac = "";
						if(GetLocalIpMacs(vecIpMacs)&&vecIpMacs.size()>0)
						{
							pair<string, string> pairPpMac=vecIpMacs[0];
							strMac = pairPpMac.second;
						}
						string strPostUrl=_T("/101ppt/addLikeNum.php");
						TCHAR  szPost[1024] = {0};
						_stprintf(szPost, _T("id=%d&mac=%s"),info->GetId(),strMac.c_str());
						m_dwLogTaskId = pHttpDownloadManager->AddTask(_T("p.101.com"),strPostUrl.c_str(),_T(""),_T("POST"),szPost,80, 
							MakeHttpDelegate(NULL),
							MakeHttpDelegate(NULL),
							MakeHttpDelegate(NULL));
					}
					//缓存点赞日期
					SYSTEMTIME st = {0};
					GetLocalTime(&st);
					int dateNow = st.wYear*10000+st.wMonth*100+st.wDay;
					WritePraiseDateFile(info->GetId(),dateNow);
				}
				catch(...)
				{

				}
			}
		}
	}
	return true;
}

void CUpdateLogUI::OnBtnSuggestClick( TNotifyUI& msg )
{
	CRect rect;
	::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd(),&rect);
	CIWantMoreUI pIWantMoreDlg;
	pIWantMoreDlg.Create(this->GetHWND(), _T("SuggestWindow"), WS_OVERLAPPED, 0, 0, 0, 0, 0);
	pIWantMoreDlg.Init(rect);
	pIWantMoreDlg.ShowModal();
}

void CUpdateLogUI::OnBtnRefreshClick( TNotifyUI& msg )
{
	ShowLoading();
	StarGetInfoTask();
}

void CUpdateLogUI::ReadPraiseDateFile()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hPraiseDateFind = ::FindFirstFile(m_strPraiseDateFilePath.c_str(), &FindFileData);
	if (hPraiseDateFind != INVALID_HANDLE_VALUE)
	{
		FILE* fp = NULL;
		try
		{
			int err = _tfopen_s(&fp, m_strPraiseDateFilePath.c_str(), "rb");
			if( err == 0 )
			{
				fseek(fp, 0, SEEK_END);
				int nSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				if(nSize>0)
				{
					char* pBuffer = new char[nSize];
					if( pBuffer != NULL )
					{
						fread(pBuffer, nSize, 1, fp);
						fclose(fp);
						Json::Reader reader;
						Json::Value jRootItem;
						bool res = reader.parse(pBuffer, jRootItem);
						if(res)
						{
							if(jRootItem.isArray()&&jRootItem.size()>0)
							{
								m_mapPraiseDate.clear();
								for (int i=0;i<jRootItem.size();i++)
								{
									int id=0;
									int date=0;
									Json::Value item = jRootItem[i];
									Json::Value jId	= item["id"];
									if(jId.isInt())
									{
										id=jId.asInt();
									}
									Json::Value jNum = item["date"];
									if(jNum.isInt())
									{
										date = jNum.asInt();
									}
									m_mapPraiseDate.insert(map<int,int> :: value_type(id,date));
								}
							}
						}
						delete pBuffer;
					}
				}
			}
		}
		catch (...)
		{
		}
		fclose(fp);
	}
}

void CUpdateLogUI::WritePraiseDateFile( int n_id,int n_date )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hPraiseDateFind = ::FindFirstFile(m_strPraiseDateFilePath.c_str(), &FindFileData);
	if (hPraiseDateFind != INVALID_HANDLE_VALUE)
	{
		Json::Value jRootItem =NULL;
		//读取
		FILE* fp = NULL;
		int err = _tfopen_s(&fp, m_strPraiseDateFilePath.c_str(), "rb");
		if( err == 0 )
		{
			fseek(fp, 0, SEEK_END);
			int nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			if(nSize>0)
			{
				char* pBuffer = new char[nSize];
				if( pBuffer != NULL )
				{
					fread(pBuffer, nSize, 1, fp);
					fclose(fp);
					Json::Reader reader;
					bool res = reader.parse(pBuffer, jRootItem);
					if(res)
					{
						if(jRootItem.isArray()&&jRootItem.size()>0)
						{
							bool bFind = false;
							for (int i=0;i<jRootItem.size();i++)
							{
								int id=0;
								Json::Value item = jRootItem[i];
								Json::Value jId	= item["id"];
								if(jId.isInt())
								{
									id=jId.asInt();
								}
								if(id==n_id)
								{
									bFind = true;
									jRootItem[i]["date"]=n_date;
									break;
								}
							}
							if(!bFind)
							{
								Json::Value jValue;
								jValue["id"] = n_id;
								jValue["date"] = n_date;
								jRootItem.append(jValue);
							}
						}
					}
					delete pBuffer;
				}
			}
		}
		//修改
		if(jRootItem!=NULL)
		{
			string strResult = jRootItem.toStyledString();
			FILE* fp = NULL;
			int err = _tfopen_s(&fp, m_strPraiseDateFilePath.c_str(), "wb");
			if( err == 0 )
			{
				fwrite(strResult.c_str(), strResult.length(), 1, fp);
			}
			fclose(fp);
		}
	}
	else
	{
		Json::Value jRootItem(Json::arrayValue);
		Json::Value jItem;
		jItem["id"] = n_id;
		jItem["date"] = n_date;
		jRootItem.append(jItem);
		string strResult = jRootItem.toStyledString();
		FILE* fp = NULL;
		int err = _tfopen_s(&fp, m_strPraiseDateFilePath.c_str(), "wb");
		if( err == 0 )
		{
			fwrite(strResult.c_str(), strResult.length(), 1, fp);
		}
		fclose(fp);
	}
}

int CompareVersion(tstring strVersion1, tstring strVersion2)
{
	vector<tstring> vecStrings1 = SplitString(strVersion1, strVersion1.length(), _T('.'), true);
	vector<tstring> vecStrings2 = SplitString(strVersion2, strVersion2.length(), _T('.'), true);

	if( vecStrings1.size() != vecStrings2.size() )
		return 0;

	for(int i = 0; i < vecStrings1.size(); i++)
	{
		tstring str1 = vecStrings1[i];
		tstring str2 = vecStrings2[i];

		int val1 = atoi(str1.c_str());
		int val2 = atoi(str2.c_str());

		if( val1 < val2 )
			return -1;

		else if( val1 > val2 )
			return 1;
	}

	return 0;

}

