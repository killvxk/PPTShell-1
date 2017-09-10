// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// ("Fluent UI")，该示例仅作为参考资料提供， 
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有 (C) Microsoft Corporation
// 保留所有权利。

// CntrItem.h : CPPTShellCntrItem 类的接口
//

#pragma once

//class CPPTShellDoc;
//class CPPTShellView;

class CPPTShellCntrItem : public COleDocObjectItem
{
	DECLARE_SERIAL(CPPTShellCntrItem)

// 构造函数
public:
	CPPTShellCntrItem(COleDocument* pContainer = NULL);
		// 注意: 允许 pContainer 为 NULL 以启用 IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE 要求类具有带零
		//  参数的构造函数。OLE 项通常是用
		//  非 NULL 文档指针构造的

// 属性
public:
	CDocument* GetDocument()
		{ return reinterpret_cast<CDocument*>(COleDocObjectItem::GetDocument()); }
	//CPPTShellView* GetActiveView()
	//	{ return reinterpret_cast<CPPTShellView*>(COleDocObjectItem::GetActiveView()); }

public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();

protected:
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
//	virtual BOOL OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);

// 实现
public:
	~CPPTShellCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

