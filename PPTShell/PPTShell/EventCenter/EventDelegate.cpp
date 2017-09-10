#include "StdAfx.h"
#include "EventDelegate.h"


CEventDelegateBase::CEventDelegateBase(void* pObject, void* pFn) 
{
    m_pObject = pObject;
    m_pFn = pFn; 
}

CEventDelegateBase::CEventDelegateBase(const CEventDelegateBase& rhs) 
{
    m_pObject = rhs.m_pObject;
    m_pFn = rhs.m_pFn; 
}

CEventDelegateBase::~CEventDelegateBase()
{

}

bool CEventDelegateBase::Equals(const CEventDelegateBase& rhs) const 
{
	// 重新调用MakeEventDelegate 函数地址不相等，暂时取消所有该对象下的所有函数
    return m_pObject == rhs.m_pObject /*&& m_pFn == rhs.m_pFn*/; 
}

bool CEventDelegateBase::operator() (void* param) 
{
    return Invoke(param); 
}

void* CEventDelegateBase::GetFn() 
{
    return m_pFn; 
}

void* CEventDelegateBase::GetObject() 
{
    return m_pObject; 
}

CNotifyEventSource::~CNotifyEventSource()
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CEventDelegateBase* pObject = static_cast<CEventDelegateBase*>(m_aDelegates[i]);
        if( pObject) delete pObject;
    }
}

CNotifyEventSource::operator bool()
{
    return m_aDelegates.GetSize() > 0;
}

void CNotifyEventSource::operator+= (const CEventDelegateBase& d)
{ 
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CEventDelegateBase* pObject = static_cast<CEventDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) return;
    }

    m_aDelegates.Add(d.Copy());
}

void CNotifyEventSource::operator+= (FnType pFn)
{ 
    (*this) += MakeEventDelegate(pFn);
}

void CNotifyEventSource::operator-= (const CEventDelegateBase& d) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CEventDelegateBase* pObject = static_cast<CEventDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) {
            delete pObject;
            m_aDelegates.Remove(i);
            return;
        }
    }
}
void CNotifyEventSource::operator-= (FnType pFn)
{ 
    (*this) -= MakeEventDelegate(pFn);
}

bool CNotifyEventSource::operator() (void* param) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CEventDelegateBase* pObject = static_cast<CEventDelegateBase*>(m_aDelegates[i]);
        if( pObject && !(*pObject)(param) ) return false;
    }
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CEventPtrArray::CEventPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
{
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CEventPtrArray::CEventPtrArray(const CEventPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
{
	for(int i=0; i<src.GetSize(); i++)
		Add(src.GetAt(i));
}

CEventPtrArray::~CEventPtrArray()
{
	if( m_ppVoid != NULL ) free(m_ppVoid);
}

void CEventPtrArray::Empty()
{
	if( m_ppVoid != NULL ) free(m_ppVoid);
	m_ppVoid = NULL;
	m_nCount = m_nAllocated = 0;
}

void CEventPtrArray::Resize(int iSize)
{
	Empty();
	m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
	::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
	m_nAllocated = iSize;
	m_nCount = iSize;
}

bool CEventPtrArray::IsEmpty() const
{
	return m_nCount == 0;
}

bool CEventPtrArray::Add(LPVOID pData)
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

bool CEventPtrArray::InsertAt(int iIndex, LPVOID pData)
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

bool CEventPtrArray::SetAt(int iIndex, LPVOID pData)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return false;
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CEventPtrArray::Remove(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return false;
	if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
	return true;
}

int CEventPtrArray::Find(LPVOID pData) const
{
	for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
	return -1;
}

int CEventPtrArray::GetSize() const
{
	return m_nCount;
}

LPVOID* CEventPtrArray::GetData()
{
	return m_ppVoid;
}

LPVOID CEventPtrArray::GetAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
	return m_ppVoid[iIndex];
}

LPVOID CEventPtrArray::operator[] (int iIndex) const
{
	ASSERT(iIndex>=0 && iIndex<m_nCount);
	return m_ppVoid[iIndex];
}

