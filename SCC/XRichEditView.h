// XRichEditView.h : interface of the CXRichEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRichEditVIEW_H__ADC48421_2DD6_40DC_99E2_3E8DE2A0FA84__INCLUDED_)
#define AFX_XRichEditVIEW_H__ADC48421_2DD6_40DC_99E2_3E8DE2A0FA84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXRichEditCntrItem;

class CXRichEditDoc : public CRichEditDoc
{
protected: // create from serialization only
	CXRichEditDoc();
	DECLARE_DYNCREATE(CXRichEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXRichEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;
// Implementation
public:
	virtual ~CXRichEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CXRichEditDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CXRichEditView : public CRichEditView
{
protected: // create from serialization only
	CXRichEditView();
	DECLARE_DYNCREATE(CXRichEditView)
		
	HANDLE	m_hDisplay;
	long m_lLimit,m_lCurLen;
	CHARFORMAT	m_cfText,m_cfComment,m_cfRecv,m_cfSend,m_cfGray,m_cfNumber;
// Attributes
public:
	CXRichEditDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXRichEditView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsModifyed(){return GetDocument()->IsModified();};
	void SetReadOnly(BOOL bReadOnly=1);
	BOOL GetReadOnly(){return m_bReadOnly;};
	void SetMF(BOOL bModified=0);
	void AddString(CString str,int nFlag);
	BOOL LoadFile(LPCTSTR str);
	BOOL SaveFile(LPCTSTR str);
	void SetWrap(int nWrapMode=0);
	void SetDefaultFont();
	void ClearDatas(BOOL bEmptyUndoBuffer=1);
	CRichEditCtrl& GetREC(){return GetRichEditCtrl();};
	int GetWrapMode(){return m_nWordWrap;};
	virtual ~CXRichEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	BOOL m_bReadOnly;
	//{{AFX_MSG(CXRichEditView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in XRichEditView.cpp
inline CXRichEditDoc* CXRichEditView::GetDocument()
   { return (CXRichEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
class CXRichEditCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CXRichEditCntrItem)

// Constructors
public:
	CXRichEditCntrItem(REOBJECT* preo = NULL, CXRichEditDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CXRichEditDoc* GetDocument()
		{ return (CXRichEditDoc*)CRichEditCntrItem::GetDocument(); }
	CXRichEditView* GetActiveView()
		{ return (CXRichEditView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXRichEditCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CXRichEditCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XRichEditVIEW_H__ADC48421_2DD6_40DC_99E2_3E8DE2A0FA84__INCLUDED_)
