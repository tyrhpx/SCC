#if !defined(AFX_DLGSEND_H__E013BC75_2988_4697_97BE_576BE93ECF84__INCLUDED_)
#define AFX_DLGSEND_H__E013BC75_2988_4697_97BE_576BE93ECF84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSend.h : header file
// Download by http://www.codefans.net
#include "ListCtrlEx.h"
#define TRIGGER_MAX	10
/////////////////////////////////////////////////////////////////////////////
// CDlgSend dialog

class CDlgSend : public CDialog
{
// Construction
public:
	void SetTransper(BYTE btTrans=128);
	CDlgSend(CWnd* pParent = NULL);   // standard constructor
	struct _Trigger {
		BOOL bValiu;
		UINT unLine;
		UINT unTrigger;
	}Trigger[TRIGGER_MAX];
// Dialog Data
	//{{AFX_DATA(CDlgSend)
	enum { IDD = IDD_DIALOG_SEND };
	CListCtrlEx	m_ClassList;
	int		m_nCycMode;
	int		m_nCycSel;
	UINT	m_unTime;
	UINT	m_unLine;
	UINT	m_unStart;
	UINT	m_unEnd;
	UINT	m_unTrigger;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSend)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	CButton m_btnUse;
	// Generated message map functions
	//{{AFX_MSG(CDlgSend)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	afx_msg void OnCycLine(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEND_H__E013BC75_2988_4697_97BE_576BE93ECF84__INCLUDED_)
