#ifndef XDELTA_WRAPPER_H
#define XDELTA_WRAPPER_H

#include <windows.h>

class CXdeltaWrapper
{
public:
	static BOOL CreatePatch(LPCTSTR pszSrcFile,LPCTSTR pszDesFile,LPCTSTR pszPatchFile);
	static BOOL ApplyPatch(LPCTSTR pszSrcFile,LPCTSTR pszPatchFile,LPCTSTR pszDesFile);
protected:
	CXdeltaWrapper();
};

#endif