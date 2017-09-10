#pragma once

class CWebCrawler
{
public:
	CWebCrawler(void);
	~CWebCrawler(void);

	//请求
	bool ProcessHttpRequest(CStringW strUrl, bool isUtf8File = false);
	//查找
	void FindTextInHtml(CStringW& strFindText, std::vector<wstring>& vecMatch);
	void FindTextInHtml(wstring& strHtmlContent, CStringW& strFindText, std::vector<wstring>& vecMatch);
	//void FindName(CStringW strFindText);
	//void FindPreviewsAddress(CStringW strFindText);
	//void FindObjAddress(CStringW strFindText);

	//提取
	wstring Getmatch(std::vector<wstring>& vecMatch);

	void ResponseContent2Uni();

public:
	// 收集服务器返回的Http头和Html内容
	wstring httpResponseHeader;
	// The response content.
	wstring httpResponseContent;

	//std::vector<wstring> vecMatchName;
	//std::vector<wstring> vecMatchPreviewsAddress;
	//std::vector<wstring> vecMatchObjAddress;
};
