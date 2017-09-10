#pragma once
#include "regexpstrategyinterface.h"


class CRegExpATL :
	public CRegExpStrategyInterface
{
public:
	CRegExpATL(void);
	virtual ~CRegExpATL(void);
	virtual bool RegExpMatch(const wstring& regExp, const wstring& searchText, std::vector<wstring>& results);
};
