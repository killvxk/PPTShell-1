
// PPTShellDoc.h : interface of the CPPTShellDoc class
//


#pragma once


class CPPTShellDoc : public CDocument
{
protected: // create from serialization only
	CPPTShellDoc();
	DECLARE_DYNCREATE(CPPTShellDoc)

// Attributes
public:

// Operations
public:
	BOOL NewDocument();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName, BOOL bReadOnly = FALSE);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CPPTShellDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


