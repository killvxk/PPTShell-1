#include "XdeltaWrapper.h"

#define NOT_MAIN 1
#define XD3_WIN32 1
#define EXTERNAL_COMPRESSION 0

#include "xdelta3.c"

CXdeltaWrapper::CXdeltaWrapper()
{

}

BOOL CXdeltaWrapper::CreatePatch(LPCTSTR pszSrcFile,LPCTSTR pszDesFile,LPCTSTR pszPatchFile)
{
	int argc = 7;
	char* argv[8] = {"xdelta","-f","-e","-s",(char*)pszSrcFile,(char*)pszDesFile,(char*)pszPatchFile,"\0"};
	return EXIT_SUCCESS==xd3_main_cmdline(argc,argv);
}

BOOL CXdeltaWrapper::ApplyPatch(LPCTSTR pszSrcFile,LPCTSTR pszPatchFile,LPCTSTR pszDesFile)
{
	if(0==strcmp(pszSrcFile,pszDesFile))
	{
		char szTempFile[512]={0};
		GetCurrentDirectory(sizeof(szTempFile)-1,szTempFile);
		GetTempFileName(szTempFile,"temp",0,szTempFile);

		int argc = 7;
		char* argv[8]={"xdelta","-f","-d","-s",(char*)pszSrcFile,(char*)pszPatchFile,(char*)szTempFile,"\0"};
		if(EXIT_SUCCESS==xd3_main_cmdline(argc,argv) && TRUE==MoveFileEx(szTempFile,pszDesFile,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
		{
			return TRUE;
		}
		else
		{
			DeleteFile(szTempFile);
			return FALSE;
		}
	}
	else
	{
		int argc = 7;
		char* argv[8]={"xdelta","-f","-d","-s",(char*)pszSrcFile,(char*)pszPatchFile,(char*)pszDesFile,"\0"};
        return EXIT_SUCCESS==xd3_main_cmdline(argc,argv);
	}
}