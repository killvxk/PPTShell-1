#pragma once

class CInvokeParamer;
class CInsertParamer:
	public CInvokeParamer
{

public:
	CInsertParamer();
	virtual ~CInsertParamer();

public:
	virtual	IBaseParamer*	Copy();

	DWORD	GetPlaceHolderId();
	void	SetPlaceHolderId(DWORD dwId);

	DWORD	GetSlideId();
	void	SetSlideId(DWORD dwId);

	DWORD	GetOperationerId();
	void	SetOperationerId(DWORD dwId);

	void	SetInsertPos(int x, int y);
	void	SetInsertPos(POINT pt);
	POINT	GetInsertPos();
	void	SetUserData(void* pUserData);
	void*	GetUserData();
private:
	DWORD			m_dwPlaceHolderId;
	DWORD			m_dwOperationerId;
	DWORD			m_dwSlideId;
	POINT			m_ptInsertPos;
	void*			m_pUserData;
};
