#pragma once
#include "Util/Singleton.h"

class CCefObject
{
public:
	CCefObject();
	~CCefObject();

public:
	BOOL		Init();
	void		ShowFromDui(HWND hwnd, WCHAR* pwszUrl);
	void		CoursePlayDui(HWND hwnd, WCHAR* pwszUrl);
	void		CoursePreview(HWND hwnd, WCHAR* pwszUrl);
	void		CourseFullScreenPreview(HWND hwnd, WCHAR* pwszUrl);
	void		Destroy( HWND hwnd );
	void		Destroy( );
	void		ClearCookie( LPCWSTR szUrl,LPCWSTR szCookieName);
	void		SizeChange(HWND hwnd);
	void		SetPopup( HWND hwnd,int nPop);
	void		SetQuestionType( HWND hwnd,int nType);
	bool		UnZip(LPCSTR filezip,LPCSTR filepath);
	bool	    Zip(LPCSTR filezip,LPCSTR filepath);
private:
	HMODULE m_hCefObject;

	BOOL		m_bInit;
	BOOL		m_bChange;
	BOOL		m_bCoursePlayChange;
	BOOL		m_bCoursePreviewChange;
	BOOL		m_bCourseFullScreenPreviewChange;
};

typedef Singleton<CCefObject> CefObject;