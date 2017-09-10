#pragma once

class CResourceDescriber
{
public:
	CResourceDescriber();
	~CResourceDescriber();

	virtual	void			SetResourceType(int nType);
	virtual	void			SetResourceTitle(LPCTSTR lptcsType);

	virtual	int				GetResourceType();
	virtual	LPCTSTR			GetResourceTitle();

private:
	int			m_nType;
	tstring		m_strTiltle;
};
