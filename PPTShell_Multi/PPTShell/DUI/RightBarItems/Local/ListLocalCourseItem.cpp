#include "stdafx.h"
#include "ListLocalCourseItem.h"

CListLocalCourseItem::CListLocalCourseItem()
{

}

CListLocalCourseItem::~CListLocalCourseItem()
{

}

void CListLocalCourseItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileCourse;
}

bool CListLocalCourseItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
