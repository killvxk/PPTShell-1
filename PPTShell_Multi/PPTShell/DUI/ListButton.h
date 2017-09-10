#ifndef _LISTBUTTON_H_
#define _LISTBUTTON_H_

#include "DUICommon.h"
namespace DuiLib
{

	class CListButtonUI : public CButtonUI
	{
	public:
		CListButtonUI();
		~CListButtonUI();

		LPCTSTR		GetClass() const;
		LPVOID		GetInterface(LPCTSTR pstrName);

		void		SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void		PaintStatusImage(HDC hDC);

		//virtual void DoEvent(TEventUI& event);

		void		Expand(bool bExpand = true);

	private:
		CDuiString	m_strNormalIcon;
		CDuiString	m_strHotIcon;
		CDuiString	m_strExpandIcon;
		CDuiString	m_strTipIcon;
		int			m_nLogoWidth;
		int			m_nLogoHeight;
		CRect		m_rcLogoPadding;
		CRect		m_rcTipPos;

		bool		m_bExpand;
		bool		m_bClickDown;

		DWORD		m_dwNormalColor;
		DWORD		m_dwHoverColor;
		DWORD		m_dwExpandColor;
	};

} // DuiLib

#endif // 