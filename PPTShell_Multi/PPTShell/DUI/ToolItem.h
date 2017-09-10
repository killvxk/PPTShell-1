#pragma once

enum ToolGroupCreateType
{
	eCreate_FromDirectory,
	eCreate_FromValue,
};

enum ToolItemType
{
	eTool_Geometry,
	eTool_Exec,
	eTool_Interact,
	eTool_Empty,
};

class CToolItemUI :public CContainerUI
{
public:
	CToolItemUI(void);
	virtual ~CToolItemUI(void);

protected:
	virtual void	Init();

public:
	void		SetResource(LPCTSTR lptcsResource);
	LPCTSTR		GetResource();

	//called before init
	bool			IsEmpty();
	void			SetEmpty(bool bEmpty);
	void			SetIcon(LPCTSTR lptcsResource);
	void			SetIconRect(int nWidth, int nHeight);
	void			SetTitle(LPCTSTR lptcsTitle);
	void			ShowTopBorder(bool bShow);
	void			ShowLeftBorder(bool bShow);
	CDuiString		GetTitle();

	virtual void	OnClick(CContainerUI* pParent);

protected:
	tstring		m_strResource;
	bool		m_bEmpty;

};
