

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Mar 18 08:42:56 2016
 */
/* Compiler settings for .\.\andcef.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __andcefidl_h__
#define __andcefidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___Dandcef_FWD_DEFINED__
#define ___Dandcef_FWD_DEFINED__
typedef interface _Dandcef _Dandcef;
#endif 	/* ___Dandcef_FWD_DEFINED__ */


#ifndef ___DandcefEvents_FWD_DEFINED__
#define ___DandcefEvents_FWD_DEFINED__
typedef interface _DandcefEvents _DandcefEvents;
#endif 	/* ___DandcefEvents_FWD_DEFINED__ */


#ifndef __andcef_FWD_DEFINED__
#define __andcef_FWD_DEFINED__

#ifdef __cplusplus
typedef class andcef andcef;
#else
typedef struct andcef andcef;
#endif /* __cplusplus */

#endif 	/* __andcef_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __andcefLib_LIBRARY_DEFINED__
#define __andcefLib_LIBRARY_DEFINED__

/* library andcefLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_andcefLib;

#ifndef ___Dandcef_DISPINTERFACE_DEFINED__
#define ___Dandcef_DISPINTERFACE_DEFINED__

/* dispinterface _Dandcef */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__Dandcef;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B495C91F-46CA-48AC-8300-7809EDAA38CC")
    _Dandcef : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DandcefVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _Dandcef * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _Dandcef * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _Dandcef * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _Dandcef * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _Dandcef * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _Dandcef * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _Dandcef * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DandcefVtbl;

    interface _Dandcef
    {
        CONST_VTBL struct _DandcefVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _Dandcef_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _Dandcef_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _Dandcef_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _Dandcef_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _Dandcef_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _Dandcef_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _Dandcef_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___Dandcef_DISPINTERFACE_DEFINED__ */


#ifndef ___DandcefEvents_DISPINTERFACE_DEFINED__
#define ___DandcefEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DandcefEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DandcefEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D239E01D-4187-49E9-840E-D08C5C2FB27B")
    _DandcefEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DandcefEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DandcefEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DandcefEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DandcefEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DandcefEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DandcefEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DandcefEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DandcefEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DandcefEventsVtbl;

    interface _DandcefEvents
    {
        CONST_VTBL struct _DandcefEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DandcefEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DandcefEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DandcefEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DandcefEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DandcefEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DandcefEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DandcefEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DandcefEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_andcef;

#ifdef __cplusplus

class DECLSPEC_UUID("EBE03648-D0A0-456A-8649-E7B9E8A199C1")
andcef;
#endif
#endif /* __andcefLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


