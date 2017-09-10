#pragma once

#include "DUICommon.h"
#include "LocalItem.h"

class CLocalCourseItemUI : public CLocalItemUI
{
public:
	CLocalCourseItemUI();
	~CLocalCourseItemUI();

	virtual	void		Init();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual	void		OnButtonClick(int nButtonIndex, TNotifyUI& msg);

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual bool		HasSubitems();

	void UploadNetdisc(int currentModeType);

private:
	bool	OnExportImagesCompleted(void* pObj);
	DWORD	m_dwExportId;

	DWORD m_dwUploadServerPathTaskId;
	DWORD m_dwUploadNetdiscTaskId;
	DWORD m_dwUplpadCommitTaskId;
	bool OnGetServerPath( void * pParam );
	void UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath,LPCTSTR strTitle,LPCTSTR strResource,int currentModeType,int userId);
	bool OnUploadCompleted( void * pParam );
	bool OnUploadCommitCompleted( void * pParam );

};
