#pragma once
#include "RichEditPane.h"

class CMessagePane : public CRichEditPane
{
public:
	void AppendToLogAndScroll(const CString& str, const COLORREF& color);
	void SetDarkMode(const BOOL bSet);

private:
	int GetNumVisibleLines();

protected:
	//{{AFX_MSG(CMessagePane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
