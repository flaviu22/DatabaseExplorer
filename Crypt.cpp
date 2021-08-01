// Crypt.cpp: implementation of the CCrypt class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Crypt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrypt::CCrypt()
	:m_hKey(NULL)
	,m_hHash(NULL)
	,m_hCryptProv(NULL)
{
	if(! CryptAcquireContext(&m_hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
		return;
	if(! CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &m_hHash))
		return;
}

CCrypt::~CCrypt()
{
	m_file.Close();
	if(NULL != m_hHash)
		::CryptDestroyHash(m_hHash);
	if(NULL != m_hKey)
		::CryptDestroyKey(m_hKey);
	if(NULL != m_hCryptProv)
		::CryptReleaseContext(m_hCryptProv, 0);
}

BOOL CCrypt::InternalEncrypt(CByteArray& arrDestination)
{
	ULONGLONG uLength = (ULONGLONG)m_file.GetLength();
	ULONGLONG uCapacity = uLength * 2;
	m_file.SetLength((ULONG)uCapacity);

	BYTE* pData = m_file.Detach();

	DWORD dwDataLength = static_cast<DWORD>(uLength);

	if(! ::CryptEncrypt(m_hKey, NULL, TRUE, 0, pData, &dwDataLength, static_cast<DWORD>(uCapacity)))
	{
		delete[] pData;
		return FALSE;
	}

	arrDestination.RemoveAll();
	arrDestination.SetSize(static_cast<INT_PTR>(dwDataLength));
	memcpy(arrDestination.GetData(), pData, dwDataLength);

	delete[] pData;

	return TRUE;
}

BOOL CCrypt::InternalDecrypt(const CByteArray& arrSource)
{
	m_file.SetLength(0);

	m_file.Write(arrSource.GetData(), static_cast<UINT>(arrSource.GetSize()));
	m_file.Flush();

	BYTE* pData = m_file.Detach();

	DWORD dwDataLength = static_cast<DWORD>(arrSource.GetSize());

	if(! ::CryptDecrypt(m_hKey, NULL, TRUE, 0, pData, &dwDataLength))
	{
		delete[] pData;
		return FALSE;
	}

	m_file.SetLength(dwDataLength);
	m_file.Write(pData, dwDataLength);
	m_file.Flush();
	m_file.SeekToBegin();

	delete[] pData;

	return TRUE;
}

BOOL CCrypt::DeriveKey(CString sPassword)
{
	if(NULL == m_hCryptProv || NULL == m_hHash)
		return FALSE;

	if(NULL != m_hHash)
	{
		CryptDestroyHash(m_hHash);
		m_hHash = NULL;
		if(! CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &m_hHash))
			return FALSE;
	}

	if(NULL != m_hKey)
	{
		::CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}

	if(! CryptHashData(m_hHash, (const BYTE*)sPassword.GetBuffer(_MAX_PATH), sPassword.GetLength() * sizeof(TCHAR), 0))
		return FALSE;
	sPassword.ReleaseBuffer();
	if(! CryptDeriveKey(m_hCryptProv, CALG_RC2, m_hHash, CRYPT_EXPORTABLE, &m_hKey))
		return FALSE;

	return TRUE;
}

BOOL CCrypt::Encrypt(const CObject& serializable, CByteArray& arrData)
{
	if(NULL == m_hCryptProv || NULL == m_hKey)
		return FALSE;
	if(! serializable.IsSerializable())
		return FALSE;

	m_file.SetLength(0);

	try
	{
		CArchive ar(&m_file, CArchive::store);
		const_cast<CObject&>(serializable).Serialize(ar);
		ar.Close();
	}
	catch (...) {}

	return InternalEncrypt(arrData);
}

BOOL CCrypt::Decrypt(const CByteArray& arrData, CObject& serializable)
{
	if(NULL == m_hCryptProv || NULL == m_hKey)
		return FALSE;
	if(! serializable.IsSerializable())
		return FALSE;
	if(! InternalDecrypt(arrData))
		return FALSE;

	try
	{
		CArchive ar(&m_file, CArchive::load);
		serializable.Serialize(ar);
		ar.Close();
	}
	catch (...) {}

	return TRUE;
}

BOOL CCrypt::Encrypt(const CString& sData, CByteArray& arrData)
{
	if(NULL == m_hCryptProv || NULL == m_hKey)
		return FALSE;

	m_file.SetLength(0);

	try
	{
		CArchive ar(&m_file, CArchive::store);
		ar << sData;
		ar.Close();
	}
	catch (...) {}

	return InternalEncrypt(arrData);
}

BOOL CCrypt::Decrypt(const CByteArray& arrData, CString& sData)
{
	if(NULL == m_hCryptProv || NULL == m_hKey)
		return FALSE;
	if(! InternalDecrypt(arrData))
		return FALSE;

	try
	{
		CArchive ar(&m_file, CArchive::load);
		ar >> sData;
		ar.Close();
	}
	catch (...) {}

	return TRUE;
}
