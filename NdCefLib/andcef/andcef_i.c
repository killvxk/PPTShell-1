

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_andcefLib,0xBC25CF1D,0xD20F,0x4160,0xAC,0x3B,0x90,0x8F,0x83,0x83,0x85,0x3A);


MIDL_DEFINE_GUID(IID, DIID__Dandcef,0xB495C91F,0x46CA,0x48AC,0x83,0x00,0x78,0x09,0xED,0xAA,0x38,0xCC);


MIDL_DEFINE_GUID(IID, DIID__DandcefEvents,0xD239E01D,0x4187,0x49E9,0x84,0x0E,0xD0,0x8C,0x5C,0x2F,0xB2,0x7B);


MIDL_DEFINE_GUID(CLSID, CLSID_andcef,0xEBE03648,0xD0A0,0x456A,0x86,0x49,0xE7,0xB9,0xE8,0xA1,0x99,0xC1);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



