//=========================================================
// FileName:			RSA.cpp
//
// Desc:
//=========================================================
#include "stdafx.h"
#include "Base64.h"
#include "RSA.h"

 
#pragma comment(lib, "ThirdParty/openssl/libeay32.lib")
#pragma comment(lib, "ThirdParty/openssl/ssleay32.lib")


CRSA::CRSA()
{
	m_pRSA = NULL;
}

CRSA::~CRSA()
{
	if( m_pRSA != NULL )
	{
		RSA_free(m_pRSA);
		m_pRSA = NULL;
	}
}

void CRSA::LoadPublicKey(string strPem)
{
	char* p = (char*)strPem.c_str();
	BIO* bio = BIO_new_mem_buf((void*)strPem.c_str(), strPem.length());
	if( bio == NULL )
		return;

	m_pRSA = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	BIO_free_all(bio);


}

string CRSA::EncryptString(string strSource)
{
	if( m_pRSA == NULL )
		return "";

	BYTE buffer[4096] = {0};

	int flen;
	int rsaSize = RSA_size(m_pRSA);
	int len = strSource.length();

	if( rsaSize - 11 <= len )
		flen = rsaSize - 11;
	else
		flen = len;
 

	int outSize = RSA_public_encrypt(flen, (BYTE*)strSource.c_str(), buffer, m_pRSA, RSA_PKCS1_PADDING);
	if( outSize <= 0 )
		return "";

	string str = "";
	CBase64::Encode(buffer, outSize, str);

	return str;
}

string CRSA::DecryptString(string strSource)
{
	if( m_pRSA == NULL )
		return "";


	// base64 decode
	BYTE buffer[4096] = {0};
	unsigned long nOutLen = 4096;

	bool res = CBase64::Decode(strSource, buffer, &nOutLen );

	// RSA_public_decrypt
	BYTE szTemp[4096] = {0};

	int outSize = RSA_public_decrypt(nOutLen, buffer, szTemp, m_pRSA, RSA_PKCS1_PADDING);
	if( outSize <= 0 )
		return "";

	return (char*)szTemp;
}