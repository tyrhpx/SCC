
// SCCView.h : CSCCView 类的接口
//

#pragma once

class CSCCCntrItem;
#include "XRichEditView.h"
class CSCCDoc;
class CSCCView : public CXRichEditView
{
protected: // 仅从序列化创建
	CSCCView();
	DECLARE_DYNCREATE(CSCCView)

// 特性
public:
	CSCCDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

// 实现
public:
	virtual ~CSCCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // SCCView.cpp 中的调试版本
inline CSCCDoc* CSCCView::GetDocument() const
   { return reinterpret_cast<CSCCDoc*>(m_pDocument); }
#endif

