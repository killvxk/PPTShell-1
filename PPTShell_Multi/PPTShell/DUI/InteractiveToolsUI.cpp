#include "StdAfx.h"
#include "InteractiveToolsUI.h"
#include "Resource.h"
#include "Plugins/Icr/IcrPlayer.h"

CInteractiveToolsUI::CInteractiveToolsUI(void):m_pContainer(NULL)
{
	GetModuleFileName(NULL,m_szCurDir,MAX_PATH);
	char *pTmp		= strrchr(m_szCurDir, '\\');
	if (pTmp != NULL)
	{
		*(pTmp) = 0;
	}
	maxWidth = 373;
	maxHeight = 712;
}

CInteractiveToolsUI::~CInteractiveToolsUI(void)
{
}

LPCTSTR CInteractiveToolsUI::GetWindowClassName() const
{
	return _T("InteractiveToolsUI");
}

DuiLib::CDuiString CInteractiveToolsUI::GetSkinFile()
{
	return _T("InteractiveTools\\InteractiveTools.xml");
}

DuiLib::CDuiString CInteractiveToolsUI::GetSkinFolder()
{
	return _T("skins");
}

void CInteractiveToolsUI::InitWindow()
{
	::SetWindowLong(this->GetHWND(), GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_CONTROLPARENT);
	m_pContainer = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("container")));
}

void CInteractiveToolsUI::LoadTools()
{
	totalHeight = 0;
	totalWidth = 0;
	m_pContainer->RemoveAll();
	m_mapInteractiveTools.clear();
	//放大镜
	vector<CInteractiveTool> vTools;
	CInteractiveTool toolMagnifier;
	toolMagnifier.SetName("放大镜");
	string imagePath = m_szCurDir;
	imagePath+="\\Skins\\InteractiveTools\\hdgj_magnifier.png";
	toolMagnifier.SetImagePath(imagePath);
	toolMagnifier.SetType(Magnifier);
	vTools.push_back(toolMagnifier);
	m_mapInteractiveTools.insert(map<string,vector<CInteractiveTool>> :: value_type("工具",vTools));
	//课堂工具
	vector<CInteractiveTool> vCourseTools;
	//花名册
	CInteractiveTool toolRoster;
	toolRoster.SetName("花名册");
	imagePath = m_szCurDir;
	imagePath+="\\Skins\\InteractiveTools\\hdgj_hmc.png";
	toolRoster.SetImagePath(imagePath);
	toolRoster.SetType(Roster);
	vCourseTools.push_back(toolRoster);
	//推屏
	CInteractiveTool toolPushScreen;
	toolPushScreen.SetName("推屏");
	imagePath = m_szCurDir;
	imagePath+="\\Skins\\InteractiveTools\\hdgj_tp.png";
	toolPushScreen.SetImagePath(imagePath);
	toolPushScreen.SetType(PushScreen);
	vCourseTools.push_back(toolPushScreen);
	//随堂练习
	CInteractiveTool toolCourseTraining;
	toolCourseTraining.SetName("随堂练习");
	imagePath = m_szCurDir;
	imagePath+="\\Skins\\InteractiveTools\\hdgj_fsxt.png";
	toolCourseTraining.SetImagePath(imagePath);
	toolCourseTraining.SetType(CourseTraining);
	vCourseTools.push_back(toolCourseTraining);
	//课堂总结
	CInteractiveTool toolCourseSummary;
	toolCourseSummary.SetName("课堂总结");
	imagePath = m_szCurDir;
	imagePath+="\\Skins\\InteractiveTools\\hdgj_ktzj.png";
	toolCourseSummary.SetImagePath(imagePath);
	toolCourseSummary.SetType(CourseSummary);
	vCourseTools.push_back(toolCourseSummary);
	m_mapInteractiveTools.insert(map<string,vector<CInteractiveTool>> :: value_type("互动课堂",vCourseTools));
	//显示设置
	int itemCountPerRow = 4;//每行个数
	for(map<string,vector<CInteractiveTool>>::iterator iter = m_mapInteractiveTools.begin(); iter != m_mapInteractiveTools.end();iter++)
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
	if(totalHeight+38<maxHeight)
	{
		totalHeight+=38;
	}
	m_pContainer->SetFixedHeight(totalHeight);
	m_pContainer->SetFixedWidth(totalWidth);
	//
	if(totalHeight>=maxHeight)
	{
		::SetWindowPos(this->GetHWND(),NULL,-10000, -10000, maxWidth,maxHeight,SWP_NOACTIVATE|SWP_SHOWWINDOW);
	}
	else
	{
		::SetWindowPos(this->GetHWND(),NULL,-10000, -10000, totalWidth,totalHeight,SWP_NOACTIVATE|SWP_SHOWWINDOW);
	}
}

CVerticalLayoutUI* CInteractiveToolsUI::CreateCourseToolsGroup( string strGroupName,vector<CInteractiveTool>& tools )
{
	CVerticalLayoutUI* layoutGroup = NULL;
	if(tools.size()>0)
	{
		try
		{
			if( !m_builderGroup.GetMarkup()->IsValid() ) {
				layoutGroup = dynamic_cast<CVerticalLayoutUI*>(m_builderGroup.Create(_T("InteractiveTools\\InteractiveToolsGroup.xml"), (UINT)0, this, &m_PaintManager));
			}
			else {
				layoutGroup = dynamic_cast<CVerticalLayoutUI*>(m_builderGroup.Create(this, &m_PaintManager));
			}
			if(layoutGroup)
			{
				int itemCountPerRow = 4;//每行个数
				int lines = tools.size() / itemCountPerRow;
				int count = tools.size() % itemCountPerRow;
				if(count>0)
				{
					lines++;
				}
				//设置整个组的高度
				int groupHeight = 38+lines*70+lines-1;
				totalHeight += groupHeight;
				layoutGroup->SetFixedHeight(groupHeight);
				layoutGroup->SetFixedWidth(itemCountPerRow*70+itemCountPerRow-1);
				totalWidth = itemCountPerRow*70+itemCountPerRow-1;
				//设置组头
				CLabelUI* lblGroupName = static_cast<CLabelUI*>(layoutGroup->FindSubControl(_T("lblGroupName")));
				if(lblGroupName)
				{
					lblGroupName->SetText(strGroupName.c_str());
					lblGroupName->SetFont(160100);
				}
				//设置组内工具
				CVerticalLayoutUI* layGroupTools = static_cast<CVerticalLayoutUI*>(layoutGroup->FindSubControl(_T("containerGroupTools")));
				if(layGroupTools)
				{
					for(int i=0;i<lines;i++)//添加每一行tool
					{
						CHorizontalLayoutUI* lineToolsLayout = new CHorizontalLayoutUI;
						for (int j=0;j<itemCountPerRow;j++)
						{
							//添加Item
							CContainerUI* item = NULL;
							if( !m_builderItem.GetMarkup()->IsValid() ) {
								item = dynamic_cast<CContainerUI*>(m_builderItem.Create(_T("InteractiveTools\\InteractiveToolsItem.xml"), (UINT)0, this, &m_PaintManager));
							}
							else {
								item = dynamic_cast<CContainerUI*>(m_builderItem.Create(this, &m_PaintManager));
							}
							if(item)
							{
								item->OnNotify+=MakeDelegate(this,&CInteractiveToolsUI::OnToolItemClick);
								if(i*itemCountPerRow+j<tools.size())//设置tag及界面图片文本显示
								{
									CInteractiveTool* toolPtr = &(tools[i*itemCountPerRow+j]);
									item->SetTag((UINT_PTR)toolPtr);
									CControlUI* ctrGroupItemImage = static_cast<CControlUI*>(item->FindSubControl(_T("ctrItemImage")));
									if(ctrGroupItemImage)
									{
										ctrGroupItemImage->SetBkImage(tools[i*itemCountPerRow+j].GetImagePath().c_str());
									}
									CLabelUI* lblGroupItemName = static_cast<CLabelUI*>(item->FindSubControl(_T("lblItemName")));
									if(lblGroupItemName)
									{
										lblGroupItemName->SetText(tools[i*itemCountPerRow+j].GetName().c_str());
										lblGroupItemName->SetFont(120000);
										lblGroupItemName->SetTextColor(0x99FFFFFF);
									}
								}
								lineToolsLayout->Add(item);
							}
							//如果不是最后一列，则添加一条分割竖线
							if(j<itemCountPerRow-1)
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

bool CInteractiveToolsUI::OnToolItemClick( void* pNotify )
{
	TNotifyUI* msg = (TNotifyUI*)pNotify;
	if (msg&&msg->sType == _T("setfocus"))
	{
		if(msg->pSender!=NULL && msg->pSender->GetTag()!=NULL)
		{
			CInteractiveTool* tool = (CInteractiveTool*)(msg->pSender->GetTag());
			if(tool)
			{
				try
				{
					if(tool->GetType()==Magnifier)//放大镜
					{
	
					}
					else if (tool->GetType()==Roster)//花名册
					{
						IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_ROSTER);
					}
					else if (tool->GetType()==PushScreen)//推屏
					{
						IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_PUSHSCREEN);
					}
					else if (tool->GetType()==CourseTraining)//随堂练习
					{
						IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_EXAM);
					}
					else if (tool->GetType()==CourseSummary)//课堂总结
					{
						IcrPlayer::GetInstance()->IcrOnClick(ICR_BUTTON_ID_SUMMARY);
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
