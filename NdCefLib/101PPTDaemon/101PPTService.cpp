#include "StdAfx.h"
#include "101PPTService.h"

C101PPTService::C101PPTService(void)
{
	schService=NULL;
	schSCManager=NULL;
}

C101PPTService::~C101PPTService(void)
{
	CloseSvcHandles();
}
void C101PPTService::CloseSvcHandles()
{
	if(schService)
		CloseServiceHandle(schService);
	if(schSCManager)
		CloseServiceHandle(schSCManager);
	schService=NULL;
	schSCManager=NULL;
}
BOOL C101PPTService::CreateSvc()
{
	CloseSvcHandles();

	TCHAR szPath[MAX_PATH]; 
    
    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        SvcDebugOut("GetModuleFileName failed (%d)\n", GetLastError()); 
        return FALSE;
    }

	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		SvcDebugOut("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}
	schService = CreateService(
		schSCManager,				// SCManager database
		TEXT("101PPTDaemon"),		// name of service
		TEXT("101PPTDaemon"),		// service name to display
		SERVICE_ALL_ACCESS,			// desired access
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,	// service type
		SERVICE_AUTO_START,			// start type
		SERVICE_ERROR_NORMAL,		// error control type
		szPath,			// service's binary
		NULL,						// no load ordering group
		NULL,						// no tag identifier
		NULL,						// no dependencies
		NULL,						// LocalSystem account
		NULL);						// no password

    if (schService == NULL) 
    {
        SvcDebugOut("CreateService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
    else
    {
		 SvcDebugOut("CreateService Successfully (%d)\n",0);
    }
	return TRUE;
}
BOOL C101PPTService::DeleteSvc() 
{ 
	CloseSvcHandles();
	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		SvcDebugOut("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

    schService = OpenService( 
        schSCManager,       // SCManager database 
        TEXT("101PPTDaemon"), // name of service 
        DELETE);            // only need DELETE access 
 
    if (schService == NULL)
    { 
        SvcDebugOut("OpenService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
 
    if (! DeleteService(schService) ) 
    {
        SvcDebugOut("DeleteService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
    else 
        SvcDebugOut("DeleteService succeeded\n",0); 
	CloseSvcHandles();
    return TRUE;
}

BOOL C101PPTService::StartSvc() 
{ 
    SC_HANDLE schService;
    SERVICE_STATUS_PROCESS ssStatus; 
    DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
 
	CloseSvcHandles();
	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		SvcDebugOut("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

    schService = OpenService( 
        schSCManager,          // SCM database 
        TEXT("101PPTDaemon"),          // service name
        SERVICE_ALL_ACCESS); 
 
    if (schService == NULL) 
    { 
        return 0; 
    }
 
    if (!StartService(
            schService,  // handle to service 
            0,           // number of arguments 
            NULL) )      // no arguments 
    {
        return 0; 
    }
    else 
    {
        SvcDebugOut("Service start pending.\n",0); 
    }
 
    // Check the status until the service is no longer start pending. 
 
    if (!QueryServiceStatusEx( 
            schService,             // handle to service 
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE)&ssStatus,              // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
    {
        return 0; 
    }
	 
    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
    { 
        // Do not wait longer than the wait hint. A good interval is 
        // one tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 
 
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if( dwWaitTime < 1000 )
            dwWaitTime = 1000;
        else if ( dwWaitTime > 10000 )
            dwWaitTime = 10000;

        Sleep( dwWaitTime );

        // Check the status again. 
 
    if (!QueryServiceStatusEx( 
            schService,             // handle to service 
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE)&ssStatus,              // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
            break; 
 
        if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
        {
            // The service is making progress.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint
                break;
            }
        }
    } 

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
    {
        SvcDebugOut("StartService SUCCESS.\n",0); 
        return 1;
    }
    else 
    { 
        SvcDebugOut("\nService not started. \n",0);
        SvcDebugOut("  Current State: %d\n", ssStatus.dwCurrentState); 
        SvcDebugOut("  Exit Code: %d\n", ssStatus.dwWin32ExitCode); 
        SvcDebugOut("  Service Specific Exit Code: %d\n", 
            ssStatus.dwServiceSpecificExitCode); 
        SvcDebugOut("  Check Point: %d\n", ssStatus.dwCheckPoint); 
        SvcDebugOut("  Wait Hint: %d\n", ssStatus.dwWaitHint); 
        return 0;
    } 
	return TRUE;
}

BOOL C101PPTService::StopSvc() 
{ 
	CloseSvcHandles();
	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		SvcDebugOut("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

    schService = OpenService( 
        schSCManager,          // SCM database 
        TEXT("101PPTDaemon"),          // service name
        SERVICE_ALL_ACCESS); 
 
    if (schService == NULL) 
    { 
        return 0; 
    }
	//DWORD  nCode = StopService(schSCManager, schService, TRUE, 30000);
	BOOL fStopDependencies = TRUE;
	DWORD dwTimeout = 30000;
	
   SERVICE_STATUS_PROCESS ss;
   DWORD dwStartTime = GetTickCount();
   DWORD dwBytesNeeded;

   // Make sure the service is not already stopped
   if ( !QueryServiceStatusEx( 
             schService, 
             SC_STATUS_PROCESS_INFO,
             (LPBYTE)&ss, 
             sizeof(SERVICE_STATUS_PROCESS),
             &dwBytesNeeded ) )
      return GetLastError();

   if ( ss.dwCurrentState == SERVICE_STOPPED ) 
      return ERROR_SUCCESS;
   // If a stop is pending, just wait for it
   while ( ss.dwCurrentState == SERVICE_STOP_PENDING ) 
   {
      Sleep( ss.dwWaitHint );
   if ( !QueryServiceStatusEx( 
             schService, 
             SC_STATUS_PROCESS_INFO,
             (LPBYTE)&ss, 
             sizeof(SERVICE_STATUS_PROCESS),
             &dwBytesNeeded ) )
         return GetLastError();

      if ( ss.dwCurrentState == SERVICE_STOPPED )
         return ERROR_SUCCESS;

      if ( GetTickCount() - dwStartTime > dwTimeout )
         return ERROR_TIMEOUT;
   }

   // If the service is running, dependencies must be stopped first
   if ( fStopDependencies ) 
   {
      DWORD i;
      DWORD dwBytesNeeded;
      DWORD dwCount;

      LPENUM_SERVICE_STATUS   lpDependencies = NULL;
      ENUM_SERVICE_STATUS     ess;
      SC_HANDLE               hDepService;

      // Pass a zero-length buffer to get the required buffer size
      if ( EnumDependentServices( schService, SERVICE_ACTIVE, 
         lpDependencies, 0, &dwBytesNeeded, &dwCount ) ) 
      {
         // If the Enum call succeeds, then there are no dependent
         // services so do nothing
      } 
      else 
      {
         if ( GetLastError() != ERROR_MORE_DATA )
            return GetLastError(); // Unexpected error

         // Allocate a buffer for the dependencies
         lpDependencies = (LPENUM_SERVICE_STATUS) HeapAlloc( 
               GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded );

         if ( !lpDependencies )
            return GetLastError();

         __try {
            // Enumerate the dependencies
            if ( !EnumDependentServices( schService, SERVICE_ACTIVE, 
                  lpDependencies, dwBytesNeeded, &dwBytesNeeded,
                  &dwCount ) )
               return GetLastError();

            for ( i = 0; i < dwCount; i++ ) 
            {
               ess = *(lpDependencies + i);

               // Open the service
               hDepService = OpenService( schSCManager, ess.lpServiceName, 
                     SERVICE_STOP | SERVICE_QUERY_STATUS );
               if ( !hDepService )
                  return GetLastError();

               __try {
                   // Send a stop code
                  if ( !ControlService( hDepService, 
                           SERVICE_CONTROL_STOP,
                           (LPSERVICE_STATUS)&ss ) )
                     return GetLastError();

                  // Wait for the service to stop
                  while ( ss.dwCurrentState != SERVICE_STOPPED ) 
                  {
                     Sleep( ss.dwWaitHint );
                     if ( !QueryServiceStatusEx( 
                              hDepService, 
                              SC_STATUS_PROCESS_INFO,
                              (LPBYTE)&ss, 
                              sizeof(SERVICE_STATUS_PROCESS),
                              &dwBytesNeeded ) )
                        return GetLastError();

                     if ( ss.dwCurrentState == SERVICE_STOPPED )
                        break;

                     if ( GetTickCount() - dwStartTime > dwTimeout )
                        return ERROR_TIMEOUT;
                  }

               } 
               __finally 
               {
                  // Always release the service handle
                  CloseServiceHandle( hDepService );

               }
            }

         } 
         __finally 
         {
            // Always free the enumeration buffer
            HeapFree( GetProcessHeap(), 0, lpDependencies );
         }
      } 
   }

   // Send a stop code to the main service
   if ( !ControlService( schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ss ) )
      return GetLastError();

   // Wait for the service to stop
   while ( ss.dwCurrentState != SERVICE_STOPPED ) 
   {
      Sleep( ss.dwWaitHint );
      if ( !QueryServiceStatusEx( 
               schService, 
               SC_STATUS_PROCESS_INFO,
               (LPBYTE)&ss, 
               sizeof(SERVICE_STATUS_PROCESS),
               &dwBytesNeeded ) )
         return GetLastError();

      if ( ss.dwCurrentState == SERVICE_STOPPED )
         break;

      if ( GetTickCount() - dwStartTime > dwTimeout )
         return ERROR_TIMEOUT;
   }

   // Return success
   return ERROR_SUCCESS;
}

