#include "StdAfx.h"
#include "CourseToolsUI.h"
#include "NDCloud\NDCloudFile.h"
#include "Util\Util.h"
#include "Statistics\Statistics.h"
#include "PPTControl/PPTController.h"

CCourseToolsUI::CCourseToolsUI(void):m_pContainer(NULL)
{
	GetModuleFileName(NULL,m_szCurDir,MAX_PATH);
	char *pTmp		= strrchr(m_szCurDir, '\\');
	if (pTmp != NULL)
	{
		*(pTmp) = 0;
	}
	m_strToolsName = _T("CoursePlayer.exe");
	totalHeight = 0;
	maxHeight = 712;
	m_nLeftOrRight = 0;
}

CCourseToolsUI::~CCourseToolsUI(void)
{
}

LPCTSTR CCourseToolsUI::GetWindowClassName() const
{
	return _T("CourseToolsUI");
}

DuiLib::CDuiString CCourseToolsUI::GetSkinFile()
{
	return _T("CourseTools\\CourseTools.xml");
}

DuiLib::CDuiString CCourseToolsUI::GetSkinFolder()
{
	return _T("skins");
}

void CCourseToolsUI::InitWindow()
{
	::SetWindowLong(this->GetHWND(), GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_CONTROLPARENT);
	m_pContainer = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("container")));
}

void CCourseToolsUI::LoadTools()
{
	m_pContainer->RemoveAll();
	m_mapCourseTools.clear();
	//读取配置
	map<string,vector<string>> m_mapConfig;
	string strConfigPath = m_szCurDir;
	strConfigPath+= "\\Skins\\CourseTools\\Config.xml";
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(strConfigPath.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		TiXmlDocument doc;
		bool res = doc.LoadFile(strConfigPath.c_str(),TIXML_ENCODING_UTF8);
		if(res)
		{
			TiXmlElement* pRootElement = doc.FirstChildElement();
			if( pRootElement)
			{
				TiXmlElement* pGroupElement = pRootElement->FirstChildElement();
				if(pGroupElement)
				{
					do 
					{
						string strGroupNameTmp = pGroupElement->Attribute("name");
						string strGroupName=Utf82Str(strGroupNameTmp);
						vector<string> vFolders;
						TiXmlElement* pItem = pGroupElement->FirstChildElement();
						if(pItem)
						{
							do 
							{
								string strFolderTmp=pItem->GetText();
								string strFolder=Utf82Str(strFolderTmp);
								vFolders.push_back(strFolder);
							} while (pItem = dynamic_cast<TiXmlElement*>(pItem->NextSibling()));
						}
						m_mapConfig.insert(map<string,vector<string>> :: value_type(strGroupName,vFolders));
					} while (pGroupElement = dynamic_cast<TiXmlElement*>(pGroupElement->NextSibling()));
				}
			}
		}
	}
	//获取目录
	vector<string> lFolderList, lFileList;
	vector<string> lFolderNameList, lFolderImageList;
	char szImage[MAX_PATH]; 
	sprintf(szImage, "%s\\Package\\tools", GetLocalPath());
	GetToolsList(_T(szImage), lFolderList);//查询目录
	//添加配置工具
	for(map<string,vector<string>>::iterator iter = m_mapConfig.begin(); iter != m_mapConfig.end();iter++)
	{
		if(iter->second.size()>0)
		{
			vector<CCourseTool> vTools;
			if(iter->first.compare("常用工具")==0)
			{
				//添加计算器
				CCourseTool toolCal;
				toolCal.SetName("计算器");
				string strImagePath = m_szCurDir;
				strImagePath+="\\Skins\\floatctrl\\icon_jsq2.png";
				toolCal.SetImagePath(strImagePath);
				toolCal.SetActionPath("calc.exe");
				vTools.push_back(toolCal);
			}
			for (int i=0;i<iter->second.size();i++)
			{
				string strFolder=iter->second[i];
				string folderPath = GetLocalPath();
				folderPath+="\\Package\\tools\\"+strFolder;
				AddCourseToolsByFolderPath(folderPath,vTools);
			}
			if(vTools.size()>0)
			{
				m_mapCourseTools.insert(map<string,vector<CCourseTool>> :: value_type(iter->first,vTools));
			}
		}
	}
	for(int i =0; i < lFolderList.size(); i++)
	{
		bool isDeal = false;
		for(map<string,vector<string>>::iterator iter = m_mapConfig.begin(); iter != m_mapConfig.end();iter++)
		{
			for (int j=0;j<iter->second.size();j++)
			{
				string strFolder=iter->second[j];
				if(strFolder.compare(lFolderList[i])==0)
				{
					isDeal=true;
					break;
				}
			}
		}
		if(!isDeal)
		{
			vector<CCourseTool> vTools;
			string folderPath = GetLocalPath();
			folderPath+="\\Package\\tools\\"+lFolderList[i];
			AddCourseToolsByFolderPath(folderPath,vTools);
			m_mapCourseTools.insert(map<string,vector<CCourseTool>> :: value_type(lFolderList[i],vTools));
		}
	}
	//设置排序优先级
	string strSort[3] = {_T("常用工具"),_T("平面几何"),_T("立体几何")};
	//设置显示
	if(m_mapCourseTools.size()>0)
	{
		//先添加优先级高的项
		for(int i=0;i<3;i++)
		{
			if(m_mapCourseTools[strSort[i]].size()>0)
			{
				CVerticalLayoutUI* layoutGroup = CreateCourseToolsGroup(strSort[i],m_mapCourseTools[strSort[i]]);
				if(layoutGroup)
				{
					m_pContainer->Add(layoutGroup);
				}
			}
		}
		//添加其余项
		for(map<string,vector<CCourseTool>>::iterator iter = m_mapCourseTools.begin(); iter != m_mapCourseTools.end();iter++)
		{
			bool isOtherItem = true;
			for(int i=0;i<3;i++)
			{
				if(iter->first==strSort[i])
				{
					isOtherItem=false;
					break;
				}
			}
			if(isOtherItem)
			{
				if(iter->second.size()>0)
				{
					CVerticalLayoutUI* layoutGroup = CreateCourseToolsGroup(iter->first,iter->second);
					if(layoutGroup)
					{
						m_pContainer->Add(layoutGroup);
					}
				}
			}
		}
		//没有出现滚动条的情况下，尝试预留38px底部位置
		if(totalHeight+38<maxHeight)
		{
			::SetWindowPos(this->GetHWND(),NULL,-10000, -10000, 373,totalHeight+38,SWP_NOACTIVATE|SWP_SHOWWINDOW);
		}
	}
}

void CCourseToolsUI::AddCourseToolsByFolderPath(string folderPath,vector<CCourseTool>& vTools)
{
	vector<string> lFileList;
	GetToolsList(folderPath.c_str(), lFileList, 1);//查询文件
	for (int j = 0; j < lFileList.size(); j ++)
	{
		string filePath = folderPath+"\\"+lFileList[j];
		string name = filePath.substr(filePath.find_last_of("\\")+1,filePath.find_last_of(".")-filePath.find_last_of("\\")-1);
		string strImagePath = m_szCurDir;
		strImagePath+= "\\Skins\\CourseTools\\"+name+".png";
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(strImagePath.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			strImagePath = m_szCurDir;
			strImagePath+="\\Skins\\floatctrl\\icon_hdgj_auto_1.png";
		}
		CCourseTool tool;
		tool.SetName(name);
		tool.SetImagePath(strImagePath);
		tool.SetActionPath(filePath);
		vTools.push_back(tool);
	}
}

CVerticalLayoutUI* CCourseToolsUI::CreateCourseToolsGroup( string strGroupName,vector<CCourseTool>& tools)
{
	CVerticalLayoutUI* layoutGroup = NULL;
	if(tools.size()>0)
	{
		try
		{
			if( !m_builderGroup.GetMarkup()->IsValid() ) {
				layoutGroup = dynamic_cast<CVerticalLayoutUI*>(m_builderGroup.Create(_T("CourseTools\\CourseToolsGroup.xml"), (UINT)0, this, &m_PaintManager));
			}
			else {
				layoutGroup = dynamic_cast<CVerticalLayoutUI*>(m_builderGroup.Create(this, &m_PaintManager));
			}
			if(layoutGroup)
			{
				int lines = tools.size() / 5;
				int count = tools.size() % 5;
				if(count>0)
				{
					lines++;
				}
				//设置整个组的高度
				int groupHeight = 38+lines*70+lines-1;
				totalHeight += groupHeight;
				layoutGroup->SetFixedHeight(groupHeight);
				layoutGroup->SetFixedWidth(354);
				//设置组头
				CLabelUI* lblGroupName = static_cast<CLabelUI*>(layoutGroup->FindSubControl(_T("lblGroupName")));
				if(lblGroupName)
				{
					lblGroupName->SetText(strGroupName.c_str());
					lblGroupName->SetFont(160100);
				}
				//设置组内工具
				CVerticalLayoutUI*  layGroupTools = static_cast<CVerticalLayoutUI*>(layoutGroup->FindSubControl(_T("containerGroupTools")));
				if(layGroupTools)
				{
					for(int i=0;i<lines;i++)//添加每一行tool
					{
						CHorizontalLayoutUI* lineToolsLayout = new CHorizontalLayoutUI;
						for (int j=0;j<5;j++)
						{
							//添加Item
							CContainerUI* item = NULL;
							if( !m_builderGroupItem.GetMarkup()->IsValid() ) {
								item = dynamic_cast<CContainerUI*>(m_builderGroupItem.Create(_T("CourseTools\\CourseToolsGroupItem.xml"), (UINT)0, this, &m_PaintManager));
							}
							else {
								item = dynamic_cast<CContainerUI*>(m_builderGroupItem.Create(this, &m_PaintManager));
							}
							if(item)
							{
								item->OnNotify+=MakeDelegate(this,&CCourseToolsUI::OnToolItemClick);
								if(i*5+j<tools.size())//设置tag及界面图片文本显示
								{
									CCourseTool* toolPtr = &(tools[i*5+j]);
									item->SetTag((UINT_PTR)toolPtr);
									CControlUI* ctrGroupItemImage = static_cast<CControlUI*>(item->FindSubControl(_T("ctrGroupItemImage")));
									if(ctrGroupItemImage)
									{
										ctrGroupItemImage->SetBkImage(tools[i*5+j].GetImagePath().c_str());
									}
									CLabelUI* lblGroupItemName = static_cast<CLabelUI*>(item->FindSubControl(_T("lblGroupItemName")));
									if(lblGroupItemName)
									{
										lblGroupItemName->SetText(tools[i*5+j].GetName().c_str());
										lblGroupItemName->SetFont(120000);
										lblGroupItemName->SetTextColor(0x99FFFFFF);
									}
								}
								lineToolsLayout->Add(item);
							}
							//如果不是最后一列，则添加一条分割竖线
							if(j<4)
							{
								CControlUI* ctrVerticalLine = new CControlUI;
								ctrVerticalLine->SetBkColor(0xFF545454);
								ctrVerticalLine->SetFixedWidth(1);
								ctrVerticalLine->SetFixedHeight(70);
								lineToolsLayout->Add(ctrVerticalLine);
							}
						}
						layGroupTools->Add(lineToolsLayout);
						//如果不是最后一行，则添加一条分割横线
						if(i<lines-1)
						{
							CControlUI* ctrHorizontalLine = new CControlUI;
							ctrHorizontalLine->SetBkColor(0xFF545454);
							ctrHorizontalLine->SetFixedHeight(1);
							layGroupTools->Add(ctrHorizontalLine);
						}
					}
				}
			}
		}
		catch (...)
		{
			layoutGroup = NULL;
		}
	}
	return layoutGroup;
}

BOOL CCourseToolsUI::GetToolsList(LPCTSTR lpszPath, vector<string>& arrFileNames, int pSearchType)
{
	arrFileNames.clear();
	if (lpszPath && *lpszPath != '\0')
	{
		string sPath;
		string sFindFile;
		sPath = lpszPath;
		if (lpszPath[sPath.length() - 1] != '\\')
		{
			if (pSearchType ==  0)
			{
				sPath += "\\*";
			}
			else if(pSearchType == 1)
			{
				sPath += "\\*.html";
			}
		}
		sFindFile = sPath;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(sFindFile.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		do 
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if( (strcmp(FindFileData.cFileName,".") != 0 ) &&(strcmp(FindFileData.cFileName,"..") != 0))   
				{
					if(pSearchType == 0)
					{
						arrFileNames.push_back(FindFileData.cFileName);
					}
				} 
				continue;
			}
			else
			{
				if (pSearchType == 1)
				{ 
					arrFileNames.push_back(FindFileData.cFileName);					 
				}
			}

		} while (::FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	return true;
}

void CCourseToolsUI::KillOsrWindows()
{
	KillExeCheckParentPidNotPlayer(_T(m_strToolsName.c_str()), TRUE);//隐藏时关闭进程
}

bool CCourseToolsUI::OnToolItemClick( void* pNotify )
{
	TNotifyUI* msg = (TNotifyUI*)pNotify;
	if (msg&&msg->sType == _T("setfocus"))
	{
		if(msg->pSender!=NULL && msg->pSender->GetTag()!=NULL)
		{
			CCourseTool* tool = (CCourseTool*)(msg->pSender->GetTag());
			if(tool)
			{
				try
				{
					string strActionPath = tool->GetActionPath();
					if(!strActionPath.empty())
					{
						KillExeCheckParentPidNotPlayer(_T(m_strToolsName.c_str()), TRUE);//先把原先打开的关闭
						if(strActionPath.compare(_T("calc.exe"))==0)//计算器
						{
							char szCalcPath[MAX_PATH]; 
							sprintf(szCalcPath, "calc.exe");
							KillExeCheckParentPid(_T("CALC.EXE"), TRUE);
							//ShellExecute(NULL,"open","calc.exe","", "", SW_SHOWNORMAL );
							PROCESS_INFORMATION pi;
							STARTUPINFO si;
							memset( &si, 0, sizeof(STARTUPINFO) );
							si.cb = sizeof(STARTUPINFO);  
							if (CreateProcess(NULL, _T(szCalcPath), NULL, NULL, FALSE, 0, NULL, m_szCurDir, &si, &pi ))
							{
								Sleep(200);
								HWND hWnd = FindWindow(_T("CalcFrame"), NULL);
								if( hWnd )
								{
									RECT rect = {0};
									::GetWindowRect(this->GetHWND(),&rect);
									int width = rect.right - rect.left;
									RECT rectCal = {0};
									::GetWindowRect(hWnd,&rectCal);
									int calWidth = rectCal.right-rectCal.left;
									int calHeight = rectCal.bottom - rectCal.top;
									if(m_nLeftOrRight==1)//左边显示
									{
										::SetWindowPos(hWnd,NULL,rect.left, rect.top, calWidth,calHeight,SWP_NOACTIVATE|SWP_SHOWWINDOW);
									}
									else if(m_nLeftOrRight==2)//右边显示
									{
										::SetWindowPos(hWnd,NULL,rect.left+width-calWidth, rect.top, calWidth,calHeight,SWP_NOACTIVATE|SWP_SHOWWINDOW);
									}
								}
							}
							Statistics::GetInstance()->Report(STAT_EXEC_CALC);
						}
						else
						{
							HWND nPPTWnd = CPPTController::GetSlideShowViewHwnd();
							char szImage[MAX_PATH*2]; 
							sprintf(szImage, "%s\\bin\\CoursePlayer\\%s OSR \"%s\" %d", m_szCurDir, m_strToolsName.c_str(), strActionPath.c_str(),nPPTWnd);
							PROCESS_INFORMATION pi;
							STARTUPINFO si;
							memset( &si, 0, sizeof(STARTUPINFO) );
							si.cb = sizeof(STARTUPINFO);  
							if ( !CreateProcess(NULL, _T(szImage), NULL, NULL, FALSE, 0, NULL, m_szCurDir, &si, &pi ) )
							{ 
							} 
						}
					}
				}
				catch(...)
				{

				}
			}
			this->m_pContainer->SetFocus();
			this->ShowWindow(false);
		}
	}
	return true;
}

void CCourseToolsUI::SetLeftOrRight( int nLeftOrRight )
{
	m_nLeftOrRight=nLeftOrRight;
}
