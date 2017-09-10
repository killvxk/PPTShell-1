#include "StdAfx.h"
#include "QuestionDialogUI.h"


CQuestionDialogUI::CQuestionDialogUI(void)
{
}

CQuestionDialogUI::~CQuestionDialogUI(void)
{
}

LPCTSTR CQuestionDialogUI::GetWindowClassName( void ) const
{
	return _T("QuestionDialog");
}

DuiLib::CDuiString CQuestionDialogUI::GetSkinFile()
{
	return _T("Question\\Question.xml");
}

DuiLib::CDuiString CQuestionDialogUI::GetSkinFolder()
{
	return _T("skins");
}


void CQuestionDialogUI::CreateDlg()
{
	HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	Create(hwnd, this->GetWindowClassName(), WS_POPUP, 0);
	CRect rect;
	::GetWindowRect(hwnd, &rect);
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
	//CenterWindow();

}

void CQuestionDialogUI::InitWindow()
{
//	m_WndShadow.Create(this->GetHWND(), AfxGetApp()->m_pMainWnd->GetSafeHwnd());

	m_layBaseList			= dynamic_cast<CTileLayoutUI*>(m_PaintManager.FindControl(_T("base_list")));
	m_layInteractionList	= dynamic_cast<CTileLayoutUI*>(m_PaintManager.FindControl(_T("interaction_list")));

	CreateBaseList();

	CreateInteractionList();

}


void CQuestionDialogUI::CreateBaseList()
{
	TCHAR szBaseTypeName[][MAX_PATH / 4 ] ={
		_T("choice"),
		_T("multiplechoice"),
		_T("textentry"),
		_T("graphicgapmatch"),
		_T("match"),
		_T("order"),
		_T("judge"),
		_T("drawing"),
		_T("handwrite"),
		_T("data"),
		_T("vote"),
	};

	TCHAR	szIconReleativeDir[]	= _T("Question\\Base\\");
	TCHAR	szImageFormat[]			= _T("file='%s%s.jpg' source='%d,%d,%d,%d'");
	TCHAR	szBuffer[MAX_PATH]		= {0};

	for (int i = 0; i < _countof(szBaseTypeName); ++i)
	{
		
		CButtonUI*	pBtn	= new CButtonUI;
		pBtn->SetName(_T("item"));
		pBtn->SetFixedHeight(140);
		pBtn->SetFixedWidth(140);
		pBtn->SetUserData(szBaseTypeName[i]);
		pBtn->SetTag(1);

		memset(szBuffer, 0, sizeof(szBuffer));
		_stprintf_s(szBuffer, szImageFormat, szIconReleativeDir, szBaseTypeName[i], 0, 0, 140, 140);
		pBtn->SetAttribute(_T("bkimage"), szBuffer);


		memset(szBuffer, 0, sizeof(szBuffer));
		_stprintf_s(szBuffer, szImageFormat, szIconReleativeDir, szBaseTypeName[i], 0, 140, 140, 280);
		pBtn->SetAttribute(_T("hotimage"), szBuffer);
		
		m_layBaseList->Add(pBtn);
	}
}

void CQuestionDialogUI::CreateInteractionList()
{

	TCHAR szInteractImageFileName[][MAX_PATH / 4 ] ={
		_T("linkup"),
		_T("fillblanks"),
		_T("table"),
		_T("arithmetic"),
		_T("memorycard"),
		_T("guessword"),
		_T("classified"),
		_T("fraction"),
		_T("textselect"),
		_T("magicbox"),
		_T("seqencing"),
		_T("wordpuzzles"),
		_T("compare"),
		_T("pointseqencing"),
		_T("imagemark"),
		_T("probabilitycard"),
		_T("catchball"),
		_T("balance"),
	};

	TCHAR szInteractTypeName[][MAX_PATH / 4 ] ={
		_T("linkup"),
		_T("nd_fillblank"),
		_T("nd_table"),
		_T("nd_arithmetic"),
		_T("nd_memorycard"),
		_T("nd_guessword"),
		_T("nd_classified"),
		_T("nd_fraction"),
		_T("nd_textselect"),
		_T("nd_magicbox"),
		_T("nd_order"),
		_T("nd_wordpuzzle"),
		_T("nd_compare"),
		_T("nd_pointsequencing"),
		_T("nd_imagemark"),
		_T("nd_probabilitycard"),
		_T("nd_catchball"),
		_T("nd_balance"),
	};

	TCHAR	szIconReleativeDir[]	= _T("Question\\Interaction\\");
	TCHAR	szImageFormat[]			= _T("file='%s%s.jpg' source='%d,%d,%d,%d'");
	TCHAR	szBuffer[MAX_PATH]		= {0};

	for (int i = 0; i < _countof(szInteractImageFileName); ++i)
	{

		CButtonUI*	pBtn	= new CButtonUI;
		pBtn->SetName(_T("item"));
		pBtn->SetFixedHeight(140);
		pBtn->SetFixedWidth(140);
		pBtn->SetUserData(szInteractTypeName[i]);
		pBtn->SetTag(0);

		memset(szBuffer, 0, sizeof(szBuffer));
		_stprintf_s(szBuffer, szImageFormat, szIconReleativeDir, szInteractImageFileName[i], 0, 0, 140, 140);
		pBtn->SetAttribute(_T("bkimage"), szBuffer);


		memset(szBuffer, 0, sizeof(szBuffer));
		_stprintf_s(szBuffer, szImageFormat, szIconReleativeDir, szInteractImageFileName[i], 0, 140, 140, 280);
		pBtn->SetAttribute(_T("hotimage"), szBuffer);

		m_layInteractionList->Add(pBtn);
	}
}

void CQuestionDialogUI::OnItemClick( TNotifyUI& msg )
{
	m_strQuestionType = msg.pSender->GetUserData();
	m_bBasicQuestion = msg.pSender->GetTag();

	Close(IDOK);
}

void CQuestionDialogUI::OnBtnClose( TNotifyUI& msg )
{
	Close(IDCLOSE);
}


LPCTSTR CQuestionDialogUI::GetQuestionType()
{
	return m_strQuestionType.c_str();
}

bool CQuestionDialogUI::IsBasicQuestion()
{
	return m_bBasicQuestion;
}