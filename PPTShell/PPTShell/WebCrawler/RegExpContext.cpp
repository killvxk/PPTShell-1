#include "StdAfx.h"
#include "RegExpContext.h"

// CRegExpContext::CRegExpContext(void)
// {
// }

CRegExpContext::CRegExpContext(REGEXP_RULE reg)
{
	switch( reg )
	{
	case USE_ATL_REGEXP:
		_regexp = new CRegExpATL();
		break;

	case USE_BOOST_REGEXP:
		_regexp = new CRegExpBoost();
		break;

	case USE_PERL_REGEXP:
		_regexp = NULL;
		break;

	default:
		_regexp = NULL;
	}
	
}

CRegExpContext::~CRegExpContext(void)
{
	delete _regexp;
}

bool CRegExpContext::RegExpMatch(const wstring& regExp, const wstring& searchText, std::vector<wstring>& results)
{
	if ( _regexp != NULL )
	{
		return _regexp->RegExpMatch(regExp, searchText, results);
	}
	return false;
}
