#include "stdafx.h"
#include "HttpDelegate.h"

 

CHttpDelegateBase::CHttpDelegateBase(void* pObject, void* pFn) 
{
	m_pObject = pObject;
	m_pFn = pFn; 
}

CHttpDelegateBase::CHttpDelegateBase(const CHttpDelegateBase& rhs) 
{
	m_pObject = rhs.m_pObject;
	m_pFn = rhs.m_pFn; 
}

CHttpDelegateBase::~CHttpDelegateBase()
{

}

bool CHttpDelegateBase::Equals(const CHttpDelegateBase& rhs) const 
{
	// 重新调用MakeEventDelegate 函数地址不相等，暂时取消所有该对象下的所有函数
	return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool CHttpDelegateBase::EqualsWithoutFn( const CHttpDelegateBase& rhs ) const
{
	return m_pObject == rhs.m_pObject;
}

bool CHttpDelegateBase::operator() (void* param) 
{
	return Invoke(param); 
}

void* CHttpDelegateBase::GetFn() 
{
	return m_pFn; 
}

void* CHttpDelegateBase::GetObject() 
{
	return m_pObject; 
}



CHttpEventSource::CHttpEventSource()
{
	 
}

CHttpEventSource::~CHttpEventSource()
{
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CHttpDelegateBase* pObject = static_cast<CHttpDelegateBase*>(m_aDelegates[i]);
		if( pObject) delete pObject;
	}
}

CHttpEventSource::operator bool()
{
	 return m_aDelegates.GetSize() > 0;
}

void CHttpEventSource::operator+= (const CHttpDelegateBase& d)
{ 
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CHttpDelegateBase* pObject = static_cast<CHttpDelegateBase*>(m_aDelegates[i]);
		if( pObject && pObject->Equals(d) ) 
			return;
	}

	m_aDelegates.Add(d.Copy());
}

void CHttpEventSource::operator+= (FnType pFn)
{ 
	(*this) += MakeHttpDelegate(pFn);
}

void CHttpEventSource::operator-= (const CHttpDelegateBase& d) 
{
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CHttpDelegateBase* pObject = static_cast<CHttpDelegateBase*>(m_aDelegates[i]);
		if( pObject && pObject->EqualsWithoutFn(d) ) {
			delete pObject;
			m_aDelegates.Remove(i);
			return;
		}
	}
}
void CHttpEventSource::operator-= (FnType pFn)
{ 
	(*this) -= MakeHttpDelegate(pFn);
}


bool CHttpEventSource::operator() (void* param) 
{
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) 
	{
		CHttpDelegateBase* pObject = static_cast<CHttpDelegateBase*>(m_aDelegates[i]);
		if( pObject && !(*pObject)(param) ) 
			return false;
	}

	return true;
}

void CHttpEventSource::clear()
{
	for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
		CHttpDelegateBase* pObject = static_cast<CHttpDelegateBase*>(m_aDelegates[i]);
		if( pObject) delete pObject;
	}

	m_aDelegates.Empty();
}

CHttpPtrArray* CHttpEventSource::GetDelegates()
{
	return &m_aDelegates;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CHttpPtrArray::CHttpPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
{
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CHttpPtrArray::CHttpPtrArray(const CHttpPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
{
	for(int i=0; i<src.GetSize(); i++)
		Add(src.GetAt(i));
}

CHttpPtrArray::~CHttpPtrArray()
{
	if( m_ppVoid != NULL ) free(m_ppVoid);
}

void CHttpPtrArray::Empty()
{
	if( m_ppVoid != NULL ) free(m_ppVoid);
	m_ppVoid = NULL;
	m_nCount = m_nAllocated = 0;
}

void CHttpPtrArray::Resize(int iSize)
{
	Empty();
	m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
	::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
	m_nAllocated = iSize;
	m_nCount = iSize;
}

bool CHttpPtrArray::IsEmpty() const
{
	return m_nCount == 0;
}

bool CHttpPtrArray::Add(LPVOID pData)
{
	if( ++m_nCount >= m_nAllocated) {
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 ) nAllocated = 11;
		LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
		if( ppVoid != NULL ) {
			m_nAllocated = nAllocated;
			m_ppVoid = ppVoid;
		}
		else {
			--m_nCount;
			return false;
		}
	}
	m_ppVoid[m_nCount - 1] = pData;
	return true;
}

bool CHttpPtrArray::InsertAt(int iIndex, LPVOID pData)
{
	if( iIndex == m_nCount ) return Add(pData);
	if( iIndex < 0 || iIndex > m_nCount ) return false;
	if( ++m_nCount >= m_nAllocated) {
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 ) nAllocated = 11;
		LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
		if( ppVoid != NULL ) {
			m_nAllocated = nAllocated;
			m_ppVoid = ppVoid;
		}
		else {
			--m_nCount;
			return false;
		}
	}
	memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CHttpPtrArray::SetAt(int iIndex, LPVOID pData)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return false;
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CHttpPtrArray::Remove(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return false;
	if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
	return true;
}

int CHttpPtrArray::Find(LPVOID pData) const
{
	for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
	return -1;
}

int CHttpPtrArray::GetSize() const
{
	return m_nCount;
}

LPVOID* CHttpPtrArray::GetData()
{
	return m_ppVoid;
}

LPVOID CHttpPtrArray::GetAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
	return m_ppVoid[iIndex];
}

LPVOID CHttpPtrArray::operator[] (int iIndex) const
{
	ASSERT(iIndex>=0 && iIndex<m_nCount);
	return m_ppVoid[iIndex];
}

