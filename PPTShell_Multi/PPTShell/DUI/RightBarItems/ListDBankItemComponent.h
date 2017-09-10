#pragma once
//
// ListDBankItemComponent.h
// 
#include "DUI/GifAnimUI.h"

class CListDBankItemComponent 
{

protected:
	CGifAnimUI*			m_pTotalLoadGif;				//获取总量gif

	CLabelUI*			m_pTextTotalCount;				//当前文本

	CButtonUI*			m_pContentBtn;					//文本按钮
			
	CHorizontalLayoutUI*m_pCountContainer;
	COptionUI*			m_pContainerIcon;

	int					m_nTotalCount;

	DWORD				m_dwSelectedTextColor;			//选中文本颜色
	DWORD				m_dwSelectedCountColor;			//选中数量颜色
	DWORD				m_dwNormalTextColor;			//默认文本颜色
	DWORD				m_dwNormalCountColor;			//默认数量颜色

	bool				m_bTotalNetLess;				//用来判断网络状态
};