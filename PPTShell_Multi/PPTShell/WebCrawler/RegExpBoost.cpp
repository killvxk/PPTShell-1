#include "StdAfx.h"
#include "RegExpBoost.h"
#include <regex>
//#include "../../../NdCefLib/Boost/include/boost-1_60/boost/xpressive/xpressive.hpp"

CRegExpBoost::CRegExpBoost(void)
{
}

CRegExpBoost::~CRegExpBoost(void)
{
}

bool CRegExpBoost::RegExpMatch(const wstring& regExp, const wstring& searchText, std::vector<wstring>& results)
{
	//-------------------------------------------
	//函数功能：使用boost库中的 xpressive 正则表达式搜索
	//参数说明： 
	//注意事项： 
	//-------------------------------------------

	//using namespace boost::xpressive;
	try
	{
		std::tr1::regex_constants::syntax_option_type optype = std::tr1::regex_constants::icase;
		std::tr1::wregex reg(regExp.c_str(), regExp.size(), optype);
		std::tr1::wsregex_iterator pos( searchText.begin(), searchText.end(), reg );
		std::tr1::wsregex_iterator end;
		while( pos != end )
		{
			results.push_back( (*pos)[0] );
			++pos;
		}

	}
	catch (...)
	{
		return false;
	}
	return true;

}