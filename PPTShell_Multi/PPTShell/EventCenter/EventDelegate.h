#pragma once


class CEventPtrArray
{
public:
	CEventPtrArray(int iPreallocSize = 0);
	CEventPtrArray(const CEventPtrArray& src);
	~CEventPtrArray();

	void Empty();
	void Resize(int iSize);
	bool IsEmpty() const;
	int Find(LPVOID iIndex) const;
	bool Add(LPVOID pData);
	bool SetAt(int iIndex, LPVOID pData);
	bool InsertAt(int iIndex, LPVOID pData);
	bool Remove(int iIndex);
	int GetSize() const;
	LPVOID* GetData();

	LPVOID GetAt(int iIndex) const;
	LPVOID operator[] (int nIndex) const;

protected:
	LPVOID* m_ppVoid;
	int m_nCount;
	int m_nAllocated;
};

class CEventDelegateBase	 
{
public:
    CEventDelegateBase(void* pObject, void* pFn);
    CEventDelegateBase(const CEventDelegateBase& rhs);
    virtual ~CEventDelegateBase();
    bool Equals(const CEventDelegateBase& rhs) const;
    bool operator() (void* param);
    virtual CEventDelegateBase* Copy() const = 0; // add const for gcc

public:
    void* GetFn();
    void* GetObject();
    virtual bool Invoke(void* param) = 0;

private:
    void* m_pObject;
    void* m_pFn;
};

class CEventDelegateStatic: public CEventDelegateBase
{
    typedef bool (*Fn)(void*);
public:
    CEventDelegateStatic(Fn pFn) : CEventDelegateBase(NULL, pFn) { } 
    CEventDelegateStatic(const CEventDelegateStatic& rhs) : CEventDelegateBase(rhs) { } 
    virtual CEventDelegateBase* Copy() const { return new CEventDelegateStatic(*this); }

protected:
    virtual bool Invoke(void* param)
    {
        Fn pFn = (Fn)GetFn();
        return (*pFn)(param); 
    }
};

template <class O, class T>
class CEventDelegate : public CEventDelegateBase
{
    typedef bool (T::* Fn)(void*);
public:
    CEventDelegate(O* pObj, Fn pFn) : CEventDelegateBase(pObj, &pFn), m_pFn(pFn) { }
    CEventDelegate(const CEventDelegate& rhs) : CEventDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
    virtual CEventDelegateBase* Copy() const { return new CEventDelegate(*this); }

protected:
    virtual bool Invoke(void* param)
    {
        O* pObject = (O*) GetObject();
        return (pObject->*m_pFn)(param); 
    }  

private:
    Fn m_pFn;
};

template <class O, class T>
CEventDelegate<O, T> MakeEventDelegate(O* pObject, bool (T::* pFn)(void*))
{
    return CEventDelegate<O, T>(pObject, pFn);
}

inline CEventDelegateStatic MakeEventDelegate(bool (*pFn)(void*))
{
    return CEventDelegateStatic(pFn); 
}

class CNotifyEventSource
{
    typedef bool (*FnType)(void*);
public:
    ~CNotifyEventSource();
    operator bool();
    void operator+= (const CEventDelegateBase& d); // add const for gcc
    void operator+= (FnType pFn);
    void operator-= (const CEventDelegateBase& d);
    void operator-= (FnType pFn);
    bool operator() (void* param);

protected:
    CEventPtrArray m_aDelegates;
};

 