// 101PPTDaemon.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "101PPTService.h"
#include "101PPTServiceCtrl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc > 1)
	{
		C101PPTService svc;
		wstring nCommand(argv[1]);

		if (nCommand == TEXT("-i"))
		{
			svc.CreateSvc();	
			svc.StartSvc();			
		}
		else if (nCommand == TEXT("-u"))
		{
			svc.StopSvc();
			svc.DeleteSvc();			
		}
		else if (nCommand == TEXT("-start"))
		{
			svc.StartSvc();			
		}
		else if (nCommand == TEXT("-stop"))
		{
			svc.StopSvc();			
		}
		return 0;
	}
	C101PPTServiceCtrl::ServiceCtrlDispatcher();
	return 0;
}

void SvcDebugOut(LPSTR String, DWORD Status) 
{ 
   CHAR  Buffer[1024]; 
   if (strlen(String) < 1000) 
   { 
	  printf(String, Status); 
      sprintf(Buffer, String, Status); 
      OutputDebugStringA(Buffer); 
   } 
}
void SvcDebugOutFile(std::wstring nFilePath,LPSTR String, DWORD Status) 
{ 
	CHAR  Buffer[1024]; 
	if (strlen(String) < 1000) 
	{ 
		printf(String, Status); 
		sprintf(Buffer, String, Status); 
		__time64_t long_time;
		_time64( &long_time );
		tm local = {0}; 
		localtime_s( &local, &long_time );
		char szTime[32] = {0};
		sprintf_s(szTime, "%02d/%02d %02d:%02d:%02d ", local.tm_mon+1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
		
		string strBuffer = "";
		strBuffer += szTime;
		strBuffer += Buffer;
		strBuffer += "\n";

		FILE* file = NULL;
		file = _wfopen(nFilePath.c_str(), L"a");
		if (file == NULL)
		{
			return;
		}
		//fprintf(file, strBuffer.c_str());
		fwrite(strBuffer.c_str(), strBuffer.length(), 1, file);
		fclose(file);
		OutputDebugStringA(Buffer); 
	} 
}
