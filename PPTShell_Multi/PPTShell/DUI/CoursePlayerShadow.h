#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"

class CCoursePlayerShadow : public WindowImplBase
{
public:
	CCoursePlayerShadow(void);
	~CCoursePlayerShadow(void);
public:
	LPCTSTR				GetWindowClassName() const;	

	virtual void		InitWindow();

	virtual CDuiString	GetSkinFile();

	virtual CDuiString	GetSkinFolder();

	void SetShadowLayoutSize(RECT rcPos);
private:
	CVerticalLayoutUI*		m_pShadowLayout; 
};

typedef Singleton<CCoursePlayerShadow>	CoursePlayerShadow;