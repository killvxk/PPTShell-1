#pragma once
class CItemOptionUI:
	public COptionUI
{
public:
	CItemOptionUI();
	~CItemOptionUI();
public:
	virtual	void	Selected(bool bSelected);

public:
	CEventSource	OnSelect;
};

