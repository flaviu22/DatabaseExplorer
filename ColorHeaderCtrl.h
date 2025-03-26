#pragma once

// CColorHeaderCtrl

class CColorHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CColorHeaderCtrl)

public:
	CColorHeaderCtrl();
	~CColorHeaderCtrl() override = default;

public:
	void UpdateCustomDrawFlag();
	BOOL IsCustomDraw() const { return m_bCustomDraw; }
	COLORREF GetTextColorEx() const { return m_crText; }
	COLORREF GetBackgroundColor() const { return m_crBackground; }
	COLORREF GetBackgroundColorHot() const { return m_crBackgroundHot; }
	COLORREF GetBackgroundColorPressed() const { return m_crBackgroundPressed; }
	void SetTextColorEx(const COLORREF cr)
	{
		m_crText = cr;
		UpdateCustomDrawFlag();
	}
	void SetBackgroundColor(const COLORREF cr)
	{
		m_crBackground = cr;
		UpdateCustomDrawFlag();
	}
	void SetBackgroundColorHot(const COLORREF cr) { m_crBackgroundHot = cr; }
	void SetBackgroundColorPressed(const COLORREF cr) { m_crBackgroundPressed = cr; }

protected:
	COLORREF m_crText{ 0 };
	COLORREF m_crBackground{ 0 };
	COLORREF m_crBackgroundHot{ 0 };
	COLORREF m_crBackgroundPressed{ 0 };
	COLORREF m_crBackgroundTheme{ 0 };
	COLORREF m_crBackgroundHotTheme{ 0 };
	COLORREF m_crBackgroundPressedTheme{ 0 };
	BOOL m_bCustomDraw{ FALSE };
	BOOL m_bPrint{ FALSE };	// To avoid infinite loop with WM_PRINTCLIENT

protected:
	//{{AFX_MSG(CHeaderCtrlEx)
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
