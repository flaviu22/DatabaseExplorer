#pragma once
#include "RichEditPane.h"

class CMessagePane : public CRichEditPane
{
public:
	void AppendToLogAndScroll(const CString& str, const COLORREF& color);

private:
	int GetNumVisibleLines();

protected:
	//{{AFX_MSG(CMessagePane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

