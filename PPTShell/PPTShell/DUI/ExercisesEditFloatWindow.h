#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"


class CExercisesEditFloatWindow : public WindowImplBase
{
public:
	CExercisesEditFloatWindow(void);
	~CExercisesEditFloatWindow(void);
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("ExerEdit"),		OnExerEditBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR				GetWindowClassName() const;	

	virtual void		InitWindow();

	virtual CDuiString	GetSkinFile();

	virtual CDuiString	GetSkinFolder();

	bool				GetExercisesInfo(tstring strPath);

	tstring				GetExercisePath()	{ return m_strExercisesPath;	}

private:
	void				OnExerEditBtn(TNotifyUI& msg);

	tstring				m_strExercisesPath;
	tstring				m_strExercisesGuid;
	tstring				m_strExercisesType;

};

typedef Singleton<CExercisesEditFloatWindow>	ExercisesEditUI;