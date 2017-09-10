#pragma once
#include "../DUICommon.h"
#include <WinInet.h>
#include <utility>
#include <shlwapi.h >
#include <shlobj.h>
#include <algorithm>
#include <ShellAPI.h>
#include <CommCtrl.h>
#include <Psapi.h>
#pragma comment(lib,"Imm32.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"Msimg32.lib")
#pragma comment(lib,"comctl32.lib")
#include <Windows.h>
//{{
typedef bool (*array_compare_t)(LPVOID ptrA, LPVOID ptrB);
//}}
/// 序列容器.
class CStdPtrEffectArray
{
	//{{
public:
	CStdPtrEffectArray(int iPreallocSize = 0);
	 ~CStdPtrEffectArray();
	//}}
	void Empty();
	void Resize(int iSize);
	bool IsEmpty() const;
	int Find(LPVOID iIndex) const;
	bool Add(LPVOID pData);
	bool SetAt(int iIndex, LPVOID pData);
	bool InsertAt(int iIndex, LPVOID pData);
	bool Remove(int iIndex);
	bool Remove(LPVOID iIndex);
	void Swap(int nStart, int nEnd);
	/// 区间交换
	void Swap(int nStart1, int nEnd1, int nStart2, int nEnd2);
	bool Move(int nStart, int nEnd, int nBoundryTo);
	int GetSize() const;
	LPVOID* GetData();

	LPVOID GetAt(int iIndex) const;
	LPVOID operator[] (int nIndex) const;
	//{{
	void Sort(array_compare_t cmp, int nBegin = 0, int nSize = -1);
protected:
	LPVOID* m_ppVoid;
	int m_nCount;
	int m_nAllocated;//已申请空间大小(可储存Ptr的数量), 如果空间不足则去realloc m_nAllocated*2的空间
	//}}
};


class  TweenAlgorithm
{
public:
	typedef enum{
		TweenEasein = 0,
		TweenEaseout = 1,
		TweenEaseinout = 2,
	}TweenEaseType;
	typedef enum{
		TweenLinear = 0,
		TweenQuadratic = 1,
		TweenCubic = 2,
		TweenQuartic = 3,
		TweenQuintic = 4,
		TweenSinusoidal = 5,
		TweenExponential = 6,
		TweenCircular = 7,
		TweenElastic = 8,
		TweenBack = 9,
		TweenBounce = 10,
	}TweenType;
	//{{
	static double Tween(TweenAlgorithm::TweenType type, double currentTime, double beginValue, double changeValue, double duration, TweenAlgorithm::TweenEaseType iTweenEaseType = TweenAlgorithm::TweenEasein, double s = 1.70158);

	static double Linear(	double currentTime, double beginValue, double changeValue, double duration );
	static double Quad(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 ); 
	static double Cubic(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 ); 
	static double Quart(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Quint(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Sine(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Expo(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Circ(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Elastic(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Back(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0, double s = 1.70158);
	static double Bounce(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	//}}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CurveImpl;
class  CurveObj : public CControlUI
{
	//{{
public:
	friend class BezierAlgorithm;
	CurveObj();

	virtual ~CurveObj();
	void AddPoint(float tp, float sp);
	void Reset();
protected:
	CurveImpl* m_pCurveImpl;
	//}}
};
class  BezierAlgorithm
{
public:
	static long BezierValue(CurveObj* curve, long currentTime, long beginValue, long changeValue, long totalTime);
	static POINT BezierPoint(CurveObj* curve, long currentTime, const POINT& beginPoint, const POINT& destPoint, long totalTime);
	static RECT  BezierRect(CurveObj* curve, long currentTime, const RECT& beginRect, const RECT& destRect, long totalTime);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class  StrUtil
{
public:
	static tstring Replace(tstring &str, const TCHAR* string_to_replace, const TCHAR* new_string);
    static void ToLowerCase( tstring& str );
    static void ToUpperCase( tstring& str );
	static void Trim( tstring& str, bool left = true, bool right = true );
	static tstring ConvertBSTR(BSTR bstr);
	//{{
    static vector<tstring> Split( const tstring& str, const tstring& delims = _T("\t\n "), unsigned int maxSplits = 0);
	//}}
	static bool IsMailFormat(const tstring& str);
	static bool IsNumberFormat(const tstring& str);
	static bool IsLetterFormat(const tstring& str);
	static bool IsChineseFormat(const tstring& str);
};
//{{
extern "C" 
{
//}}
	
//{{
}
//}}

