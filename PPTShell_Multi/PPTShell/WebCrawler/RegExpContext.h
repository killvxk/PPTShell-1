#pragma once
#include "RegExpStrategyInterface.h"
#include "RegExpATL.h"
#include "RegExpBoost.h"

class CRegExpContext
{
private:
	CRegExpContext();
public:
// 	static const int USE_ATL_REGEXP			= 1;
// 	static const int USE_BOOST_REGEXP		= 2;
// 	static const int USE_PERL_REGEXP		= 3;
	typedef enum 
	{ 
		USE_ATL_REGEXP, 
		USE_BOOST_REGEXP, 
		USE_PERL_REGEXP
	}REGEXP_RULE;

	CRegExpContext(REGEXP_RULE reg);
	~CRegExpContext(void);
	bool RegExpMatch(const wstring& regExp, const wstring& searchText, std::vector<wstring>& results);
private:
	CRegExpStrategyInterface* _regexp;
};
