#pragma once
//
// ListItem.h
// 
#include "../ListBarLayout.h"

class CListBarSelectChapterLayout : public CListBarLayout
{
public:
	CListBarSelectChapterLayout();
	~CListBarSelectChapterLayout();

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//×ó¼üµã»÷
	virtual void			DoRClick(TNotifyUI*		pNotify) ;				//ÓÒ¼üµã»÷

	bool					OnBtnSelectChapter(void* pObj);
protected:
	//override control
	virtual	void	DoInit();

private:
	CButtonUI*			m_pSelectChapterBtn;
};