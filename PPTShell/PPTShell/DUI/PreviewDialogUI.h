#ifndef _PREVIEWDLG_H_
#define _PREVIEWDLG_H_

#pragma once
#include "DUICommon.h"
#include "DUI/GifAnimUI.h"
#include "ResourceItem.h"
#include "WndShadow.h"

enum EPLUS_WND_TYPE
{
	E_WND_FLASH,		//FLASH
	E_WND_COURSE,		//¿Î¼þ
	E_WND_PICTURE,		//Í¼Æ¬
	E_WND_COURSE_LOCAL,
	E_WND_PICTURE_LOCAL,
	E_WND_VR,
};


#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"


class CPreviewFullUI;
class CPreviewDlgUI :
	public WindowImplBase,
	public IDownloadListener
{
public:
	CPreviewDlgUI(void);
	~CPreviewDlgUI(void); 
	

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("CloseBtn"),					OnBtnClose);
		EVENT_BUTTON_CLICK(_T("playBtn") ,					OnBtnPlay );
		EVENT_BUTTON_CLICK(_T("allApplyBtn") ,				OnBtnApply);
		EVENT_BUTTON_CLICK(_T("pageLeftBtn") ,				OnBtnPageLeft);
		EVENT_BUTTON_CLICK(_T("pageRightBtn") ,				OnBtnPageRight);
		EVENT_BUTTON_CLICK(_T("fullScreenBtn") ,			OnBtnFullScreen);
		EVENT_BUTTON_CLICK(_T("insertPageBtn") ,			OnBtnInsertPage);
		EVENT_BUTTON_CLICK(_T("cancelInsertBtn") ,			OnBtnCancelInsert);
		EVENT_BUTTON_CLICK(_T("scrollLeftBtn") ,			OnBtnScrollLeftBtn);
		EVENT_BUTTON_CLICK(_T("scrollRigthBtn") ,			OnBtnScrollRightBtn);
		EVENT_BUTTON_CLICK(_T("btn_insert_vr"),				OnBtnInsertVR);
		EVENT_BUTTON_CLICK(_T("btn_download_vr"),			OnBtnDownloadVR);
		EVENT_BUTTON_CLICK(_T("btn_pause_vr"),				OnBtnPauseVR);
	UIEND_MSG_MAP

public:

	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void		 ShowPreviewWindow(CStream* pStream, EPLUS_WND_TYPE eType = E_WND_PICTURE);
	void		 OnPictureHttpSucess(LPCTSTR lpszPath, LPCTSTR lpszFileSize, LPCTSTR lpszDownLoadNums);
	void		 OnFlashHttpSucess(LPCTSTR lpszPath, LPCTSTR lpszFileSize, LPCTSTR lpszDownLoadNums);
	void		 OnCourseHttpSucess(LPCTSTR lpszPath, LPCTSTR lpszFileSize, LPCTSTR lpszDownLoadNums);
	void		 OnHttpReturnError();
	bool		 OnPPTFileDownloading(void* param);
	bool		 OnPictureDownloaded(void* param);
	bool		 OnPPTFileDownloaded(void* param);

	//new 
	bool		 OnPictrueItemClick(void* pObj);
	bool		 OnCourseItemClick(void* pObj);


	bool		 OnPhotoItemClick(void* param);
	bool		 OnPreviewFullPictureDownloaded(void* param);
	bool		 OnPreviewInsertComplete(void* pParam);

	void		 SetCurItemIndex(int idx);
	int			 GetCurItemIndex(){return m_nCurItemIndex;}
	int			 GetBottomItemCount(){return m_pBottomScrollLayout->GetCount();}
	tstring		 GetItemUrlByIndex();	
	void		 DownloadItemImage();
	void		 CancelPrevieFullDownLoad();
	tstring		GeneratePngPic(LPCTSTR lpszSourceUrl);
//	void		 Init(HWND hWndParent);
	void		 Init(CRect &rect);

	tstring		 m_strTempThumbImage;
 
public:
	void		OnBtnPageLeft(TNotifyUI& msg);
	void		OnBtnPageRight(TNotifyUI& msg);
	

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;

	void		OnBtnScrollLeftBtn(TNotifyUI& msg);
	void		OnBtnScrollRightBtn(TNotifyUI& msg);

	void		OnBtnInsertVR(TNotifyUI& msg);
	void		OnBtnDownloadVR(TNotifyUI& msg);
	void		OnBtnPauseVR(TNotifyUI& msg);
	void		OnBtnClose(TNotifyUI& msg);
	void		OnBtnPlay(TNotifyUI& msg);
	void		OnBtnApply(TNotifyUI& msg);
	
	void		OnBtnFullScreen(TNotifyUI& msg);
	void		OnBtnInsertPage(TNotifyUI& msg);
	void		OnBtnCancelInsert(TNotifyUI& msg);

	
	void		InitFlashWnd(LPCTSTR lpszFileName);		//³õÊ¼»¯flashÔ¤ÀÀ´°¿Ú
	void		InitCourseWnd(CStream* pStream);	//³õÊ¼»¯¿Î¼þÔ¤ÀÀ´°¿Ú
	void		InitPictureWnd(CStream* pStream);	//³õÊ¼»¯Í¼Æ¬Ô¤ÀÀ´°¿Ú
	//void		InitPictureWnd(CStream* pStream);	//³õÊ¼»¯Í¼Æ¬Ô¤ÀÀ´°¿Ú
	void		InitCourseWndLocal(CStream* pStream);

	bool		CheckPtr();

	static DWORD WINAPI SetPosThread(LPARAM lpParam);

	CResourceItemUI* CreateItem();
	void		CreateResourceItem();
	static		DWORD WINAPI ShowPicThread(LPVOID lpVoid);
	void		ShowPicByThread();

	virtual void OnDownloadBefore( THttpNotify* pHttpNotify );
	virtual void OnDownloadProgress( THttpNotify* pHttpNotify );
	virtual void OnDownloadCompleted( THttpNotify* pHttpNotify );
	virtual void OnDownloadInterpose( THttpNotify* pHttpNotify );

	void		SetPreviewTitle(LPCTSTR lpcsTitle);
	void		SetPreviewSize(DWORD dwFileSize);

private:
	CControlUI*				m_pPauseVRBtn;
	CControlUI*				m_pInsertVRBtn;
	CControlUI*				m_pDownloadVRBtn;
	CItemHandler*			m_pVRItemHandler;
	CLabelUI*				m_pDecription;
	CContainerUI*			m_pDecriptionLay;
	CLabelUI*				m_pDuration;
	CContainerUI*			m_pDurationLay;

	CHorizontalLayoutUI*	m_pTopLayout;
	CHorizontalLayoutUI*	m_pMiddleLayout;
	CHorizontalLayoutUI*	m_pBottomLayout;
	CHorizontalLayoutUI*    m_pBottomPicLayout;
	CHorizontalLayoutUI*    m_pPreviewBigLayout;
	CVerticalLayoutUI*	    m_pPreviewBigLayout2;

	CHorizontalLayoutUI*	m_pBottomScrollLayout;
	CLabelUI*				m_pWindowTitleLabel;
	CLabelUI*				m_pFileSizeLabel;
	CLabelUI*				m_pDownloadNumsLabel;
	CButtonUI*				m_pAllApplyBtn;
	CButtonUI*				m_pPlayBtn;
	CButtonUI*				m_pPageLeftBtn;
	CButtonUI*				m_pPageRightBtn;
	CButtonUI*				m_pFullScreenBtn;
	CButtonUI*				m_pInsertPageBtn;
	CButtonUI*				m_pCancelInsertBtn;
	CControlUI*				m_pPreviewBig;
	CControlUI*				m_pPreviewBigError;
	CGifAnimUI*				m_pAnimation;
	CProgressUI*			m_pProgress;
	EPLUS_WND_TYPE			m_eCurrentType;
	EPLUS_WND_TYPE			m_eOriginalType;
	CDialogBuilder		    m_builder;

	tstring					m_strPPTTitle;
	tstring					m_strPPTGuid;
	tstring					m_strPPTUrl;
	int						m_nCurItemIndex;
	int						m_nItemCount;
	int						m_nStartSlideIndex;
	int						m_nEndSlideIndex;
	DWORD					m_dwBigPictureDownloadId;
	DWORD					m_dwPPTDownloadId;
	DWORD					m_dwFullScreenBigPictureDownloadId;
	tstring					m_strPicSourceUrl;

	CVerticalLayoutUI*		m_pMiddlePicLayout;
	tstring					m_strPath;
	tstring					m_strPicSize;
	tstring					m_strDownloadNums;
	HANDLE					m_phThread;
	HANDLE					m_phSetPosThread;
	int						m_nSourceType;//2015.10.21 cwsÀàÐÍ

	int						m_nWidth;//ÆÁÄ»
	int						m_nHeight;

	int						m_nInsertCount;

	CWndShadow				m_WndShadow;

};



#define PIC_PREVIEW_WIDTH 800
#define PIC_PREVIEW_HEIGHT 560



class CPreviewFullUI : public WindowImplBase
{
public:
	CPreviewFullUI(void);
	~CPreviewFullUI(void);
	friend class CPreviewDlgUI;

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("CloseBtn"),			OnBtnClose);
	UIEND_MSG_MAP
public:
	void			InitFullWnd(LPCTSTR lpszImgFile, bool bSuccess, EPLUS_WND_TYPE eWndType);
	static CPreviewFullUI* GetInstance(CPreviewDlgUI* pDlg, HWND hwnd, EPLUS_WND_TYPE eWndType, LPCTSTR lpszImgFile, bool bSuccess);
	static void		ReleaseInstance();
	virtual void	OnFinalMessage( HWND hWnd );




protected:
	virtual CDuiString	 GetSkinFolder();
	virtual CDuiString	 GetSkinFile();
	virtual LPCTSTR		 GetWindowClassName(void) const;
	virtual void	     InitWindow();
	virtual CControlUI*	 CreateControl(LPCTSTR pstrClass);

	void				 OnBtnClose(TNotifyUI& msg);

	SIZE			     GetImageFileSize(LPCTSTR lpszFile);
	LRESULT				 OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	static CPreviewDlgUI*		 m_pPreViewDlg;

	void			      ShowBigPicNormal(tstring strImage);
	void			      ShowBigPicThumb(tstring strImage);

	

private:
	CVerticalLayoutUI*		m_pVerticalLayout;
	CContainerUI*			m_layLoading;
	CGifAnimUI*				m_pLoading;
	CVerticalLayoutUI*		m_pPicCtnLayout;
	CControlUI*				m_pPreviewBig;
	CControlUI*				m_pPreviewError;
	int						m_nHeight;
	int						m_nWidth;
	static CPreviewFullUI*	m_pInstance;
	EPLUS_WND_TYPE			m_eCurrentType;
	SIZE					m_rImgeSize;



};


#endif //_PREVIEWDLG_H_