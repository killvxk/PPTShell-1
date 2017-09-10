// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

//#import "E:\\PPTShellÕýÊ½°æ\\bin\\Debug\\bin\\cefocx\\andcef.ocx" no_namespace
// CDandcef wrapper class

class CDandcef : public COleDispatchDriver
{
public:
	CDandcef(){} // Calls COleDispatchDriver default constructor
	CDandcef(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CDandcef(const CDandcef& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// _Dandcef methods
public:
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL SendCallback2H5(LPCTSTR eventName, LPCTSTR json)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, eventName, json);
		return result;
	}
	void SetFocus(long nIndex)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nIndex);
	}
	void SetSlideIndex(long nIndex)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nIndex);
	}
	// _Dandcef properties
public:
	CString GetUrl()
	{
		CString result;
		GetProperty(0x1, VT_BSTR, (void*)&result);
		return result;
	}
	void SetUrl(CString propVal)
	{
		SetProperty(0x1, VT_BSTR, propVal);
	}
	CString GetimgUrl()
	{
		CString result;
		GetProperty(0x6, VT_BSTR, (void*)&result);
		return result;
	}
	void SetimgUrl(CString propVal)
	{
		SetProperty(0x6, VT_BSTR, propVal);
	}
};
