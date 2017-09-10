#pragma once
//
// ListCloudItemComponent.h
// 
#include "DUI/GifAnimUI.h"

class CListCloudItemComponent 
{

protected:
	CGifAnimUI*			m_pTotalLoadGif;				//获取总量gif
	CGifAnimUI*			m_pCurLoadGif;					//获取当前gif

	CLabelUI*			m_pTextCurCount;				//当前总量文本
	CLabelUI*			m_pTextSeparator;				//分隔符
	CLabelUI*			m_pTextTotalCount;				//全部总量文本

	CButtonUI*			m_pContentBtn;					//文本按钮

	CHorizontalLayoutUI*m_pCountContainer;

	COptionUI*			m_pContainerIcon;

	DWORD				m_dwSelectedTextColor;			//选中文本颜色
	DWORD				m_dwSelectedCountColor;			//选中数量颜色
	DWORD				m_dwNormalTextColor;			//默认文本颜色
	DWORD				m_dwNormalCountColor;			//默认数量颜色

	int					m_nCurCount;					
	int					m_nTotalCount;

	bool				m_bCurNetLess;					//用来判断网络状态
	bool				m_bTotalNetLess;				//用来判断网络状态
};