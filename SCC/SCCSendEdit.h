#if !defined(AFX_SCCSENDEDIT_H__A4ABBAD5_6EAE_4896_A3F3_7AB8B2E9317A__INCLUDED_)
#define AFX_SCCSENDEDIT_H__A4ABBAD5_6EAE_4896_A3F3_7AB8B2E9317A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCCSendEdit.h : header file
// Download by http://www.codefans.net
#include "XRichEditCtrl.h"
#include "CheckSum.h"	// Added by ClassView
/////////////////////////////////////////////////////////////////////////////
// CSCCSendEdit window

class CSCCSendEdit : public CXRichEditCtrl
{
// Construction
public:
	CSCCSendEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCCSendEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	CCheckSum m_cs;
	WORD CheckSum(int nFlag,BOOL bAddEnd=1);
	virtual ~CSCCSendEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCCSendEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCCSENDEDIT_H__A4ABBAD5_6EAE_4896_A3F3_7AB8B2E9317A__INCLUDED_)
