#if !defined(AFX_TRANSMIT_H__D1DFB16D_FE53_4EB8_B305_BDF760E92633__INCLUDED_)
#define AFX_TRANSMIT_H__D1DFB16D_FE53_4EB8_B305_BDF760E92633__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Transmit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransmit dialog

class CTransmit : public CDialog
{
// Construction
public:
	CTransmit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransmit)
	enum { IDD = IDD_DIALOG_TRANSMIT };
	BYTE	m_btCOM1;
	BYTE	m_btCOM2;
	BOOL	m_bCOM;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransmit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransmit)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSMIT_H__D1DFB16D_FE53_4EB8_B305_BDF760E92633__INCLUDED_)
