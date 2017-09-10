#pragma once
#include "AssetsLayout.h"

class CAssetsItemComponent
{
protected:
	int				m_nType;								//Item类型		
	tstring			m_strName;								//Item名称

	CGifAnimUI*		m_pCurLoadGif;							//获取当前gif

	CLabelUI*		m_pTextLabel;
	CLabelUI*		m_pCountLabel;
	CGifAnimUI*		m_pGif;

	CAssetsLayout*	m_pLayout;								//PList

	int				m_nCurCount;					

	bool			m_bCurNetLess;							//用来判断网络状态

};