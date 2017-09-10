#pragma once

class CPraiseInfo
{
public:
	CPraiseInfo(void);
	~CPraiseInfo(void);
	void SetId(int id);
	int GetId();
	void SetNum(int num);
	int GetNum();
	void SetText(tstring text);
	tstring GetText();
	void SetIsParised(bool isParised);
	bool GetIsParised();
private:
	int m_nId;
	tstring m_sText;
	int m_nNum;
	bool m_bIsParised;
};
