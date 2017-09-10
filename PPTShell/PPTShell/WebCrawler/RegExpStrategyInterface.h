#pragma once
#include <string>
#include <vector>
#include <algorithm>

using std::string;
using std::wstring;
using std::vector;

class CRegExpStrategyInterface
{
public:
	CRegExpStrategyInterface(void);
	virtual ~CRegExpStrategyInterface(void);
	virtual bool RegExpMatch(const wstring& regExp, const wstring& searchText, std::vector<wstring>& results) = 0;
};
