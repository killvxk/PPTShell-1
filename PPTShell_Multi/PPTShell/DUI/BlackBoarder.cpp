#include "StdAfx.h"
#include "Util/Stream.h"
#include "BlackBoarder.h"
#include "PanelView.h"
#include "PPTControl/PPTController.h"
#include "Util/Util.h"
#include "DUI/Shape.h"
CBlackBoarderUI*  CBlackBoarderUI::m_pInstance = NULL;

CBlackBoarderUI::CBlackBoarderUI(void)
{
	m_pTabPageContainer = NULL;
	m_pPageTotalCount = NULL;
	m_pPageCurrentIndex = NULL;
	m_pBtnPagePrevious = NULL;
	m_pBtnPageNext = NULL;
	isShown = false;
	//设置默认颜色
	dwCurrentColor = RGB(234,55,68);
	dwCurrentColor  &= 0x00FFFFFF;
	if(dwCurrentColor == 0)
	{
		dwCurrentColor = 0x1;
	}
	dwCurrentColor = RGB(GetBValue(dwCurrentColor), GetGValue(dwCurrentColor), GetRValue(dwCurrentColor));
	//设置penview默认类型
	m_nCurrentPenViewType = ePoiner_Pen;
}

CBlackBoarderUI::~CBlackBoarderUI(void)
{
}

CBlackBoarderUI* CBlackBoarderUI::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CBlackBoarderUI;
	}
	return m_pInstance;
}

void CBlackBoarderUI::ReleaseInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
	}
	m_pInstance = NULL;
}


DuiLib::CDuiString CBlackBoarderUI::GetSkinFolder()
{
	return _T("skins");
}

DuiLib::CDuiString CBlackBoarderUI::GetSkinFile()
{
	return _T("ScreenTool\\BlackBoarder.xml");
}

LPCTSTR CBlackBoarderUI::GetWindowClassName( void ) const
{
	return _T("CBlackBoarderUI");
}

CControlUI* CBlackBoarderUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("PanelView"), pstrClass) == 0)
	{
		return new CPanelViewUI;
	}
	return NULL;
}

void CBlackBoarderUI::InitWindow()
{
	__super::InitWindow();
	m_pLayoutBG = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("layoutBG"));
	m_pTabPageContainer = (CTabLayoutUI*)m_PaintManager.FindControl(_T("tabPageContainer"));
	m_pPageCurrentIndex = (CLabelUI*)m_PaintManager.FindControl(_T("lblPageCurrentIndex"));
	m_pPageTotalCount = (CLabelUI*)m_PaintManager.FindControl(_T("lblPageTotalCount"));
	m_pBtnPagePrevious = (CButtonUI*)m_PaintManager.FindControl(_T("btnPagePrevious"));
	m_pBtnPageNext = (CButtonUI*)m_PaintManager.FindControl(_T("btnPageNext"));
	if(m_pLayoutBG)
	{
		int fixHeight = m_pLayoutBG->GetFixedHeight();
		int fixWidth = m_pLayoutBG->GetFixedWidth();
	}
}

void CBlackBoarderUI::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	ReleaseInstance();
}

#pragma region 黑板显示关闭相关
LRESULT CBlackBoarderUI::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(wParam == VK_ESCAPE)
	{
		CloseBlackBoard();
		return FALSE;
	}
	return __super::OnKeyDown(uMsg, wParam, lParam, bHandled);
}

void CBlackBoarderUI::ShowBlackBoard()
{
	if (!GetHWND() || !::IsWindow(GetHWND()))
	{
		Create(CPPTController::GetSlideShowViewHwnd(), GetWindowClassName(), WS_POPUP | WS_VISIBLE, 0);
		MONITORINFO oMonitor = {0};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(this->GetHWND(), MONITOR_DEFAULTTONEAREST), &oMonitor);
		m_nScreenWidth = oMonitor.rcMonitor.right - oMonitor.rcMonitor.left;
		m_nScreenHeight	= oMonitor.rcMonitor.bottom - oMonitor.rcMonitor.top;
		::MoveWindow(m_hWnd, 0,0, m_nScreenWidth, m_nScreenHeight, false);
	}
	::SetWindowPos(GetHWND(), HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	//
	if(m_pLayoutBG)
	{
		int fixHeight = m_pLayoutBG->GetHeight();
		int fixWidth = m_pLayoutBG->GetFixedWidth();
	}
	LoadBlackboard();
	//加载后如果没有数据则插入view
	if(m_pTabPageContainer)
	{
		int count = m_pTabPageContainer->GetCount();
		if(count==0)
		{
			CPanelViewUI* pPanelView = new CPanelViewUI;
			pPanelView->SetReferenceBackground(true);
			pPanelView->SetShapeColor(dwCurrentColor);
			pPanelView->SetPointer(m_nCurrentPenViewType);
			m_pTabPageContainer->Add(pPanelView);
			m_pTabPageContainer->SelectItem(pPanelView);
			//设置页数显示
			SetCurrentIndex(m_pTabPageContainer->GetCount());
			SetTotalCount(m_pTabPageContainer->GetCount());
			//设置按钮状态
			FreshBtnPageActionState();
		}
	}
	isShown = true;
	BroadcastEvent(EVT_SCREEN_PRE_NEXT_ENABLE,(WPARAM)false,0,NULL);
	BroadcastEvent(EVT_BLACKBROAD_COLOR,0,0,NULL);
}

bool CBlackBoarderUI::GetIsShown()
{
	return isShown;
}

void CBlackBoarderUI::CloseBlackBoard()
{
	//保存黑板内容
	SaveBlackboard();
	this->Close();
	//
	BroadcastEvent(EVT_SCREEN_PRE_NEXT_ENABLE,(WPARAM)true,0,NULL);
}

void CBlackBoarderUI::OnBtnCloseClick( TNotifyUI& msg )
{
	CloseBlackBoard();
}

void CBlackBoarderUI::CloseBlackBoardForce()
{
	SaveBlackboard();
	::DestroyWindow(this->GetHWND());
}
#pragma endregion

void CBlackBoarderUI::OnBtnPageAppendClick( TNotifyUI& msg )
{	
	if(m_pTabPageContainer)
	{
		CPanelViewUI* pPanelView = new CPanelViewUI;
		pPanelView->SetReferenceBackground(true);
		pPanelView->SetShapeColor(dwCurrentColor);
		pPanelView->SetPointer(m_nCurrentPenViewType);
		int currentIndex = m_pTabPageContainer->GetCurSel();
		if(currentIndex>=0)
		{
			m_pTabPageContainer->AddAt(pPanelView,currentIndex+1);
		}
		else
		{
			m_pTabPageContainer->Add(pPanelView);
		}
		m_pTabPageContainer->SelectItem(pPanelView);
		//设置页数显示
		SetCurrentIndex(m_pTabPageContainer->GetCurSel()+1);
		SetTotalCount(m_pTabPageContainer->GetCount());
		//设置按钮状态
		FreshBtnPageActionState();
	}
}

void CBlackBoarderUI::OnBtnPageRemoveClick( TNotifyUI& msg )
{
	if(m_pTabPageContainer)
	{
		int currentIndex = m_pTabPageContainer->GetCurSel();
		if(currentIndex>=0)
		{
			m_pTabPageContainer->RemoveAt(currentIndex);
			int count = m_pTabPageContainer->GetCount();
			if(count>0)
			{
				if(currentIndex>=count-1)
				{
					currentIndex = count-1;
				}
				m_pTabPageContainer->SelectItem(currentIndex);
			}
			else//
			{
				CPanelViewUI* pPanelView = new CPanelViewUI;
				pPanelView->SetReferenceBackground(true);
				pPanelView->SetShapeColor(dwCurrentColor);
				pPanelView->SetPointer(m_nCurrentPenViewType);
				m_pTabPageContainer->Add(pPanelView);
				m_pTabPageContainer->SelectItem(pPanelView);
				currentIndex = m_pTabPageContainer->GetCurSel();
				count = m_pTabPageContainer->GetCount();
			}
			//设置页数显示
			SetCurrentIndex(m_pTabPageContainer->GetCurSel()+1);
			SetTotalCount(count);
			//设置按钮状态
			FreshBtnPageActionState();
		}
	}
}

void CBlackBoarderUI::OnBtnPagPreviousClick( TNotifyUI& msg )
{
	if(m_pTabPageContainer)
	{
		int currentIndex = m_pTabPageContainer->GetCurSel();
		if(currentIndex>0)
		{
			currentIndex--;
			m_pTabPageContainer->SelectItem(currentIndex);
			SetCurrentIndex(m_pTabPageContainer->GetCurSel()+1);
			//设置按钮状态
			FreshBtnPageActionState();
		}
	}
}

void CBlackBoarderUI::OnBtnPageNextClick( TNotifyUI& msg )
{
	if(m_pTabPageContainer)
	{
		int currentIndex = m_pTabPageContainer->GetCurSel();
		int count = m_pTabPageContainer->GetCount();
		if(currentIndex<count-1)
		{
			currentIndex++;
			m_pTabPageContainer->SelectItem(currentIndex);
			SetCurrentIndex(m_pTabPageContainer->GetCurSel()+1);
			//设置按钮状态
			FreshBtnPageActionState();
		}
	}
}

void CBlackBoarderUI::SetCurrentIndex( int index )
{
	if(m_pPageCurrentIndex)
	{
		TCHAR szIndex[MAX_PATH];
		_stprintf(szIndex,"%d",index);
		m_pPageCurrentIndex->SetText(szIndex);
	}
}

void CBlackBoarderUI::SetTotalCount(int count)
{
	if(m_pPageTotalCount)
	{
		TCHAR szCount[MAX_PATH];
		_stprintf(szCount,"%d",count);
		m_pPageTotalCount->SetText(szCount);
	}
}

void CBlackBoarderUI::FreshBtnPageActionState()
{
	if(m_pTabPageContainer)
	{
		int currentIndex = m_pTabPageContainer->GetCurSel();
		if(currentIndex>0)
		{
			SetBtnPagePreviousEnable(true);
		}
		else
		{
			SetBtnPagePreviousEnable(false);
		}
		int count = m_pTabPageContainer->GetCount();
		if(currentIndex<count-1)
		{
			SetBtnPageNextEnable(true);
		}
		else
		{
			SetBtnPageNextEnable(false);
		}
	}
}

void CBlackBoarderUI::SetBtnPagePreviousEnable( bool enable )
{
	if(m_pBtnPagePrevious)
	{
		m_pBtnPagePrevious->SetEnabled(enable);
		if(enable)
		{
			m_pBtnPagePrevious->SetNormalImage("ScreenTool\\blackboard\\btn_pre_page_normal.png");
			m_pBtnPagePrevious->SetPushedImage("ScreenTool\\blackboard\\btn_pre_page_press.png");
		}
		else
		{
			m_pBtnPagePrevious->SetNormalImage("ScreenTool\\blackboard\\btn_pre_page_unenable.png");
			m_pBtnPagePrevious->SetPushedImage("");
		}
	}
}

void CBlackBoarderUI::SetBtnPageNextEnable( bool enable )
{
	if(m_pBtnPageNext)
	{
		m_pBtnPageNext->SetEnabled(enable);
		if(enable)
		{
			m_pBtnPageNext->SetNormalImage("ScreenTool\\blackboard\\btn_next_page_normal.png");
			m_pBtnPageNext->SetPushedImage("ScreenTool\\blackboard\\btn_next_page_press.png");
		}
		else
		{
			m_pBtnPageNext->SetNormalImage("ScreenTool\\blackboard\\btn_next_page_unenable.png");
			m_pBtnPageNext->SetPushedImage("");
		}
	}
}

void CBlackBoarderUI::SetPenColor( DWORD dwColor )
{
	dwColor  &= 0x00FFFFFF;
	if(dwColor == 0)
	{
		dwColor = 0x1;
	}
	dwColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
	dwCurrentColor = dwColor;
	SetPenType(ePoiner_Pen);
	if(m_pTabPageContainer)
	{
		for(int i=0;i<m_pTabPageContainer->GetCount();i++)
		{
			CPanelViewUI* pPenelView= dynamic_cast<CPanelViewUI*>(m_pTabPageContainer->GetItemAt(i));
			if(pPenelView)
			{
				pPenelView->SetShapeColor(dwColor);
			}
		}
	}
}

DWORD CBlackBoarderUI::GetPenColor()
{
	return  RGB(GetRValue(dwCurrentColor), GetGValue(dwCurrentColor), GetBValue(dwCurrentColor));
}

void CBlackBoarderUI::SetPenType( int type )
{
	m_nCurrentPenViewType = type;
	if(m_pTabPageContainer)
	{
		for(int i=0;i<m_pTabPageContainer->GetCount();i++)
		{
			CPanelViewUI* pPenelView= dynamic_cast<CPanelViewUI*>(m_pTabPageContainer->GetItemAt(i));
			if(pPenelView)
			{
				pPenelView->SetPointer(m_nCurrentPenViewType);
			}
		}
	}
}

void CBlackBoarderUI::ClearPanel()
{
	if(m_pTabPageContainer)
	{
		int currentIndex = m_pTabPageContainer->GetCurSel();
		if(currentIndex>=0)
		{
			CPanelViewUI* pPenelView= dynamic_cast<CPanelViewUI*>(m_pTabPageContainer->GetItemAt(currentIndex));
			if(pPenelView)
			{
				pPenelView->ClearShapes();
			}
		}
	}
}

#pragma region 数据加载保存相关
void CBlackBoarderUI::LoadBlackboard()
{
	if(!m_pTabPageContainer)
	{
		return;
	}

	tstring strConfigPath = GetLocalPath();
	strConfigPath+="\\Setting\\Blackboard.dat";
	CStream* pStream = NULL;
	if(!ReadFile(strConfigPath.c_str(), &pStream))
	{
		return;
	}

	int nTotalCount		= pStream->ReadInt();
	int nCurrentIndex	= pStream->ReadInt();

	for (int i = 0; i < nTotalCount; i++)
	{
		//添加UI
		CPanelViewUI* pPanelView = new CPanelViewUI;
		pPanelView->SetReferenceBackground(true);
		pPanelView->SetShapeColor(dwCurrentColor);
		pPanelView->SetPointer(m_nCurrentPenViewType);

		vector<IShape*> vShapes;
		int nShapeCount	= pStream->ReadInt();
		for (int j = 0; j < nShapeCount; ++j)
		{
			IShape*	pShape = NULL;
			int nShapeType = pStream->ReadInt();
			if (nShapeType == eShape_Line)
			{
				pShape = new CLineShape;
			}
			else if (nShapeType == eShape_Arc)
			{
				pShape = new CArcShape;
			}
			pShape->ReadFrom(pStream, m_nScreenWidth, m_nScreenHeight);

			vShapes.push_back(pShape);

		}
		
		pPanelView->SetShapes(&vShapes);
		m_pTabPageContainer->Add(pPanelView);
	}

	delete pStream;

	if(nCurrentIndex >= 0 
		&& nCurrentIndex < m_pTabPageContainer->GetCount())
	{
		m_pTabPageContainer->SelectItem(nCurrentIndex);
	}

	//设置页数显示
	SetCurrentIndex(m_pTabPageContainer->GetCurSel()+1);
	SetTotalCount(m_pTabPageContainer->GetCount());
	//设置按钮状态
	FreshBtnPageActionState();

}

void CBlackBoarderUI::SaveBlackboard()
{
	if(!m_pTabPageContainer
		|| m_pTabPageContainer->GetCount() < 0)
	{
		return;
	}

	CStream stream(1024);
	int nCurrentIndex	= m_pTabPageContainer->GetCurSel();
	int nTotalCount		= m_pTabPageContainer->GetCount();

	stream.WriteDWORD(nTotalCount);
	stream.WriteDWORD(nCurrentIndex);

	for (int i = 0;i < nTotalCount; i++)
	{
		CPanelViewUI* pPenelView = dynamic_cast<CPanelViewUI*>(m_pTabPageContainer->GetItemAt(i));
		if(!pPenelView)
		{
			continue;
		}

		vector<IShape*>* vShapes = pPenelView->GetShapes();
		stream.WriteDWORD(vShapes->size());
		for(int j = 0; j < (int)vShapes->size(); ++j)
		{
			IShape* pShape = vShapes->at(j);

			stream.WriteInt(pShape->GetShapeType());
			pShape->WriteTo(&stream, m_nScreenWidth, m_nScreenHeight);
		}

	}

	string strConfigPath = GetLocalPath();
	strConfigPath+="\\Setting\\Blackboard.dat";
	WriteFile(strConfigPath.c_str(), &stream);
}

// bool CBlackBoarderUI::ReadFile( string strFilePath,string& strContent )
// {
// 	bool result = false;
// 	WIN32_FIND_DATA FindFileData;
// 	HANDLE hFind = ::FindFirstFile(strFilePath.c_str(), &FindFileData);
// 	if (hFind != INVALID_HANDLE_VALUE)
// 	{
// 		FILE* fp = NULL;
// 		int err = _tfopen_s(&fp, strFilePath.c_str(), "rb");
// 		if( err == 0 )
// 		{
// 			fseek(fp, 0, SEEK_END);
// 			int nSize = ftell(fp);
// 			fseek(fp, 0, SEEK_SET);
// 			if( nSize >= 0 )
// 			{
// 				char* pBuffer = new char[nSize];
// 				if( pBuffer != NULL )
// 				{
// 					fread(pBuffer, nSize, 1, fp);
// 					strContent = pBuffer;
// 					result = true;
// 				}
// 			}
// 		}
// 		if(fp)
// 		{
// 			fclose(fp);
// 		}
// 	}
// 	return result;
// }

bool CBlackBoarderUI::ReadFile( LPCTSTR lptcsFilePath, CStream** pStream)
{
	FILE* fp = NULL;
	_tfopen_s(&fp, lptcsFilePath, _T("rb"));
	if(!fp)
	{
		remove(lptcsFilePath);
		return false;
	}

	fseek(fp, 0,  SEEK_END);
	int nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char* pBuff = new unsigned char[nSize];
	fread(pBuff, nSize, 1, fp);
	fclose(fp);

	DWORD dwCrc = crc_32(pBuff + 4, nSize - 4);

	if (dwCrc != *(DWORD*)(pBuff))
	{
		delete pBuff;
		remove(lptcsFilePath);
		return false;
	}

	*pStream = new CStream((char*)(pBuff + 4), nSize - 4);
	return true;
}

bool CBlackBoarderUI::WriteFile( LPCTSTR lptcsFilePath, CStream* pStream)
{

	FILE* fp = NULL;
	_tfopen_s(&fp, lptcsFilePath, _T("wb"));
	if(!fp)
	{
		return false;
	}

	unsigned char*	pData	= (unsigned char*)pStream->GetBuffer();
	unsigned int	nLen	= pStream->GetDataSize();

	DWORD dwCrc = crc_32(pData, nLen);

	//crc
	fwrite(&dwCrc, sizeof(dwCrc), 1, fp);
	//stream
	fwrite(pData, nLen, 1, fp);

	fclose(fp);
	return true;
}

void CBlackBoarderUI::ConvertPointToPercent( int x,int y,float& xPercent,float& yPercent )
{
	xPercent=(float)x/m_nScreenWidth;
	yPercent=(float)y/m_nScreenHeight;
}

void CBlackBoarderUI::ConvertPercentToPoint( float xPercent,float yPercent,int& x,int& y )
{
	x=m_nScreenWidth*xPercent;
	y=m_nScreenHeight*yPercent;
}

#pragma endregion