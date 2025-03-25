#pragma once

#include "ColorHeaderCtrl.h"

#include <memory>

// CColorListView view

class CColorListView : public CListView
{
	DECLARE_DYNCREATE(CColorListView)

protected:
	CColorListView() = default;           // protected constructor used by dynamic creation

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorListView)
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

public:
	BOOL SetTextColor(const COLORREF cr);
	BOOL SetTextBkColor(const COLORREF cr);
	CColorHeaderCtrl* GetHeaderCtrl() { return m_pHeaderCtrl.get(); }

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	COLORREF m_clrDefBack{}, m_clrDefText{};
	std::unique_ptr<CColorHeaderCtrl> m_pHeaderCtrl{ std::make_unique<CColorHeaderCtrl>() };

protected:
	DECLARE_MESSAGE_MAP()
};
