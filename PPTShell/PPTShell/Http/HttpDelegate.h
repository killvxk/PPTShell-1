#pragma once


class CHttpPtrArray
{
public:
	CHttpPtrArray(int iPreallocSize = 0);
	CHttpPtrArray(const CHttpPtrArray& src);
	~CHttpPtrArray();

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

class  CHttpDelegateBase	 
{
public:
	CHttpDelegateBase(void* pObject, void* pFn);
	CHttpDelegateBase(const CHttpDelegateBase& rhs);
	virtual ~CHttpDelegateBase();
	bool Equals(const CHttpDelegateBase& rhs) const;
	bool EqualsWithoutFn(const CHttpDelegateBase& rhs) const;
	bool operator() (void* param);
	virtual CHttpDelegateBase* Copy() const = 0; // add const for gcc

public:
	void* GetFn();
	void* GetObject();
	virtual bool Invoke(void* param) = 0;

private:
	void* m_pObject;
	void* m_pFn;
};

class CHttpDelegateStatic: public CHttpDelegateBase
{
	typedef bool (*Fn)(void*);
public:
	CHttpDelegateStatic(Fn pFn) : CHttpDelegateBase(NULL, pFn) { } 
	CHttpDelegateStatic(const CHttpDelegateStatic& rhs) : CHttpDelegateBase(rhs) { } 
	virtual CHttpDelegateBase* Copy() const { return new CHttpDelegateStatic(*this); }

protected:
	virtual bool Invoke(void* param)
	{
		Fn pFn = (Fn)GetFn();
		return (*pFn)(param); 
	}
};

template <class O, class T>
class CHttpDelegate : public CHttpDelegateBase
{
	typedef bool (T::* Fn)(void*);
public:
	CHttpDelegate(O* pObj, Fn pFn) : CHttpDelegateBase(pObj, &pFn), m_pFn(pFn) { }
	CHttpDelegate(const CHttpDelegate& rhs) : CHttpDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
	virtual CHttpDelegateBase* Copy() const { return new CHttpDelegate(*this); }

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
CHttpDelegate<O, T> MakeHttpDelegate(O* pObject, bool (T::* pFn)(void*))
{
	return CHttpDelegate<O, T>(pObject, pFn);
}

inline CHttpDelegateStatic MakeHttpDelegate(bool (*pFn)(void*))
{
	return CHttpDelegateStatic(pFn); 
}

class CHttpEventSource
{
	typedef bool (*FnType)(void*);
public:
	CHttpEventSource();
	~CHttpEventSource();

	operator bool();
	void operator+= (const CHttpDelegateBase& d); // add const for gcc
	void operator+= (FnType pFn);
	void operator-= (const CHttpDelegateBase& d);
	void operator-= (FnType pFn);
	bool operator() (void* param);
	void clear();
	CHttpPtrArray* GetDelegates();

protected:
	CHttpPtrArray m_aDelegates;
};

