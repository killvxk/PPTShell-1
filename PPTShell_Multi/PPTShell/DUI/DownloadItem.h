#pragma once


#include "DUI/IComponent.h"
#include "DUI/ItemHandler.h"
#include "DUI/IVisitor.h"
#include "DUI/IComponent.h"

class IDownloadListener;
class CStyleItemUI;

class CDownloadItemUI : 
	public CStyleItemUI,
	public IDownloadListener
{
public:
	CDownloadItemUI(IComponent* pComponent);
	virtual ~CDownloadItemUI();

protected:
	//super
	virtual	void	DoInit();

	//IControlDelegate
	virtual bool	OnControlNotify(void* pObj);
	virtual bool	OnControlEvent(void* pObj);

	//IDownloadListener
	virtual void	OnDownloadBefore( THttpNotify* pHttpNotify );
	virtual void	OnDownloadProgress( THttpNotify* pHttpNotify );
	virtual void	OnDownloadCompleted( THttpNotify* pHttpNotify );
	virtual void	OnDownloadInterpose(THttpNotify* pHttpNotify);

	//ui progress
	void			ShowNormalProgressLayout(bool bShow);
	void			ShowErrorProgressLayout(bool bShow);
	void			UpdateProgressLayout(int nProgress);
	void			UpdateProgressLayout(CContainerUI* pLayout, int nProgress);

public:
	//invoke after init
	void			SetTitle(LPCTSTR lptcsName);
	void			SetIcon(LPCTSTR lptcsIcon);
	void			SetGuid(LPCTSTR lptcsIcon);
	LPCTSTR			GetGuid();
	void			Cancel();
	void			Pause();
	void			Retry();
	void			Resume();
	bool			IsAsycRelease();
	bool			IsPaused();
	bool			IsErrorOccurred();


private:
	CButtonUI*		m_btnStart;
	CButtonUI*		m_btnPause;
	CButtonUI*		m_btnClose;
	CButtonUI*		m_btnRetry;
	CLabelUI*		m_lbName;
	CControlUI*		m_ctrlIcon;
	CContainerUI*	m_pLayProgressNormal;
	CContainerUI*	m_pLayProgressError;
	bool			m_bAsycRelease;
	bool			m_bPaused;
	bool			m_bDownloadError;
};
