#include "stdafx.h"
#include "ListLocalQuestionItem.h"

CListLocalQuestionItem::CListLocalQuestionItem()
{

}

CListLocalQuestionItem::~CListLocalQuestionItem()
{

}

void CListLocalQuestionItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileBasicExercises;
}

bool CListLocalQuestionItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
