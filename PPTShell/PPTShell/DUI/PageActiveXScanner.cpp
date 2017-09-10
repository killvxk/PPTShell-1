#include "StdAfx.h"
#include "PageActiveXScanner.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Common.h"


// enum
// {
// 	SHAPE_QUESTION	= 1,
// 	SHAPE_FLASH,
// 	SHAPE_3D,
// 	SHAPE_VR,
// };

CPageActiveXScanner::CPageActiveXScanner(void)
{
}

CPageActiveXScanner::~CPageActiveXScanner(void)
{
}

bool CPageActiveXScanner::Init( int nPageCount )
{
	m_mapHasScanned.clear();
	m_mapVRData.clear();
	m_mapQuestionData.clear();
	
	for(int i = 1; i <= nPageCount; ++i)
	{
		m_mapHasScanned.insert(make_pair(i,false));
	}

	m_nCurrentIndex = 1;
	m_nPageCount	= nPageCount;

	return true;
}

bool CPageActiveXScanner::HasScannedAt( int nIndex )
{
	map<int,bool>::iterator itor = m_mapHasScanned.find(nIndex);
	if (itor != m_mapHasScanned.end())
	{
		return itor->second;
	}

	return false;
}

bool CPageActiveXScanner::HasScannedAtCurrentPage()
{
	return HasScannedAt(m_nCurrentIndex);
}

bool CPageActiveXScanner::HasQuestionAt( int nIndex )
{
	map<int, QuestionActiveXData>::iterator itor = m_mapQuestionData.find(nIndex);
	if (itor != m_mapQuestionData.end())
	{
		return itor->second.bHad;
	}

	return false;
}

bool CPageActiveXScanner::HasQuestionAtCurrentPage( )
{
	return HasQuestionAt(m_nCurrentIndex);
}

bool CPageActiveXScanner::HasVRAt( int nIndex )
{
	map<int, VRActiveXData>::iterator itor = m_mapVRData.find(nIndex);
	if (itor != m_mapVRData.end())
	{
		return itor->second.bHad;
	}

	return false;
}

bool CPageActiveXScanner::HasVRAtCurrentPage()
{
	return HasVRAt(m_nCurrentIndex);
}


void CPageActiveXScanner::SetActiveX( CStream* pStream )
{
	int nIndex = pStream->ReadInt();
	if (nIndex < 0 || nIndex > m_nPageCount)
	{
		return;
	}
	//has scanned
	m_mapHasScanned[nIndex] = true;

	//default data
	QuestionActiveXData	qdata;
	m_mapQuestionData.insert(make_pair(nIndex, qdata));

	VRActiveXData vrdata;
	m_mapVRData.insert(make_pair(nIndex, vrdata));

	int nDataCount = pStream->ReadInt();
	if (nDataCount <= 0)
	{
		return;
	}

	for (int i = 0; i < nDataCount; ++i)
	{
		int nType = pStream->ReadInt();
		if (nType == SHAPE_QUESTION)
		{
			QuestionActiveXData data;
			data.bHad = true;
			data.ReadFrom(pStream);
			m_mapQuestionData[nIndex] = data;
			
		}
		else if (nType == SHAPE_VR)
		{
			VRActiveXData data;
			data.bHad = true;
			data.ReadFrom(pStream);
			m_mapVRData[nIndex] = data;
		}
	}

	
}

bool CPageActiveXScanner::ScanActiveXAt( int nIndex )
{
	if (nIndex <= 0 || nIndex > m_nPageCount)
	{
		return false;
	}

	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_HAS_ACTIVEX_AT_PADGE);
	pStream->WriteInt(nIndex);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return true;
}

void CPageActiveXScanner::SetCurrentPage( int nIndex )
{
	m_nCurrentIndex = nIndex;
}

LPCTSTR CPageActiveXScanner::GetVRPathAt( int nIndex )
{
	map<int, VRActiveXData>::iterator itor = m_mapVRData.find(nIndex);
	if (itor != m_mapVRData.end())
	{
		return itor->second.strPath.c_str();
	}

	return NULL;
}

LPCTSTR CPageActiveXScanner::GetVRPathAtCurrentPage( )
{
	return GetVRPathAt(m_nCurrentIndex);
}	

