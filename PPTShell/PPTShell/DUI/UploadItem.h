#pragma once
#include "DUICommon.h"
#include "BaseItem.h"
#include "DragDialogUI.h"



class CUploadItemUI : 
	public CBaseItemUI
{
public:
	CUploadItemUI();
	virtual  ~CUploadItemUI();

protected:
	virtual	void	Init();
	virtual	void	OnButtonDelete(TNotifyUI& msg);
	virtual bool	OnItemEvent(void* pEvent);
	virtual bool	OnEmptyControlEvent(void* pEvent);
	virtual bool	OnBtnNotify(void* pNotify);
	virtual	void	OnItemClick(TNotifyUI& msg);
	

	virtual bool	OnRenameEvent(void* pObj);
	void			OnRenameStart();
	void			OnRenameEnd();

	DWORD			UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath);
	bool			OnGetServerPath(void * pParam);
	bool			OnUploadProgress(void * pParam);
	bool			OnUploadCompleted(void * pParam);
	bool			OnCommitCompleted(void * pParam);

	void			ShowProgress(bool bVisible);
	void			SetProgress(int nPos);

	void			SetState(int nState);
	

public:
	void			SetTypeTitle(LPCTSTR lptcsType);
	void			SetType(int nType);
	void			StartUpload();
	bool			IsUploaded();
	bool			IsUploading();

	void			SetUploadFinishCallBack(CDelegateBase &delegate);
private:
	CEditUI*		m_edtRename;
	CProgressUI*	m_proDownload;
	DWORD			m_dwDownloadServerPathId;
	DWORD			m_dwUploadId;
	DWORD			m_dwCommitId;
	tstring			m_strUploadGuid;
	tstring			m_strServerPath;
	int				m_nType;

	enum UploadState
	{
		eReady = 0,
		eUploading,
		eSuccess,
		eFail
	};

	int				m_nState;

	CEventSource	m_OnUploadStateCallBack;

};
