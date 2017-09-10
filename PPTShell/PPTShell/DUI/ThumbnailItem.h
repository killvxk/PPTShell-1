#pragma once

#include "DUI/StyleItem.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/ItemOption.h"
class CStream;

class CThumbnailItemUI: 
	public CStyleItemUI,
	public IThumbnailListener
{
public:
	CThumbnailItemUI(IComponent* pComponent);
	~CThumbnailItemUI();

protected:
	//override ui
	virtual	void			DoInit();
	virtual	void			DoPaint(HDC hDC, const RECT& rcPaint);

	//super
	virtual bool			OnControlNotify(void* pObj);
	virtual bool			OnControlEvent( void* pObj );

	//IThumbnailListener
	virtual void			OnGetThumbnailBefore();
	virtual void			OnGetThumbnailCompleted( LPCTSTR lptcsPath );

	//self
	virtual void			SetThumbnail();

public:
	void					ReadStream(CStream* pStream);

	void					StartMask();
	void					StopMask();
	CContainerUI*			GetContent();
	CItemOptionUI*			GetOption();
	CControlUI*				GetImageCtrl();
	void					SetImage(LPCTSTR lptcsPath);

	static bool				IsResourceExist(LPCTSTR lptcsPath);

private:
	//ui
	CControlUI*				m_lbImage;
	CContainerUI*			m_layContent;
	CContainerUI*			m_layMask;
	CItemOptionUI*			m_pOption;


	bool					m_bHasSetThumbnail;
};
