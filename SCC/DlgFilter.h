#if !defined(AFX_DLGFILTER_H__52FAB9EB_A960_4E27_8BCD_B04EC9207BDA__INCLUDED_)
#define AFX_DLGFILTER_H__52FAB9EB_A960_4E27_8BCD_B04EC9207BDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFilter.h : header file
//
#include "ListCtrlEx.h"
#define FILTER_MAX	10
/////////////////////////////////////////////////////////////////////////////
// CDlgFilter dialog

class CDlgFilter : public CDialog
{
// Construction
public:
	CDlgFilter(CWnd* pParent = NULL);   // standard constructor
	struct  _Filter{
		BOOL bFilter;
		BYTE btNo;
		BYTE btValiu;
	}Filter[FILTER_MAX];
// Dialog Data
	//{{AFX_DATA(CDlgFilter)
	enum { IDD = IDD_DIALOG_FILTER };
	CListCtrlEx	m_ClassList;
	int		m_nFilter;
	BOOL	m_bFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFilter)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFilter)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFilter();
	afx_msg void OnButtonExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILTER_H__52FAB9EB_A960_4E27_8BCD_B04EC9207BDA__INCLUDED_)
