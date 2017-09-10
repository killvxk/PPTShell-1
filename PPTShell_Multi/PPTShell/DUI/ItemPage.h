#pragma once

#include "DUICommon.h"
#include <vector>
#include "TabPage.h"

#define Search_MaxItemPage_Count 5
typedef struct _ITEM_PAGE
{
	int			nItemType;
	tstring		strPath;
	tstring		strName;
	tstring		strGroup;
	tstring		strChapter;
}ITEM_PAGE,*PITEM_PAGE;

namespace DuiLib
{
	class CItemPageUI : public CVerticalLayoutUI
	{
	public:
		CItemPageUI();
		~CItemPageUI();

		virtual LPCTSTR GetClass() const;
		virtual LPVOID	GetInterface(LPCTSTR pstrName);

		void		AddSrcData( ITEM_PAGE & itemPage);

		int			GetAllPagesCount()			{return m_nAllPagesCount;}
		
		int			GetPerPageCount()			{return m_nPerPageCount;}
		void		SetPerPageCount(int nCount)	{m_nPerPageCount = nCount;}


		void		Create(int nIndex = 0);					//创建
		int			GetSelectPage()				{return m_nCurrentPageIndex;}

		void		Clear();
		CVerticalLayoutUI*	GetMainLayout()		{return m_pMainLayout;}

		void			InsertPageIndex(DWORD dwCount, int nStart, int nIndex, bool bInit);
		bool			OnBtnPageChange(void* pNotify);

		CVerticalLayoutUI*					CreateItemPage();
		CVerticalLayoutUI*					GetContentLayout();//获取内容布局
	protected:
		int									m_nCurrentPageIndex;//当前页数
		int									m_nAllPagesCount;//总页数
		int									m_nPerPageCount;//每页显示个数

		CVerticalLayoutUI*					m_pContentViewLayout; //内容布局
		CVerticalLayoutUI*					m_pContentPageLayout; //翻页布局
		CVerticalLayoutUI*					m_pMainLayout;

		vector<ITEM_PAGE>					m_vecItems;

	public:
		CEventSource						m_OnCreateCallback;

		CDialogBuilder						m_itemPageBuilder;
	};
}
