#pragma once

class CSyncInternetTime
{
public:
	CSyncInternetTime(void);
	~CSyncInternetTime(void);
	BOOL SyncSystemClock(CTime tmServer);
	BOOL GetInternetTime(CTime* pTime, CString strServer);
	void SyncSystime();
};
