
// CntrItem.cpp : CSCCCntrItem 类的实现
//

#include "stdafx.h"
#include "SCC.h"

#include "SCCDoc.h"
#include "SCCView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSCCCntrItem 的实现

IMPLEMENT_SERIAL(CSCCCntrItem, CRichEditCntrItem, 0)

CSCCCntrItem::CSCCCntrItem(REOBJECT* preo, CSCCDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: 在此添加一次性构造代码
}

CSCCCntrItem::~CSCCCntrItem()
{
	// TODO: 在此处添加清理代码
}


// CSCCCntrItem 诊断

#ifdef _DEBUG
void CSCCCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CSCCCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

