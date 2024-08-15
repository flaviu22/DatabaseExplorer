// RedrawHelper.h: interface for the CRedrawHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RedrawHelper_H__3BADA50B_81BF_4C24_AA9A_35A06E1D93C6__INCLUDED_)
#define AFX_RedrawHelper_H__3BADA50B_81BF_4C24_AA9A_35A06E1D93C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRedrawHelper  
{
public:
	CRedrawHelper(CWnd* pWnd)
		:m_pWnd(pWnd)
	{
		ASSERT(NULL != m_pWnd);
		m_pWnd->SetRedraw(FALSE);
	}
	~CRedrawHelper()
	{
		m_pWnd->SetRedraw();
	}
	CRedrawHelper(const CRedrawHelper& rhs) = delete;
	CRedrawHelper& operator=(const CRedrawHelper& rhs) = delete;
	CRedrawHelper(CRedrawHelper&& rhs) = delete;
	CRedrawHelper& operator=(CRedrawHelper&& rhs) = delete;

private:
	CWnd* m_pWnd{ nullptr };
};

#endif // !defined(AFX_RedrawHelper_H__3BADA50B_81BF_4C24_AA9A_35A06E1D93C6__INCLUDED_)
