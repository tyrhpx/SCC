#if !defined(AFX_LISTCTRLEX_H__E29BCEF5_3FCB_4A2E_8EFF_4C6DCCD6485C__INCLUDED_)
#define AFX_LISTCTRLEX_H__E29BCEF5_3FCB_4A2E_8EFF_4C6DCCD6485C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file
//
/////////////////////////////////////////
//该类改造于Lee Nowotny的gxListCtrl类
//详细见例子http://www.codetools.com/listctrl/listeditor.asp
/////////////////////////////////////////
class CListCellEdit : public CEdit
{
// Construction
public:
	CListCellEdit(int nItem, int nSubItem, CString strInitTex);
	void SetListItemText();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCellEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCellEdit();
 
protected:
	int			m_nItem;
    int			m_nSubItem;
    CString		m_strInitText;
    BOOL		m_bEscape;

	// Generated message map functions
	//{{AFX_MSG(CListCellEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

class CListCtrlEx : public CListCtrl
{
// Construction
public:
	CListCtrlEx();

// Attributes
public:

// Operations
public:
    int	    HitTestEx (CPoint& Point, int& nSubItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	CEdit* EditSubItem(int nItem, int nSubItem);
	virtual ~CListCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__E29BCEF5_3FCB_4A2E_8EFF_4C6DCCD6485C__INCLUDED_)
