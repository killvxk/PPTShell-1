#include "stdafx.h"
#include "ListLocalCoursewareObjectsItem.h"

CListLocalCoursewareObjectsItem::CListLocalCoursewareObjectsItem()
{

}

CListLocalCoursewareObjectsItem::~CListLocalCoursewareObjectsItem()
{

}

void CListLocalCoursewareObjectsItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileInteractiveExercises;
}

bool CListLocalCoursewareObjectsItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
