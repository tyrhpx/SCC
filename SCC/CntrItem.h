
// CntrItem.h : CSCCCntrItem 类的接口
//

#pragma once

class CSCCDoc;
class CSCCView;

class CSCCCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CSCCCntrItem)

// 构造函数
public:
	CSCCCntrItem(REOBJECT* preo = NULL, CSCCDoc* pContainer = NULL);
		// 注意: 允许 pContainer 为 NULL 以启用 IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE 要求类具有带零
		//  参数的构造函数。OLE 项通常是用
		//  非 NULL 文档指针构造的

// 特性
public:
	CSCCDoc* GetDocument()
		{ return reinterpret_cast<CSCCDoc*>(CRichEditCntrItem::GetDocument()); }
	CSCCView* GetActiveView()
		{ return reinterpret_cast<CSCCView*>(CRichEditCntrItem::GetActiveView()); }

// 实现
public:
	~CSCCCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

