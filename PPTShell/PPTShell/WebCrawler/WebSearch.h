#pragma once
#include "WebCrawler.h"

typedef struct _tagJsonEntity
{
	string Name;
	string previewsAddress;
	string objAddress;
}JsonEntity;


class WebSearch
{
public:
	WebSearch(void);
	~WebSearch(void);

	typedef enum _tagResType
	{
		enResTypeCourseware = 0,
		enResTypeVedio,
		enResTypePicture,
		enResTypeCartoon,
		enResTypeAudio,
		enResTypeBasedproblem,
		enResTypeInteractivetopic,
		enResTypePPTTemplate,
		enResTypeOthers
	}ENResType;

	void SearchPic(CStringW keyWord, int begin);
	void SearchPPT(CStringW keyWord, int begin);

	void SearchAudio(int index);
	void SearchVideo(int index);

	void BuildJsonEntity4PPT();
	void GetPPTName(std::vector<wstring>& vecOrigin, std::vector<wstring>& vecMatch);

	void DecodeObjAddress();

	wstring& replace_all(wstring& strGoal, const wstring& strOld, const wstring& strNew);

	void GetAllPagesResInfo(tstring strKeyWord, ENResType enResType, UINT uIBeginCount, UINT uIEndCount);

	tstring JsonResInfoByType(tstring strKeyWord, ENResType enResType, UINT uIBeginCount, UINT uIEndCount);

	std::vector<JsonEntity> m_vecJsonEntity;
	std::vector<JsonEntity> m_vecAllPagesJsonEntity;

	std::vector<JsonEntity> m_vecAllPagesJsonEntity2;

private:
	CWebCrawler m_webCrawler;
	
	std::vector<wstring> m_vecTemp;
	std::vector<wstring> m_vecMatchName;
	std::vector<wstring> m_vecMatchPreviewsAddress;
	std::vector<wstring> m_vecMatchObjAddress;
};
