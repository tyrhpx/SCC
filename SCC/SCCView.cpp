
// SCCView.cpp : CSCCView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SCC.h"
#endif

#include "SCCDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "SCCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSCCView

IMPLEMENT_DYNCREATE(CSCCView, CXRichEditView)

BEGIN_MESSAGE_MAP(CSCCView, CXRichEditView)
	ON_WM_DESTROY()
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CXRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CXRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSCCView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CSCCView 构造/析构

CSCCView::CSCCView()
{
	// TODO: 在此处添加构造代码

}

CSCCView::~CSCCView()
{
}

BOOL CSCCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CXRichEditView::PreCreateWindow(cs);
}

void CSCCView::OnInitialUpdate()
{
	CXRichEditView::OnInitialUpdate();


	// 设置打印边距(720 缇 = 1/2 英寸)
	SetMargins(CRect(720, 720, 720, 720));
}


// CSCCView 打印


void CSCCView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSCCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}


void CSCCView::OnDestroy()
{
	// 析构时停用此项；这在
	// 使用拆分视图时非常重要 
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CXRichEditView::OnDestroy();
}


void CSCCView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSCCView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSCCView 诊断

#ifdef _DEBUG
void CSCCView::AssertValid() const
{
	CXRichEditView::AssertValid();
}

void CSCCView::Dump(CDumpContext& dc) const
{
	CXRichEditView::Dump(dc);
}

CSCCDoc* CSCCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSCCDoc)));
	return (CSCCDoc*)m_pDocument;
}
#endif //_DEBUG


// CSCCView 消息处理程序
