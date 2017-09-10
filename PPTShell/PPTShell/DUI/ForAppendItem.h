#pragma once

class CForAppendItemUI : public CContainerUI
{
public:
	CForAppendItemUI();
	virtual ~CForAppendItemUI();


protected:
	virtual	void		Init();
	bool				OnItemClick(void* pObj);

public:
	void				SetClickDelegate(CDelegateBase& OnClick);
private:
	CEventSource		m_OnClick;

};
