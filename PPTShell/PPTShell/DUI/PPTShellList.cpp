#include "stdafx.h"
#include "PPTShellList.h"

namespace DuiLib
{

	const int kListItemNormalHeight = 32;

	CPPTShellListUI::CPPTShellListUI(/*CPaintManagerUI& paint_manager*/)
		: root_node_(NULL)
		, delay_deltaY_(0)
		, delay_number_(0)
		, delay_left_(0)
		, level_expand_image_(_T("<i list_icon_b.png>"))
		, level_collapse_image_(_T("<i list_icon_a.png>"))
		, level_text_start_pos_(10)
		, text_padding_(0, 0, 0, 0)
		/*, paint_manager_(paint_manager)*/
	{
		SetItemShowHtml(true);

		root_node_ = new PPTShellListNode;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	CPPTShellListUI::~CPPTShellListUI()
	{
		if (root_node_)
			delete root_node_;

		root_node_ = NULL;
	}

	bool CPPTShellListUI::Add(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) == 0 
			|| _tcsicmp(pControl->GetClass(), _T("ListIconItemUI")) == 0
			|| _tcsicmp(pControl->GetClass(), _T("SearchItemUI")) == 0 )
		{
			return CListUI::Add(pControl);
		}
		return false;
	
	}

	bool CPPTShellListUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!pControl)
			return false;
			
		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) == 0 
			|| _tcsicmp(pControl->GetClass(), _T("ListIconItemUI")) == 0
			|| _tcsicmp(pControl->GetClass(), _T("SearchItemUI")) == 0 )
			return CListUI::AddAt(pControl, iIndex);

		return false;
	}

	bool CPPTShellListUI::Remove(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0 
			&& _tcsicmp(pControl->GetClass(), _T("ListIconItemUI")) != 0
			&& _tcsicmp(pControl->GetClass(), _T("SearchItemUI")) != 0 )
			return false;

		if (reinterpret_cast<PPTShellListNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
			return CListUI::Remove(pControl);
		else
			return RemoveNode(reinterpret_cast<PPTShellListNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
	}

	bool CPPTShellListUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0 
			&& _tcsicmp(pControl->GetClass(), _T("ListIconItemUI")) != 0
			&& _tcsicmp(pControl->GetClass(), _T("SearchItemUI")) != 0 )
			return false;

		if (reinterpret_cast<PPTShellListNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
			return CListUI::RemoveAt(iIndex);
		else
			return RemoveNode(reinterpret_cast<PPTShellListNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
	}

	void CPPTShellListUI::RemoveAll()
	{
		CListUI::RemoveAll();
		for (int i = 0; i < root_node_->num_children(); ++i)
		{
			PPTShellListNode* child = root_node_->child(i);
			RemoveNode(child);
		}
		delete root_node_;

		root_node_ = new PPTShellListNode;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	void CPPTShellListUI::DoEvent(TEventUI& event) 
	{
		
		if (event.Type == UIEVENT_RBUTTONDOWN )
		{
			return;
		}
		else if(event.Type==UIEVENT_RBUTTONUP)
		{
			if( IsEnabled() ){
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMRCLICK);
			}
			return;
		}

		if ( event.Type == UIEVENT_BUTTONDOWN )
		{
			if( IsEnabled() ){
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL)
				m_pParent->DoEvent(event);
			else
				CVerticalLayoutUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID)
		{
			if (delay_left_ > 0)
			{
				--delay_left_;
				SIZE sz = GetScrollPos();
				LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
				if ((lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ))
				{
					sz.cy -= lDeltaY;
					SetScrollPos(sz);
					return;
				}
			}
			delay_deltaY_ = 0;
			delay_number_ = 0;
			delay_left_ = 0;
			m_pManager->KillTimer(this, SCROLL_TIMERID);
			return;
		}
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			LONG lDeltaY = 0;
			if (delay_number_ > 0)
				lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
			switch (LOWORD(event.wParam))
			{
			case SB_LINEUP:
				if (delay_deltaY_ >= 0)
					delay_deltaY_ = lDeltaY + 8;
				else
					delay_deltaY_ = lDeltaY + 12;
				break;
			case SB_LINEDOWN:
				if (delay_deltaY_ <= 0)
					delay_deltaY_ = lDeltaY - 8;
				else
					delay_deltaY_ = lDeltaY - 12;
				break;
			}
			if
				(delay_deltaY_ > 100) delay_deltaY_ = 100;
			else if
				(delay_deltaY_ < -100) delay_deltaY_ = -100;

			delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
			delay_left_ = delay_number_;
			m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
			return;
		}

		CListUI::DoEvent(event);
	}

	PPTShellListNode* CPPTShellListUI::GetRoot()
	{
		return root_node_;
	}

	const TCHAR* const kLogoButtonControlName = _T("logo");
	const TCHAR* const kLogoContainerControlName = _T("logo_container");
	const TCHAR* const kTitleControlName = _T("title");
	const TCHAR* const kDescriptionControlName = _T("description");
	const TCHAR* const kOperatorPannelControlName = _T("operation");

	static bool OnLogoButtonEvent(void* event) {
		if( ((TEventUI*)event)->Type == UIEVENT_BUTTONDOWN ) {
			CControlUI* pButton = ((TEventUI*)event)->pSender;
			if( pButton != NULL ) {
				CListContainerElementUI* pListElement = (CListContainerElementUI*)(pButton->GetTag());
				if( pListElement != NULL ) pListElement->DoEvent(*(TEventUI*)event);
			}
		}
		return true;
	}

	PPTShellListNode* CPPTShellListUI::AddNode(const PPTShellListItemInfo& item, PPTShellListNode* parent)
	{
		if (!parent)
			parent = root_node_;

		TCHAR szBuf[MAX_PATH] = {0};

		CListContainerElementUI* pListElement = NULL;

		if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("Skin\\PPTShell_list_item.xml"), (UINT)0, NULL, GetManager()));
		}
		else {
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, GetManager()));
		}
		

		if (pListElement == NULL)
			return NULL;

		PPTShellListNode* node = new PPTShellListNode;

		node->data().level_ = parent->data().level_ + 1;
		if (item.folder)
			node->data().has_child_ = !item.empty;
		else
			node->data().has_child_ = false;

		node->data().folder_ = item.folder;

		node->data().child_visible_ = (node->data().level_ == 0);
		node->data().child_visible_ = false;

		node->data().text_ = item.title;
		node->data().value = item.id;
		node->data().list_elment_ = pListElement;

		if (!parent->data().child_visible_)
			pListElement->SetVisible(false);

		if (parent != root_node_ && !parent->data().list_elment_->IsVisible())
			pListElement->SetVisible(false);

		CDuiRect rcPadding = text_padding_;
// 		for (int i = 0; i < node->data().level_; ++i)
// 		{
// 			rcPadding.left += level_text_start_pos_;		
// 		}
		pListElement->SetPadding(rcPadding);

		CButtonUI* log_button = static_cast<CButtonUI*>(GetManager()->FindSubControlByName(pListElement, kLogoButtonControlName));
		if (log_button != NULL)
		{
			if (/*!item.folder && */!item.logo.IsEmpty())
			{
#if defined(UNDER_WINCE)
				_stprintf(szBuf, _T("%s"), item.logo);
#else
				_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.logo);
#endif
				log_button->SetNormalImage(szBuf);
			}
			else
			{
				CContainerUI* logo_container = static_cast<CContainerUI*>(GetManager()->FindSubControlByName(pListElement, kLogoContainerControlName));
				if (logo_container != NULL)
					logo_container->SetVisible(false);
			}
			log_button->SetTag((UINT_PTR)pListElement);
			log_button->OnEvent += MakeDelegate(&OnLogoButtonEvent);
		}

		CDuiString html_text;
		if (node->data().has_child_)
		{
			if (node->data().child_visible_)
				html_text += level_expand_image_;
			else
				html_text += level_collapse_image_;

#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("<x %d>"), level_text_start_pos_);
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
#endif
			html_text += szBuf;
		}

		if (item.folder)
		{
			html_text += node->data().text_;
		}
		else
		{
#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("%s"), item.nick_name);
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.title);
#endif
			html_text += szBuf;
		}

		CLabelUI* title = static_cast<CLabelUI*>(GetManager()->FindSubControlByName(pListElement, kTitleControlName));
		if (title != NULL)
		{
			if (item.folder)
				title->SetFixedWidth(0);

			title->SetShowHtml(true);
			title->SetText(html_text);
		}

// 		if (!item.folder && !item.description.IsEmpty())
// 		{
// 			CLabelUI* description = static_cast<CLabelUI*>(GetManager()->FindSubControlByName(pListElement, kDescriptionControlName));
// 			if (description != NULL)
// 			{
// #if defined(UNDER_WINCE)
// 				_stprintf(szBuf, _T("<x 20><c #808080>%s</c>"), item.description);
// #else
// 				_stprintf_s(szBuf, MAX_PATH - 1, _T("<x 20><c #808080>%s</c>"), item.description);
// #endif
// 				description->SetShowHtml(true);
// 				description->SetText(szBuf);
// 			}
// 		}

	//	pListElement->SetFixedHeight(kFriendListItemNormalHeight);
		pListElement->SetTag((UINT_PTR)node);
		int index = 0;
		if (parent->has_children())
		{
			PPTShellListNode* prev = parent->get_last_child();
			index = prev->data().list_elment_->GetIndex() + 1;
		}
		else 
		{
			if (parent == root_node_)
				index = 0;
			else
				index = parent->data().list_elment_->GetIndex() + 1;
		}
		if (!CListUI::AddAt(pListElement, index))
		{
			delete pListElement;
			delete node;
			node = NULL;
		}

		parent->add_child(node);
		return node;
	}

	bool CPPTShellListUI::RemoveNode(PPTShellListNode* node)
	{
		if (!node || node == root_node_) return false;

		for (int i = 0; i < node->num_children(); ++i)
		{
			PPTShellListNode* child = node->child(i);
			RemoveNode(child);
		}

		CListUI::Remove(node->data().list_elment_);
		node->parent()->remove_child(node);
		delete node;

		return true;
	}

	void CPPTShellListUI::SetChildVisible(PPTShellListNode* node, bool visible)
	{
		if (!node || node == root_node_)
			return;

		if (node->data().child_visible_ == visible)
			return;

		node->data().child_visible_ = visible;

		TCHAR szBuf[MAX_PATH] = {0};
		CDuiString html_text;
		if (node->data().has_child_)
		{
			if (node->data().child_visible_)
				html_text += level_expand_image_;
			else
				html_text += level_collapse_image_;

#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("<x %d>"), level_text_start_pos_);
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
#endif
			html_text += szBuf;

			html_text += node->data().text_;

			CLabelUI* title = static_cast<CLabelUI*>(GetManager()->FindSubControlByName(node->data().list_elment_, kTitleControlName));
			if (title != NULL)
			{
				title->SetShowHtml(true);
				title->SetText(html_text);
			}
		}

		if (!node->data().list_elment_->IsVisible())
			return;

		if (!node->has_children())
			return;

		PPTShellListNode* begin = node->child(0);
		PPTShellListNode* end = node->get_last_child();
		for (int i = begin->data().list_elment_->GetIndex(); i <= end->data().list_elment_->GetIndex(); ++i)
		{
			CControlUI* control = GetItemAt(i);
			if (_tcsicmp(control->GetClass(), _T("ListContainerElementUI")) == 0)
			{
				if (visible) 
				{
					PPTShellListNode* local_parent = ((PPTShellListNode*)control->GetTag())->parent();
					if (local_parent->data().child_visible_ && local_parent->data().list_elment_->IsVisible())
					{
						control->SetVisible(true);
					}
				}
				else
				{
					control->SetVisible(false);
				}
			}
		}
	}

	bool CPPTShellListUI::CanExpand(PPTShellListNode* node) const
	{
		if (!node || node == root_node_)
			return false;

		return node->data().has_child_;
	}

	bool CPPTShellListUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		if( iIndex == m_iCurSel ) return true;

		// We should first unselect the currently selected item
		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = GetItemAt(m_iCurSel);
			if( pControl != NULL) {
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL )
				{
					CListContainerElementUI* pFriendListItem = static_cast<CListContainerElementUI*>(pControl);
					PPTShellListNode* node = (PPTShellListNode*)pControl->GetTag();
					if ((pFriendListItem != NULL) && (node != NULL) && !node->folder())
					{
						//pFriendListItem->SetFixedHeight(kFriendListItemNormalHeight);
						CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(GetManager()->FindSubControlByName(pFriendListItem, kOperatorPannelControlName));
						if (pOperatorPannel != NULL)
						{
							pOperatorPannel->SetVisible(false);
						}
					}
					pListItem->Select(false);
				}
			}

			m_iCurSel = -1;
		}

		if( iIndex < 0 )
			return false;

		if (!__super::SelectItem(iIndex, bTakeFocus))
			return false;


		CControlUI* pControl = GetItemAt(m_iCurSel);
		if( pControl != NULL) {
			CListContainerElementUI* pFriendListItem = static_cast<CListContainerElementUI*>(pControl);
			PPTShellListNode* node = (PPTShellListNode*)pControl->GetTag();
			if ((pFriendListItem != NULL) && (node != NULL) && !node->folder())
			{
				//pFriendListItem->SetFixedHeight(kFriendListItemSelectedHeight);
				CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(GetManager()->FindSubControlByName(pFriendListItem, kOperatorPannelControlName));
				if (pOperatorPannel != NULL)
				{
					pOperatorPannel->SetVisible(true);
				}
			}
		}
		return true;
	}

	void CPPTShellListUI::SetCourseText(TCHAR * szName,TCHAR * szText)
	{
		CListContainerElementUI * pElement = static_cast<CListContainerElementUI*>(GetItem(szName));
		if(pElement)
		{	
			CControlUI * pControl = pElement->FindSubControl(_T("Course"));
			if(pControl)
			{
				pControl -> SetText(szText);
			}
		}

	}

	void CPPTShellListUI::SetSaveCount(TCHAR * szName,int nCount)
	{
		CListContainerElementUI * pElement = static_cast<CListContainerElementUI*>(GetItem(szName));
		if(pElement)
		{	
			CControlUI * pControl = pElement->FindSubControl(_T("SaveCount"));
			if(pControl)
			{
				TCHAR szText[20] = {0};
				_itot(nCount, szText, 10);
				pControl->SetText(szText);
			}
		}
	}

	void CPPTShellListUI::SetResCount(TCHAR * szName,int nCount)
	{
		CListContainerElementUI * pElement = static_cast<CListContainerElementUI*>(GetItem(szName));
		if(pElement)
		{	
			CControlUI * pControl = pElement->FindSubControl(_T("ResCount"));
			if(pControl)
			{
				TCHAR szText[20] = {0};
				_itot(nCount, szText, 10);
				pControl->SetText(szText);
			}
		}
	}

	CControlUI* CPPTShellListUI::GetItem(TCHAR * szName)
	{
		CControlUI * p;
		for(int i = 0; i< GetCount(); i++)
		{
			p = GetItemAt(i);
			if(_tcsicmp(szName,p->GetName()) == 0)
			{
				return p;
			}
		}
		return NULL;
	}

	CListContainerElementUI* CPPTShellListUI::GetItemByFolderName(LPCTSTR szFolderName)
	{
		for(int i = 0; i< GetCount(); i++)
		{
			CListContainerElementUI * pElement = static_cast<CListContainerElementUI*>(GetItemAt(i));
			if(pElement)
			{	
				CControlUI * pControl = pElement->FindSubControl(_T("contentBtn"));
				if(pControl)
				{
					if(_tcsicmp(szFolderName, pControl->GetText()) == 0)
					{
						return pElement;
					}
				}
			}
			
		}
		return NULL;
	}

} // namespace DuiLib