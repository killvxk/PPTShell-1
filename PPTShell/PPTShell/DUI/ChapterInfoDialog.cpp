#include "stdafx.h"
#include "ChapterInfoDialog.h"
#include "NDCloud/CategoryTree.h"
#include "NDCloud/ChapterTree.h"
#include "NDCloud/NDCloudAPI.h"
#include "EventCenter/EventDefine.h"

CChapterInfoUI::CChapterInfoUI()
{
	::OnEvent(EVT_SET_CHAPTER_GUID,		MakeEventDelegate(this, &CChapterInfoUI::OnEventChapterChanged));
}

CChapterInfoUI::~CChapterInfoUI()
{
	::CancelEvent(EVT_SET_CHAPTER_GUID, MakeEventDelegate(this, &CChapterInfoUI::OnEventChapterChanged));
}

LPCTSTR CChapterInfoUI::GetWindowClassName() const
{
	return _T("ChapterInfoUI");
}

void CChapterInfoUI::InitWindow()
{
	m_pTitleLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title")));
	m_pTitle1Label = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title1")));
	m_pChapterLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("chapter")));
	m_pSectionLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("section")));
	m_pSubSectionLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("subSection")));
	m_pSubSubSectionLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("subsubSetion")));
}

DuiLib::CDuiString CChapterInfoUI::GetSkinFile()
{
	return _T("TopBar\\ChapterInfo.xml");
}

DuiLib::CDuiString CChapterInfoUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

void CChapterInfoUI::SetPos( POINT & point )
{
	CRect rect;
	::GetWindowRect(GetHWND(), &rect);
	::MoveWindow(GetHWND(), point.x, point.y, rect.Width(), rect.Height(), TRUE);
	ShowWindow(true);
}

bool CChapterInfoUI::OnEventChapterChanged( void* pObj )
{
	CCategoryTree * pCategoryTree;
	NDCloudGetCategoryTree(pCategoryTree);
	if(pCategoryTree)
	{	
		CategoryNode* pGradeNode = pCategoryTree->FindNode(pCategoryTree->GetSelectedSectionCode(),
			pCategoryTree->GetSelectedGradeCode()
			);
		
		CategoryNode* pCourseNode = pCategoryTree->FindNode(pCategoryTree->GetSelectedSectionCode(),
			pCategoryTree->GetSelectedGradeCode(),
			pCategoryTree->GetSelectedCourseCode()
			);
		CategoryNode* pEditionNode = pCategoryTree->FindNode(pCategoryTree->GetSelectedSectionCode(),
			pCategoryTree->GetSelectedGradeCode(),
			pCategoryTree->GetSelectedCourseCode(),
			pCategoryTree->GetSelectedEditionCode()
			);
		CategoryNode* pSubEditionNode = pCategoryTree->FindNode(pCategoryTree->GetSelectedSectionCode(),
			pCategoryTree->GetSelectedGradeCode(),
			pCategoryTree->GetSelectedCourseCode(),
			pCategoryTree->GetSelectedEditionCode(),
			pCategoryTree->GetSelectedSubEditionCode()
			);

		if(!pGradeNode || !pCourseNode || !pEditionNode || !pSubEditionNode )
			return false;
		tstring strTitle;
		strTitle = pGradeNode->strTitle;
		strTitle += _T(" ¡¤ ");
		strTitle += pCourseNode->strTitle;
		strTitle += _T(" ¡¤ ");
		strTitle += pSubEditionNode->strTitle;
		m_pTitleLabel->SetText(strTitle.c_str());

		tstring strTitle1;
		strTitle1 += pEditionNode->strTitle;
		m_pTitle1Label->SetText(strTitle1.c_str());

		tstring strGuid = NDCloudGetChapterGUID();

		CChapterTree * pChapterTree;
		NDCloudGetChapterTree(pChapterTree);
		if(pChapterTree)
		{
			vector<ChapterNode*> vecChapterLevel;

			ChapterNode* pChapterNode = pChapterTree->FindNode(strGuid);;
			while(pChapterNode && pChapterNode->pParent)
			{
				vecChapterLevel.push_back(pChapterNode);
				pChapterNode = pChapterNode->pParent;
			}

			TCHAR szName[MAX_PATH] = {0};

			for(int i = 0; i < (int)vecChapterLevel.size() && i < 4 ; i++)
			{
				tstring str = vecChapterLevel[vecChapterLevel.size() - i - 1]->strTitle;
				if(i == 0)
				{
					m_pChapterLabel->SetText(str.c_str());
					m_pChapterLabel->SetToolTip(str.c_str());
				}
				else if(i == 1)
				{
					m_pSectionLabel->SetText(str.c_str());
					m_pSectionLabel->SetToolTip(str.c_str());
				}
				else if(i == 2)
				{
					m_pSubSectionLabel->SetText(str.c_str());
					m_pSubSectionLabel->SetToolTip(str.c_str());
				}
				else if(i == 3)
				{
					m_pSubSubSectionLabel->SetText(str.c_str());
					m_pSubSubSectionLabel->SetToolTip(str.c_str());
				}
			}

			ResizeClient(242, 68 + vecChapterLevel.size() * 36);
		}
	}

	return true;
}

void CChapterInfoUI::Init()
{
	OnEventChapterChanged(NULL);
}

