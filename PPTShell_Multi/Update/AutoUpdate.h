#ifndef _AUTOUPDATE_H_
#define _AUTOUPDATE_H_

#define WM_MYMESSAGE		(WM_USER+0x200)



enum E_ACTION
{
	e_act_dat
,	e_act_down
,	e_act_move
,	e_act_close
,	e_val_all
,	e_val_down
,   e_act_show
};

enum E_RET
{
	e_newest			//版本为最新
,	e_download_newest//版本不是最新但是已经下载了最新版本
,	e_cancel
,	e_download_success//下载最新版本完成,有管道
,   e_success//下载最新版本完成，无管道
,	e_f_dir
,	e_f_dat
,	e_f_down
,	e_f_mov
,	e_f_size

};

enum E_UPDATESTATEVALUE
{
	e_hasupdate,
	e_requestupdate,
	e_terminate
};

typedef struct _UPDATESTATE
{
	UCHAR type;
	bool value;
}UPDATESTATE,*PUPDATESTATE;


enum UpdateType
{
	eUpdateType_Has_Update = 1,
	eUpdateType_Newset,
	eUpdateType_Failed,
	eUpdateType_Request_Update,
	eUpdateType_Terminate,
	eUpdateType_Update_Self,
	eUpdateType_Update_Success,
	eUpdateType_Update_Processing,
};

typedef struct _PipeMessageHead
{
	short	wType;
	short	wSize;

	_PipeMessageHead()
	{
		wType		= -1;
		wSize		= sizeof(_PipeMessageHead);
	}

}PipeMessageHead,*PPipeMessageHead;

typedef struct _HasUpdate:PipeMessageHead
{
	bool	bValue;
	char	szVersion[32];
	char	szContent[MAX_PATH * 2];
	_HasUpdate()
	{
		wType		= eUpdateType_Has_Update;
		wSize		= sizeof(_HasUpdate);
	}

}HasUpdate,*PHasUpdate;

typedef struct _ReuqestUpdate:PipeMessageHead
{
	bool	bValue;
	_ReuqestUpdate()
	{
		wType		= eUpdateType_Request_Update;
		wSize		= sizeof(_ReuqestUpdate);
	}

}ReuqestUpdate,*PReuqestUpdate;


typedef struct _VersionStruct
{
	int aVersion[4];
	_VersionStruct()
	{
		for (int i = 0; i < 4; i++)
		{
			aVersion[i] = 0;
		}
	}
	int CompareTo(_VersionStruct* pVersion)
	{
		for (int i = 4 - 1; i >= 0; i--)
		{
			if (this->aVersion[ i ] > pVersion->aVersion[ i ])
			{
				return 1;
			}
			else if (this->aVersion[ i ] == pVersion->aVersion[ i ])
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		return 0;
	}

	bool	InitFromString(LPCSTR	lpStr)
	{
		if(sscanf_s(lpStr, "%d.%d.%d.%d", &aVersion[3
		], &aVersion[2], &aVersion[1], &aVersion[0]) == 4)
		return true;
		return false;
	}

}VersionStruct;

typedef struct _UpdateInfo_Param
{
	int				nType;
	int				nRet;
	DWORD			dwTotalProgress;
	DWORD			dwCurrentProgress;
	char			szFileName[MAX_PATH];
	float			fDownloadSpeed;
}UpdateInfo_Param,*pUpdateInfo_Param;

typedef struct _AutoUpdate_Param
{
	DWORD			dwDownloadIndex;
	DWORD			dwDownloadCount;
	DWORD			dwDownloadingSize;
	DWORD			dwDownloadFileSize;
	DWORD			dwPreTotalProgress;
	UpdateInfo_Param updateInfoParam;
}AutoUpdate_Param,*pAutoUpdate_Param;

unsigned __stdcall AutoUpdate(void* pParam);

#endif //_AUTOUPDATE_H_
