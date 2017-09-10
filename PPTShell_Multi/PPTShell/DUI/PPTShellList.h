#ifndef _PPTSHELLLIST_H_
#define _PPTSHELLLIST_H_
#include "PPTShellListNode.h"

namespace DuiLib
{
	struct PPTShellListItemInfo
	{
		bool folder;
		bool empty;
		CDuiString id;
		CDuiString logo;
		CDuiString title;
	};


	class CPPTShellListUI : public CListUI
	{
	public:
		enum {SCROLL_TIMERID = 10};

		CPPTShellListUI(/*CPaintManagerUI& paint_manager*/);

		~CPPTShellListUI();

		bool Add(CControlUI* pControl);

		bool AddAt(CControlUI* pControl, int iIndex);

		bool Remove(CControlUI* pControl);

		bool RemoveAt(int iIndex);

		void RemoveAll();

		void DoEvent(TEventUI& event);

		PPTShellListNode* GetRoot();

		PPTShellListNode* AddNode(const PPTShellListItemInfo& item, PPTShellListNode* parent = NULL);

		bool RemoveNode(PPTShellListNode* node);

		void SetChildVisible(PPTShellListNode* node, bool visible);

		bool CanExpand(PPTShellListNode* node) const;

		bool SelectItem(int iIndex, bool bTakeFocus = false);

		CControlUI* GetItem(TCHAR * szName);
		CListContainerElementUI* GetItemByFolderName(LPCTSTR szFolderName);

		void SetCourseText(TCHAR * szName,TCHAR * szText);
		void SetSaveCount(TCHAR * szName,int nCount);
		void SetResCount(TCHAR * szName,int nCount);
	private:
		PPTShellListNode*	root_node_;
		LONG	delay_deltaY_;
		DWORD	delay_number_;
		DWORD	delay_left_;
		CDuiRect	text_padding_;
		int level_text_start_pos_;
		CDuiString level_expand_image_;
		CDuiString level_collapse_image_;
		/*CPaintManagerUI* paint_manager_;*/

		CDialogBuilder m_dlgBuilder;
	};

} // DuiLib

#endif // 