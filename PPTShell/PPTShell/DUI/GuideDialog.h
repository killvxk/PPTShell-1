#pragma once
#include "DUICommon.h"
#include "AutoResizeOption.h"
#include "NDCloud/NDCloudAPI.h"
#include "CSliderTabLayoutUI.h"
#include "GifAnimUI.h"
#include "DUI/WndShadow.h"
#include "DUI/LabelMulti.h"

#define Course_layout_top 72 //顶部欢迎界面高度
#define Course_layout_middle_top 41 //顶部欢迎界面高度

#define Course_btn_layout_height 32

#define Course_btn_text_width	10*sizeof(TCHAR)//按钮单个字宽度
#define Course_btn_padding_top 10	//按钮间隔上
#define Course_btn_padding_right 12	//按钮间隔右
#define Course_btn_padding_left 12	//按钮间隔左
#define Course_btn_padding_bottom 15//按钮间隔下

#define Course_btn_inset_top 15	//间隔上
#define Course_btn_inset_right 12	//间隔右
#define Course_btn_inset_left 22	//间隔左
#define Course_btn_inset_bottom 15//间隔下

#define Course_btn_height 32		//按钮高度
#define Course_btn_minwidth 80		//按钮最小宽度 
#define Course_label_height	40		//文本高度
#define Course_grade_magrin	8		//年级之间的间距
#define Course_btn_magrin	10		//按钮之间的间距
#define Course_layout_right_width	80		//按钮布局整体宽度
#define Course_layout_bottom_height	25		//布局底部高度

#define Course_Layer_magrin			25
#define Course_high_school_width	0.22	//高中宽度比例
#define Course_Middle_school_width	0.3	//初中小学宽度比例
#define Course_line_padding_left 22	//按钮间隔左

#define Course_status_height	28		//状态图片高度
#define Course_status_width		28		//状态图片宽度

#define Course_chapter_title	_T("请选择课程:")

#define Course_Failed_id		65
#define Course_Complete_id		66
#define Chapter_Complete_id		67
#define Course_Auto_Scroll		68
#define Window_Size_Height		0.72
#define Window_Size_Width		0.65


#define Chapter_Complete_Text _T("恭喜您")
#define Chapter_Complete_Text1 _T("完成了课程的选择")
#define Course_Complete_Text _T("恭喜您")
#define Course_Complete_Text1 _T("完成了教材和版本的选择")
#define Course_Failed_Text _T("出错了,请稍后再试")


enum
{
// 	SectionLabelLayout		= 0,
// 	SectionLayout,
	GradeLabelLayout = 0,
	GradeLayout,
	CourseLabelLayout,
	CourseLayout,
	EditionLabelLayout,
	EditionLayout,
	SubEditionLabelLayout,
	SubEditionLayout,
	LayoutMax,
};

class CGuideDialogUI;
class CMainCourseUI : public CVerticalLayoutUI
{
public:
	CMainCourseUI() {}
	~CMainCourseUI() {}

public:
	CGuideDialogUI* m_pCGuideDialogUI; 
	virtual void PaintBkImage(HDC hDC);
};

class CGuideGradeDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
			return new CGifAnimUI;
		else if( _tcscmp(pstrClass, _T("LabelMultiUI")) == 0 )
			return new CLabelMultiUI;
		else if( _tcscmp(pstrClass, _T("MainCourseUI")) == 0 )
			return new CMainCourseUI;

		return NULL;
	}
};

class CGuideGradeUI : public CContainerUI
{
public:
	CGuideGradeUI();
	~CGuideGradeUI();
};

class CGuideChapterUI : public CContainerUI
{
public:
	CGuideChapterUI()
	{
		CDialogBuilder builder;
		CContainerUI* pGuideChapter = static_cast<CContainerUI*>(builder.Create(_T("Guide\\GuideChapter.xml"), (UINT)0));
		if( pGuideChapter ) 
		{
			this->Add(pGuideChapter);
		}
		else 
		{
			this->RemoveAll();
			return;
		}
	}
};

typedef struct _GUIDETABBAR
{
	CContainerUI*			pTabbarContanier;
	CHorizontalLayoutUI*	pTabbarLayout;
	CLabelUI*				pIconLabel;
	CLabelUI*				pTextLabel;
	CButtonUI*				pButton;
}GUIDETABBAR, *PGUIDETABBAR;

class CGuideDialogUI : public WindowImplBase
{
public:
	static CGuideDialogUI* m_pInstance;
	static CGuideDialogUI* GetInstance();

public:
	CGuideDialogUI();
	~CGuideDialogUI();

	LPCTSTR GetWindowClassName() const;	
	void Notify(TNotifyUI& msg);

	virtual void OnFinalMessage(HWND hWnd);

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	bool OnGradeOneInit(void * pParam);
	bool OnGetBookGUID(void * pParam);
	bool OnGetChapter(void * pParam);
	bool OnGetCourse(void * pParam);
	bool OnGetEdition(void * pParam);

	void ResizeOptText(COptionUI* pOption,bool bResize = false);
	void Init(CRect rect);
	void LoadTree(bool bLocal);

	void OnInitLocalTree();
	bool OnInitTree(tstring strGrade=_T(""), tstring strCourse=_T(""), tstring strEdition=_T(""), tstring strSubEdition=_T(""), bool bInit = false);

	static void  TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

	void	selectImageStatus(int nIndex);//对应0,1,2,3,4
	void	selectButtonStatus(int nIndex);//对应0,1,2,3
	void	TraversalControl(CContainerUI * pContainer, LPCTSTR szName, bool bVisible);
	COptionUI*	TraversalOptionSelect(CContainerUI * pContainer);

	bool	OnChapterChanged( void* pNotify );
	bool	OnTabbarNotify( void* pNotify );
	
	void	startLoading();
	void	stopLoading();

	void	insertTabLayout(ChapterNode * pChapterNode, tstring strNDGuid=_T(""));
	void	insertChapterTabLayout(CContainerUI* pGuideChapter, ChapterNode * pChapterNode, tstring strNDGuid=_T(""));

	void	insertCourseLayout(CategoryNode * pNode, int &nHeight, CVerticalLayoutUI* pVLayout, CVerticalLayoutUI* pContentLayout, LPCTSTR szGroupName, int nLayoutID, tstring strNDCode, bool bAutoSize = false);
	void	insertGradeLayout(CategoryNode * pNode, int &nHeight, CVerticalLayoutUI* pVLayout, CVerticalLayoutUI* pContentLayout, LPCTSTR szGroupName, int nLayoutID, tstring strNDCode);

	bool	OnChapterSwitchFinish( void* pObj );
	void	OnSwitchChapter(int nIndex, bool bPreSwitch, bool bRefreshData = true, bool bAnim = true);
	void	OnSwitchChapterFinish();

	void	MyMessageBox(int nType, LPCTSTR pstr, LPCTSTR pstr1);

	bool	GetClickedChapterNode( CContainerUI * pContainer, ChapterNode * pChapterNode, tstring& strGuid );
private:
	void	CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

	tstring			GetSectionNodeCode(tstring strGradeNodeCode);

	tstring			GetTempChapterGUID();

protected:

	CCategoryTree*			m_CategoryTree;
	CChapterTree*			m_ChapterTree;
	CChapterTree*			m_tmpChapterTree;
	
public:
	CWndShadow				m_WndShadow;
	int						m_LayoutHeight[LayoutMax];
	int						m_nGuideProgress;
protected:

	CAutoResizeOptionUI*		m_pSectionOpt;
	CAutoResizeOptionUI*		m_pGradeOpt;
	CAutoResizeOptionUI*		m_pCourseOpt;
	CAutoResizeOptionUI*		m_pEditionOpt;
	CAutoResizeOptionUI*		m_pSubEditionOpt;

	CAutoHideOptionUI*			m_pChapterOpt;

	CSliderTabLayoutUI*			m_pGuideSwitchTab;

	CVerticalLayoutUI*			m_pMainLayout;
	CControlUI*					m_pMainContainer;
	CMainCourseUI*				m_pMainCourse;

 	CVerticalLayoutUI*			m_pCourseSection;
	CVerticalLayoutUI*			m_pCourseGrade;
	CVerticalLayoutUI*			m_pCourseCourse;
	CVerticalLayoutUI*			m_pCourseEdition;
	CVerticalLayoutUI*			m_pCourseSubEdition;

	CVerticalLayoutUI*			m_pCourseSectionLayout;
	CVerticalLayoutUI*			m_pCourseGradeLayout;
	CVerticalLayoutUI*			m_pCourseCourseLayout;
	CVerticalLayoutUI*			m_pCourseEditionLayout;
	CVerticalLayoutUI*			m_pCourseSubEditionLayout;

	CVerticalLayoutUI*			m_pGuideTop;

	CVerticalLayoutUI*			m_pSwitchTitle;
	CVerticalLayoutUI*			m_pFirstSelectTitle;

	CRect						m_mainRect;
	CCheckBoxUI*				m_pCheckBox;

	vector<CContainerUI*>		m_vecGuideChapter;
	
	CVerticalLayoutUI*			m_pCompleteTip;
	CLabelUI*					m_pCompleteTipText;
	CLabelUI*					m_pCompleteTipText1;
	CVerticalLayoutUI*			m_pInitShowWindow;


	CVerticalLayoutUI*			m_pGifAnimLayout;
	CGifAnimUI*					m_pGifAnim;

	CVerticalLayoutUI*			m_pChapterMask;

	bool						m_bDefaultSetting;
	bool						m_bLoadChapterTree;

	int							m_CourseVisibleWidth;
	int							m_nCurrentSelectLayoutID;

	//CScrollBarUI*				m_pScrollBar;
	BOOL						m_bLocal;
	SIZE						m_AnimPoint;
	CRect						m_AnimRect;

	CHorizontalLayoutUI*		m_pGuideTabBarLayout;
	vector<GUIDETABBAR>			m_vecGuideTabbar;

	CGuideGradeUI*				m_pGuideGrade;
	CVerticalLayoutUI*			m_pConfirmLayout;

	bool						m_bPreSwitch; //向前翻页
	bool						m_bRefreshData;
	int							m_nSliderCount;

	tstring						m_strSectionNodeCode;
	tstring						m_strGradeNodeCode;
	tstring						m_strCourseNodeCode;
	tstring						m_strEditionNodeCode;
	tstring						m_strSubEditionNodeCode;

	DWORD						m_dwDownloadId;
	
	tstring						m_strTempChapterGuid;
};
