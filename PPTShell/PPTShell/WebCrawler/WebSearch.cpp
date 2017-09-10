#include "StdAfx.h"
#include "WebSearch.h"
#include "Util/Util.h"

TCHAR g_chResTypeNa[][25] = {
							  _T("Courseware"),
							  _T("Vedio"),
							  _T("Picture"),
							  _T("Cartoon"),
							  _T("Audio"),
							  _T("Basedproblem"),
							  _T("Interactivetopic"),
							  _T("PPTTemplate"),
							  _T("Others")
							};


WebSearch::WebSearch(void)
{
}

WebSearch::~WebSearch(void)
{
}


void WebSearch::SearchPPT(CStringW keyWord, int begin)
{
	CStringW strUrl;
	//strUrl.Format(L"http://www.baidu.com/baidu?q1=%s&ft=ppt", keyWord);
	strUrl.Format(L"http://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&tn=baiduadv&wd=filetype:ppt %s -site:(wenku.baidu.com)&rn=50&pn=%02d", keyWord, begin);
	m_webCrawler.ProcessHttpRequest(strUrl);

	//CStringW regExp = L"<b>【PPT】</b></font><a[^>]*href[=\"\'\s]+([^\"\']*)[\"\']?[^>]*>";
	CStringW regExp = L"<b>【PPT】</b></font><a[^>]*>(.*?)</a>";
	//CStringW regExp = L"<b>【PPT】</b></font><a.*?[^<]>.*?</a>";
	m_vecTemp.clear();
	replace_all(m_webCrawler.httpResponseContent, L"\n", L""); 
	m_webCrawler.FindTextInHtml(regExp, m_vecTemp);

	BuildJsonEntity4PPT();

	return;
}


void WebSearch::BuildJsonEntity4PPT()
{
	wstring strMatch = m_webCrawler.Getmatch(m_vecTemp);
	m_vecMatchName.clear();
	GetPPTName(m_vecTemp, m_vecMatchName);

	//CStringW regExp2 = L"(http|ftp|https):\/\/[\w\-_]+(\.[\w\-_]+)+([\w\-\.,@?^=%&amp;:/~\+#]*[\w\-\@?^=%&amp;/~\+#])?";
	CStringW regExp2 = L"(http|ftp|https):\\\/\\\/[\\w\\-_]+(\\.[\\w\\-_]+)+([\\w\\-\\.,@?^=%&amp;:\/~\\+#]*[\\w\\-\\@?^=%&amp;\/~\\+#])?";
	m_vecMatchObjAddress.clear();
	m_webCrawler.FindTextInHtml(strMatch, regExp2, m_vecMatchObjAddress);

	size_t vecSize = min(m_vecMatchName.size(), m_vecMatchObjAddress.size());

	m_vecJsonEntity.clear();
	for(int i = 0; i < vecSize; i++)
	{
		JsonEntity* jsonEntity = new JsonEntity;
		jsonEntity->Name = UnicodeToUtf8(m_vecMatchName.at(i));
		jsonEntity->objAddress = UnicodeToUtf8(m_vecMatchObjAddress.at(i));
		m_vecJsonEntity.push_back(*jsonEntity);
	}
	return;
}


wstring& WebSearch::replace_all(wstring& strGoal, const wstring& strOld, const wstring& strNew)     
{     
	while(true)
	{
		wstring::size_type pos(0);     
		if( (pos=strGoal.find(strOld))!=string::npos )
		{
			strGoal.replace(pos,strOld.length(),strNew);
		}
		else
		{
			break;
		}
	}     
	return strGoal;     
}     


void WebSearch::GetPPTName(std::vector<wstring>& vecOrigin, std::vector<wstring>& vecMatch)
{
	wstring strMatch = m_webCrawler.Getmatch(vecOrigin);
	CStringW regExp3 = L"<a[^>]*>(.*?)</a>";
	m_vecMatchName.clear();
	m_webCrawler.FindTextInHtml(strMatch, regExp3, m_vecMatchName);

	strMatch = m_webCrawler.Getmatch(m_vecMatchName);
	regExp3 = L">[\\s\\S]*?</a>";
	m_vecMatchName.clear();
	m_webCrawler.FindTextInHtml(strMatch, regExp3, m_vecMatchName);

	std::vector<wstring>::iterator iter;
	for(iter=m_vecMatchName.begin(); iter!=m_vecMatchName.end(); iter++){  
		wstring strTemp = *iter;
		int posEnd = strTemp.find_last_of(L"<");
		strTemp = strTemp.substr(1,posEnd-1);
		strTemp = replace_all(strTemp,L"<em>", L"");
		strTemp = replace_all(strTemp,L"</em>", L"");
		strTemp = replace_all(strTemp,L"\n", L"");
		strTemp = replace_all(strTemp,L"\r", L"");
		*iter = strTemp;
	}
}


void WebSearch::GetAllPagesResInfo(tstring strKeyWord, ENResType enResType, UINT uIBeginCount, UINT uIEndCount)
{
	switch (enResType)
	{
	case enResTypeCourseware:
		{
			for (int i=0; (uIBeginCount+(i + 1)*50<=uIEndCount) || ( (uIEndCount<uIBeginCount+(i + 1)*50) && (uIBeginCount+(i + 1)*50 < uIEndCount+50)); i++)
			{
				SearchPPT(Str2Unicode(strKeyWord).c_str(), uIBeginCount);
				m_vecAllPagesJsonEntity.insert(m_vecAllPagesJsonEntity.end(), m_vecJsonEntity.begin(), m_vecJsonEntity.end());
				if (m_vecJsonEntity.size()<50)
				{
					break;
				}
				uIBeginCount = uIBeginCount+i*50;

			}
			break;
		}
	case enResTypePicture: 
		{
			for (int i=0; (uIBeginCount+(i + 1)*60<=uIEndCount) || ( (uIEndCount<uIBeginCount+(i + 1)*60) && (uIBeginCount+(i + 1)*60 < uIEndCount+60)); i++)
			{
				SearchPic(Str2Unicode(strKeyWord).c_str(), uIBeginCount);
				m_vecAllPagesJsonEntity.insert(m_vecAllPagesJsonEntity.end(), m_vecJsonEntity.begin(), m_vecJsonEntity.end());
				if (m_vecJsonEntity.size()<60)
				{
					break;
				}
				uIBeginCount = uIBeginCount+i*60;
				
			}
			break;
		}
	default :
		break;
	}
}



void WebSearch::DecodeObjAddress()
{
	size_t size = m_vecMatchObjAddress.size();
	for (int i = 0; i < size; i++)
	{
		wstring ObjAddress = m_vecMatchObjAddress.at(i);

		//ObjAddress.replace(L"_z&e3B", L".");
		//ObjAddress.replace(L"AzdH3F", L"/");
		//ObjAddress.replace(L"_z2C$q", L":");

		replace_all(ObjAddress, L"_z&e3B", L".");
		replace_all(ObjAddress, L"AzdH3F", L"/");
		replace_all(ObjAddress, L"_z2C$q", L":");
		//trans = string.maketrans('0123456789abcdefghijklmnopqrstuvwxyz', 
		//	'7dgjmoru140852vsnkheb963wtqplifcaxyz')
		for(int j = 0; j < ObjAddress.size(); j++)
		{
			WCHAR wchar = ObjAddress.at(j);
			switch (wchar)
			{
			case L'0':
				ObjAddress.replace(j, 1, 1, L'7');
				break;
			case L'1':
				ObjAddress.replace(j, 1, 1, L'd');
				break;
			case L'2':
				ObjAddress.replace(j, 1, 1, L'g');
				break;
			case L'3':
				ObjAddress.replace(j, 1, 1, L'j');
				break;
			case L'4':
				ObjAddress.replace(j, 1, 1, L'm');
				break;
			case L'5':
				ObjAddress.replace(j, 1, 1, L'o');
				break;
			case L'6':
				ObjAddress.replace(j, 1, 1, L'r');
				break;
			case L'7':
				ObjAddress.replace(j, 1, 1, L'u');
				break;
			case L'8':
				ObjAddress.replace(j, 1, 1, L'1');
				break;
			case L'9':
				ObjAddress.replace(j, 1, 1, L'4');
				break;
			case L'a':
				ObjAddress.replace(j, 1, 1, L'0');
				break;
			case L'b':
				ObjAddress.replace(j, 1, 1, L'8');
				break;
			case L'c':
				ObjAddress.replace(j, 1, 1, L'5');
				break;
			case L'd':
				ObjAddress.replace(j, 1, 1, L'2');
				break;
			case L'e':
				ObjAddress.replace(j, 1, 1, L'v');
				break;
			case L'f':
				ObjAddress.replace(j, 1, 1, L's');
				break;
			case L'g':
				ObjAddress.replace(j, 1, 1, L'n');
				break;
			case L'h':
				ObjAddress.replace(j, 1, 1, L'k');
				break;				
			case L'i':
				ObjAddress.replace(j, 1, 1, L'h');
				break;
			case L'j':
				ObjAddress.replace(j, 1, 1, L'e');
				break;
			case L'k':
				ObjAddress.replace(j, 1, 1, L'b');
				break;
			case L'l':
				ObjAddress.replace(j, 1, 1, L'9');
				break;
			case L'm':
				ObjAddress.replace(j, 1, 1, L'6');
				break;
			case L'n':
				ObjAddress.replace(j, 1, 1, L'3');
				break;
			case L'o':
				ObjAddress.replace(j, 1, 1, L'w');
				break;
			case L'p':
				ObjAddress.replace(j, 1, 1, L't');
				break;
			case L'q':
				ObjAddress.replace(j, 1, 1, L'q');
				break;
			case L'r':
				ObjAddress.replace(j, 1, 1, L'p');
				break;
			case L's':
				ObjAddress.replace(j, 1, 1, L'l');
				break;				
			case L't':
				ObjAddress.replace(j, 1, 1, L'i');
				break;
			case L'u':
				ObjAddress.replace(j, 1, 1, L'f');
				break;
			case L'v':
				ObjAddress.replace(j, 1, 1, L'c');
				break;
			case L'w':
				ObjAddress.replace(j, 1, 1, L'a');
				break;
			case L'x':
				ObjAddress.replace(j, 1, 1, L'x');
				break;
			case L'y':
				ObjAddress.replace(j, 1, 1, L'y');
				break;
			case L'z':
				ObjAddress.replace(j, 1, 1, L'z');
				break;
			default:
				break;
			}
		}
		m_vecMatchObjAddress[i] = ObjAddress;
	}

}


void WebSearch::SearchPic(CStringW keyWord, int begin)
{
	CStringW strUrl;
	//strUrl.Format(L"http://image.baidu.com/search/acjson?tn=resultjson_com&ipn=rj&ct=201326592&is=&fp=result&queryWord=%s&cl=2&lm=-1&ie=utf-8&oe=utf-8&adpicid=&st=-1&z=&ic=0&word=%s&s=&se=&tab=&width=&height=&face=0&istype=2&qc=&nc=1&fr=&pn=30&rn=60", keyWord, keyWord);
	strUrl.Format(L"http://image.baidu.com/search/acjson?tn=resultjson_com&ipn=rj&ct=201326592&is=&fp=result&queryWord=%s&cl=2&lm=-1&ie=utf-8&oe=utf-8&adpicid=&st=-1&z=&ic=0&word=%s&s=&se=&tab=&width=&height=&face=0&istype=2&qc=&nc=1&fr=&pn=%02d&rn=60", keyWord, keyWord, begin);

	m_webCrawler.ProcessHttpRequest(strUrl, true);

	//m_webCrawler.ResponseContent2Uni();

	//CStringW regExpName = L"(?<=\"fromPageTitleEnc\":\")(.*?)(?=\")";
	CStringW regExpName = L"\"fromPageTitleEnc\":\"(.*?)(?=\")";
	m_vecMatchName.clear();
	m_webCrawler.FindTextInHtml(regExpName, m_vecMatchName);

	size_t vecSizeName = m_vecMatchName.size();
	for(int i = 0; i < vecSizeName; i++)
	{
		replace_all(m_vecMatchName.at(i), L"\"fromPageTitleEnc\":\"", L"");
	}

	
	//CStringW regExpPreviewsAddress = L"(?<=\"thumbURL\":\")(.*?)(?=\")";
	CStringW regExpPreviewsAddress = L"\"thumbURL\":\"(.*?)(?=\")";
	m_vecMatchPreviewsAddress.clear();
	m_webCrawler.FindTextInHtml(regExpPreviewsAddress, m_vecMatchPreviewsAddress);

	size_t vecSizePreviewsAddress = m_vecMatchPreviewsAddress.size();
	for(int i = 0; i < vecSizePreviewsAddress; i++)
	{
		replace_all(m_vecMatchPreviewsAddress.at(i), L"\"thumbURL\":\"", L"");
	}

	//CStringW regExpObjAddress = L"(?<=\"objURL\":\")(.*?)(?=\")";
	CStringW regExpObjAddress = L"\"objURL\":\"(.*?)(?=\")";
	m_vecMatchObjAddress.clear();
	m_webCrawler.FindTextInHtml(regExpObjAddress, m_vecMatchObjAddress);

	size_t vecSizeObjAddress = m_vecMatchObjAddress.size();
	for(int i = 0; i < vecSizeObjAddress; i++)
	{
		replace_all(m_vecMatchObjAddress.at(i), L"\"objURL\":\"", L"");
	}
	DecodeObjAddress();

	m_vecJsonEntity.clear();
	size_t vecSize = min(min(m_vecMatchName.size(), m_vecMatchObjAddress.size()), m_vecMatchPreviewsAddress.size());
	for(int i = 0; i < vecSize; i++)
	{
		JsonEntity* jsonEntity = new JsonEntity;
		jsonEntity->Name = UnicodeToUtf8(m_vecMatchName.at(i));
		jsonEntity->objAddress = UnicodeToUtf8(m_vecMatchObjAddress.at(i));
		jsonEntity->previewsAddress = UnicodeToUtf8(m_vecMatchPreviewsAddress.at(i));

		m_vecJsonEntity.push_back(*jsonEntity);
	}
}


tstring WebSearch::JsonResInfoByType(tstring strKeyWord, ENResType enResType, UINT uIBeginCount, UINT uIEndCount)
{
	m_vecJsonEntity.clear();
	m_vecAllPagesJsonEntity.clear();
	m_vecAllPagesJsonEntity2.clear();

	if (uIBeginCount<0)
	{
		uIBeginCount = 0;
	}
	if (uIEndCount<0)
	{
		uIEndCount = 50;
	}
	if (uIEndCount<=uIBeginCount)
	{
		uIBeginCount = 0;
		uIEndCount = 50;
	}

	// 获取全部资源信息
	GetAllPagesResInfo(strKeyWord, enResType, uIBeginCount, uIEndCount);

	Json::Value itemArray(Json::arrayValue);
	vector<JsonEntity>::iterator iter;

	TCHAR chResTypeNa[30];
	TCHAR chResTypeForm[30];
	TCHAR chResTypeUrl[30];

	memset(chResTypeNa, 0, sizeof(chResTypeNa));
	memset(chResTypeForm, 0, sizeof(chResTypeForm));
	memset(chResTypeUrl, 0, sizeof(chResTypeUrl));

	_tcscpy_s(chResTypeNa,g_chResTypeNa[enResType]);
	_tcscpy_s(chResTypeForm, g_chResTypeNa[enResType]);
	_tcscpy_s(chResTypeUrl, g_chResTypeNa[enResType]);

	_tcscat_s(chResTypeNa, _T("Na"));
	_tcscat_s(chResTypeForm, _T("PreAdd"));
	_tcscat_s(chResTypeUrl, _T("ObjAdd"));

	vector<JsonEntity>::iterator iter2;
	iter2 = m_vecAllPagesJsonEntity.begin();
	if (m_vecAllPagesJsonEntity.size()>uIEndCount-uIBeginCount)
	{
		int iTemp = uIEndCount-uIBeginCount;
		m_vecAllPagesJsonEntity2.assign(iter2, iter2+iTemp);
		m_vecAllPagesJsonEntity.clear();
		m_vecAllPagesJsonEntity = m_vecAllPagesJsonEntity2;
	}

	Json::Value json_temp0;
	json_temp0["ResCount"] = Json::Value(m_vecAllPagesJsonEntity.size());

	for (iter=m_vecAllPagesJsonEntity.begin(); iter!=m_vecAllPagesJsonEntity.end(); iter++)  
	{  
		Json::Value json_temp;
		json_temp[Str2Utf8(chResTypeNa).c_str()] = Json::Value((*iter).Name.c_str());
		json_temp[Str2Utf8(chResTypeForm).c_str()] = Json::Value((*iter).previewsAddress.c_str());
		json_temp[Str2Utf8(chResTypeUrl).c_str()] = Json::Value((*iter).objAddress.c_str());
		itemArray.append(json_temp);
	}
	json_temp0["ItemsInfo"] = itemArray;
	tstring strRet =  Utf82Str(json_temp0.toStyledString());
	OutputDebugString(strRet.c_str());
	return strRet;
}