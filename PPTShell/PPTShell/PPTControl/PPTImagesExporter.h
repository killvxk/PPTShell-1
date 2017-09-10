#pragma once

typedef struct ExportTask
{
	tstring			strPath;
	CEventSource	OnCompleted;
	bool			bShouldCancel;
	bool			bUseExists;
	DWORD			dwTaskId;
	int				nWidth;
	int				nHeight;
	int				nStartIndex;
	int				nEndIndex;
	tstring			strExt;
}ExportTask;


typedef struct ExportNotify
{
	tstring	strPath;
	tstring	strDir;
	LPCTSTR	lptcsExt;
	int		nStartIndex;
	int		nCount;
	DWORD	dwErrorCode;
	DWORD	dwExportId;
}ExportNotify;



class PPTImagesExporter
{
private:
	PPTImagesExporter();
	~PPTImagesExporter();

	static	PPTImagesExporter*		m_pInstance;
	static	deque<ExportTask*>		m_deqTasks;
	static	map<DWORD,ExportTask*>	m_mapTasks;
	static	CRITICAL_SECTION		m_lock;
	static	HANDLE					m_hExportThread;
	static	HWND					m_hOwner;


	static	void					ExportThread(void*);
	static	bool					GenarateThumbnailDir(LPCTSTR lpctsPath, tstring& strDir);
	static	int						CountSlides(LPCTSTR lptcsPath, LPCTSTR lptcsExt );
public:
	static	PPTImagesExporter * GetInstance();



public:
	DWORD   ExportCurrentPPTTo3DImages( LPCTSTR lptcsDir, CDelegateBase& OnCompleted, int nStartIndex = -1, int nEndIndex = -1, LPCTSTR lptcsExt  =_T("jpg"));
	DWORD	ExportCurrentPPTToImages(CDelegateBase& OnCompleted, int nStartIndex = -1, int nEndIndex = -1, LPCTSTR lptcsExt =_T("jpg"));
	DWORD	ExportImages(LPCTSTR lptcsPath, CDelegateBase& OnCompleted, bool bUseExists = true);
	DWORD	ExportImages( LPCTSTR lptcsPath, int nWidth, int nHeight, CDelegateBase& OnCompleted, int nStartIndex = -1, int nEndIndex = -1, bool bUseExists = true, LPCTSTR lptcsExt =_T("jpg"));
	void	CancelExport(DWORD dwExportId);
	void	SetOwner(HWND hWnd);

};