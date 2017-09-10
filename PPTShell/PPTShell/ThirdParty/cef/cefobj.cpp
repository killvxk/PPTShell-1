#include "stdafx.h"
#include "cefobj.h"
#include "Util/Util.h"

typedef int(*NdCefSumFun)(int,int); 
typedef bool(*NdCefInitFun)(); 
typedef bool(*NdCefShowFun)(HWND);
typedef bool(*NdCefSizeChangeFun)(HWND); 
typedef bool(*NdCefCloseByWndFun)(HWND); 
typedef bool(*NdCefShowbyUrlFun)(HWND,LPCWSTR nUrl); 
typedef bool(*NdCefPopupFun)(HWND,int nJsCode); 
typedef bool(*NdCefQuestionTypeFun)(HWND,int nJsCode); 
typedef bool(*NdCefChangeUrlFun)(HWND,LPCWSTR nUrl); 
typedef bool(*NdCefExecuteJSFun)(HWND,LPCWSTR nJsCode); 
typedef bool(*NdCefExecuteJSCallBackFun)(HWND,LPCWSTR nJsCode,LPCWSTR nParam); 
typedef bool(*NdCefClearCookie)(LPCWSTR nUrl,LPCWSTR nCookieName);
typedef void(*NdCefUnInitFun)();
typedef bool(*NdCefUnZipFun)(LPCSTR filezip,LPCSTR filepath);
typedef bool(*NdCefZipFun)(LPCSTR filezip,LPCSTR filepath);

CCefObject::CCefObject()
{
	m_bInit = FALSE;
	m_hCefObject = NULL;
	m_bChange = FALSE;
	m_bCoursePlayChange = false;
	m_bCoursePreviewChange = false;
	m_bCourseFullScreenPreviewChange = false;
}

CCefObject::~CCefObject()
{
	
}

BOOL CCefObject::Init( )
{
	BOOL bRet = FALSE;
	if(m_bInit == FALSE)
	{
		tstring strPath = GetLocalPath();
		strPath += _T("\\bin\\CoursePlayer\\NdCefLib.dll");
		m_hCefObject = LoadLibraryEx(strPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH); 
		if (m_hCefObject != NULL) 
		{ 
			NdCefInitFun pFnNdCefInit = (NdCefInitFun )GetProcAddress(m_hCefObject,"NdCefInit"); 

			if (pFnNdCefInit != NULL) 
			{ 
				bool initSuccess = pFnNdCefInit(); 
				if (initSuccess){
					m_bInit = TRUE;
					bRet = TRUE;
				}
			} 
		}

	}
	else
		bRet = TRUE;

	return bRet;
}

void CCefObject::SizeChange( HWND hwnd)
{
	if (m_hCefObject) {
		NdCefSizeChangeFun pFnNdCefSizeChange = (NdCefSizeChangeFun)GetProcAddress(m_hCefObject,"NdCefSizeChange"); 
		if (pFnNdCefSizeChange != NULL) 
		{ 
			pFnNdCefSizeChange(hwnd); 
		}
	}

}
void CCefObject::SetPopup( HWND hwnd,int nPop)
{
	if (m_hCefObject) {
		NdCefPopupFun pFnNNdCefPopup = (NdCefPopupFun)GetProcAddress(m_hCefObject,"NdCefPopup"); 
		if (pFnNNdCefPopup != NULL) 
		{ 
			pFnNNdCefPopup(hwnd,nPop); 
		}
	}
}
void CCefObject::SetQuestionType( HWND hwnd,int nType)
{
	if (m_hCefObject) {
		NdCefQuestionTypeFun pFnNNdCefQuestionType = (NdCefQuestionTypeFun)GetProcAddress(m_hCefObject,"NdCefQuestionType"); 
		if (pFnNNdCefQuestionType != NULL) 
		{ 
			pFnNNdCefQuestionType(hwnd,nType); 
		}
	}
}
void CCefObject::CoursePlayDui( HWND hwnd, WCHAR* pwszUrl)
{
	if(!m_bCoursePlayChange)
	{
		NdCefShowbyUrlFun pFnNdCefShowbyUrl = (NdCefShowbyUrlFun)GetProcAddress(m_hCefObject,"NdCefShowbyUrl"); 
		if (pFnNdCefShowbyUrl != NULL) 
		{ 
			bool result = pFnNdCefShowbyUrl(hwnd,pwszUrl); 
		}

		m_bCoursePlayChange = TRUE;
	}
	else
	{
		NdCefChangeUrlFun pFnNdCefChangeUrl = (NdCefChangeUrlFun)GetProcAddress(m_hCefObject,"NdCefChangeUrl"); 
		if (pFnNdCefChangeUrl != NULL) 
		{ 
			pFnNdCefChangeUrl(hwnd,pwszUrl); 
		}
	}

}

void CCefObject::CoursePreview( HWND hwnd, WCHAR* pwszUrl )
{
	if(!m_bCoursePreviewChange)
	{
		NdCefShowbyUrlFun pFnNdCefShowbyUrl = (NdCefShowbyUrlFun)GetProcAddress(m_hCefObject,"NdCefShowbyUrl"); 
		if (pFnNdCefShowbyUrl != NULL) 
		{ 
			bool result = pFnNdCefShowbyUrl(hwnd,pwszUrl); 
		}
		m_bCoursePreviewChange = TRUE;
	}
	else
	{
		NdCefChangeUrlFun pFnNdCefChangeUrl = (NdCefChangeUrlFun)GetProcAddress(m_hCefObject,"NdCefChangeUrl"); 
		if (pFnNdCefChangeUrl != NULL) 
		{ 
			pFnNdCefChangeUrl(hwnd,pwszUrl); 
		}
	}
}

void CCefObject::CourseFullScreenPreview( HWND hwnd, WCHAR* pwszUrl )
{
	if(!m_bCourseFullScreenPreviewChange)
	{
		NdCefShowbyUrlFun pFnNdCefShowbyUrl = (NdCefShowbyUrlFun)GetProcAddress(m_hCefObject,"NdCefShowbyUrl"); 
		if (pFnNdCefShowbyUrl != NULL) 
		{ 
			bool result = pFnNdCefShowbyUrl(hwnd,pwszUrl); 
		}
		m_bCourseFullScreenPreviewChange = TRUE;
	}
	else
	{
		NdCefChangeUrlFun pFnNdCefChangeUrl = (NdCefChangeUrlFun)GetProcAddress(m_hCefObject,"NdCefChangeUrl"); 
		if (pFnNdCefChangeUrl != NULL) 
		{ 
			pFnNdCefChangeUrl(hwnd,pwszUrl); 
		}
	}
}

void CCefObject::ShowFromDui( HWND hwnd , WCHAR* pwszUrl)
{
	if(!m_bChange)
	{
		NdCefShowbyUrlFun pFnNdCefShowbyUrl = (NdCefShowbyUrlFun)GetProcAddress(m_hCefObject,"NdCefShowbyUrl"); 
		if (pFnNdCefShowbyUrl != NULL) 
		{ 
			pFnNdCefShowbyUrl(hwnd,pwszUrl); 
		}

		m_bChange = TRUE;
	}
	else
	{
		NdCefChangeUrlFun pFnNdCefChangeUrl = (NdCefChangeUrlFun)GetProcAddress(m_hCefObject,"NdCefChangeUrl"); 
		if (pFnNdCefChangeUrl != NULL) 
		{ 
			pFnNdCefChangeUrl(hwnd,pwszUrl); 
		}
	}
	
}

void CCefObject::ClearCookie( LPCWSTR szUrl,LPCWSTR szCookieName )
{
	if (m_hCefObject) {
		NdCefClearCookie pFnNdCefClearCookie = (NdCefClearCookie)GetProcAddress(m_hCefObject,"NdCefClearCookie"); 

		if (pFnNdCefClearCookie != NULL) 
		{ 
			pFnNdCefClearCookie(szUrl, szCookieName); 
		}
	}
}

void CCefObject::Destroy( HWND hwnd )
{
	if (m_hCefObject) {
		NdCefCloseByWndFun pFnNdCefCloseByWnd = (NdCefCloseByWndFun)GetProcAddress(m_hCefObject,"NdCefCloseByWnd"); 

		if (pFnNdCefCloseByWnd != NULL) 
		{ 
			pFnNdCefCloseByWnd(hwnd); 
		}
	}
}

void CCefObject::Destroy()
{	
	if (m_hCefObject) {
		NdCefUnInitFun pFnNdCefUnInit = (NdCefUnInitFun)GetProcAddress(m_hCefObject,"NdCefUnInit"); 

		if (pFnNdCefUnInit != NULL) 
		{ 
			pFnNdCefUnInit(); 
			FreeLibrary(m_hCefObject);
			m_hCefObject = NULL;
		}
	}
}
bool CCefObject::UnZip(LPCSTR filezip,LPCSTR filepath)
{	
	if (m_hCefObject) {
		NdCefUnZipFun pFnNdCefUnZip = (NdCefUnZipFun)GetProcAddress(m_hCefObject,"NdCefUnZip"); 

		if (pFnNdCefUnZip != NULL) 
		{ 
			return pFnNdCefUnZip(filezip,filepath); 
		}
	}
	return false;
}
bool CCefObject::Zip(LPCSTR filezip,LPCSTR filepath)
{	
	if (m_hCefObject) {
		NdCefZipFun pFnNdCefZip = (NdCefZipFun)GetProcAddress(m_hCefObject,"NdCefZip"); 

		if (pFnNdCefZip != NULL) 
		{ 
			return pFnNdCefZip(filezip,filepath); 
		}
	}
	return false;
}