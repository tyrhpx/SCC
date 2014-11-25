// SendFrameWnd.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "SendFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendFrameWnd

CSendFrameWnd::CSendFrameWnd()
{
}

CSendFrameWnd::~CSendFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CSendFrameWnd, CDockablePane)
	//{{AFX_MSG_MAP(CSendFrameWnd)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	XRICHEDIT_MSG_MAP
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendFrameWnd message handlers
XRICHEDIT_MSG_MAP_X(CSendFrameWnd,RecvSendWnd)

BOOL CSendFrameWnd::CreateX(LPCTSTR str, CWnd *pf, UINT unID, DWORD dwstyle)
{
	if (!Create(str, pf, CSize(300,150), TRUE, unID, dwstyle))		
		return 0;
	SetPaneStyle(GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	return 1;
}

int CSendFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_xrec.CreateX(this,CRect(0,0,100,100),  2))		
		return -1;
	DrawLnCol(m_xrec.GetLineIndex(),m_xrec.GetCursorCol());
	
	return 0;
}

void CSendFrameWnd::DrawLnCol(int nLine, int nCol)
{
	CString str;
	str.Format("[行 %d，字节 %d (%d)]",nLine+1,nCol/3+1,nCol+1);
	SetWindowText("发报窗    "+str);
	OnNcPaint();
	str.Empty();
}

void CSendFrameWnd::RecvSendWnd(WPARAM wParam, LPARAM lParam)
{
	if(wParam==1)	
		DrawLnCol(LOWORD(lParam),HIWORD(lParam));
}

void CSendFrameWnd::SaveConfig()
{
	m_xrec.SaveFile(m_xrec.GetFilePath(0));
}


void CSendFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 控件应覆盖整个工作区:
	m_xrec.SetWindowPos (NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}


BOOL CSendFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	CPen* pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	pDC->Rectangle(rc);
	pDC->SelectObject(pOldPen);
	return TRUE;
	return CDockablePane::OnEraseBkgnd(pDC);
}
