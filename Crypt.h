// Crypt.h: interface for the CCrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRYPT_H__DB8F3C14_B91F_4DCE_A8B8_443D24FC9D64__INCLUDED_)
#define AFX_CRYPT_H__DB8F3C14_B91F_4DCE_A8B8_443D24FC9D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wincrypt.h>

class CCrypt
{
public:
	CCrypt();
	virtual ~CCrypt();
	virtual BOOL Decrypt(const CByteArray& arrData, CString& sData);
	virtual BOOL Encrypt(const CString& sData, CByteArray& arrData);
	virtual BOOL Decrypt(const CByteArray& arrData, CObject& serializable);
	virtual BOOL Encrypt(const CObject& serializable, CByteArray& arrData);
	virtual BOOL DeriveKey(CString sPassword);

protected:
	virtual BOOL InternalDecrypt(const CByteArray& arrSource);
	virtual BOOL InternalEncrypt(CByteArray& arrDestination);

protected:
	CMemFile m_file;
	HCRYPTKEY m_hKey;
	HCRYPTHASH m_hHash;
	HCRYPTPROV m_hCryptProv;
};

#endif // !defined(AFX_CRYPT_H__DB8F3C14_B91F_4DCE_A8B8_443D24FC9D64__INCLUDED_)
