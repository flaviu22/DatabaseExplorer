#pragma once

#define AFX_NON_CLOSE_DOCKING_PANE_STYLE	AFX_CBRS_FLOAT | AFX_CBRS_RESIZE | AFX_CBRS_AUTOHIDE

// CNonClosableDockablePane

class CNonClosableDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(CNonClosableDockablePane)

public:
	CNonClosableDockablePane();
	virtual ~CNonClosableDockablePane() override;
	virtual CTabbedPane* CreateTabbedPane();

protected:
	virtual BOOL CanBeClosed() const { return FALSE; }
	virtual BOOL CanAdjustLayout() const {return ! m_bIsSliding || ! m_bIsHiding;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNonClosableDockablePane)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CNonClosableDockablePane)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
