// SCCSendEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SCCSendEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCCSendEdit

CSCCSendEdit::CSCCSendEdit()
{
}

CSCCSendEdit::~CSCCSendEdit()
{
}


BEGIN_MESSAGE_MAP(CSCCSendEdit, CXRichEditCtrl)
	//{{AFX_MSG_MAP(CSCCSendEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCCSendEdit message handlers

WORD CSCCSendEdit::CheckSum(int nFlag, BOOL bAddEnd)
{
	CString str;
	str=GetSelText();
	if((nFlag==0)||str.IsEmpty()) GetLineStr(str);	//modbus
	if(!IsHexFormat(str))	
	{
		AfxMessageBox("格式不对！组合键Ctrl+Alt+P分析错误！");
		SetFocus();
		return -1;
	}

	WORD CRC;
	BYTE btData[4096];
	int nLength=StrToHex(str,btData);
	
	if(nFlag==0)	//modbus
	{
		if(nLength<6)	
		{
			AfxMessageBox("MODBUS查询报文长度不能小于6个字节！");
			SetFocus();
			return -1;
		}
		CRC=m_cs.ANSI_CRC_16(btData,6);
		btData[6]=CRC>>8;
		btData[7]=(BYTE)CRC;
		str=HexToStr(btData,8);
		ReplaceStr(str,0);
		str.Empty();
		return CRC;
	}
	else if(nFlag==1)	
	{
		CRC=m_cs.CCITT_CRC_16(btData,nLength);	//CCITT-16
		str.Format(" %02X %02X",(BYTE)CRC,(BYTE)(CRC>>8));
	}
	else if(nFlag==2)	
	{
		CRC=m_cs.CCITT_CRC_8(btData,nLength);	//CCITT-8
		str.Format(" %02X",(BYTE)CRC);
	}
	else if(nFlag==3)	
	{
		CRC=m_cs.ANSI_CRC_16(btData,nLength);	//ANSI-16
		str.Format(" %02X %02X",(BYTE)(CRC>>8),(BYTE)CRC);
	}
	else if(nFlag==4)	
	{
		CRC=m_cs.AddSum(btData,nLength);	//ANSI-16
		str.Format(" %02X",(BYTE)CRC);
	}
	else if(nFlag==5)	
	{
		CRC=m_cs.AddSum(btData,nLength);	//ANSI-16
		str.Format(" %02X %02X",(BYTE)CRC,(BYTE)(CRC>>8));
	}
	if(bAddEnd)	ReplaceStr(str,2);
	else	ReplaceStr(str,1);
	
	str.Empty();
	return CRC;
}

BOOL CSCCSendEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message) 
	{
	case WM_KEYDOWN:
		if(IsCASDown(4))//ctrl+alt
		{
			switch (pMsg->wParam)
			{  
			case 'N':
				ReplaceSel("");
				if(!GetLineStr().IsEmpty())	ReplaceStr("",5);
				SetSel(GetLineStart(),GetLineStart());
				SetWordCharFormat(m_cfComment);
				ReplaceStr("<<--设备：PLC；波特率：9600；校验：无；规约：MODUBUS-->>",5);
			//	SetSel(GetLineStart(),GetLineEnd());
			//	SetWordCharFormat(m_cfComment);
			//	SetSel(GetLineStart(),GetLineStart());
				return TRUE;
			}
		}
		break;
	}

	return CXRichEditCtrl::PreTranslateMessage(pMsg);
}
