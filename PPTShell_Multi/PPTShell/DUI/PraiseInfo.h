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
	void SetText(string text);
	string GetText();
	void SetIsParised(bool isParised);
	bool GetIsParised();
private:
	int m_nId;
	string m_sText;
	int m_nNum;
	bool m_bIsParised;
};
