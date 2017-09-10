//-----------------------------------------------------------------------
// FileName:				ChapterTree.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _CHAPTER_TREE_H_
#define _CHAPTER_TREE_H_

#include "ThirdParty/json/json.h"

struct ChapterNode
{
	tstring					strTitle;
	tstring					strGuid;
	ChapterNode*			pParent;
	ChapterNode*			pPrevSlibing;
	ChapterNode*			pNextSlibing;
	ChapterNode*			pFirstChild;
	ChapterNode*			pNextChild;		// temp use
};

class CChapterTree
{
public:
	CChapterTree();
	~CChapterTree();

	BOOL			CreateTree(string str);
	BOOL			DestroyTree();
	BOOL			LoadTree();
	BOOL			SaveTree();

	ChapterNode*	GetRootNode();
	ChapterNode*	FindNode(tstring strGuid);
	ChapterNode*	FindNodeByTitle(tstring strTitle);
	

protected:
	ChapterNode*	CreateNode(tstring strTitle, tstring strGuid);
	ChapterNode*	LoadNode(Json::Value& item);
	void			SaveNode(Json::Value& parentItem, ChapterNode* pNode);

protected:
	ChapterNode*					m_pRoot;
	map<tstring, ChapterNode*>		m_mapNodes;
};

#endif