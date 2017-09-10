//-----------------------------------------------------------------------
// FileName:				ChapterTree.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "ChapterTree.h"
#include "Util/Util.h"


CChapterTree::CChapterTree()
{
	m_pRoot = NULL;
}

CChapterTree::~CChapterTree()
{

}

BOOL CChapterTree::CreateTree(string str)
{
	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].asInt() == 0 )
		return FALSE;

	Json::Value& items = root["items"];

	int i = 0;
	tstring strMaterialGuid = Ansi2Str(items[i]["teaching_material"].asCString());

	// root
	m_pRoot = CreateNode(_T(""), strMaterialGuid);
	m_mapNodes[strMaterialGuid] = m_pRoot;

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle		= Ansi2Str(item["title"].asCString());
		tstring strGuid			= Ansi2Str(item["identifier"].asCString());
		tstring strParentGuid	= Ansi2Str(item["parent"].asCString());

		ChapterNode* pNode = CreateNode(strTitle, strGuid);
		ChapterNode* pParentNode = FindNode(strParentGuid);
		
		if( pParentNode != NULL )
		{
			pNode->pParent = pParentNode;

			if( pParentNode->pFirstChild == NULL)
			{
				pParentNode->pFirstChild = pNode;
				pParentNode->pNextChild  = pNode;
				pNode->pPrevSlibing = NULL;
			}
			else
			{
				pParentNode->pNextChild->pNextSlibing = pNode;
				pNode->pPrevSlibing = pParentNode->pNextChild;

				pParentNode->pNextChild = pNode;
			}

			m_mapNodes[strGuid] = pNode;
		}
	}

	return TRUE;
}

BOOL CChapterTree::DestroyTree()
{
	map<tstring, ChapterNode*>::iterator itr;
	for(itr = m_mapNodes.begin(); itr != m_mapNodes.end(); itr++)
	{
		ChapterNode* pNode = itr->second;
		if( pNode != NULL )
		{
			delete pNode;
			pNode = NULL;
		}
	}

	m_mapNodes.clear();
	m_pRoot = NULL;

	return TRUE;
}

BOOL CChapterTree::LoadTree()
{
	tstring strLocalPath = GetLocalPath();

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Setting\\Chapter.dat"), strLocalPath.c_str());

	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, szPath, _T("rb"));
	if( err != 0 )
		return FALSE;


	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pBuffer = new char[size+1];
	memset(pBuffer, 0, size+1);

	fread(pBuffer, size, 1, fp);
	fclose(fp);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(pBuffer, root);
	if( !res )
	{
		delete pBuffer;
		return FALSE;
	}

	//
	Json::Value& items = root["child"];
	int count = items.size();

	m_mapNodes.clear();

	m_pRoot = LoadNode(root);

	delete pBuffer;
	pBuffer = NULL;

	if( m_pRoot == NULL )
		return FALSE;

	return TRUE;
}

BOOL CChapterTree::SaveTree()
{
	if( m_pRoot == NULL || m_pRoot->pFirstChild == NULL )
		return FALSE;

	Json::Value rootItem;
	Json::Value arrayItem(Json::arrayValue);

	rootItem["title"] = Str2Ansi(m_pRoot->strTitle);
	rootItem["guid"] = Str2Ansi(m_pRoot->strGuid);
	rootItem["child"] = arrayItem;

	SaveNode(rootItem, m_pRoot->pFirstChild);

	Json::FastWriter writer;
	string str = writer.write(rootItem);

	tstring strLocalPath = GetLocalPath();

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Setting\\Chapter.dat"), strLocalPath.c_str());

	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, szPath, _T("wb+"));
	if( err != 0 )
		return FALSE;

	fwrite(str.c_str(), str.length(), 1, fp);
	fclose(fp);

	return TRUE;
}

ChapterNode* CChapterTree::CreateNode(tstring strTitle, tstring strGuid)
{
	ChapterNode* pNode = new ChapterNode;
	pNode->strTitle		= strTitle;
	pNode->strGuid		= strGuid;
	pNode->pParent		= NULL;
	pNode->pPrevSlibing	= NULL;
	pNode->pNextSlibing	= NULL;
	pNode->pFirstChild	= NULL;
	pNode->pNextChild	= NULL;

	m_mapNodes[strGuid] = pNode;
	return pNode;
}

ChapterNode* CChapterTree::FindNode(tstring strGuid)
{
	try
	{
		map<tstring, ChapterNode*>::iterator itr = m_mapNodes.find(strGuid);
		if( itr == m_mapNodes.end() )
			return NULL;

		return itr->second;
	}
	catch (...)
	{
	}
	return NULL;
}

ChapterNode* CChapterTree::FindNodeByTitle(tstring strTitle)
{
	map<tstring, ChapterNode*>::iterator itr;
	for(itr = m_mapNodes.begin(); itr != m_mapNodes.end() ;itr++)
	{
		ChapterNode* pNode = itr->second;
		if( strTitle == pNode->strTitle )
			return pNode;
	}
	return NULL;
}


ChapterNode* CChapterTree::GetRootNode()
{
	return m_pRoot;
}

ChapterNode* CChapterTree::LoadNode(Json::Value &item)
{
	tstring strTitle = Ansi2Str(item["title"].asCString());
	tstring strNDCode = Ansi2Str(item["guid"].asCString());
	Json::Value childItems = item["child"];

	ChapterNode* pNode = CreateNode(strTitle, strNDCode);
	if( pNode == NULL )
		return NULL;

	// child
	ChapterNode* pLastChild = NULL;
	for(int i = childItems.size()-1; i >=0 ; i--)
	{
		Json::Value childItem = childItems[i];
		ChapterNode* pChildNode = LoadNode(childItem);

		pChildNode->pParent = pNode;
		pChildNode->pPrevSlibing = pLastChild;

		if( i == childItems.size()-1 )
		{
			pNode->pFirstChild = pChildNode;
			pLastChild = pChildNode;
		}
		else
		{
			pLastChild->pNextSlibing = pChildNode;
			pLastChild = pChildNode;
		}

	}

	return pNode;
}

void CChapterTree::SaveNode(Json::Value &parentItem, ChapterNode *pNode)
{
	if( pNode == NULL )
		return;

	// self
	Json::Value item;
	Json::Value arrayItem(Json::arrayValue);

	item["title"]	= Str2Ansi(pNode->strTitle).c_str();
	item["guid"]	= Str2Ansi(pNode->strGuid).c_str();
	item["child"]	= arrayItem;

	// child
	ChapterNode* pChild = pNode->pFirstChild;
	if( pChild != NULL )
		SaveNode(item, pChild);


	// slibing
	ChapterNode* pSlibing = pNode->pNextSlibing;
	if( pSlibing != NULL )
		SaveNode(parentItem, pSlibing);

	parentItem["child"].append(item);

}