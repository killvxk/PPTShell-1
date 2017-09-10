#ifndef _AUTORESIZEOPTION_H_
#define _AUTORESIZEOPTION_H_

#include "DUICommon.h"
#include "NDCloud/NDCloudAPI.h"

#define MAX_OPTION_LENGTH 40
namespace DuiLib
{

	class CAutoResizeOptionUI : public COptionUI
	{
	public:
		CAutoResizeOptionUI();
		~CAutoResizeOptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetText(LPCTSTR pstrText);
		void SetNodeCode(LPCTSTR pstrNodeCode);

		tstring GetNodeCode();
		CategoryNode* getCategoryNode() {return m_pCategoryNode;}
		void setCategoryNode(CategoryNode* pCategoryNode) {m_pCategoryNode = pCategoryNode;}
//		virtual void Selected(bool bSelected);

	private:
		int m_nDefaultWidth;
		tstring	m_szNodeCode;
		CategoryNode* m_pCategoryNode;
		
	};

	class CAutoHideOptionUI : public COptionUI
	{
	public:
		CAutoHideOptionUI();
		~CAutoHideOptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetText(LPCTSTR pstrText);
		void SetGuid(tstring& strGuid);

		ChapterNode* getChapterNode() {return m_pChapterNode;}
		void setChapterNode(ChapterNode* pChapterNode) {m_pChapterNode = pChapterNode;}

		tstring GetGuid();

	private:

		tstring	m_szGuid;

		ChapterNode* m_pChapterNode;

	};

} // DuiLib

#endif // 