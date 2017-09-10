//-----------------------------------------------------------------------
// FileName:				CategoryTree.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "CategoryTree.h"
#include "Util/Util.h"


CCategoryTree::CCategoryTree()
{
	m_pRoot		= NULL;
}

CCategoryTree::~CCategoryTree()
{

}

BOOL CCategoryTree::CreateTree(string strJson)
{
	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(strJson, root);
	if( !res )
		return FALSE;

	m_pRoot = CreateNode(_T(""), _T(""), _T("$CourseRootNode"));

	// 
	// level: 1 小学 初中 高中  / 一年级 二年级 
	// level: 2 语文 数学 英语  / null
	// level: 3 人教 / 上下册
	//
	int nLevelCount = root.size();
	CategoryNode* pGradeOneNode = NULL;
	CategoryNode* pChineseNode = NULL;
 
	for(int i = 0; i < nLevelCount; i++)
	{
		int level = root[i]["level"].asInt();
		Json::Value& items = root[i]["items"];

		if( level == 1 )
		{
			// 小学/初中/高中
			for(int j = 0; j < (int)items.size(); j++)
			{
				CategoryNode* pSectionNode = CreateSectionNode(items[j]);
				LinkChildNode(pSectionNode, m_pRoot);

				if( j == 0 )
					pGradeOneNode = pSectionNode->pFirstChild;
			}

			m_pRoot->pNextChild = NULL;
		}
		else if( level == 2 )
		{
			// 小学一年级的课程
			for(int j = 0; j < (int)items.size(); j++)
			{
				Json::Value& item = items[j];
 
				tstring strGuid				= Ansi2Str(item["identifier"].asCString());
				tstring strCourseTitle		= Ansi2Str(item["target"]["title"].asCString());
				tstring strCourseNDCode		= Ansi2Str(item["target"]["nd_code"].asCString());

				CategoryNode* pCourseNode = CreateNode(strGuid, strCourseTitle, strCourseNDCode);
				LinkChildNode(pCourseNode, pGradeOneNode);

 				if( j == 0 )
 					pChineseNode = pCourseNode;
			}

		}
		else if( level == 3 )
		{
			// 小学一年级语文的版本和子版本
			for(int j = 0; j < (int)items.size(); j++)
			{
				CategoryNode* pEditionNode = CreateEditionNode(items[j]);
				LinkChildNode(pEditionNode, pChineseNode);
			}
		}
	}

	return TRUE;
}

BOOL CCategoryTree::AddTreeNode(string strJson, tstring strSection, tstring strGrade, tstring strCourse, tstring strEdition)
{	
	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(strJson, root);
	if( !res )
		return FALSE;

	// find out this node
	CategoryNode* pParentNode = FindNode(strSection, strGrade, strCourse, strEdition);
	if( pParentNode == NULL )
		return FALSE;

	CategoryNode* pChineseNode = NULL;
	int nLevelCount = root.size();
	for(int i = 0; i < nLevelCount; i++)
	{
		int level = root[i]["level"].asInt();
		Json::Value& items = root[i]["items"];

	
		if( level == 2 )
		{
			for(int j = 0; j < (int)items.size(); j++)
			{
				Json::Value& item = items[j];

				tstring strGuid				= Ansi2Str(item["identifier"].asCString());
				tstring strCourseTitle		= Ansi2Str(item["target"]["title"].asCString());
				tstring strCourseNDCode		= Ansi2Str(item["target"]["nd_code"].asCString());

				CategoryNode* pCourseNode = CreateNode(strGuid, strCourseTitle, strCourseNDCode);
				LinkChildNode(pCourseNode, pParentNode);

				if( j == 0 )
					pChineseNode = pCourseNode;
			}
		}
		else if( level == 3 )
		{
			// 语文的版本和子版本
			for(int j = 0; j < (int)items.size(); j++)
			{
				CategoryNode* pEditionNode = CreateEditionNode(items[j]);

				if( pChineseNode != NULL )
					LinkChildNode(pEditionNode, pChineseNode);
				else
					LinkChildNode(pEditionNode, pParentNode);
			}
		}
	}

	return TRUE;
}

BOOL CCategoryTree::DestroyTree()
{
	DestroyNode(m_pRoot);
	return TRUE;
}

BOOL CCategoryTree::LoadTree()
{
	tstring strLocalPath = GetLocalPath();

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Setting\\Category.dat"), strLocalPath.c_str());

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
	
	m_pRoot = LoadNode(root);

	delete pBuffer;
	pBuffer = NULL;

	return TRUE;

}

BOOL CCategoryTree::SaveTree()
{
	if( m_pRoot == NULL || m_pRoot->pFirstChild == NULL )
		return FALSE;

	Json::Value rootItem;
	Json::Value arrayItem(Json::arrayValue);

	rootItem["guid"]	= Str2Ansi(m_pRoot->strGuid);
	rootItem["title"]	= Str2Ansi(m_pRoot->strTitle);
	rootItem["ndcode"]	= Str2Ansi(m_pRoot->strNDCode);
	rootItem["child"]	= arrayItem;

	SaveNode(rootItem, m_pRoot->pFirstChild);
	
	Json::FastWriter writer;
	string str = writer.write(rootItem);

	tstring strLocalPath = GetLocalPath();

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Setting\\Category.dat"), strLocalPath.c_str());

	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, szPath, _T("wb+"));
	if( err != 0 )
		return FALSE;


	fwrite(str.c_str(), str.length(), 1, fp);
	fclose(fp);

	return TRUE;
}

CategoryNode* CCategoryTree::CreateSectionNode(Json::Value &item)
{
	tstring strGuid				= Ansi2Str(item["identifier"].asCString());
	tstring strSectionTitle		= Ansi2Str(item["target"]["title"].asCString());
	tstring strSectionNDCode	= Ansi2Str(item["target"]["nd_code"].asCString());

	CategoryNode* pSectionNode = CreateNode(strGuid, strSectionTitle, strSectionNDCode);
	if( pSectionNode == NULL )
		return NULL;

	Json::Value& level_items = item["level_items"];

	for(int i = 0; i < (int)level_items.size(); i++)
	{
		Json::Value& levelitem = level_items[i];

		tstring strGuid        = Ansi2Str(levelitem["identifier"].asCString());
		tstring strGradeTitle  = Ansi2Str(levelitem["target"]["title"].asCString());
		tstring strGradeNDCode = Ansi2Str(levelitem["target"]["nd_code"].asCString());

		CategoryNode* pGradeNode = CreateNode(strGuid, strGradeTitle, strGradeNDCode);
		if( pGradeNode == NULL )
			return NULL;

		LinkChildNode(pGradeNode, pSectionNode);
	}

	// 高中
	if( strSectionNDCode == _T("$ON040000") )
	{
		TCHAR* szGradeName[] = 
		{	
			_T("十年级"),
			_T("十一年级"),
			_T("十二年级"),
		};

		for(int i = 0; i < 3; i++)
		{
			CategoryNode* pGradeNode = CreateNode(_T(""), szGradeName[i], _T("$ON040000"));
			if( pGradeNode == NULL )
				return NULL;

			LinkChildNode(pGradeNode, pSectionNode);

		}
	}


	pSectionNode->pNextChild = NULL;
	return pSectionNode;
}	

CategoryNode* CCategoryTree::CreateEditionNode(Json::Value& item)
{
	tstring strGuid				= Ansi2Str(item["identifier"].asCString());
	tstring strEditionTitle		= Ansi2Str(item["target"]["title"].asCString());
	tstring strEditionNDCode	= Ansi2Str(item["target"]["nd_code"].asCString());

	CategoryNode* pEditionNode = CreateNode(strGuid, strEditionTitle, strEditionNDCode);
	if( pEditionNode == NULL )
		return NULL;

	Json::Value& level_items = item["level_items"];

	for(int i = 0; i < (int)level_items.size(); i++)
	{
		Json::Value& levelitem = level_items[i];

		tstring strGuid				= Ansi2Str(levelitem["identifier"].asCString());
		tstring strSubEditionTitle  = Ansi2Str(levelitem["target"]["title"].asCString());
		tstring strSubEditionNDCode = Ansi2Str(levelitem["target"]["nd_code"].asCString());

		CategoryNode* pSubEditionNode = CreateNode(strGuid, strSubEditionTitle, strSubEditionNDCode);
		if( pSubEditionNode == NULL )
			return NULL;

		LinkChildNode(pSubEditionNode, pEditionNode);
	}

	pEditionNode->pNextChild = NULL;
	return pEditionNode;
}

void CCategoryTree::LinkChildNode(CategoryNode *pNode, CategoryNode *pParent)
{
	if( pNode == NULL || pParent == NULL )
		return;

	pNode->pParent = pParent;

	if( pParent->pFirstChild == NULL)
	{
		pParent->pFirstChild = pNode;
		pParent->pNextChild  = pNode;
	}
	else
	{
		if(pParent->pNextChild != NULL)
		{		
			pParent->pNextChild->pNextSlibing = pNode;
			pParent->pNextChild = pNode;
		}

	}
		
}


CategoryNode* CCategoryTree::CreateNode(tstring strGuid, tstring strTitle, tstring strNDCode)
{
	CategoryNode* pNode = new CategoryNode;

	pNode->strGuid		= strGuid;
	pNode->strTitle		= strTitle;
	pNode->strNDCode	= strNDCode;
	pNode->pParent		= NULL;
	pNode->pNextSlibing = NULL;
	pNode->pFirstChild	= NULL;
	pNode->pNextChild	= NULL;

	return pNode;
}

void CCategoryTree::DestroyNode(CategoryNode* pNode)
{
	if( pNode == NULL )
		return;

	// child
	CategoryNode* pChild = pNode->pFirstChild;
	if( pChild != NULL )
		DestroyNode(pChild);

	// slibing
	CategoryNode* pSlibing = pNode->pNextSlibing;
	if( pSlibing != NULL )
		DestroyNode(pSlibing);

	delete pNode;
	pNode = NULL;
}

CategoryNode* CCategoryTree::LoadNode(Json::Value& item)
{
	tstring strGuid;

	if( !item["guid"].isNull() )
		strGuid = Ansi2Str(item["guid"].asCString());

	tstring strTitle = Ansi2Str(item["title"].asCString());
	tstring strNDCode = Ansi2Str(item["ndcode"].asCString());

	Json::Value childItems = item["child"];

	CategoryNode* pNode = CreateNode(strGuid, strTitle, strNDCode);
	if( pNode == NULL )
		return NULL;

	// child
	CategoryNode* pLastChild = NULL;
	for(int i = childItems.size()-1; i >=0 ; i--)
	{
		Json::Value childItem = childItems[i];
		CategoryNode* pChildNode = LoadNode(childItem);

		pChildNode->pParent = pNode;

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

void CCategoryTree::SaveNode(Json::Value& parentItem, CategoryNode* pNode)
{
	if( pNode == NULL )
		return;

	// self
	Json::Value item;
	Json::Value arrayItem(Json::arrayValue);

	item["guid"]	= Str2Ansi(pNode->strGuid);
	item["title"]	= Str2Ansi(pNode->strTitle);
	item["ndcode"]	= Str2Ansi(pNode->strNDCode);
	item["child"]	= arrayItem;
 
	// child
	CategoryNode* pChild = pNode->pFirstChild;
	if( pChild != NULL )
		SaveNode(item, pChild);


	// slibing
	CategoryNode* pSlibing = pNode->pNextSlibing;
	if( pSlibing != NULL )
		SaveNode(parentItem, pSlibing);
	
	parentItem["child"].append(item);


}

CategoryNode* CCategoryTree::FindNode(tstring strSection, tstring strGrade, tstring strCourse, tstring strEdition, tstring strSubEdition)
{
	if( m_pRoot == NULL )
		return NULL;

	// section
	CategoryNode* pSection = m_pRoot->pFirstChild;
	CategoryNode* pFoundSection = NULL;

	while( pSection != NULL )
	{
		if( pSection->strNDCode == strSection )
		{
			pFoundSection = pSection;
			break;
		}

		pSection = pSection->pNextSlibing;
	}

	// grade
	if( strGrade == _T("") )
		return pFoundSection;

	if( pFoundSection == NULL )
		return NULL;

	CategoryNode* pGradeNode = pFoundSection->pFirstChild;


	CategoryNode* pFoundGradeNode = NULL;
	while( pGradeNode != NULL )
	{
		if( pGradeNode->strNDCode == strGrade )
		{
			pFoundGradeNode = pGradeNode;
			break;
		}

		pGradeNode = pGradeNode->pNextSlibing;
	}

	// course
	if( strCourse == _T("") )
		return pFoundGradeNode;

	if( pFoundGradeNode == NULL )
		return NULL;


	CategoryNode* pCourseNode = pFoundGradeNode->pFirstChild;
	CategoryNode* pFoundCourseNode = NULL;

	while( pCourseNode != NULL )
	{
		if( pCourseNode->strNDCode == strCourse )
		{
			pFoundCourseNode = pCourseNode;
			break;
		}

		pCourseNode = pCourseNode->pNextSlibing;
	}

	// edition
	if( strEdition == _T("") )
		return pFoundCourseNode;

	if( pFoundCourseNode == NULL )
		return NULL;

	CategoryNode* pFoundEditionNode = NULL;

	CategoryNode* pEditionNode = pFoundCourseNode->pFirstChild;
	while( pEditionNode != NULL )
	{
		if( pEditionNode->strNDCode == strEdition )
		{
			pFoundEditionNode = pEditionNode;
			break;
		}

		pEditionNode = pEditionNode->pNextSlibing;
	}

	// sub edition
	if( strSubEdition == _T("") )
		return pFoundEditionNode;

	if( pFoundEditionNode == NULL )
		return NULL;

	CategoryNode* pFoundSubEditionNode = NULL;

	CategoryNode* pSubEditionNode = pFoundEditionNode->pFirstChild;
	while( pSubEditionNode != NULL )
	{
		if( pSubEditionNode->strNDCode == strSubEdition )
		{
			pFoundSubEditionNode = pSubEditionNode;
			break;
		}

		pSubEditionNode = pSubEditionNode->pNextSlibing;
	}


	return pFoundSubEditionNode;

}


CategoryNode* CCategoryTree::GetRootNode()
{
	return m_pRoot;
}

CategoryNode* CCategoryTree::GetSelectedNode()
{
	CategoryNode* pNode = FindNode(m_strSelSectionCode, m_strSelGradeCode, m_strSelCourseCode, m_strSelEditionCode);
	return pNode;
}

void CCategoryTree::SelectSectionCode(tstring strSectionCode)
{
	m_strSelSectionCode = strSectionCode;
}

void CCategoryTree::SelectGradeCode(tstring strGradeCode)
{
	m_strSelGradeCode = strGradeCode;
}

void CCategoryTree::SelectCourseCode(tstring strCourseCode)
{
	m_strSelCourseCode = strCourseCode;
}

void CCategoryTree::SelectEditionCode(tstring strEditionCode)
{
	m_strSelEditionCode = strEditionCode;
}

void CCategoryTree::SelectSubEditionCode(tstring strSubEditionCode)
{
	m_strSelSubEditionCode = strSubEditionCode;
}

tstring CCategoryTree::GetSelectedSectionCode()
{
	return m_strSelSectionCode;
}

tstring CCategoryTree::GetSelectedGradeCode()
{
	return m_strSelGradeCode;
}

tstring CCategoryTree::GetSelectedCourseCode()
{
	return m_strSelCourseCode;
}

tstring CCategoryTree::GetSelectedEditionCode()
{
	return m_strSelEditionCode;
}

tstring CCategoryTree::GetSelectedSubEditionCode()
{
	return m_strSelSubEditionCode;
}

tstring CCategoryTree::GetSelectedFullCode()
{
	tstring str = m_strSelSectionCode;
	str += _T("/");
	str += m_strSelGradeCode;
	str += _T("/");
	str += m_strSelCourseCode;
	str += _T("/");
	str += m_strSelEditionCode;
	str += _T("/");
	str += m_strSelSubEditionCode;

	return str;
}

tstring CCategoryTree::GetSelectedCourseGuid()
{
	CategoryNode* pNode = FindNode(m_strSelSectionCode, m_strSelGradeCode, m_strSelCourseCode);
	if( pNode == NULL )
		return _T("");

	return pNode->strGuid;
}