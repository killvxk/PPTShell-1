#pragma once

#include "DUI/Shape.h"
typedef struct LineShape
{
	vector<POINT>	Points;
	int				Width;
	DWORD			Color;
	DWORD			LineID;

}LineShape;

enum PanelPointerType
{
	ePoiner_Unkown,
	ePoiner_Arrow,
	ePoiner_Pen,//»­±Ê
	ePoiner_Erase,//²Á³ý
	ePoiner_Total,
};



class CPanelViewUI:
	public CControlUI
{
public:
	CPanelViewUI(void);
	~CPanelViewUI(void);
protected:
	virtual	void		Init();
	virtual UINT		GetControlFlags() const;
	virtual void		DoPaint( HDC hDC, const RECT& rcPaint );
	virtual	void		DoEvent(TEventUI& event);


private:
	void				OnHandlePenEvent(TEventUI& event);
	void				OnHandleEraseEvent(TEventUI& event);
	void				OnHandleArrowEvent(TEventUI& event);
	void				AddErasePoint(POINT pt);

public:
	void				SetReferenceBackground(bool bReference);
	void				SetShapeColor(DWORD dwColor);
	void				SetShapeWidth( int nWidth );
	void				ClearShapes();
	void				SetPointer(int nPointerType);


	vector<IShape*>*	GetShapes();
	void				SetShapes(vector<IShape*>* vShapes);
	void				AddShapes(IShape* pShape);

private:
	bool					m_bMouseDown;
	bool					m_bCanDraw;
	bool					m_bCanErase;
	bool					m_bReferenceBackground;
	int						m_nWidth;
	int						m_nCurrentPointer;
	DWORD					m_dwColor;
	DWORD					m_ShapeID;
	CLineShape*				m_pCurrentLineShape;
	vector<IShape*>			m_Shapes;
	vector<POINT>			m_ErasePoints;
	map<DWORD, DWORD>		m_mapReferencePoints;

};
