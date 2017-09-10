#pragma once
#include "regexpstrategyinterface.h"

class CRegExpBoost :
	public CRegExpStrategyInterface
{
public:
	CRegExpBoost(void);
	virtual ~CRegExpBoost(void);
	virtual bool RegExpMatch(const wstring& regExp, const wstring& searchText, std::vector<wstring>& results);
};
