#ifndef _SPOTLIGHTWINDOW_H_
#define _SPOTLIGHTWINDOW_H_

#include "DUICommon.h"
#include "DUI/SpotLightUI.h"

enum
{
	SPOTLIGHT_DIRECT_RIGHTTOP = 0,
	SPOTLIGHT_DIRECT_RIGHTBOTTOM,
	SPOTLIGHT_DIRECT_LEFTTOP,
	SPOTLIGHT_DIRECT_LEFTBOTTOM,
};

enum
{
	SPOTLIGHT_TOOLS_DIRECT_LEFT = 0,
	SPOTLIGHT_TOOLS_DIRECT_LEFTTOP,
	SPOTLIGHT_TOOLS_DIRECT_LEFTBOTTOM,
	SPOTLIGHT_TOOLS_DIRECT_TOP,
	SPOTLIGHT_TOOLS_DIRECT_TOPLEFT,
	SPOTLIGHT_TOOLS_DIRECT_TOPRIGHT,
	SPOTLIGHT_TOOLS_DIRECT_RIGHT,
	SPOTLIGHT_TOOLS_DIRECT_RIGHTTOP,
	SPOTLIGHT_TOOLS_DIRECT_RIGHTBOTTOM,
	SPOTLIGHT_TOOLS_DIRECT_BOTTOM,
};

enum
{
	SPOTLIGHT_SHAPE_CIRCLE = 0,
	SPOTLIGHT_SHAPE_RECTANGLE
};

enum
{
	SPOTLIGHT_STATUS_ON = 0,
	SPOTLIGHT_STATUS_CLOSE
};

#define SpotLightToolsWidth 64
#define SpotLightToolsHeight 64
#define SpotLightToolsMargin 26

#define CloseLightColor		0xFE000000
#define OpenLightColor		0xAA000000

#define DefaultRadius		300


#if(WINVER < 0x0601)

#define SM_DIGITIZER 94
#define SM_MAXIMUMTOUCHES 95

#define WM_GESTURE                      0x0119
#define WM_GESTURENOTIFY                0x011A

/*
 * Gesture defines and functions
 */

/*
 * Gesture information handle
 */
DECLARE_HANDLE(HGESTUREINFO);

/*
 * Gesture flags - GESTUREINFO.dwFlags
 */
#define GF_BEGIN                        0x00000001
#define GF_INERTIA                      0x00000002
#define GF_END                          0x00000004

/*
 * Gesture IDs
 */
#define GID_BEGIN                       1
#define GID_END                         2
#define GID_ZOOM                        3
#define GID_PAN                         4
#define GID_ROTATE                      5
#define GID_TWOFINGERTAP                6
#define GID_PRESSANDTAP                 7
#define GID_ROLLOVER                    GID_PRESSANDTAP

/*
 * Gesture information structure
 *   - Pass the HGESTUREINFO received in the WM_GESTURE message lParam into the
 *     GetGestureInfo function to retrieve this information.
 *   - If cbExtraArgs is non-zero, pass the HGESTUREINFO received in the WM_GESTURE
 *     message lParam into the GetGestureExtraArgs function to retrieve extended
 *     argument information.
 */
typedef struct tagGESTUREINFO {
    UINT cbSize;                    // size, in bytes, of this structure (including variable length Args field)
    DWORD dwFlags;                  // see GF_* flags
    DWORD dwID;                     // gesture ID, see GID_* defines
    HWND hwndTarget;                // handle to window targeted by this gesture
    POINTS ptsLocation;             // current location of this gesture
    DWORD dwInstanceID;             // internally used
    DWORD dwSequenceID;             // internally used
    ULONGLONG ullArguments;         // arguments for gestures whose arguments fit in 8 BYTES
    UINT cbExtraArgs;               // size, in bytes, of extra arguments, if any, that accompany this gesture
} GESTUREINFO, *PGESTUREINFO;
typedef GESTUREINFO const * PCGESTUREINFO;

/*
 * Gesture notification structure
 *   - The WM_GESTURENOTIFY message lParam contains a pointer to this structure.
 *   - The WM_GESTURENOTIFY message notifies a window that gesture recognition is
 *     in progress and a gesture will be generated if one is recognized under the
 *     current gesture settings.
 */
typedef struct tagGESTURENOTIFYSTRUCT {
    UINT cbSize;                    // size, in bytes, of this structure
    DWORD dwFlags;                  // unused
    HWND hwndTarget;                // handle to window targeted by the gesture
    POINTS ptsLocation;             // starting location
    DWORD dwInstanceID;             // internally used
} GESTURENOTIFYSTRUCT, *PGESTURENOTIFYSTRUCT;

/*
 * Gesture information retrieval
 *   - HGESTUREINFO is received by a window in the lParam of a WM_GESTURE message.
 */
typedef
BOOL
(WINAPI *pFnGetGestureInfo)(
							__in HGESTUREINFO hGestureInfo,
							__out PGESTUREINFO pGestureInfo);

typedef
BOOL
(WINAPI *pFnRegisterTouchWindow)(
					__in HWND hwnd,
					__in ULONG ulFlags);
#else
typedef
BOOL
(WINAPI *pFnGetGestureInfo)(
							__in HGESTUREINFO hGestureInfo,
							__out PGESTUREINFO pGestureInfo);
#endif

namespace DuiLib
{
	class CSpotLightWindowUI : public CVerticalLayoutUI
	{
	public:
		CSpotLightWindowUI();
		~CSpotLightWindowUI();

		virtual LPCTSTR GetClass() const;
		virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	public:

		virtual void Init();

		void	SetZoomBtnDirect(DWORD dwDirect);
		void	SetToolsPos();
		void	MoveSpotLight(int dwOffsetX, int dwOffsetY );

		void	SetCircleRadius(DWORD dwCircleRadius);
		void	SetRectanglePos(int nRectangleWidth, int nRectangleHeight);

		bool	ZoomBtnEvent(void *pParam);
		bool	ToolsBtnEvent(void *pParam);
		
		bool	ZoomBtnNotify(TNotifyUI& msg);

		bool	BackgroundEvent(void *pParam);

		void	SetRect(CRect &rect);

		void	ShowSpotLightWindow(bool bShow);

		bool	GetSpotLightVisible();

		CSpotLightUI* GetSpotLightUI();

		HRGN	GetSpotLightHRgn() {return m_SpotLightRgn;}

		void	SetGesture(DWORD dwFlag, ULONGLONG ullArguments);
	private:
		POINT	m_pointCirclePointPos;
		DWORD	m_dwCircleRadius;

		int		m_nRectangleWidth; //矩形离原点宽
		int		m_nRectangleHeight; //矩形离原点高

		DWORD	m_dwDirection; //4个方位

		DWORD	m_dwShape;	 //圆形，正方形
		DWORD	m_dwLightStatus;	 //开关灯

		CButtonUI*		m_pZoomBtn;
		CSpotLightUI*	m_pSpotLightUI;


		bool		m_bButtonDown;
		bool		m_bDialogDown;
		bool		m_bRgnDown;
		bool		m_bRgnMove;
		DWORD		m_dwRgnMoveDirect; // 0水平 1 垂直

		POINT	m_pointStart;

		CRect	m_rect;

		CListUI* m_pSpotLightToolsV;
		CListUI* m_pSpotLightToolsH;

		CListUI* m_pCurSpotLightTools;

		CButtonUI*	m_pCurHotBtn;
		CLabelUI*	m_pCurHotLabel;
		CDuiString	m_strBtnImagePath;
		CDuiString	m_strLabelImagePath;

		ULONGLONG	m_ullArguments;

		HRGN		m_SpotLightRgn;


	};


} // DuiLib

#endif // 