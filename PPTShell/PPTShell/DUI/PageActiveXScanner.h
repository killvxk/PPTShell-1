#pragma once

#include "Util/Stream.h"
//
//ActiveXData
//
typedef struct ActiveXData
{	
	bool	bHad;

	ActiveXData()
	{	
		bHad = false;
	}

}ActiveXData;


//
//QuestionActiveXData
//
typedef struct QuestionActiveXData : ActiveXData
{	
	tstring strPath;

	void ReadFrom(CStream* pStream)
	{
		strPath = pStream->ReadString();
	}

}QuestionActiveXData;


//
//VRActiveXData
//
typedef struct VRActiveXData : ActiveXData
{	
	tstring strPath;

	void ReadFrom(CStream* pStream)
	{
		strPath = pStream->ReadString();
	}

}VRActiveXData;


class CPageActiveXScanner
{
public:
	CPageActiveXScanner(void);
	virtual ~CPageActiveXScanner(void);

public:
	bool	Init(int nPageCount);

	//is scan
	bool	HasScannedAt(int nIndex);
	bool	HasScannedAtCurrentPage();

	//is had
	bool	HasQuestionAt(int nIndex);
	bool	HasQuestionAtCurrentPage();
	bool	HasVRAt(int nIndex);
	bool	HasVRAtCurrentPage();

	LPCTSTR	GetVRPathAt(int nIndex);
	LPCTSTR	GetVRPathAtCurrentPage();

	//
	void	SetCurrentPage(int nIndex);
	void	SetActiveX(CStream* pStream);

	bool	ScanActiveXAt(int nIndex);

private:
	int								m_nPageCount;
	int								m_nCurrentIndex;
	map<int, bool>					m_mapHasScanned;
	map<int, QuestionActiveXData>	m_mapQuestionData;
	map<int, VRActiveXData>			m_mapVRData;

};
