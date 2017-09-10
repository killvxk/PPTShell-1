//=========================================================
// FileName:			RSA.h
//
// Desc:
//=========================================================
#ifndef _RSA_H_
#define _RSA_H_

#include "ThirdParty/openssl/rsa.h"
#include "ThirdParty/openssl/pem.h"

class CRSA
{
public:
	CRSA();
	~CRSA();
 
	void   LoadPublicKey(string strPem);
	string EncryptString(string strSource);
	string DecryptString(string strSource);

protected:
	RSA*				m_pRSA;
	
};

#endif