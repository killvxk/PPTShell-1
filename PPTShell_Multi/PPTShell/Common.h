
#pragma once
#include "Log/Log.h"
#include "Log/LogManager.h"
#include "PPTWMUserDef.h"

enum
{
	NOTIFY_FORM_UPDATE = 1,
	NOTIFY_FORM_MENU,
	NOTIFY_FORM_TIMER,
};

enum
{
	SHAPE_QUESTION	= 1,
	SHAPE_FLASH,
	SHAPE_3D,
	SHAPE_VR,
};

#define MAIN_APP_NAME			_T("101教育PPT")
#define PPT_EXE_NAME			_T("POWERPNT.EXE")
#define COURSE_PLAYER_EXE_NAME	_T("COURSEPLAYER.EXE")
#define UNZIP_EXE_NAME _T("UNZIP101.exe")

#define PLUGIN_SIGN			_T("[MDI]")
#define PLUGIN_SIGN_HIDE	_T("[HID]") //运行时隐藏的插件
#define	PLUGIN_VIDEO		_T("[VIDEO]")
#define	PLUGIN_FLASH		_T("[FLASH]")
#define	PLUGIN_3D			_T("[3D]")
#define	PLUGIN_VR			_T("[VR]")

#define MSG_PPT_STOP						211
#define MSG_PPT_ENDSTOP						212
// CMainFrame
#define IDC_TITLEBUTTON_PREVIOUS			220
#define IDC_TITLEBUTTON_LESSON				219
#define IDC_TITLEBUTTON_NEXT				218
#define IDC_TITLEBUTTON_USERLOGIN			217//用戶登陸
#define IDC_TITLEBUTTON_DOWNLOADMGR			216//用戶登陸


//#define IDC_TITLEBUTTON_ABOUT_VERSION		21//关于版本号
#define IDC_TITLEBUTTON_3DPPT				21
#define IDC_TITLEBUTTON_PLAY_FULLSCREEN		20
#define IDC_TITLEBUTTON_REPEALS				19
#define IDC_TITLEBUTTON_INSART_EXERCISES	18
#define IDC_TITLEBUTTON_INSERT_VIDEO		17
#define IDC_TITLEBUTTON_INSERT_FLASH		16
#define IDC_TITLEBUTTON_INSERT_PICTURE		15
#define IDC_TITLEBUTTON_ABOUT				14

#define IDC_TITLEBUTTON_PACKAGE_LESSION		11
#define IDC_TITLEBUTTON_OPEN				10
#define IDC_TITLEBUTTON_PHONE				9//2015.11.05 CWS 手机连接
#define IDC_TITLEBUTTON_NEW					8
#define IDC_TITLEBUTTON_LOGO				7

//来着插件播放器的上下页消息
#define MSG_NEXT							5
#define MSG_PREV							6
#define MSG_ESC                             7
#define MSG_CEFINFO                         8
#define MSG_OBTAIN_ONLINE_STUDENTS          9
#define MSG_CEF_ICRINVOKENATIVE				10
#define MSG_DRAWMETHOD                      11
#define MSG_CEFNEXT							12
#define MSG_CEFPREV							13
#define MSG_OPEN_FILE						1001
#define MSG_QUESTION_ADD					1002
#define MSG_QUESTION_SAVE					1003
#define MSG_OPEN_FILE_WEBSITE				1004
#define MSG_QUESTION_ADDSAVE				1005
#define MSG_CEFOCX_PAGECHANGE				1006
#define MSG_CEFOCX_ICR						1007
//
// Log Types and Folders
//

//
// Logs
//
void logMsg(char* filename, int logtype, int logfolder, char *format, ...);
void logToOutput(char *format,...);

#ifdef _DEBUG
//#define WRITE_LOG_EX(logtype, logfolder, format, ...)	{ printf(format, __VA_ARGS__);printf("\r\n"); fflush(stdout); logMsg(NULL, logtype, logfolder, format, __VA_ARGS__); }
#define WRITE_LOG_EX(logtype, logfolder, format, ...)		\
	{	\
		logToOutput(format, __VA_ARGS__);		\
		logMsg(NULL, logtype, logfolder, format, __VA_ARGS__); }
#else
#define WRITE_LOG_EX(logtype, logfolder, format, ...)	{ logMsg(NULL, logtype, logfolder, format, __VA_ARGS__); }
#endif

#ifdef _DEBUG
#define WRITE_LOG_TO_FILE(filename, format, ...)		{ printf(format, __VA_ARGS__);printf("\r\n"); logMsg(filename, -1, -1, format, __VA_ARGS__); }
#else
#define WRITE_LOG_TO_FILE(filename, format, ...)		{ logMsg(filename, -1, -1, format, __VA_ARGS__); }
#endif

#define WRITE_LOG_LOCAL(format, ...)					WRITE_LOG_EX(AT_ONCE, LOG_FOLDER_LOG_LOCAL, format, __VA_ARGS__)
#define WRITE_LOG_SERVER(format, ...)					WRITE_LOG_EX(AT_ONCE, LOG_FOLDER_LOG_SERVER, format, __VA_ARGS__)
#define WRITE_LOG_TEMP(format, ...)						WRITE_LOG_EX(AT_ONCE, LOG_FOLDER_LOG_TEMP, format, __VA_ARGS__)

#ifdef _DEBUG
#define WRITE_LOG_DEBUG(format, ...)					WRITE_LOG_EX(AT_ONCE, LOG_FOLDER_LOG_DEBUG, format, __VA_ARGS__)
#else
#define WRITE_LOG_DEBUG
#endif
