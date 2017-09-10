//===========================================================================
// FileName:				Statistics.h
// 
// Desc:				
//============================================================================
#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "Util/Singleton.h"
#include "Util/Stream.h"

enum
{
	//
	STAT_RUN								= 1001,
	STAT_RUNPC								= 1002,
	STAT_OPEN_PPT							= 1003,
	STAT_NEW_PPT							= 1004,
	STAT_INSTALL_PPTSHELL					= 1005,

	// Edit Relative Operation
	STAT_REMOVE_PALCE_HOLDER				= 1099,
	STAT_INSET_PALCE_HOLDER					= 1100,
	STAT_INSERT_PICTURE						= 1101,
	STAT_INSERT_VIDEO						= 1102,
	STAT_INSERT_VOLUME						= 1103,
	STAT_INSERT_FLASH						= 1104,
	STAT_INSERT_PPT							= 1105,
	STAT_INSERT_QUESTION					= 1106,
	STAT_INSERT_SLIDE						= 1107,
	STAT_INSERT_OTHER						= 1108,
	STAT_UNDO								= 1109,
	STAT_INSERT_3D							= 1110,
	STAT_INSERT_VR							= 1111,
	STAT_UPDATE_QUESTION					= 1112,
	STAT_INSERT_HYPERLINKPICTURE			= 1113,
	STAT_INSERT_UPDATE_QUESTION             = 1114,
	


	// Play Relative Operation
	STAT_PLAY_FULLSCREEN					= 1201,
	STAT_PLAY_EXIT							= 1202,
	STAT_EXEC_CALC							= 1203,
	STAT_EFFECT_HANDCLAP					= 1204,
	STAT_EFFECT_CAIDAI						= 1205,
	STAT_EFFECT_CAIXIE						= 1206,
	STAT_PEN								= 1207,
	STAT_ERASER								= 1208,
	STAT_CLEAR_INK							= 1209,
	STAT_ARRORW								= 1210,

	// Others
	STAT_PREIVIEW_PICUTURE					= 1301,
	STAT_FULLSCREEN_PREVIEW_PICTURE			= 1302,
	STAT_PREVIEW_THUMBNAIL					= 1303,
	STAT_FULLSCREEN_PREVIEW_THUMBNAIL		= 1304,
	STAT_PREVIEW_VIDEO						= 1305,
	STAT_PREVIEW_QUESTION					= 1305,

};

class CStatistics
{
private:
	CStatistics();
	~CStatistics();

public:
	BOOL Initialize();
	BOOL Destory();

	void SetUserId(DWORD dwUserId){m_dwUserId = dwUserId;}
	void Report(int nStatType, DWORD dwUserId = 0, CStream* pParamStream = NULL);
	bool OnStatisticsReported(void* param);

	DECLARE_SINGLETON_CLASS(CStatistics);

protected:
	tstring			m_strVersionCode;
	DWORD			m_dwUserId;

};

typedef Singleton<CStatistics> Statistics;

#endif