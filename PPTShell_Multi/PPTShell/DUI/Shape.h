#pragma once

enum ShapeType{
	eShape_Unkown	= 0,
	eShape_Line		= 0,
	eShape_Arc		= 0,
};


class CStream;
class IShape
{
public:
	virtual	void		DrawShape(HDC hDC)				= 0;
	virtual	int			GetShapeType()					= 0;
	virtual	void		WriteTo(
							CStream*	pStream,
							int			nScreenWidth,
							int			nScreenHeight
												)		= 0;
	virtual	void		ReadFrom(
							CStream*	pStream,
							int			nScreenWidth,
							int			nScreenHeight
							)							= 0;
};

//////////////////////////////////////////////////////////////////////////
class CDrawAttribute
{
public:
	CDrawAttribute();
	~CDrawAttribute();

	//width
	int			GetWidth();
	void		SetWidth(int nWidth);
	//color
	DWORD		GetColor();
	void		SetColor(DWORD dwColor);


private:
	int			m_nWidth;
	DWORD		m_dwColor;
};



//////////////////////////////////////////////////////////////////////////
class CLineShape:
	public IShape,
	public CDrawAttribute
{

public:
	void				AddPoint(POINT pt); 
	void				ClearPoints(){m_vctrLinePoint.clear();}

protected:
	virtual void		DrawShape( HDC hDC );
	virtual	int			GetShapeType();
	virtual	void		WriteTo(
							CStream*	pStream,
							int			nScreenWidth,
							int			nScreenHeight
							);
	virtual	void		ReadFrom(
							CStream*	pStream,
							int			nScreenWidth,
							int			nScreenHeight
							);
	
private:
	vector<POINT>		m_vctrLinePoint;
};

//////////////////////////////////////////////////////////////////////////
class CArcShape:
	public IShape,
	public CDrawAttribute
{

public:
	CArcShape();
	~CArcShape();

	void				SetRetangle(RECT& rt);
	void				SetRetangle(int left, int top, int right, int bottom);
	void				SetStartAngle(int nAngle);
	void				SetSweepAngle(int nAngle);

protected:
	virtual void		DrawShape( HDC hDC );
	virtual	int			GetShapeType();
	virtual	void		WriteTo(
							CStream*	pStream,
							int			nScreenWidth,
							int			nScreenHeight
							);
	virtual	void		ReadFrom(
							CStream*	pStream,
							int			nScreenWidth,
							int			nScreenHeight
							);

private:
	RECT	m_rtRetangle;
	int		m_nStartAngle;
	int		m_nSweepAngle;
};