#ifndef _LISTITEM_H_
#define _LISTITEM_H_

#include "DUICommon.h"
#include "DUI/GifAnimUI.h"
#include "NDCloud/NDCloudAPI.h"

#define Singer_Char_Width 6
#define RightBar_Width		100

static TCHAR *ListItemName[] =
{
	_T(""),
	_T("CloudFileCourse"),
	_T("CloudFileVideo"),
	_T("CloudFileImage"),
	_T("CloudFileFlash"),
	_T("CloudFileVolume"),
	_T("CloudFileQuestion"),
	_T("CloudFileCoursewareObjects"),
	_T("CloudFilePPTTemplate"),
	_T(""),
	_T("CloudFile3DResource"),
	_T(""),
	_T("CloudFileVRResource"),
	_T("CloudFileAssets"),
};

namespace DuiLib
{
	class CListItemUI : public CListContainerElementUI
	{
	public:
		CListItemUI();
		~CListItemUI();

		virtual LPCTSTR GetClass() const;
		virtual LPVOID	GetInterface(LPCTSTR pstrName);
		virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		//UIBEGIN_MSG_MAP
		//	EVENT_BUTTON_CLICK(_T("content"), OnContentClick)
		//UIEND_MSG_MAP
	public:
		void			InitItem();
		virtual	void	DoInit();
		virtual void	DoEvent(TEventUI& event);

		void			AddButtonClick();

		//virtual bool	OnBtnNotify(void* pNotify);

		//void			Invalidate();
		virtual bool	Select(bool bSelect /* = true */);

		void			SetFolderName(LPCTSTR strName);
		CDuiString		GetFolderName();

		void			SetFileCount(int nCount);
		int				GetFileCount()				{ return m_nCurCount; }

		bool			OnGetTotalCount(void * pParam);
		bool			OnGetCurCount(void * pParam);
		void			GetTotalCountInterface();
		void			GetCurCountInterface(tstring strGuid);
		
		bool			OnGetCurDBCount(void* pParam);
		bool			OnGetCurDBExercisesCount(void* pParam);
		void			GetCurDBCountInterface(DWORD dwUserId);

		void			GetCloudAssetsCurCountInterface(tstring strGuid);
		void			GetCloudAssetsTotalCountInterface();
		bool			OnGetCloudAssetsCurCount(void * pParam);
		bool			OnGetDBankAssetsCurCount(void * pParam);
		bool			OnGetCloudAssetsTotalCount(void * pParam);

		void			GetDBankAssetsCurCountInterface(DWORD dwUserId);

		bool			OnChapterChanged( void* pObj );
		bool			OnRefreshGroupExplorer( void* pObj );
		
		void			SetType(int nType);
		int				GetType();

		CStream*		GetPPTTemplateStream();
		void			SetPPTTemplateStream(CStream *pPPTTemplateStream);

		void			SetModuleIcon(tstring strIconPath, tstring strNormalPath, tstring strSelectPath);

	public:
		CButtonUI*			m_pBtn;
	private:
		CDuiString			m_strLogo;
		CDuiString			m_strName;
		int					m_nCurCount;
		int					m_nTotalCount;

		CDuiString			m_strNormalIcon;
		CDuiString			m_strHotIcon;

		DWORD				m_dwNormalColor;
		DWORD				m_dwHotColor;
		DWORD				m_dwSelectedColor;
		DWORD				m_dwSelCountColor;

		COptionUI*			m_pContainerIcon;
		
		CLabelUI*			m_pTextCurCount;
		CLabelUI*			m_pTextSeparator;
		CLabelUI*			m_pTextTotalCount;
		CGifAnimUI*			m_pTotalLoadGif;
		CGifAnimUI*			m_pCurLoadGif;
		CGifAnimUI*			m_pCurDBLoadGif;

		DWORD				m_dwGetCurCountId;

		CHorizontalLayoutUI*m_pCountContainer;

		int					m_nType;

		bool				m_bCurNetLess;//用来判断网络状态
		bool				m_bTotalNetLess;//用来判断网络状态

		bool				m_bModifyName; //用来判断改名按钮是否可用
		map<tstring,CStream *>		m_mapPPTTemplateStream;

		int					m_nGetCurAssetsThreadCount;
		int					m_nGetTotalAssetsThreadCount;

		int					m_nGetCurAssetsCount;
		int					m_nGetTotalAssetsCount;

		DWORD				m_dwGetCurImageThreadId;
		DWORD				m_dwGetCurVideoThreadId;
		DWORD				m_dwGetCurVolumeThreadId;
		DWORD				m_dwGetCurFlashThreadId;
	private:
		void CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);
	};

} // DuiLib

#endif // 