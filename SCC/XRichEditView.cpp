// XRichEditView.cpp : implementation of the CXRichEditView class
// Download by http://www.codefans.net

#include "stdafx.h"
#include "XRichEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CXRichEditDoc

IMPLEMENT_DYNCREATE(CXRichEditDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CXRichEditDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CXRichEditDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, CRichEditDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRichEditDoc construction/destruction

CXRichEditDoc::CXRichEditDoc()
{
	// TODO: add one-time construction code here

}

CXRichEditDoc::~CXRichEditDoc()
{
}

BOOL CXRichEditDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

CRichEditCntrItem* CXRichEditDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return new CXRichEditCntrItem(preo, (CXRichEditDoc*) this);
}



/////////////////////////////////////////////////////////////////////////////
// CXRichEditDoc serialization

void CXRichEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CRichEditDoc enables serialization
	//  of the container document's COleClientItem objects.
	// TODO: set CRichEditDoc::m_bRTF = FALSE if you are serializing as text
	CRichEditDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CXRichEditDoc diagnostics

#ifdef _DEBUG
void CXRichEditDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CXRichEditDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CXRichEditView

IMPLEMENT_DYNCREATE(CXRichEditView, CRichEditView)

BEGIN_MESSAGE_MAP(CXRichEditView, CRichEditView)
	//{{AFX_MSG_MAP(CXRichEditView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRichEditView construction/destruction

CXRichEditView::CXRichEditView()
{
	// TODO: add construction code here
	m_hDisplay=NULL;
	m_bReadOnly=0;

	UINT	uiSize			= sizeof(CHARFORMAT);
	DWORD	dwMask			= CFM_COLOR | CFM_FACE | CFM_SIZE;
	LONG	lHeight			= 180;	// 8 point => 160 * (1/20)
	m_cfText.cbSize			= uiSize;
	m_cfText.dwMask			= dwMask;
	m_cfText.dwEffects		= 0;
	m_cfText.yHeight		= lHeight;
	m_cfText.crTextColor	= RGB(0, 0, 0);
	strcpy_s(m_cfText.szFaceName, _T("宋体"));
	m_cfComment.cbSize		= uiSize;
	m_cfComment.dwMask		= dwMask;
	m_cfComment.dwEffects	= 0;
	m_cfComment.yHeight		= lHeight;
	m_cfComment.crTextColor	= RGB(0, 128, 0);
	strcpy_s(m_cfComment.szFaceName, _T("宋体"));
	m_cfRecv.cbSize			= uiSize;
	m_cfRecv.dwMask			= dwMask;
	m_cfRecv.dwEffects		= 0;
	m_cfRecv.yHeight		= lHeight;
	m_cfRecv.crTextColor	= RGB(0, 0, 200);
	strcpy_s(m_cfRecv.szFaceName,	_T("宋体"));
	m_cfSend.cbSize		= uiSize;
	m_cfSend.dwMask		= dwMask;
	m_cfSend.dwEffects	= 0;
	m_cfSend.yHeight		= lHeight;
	m_cfSend.crTextColor	= RGB(200, 0, 0);
	strcpy_s(m_cfSend.szFaceName, _T("宋体"));
	m_cfGray.cbSize		= uiSize;
	m_cfGray.dwMask		= dwMask;
	m_cfGray.dwEffects	= 0;
	m_cfGray.yHeight		= lHeight;
	m_cfGray.crTextColor	= RGB(128, 128, 128);
	strcpy_s(m_cfGray.szFaceName, _T("宋体"));
	m_cfNumber.cbSize		= uiSize;
	m_cfNumber.dwMask		= dwMask;
	m_cfNumber.dwEffects	= 0;
	m_cfNumber.yHeight		= lHeight;
	m_cfNumber.crTextColor	= RGB(200, 0, 0);
	strcpy_s(m_cfNumber.szFaceName, _T("宋体"));
}

CXRichEditView::~CXRichEditView()
{
	m_hDisplay=NULL;
}

BOOL CXRichEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	if (LoadLibraryA(_T("RICHED20.DLL")) == NULL)
	{
		AfxMessageBox("RICHED20.DLL 未找到", MB_OK|MB_ICONEXCLAMATION);
		PostMessage(WM_QUIT,0,0);
		return FALSE;
	}

	return CRichEditView::PreCreateWindow(cs);
}

void CXRichEditView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	m_hDisplay=CreateEvent(NULL, TRUE, TRUE, NULL);
	m_lCurLen=0;
	m_lLimit=GetRichEditCtrl().GetLimitText();
	SetDefaultFont();
	SetWrap();
	GetRichEditCtrl().SetBackgroundColor(FALSE, RGB(230,230,230));
	GetDocument()->SetModifiedFlag(0);
	// Set the printing margins (720 twips = 1/2 inch).
	SetMargins(CRect(720, 720, 720, 720));
}

/////////////////////////////////////////////////////////////////////////////
// CXRichEditView printing

BOOL CXRichEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}


void CXRichEditView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CRichEditView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CXRichEditView diagnostics

#ifdef _DEBUG
void CXRichEditView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CXRichEditView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CXRichEditDoc* CXRichEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXRichEditDoc)));
	return (CXRichEditDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CXRichEditCntrItem implementation

IMPLEMENT_SERIAL(CXRichEditCntrItem, CRichEditCntrItem, 0)

CXRichEditCntrItem::CXRichEditCntrItem(REOBJECT* preo, CXRichEditDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
	
}

CXRichEditCntrItem::~CXRichEditCntrItem()
{
	// TODO: add cleanup code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CXRichEditCntrItem diagnostics

#ifdef _DEBUG
void CXRichEditCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CXRichEditCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CXRichEditView message handlers

void CXRichEditView::SetDefaultFont()
{
	CFont FontTmp;
	FontTmp.CreateFont(12,0,0,0,650,FALSE,FALSE,0,
						ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,VARIABLE_PITCH&FF_SWISS,"宋体");
	SetFont(&FontTmp);
	FontTmp.DeleteObject();
}
void CXRichEditView::ClearDatas(BOOL bEmptyUndoBuffer)
{
	GetREC().SetSel(0,-1);
	GetREC().Clear();
	if(bEmptyUndoBuffer)	GetREC().EmptyUndoBuffer();
}

void CXRichEditView::SetWrap(int nWrapMode)
{
	m_nWordWrap=nWrapMode;
	WrapChanged();
}

static DWORD CALLBACK 
CXRichEditViewStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	pFile->Write(pbBuff, cb);
	*pcb = cb;

	return 0;
}

BOOL CXRichEditView::SaveFile(LPCTSTR str)
{
	CFile fld;
	if(!fld.Open( str, CFile::modeCreate|CFile::modeWrite))	return 0;

	EDITSTREAM strm;
	strm.dwCookie = (DWORD) &fld;
	strm.pfnCallback = CXRichEditViewStreamOutCallback;
	GetRichEditCtrl().StreamOut(SF_RTF, strm);
	GetRichEditCtrl().SetModify(FALSE);
	fld.Close();
	return 1;
}
static DWORD CALLBACK 
CXRichEditViewStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	*pcb = pFile->Read(pbBuff, cb);

	return 0;
}

BOOL CXRichEditView::LoadFile(LPCTSTR str)
{
	CFile fld;
	if(!fld.Open( str, CFile::modeRead))	return 0;
		EDITSTREAM strm;
		strm.dwCookie = (DWORD) &fld;
		strm.pfnCallback = CXRichEditViewStreamInCallback;
		GetRichEditCtrl().StreamIn(SF_RTF, strm);
		GetRichEditCtrl().SetModify(FALSE);		
		fld.Close();
		return 1;
}

void CXRichEditView::AddString(CString str, int nFlag)
{
	WaitForSingleObject(m_hDisplay, INFINITE);
	ResetEvent(m_hDisplay);
	
	CRichEditCtrl& reCtrl=GetRichEditCtrl();
	m_lCurLen=reCtrl.GetTextLength();
	m_lCurLen+=str.GetLength();
	if(m_lCurLen>=m_lLimit)	
	{
		reCtrl.SetSel(0,-1);
		reCtrl.Clear();
		m_lCurLen=0;
	}
	reCtrl.SetSel(m_lCurLen,m_lCurLen);
	if(nFlag==0)		
		reCtrl.SetWordCharFormat(m_cfText);
	else if(nFlag==1)	
		reCtrl.SetWordCharFormat(m_cfNumber);
	else if(nFlag==2)	
		reCtrl.SetWordCharFormat(m_cfComment);
	else if(nFlag==3)	
		reCtrl.SetWordCharFormat(m_cfRecv);
	else if(nFlag==4)	
		reCtrl.SetWordCharFormat(m_cfSend);
	else if(nFlag==5)	
		reCtrl.SetWordCharFormat(m_cfGray);
	reCtrl.ReplaceSel(str); 
	reCtrl.SetWordCharFormat(m_cfText);
	SetEvent(m_hDisplay);
}

void CXRichEditView::SetMF(BOOL bModified)
{
	GetDocument()->SetModifiedFlag(bModified);
}

void CXRichEditView::SetReadOnly(BOOL bReadOnly)
{
	m_bReadOnly=bReadOnly;
	GetRichEditCtrl().SetReadOnly(bReadOnly);
}
