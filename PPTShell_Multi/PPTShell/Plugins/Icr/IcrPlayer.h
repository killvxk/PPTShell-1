//==============================================================================
// FileName:				IcrPlayer.h
//
// Desc:				Load and invoke Icr plugin
//==============================================================================
#ifndef _ICR_PLAYER_H_
#define _ICR_PLAYER_H_

#include "Util/Singleton.h"
#include "Util/Stream.h"
#include "IcrSdk.h"

class CIcrPlayer
{
private:
	CIcrPlayer();
	~CIcrPlayer();

public:
	BOOL	Initialize();
	BOOL	Destroy();

	BOOL	CheckCrash(tstring& strPPTFilePath, int& nPageIndex);

	BOOL	IcrOnStart(CStream* pStream);
	BOOL	IcrOnClick(int nButtonID);
	BOOL	IcrOnPageIndexChange(int nCurrentPageIndex, int nNextPageIndex);
	BOOL	IcrOnRecvExamEvent(const char* szEventName, const char* szEventData);
	BOOL	IcrGetOnlineStudents(char** ppStudentsJson);
	char*   IcrInvokeNativeMethod(char* szMethodJson);
	BOOL	IcrSendCmd(tstring strCategory, tstring strCmd, CStream* pStream);
	BOOL	IcrReleaseMemory(void* pAddress);
	BOOL	IcrStop();


	static	void  CALLBACK IcrRelease(void* pAddress);
	static	bool  CALLBACK IcrGetScreen(int nQuality, unsigned char** pData, int& nSize);
	static	bool  CALLBACK IcrSendExamEvent(const char* szEventName, const char* szEventData);
	static	HWND  CALLBACK IcrGetTopLevelWindowHandle();
	static  int	  CALLBACK IcrMessageBox(const char* szText, const char* szCaption, unsigned int uType);
	static  void  CALLBACK IcrShowToast(const char* szText, int nDuration);
	static  void  CALLBACK IcrEnable();
	static  char* CALLBACK IcrGetExamType();
	static  void  CALLBACK IcrSetButtonState(int nUIComponentId, int nButtonId, int nState);	

	DECLARE_SINGLETON_CLASS(CIcrPlayer);

protected:
	void	GetWindowScreen(HWND hWnd, int nZoomValue, char** ppOutBuffer, int& nOutSize, RECT* pSrcRect = NULL);
	void	SetButtonState(CControlUI* pControl, int nState);
	tstring	GetCurrentQuestionType();

protected:
	HMODULE							m_hIcrModule;
	vector<tstring>					m_vecQuestionPaths;
	int								m_nCurrentPageIndex;
	tstring							m_strPPTFilePath;

	int								m_nSlideShowLeft;
	int								m_nSlideShowTop;
	int								m_nSlideShowWidth;
	int								m_nSlideShowHeight;
	 
	LPFN_IcrInit					m_fnIcrInit;
	LPFN_IcrSetCallback				m_fnIcrSetCallBack;
	LPFN_IcrOnStart					m_fnIcrOnStart;
	LPFN_IcrOnClick					m_fnIcrOnClick;
	LPFN_IcrOnPageIndexChange		m_fnIcrOnPageIndexChange;
	LPFN_IcrOnRecvExamEvent			m_fnIcrOnRecvExamEvent;
	LPFN_IcrGetOnlineStudents		m_fnIcrGetOnlineStudents;
	LPFN_IcrInvokeNativeMethod		m_fnIcrInvokeNativeMethod;
	LPFN_IcrSendCmd					m_fnIcrSendCmd;
	LPFN_IcrRelease					m_fnIcrRelease;
	LPFN_IcrStop					m_fnIcrStop;
	LPFN_IcrUnInit					m_fnIcrUnInit;	
	bool							m_bUseStart;//是否调用了onstart

};

typedef Singleton<CIcrPlayer>		IcrPlayer;

#endif