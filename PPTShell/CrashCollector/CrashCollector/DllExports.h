#ifndef DLL_EXPORTS_H
#define DLL_EXPORTS_H

#include "StdAfx.h"
#include "Common/MyInitGuid.h"
#include "Common/ComTry.h"
#include "Windows/PropVariant.h"

STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject);

STDAPI SetLargePageMode();

STDAPI GetNumberOfFormats(UINT32 *numFormats);

STDAPI GetHandlerProperty(PROPID propID, PROPVARIANT *value);
STDAPI GetHandlerProperty2(UInt32 formatIndex, PROPID propID, PROPVARIANT *value);

bool IsItWindowsNT();
extern bool g_IsNT;
#endif