#pragma once
#include "DUICommon.h"
#include "Util/Stream.h"
#include "ResourceItem.h"
#include "Toast.h"

class CLocalItemUI : public CResourceItemUI
{
public:
	CLocalItemUI();
	virtual ~CLocalItemUI();

	virtual void UploadNetdisc(int currentModeType);
protected:
	virtual	void	OnItemDragFinish();
	virtual void	Init();
	virtual bool	OnEmptyControlEvent(void* pEvent);

public:
	void			EnableChapter(bool bEnable);
	void			SetChapter(LPCTSTR lptcsChapter);
	void			SetContentHeight( int nHeight );


protected:
	bool			m_bEnableChapter;

private:
	tstring			m_strChapter;

	static CDialogBuilder	m_ChapterStyleBuilder;

	DWORD m_dwUploadServerPathTaskId;
	DWORD m_dwUploadNetdiscTaskId;
	DWORD m_dwUplpadCommitTaskId;
	bool OnGetServerPath( void * pParam );
	void UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath,LPCTSTR strTitle,LPCTSTR strResource,int currentModeType);
	bool OnUploadCompleted( void * pParam );
	bool OnUploadCommitCompleted( void * pParam );

};
