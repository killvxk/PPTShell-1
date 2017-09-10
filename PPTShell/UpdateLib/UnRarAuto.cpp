// UnRarAutoitWrapper.cpp : Defines the entry point for the DLL application.
//

#include "UnRarAuto.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

extern "C" __declspec(dllexport) void ExtractArchive(char *ArcName,int Mode,char *Password)
{
    HANDLE hArcData;
    int RHCode,PFCode;
    char CmtBuf[16384];
    struct RARHeaderData HeaderData;
    struct RAROpenArchiveDataEx OpenArchiveData;
	
    memset(&OpenArchiveData,0,sizeof(OpenArchiveData));
    OpenArchiveData.ArcName=ArcName;
    OpenArchiveData.CmtBuf=CmtBuf;
    OpenArchiveData.CmtBufSize=sizeof(CmtBuf);
    OpenArchiveData.OpenMode=RAR_OM_EXTRACT;
    hArcData=RAROpenArchiveEx(&OpenArchiveData);
    
    if(strlen(Password)) RARSetPassword(hArcData,Password);
	
	if (OpenArchiveData.OpenResult!=0)
	{
		OutOpenArchiveError(OpenArchiveData.OpenResult,ArcName);
		return;
	}
	
	ShowArcInfo(OpenArchiveData.Flags,ArcName);
	
	if (OpenArchiveData.CmtState==1)
		ShowComment(CmtBuf);
	
	RARSetCallback(hArcData,CallbackProc,(LONG)&Mode);
	
	HeaderData.CmtBuf=NULL;
	
	while ((RHCode=RARReadHeader(hArcData,&HeaderData))==0)
	{
		switch(Mode)
		{
		case EXTRACT:
			//printf("\nExtracting %-45s",HeaderData.FileName);
			break;
		case TEST:
			//printf("\nTesting %-45s",HeaderData.FileName);
			break;
		case PRINT:
			//printf("\nPrinting %-45s\n",HeaderData.FileName);
			break;
		}
		PFCode=RARProcessFile(hArcData,(Mode==EXTRACT) ? RAR_EXTRACT:RAR_TEST,
			NULL,NULL);
		if (PFCode==0)
		{
			//printf(" Ok");
		}
		else
		{
			OutProcessFileError(PFCode);
			break;
		}
	}
	
	if (RHCode==ERAR_BAD_DATA)
		//printf("\nFile header broken");
		
		RARCloseArchive(hArcData);
}


extern "C" __declspec(dllexport) int ListArchive(char *ArcName)
{
	HANDLE hArcData;
	int RHCode,PFCode;
	char CmtBuf[16384];
	struct RARHeaderDataEx HeaderData;
	struct RAROpenArchiveDataEx OpenArchiveData;
    
	////printf("Livewire\n");
	
	memset(&OpenArchiveData,0,sizeof(OpenArchiveData));
	OpenArchiveData.ArcName=ArcName;
	OpenArchiveData.CmtBuf=CmtBuf;
	OpenArchiveData.CmtBufSize=sizeof(CmtBuf);
	OpenArchiveData.OpenMode=RAR_OM_LIST;
	hArcData=RAROpenArchiveEx(&OpenArchiveData);
	
	if (OpenArchiveData.OpenResult!=0)
	{
		OutOpenArchiveError(OpenArchiveData.OpenResult,ArcName);
		return -1;
	}
	
	ShowArcInfo(OpenArchiveData.Flags,ArcName);
	
	if (OpenArchiveData.CmtState==1)
		ShowComment(CmtBuf);
	
	RARSetCallback(hArcData,CallbackProc,0);
	
	HeaderData.CmtBuf=CmtBuf;
	HeaderData.CmtBufSize=sizeof(CmtBuf);
	
	//printf("\nFile                     Size");
	//printf("\n-------------------------------");
	while ((RHCode=RARReadHeaderEx(hArcData,&HeaderData))==0)
	{
		__int64 UnpSize=HeaderData.UnpSize+(((__int64)HeaderData.UnpSizeHigh)<<32);
		//printf("\n%-20s %10Ld ",HeaderData.FileName,UnpSize);
		if (HeaderData.CmtState==1)
			ShowComment(CmtBuf);
		if ((PFCode=RARProcessFile(hArcData,RAR_SKIP,NULL,NULL))!=0)
		{
			OutProcessFileError(PFCode);
			break;
		}
	}
	
	if (RHCode==ERAR_BAD_DATA)
		//printf("\nFile header broken");
		
		RARCloseArchive(hArcData);
	return 1;
}


void ShowComment(char *CmtBuf)
{
    //MessageBoxA(NULL, _T(CmtBuf), _T("Comment:"), MB_OK);
    MessageBoxA(NULL, CmtBuf, "Comment:", MB_OK);
    ////printf("\nComment:\n%s\n",CmtBuf);
}


extern "C" __declspec(dllexport) void OutHelp(void)
{
    //MessageBox(NULL, _T("This is a message box test!"), _T("Text:"), MB_OK);
	
    //printf("\nUNRDLL.   This is a simple example of UNRAR.DLL usage\n");
    //printf("\nSyntax:\n");
    //printf("\nUNRDLL X <Archive>   extract archive contents");
    //printf("\nUNRDLL T <Archive>   test archive contents");
    //printf("\nUNRDLL P <Archive>   print archive contents to stdout");
    //printf("\nUNRDLL L <Archive>   view archive contents\n");
}


void OutOpenArchiveError(int Error,char *ArcName)
{
    switch(Error)
    {
	case ERAR_NO_MEMORY:
		MessageBoxA(NULL, "Not enough memory", ArcName, MB_OK);
		break;
	case ERAR_EOPEN:
		MessageBoxA(NULL, "Cannot open archive", ArcName, MB_OK);
		break;
	case ERAR_BAD_ARCHIVE:
		MessageBoxA(NULL, "Not RAR archive", ArcName, MB_OK);
		break;
	case ERAR_BAD_DATA:
		MessageBoxA(NULL, "Archive header broken", ArcName, MB_OK);
		break;
	case ERAR_UNKNOWN:
		MessageBoxA(NULL, "Unknown error", ArcName, MB_OK);
		break;
    }
}


void ShowArcInfo(unsigned int Flags,char *ArcName)
{
    //printf("\nArchive %s\n",ArcName);
    //printf("\nVolume:\t\t%s",(Flags & 1) ? "yes":"no");
    //printf("\nComment:\t%s",(Flags & 2) ? "yes":"no");
    //printf("\nLocked:\t\t%s",(Flags & 4) ? "yes":"no");
    //printf("\nSolid:\t\t%s",(Flags & 8) ? "yes":"no");
    //printf("\nNew naming:\t%s",(Flags & 16) ? "yes":"no");
    //printf("\nAuthenticity:\t%s",(Flags & 32) ? "yes":"no");
    //printf("\nRecovery:\t%s",(Flags & 64) ? "yes":"no");
    //printf("\nEncr.headers:\t%s",(Flags & 128) ? "yes":"no");
    //printf("\nFirst volume:\t%s",(Flags & 256) ? "yes":"no or older than 3.0");
    //printf("\n---------------------------\n");
}


void OutProcessFileError(int Error)
{
    switch(Error)
    {
	case ERAR_UNKNOWN_FORMAT:
		MessageBoxA(NULL, "Unknown archive format", "OutProcessFileError", MB_OK);
		break;
	case ERAR_BAD_ARCHIVE:
		MessageBoxA(NULL, "Bad volume", "OutProcessFileError", MB_OK);
		break;
	case ERAR_ECREATE:
		MessageBoxA(NULL, "File create error", "OutProcessFileError", MB_OK);
		break;
	case ERAR_EOPEN:
		MessageBoxA(NULL, "Volume open error", "OutProcessFileError", MB_OK);
		break;
	case ERAR_ECLOSE:
		MessageBoxA(NULL, "File close error", "OutProcessFileError", MB_OK);
		break;
	case ERAR_EREAD:
		MessageBoxA(NULL, "Read error", "OutProcessFileError", MB_OK);
		break;
	case ERAR_EWRITE:
		MessageBoxA(NULL, "Write error", "OutProcessFileError", MB_OK);
		break;
	case ERAR_BAD_DATA:
		MessageBoxA(NULL, "CRC error", "OutProcessFileError", MB_OK);
		break;
	case ERAR_UNKNOWN:
		MessageBoxA(NULL, "Unknown error", "OutProcessFileError", MB_OK);
		break;
    }
}

int CALLBACK CallbackProc(UINT msg,LONG UserData,LONG P1,LONG P2)
{
	switch(msg)
	{
    case UCM_CHANGEVOLUME:
		if (P2==RAR_VOL_ASK)
		{
			//printf("\n\nVolume %s is required\nPossible options:\n",(char *)P1);
			//printf("\nEnter - try again");
			//printf("\n'R'   - specify a new volume name");
			//printf("\n'Q'   - quit");
			//printf("\nEnter your choice: ");
			switch(toupper(getchar()))
			{
			case 'Q':
				return(-1);
			case 'R':
				{
					char *eol;
					//printf("\nEnter new name: ");
					fflush(stdin);
					fgets((char *)P1,MAX_PATH,stdin);
					eol=strpbrk((char *)P1,"\r\n");
					if (eol!=NULL)
						*eol=0;
				}
				return(0);
			default:
				return(0);
			}
		}
		if (P2==RAR_VOL_NOTIFY)
			//printf("\n  ... volume %s\n",(char *)P1);
			return(0);
    case UCM_PROCESSDATA:
		if (UserData!=0 && *(int *)UserData==PRINT)
		{
			fflush(stdout);
			fwrite((char *)P1,1,P2,stdout);
			fflush(stdout);
		}
		return(0);
    case UCM_NEEDPASSWORD:
		//printf("\nPassword required: ");
		gets((char *)P1);
		return(0);
	}
	return(0);
}
