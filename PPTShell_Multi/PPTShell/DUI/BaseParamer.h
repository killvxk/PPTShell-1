#pragma once

struct THttpNotify;
class ITransfer;
class IBaseParamer
{
public:
	virtual	void			InvokeHandler(THttpNotify* pNotify, ITransfer* pTransfer)	= 0;
	virtual	IBaseParamer*	Copy()														= 0;
};

class ITriggerVisitor;

class CInvokeParamer:
	public IBaseParamer,
	public ITriggerVisitor
{

public:
	CInvokeParamer();
	virtual ~CInvokeParamer();

public:
	THttpNotify*		GetHttpNotify();
	ITransfer*			GetTransfer();

	void				SetCompletedDelegate(CDelegateBase& completedHandler);

	//IBaseParamer
	virtual	void			InvokeHandler(THttpNotify* pNotify, ITransfer* pTransfer);
	virtual	IBaseParamer*	Copy();

	//ITriggerVisitor
	virtual CControlUI* GetTrigger();
	virtual void		SetTrigger( CControlUI* pTrigger );

private:
	THttpNotify*	m_pHttpNotify;
	ITransfer*		m_pTransfer;
	
protected:
	CEventSource	OnCompletedHandler;
	CControlUI*		m_pTrigger;

};


