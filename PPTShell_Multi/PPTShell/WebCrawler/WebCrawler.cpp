#include "StdAfx.h"
#include "WebCrawler.h"
#include "RegExpContext.h"
#include "ThirdParty/ATLRegExp/WinHttpClient.h"		// http连接的包装类
#include "Util/Util.h"

CWebCrawler::CWebCrawler(void)
{
}

CWebCrawler::~CWebCrawler(void)
{
}

bool CWebCrawler::ProcessHttpRequest(CStringW strUrl, bool isUtf8File)
{
	//-------------------------------------------
	//函数功能：获取URL地址，然后请求服务，返回Html头和内容
	//参数说明： 
	//注意事项： 
	//-------------------------------------------

	// 获取需要请求的URL地址
	CStringW url;
	url = strUrl;

	// 检查是否需要添加http协议头
	if ( url.Find(L"http://") == -1 )
	{
		url.Insert(0, L"http://");
	}

	// 构造Http客户端，发起请求
	WinHttpClient client( url.GetBuffer());
	// Send HTTP request, a GET request by default.
	if (isUtf8File)
	{
		client.SendHttpRequest(L"GET", false, true);
	}
	else
	{
		client.SendHttpRequest();
	}


	// 收集服务器返回的Http头和Html内容
	httpResponseHeader = client.GetResponseHeader();
	// The response content.
	httpResponseContent = client.GetResponseContent();

	//// 把http头和Html内容显示在Tab标签页的文本框中。
	//_tabHtmlHead._edtHtmlHead.SetWindowText( httpResponseHeader.c_str() );
	//_tabHtmlBody._edtHtmlBody.SetWindowText( httpResponseContent.c_str() );

	return true;
}

void CWebCrawler::FindTextInHtml(CStringW& strFindText, std::vector<wstring>& vecMatch)
{
	//-------------------------------------------
	//函数功能：在Html内容中搜索所出需要的文本
	//参数说明： 
	//注意事项： 
	//-------------------------------------------

	// 获取需要搜索的文本
	CStringW searchText;
	searchText = strFindText;
	CStringW htmlContent;
	htmlContent = httpResponseContent.c_str();

	// 判断用户选择那种正则表达式
	CRegExpContext::REGEXP_RULE index = CRegExpContext::REGEXP_RULE::USE_BOOST_REGEXP;
	CRegExpContext RegExpContext(index);

	// 正则表达式搜索
	vecMatch.clear();
	std::wstring regExp = searchText.GetBuffer();
	std::wstring searchContent = htmlContent.GetBuffer();
	RegExpContext.RegExpMatch(regExp, searchContent, vecMatch);

	//// 显示匹配正则表达式项目的个数
	//size_t matchNum = vecMatch.size();
	//CString temp; temp.Format(L"%d", matchNum );
	//_edtMatchNumber.SetWindowText(temp);

	//// 遍历所有匹配结果，然后在Richedit里进行匹配内容
	//// 的搜索和着色
	//for ( std::vector<wstring>::iterator it = vecMatch.begin();
	//	it != vecMatch.end();
	//	++it )
	//{
	//	CString test( (*it).c_str() );
	//	SearchInHtmlContent(test);
	//}

}

void CWebCrawler::FindTextInHtml(wstring& strHtmlContent, CStringW& strFindText, std::vector<wstring>& vecMatch)
{
	// 获取需要搜索的文本
	CStringW searchText;
	searchText = strFindText;

	// 判断用户选择那种正则表达式
	CRegExpContext::REGEXP_RULE index = CRegExpContext::REGEXP_RULE::USE_BOOST_REGEXP;
	CRegExpContext RegExpContext(index);

	// 正则表达式搜索
	vecMatch.clear();
	std::wstring regExp = searchText.GetBuffer();
	//std::wstring searchContent = strHtmlContent.GetBuffer();
	std::wstring searchContent = strHtmlContent;
	RegExpContext.RegExpMatch(regExp, searchContent, vecMatch);
}

//void CWebCrawler::FindName(CStringW strFindText)
//{
//	FindTextInHtml(strFindText, vecMatchName);
//}
//void CWebCrawler::FindPreviewsAddress(CStringW strFindText)
//{
//	FindTextInHtml(strFindText, vecMatchPreviewsAddress);
//}
//void CWebCrawler::FindObjAddress(CStringW strFindText)
//{
//	FindTextInHtml(strFindText, vecMatchObjAddress);
//}


wstring CWebCrawler::Getmatch(std::vector<wstring>& vecMatch)
{
	// TODO: 在此添加命令处理程序代码
	//CString temp;
	//for ( vector<wstring>::iterator it = vecMatch.begin();
	//	it != vecMatch.end();
	//	++it)
	//{
	//	temp += (*it).c_str();
	//	temp += L"\r\n";
	//}
	//_tabHtmlGet._edtHtmlGet.SetWindowText(temp);

	wstring temp;
	for ( vector<wstring>::iterator it = vecMatch.begin();
		it != vecMatch.end();
		++it)
	{
		temp += (*it);
		temp += L"\r\n";
	}

	return temp;
}

void CWebCrawler::ResponseContent2Uni()
{
	
	//string httpResponseContentA = Un2Str(httpResponseContent);
	string httpResponseContentA = UnicodeToAnsi(httpResponseContent);

	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, httpResponseContentA.c_str(), httpResponseContentA.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	MultiByteToWideChar(CP_UTF8, NULL, httpResponseContentA.c_str(), httpResponseContentA.length(), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	httpResponseContent = wszString;
	delete wszString;


}