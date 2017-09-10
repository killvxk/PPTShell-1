//===========================================================================
// FileName:				Stream.cpp
//	
// Desc:					 
//============================================================================
#include "stdafx.h"
#include "Stream.h"
#include "Util.h"

CStream::CStream(char* pBuffer/* = NULL*/, int nBufferSize /*= 0*/)
{
	m_pBuffer				= pBuffer;
	m_nBufferSize			= nBufferSize;
	m_nDataSize				= 0;
	m_nCursor				= 0;
	m_bBufferAllocated		= FALSE;
}

CStream::CStream(int nAllocSize)
{
	m_pBuffer				= (char*)malloc(nAllocSize);
	m_nBufferSize			= nAllocSize;
	m_nDataSize				= 0;
	m_nCursor				= 0;
	m_bBufferAllocated		= TRUE;

	memset(m_pBuffer, 0, nAllocSize);
}

CStream::~CStream()
{
	if( m_bBufferAllocated && m_pBuffer != NULL )
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
		m_bBufferAllocated = FALSE;
	}

}

void CStream::operator=(const CStream& s)
{
	if( m_nBufferSize < s.m_nBufferSize )
	{
		if( m_pBuffer != NULL )
			free(m_pBuffer);

		m_pBuffer = (char*)malloc(s.m_nBufferSize);
	}

	memcpy(m_pBuffer, s.m_pBuffer, s.m_nDataSize);

	m_nBufferSize			= s.m_nBufferSize;
	m_nDataSize				= s.m_nDataSize;
	m_nCursor				= s.m_nCursor;
	m_bBufferAllocated		= s.m_bBufferAllocated;
}

void CStream::operator +=(const CStream& s)
{
	Write(s.m_pBuffer, s.m_nDataSize);
}

void CStream::AllocMemory(int nAllocSize)
{
	m_pBuffer				= (char*)malloc(nAllocSize);
	m_nBufferSize			= nAllocSize;
	m_nDataSize				= 0;
	m_nCursor				= 0;
	m_bBufferAllocated		= TRUE;
}

void CStream::ResetCursor()
{
	m_nCursor = 0;
}

BYTE CStream::ReadByte()
{
	//if( m_nCursor + sizeof(BYTE) > m_nBufferSize )
	//	return 0;

	BYTE val = *(BYTE*)(m_pBuffer+m_nCursor);
	m_nCursor ++;

	return val;
}

WORD CStream::ReadWORD()
{
	//if( m_nCursor + sizeof(WORD) > m_nBufferSize  )
	//	return 0;

	WORD val = *(WORD*)(m_pBuffer+m_nCursor);
	m_nCursor += sizeof(WORD);
  
	return val;
}

DWORD CStream::ReadDWORD()
{
	//if( m_nCursor + sizeof(DWORD) > m_nBufferSize  )
	//	return 0;

	DWORD val = *(DWORD*)(m_pBuffer+m_nCursor);
	m_nCursor += sizeof(DWORD);
 
	return val;
}

int CStream::ReadInt()
{
	//if( m_nCursor + sizeof(int) > m_nBufferSize  )
	//	return 0;

	int val = *(int*)(m_pBuffer+m_nCursor);
	m_nCursor += sizeof(int);
 

	return val;
}

BOOL CStream::ReadBOOL()
{
	BOOL val = *(BOOL*)(m_pBuffer+m_nCursor);
	m_nCursor += sizeof(BOOL);


	return val;
}

float CStream::ReadFloat()
{
	//if( m_nCursor + sizeof(float) > m_nBufferSize  )
	//	return 0;

	float val = *(float*)(m_pBuffer+m_nCursor);
	m_nCursor += sizeof(float);
 

	return val;
}

POINTF CStream::ReadPointF()
{
	float x = ReadFloat();
	float y = ReadFloat();

	POINTF pt;
	pt.x = x;
	pt.y = y;

	return pt;
}


tstring CStream::ReadString()
{
	// zero terminated
	tstring str = tstring((TCHAR*)(m_pBuffer+m_nCursor));

#ifdef _UNICODE	
	m_nCursor += str.length()*2 + 2;
#else
	m_nCursor += str.length() + 1;
#endif

	return str;
}

string CStream::ReadCString()
{
	// zero terminated
	string str = string(m_pBuffer+m_nCursor);
	m_nCursor += str.length() + 1;

	return str;
}

void CStream::Read(char *pOutBuffer, int nBufferLen)
{
	memcpy(pOutBuffer, m_pBuffer+m_nCursor, nBufferLen);
	m_nCursor += nBufferLen;
}

BOOL CStream::WriteByte(BYTE val)
{
	if( m_nCursor + sizeof(BYTE) > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}


	*(BYTE*)(m_pBuffer+m_nCursor) = val;
	m_nCursor += sizeof(BYTE);
	m_nDataSize += sizeof(BYTE);

	return TRUE;
}

BOOL CStream::WriteWORD(WORD val)
{
	if( m_nCursor + sizeof(WORD) > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	*(WORD*)(m_pBuffer+m_nCursor) = val;
	m_nCursor += sizeof(WORD);
	m_nDataSize += sizeof(WORD);

	return TRUE;

}

BOOL CStream::WriteDWORD(DWORD val)
{
	if( m_nCursor + sizeof(DWORD) > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	*(DWORD*)(m_pBuffer+m_nCursor) = val;
	m_nCursor += sizeof(DWORD);
	m_nDataSize += sizeof(DWORD);

	return TRUE;
}


BOOL CStream::WriteInt(int val)
{
	if( m_nCursor + sizeof(int) > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	*(int*)(m_pBuffer+m_nCursor) = val;
	m_nCursor += sizeof(int);
	m_nDataSize += sizeof(int);

	return TRUE;
}

BOOL CStream::WriteBOOL(BOOL val)
{

	if( m_nCursor + sizeof(BOOL) > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	*(BOOL*)(m_pBuffer+m_nCursor) = val;
	m_nCursor += sizeof(BOOL);
	m_nDataSize += sizeof(BOOL);

	return TRUE;
}

BOOL CStream::WriteFloat(float val)
{
	if( m_nCursor + sizeof(float) > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	*(float*)(m_pBuffer+m_nCursor) = val;
	m_nCursor += sizeof(float);
	m_nDataSize += sizeof(float);

	return TRUE;
}

BOOL CStream::WriteString(char *str)
{
	int nLen = strlen(str);

	while( m_nCursor + nLen + 1 > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	// zero terminated
	strcpy(m_pBuffer+m_nCursor, str);
	m_nCursor += nLen + 1;
	m_nDataSize += nLen + 1;

	return TRUE;
}

BOOL CStream::WriteString(tstring str)
{

#ifdef _UNICODE	
	int nLen = str.length()*2;
#else
	int nLen = str.length();
#endif

#ifdef _UNICODE	
	while( m_nCursor + nLen + 2 > m_nBufferSize )
#else
	while( m_nCursor + nLen + 1 > m_nBufferSize )
#endif
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	memcpy(m_pBuffer+m_nCursor, str.c_str(), nLen);

#ifdef _UNICODE
	*(WCHAR*)&m_pBuffer[m_nCursor+nLen] = L'\0';

	m_nCursor += nLen+2;
	m_nDataSize += nLen+2;
#else
	m_pBuffer[m_nCursor+nLen] = '\0';

	m_nCursor += nLen+1;
	m_nDataSize += nLen+1;
#endif

	return TRUE;
}

BOOL CStream::Write(char* pInBuffer, int nBufferLen)
{
	while( m_nCursor + nBufferLen > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	
	memcpy(m_pBuffer+m_nCursor, pInBuffer, nBufferLen);
	m_nCursor += nBufferLen;
	m_nDataSize += nBufferLen;

	return TRUE;
}

BOOL CStream::Write(int nOffset, char* pInBuffer, int nBufferLen)
{
	while( nOffset + nBufferLen > m_nBufferSize )
	{
		if( m_bBufferAllocated )
		{
			// double buffer size
			char* pNewBuf = (char*)realloc(m_pBuffer, m_nBufferSize*2);
			if( pNewBuf == NULL )
				return FALSE;

			m_pBuffer = pNewBuf;
			m_nBufferSize *= 2;
		}
		else
			return FALSE;
	}

	memcpy(m_pBuffer+nOffset, pInBuffer, nBufferLen);
	return TRUE;
}

char* CStream::GetBuffer()
{
	return m_pBuffer;
}

int CStream::GetBufferSize()
{
	return m_nBufferSize;
}

int CStream::GetDataSize()
{
	return m_nDataSize;
}

int CStream::GetCursor()
{
	return m_nCursor;
}