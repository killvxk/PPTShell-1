#pragma once

 #include <objsafe.h>
// andcefCtrl.h : Declaration of the CandcefCtrl ActiveX Control class.
#include "NdCefIRegisterJs.h"

#define MSG_CEFOCX_PAGECHANGE				1006
#define MSG_CEFOCX_ICR						1007
#define	MSG_CEFOSR_ESC						1008
#ifndef HIMETRIC_PER_INCH  
	#define HIMETRIC_PER_INCH 2540  
#endif  

class CandcefCtrl;

class CandcefCtrlRegisterJs : public NdCef::NdCefIRegisterJs
{
public:
	CandcefCtrlRegisterJs();
	void CefEvent(LPCTSTR,LPCTSTR);
	void SetCandcefCtrl(CandcefCtrl*);
public:
	friend class CandcefCtrl;
private:
	CandcefCtrl* m_CandcefCtrl;
};
// CandcefCtrl : See andcefCtrl.cpp for implementation.
class CandcefCtrl : public COleControl
{
	DECLARE_DYNCREATE(CandcefCtrl)

	// Constructor
public:
	CandcefCtrl();

	// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	int CefCreate();
	int CefShow();
	int CefFocus();
	int CefSizeChange();
	// Implementation
protected:
	~CandcefCtrl();

	DECLARE_OLECREATE_EX(CandcefCtrl)    // Class factory and guid
	//***************************************************************add begin
	//ISafeObject
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ObjSafe, IObjectSafety)


		STDMETHOD_(HRESULT, GetInterfaceSafetyOptions) ( 
		/* [in] */ REFIID riid,
		/* [out] */ DWORD __RPC_FAR *pdwSupportedOptions,
		/* [out] */ DWORD __RPC_FAR *pdwEnabledOptions
		);

		STDMETHOD_(HRESULT, SetInterfaceSafetyOptions) ( 
			/* [in] */ REFIID riid,
			/* [in] */ DWORD dwOptionSetMask,
			/* [in] */ DWORD dwEnabledOptions
			);
	END_INTERFACE_PART(ObjSafe);

	DECLARE_OLETYPELIB(CandcefCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CandcefCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CandcefCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

	// Message maps
	DECLARE_MESSAGE_MAP()

	// Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	// Event maps
	DECLARE_EVENT_MAP()


	// Dispatch and event IDs
public:
	enum {
		dispidimgUrl = 6,
		dispidSetSlideIndex = 5L,
		dispidSetFocus = 4L,
		dispidSendCallback2H5 = 3L,
		eventidcefEvent = 2L,
		dispidUrl = 1
	};
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void CefEventCall(LPCTSTR eventName, LPCTSTR eventParam);
protected:
	void OnUrlChanged(void);
	void OnimgUrlChanged(void);
	std::wstring CefUrlReplace();
	std::wstring CefImagePath();
	std::wstring CefImageUrlPath();
	std::wstring CefPath();
	CString m_Url;
	CString m_imgUrl;
	bool m_bUrlShow;
	void cefEvent(LPCTSTR eventName, LPCTSTR eventParam)
	{
		FireEvent(eventidcefEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR), eventName, eventParam);
	}
	static LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GetMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	bool m_cefCreate;
	LONG m_nSlideIndex;
	static HWND  m_oldhAppWnd;
	static HHOOK m_hHook;
	static HHOOK m_hHook1;
	static BOOL m_nPPTBlank ;
	static BOOL m_nPPTICR ;
	static int m_nPPTOSRESC;
protected:
	VARIANT_BOOL SendCallback2H5(LPCTSTR eventName, LPCTSTR json);
	void SetFocus(LONG nIndex);
	void SetSlideIndex(LONG nIndex);
	HWND GetCefWnd();
	void SetPPTProc();
	void SendEsc2PPT();
public:
	afx_msg void OnDestroy();
public:
	//创建窗口进行定时和消息接受
	void CefRegisterClass();
	void CefCreateWnd();
	static LRESULT CALLBACK CefWndProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);
	static HWND  m_hCefpWnd;
	static CandcefCtrl* m_nCefCtrl;
};

