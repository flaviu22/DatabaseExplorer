#pragma once
#include "RichEditPane.h"
#include <vector>

class CQueryPane : public CRichEditPane
{
public:
	CQueryPane() = default;
	~CQueryPane() = default;

public:
	CString GetSelText() const;
	void SetText(std::vector<CString>&& text);
	void SetDarkMode(const BOOL bSet);

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
