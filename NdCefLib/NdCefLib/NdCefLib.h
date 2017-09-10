#pragma once

#include "NdCef.h"

extern "C" NDCEF_API int  NdCefSum(int a, int b); 
extern "C" NDCEF_API bool NdCefInit(); 
extern "C" NDCEF_API bool NdCefPop(); 
extern "C" NDCEF_API bool NdCefShow(HWND hParentWnd); 
extern "C" NDCEF_API bool NdCefSizeChange(HWND hParent);
extern "C" NDCEF_API bool NdCefCloseByWnd(HWND hParentWnd); 
extern "C" NDCEF_API bool NdCefShowbyUrl(HWND hParent,LPCTSTR nUrl);
extern "C" NDCEF_API bool NdCefQuestionType(HWND hParent,int nType);
extern "C" NDCEF_API bool NdCefPopup(HWND hParent,int nPop);
extern "C" NDCEF_API bool NdCefChangeUrl(HWND hParent,LPCTSTR nUrl);
extern "C" NDCEF_API bool NdCefClearCookie(LPCTSTR nUrl,LPCTSTR nCookieName);
extern "C" NDCEF_API bool NdCefExecuteJS(HWND hParent,LPCTSTR nJSCode);
extern "C" NDCEF_API bool NdCefExecuteJSCallBack(HWND hParent,LPCTSTR eventName,LPCTSTR nParam);
extern "C" NDCEF_API void NdCefUnInit(); 
extern "C" NDCEF_API bool NdCefUnZip(LPCSTR filezip,LPCSTR filepath);
extern "C" NDCEF_API bool NdCefZip(LPCSTR filezip,LPCSTR filepath);
