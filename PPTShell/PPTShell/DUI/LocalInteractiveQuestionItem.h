#pragma once
#include "DUICommon.h"
#include "LocalPhotoItem.h"
#include "EventCenter/EventDefine.h"


class CLocalInteractiveQuestionItemUI : public CLocalPhotoItemUI
{
public:
	CLocalInteractiveQuestionItemUI();
	~CLocalInteractiveQuestionItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick(int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		ReadStream( CStream* pStream );

	virtual void		DownloadResource( int nButtonIndex, int nType, int nThumbnailSize );

	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );

	virtual void		OnItemDragFinish();

	virtual void		OnDownloadThumbnailFailed();

	void				UploadNetdisc(int currentModeType);
	void				DownloadLocal();//下载到本地
	void				EditExercises();
protected:

	static DWORD  WINAPI CreateQuestionThumbnailFuc(LPARAM lParam);

	BOOL				GetQuestionInfo( tstring strJosnPath, tstring& strGuid, tstring& strQuestionType );
	bool				OnQuestionFolderUploaded(void* param);
	bool				OnQuestionModelCommited(void* param);
	bool				OnQuestionThumbnail(void* param);
	bool				OnResIdQueried(void* param);

	tstring				m_strDesc;
	tstring				m_strGuid;
	tstring				m_strQuestionType;
	tstring				m_strExercisesDir;
	tstring				m_strPreviewUrl;

	HANDLE				m_hThread;

	CNotifyEventSource	m_OnComplete;
	
};
