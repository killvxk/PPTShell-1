#pragma once


class C101PPTService
{
public:
	C101PPTService(void);
	~C101PPTService(void);
public:
	BOOL CreateSvc();         ///<创建Service
	BOOL DeleteSvc();         ///<删除Service
	BOOL StartSvc();          ///<启动Service
	BOOL StopSvc();           ///<停止Service
private:
	void CloseSvcHandles();      ///<清除句柄
	/** \brief 停止Service
	*
	* @param hSCM - Handle to the service control manager.
	* @param hService - Handle to the service to be stopped.
	* @param fStopDependencies - Indicates whether to stop dependent services.
	* @param dwTimeout - maximum time (in milliseconds) to wait
	*
	* @return If the operation is successful, returns ERROR_SUCCESS. Otherwise, returns a system error code.
	*/
	DWORD StopService( SC_HANDLE hSCM, SC_HANDLE hService, BOOL fStopDependencies, DWORD dwTimeout );
private:
	SC_HANDLE schSCManager;   ///<NT服务管理句柄
	SC_HANDLE schService;     ///<NT服务句柄
};
