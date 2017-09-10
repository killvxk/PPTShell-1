#pragma once


class CResourceItemExUI;
class CVRItemExUI : 
	public CResourceItemExUI
{
public:
	CVRItemExUI(IComponent* pComponent);
	virtual  ~CVRItemExUI();

protected:
	//override control
	virtual	void	DoInit();


	//super
	virtual bool	OnControlNotify(void* pNotify);


	//IDownloadListener
	virtual void	OnDownloadBefore( THttpNotify* pHttpNotify );
	virtual void	OnDownloadProgress( THttpNotify* pHttpNotify );
	virtual void	OnDownloadCompleted( THttpNotify* pHttpNotify );
	virtual void	OnDownloadInterpose(THttpNotify* pHttpNotify);

	virtual void    ShowProgress(bool bVisible);

public:
	void			SetTitleColor(DWORD dwColor);

private:
	CControlUI*				m_btnInsert;
	CControlUI*				m_btnResume;
	CControlUI*				m_btnPause;

};
