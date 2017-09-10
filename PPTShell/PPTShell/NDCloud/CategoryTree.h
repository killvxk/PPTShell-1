//-----------------------------------------------------------------------
// FileName:				CategoryTree.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _COURSE_TREE_H_
#define _COURSE_TREE_H_

#include "ThirdParty/json/json.h"

struct CategoryNode
{
	tstring				strGuid;
	tstring				strTitle;
	tstring				strNDCode;	
	CategoryNode*		pParent;
	CategoryNode*		pNextSlibing;
	CategoryNode*		pFirstChild;
	CategoryNode*		pNextChild;		// temp use
	
};

class CCategoryTree
{
public:
	CCategoryTree();
	~CCategoryTree();

	BOOL			CreateTree(string strJson);
	BOOL			AddTreeNode(string strJson, tstring strSection, tstring strGrade, tstring strCourse, tstring strEdition);
	BOOL			DestroyTree();
	BOOL			LoadTree();
	BOOL			SaveTree();

	CategoryNode*	GetRootNode();
	CategoryNode*	GetSelectedNode();
	CategoryNode*	FindNode(tstring strSection, tstring strGrade = _T(""), tstring strCourse = _T(""), tstring strEdition = _T(""), tstring strSubEdition = _T(""));
	

	// user selection
	void			SelectSectionCode(tstring strSectionCode);
	void			SelectGradeCode(tstring strGradeCode);
	void			SelectCourseCode(tstring strCourseCode);
	void			SelectEditionCode(tstring strEditionCode);
	void			SelectSubEditionCode(tstring strSubEditionCode);

	tstring			GetSelectedSectionCode();
	tstring			GetSelectedGradeCode();
	tstring			GetSelectedCourseCode();
	tstring			GetSelectedEditionCode();
	tstring			GetSelectedSubEditionCode();
	tstring			GetSelectedFullCode();

	tstring			GetSelectedCourseGuid();

protected:
	CategoryNode*	CreateNode(tstring strGuid, tstring strTitle, tstring strNDCode);
	CategoryNode*	CreateSectionNode(Json::Value& item);
	CategoryNode*	CreateEditionNode(Json::Value& item);
	void			LinkChildNode(CategoryNode* pNode, CategoryNode* pParent);
	void			DestroyNode(CategoryNode* pNode);
	CategoryNode*	LoadNode(Json::Value& item);
	void			SaveNode(Json::Value& parentItem, CategoryNode* pNode);

protected:
	CategoryNode*		m_pRoot;

	tstring				m_strSelSectionCode;
	tstring				m_strSelGradeCode;
	tstring				m_strSelCourseCode;
	tstring				m_strSelEditionCode;
	tstring				m_strSelSubEditionCode;
	
};

#endif