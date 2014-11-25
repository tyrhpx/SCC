#if !defined(AFX_DLGAUTOSAVE_H__8CFCF599_EE73_4437_BC0E_C184A3A7EAC1__INCLUDED_)
#define AFX_DLGAUTOSAVE_H__8CFCF599_EE73_4437_BC0E_C184A3A7EAC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAutoSave.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoSave dialog

class CDlgAutoSave : public CDialog
{
// Construction
public:
	CDlgAutoSave(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDlgAutoSave)
	enum { IDD = IDD_DIALOG_SAVE };
	CString	m_strPath;
	BOOL	m_bAS;
	int		m_nASTime;
	BOOL	m_bClear;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoSave)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoSave)
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTOSAVE_H__8CFCF599_EE73_4437_BC0E_C184A3A7EAC1__INCLUDED_)
