#pragma once
#include "DUICommon.h"
#include "InteractiveTool.h"

class CInteractiveToolsUI: public WindowImplBase
{
public:
	CInteractiveToolsUI(void);
	~CInteractiveToolsUI(void);
	
	void LoadTools();
private:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	void InitWindow();
	bool OnToolItemClick(void* pNotify);
	CVerticalLayoutUI* CreateCourseToolsGroup(string strGroupName,vector<CInteractiveTool>& tools);

	CVerticalLayoutUI* m_pContainer;
	CDialogBuilder m_builderItem;
	CDialogBuilder m_builderGroup;
	char m_szCurDir[MAX_PATH];//ÔËÐÐÄ¿Â¼
	map<string,vector<CInteractiveTool>> m_mapInteractiveTools;
	int totalHeight;
	int totalWidth;
	int maxHeight;
	int maxWidth;
};
