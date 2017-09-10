#include "StdAfx.h"
#include "FileTypeFilter.h"
#include "Util.h"
#include <algorithm>

CFileTypeFilter::CFileTypeFilter(void)
{
	m_mapFilter[FILE_FILTER_PPT]					= PPT_TYPE;
	m_mapFilter[FILE_FILTER_PIC]					= PICTURE_TYPE;
	m_mapFilter[FILE_FILTER_VIDEO]					= VIDEO_TYPE;
	m_mapFilter[FILE_FILTER_FLASH]					= FLASH_TYPE;
	m_mapFilter[FILE_FILTER_VOLUME]					= VOLUME_TYPE;
	m_mapFilter[FILE_FILTER_BASIC_EXERCISES]		= QUESTION_TYPE;
	m_mapFilter[FILE_FILTER_INTERACTIVE_EXERCISES]	= QUESTION_TYPE;
	m_mapFilter[FILE_FILTER_OTHER]					= _T("");
}

CFileTypeFilter::~CFileTypeFilter(void)
{
}

bool CFileTypeFilter::IsCorrectType(int nType, tstring strName)
{
	vector<tstring> strFilter = SplitString(m_mapFilter[nType], m_mapFilter[nType].length(), _T(';'), false);

	for (int i=0; i<strFilter.size(); i++)
	{
		int iFilterLen = strFilter[i].length();
		int iNameLen		= strName.length();
		if (iNameLen > iFilterLen)
		{
			tstring strType = strName.substr(iNameLen - iFilterLen, iFilterLen);
			transform(strType.begin(), strType.end(), strType.begin(), ::tolower);
			if (0 == strFilter[i].compare(strType))
				return true;
		}
	}
	return false;
}

int CFileTypeFilter::GetFileType( tstring strPath )
{
	int nType		= FILE_FILTER_OTHER;
	int nLen		= strPath.length();
	int nStartPos	= (nLen > 5) ? (nLen - 5) : 0;

	tstring strExt	= strPath.substr(nStartPos, 5);

	int iPos		= strExt.rfind(_T("."));

	if ( string::npos != iPos )
	{
		tstring strType = strExt.substr(iPos, strExt.length()-iPos);
		transform(strType.begin(), strType.end(), strType.begin(), ::tolower);
		for (int i= FILE_FILTER_PPT; i <FILE_FILTER_OTHER; i++)
		{
			if ( string::npos != m_mapFilter[i].find(strType))
				return i;
		}
	}

	return nType;
}
