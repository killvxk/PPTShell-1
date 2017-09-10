// andcefCtrl.cpp : Implementation of the CandcefCtrl ActiveX Control class.

#include "stdafx.h"
#include "andcef.h"
#include "andcefCtrl.h"
#include "andcefPropPage.h"

#include "include/cef_app.h"
#include "include/base/cef_scoped_ptr.h"
#include "NdCefMain.h"
#include <psapi.h>  
#pragma comment (lib,"psapi.lib")

using namespace NdCef;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNCREATE(CandcefCtrl, COleControl)


extern scoped_ptr<CNdCefMain> nCefMain;
int g_nClearMemory = 0;
// Message map

HWND CandcefCtrl::m_oldhAppWnd = NULL; 
HHOOK CandcefCtrl::m_hHook=NULL;
HHOOK CandcefCtrl::m_hHook1=NULL;
HWND  CandcefCtrl::m_hCefpWnd=NULL;
BOOL CandcefCtrl::m_nPPTBlank = true;
BOOL CandcefCtrl::m_nPPTICR = false;
BOOL CandcefCtrl::m_nPPTOSRESC = 0;
CandcefCtrl* CandcefCtrl::m_nCefCtrl=NULL;

BEGIN_MESSAGE_MAP(CandcefCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, &CandcefCtrl::OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CandcefCtrl, COleControl)
	DISP_FUNCTION_ID(CandcefCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_NOTIFY_ID(CandcefCtrl, "Url", dispidUrl, m_Url, OnUrlChanged, VT_BSTR)
	DISP_FUNCTION_ID(CandcefCtrl, "SendCallback2H5", dispidSendCallback2H5, SendCallback2H5, VT_BOOL, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CandcefCtrl, "SetFocus", dispidSetFocus, SetFocus, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CandcefCtrl, "SetSlideIndex", dispidSetSlideIndex, SetSlideIndex, VT_EMPTY, VTS_I4)
	DISP_PROPERTY_NOTIFY_ID(CandcefCtrl, "imgUrl", dispidimgUrl, m_imgUrl, OnimgUrlChanged, VT_BSTR)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CandcefCtrl, COleControl)
	EVENT_CUSTOM_ID("cefEvent", eventidcefEvent, cefEvent, VTS_BSTR VTS_BSTR)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CandcefCtrl, 1)
PROPPAGEID(CandcefPropPage::guid)
END_PROPPAGEIDS(CandcefCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CandcefCtrl, "ANDCEF.andcefCtrl.1",
					   0xebe03648, 0xd0a0, 0x456a, 0x86, 0x49, 0xe7, 0xb9, 0xe8, 0xa1, 0x99, 0xc1)



					   // Type library ID and version

					   IMPLEMENT_OLETYPELIB(CandcefCtrl, _tlid, _wVerMajor, _wVerMinor)



					   // Interface IDs

					   const IID BASED_CODE IID_Dandcef =
{ 0xB495C91F, 0x46CA, 0x48AC, { 0x83, 0x0, 0x78, 0x9, 0xED, 0xAA, 0x38, 0xCC } };
const IID BASED_CODE IID_DandcefEvents =
{ 0xD239E01D, 0x4187, 0x49E9, { 0x84, 0xE, 0xD0, 0x8C, 0x5C, 0x2F, 0xB2, 0x7B } };



// Control type information

static const DWORD BASED_CODE _dwandcefOleMisc =
OLEMISC_ACTIVATEWHENVISIBLE |
OLEMISC_SETCLIENTSITEFIRST |
OLEMISC_INSIDEOUT |
OLEMISC_CANTLINKINSIDE |
OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CandcefCtrl, IDS_ANDCEF, _dwandcefOleMisc)

CandcefCtrlRegisterJs::CandcefCtrlRegisterJs(){
	m_CandcefCtrl = NULL;
}
void CandcefCtrlRegisterJs::CefEvent(LPCTSTR eventName,LPCTSTR eventParam){
	if (m_CandcefCtrl!=NULL){
		m_CandcefCtrl->CefEventCall(eventName,eventParam);
	};
}
void CandcefCtrlRegisterJs::SetCandcefCtrl(CandcefCtrl* nCandcefCtrl){
	m_CandcefCtrl = nCandcefCtrl;
}

CandcefCtrlRegisterJs g_andcefCtrlRegisterJs;

// CandcefCtrl::CandcefCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CandcefCtrl

BOOL CandcefCtrl::CandcefCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
		AfxGetInstanceHandle(),
		m_clsid,
		m_lpszProgID,
		IDS_ANDCEF,
		IDB_ANDCEF,
		afxRegInsertable | afxRegApartmentThreading,
		_dwandcefOleMisc,
		_tlid,
		_wVerMajor,
		_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}
/////////////////////////////////////////////////////////////////////////////
// Interface map for IObjectSafety

BEGIN_INTERFACE_MAP( CandcefCtrl, COleControl )
	INTERFACE_PART(CandcefCtrl, IID_IObjectSafety, ObjSafe)    
END_INTERFACE_MAP()    

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety member functions

// Delegate AddRef, Release, QueryInterface

ULONG FAR EXPORT CandcefCtrl::XObjSafe::AddRef()
{
	METHOD_PROLOGUE(CandcefCtrl, ObjSafe)
		return pThis->ExternalAddRef();
}

ULONG FAR EXPORT CandcefCtrl::XObjSafe::Release()
{
	METHOD_PROLOGUE(CandcefCtrl, ObjSafe)
		return pThis->ExternalRelease();
}

HRESULT FAR EXPORT CandcefCtrl::XObjSafe::QueryInterface(
	REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CandcefCtrl, ObjSafe)
		return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

const DWORD dwSupportedBits =
INTERFACESAFE_FOR_UNTRUSTED_CALLER |
INTERFACESAFE_FOR_UNTRUSTED_DATA;
const DWORD dwNotSupportedBits = ~ dwSupportedBits;

/////////////////////////////////////////////////////////////////////////////
// CStopLiteCtrl::XObjSafe::GetInterfaceSafetyOptions
// Allows container to query what interfaces are safe for what. We’re
// optimizing significantly by ignoring which interface the caller is
// asking for.
HRESULT STDMETHODCALLTYPE
CandcefCtrl::XObjSafe::GetInterfaceSafetyOptions(
	REFIID riid,
	DWORD __RPC_FAR *pdwSupportedOptions,
	DWORD __RPC_FAR *pdwEnabledOptions)
{
	METHOD_PROLOGUE(CandcefCtrl, ObjSafe)

		HRESULT retval = ResultFromScode(S_OK);

	// does interface exist?
	IUnknown FAR* punkInterface;
	retval = pThis->ExternalQueryInterface(&riid,
		(void * *)&punkInterface);
	if (retval != E_NOINTERFACE) { // interface exists
		punkInterface->Release(); // release it–just checking!
	}

	// we support both kinds of safety and have always both set,
	// regardless of interface
	*pdwSupportedOptions = *pdwEnabledOptions = dwSupportedBits;

	return retval; // E_NOINTERFACE if QI failed
}

/////////////////////////////////////////////////////////////////////////////
// CStopLiteCtrl::XObjSafe::SetInterfaceSafetyOptions
// Since we’re always safe, this is a no-brainer–but we do check to make
// sure the interface requested exists and that the options we’re asked to
// set exist and are set on (we don’t support unsafe mode).
HRESULT STDMETHODCALLTYPE
CandcefCtrl::XObjSafe::SetInterfaceSafetyOptions(
	REFIID riid,
	DWORD dwOptionSetMask,
	DWORD dwEnabledOptions)
{
	METHOD_PROLOGUE(CandcefCtrl, ObjSafe)

		// does interface exist?
		IUnknown FAR* punkInterface;
	pThis->ExternalQueryInterface(&riid, (void * *)&punkInterface);
	if (punkInterface) { // interface exists
		punkInterface->Release(); // release it–just checking!
	}
	else { // interface doesn’t exist
		return ResultFromScode(E_NOINTERFACE);
	}

	// can’t set bits we don’t support
	if (dwOptionSetMask & dwNotSupportedBits) {
		return ResultFromScode(E_FAIL);
	}

	// can’t set bits we do support to zero
	dwEnabledOptions &= dwSupportedBits;
	// (we already know there are no extra bits in mask )
	if ((dwOptionSetMask & dwEnabledOptions) !=
		dwOptionSetMask) {
			return ResultFromScode(E_FAIL);
	}       

	// don’t need to change anything since we’re always safe
	return ResultFromScode(S_OK);
}
/////////////////////////////////////////////////////////////////////////////


// CandcefCtrl::CandcefCtrl - Constructor

CandcefCtrl::CandcefCtrl()
{
	CefCreateWnd();
	InitializeIIDs(&IID_Dandcef, &IID_DandcefEvents);
	g_andcefCtrlRegisterJs.SetCandcefCtrl(this);
	// TODO: Initialize your control's instance data here.
}



// CandcefCtrl::~CandcefCtrl - Destructor

CandcefCtrl::~CandcefCtrl()
{
	m_nCefCtrl = NULL;
	g_andcefCtrlRegisterJs.SetCandcefCtrl(NULL);

	if (m_hHook1){
		::UnhookWindowsHookEx(m_hHook1);
		m_hHook1 = NULL;
	}
	if (m_hHook){
		::UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
	}
	// TODO: Cleanup your control's instance data here.
	//if (nCefMain.get()){
	//nCefMain.get()->CloseAllBrowsers();
	//nCefMain.get()->Shutdown();
	//nCefMain.release();
	//nCefMain.reset();
	//}
	CNdDebug::printfStr("CandcefCtrl::~CandcefCtrl()");

}
void ClearMenory(){
	g_nClearMemory++;
	if (g_nClearMemory>10){
		SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);  
		EmptyWorkingSet(GetCurrentProcess());
		g_nClearMemory = 0;
		OutputDebugString(L"-------------ClearMenory**********************");
	}
}
int g_imgWidth = 0;
int g_imgHeight = 0;
bool DisplayImage(HDC hDC, LPCTSTR szImagePath,const CRect& rcBounds,bool bimgSize)
{
	//Image  CNdCefThumbSave::FixedSize();
	bool nResult = false;
	try         
	{ 
		HANDLE hFile=CreateFile(szImagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //从指定的路径szImagePath中读取文件句柄
		if(hFile != INVALID_HANDLE_VALUE)  
		{ 
			DWORD dwFileSize=GetFileSize(hFile, NULL); //获得图片文件的大小，用来分配全局内存
			if(dwFileSize > 0)  
			{ 
				HGLOBAL hImageMemory=GlobalAlloc(GMEM_MOVEABLE, dwFileSize); //给图片分配全局内存
				void *pImageMemory=GlobalLock(hImageMemory); //锁定内存
				DWORD dwReadedSize; //保存实际读取的文件大小
				ReadFile(hFile, pImageMemory, dwFileSize, &dwReadedSize, NULL); //读取图片到全局内存当中
				GlobalUnlock(hImageMemory); //解锁内存
				CloseHandle(hFile); //关闭文件句柄
				if (hImageMemory){
					IStream *pIStream;//创建一个IStream接口指针，用来保存图片流
					IPicture *pIPicture;//创建一个IPicture接口指针，表示图片对象
				
					if(CreateStreamOnHGlobal(hImageMemory, true, &pIStream) == S_OK) {//用全局内存初使化IStream接口指针
						if(OleLoadPicture(pIStream, 0, false, IID_IPicture, (LPVOID*)&(pIPicture))== S_OK){
							//;用OleLoadPicture获得IPicture接口指针
							//得到IPicture COM接口对象后，你就可以进行获得图片信息、显示图片等操作
							HDC hdc = ::CreateDC(_T("display"), NULL, NULL, NULL);
							OLE_XSIZE_HIMETRIC hmWidth;
							OLE_YSIZE_HIMETRIC hmHeight;
							pIPicture->get_Width(&hmWidth); //用接口方法获得图片的宽和高
							pIPicture->get_Height(&hmHeight);
							OLE_XSIZE_HIMETRIC hmWidthNew = hmWidth;
							OLE_YSIZE_HIMETRIC hmHeightNew = hmHeight;
							int nPicWidth = MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_PER_INCH);
							int nPicHeight = MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_PER_INCH);
							int nrcWidth = rcBounds.Width();
							int nrcHeight = rcBounds.Height(); 
							if (rcBounds.Width()>0&&rcBounds.Height()>0){
								if (hmWidth/rcBounds.Width()<hmHeight/rcBounds.Height()){
									hmWidthNew = hmHeight/rcBounds.Height()*rcBounds.Width();
								}else if (hmWidth/rcBounds.Width()>hmHeight/rcBounds.Height()){
									hmHeightNew = hmWidth/rcBounds.Width()*rcBounds.Height();
								}
								if (hmWidthNew>hmWidth){
									hmWidthNew = hmWidth; 
								}
								if (hmHeightNew>hmHeight){
									hmHeightNew = hmHeight; 
								}
							}
							if (nPicWidth>rcBounds.Width()||nPicHeight>rcBounds.Height()){
								;
							}else{
								hmWidthNew = hmWidth; 
								hmHeightNew = hmHeight; 
							}
							
							CString ninfo=L"";
							ninfo.Format(L"Width:%d,Height:%d,hmHeight:%d,hmWidth:%d,%d,%d,%d,%d",
								rcBounds.Width(),rcBounds.Height(),hmHeight,hmWidth,hmHeightNew,hmWidthNew,nPicWidth,nPicHeight);
							OutputDebugString(ninfo);
							if (bimgSize){
								int nleft = (rcBounds.Width()-nPicWidth)/2;
								int ntop  = (rcBounds.Height()-nPicHeight)/2;
								nResult = true;
								pIPicture->Render(hDC,nleft,ntop,nPicWidth,nPicHeight,0,hmHeight,hmWidth,-hmHeight,NULL); //在指定的DC上绘出图片
							}else{
								nResult = true;
								pIPicture->Render(hDC,0,0,nrcWidth,nrcHeight,0,hmHeightNew,hmWidthNew,-hmHeightNew,NULL); //在指定的DC上绘出图片
							}
							
							pIPicture->Release(); //释放pIPicture
							::DeleteDC(hdc);
						}
						pIStream->Release(); //释放pIStream
					}
					GlobalFree(hImageMemory); //释放全局内存
					hImageMemory = NULL;
				}
				
			}else{
				CloseHandle(hFile); //关闭文件句柄
			}
		}
	}  
	catch(...)  
	{  
		//_ASSERT(0);  
		//throw;    
	} 
	return nResult;
}

// CandcefCtrl::OnDraw - Drawing function

void CandcefCtrl::OnDraw(
						 CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;
	m_bUrlShow = true;
	bool nDraw = true;
	//DoSuperclassPaint(pdc, rcBounds);
	OutputDebugString(L"**********CandcefCtrl::OnDraw**********");
	OutputDebugString(m_imgUrl);
	if (m_imgUrl!=L""){
		nDraw = false;
		CString nTempbigPath = m_imgUrl;
		nTempbigPath.Replace(L"main.xml.jpg",L"mainbig.xml.jpg");
		if (CNdCefMain::FindFirstFileExists(nTempbigPath,false)){
			OutputDebugString(L"**********CandcefCtrl::DisplayBigImage**********");
			//if (!AmbientUserMode()){
			nDraw = DisplayImage(pdc->GetSafeHdc(),nTempbigPath,rcBounds,false);
			//}
		}else if (CNdCefMain::FindFirstFileExists(m_imgUrl,false)){
				OutputDebugString(L"**********CandcefCtrl::DisplayImage**********");
				//if (!AmbientUserMode()){
				nDraw = DisplayImage(pdc->GetSafeHdc(),m_imgUrl,rcBounds,false);
				//}
		}else{
			CString nTempPath = m_imgUrl;
			nTempPath.Replace(L"\\",L"/");
			nTempPath = nTempPath.Left(nTempPath.ReverseFind('/'));
			if (!CNdCefMain::FindFirstFileExists(nTempPath,false)){
				nDraw = true;
				std::wstring nImagePath = CefImagePath();
				m_bUrlShow = false;
				if (CNdCefMain::FindFirstFileExists(nImagePath.c_str(),false)){
					if (!AmbientUserMode()){
						DisplayImage(pdc->GetSafeHdc(),nImagePath.c_str(),rcBounds,true);
					}
				}
			}
		}
	}
	if (!nDraw){
		if (IsWindow(m_hCefpWnd)){
			m_nCefCtrl = this;
			::KillTimer(m_hCefpWnd,1);
			::SetTimer(m_hCefpWnd,1,2000,NULL);
		}
	}
	//

	if (!IsOptimizedDraw())
	{
		if (AmbientUserMode()){
			if (!m_cefCreate) {
				m_cefCreate = true;
				CefSizeChange();
				CNdDebug::printfStr("AmbientUserMode");
			}
		}else{
			m_cefCreate = false;
		}
	}
}



// CandcefCtrl::DoPropExchange - Persistence support

void CandcefCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_String(pPX,TEXT("Url"),m_Url,TEXT("about:blank"));
	PX_String(pPX,TEXT("imgUrl"),m_imgUrl,TEXT(""));
	// TODO: Call PX_ functions for each persistent custom property.
}



// CandcefCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CandcefCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}



// CandcefCtrl::OnResetState - Reset control to default state

void CandcefCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CandcefCtrl::AboutBox - Display an "About" box to the user

void CandcefCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_ANDCEF);
	dlgAbout.DoModal();
}



// CandcefCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CandcefCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}



// CandcefCtrl::IsSubclassedControl - This is a subclassed control

BOOL CandcefCtrl::IsSubclassedControl()
{
	return TRUE;
}



// CandcefCtrl::OnOcmCommand - Handle command messages

LRESULT CandcefCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}



// CandcefCtrl message handlers

void CandcefCtrl::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	/*if (nCefMain.get()){
	nCefMain.get()->Shutdown();
	}*/
	OutputDebugString(L"CandcefCtrl::OnClose()");
	COleControl::OnClose(OLECLOSE_NOSAVE);
}

void CandcefCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{

	OutputDebugString(L"CandcefCtrl::OnActivate\n");
	COleControl::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
}

void CandcefCtrl::OnUrlChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	OutputDebugString(m_Url);
	BoundPropertyChanged(1);
	SetModifiedFlag();
	m_cefCreate = false;
	CefShow();
}
void CandcefCtrl::OnimgUrlChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	OutputDebugString(m_imgUrl);
	BoundPropertyChanged(6);
	SetModifiedFlag();
	OutputDebugString(L"**********CandcefCtrl::OnimgUrlChanged**********");
	Refresh();
}

int CandcefCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CefCreate();

	return 0;
}
std::wstring CandcefCtrl::CefPath(){
	TCHAR szPath[MAX_PATH];
	if( !GetModuleFileName( AfxGetInstanceHandle(), szPath, MAX_PATH ) )
	{
		return 0;
	}
	CString nPath(szPath);
	nPath.Replace(L"\\",L"/");
	nPath = nPath.Left(nPath.ReverseFind('/'));
	nPath = nPath.Left(nPath.ReverseFind('/'));
	nPath = nPath.Left(nPath.ReverseFind('/'));
	std::wstring nUrl = nPath.GetBuffer();
	nPath.ReleaseBuffer();
	return nUrl;
}
std::wstring CandcefCtrl::CefImagePath(){
	std::wstring nPath = CefPath();
	std::wstring nimgPath = nPath + TEXT("/Skins/cef/nofile.jpg");
	return nimgPath;
}
std::wstring CandcefCtrl::CefImageUrlPath(){
	std::wstring nPath = CefPath();
	std::wstring nimgPath = nPath + TEXT("/Skins/cef/nofile.html");
	return nimgPath;
}
/*
std::wstring CandcefCtrl::CefImageDefault(){
	
}
std::wstring CandcefCtrl::CefUrlDefault(){
	std::wstring nPath = CefPath();
	std::wstring configPath = nPath + TEXT("/setting/Config.ini");

	TCHAR szNDCloudPath[MAX_PATH];
	GetPrivateProfileString(TEXT("Config"), TEXT("NDCloudPath"), TEXT(""), szNDCloudPath, MAX_PATH, configPath.c_str());
	CString ndpxPath(szNDCloudPath);
	ndpxPath = ndpxPath.TrimRight('/');
	ndpxPath+=TEXT("/NDCloud/PPTTemp");
}
*/
std::wstring CandcefCtrl::CefUrlReplace(){
	CString nUrltemp = m_Url; 
	std::wstring nPath = CefPath();
	std::wstring configPath = nPath + TEXT("/setting/Config.ini");

	TCHAR szNDCloudPath[MAX_PATH];
	GetPrivateProfileString(TEXT("Config"), TEXT("NDCloudPath"), TEXT(""), szNDCloudPath, MAX_PATH, configPath.c_str());
	CString ndpxPath(szNDCloudPath);
	ndpxPath = ndpxPath.TrimRight('/');
	ndpxPath+=TEXT("/NDCloud/PPTTemp");

	nUrltemp.Replace(L"cef://player",nPath.c_str());
	nUrltemp.Replace(L"cef://ndpx",ndpxPath);
	int nLen = nUrltemp.Find(L"\\Package\\nodejs\\app\\player\\index.html?main=/");
	if (nLen!=-1){
		nUrltemp.Replace(nUrltemp.Left(nLen),nPath.c_str());
	}

	std::wstring nUrl = nUrltemp.GetBuffer();
	nUrltemp.ReleaseBuffer();
	return nUrl;
}
void CandcefCtrl::SendEsc2PPT(){
	if (nCefMain.get()){
		nCefMain->SendEsc2PPT();
	}
}

int CandcefCtrl::CefShow(){
	if (nCefMain.get()){
		std::wstring nUrl = CefUrlReplace();
		nCefMain.get()->SetBrowserSizeChange(this->GetSafeHwnd());
		if (m_bUrlShow){
			nCefMain.get()->ChangeUrl(this->GetSafeHwnd(),nUrl.c_str());
		}else{
			nCefMain.get()->ChangeUrl(this->GetSafeHwnd(),CefImageUrlPath().c_str());
		}
		::SetFocus(this->GetSafeHwnd());
		nCefMain.get()->SetFocusToBrowser(this->GetSafeHwnd());
	}
	return 0;
}

int CandcefCtrl::CefFocus(){
	if (nCefMain.get()){
		//std::wstring nUrl = m_Url.GetBuffer();
		//m_Url.ReleaseBuffer();
		nCefMain.get()->SetBrowserSizeChange(this->GetSafeHwnd());
		//nCefMain.get()->ChangeUrl(this->GetSafeHwnd(),nUrl.c_str());
		::SetFocus(this->GetSafeHwnd());
		nCefMain.get()->SetFocusToBrowser(this->GetSafeHwnd());
	}
	return 0;
}
int CandcefCtrl::CefSizeChange(){
	if (nCefMain.get()){
		nCefMain.get()->SetBrowserSizeChange(this->GetSafeHwnd());
	}
	return 0;
}
HWND CandcefCtrl::GetCefWnd(){
	HWND hAppWnd = NULL; 
	/*
	if (m_pInPlaceSite != NULL){ 
		m_pInPlaceSite->GetWindow(&hAppWnd);
		OutputDebugString(L"**********CandcefCtrl::GetWindow**********");
	}*/
	if (hAppWnd ==NULL){
		hAppWnd = this->GetSafeHwnd();
		OutputDebugString(L"**********CandcefCtrl::GetSafeHwnd**********");
	}
	return hAppWnd;
}
int CandcefCtrl::CefCreate(){
	OutputDebugString(L"CandcefCtrl::OnCreate");
	std::wstring nUrl = CefUrlReplace();

	if (!nCefMain.get()){
		OutputDebugString(L"CandcefCtrl::ShowMain create");
		nCefMain.reset(new CNdCefMain());
		nCefMain.get()->SetInstance(AfxGetInstanceHandle());
		nCefMain.get()->RunMain();
		
		if (m_bUrlShow){
			nCefMain.get()->ShowMainUrl(GetCefWnd(),nUrl.c_str());
		}else{
			nCefMain.get()->ShowMainUrl(GetCefWnd(),CefImageUrlPath().c_str());
		}

		nCefMain.get()->RegisterJS(&g_andcefCtrlRegisterJs);
	}else{
		OutputDebugString(L"CandcefCtrl::GetSafeHwnd");
		if (m_bUrlShow){
			nCefMain.get()->ShowMainUrl(GetCefWnd(),nUrl.c_str());
		}else{
			nCefMain.get()->ShowMainUrl(GetCefWnd(),CefImageUrlPath().c_str());
		}
	}
	return 0;
}

void CandcefCtrl::CefEventCall(LPCTSTR eventName, LPCTSTR eventParam){
	cefEvent(eventName,eventParam);
}
VARIANT_BOOL CandcefCtrl::SendCallback2H5(LPCTSTR eventName, LPCTSTR json)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	OutputDebugString(L"-------------SendCallback2H5---------------------------");
	OutputDebugString(eventName);
	OutputDebugString(json);
	if (nCefMain.get()){
		nCefMain->ExecuteJSCallBack(GetCefWnd(),eventName,json);
		OutputDebugString(L"-------------nCefMain->ExecuteJSCallBack---------------------------");
	}
	return VARIANT_TRUE;
}
LRESULT CALLBACK CandcefCtrl::GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	LPMSG lpMsg = (LPMSG) lParam;
	switch(lpMsg->message){
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONUP:
			{
				if (!CandcefCtrl::m_nPPTBlank&&CandcefCtrl::m_nPPTICR){
					lpMsg->message = WM_NULL;
					lpMsg->lParam = 0L;
					lpMsg->wParam = 0;
				}
			}
			break;
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			{
				if (!CandcefCtrl::m_nPPTBlank&&CandcefCtrl::m_nPPTICR){
					lpMsg->message = WM_NULL;
					lpMsg->lParam = 0L;
					lpMsg->wParam = 0;
				}
			}
			break;
		case WM_KEYUP:
		case WM_KEYDOWN:
			{
				switch(lpMsg->wParam){
				case VK_ESCAPE:
					{
						if (CandcefCtrl::m_nPPTICR){
							/*
							HWND _hwnd = ::FindWindow(_T("NdCefOSRWindowsClass"),NULL);
							if (::IsWindow(_hwnd)){

							}else{
								//HWND _fhwnd = ::GetFocus();
								//HWND _phwnd = ::GetParent(CandcefCtrl::m_oldhAppWnd);
								//if (_fhwnd==_phwnd||_fhwnd==CandcefCtrl::m_oldhAppWnd){
								//}
							}
							SetCursor (LoadCursor (NULL, IDC_ARROW));
							*/
							if (CandcefCtrl::m_nPPTOSRESC>0){
								CandcefCtrl::m_nPPTOSRESC-=1;
							}else{
								CandcefCtrl::m_nPPTOSRESC= 0;
								HWND _hwnd = ::FindWindow(_T("NdCefOSRWindowsClass"),NULL);
								if (::IsWindow(_hwnd)){
									CandcefCtrl::m_nPPTOSRESC = 2;
								}else{
									if (nCefMain.get()){
										nCefMain->SendEsc2PPT();
										//HWND _phwnd = ::GetParent(CandcefCtrl::m_oldhAppWnd);
										::SetForegroundWindow(CandcefCtrl::m_oldhAppWnd);
									}
								}

							}
							lpMsg->message = WM_NULL;
							lpMsg->lParam = 0L;
							lpMsg->wParam = 0;
						}
					}
					break;
				case VK_BACK:
				case VK_DOWN:
				case VK_UP:
				case VK_LEFT:
				case VK_RIGHT:
				case VK_CONTROL:
					if (!CandcefCtrl::m_nPPTBlank&&CandcefCtrl::m_nPPTICR){
						lpMsg->message = WM_NULL;
						lpMsg->lParam = 0L;
						lpMsg->wParam = 0;
					}
					break;
				}
			}
			break;
	}
	return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
}
LRESULT CALLBACK CandcefCtrl::GetMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	LPMSG lpMsg = (LPMSG) lParam;
	switch(lpMsg->message){
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		case WM_MOUSEACTIVATE:
			{
				if (!CandcefCtrl::m_nPPTBlank&&CandcefCtrl::m_nPPTICR){
					lpMsg->message = WM_NULL;
					lpMsg->lParam = 0L;
					lpMsg->wParam = 0;
				}
			}
			break;
	}
	return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

void CandcefCtrl::SetPPTProc()
{
	if (AmbientUserMode()){
		HWND hAppWnd = NULL; 
		CandcefCtrl::m_nPPTBlank = false;
		if (m_pInPlaceSite != NULL){ 
			m_pInPlaceSite->GetWindow(&hAppWnd);
			OutputDebugString(L"**********CandcefCtrl::GetWindow**********");
		}
		if (hAppWnd !=NULL){
			if (m_oldhAppWnd!=hAppWnd){
				m_oldhAppWnd = hAppWnd;
			}
			if (m_hHook==NULL){
				m_hHook1 = ::SetWindowsHookEx(WH_GETMESSAGE, GetMouseProc, 
					AfxGetInstanceHandle(),0);
				m_hHook = ::SetWindowsHookEx(WH_GETMESSAGE, GetMessageProc, 
					AfxGetInstanceHandle(),GetCurrentThreadId());//
			}
		}
	}
}

void CandcefCtrl::SetFocus(LONG nIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OutputDebugString(L"-------------SetFocus---------------------------");
	if (nIndex==-1){
		CefFocus();
	}else{
		CefShow();
		SetPPTProc();
	}
	OutputDebugString(L"-------------SetFocus End---------------------------");
	ClearMenory();
	//
}

void CandcefCtrl::SetSlideIndex(LONG nIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OutputDebugString(L"-------------SetSlideIndex---------------------------");
	m_nSlideIndex = nIndex;
	if (nCefMain.get()){
		nCefMain->SetSlideIndex(GetCefWnd(),nIndex);
	}
	OutputDebugString(L"-------------SetSlideIndex End---------------------------");
	// TODO: Add your dispatch handler code here
}
void CandcefCtrl::OnDestroy()
{
	::KillTimer(m_hCefpWnd,1);
	if (m_oldhAppWnd!=NULL){
		::UnhookWindowsHookEx(m_hHook1);
		::UnhookWindowsHookEx(m_hHook);
		m_hHook1 = NULL;
		m_hHook=NULL;
		//SetWindowLongPtr(m_oldhAppWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
		//m_oldWndProc = NULL;
	}
	COleControl::OnDestroy();

	// TODO: Add your message handler code here
}
void CandcefCtrl::CefRegisterClass(){
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS ;
	wcex.lpfnWndProc	= CefWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= AfxGetInstanceHandle();
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("CEFOCX");
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
}
void CandcefCtrl::CefCreateWnd(){
	OutputDebugString(L"-------------CefCreateWnd---------------------------");
	if (m_hCefpWnd==NULL){
		CefRegisterClass();
		OutputDebugString(L"-------------CefRegisterClass---------------------------");
		m_hCefpWnd = CreateWindow(_T("CEFOCX"), _T(""), WS_POPUP,  0,0,0,0 , NULL , NULL, NULL, NULL);
	}
}

LRESULT CALLBACK CandcefCtrl::CefWndProc( HWND hwnd, /* handle to window */ UINT uMsg, /* message identifier */ WPARAM wParam, /* first message parameter */ LPARAM lParam /* second message parameter */ )
{
	switch(uMsg)
	{
	case MSG_CEFOCX_PAGECHANGE:
		if (nCefMain.get()){
			CandcefCtrl::m_nPPTBlank = true;
			nCefMain.get()->SetFocusToBrowser(NULL);
		}
		break;
	case MSG_CEFOCX_ICR:
		CandcefCtrl::m_nPPTICR = true;
		break;
	case MSG_CEFOSR_ESC:
		CandcefCtrl::m_nPPTOSRESC = 2;
		break;
	case WM_TIMER:
		::KillTimer(m_hCefpWnd,wParam);
		if (m_nCefCtrl){
			m_nCefCtrl->Refresh();
			m_nCefCtrl = NULL;
		}
		OutputDebugString(L"**********CandcefCtrl::WM_TIMER**********");
		break;
	}
	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}
