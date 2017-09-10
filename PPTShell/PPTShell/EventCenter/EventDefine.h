//-----------------------------------------------------------------------
// FileName:				EventDefine.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _EVENT_DEFINE_H_
#define _EVENT_DEFINE_H_

#include "EventDelegate.h"
#include "Util/Stream.h"



//
// >>> 为避免重复使用一个事件类型值，所有事件类型定义写到此处 <<<
//
enum EventType
{
	EVT_NONE					= 1000,
	EVT_SET_CHAPTER_GUID,
	EVT_NOTIFY_UPDATE,
	EVT_PPT_FILE_OPERATION,
	EVT_CHAPTER_SWITCH,
	EVT_ENV_RELEASE,
	EVT_REFRESH_GROUPEXPLORER,
	EVT_PPT_INFO_SYNC,			// sync information to users
	EVT_SHOW_NEXT_SLIDE,
	EVT_MENU_CLEARUP,
	EVT_MENU_DOWNLOADLOCAL,
	EVT_MENU_DOWNLOADLOCAL_COMPELETE,
	EVT_MENU_UPLOADNETDISC,
	EVT_MENU_UPLOADNETDISC_COMPELETE,
	EVT_MENU_DELETE,
	EVT_MENU_DELETE_COMPELETE,
	EVT_MENU_RENAME,
	EVT_MENU_RENAME_COMPELETE,
	EVT_MENU_IMPORT,
	EVT_MENU_EDITEXERCISES,
	EVT_LOCALRES_ADD_FILE,
	EVT_LOCALRES_UPDATE_FILE,
	EVT_MENU_UPLOAD,
	EVT_BLUETOOTH_CHANGED,
	EVT_THREAD_CONTROL_PPT,
	EVT_THREAD_INSERT_PPT,
	EVT_INIT_PPT_AGAIN,
	EVT_LOCAL_SEARCH_SWITCH_STATUS,
	EVT_LOGIN,
	EVT_RELOGIN,//重新登录2015.11.20 cws
	EVT_LOGOUT, //退出登录2015.11.20 cws
	EVT_IMAGE_CONTROL,
	EVT_MOBILE_LOGIN,
	EVT_SCREEN_PRE_NEXT_ENABLE,
	EVT_BLACKBROAD_COLOR,
	EVT_DANMUKU,
	EVT_MAGICPEN_OPERATION,
};

enum
{
	NEW_FILE		= 0,
	OPEN_FILE,
	CLOSE_FILE,
	CHANGE_FILE,
	SAVE_FILE,
	AFTER_NEW_FILE,
	CHANGE_TITLE_NAME,
	WINDOW_ACTIVATE,
	SLIDE_CHANGED,
};

enum
{
	PPT_EDIT_OPENFILE,			// open file
	PPT_EDIT_NEWFILE,			// new file
	PPT_EDIT_CLOSEFILE,			// close file
	PPT_EDIT_INSERT_SLIDE,		// insert slide
	PPT_EDIT_DELETE_SLIDE,		// delete slide
	PPT_EDIT_CHANGE_SLIDE,		// change slide content
	PPT_PLAY_BEGIN,
	PPT_PLAY_SLIDE,
	PPT_PLAY_END,
	PPT_PREVIEW_CHANGE,
};

struct TEventNotify 
{
	void*	pSender;
	int		nEventType;
	WPARAM	wParam;		// CStream*
	LPARAM	lParam;
};

void OnEvent(int nEventType, CEventDelegateBase& delegate);
void CancelEvent(int nEventType, CEventDelegateBase& delegate);

void BroadcastEvent(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender = NULL);
void BroadcastEvent(int nEventType, CStream* pStream, void* pSender = NULL);
void BroadcastEventInThread(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender = NULL);	// in caller thread
void BroadcastEventInThread(int nEventType, CStream* pStream, void* pSender = NULL);				// in caller thread

void BroadcastPostedEvent(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender = NULL);

#endif