#if !defined(AFX_XRICHEDITCTRL_H__A6986EE1_4941_436D_8324_7BA839981B7A__INCLUDED_)
#define AFX_XRICHEDITCTRL_H__A6986EE1_4941_436D_8324_7BA839981B7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XRichEditCtrl.h : header file
//
#define WM_XRICHEDIT_MSG		0x7100
#define DECLARE_XRICHEDIT_MSG	afx_msg LRESULT OnXRichEditMsg(WPARAM wParam, LPARAM lParam);
#define XRICHEDIT_MSG_MAP		ON_MESSAGE(WM_XRICHEDIT_MSG, OnXRichEditMsg)

#define BEGIN_XRICHEDIT_MSG_MAP(theClass)	LRESULT theClass::OnXRichEditMsg(WPARAM wParam, LPARAM lParam){
#define END_XRICHEDIT_MSG_MAP()	return 1;}

#define XRICHEDIT_MSG_MAP_X(theClass,memberFxn)	BEGIN_XRICHEDIT_MSG_MAP(theClass)\
memberFxn(wParam,lParam);END_XRICHEDIT_MSG_MAP()
/////////////////////////////////////////////////////////////////////////////
// CXRichEditCtrl window

class CXRichEditCtrl : public CRichEditCtrl
{
// Construction
public:
	CXRichEditCtrl();

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXRichEditCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	int GotoLine(int nFlag, int nLineIndex=-1);
	CString GetFilePath(int nFlag=0);
	BOOL GetReadOnly(){return m_bReadOnly;};
	BOOL IsCASDown(int nFlag=0);
	DWORD SetEventChange();
	int GetLineIndex(int nLineIndex=-1);
	int StrToHex(CString str,BYTE* btData);
	CString HexToStr(BYTE* btData,int nLength);
	void HexToStr(BYTE* btData,int nLength,CString& strOutput);
	BOOL LoadFile(LPCTSTR str);
	BOOL SaveFile(LPCTSTR str);
	int GetLineLength(int nLineIndex=-1);
	BOOL IsHexFormat(int nLineIndex=-1);
	BOOL IsHexFormat(CString str,CString &strError,int &nErrorPos);
	BOOL IsHexFormat(CString str);
	void MakeAscHex(BOOL bToAsc=0,int nLineIndex=-1);
	BOOL MakeAscHex(CString str,CString& strNew,BOOL bToAsc=1);	
	void MakeUpperLower(BOOL bUpper=1,int nLineIndex=-1);
	void ReplaceStr(CString str,int nFlag=0,int nLineIndex=-1);
	void SetDefaultFont();
	CString GetLineStr(int nLineIndex=-1);
	int GetLineStr(CString& str,int nLineIndex=-1);
	int GetCursorCol();
	long GetLineEnd(int nLineIndex=-1);
	long GetLineStart(int nLineIndex=-1);

	BOOL CreateX(CWnd *pf,const RECT& rect,UINT unID,DWORD dwStyle =WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL|WS_HSCROLL|WS_VSCROLL);
	virtual ~CXRichEditCtrl();

	// Generated message map functions
protected:
	BOOL m_bReadOnly;
	CHARFORMAT	m_cfText,m_cfComment,m_cfRecv,m_cfSend,m_cfGray,m_cfNumber;
	//{{AFX_MSG(CXRichEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XRICHEDITCTRL_H__A6986EE1_4941_436D_8324_7BA839981B7A__INCLUDED_)
