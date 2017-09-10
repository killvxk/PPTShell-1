//===========================================================================
// FileName:				Singleton.h
//	
// Desc:					 
//============================================================================
#pragma once

#include <memory>
using namespace std;

class CResGuard;
 

template <class T>
class Singleton
{
public:
       static inline T* GetInstance();
      
private:
       Singleton(void){}
       ~Singleton(void){}
       Singleton(const Singleton&){}
       Singleton & operator= (const Singleton &){}
 
       static auto_ptr<T> _instance;
       static CResGuard _rs;
};
 
template <class T>
auto_ptr<T> Singleton<T>::_instance;
 
template <class T>
CResGuard Singleton<T>::_rs;
 
template <class T>
 inline T* Singleton<T>::GetInstance()
{
       if( 0 == _instance.get() )
       {
              CResGuard::CGuard gd(_rs);
              if( 0== _instance.get())
              {
                     _instance.reset ( new T);
              }
       }
       return _instance.get();
}
 
//Class that will implement the singleton mode,
//must use the macro in it's delare file
#define DECLARE_SINGLETON_CLASS( type ) \
       friend class auto_ptr< type >;\
       friend class Singleton< type >;


     
/******************************************************************************
Module:  Interlocked.h
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/
 
 
#pragma once
///////////////////////////////////////////////////////////////////////////////
 
// Instances of this class will be accessed by multiple threads. So,
// all members of this class (except the constructor and destructor)
// must be thread-safe.
class CResGuard {
public:
   CResGuard()  
   { 
	   m_lGrdCnt = 0; 
	   InitializeCriticalSection(&m_cs); 
   }

   ~CResGuard() 
   { 
	   DeleteCriticalSection(&m_cs); 
   }
 
   // IsGuarded is used for debugging
   BOOL IsGuarded() const { return(m_lGrdCnt > 0); }
 
public:
   class CGuard {
   public:
      CGuard(CResGuard& rg) : m_rg(rg) 
	  {
		  m_rg.Guard(); 
	  };
      ~CGuard() 
	  { 
		  m_rg.Unguard(); 
	  }
 
   private:
      CResGuard& m_rg;
   };
 
private:
   void Guard()   
   { 
	   EnterCriticalSection(&m_cs);
	   m_lGrdCnt++; 
   }

   void Unguard() 
   { 
	   m_lGrdCnt--; 
	   LeaveCriticalSection(&m_cs); 
   }
 
   // Guard/Unguard can only be accessed by the nested CGuard class.
   friend class CResGuard::CGuard;
 
private:
   CRITICAL_SECTION m_cs;
   long m_lGrdCnt;   // # of EnterCriticalSection calls
};
 