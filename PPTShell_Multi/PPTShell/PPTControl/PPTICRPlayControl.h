//===========================================================================
// FileName:				PPTICRPlayControl.h
//	
// Desc:					 
//============================================================================
#ifndef _PPTICRPLAYERCONTROL_H_
#define _PPTICRPLAYERCONTROL_H_

#include "Util/Singleton.h"

class CPPTICRPlayControl
{
private:
	CPPTICRPlayControl();
	virtual ~CPPTICRPlayControl();

public:
	

	DECLARE_SINGLETON_CLASS(CPPTICRPlayControl);

public:
	bool	OnExportImagesCompleted();
	bool	OnExportImagesCompleted(void* pObj);

	static DWORD WINAPI NDICRPlayThread(LPARAM lParam);

	tstring	m_strDirPath;
private:
	
};

typedef Singleton<CPPTICRPlayControl> PPTICRPlayControl;

#endif