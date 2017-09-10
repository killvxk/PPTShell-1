//-----------------------------------------------------------------------
// FileName:				NDCloudQuestion.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _NDCLOUD_QUESTION_H_
#define _NDCLOUD_QUESTION_H_

#include "Util/Singleton.h"


typedef struct tagQuestionResType
{
	TCHAR* szQuestionResType;			 // 索引
	TCHAR* szQuestionResDescription;      // 描述
	
}TagQuestionResType,*PTagQuestionResType;

enum tagQuestionJsonType{
	BASIC_QUESTION = 1,
	INTERACT_QUESTION
};
enum tagQuestionOptType{
	QUESTIONTYPE_NOEXIST = 1,
	QUESTIONTYPE_EXIST
};

typedef struct tagQuestionJsonResType
{
	tagQuestionJsonType nQuestionType;                       //是否为基础习题
	tstring szQuestionResCode;			 //名称
	tstring szQuestionResType;			 //索引
	tstring szQuestionResLabel;			 //描述

}TagQuestionJsonResType,*PTagQuestionJsonResType;

//static tagQuestionResType tagQuestionResTypeList[] = 
//{
//	{_T("单选题"),_T("choice")},
//	{_T("多选题"),_T("multiplechoice")},
//	{_T("投票题"),_T("vote")},
//	{_T("填空题"),_T("textentry")},
//	{_T("排序题"),_T("order")},
//	{_T("连线题"),_T("match")},
//	{_T("判断题"),_T("judge")},
//	{_T("问答题"),_T("extendedtext")},
//	{_T("拼图题"),_T("graphicgapmatch")},
//	{_T("填空题 "),_T("textentrymultiple")},
//	{_T("作文题"),_T("drawing")},
//	{_T("手写题"),_T("handwrite")},
//	{_T("分类表格题"),_T("gapmatch")},
//	{_T("文本选择题"),_T("inlinechoice")},
//	{_T("复合题"),_T("data")},
//	{_T("阅读题"),_T("reading")},
//	{_T("综合学习题"),_T("comprehensivelearning")},
//	{_T("应用题"),_T("application")},
//	{_T("计算题"),_T("calculation")},
//	{_T("解答题"),_T("explain")},
//	{_T("阅读理解"),_T("readingcomprehension")},
//	{_T("实验与探究题"),_T("experimentandinquiry")},
//	{_T("证明题"),_T("proof")},
//	{_T("所见即所得填空题"),_T("specialcomplextext")},
//	{_T("推断题"),_T("inference")},
//};

static tagQuestionResType tagQuestionResTypeList[] = 
{
	{_T("multiplechoice"),		_T("多选题")},
	{_T("choice"),				_T("单选题")},
	{_T("textentry"),			_T("填空题")},
	{_T("graphicgapmatch"),		_T("拼图题")},
	{_T("match"),				_T("连线题")},
	{_T("order"),				_T("排序题")},
	{_T("judge"),				_T("判断题")},	
	{_T("drawing"),				_T("作文题")},
	{_T("handwrite"),			_T("手写题")},
	{_T("data"),				_T("复合题")},
	{_T("vote"),				_T("投票题")},
	{_T("linkup"),				_T("连连看")},
	{_T("extendedtext"),		_T("问答题")},
	{_T("nd_fillblank"),		_T("选词填空题")},
	{_T("nd_table"),			_T("分类表格题")},
	{_T("nd_arithmetic"),		_T("竖式计算题")},
	{_T("nd_memorycard"),		_T("记忆卡片")},
	{_T("nd_guessword"),		_T("猜词游戏题")},
	{_T("nd_classified"),		_T("分类题")},
	{_T("nd_fraction"),			_T("分式加减题")},
	{_T("nd_textselect"),		_T("文本选择题")},
	{_T("nd_magicbox"),			_T("魔方盒题")},
	{_T("nd_order"),			_T("排序题")},
	{_T("nd_wordpuzzle"),		_T("字谜游戏题")},
	{_T("nd_compare"),			_T("比大小题")},
	{_T("nd_pointsequencing"),	_T("点排序题")},
	{_T("nd_imagemark"),		_T("标签题")},
	{_T("nd_probabilitycard"),	_T("抽卡牌")},
	{_T("nd_catchball"),		_T("摸球题")},
	{_T("nd_balance"),			_T("天平")},
	{_T("nd_handwritequestion"),_T("手写题")}   //添加新手写体类型 2016-01-13
	
};


static tagQuestionResType tagQuestionResTypeListEx[] = 
{
	{_T("choice"),				_T("$RE0201")},
	{_T("multiplechoice"),		_T("$RE0202")},
	{_T("textentry"),			_T("$RE0209")},
	{_T("extendedtext"),		_T("$RE0206")},
	{_T("graphicgapmatch"),		_T("$RE0207")},
	{_T("match"),				_T("$RE0205")},
	{_T("order"),				_T("$RE0204")},
	{_T("judge"),				_T("$RE0203")},	
	{_T("textentrymultiple"),	_T("$RE0216")},	
	{_T("drawing"),				_T("$RE0211")},
	{_T("handwrite"),			_T("$RE0210")},
	{_T("data"),				_T("$RE0208")},
	{_T("vote"),				_T("$RE0225")},
	{_T("gapmatch"),			_T("$RE0215")},
	{_T("inlinechoice"),		_T("$RE0217")},
	{_T("linkup"),				_T("$RE0401")},
	{_T("nd_order"),			_T("$RE0402")},
	{_T("nd_table"),			_T("$RE0403")},
	{_T("nd_wordpuzzle"),		_T("$RE0406")},
	{_T("nd_memorycard"),		_T("$RE0407")},
	{_T("nd_arithmetic"),		_T("$RE0408")},
	{_T("nd_compare"),			_T("$RE0409")},
	{_T("nd_guessword"),		_T("$RE0410")},
	{_T("nd_magicbox"),			_T("$RE0411")},
	{_T("nd_textselect"),		_T("$RE0414")},
	{_T("nd_classified"),		_T("$RE0415")},
	{_T("nd_fraction"),			_T("$RE0416")},
	{_T("nd_pointsequencing"),	_T("$RE0418")},
	{_T("nd_fillblank"),		_T("$RE0421")},
	{_T("nd_sequencefill"),		_T("$RE0423")},
	{_T("nd_imagemark"),		_T("$RE0424")},
	{_T("nd_highlightmark"),	_T("$RE0425")},
	{_T("nd_clock"),			_T("$RE0431")},
	{_T("nd_lego"),				_T("$RE0432")},
	{_T("nd_puzzle"),			_T("$RE0442")},
	{_T("nd_HandwriteQuestion"),_T("$RE0445")},
	{_T("nd_comicdialogue"),	_T("$RE0446")},


};

class CNDCloudQuestion
{
public:
	CNDCloudQuestion(tstring strGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL);
	~CNDCloudQuestion();

	bool PollPackState();
	bool Cancel();
	bool Pause(CHttpDelegateBase* pDelegate);
	bool Resume();

	bool OnPackStateObtained(void* param);
	bool OnPackStarted(void* param);

	bool OnQuestionDownloaded(void* param);
	bool OnQuestionDownloading(void* param);


protected:
	tstring					m_strGuid;
	DWORD					m_dwUserId;
	BOOL					m_bPolling;
	BOOL					m_bPolled;
	BOOL					m_bFinished;		
	BOOL					m_bPacking;
	void*					m_pUserData;
	int						m_nPendingCount;

	CHttpEventSource		m_OnProgress;
	CHttpEventSource		m_OnComplete;

	
	DWORD					m_dwDownloadId;
	
};

//
// CNDCloudQuestionManager
//
class CNDCloudQuestionManager
{
private:
	CNDCloudQuestionManager();
	~CNDCloudQuestionManager();

public:
	BOOL	Initialize();
	BOOL	Destroy();
	BOOL	DownloadQuestion(tstring strQuestionGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL);
	BOOL	CancelDownload(tstring strQuestionGuid);
	BOOL	PauseDownload(tstring strQuestionGuid, CHttpDelegateBase* pDelegate = NULL);
	BOOL	ResumeDownload(tstring strQuestionGuid);

	BOOL	AddQuestion(tstring strChapterGuid, tstring strQuestionType, 
						tstring strMaterialGuid, tstring strCategories, 
						bool bBasicQuestion,CHttpDelegateBase& delegate);

	DWORD	PackQuestion(tstring strQuestionGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate);

	tstring ComposeBasicQuestionEditUrl(tstring strTokenInfo, tstring strUserId, tstring strQuestionGuid, tstring strChapterGuid);
	tstring ComposeInteractQuestionEditUrl(tstring strTokenInfo, tstring strUserId, tstring strQuestionGuid, tstring strChapterGuid, tstring strQuestionType);

	tstring FindQuestionResType(tagQuestionOptType ntype,tstring nUrl);
	TCHAR*  GetQuestionResType(LPCTSTR szQuestionResDescription);
	TCHAR*  GetQuestionResDesc(LPCTSTR szQuestionResType);
	
	BOOL	ConvertQuestionToCourseFile(tstring strGuid, tstring strQuestionType, tstring strQuestionName, tstring strPath);

	BOOL    LoadJsonQuestionResType();
	DECLARE_SINGLETON_CLASS(CNDCloudQuestionManager);


protected:
	void PollPackState();
	static DWORD WINAPI PollPackStateThread(LPARAM lParam);
	
	bool OnQuestionAdded(void* param);
	bool TripleDESEncrypt(unsigned char* data,int datalen, unsigned char* outdata,int* outdatalen, const char* password);
 

protected:
	HANDLE									m_hThread;
	HANDLE									m_hEvent;
	CRITICAL_SECTION						m_Lock;

	// for add question
	bool                                    m_bLoadJson;
	bool									m_bBasicQuestion;
	tstring									m_strUserId;
	tstring									m_strChapterGuid;
	tstring									m_strQuestionType;
	CHttpEventSource						m_AddQuestionNotify;

	map<tstring, CNDCloudQuestion*>			m_mapQuestions;
	vector<TagQuestionJsonResType>			m_vQuestionsType;
	
};

typedef Singleton<CNDCloudQuestionManager>		NDCloudQuestionManager;

#endif