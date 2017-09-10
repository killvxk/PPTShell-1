#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"

class CChapterInfoUI : public WindowImplBase
{
public:
	CChapterInfoUI();
	~CChapterInfoUI();


public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	void	Init();
	void	SetPos(POINT & point);

	bool		OnEventChapterChanged(void* pObj);

private:
	CLabelUI*			m_pTitleLabel;
	CLabelUI*			m_pTitle1Label;
	CLabelUI*			m_pChapterLabel;
	CLabelUI*			m_pSectionLabel;
	CLabelUI*			m_pSubSectionLabel;
	CLabelUI*			m_pSubSubSectionLabel;
};

typedef Singleton<CChapterInfoUI>	ChapterInfoUI;