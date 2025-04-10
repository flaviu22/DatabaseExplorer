#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPaneTreeCtrl window

class CPaneTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CPaneTreeCtrl() = default;

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CPaneTreeCtrl() = default;

protected:
	DECLARE_MESSAGE_MAP()
};
