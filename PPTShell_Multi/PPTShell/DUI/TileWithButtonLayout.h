#ifndef _TILEWITHBUTTONLAYOUT_H_
#define _TILEWITHBUTTONLAYOUT_H_

#include "DUICommon.h"
#include "DUI/GifAnimUI.h"

#define Refresh_ScrollBar	4001

namespace DuiLib
{
	class CTileWithButtonLayoutUI : public CTileLayoutUI
	{
	public:
		CTileWithButtonLayoutUI();
		~CTileWithButtonLayoutUI();

	public:
		void	SetPos(RECT rc, bool bNeedInvalidate = true);//重载


		void	ShowMoreButton();
		void	HideMoreButton();
		void	StartMoreBtnLoading();//显示加载
		void	StopMoreBtnLoading();//隐藏加载

		void	SetBtnMoreCallBack(CDelegateBase& delegate);
	protected:
		bool	OnBtnMoreNotify(void * pObj);
		bool	OnScrollBarEvent(void * pObj);

	private:
		CEventSource m_OnMoreButtonCallBack;

		CButtonUI*				m_pDynamicMoreBtn;
		CContainerUI*			m_nDynamicMoreContainer;
		CVerticalLayoutUI*		m_nDynamicMoreLayout;
		CVerticalLayoutUI*		m_nDynamicMoreLoadLayout;
		CGifAnimUI*				m_nDynamicMoreLoadGif;
	};

} // DuiLib

#endif // 