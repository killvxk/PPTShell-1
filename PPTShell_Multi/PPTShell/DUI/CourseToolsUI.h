#pragma once
#include "DUICommon.h"
#include "..\..\DuiLib\Layout\UIVerticalLayout.h"
#include "CourseTool.h"

class CCourseToolsUI: public WindowImplBase
{
	/*UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("layoutGroupItem"),OnToolItemClick);
	UIEND_MSG_MAP*/
public:
	CCourseToolsUI(void);
	~CCourseToolsUI(void);

	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	void LoadTools();
	void KillOsrWindows();//关闭学科工具exe
	void SetLeftOrRight(int nLeftOrRight);
private:
	CDialogBuilder m_builderGroup;
	CDialogBuilder m_builderGroupItem;
	CVerticalLayoutUI* m_pContainer;
	char m_szCurDir[MAX_PATH];//运行目录
	string m_strToolsName;//学科工具加载工具
	map<string,vector<CCourseTool>> m_mapCourseTools;
	int totalHeight;
	int maxHeight;
	int m_nLeftOrRight;

	void InitWindow();
	BOOL GetToolsList(LPCTSTR lpszPath, vector<string>& arrFileNames, int pSearchType = 0);//pSearchType = 0查询目录，pSearchType = 1查询文件
	void AddCourseToolsByFolderPath(string folderPath,vector<CCourseTool>& vTools);
	CVerticalLayoutUI* CreateCourseToolsGroup(string strGroupName,vector<CCourseTool>& tools);//生成学科工具组
	bool OnToolItemClick(void* pNotify);
};
