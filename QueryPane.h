#pragma once
#include "RichEditPane.h"

class CQueryPane : public CRichEditPane
{
public:
	CQueryPane() = default;
	~CQueryPane() = default;

public:
	CString GetSelText() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryPane)
protected:
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CQueryPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
