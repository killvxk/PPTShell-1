#pragma once
#include "DUICommon.h"
#include "DUI/EditClear.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GifAnimUI.h"
#include "DUI/SearchTipListDlg.h"
#include "DUI/TileWithButtonLayout.h"

#define RequestTotal_Count	1
#define Request_Count	66
#define Search_History_MaxCount	10

#define CheckBox_NULL					0x00000000	
#define CheckBox_Course					0x00000001
#define CheckBox_Image					0x00000002
#define CheckBox_Video					0x00000004
#define CheckBox_Volume					0x00000008
#define CheckBox_Flash					0x00000010
#define CheckBox_Questions				0x00000020
#define CheckBox_CoursewareObjects		0x00000040
#define CheckBox_PPTTemplate			0x00000080
#define CheckBox_Other					0x00000100
#define CheckBox_ALL					(CheckBox_Course | CheckBox_Image | CheckBox_Video | CheckBox_Volume | CheckBox_Flash | CheckBox_Questions |\
										CheckBox_CoursewareObjects | CheckBox_PPTTemplate | CheckBox_Other)

#define Search_Type_101PPT				0x00000001
#define Search_Type_Baidu				0x00000002

#define Refresh_TileLayout				_T("Refresh_TileLayout")
typedef struct _Record_Requested
{
	int nStart;
	int nCount;
}Record_Requested;


class CSearchWindowUI : public CContainerUI , public IDialogBuilderCallback
{
public:
	CSearchWindowUI();
	~CSearchWindowUI();

	virtual void Init();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
private:
	CEditClearUI*		m_pSearchEdit;
	
	CHorizontalLayoutUI*	m_pTipLayout;
	CLabelUI*				m_pTipLabel;
	CLabelUI*				m_pTipTriangle;
	CLabelUI*				m_pKeyWordLabel;
	CLabelUI*				m_pSearchCountLabel;

	CButtonUI*			m_pSearchBtn;
	

	CVerticalLayoutUI*				m_pSearchHistoryLayout;
	CVerticalLayoutUI*				m_pSearchLoadingLayout;
	CVerticalLayoutUI*				m_pSearchContentLayout;
	CVerticalLayoutUI*				m_pNetlessLayout;
	CTileWithButtonLayoutUI*		m_pSearchContentList;

	CDialogBuilder		m_Builder;

	Json::Reader		m_jsonKeyWord;
	Json::FastWriter	m_jsonKeyWordWriter;
	Json::Value			m_jsonKeyWordRoot;
	tstring				m_strKeyWordPath;

	CGifAnimUI*			m_pLoadingGif;

//	DWORD				m_dwCloudThreadId;		//云端数据请求线程
//	DWORD				m_dwDBankThreadId;		//网盘数据请求线程
	
	DWORD				m_dwRequestAllCount;	//返回请求总数
	DWORD				m_dwRequestCount;		//已请求返回总数

	DWORD				m_dwRequestSuccess;		//任务完成数

	DWORD				m_dwResourceTotal;		//资源总数
	DWORD				m_dwRequestedResCount;	//当前显示数

	tstring				m_strCloudRequestUrl;
	tstring				m_strDBankRequestUrl;
	tstring				m_strKeyword;
//
	map<int ,CStream *>				m_mapStream;
	map<int ,Record_Requested*>		m_mapResCount;
	vector<int>			m_vecDownloadThreads;
	vector<CCheckBoxUI*>m_vecCheckBoxs;
	HANDLE				m_hThread;

	DWORD				m_dwCheckBoxFlag;

	bool				m_bSelectAllCheckBox;
	bool				m_bRequestingMore;//请求更多

	CSearchTipListDlg	m_SearchTipListDlg;

	bool				m_bShowTipListDlg;

	DWORD				m_dwSearchType;//0--101教育PPT 1--百度

	CEventSource		m_OnClickCallBack;
			
public:
	void				SetClickCallBack(CDelegateBase& delegate);

	static DWORD		WINAPI OnSearchWithBaiduThread(LPARAM lParam);
protected:
	bool				OnBtnTipEvent(void * pObj);
	bool				OnBtnSearchNotify(void * pObj);
	bool				OnBtnWindowNotify(void * pObj);

	bool				OnBtnClearHistoryNotify(void * pObj);

	bool				OnCheckBoxALLNotify(void * pObj);
	bool				OnCheckBoxNotify(void * pObj);

	bool				OnTipListEvent(void * pObj);		//tiplist

	void				ShowSearchHistory();
	void				InsertKeyWordJson(tstring & strKeyWord);

	void				OnSearchGetCountWith101PPT();//搜索101教育库
	void				OnSearchWithBaidu(bool bInit);//搜索baidu
	void				OnRequesResource(int nType, int nStart, int nCount);
	void				StartMask();
	void				StopMask();

private:
	bool				OnRequestResponse(void* pObj);
	bool				OnDoRequesResourceFinish(void* pObj);

	void				OnDoRequestCountFinish();
	
	//点击加载更多
	bool				OnBtnMoreCallBack(void* pObj);

	void				CheckBoxSelectAll(bool bSelected);
	void				SetAllCheckBox(bool bSelected);

	bool				HideTipListDlg(void* pObj);
	bool				SelectSearchType(void* pObj);

	void				CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

	void				ShowSearchResult();
	void				ClearDownloadThreads();

	BOOL				CheckNetStatus();//检测网络状态

};