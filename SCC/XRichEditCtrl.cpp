// XRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "XRichEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHKLINE(nLineIndex)	if((nLineIndex<=-1)||(nLineIndex>=GetLineCount()))\
	nLineIndex=LineFromChar(LineIndex(-1))
#define LN_COL_MSG(lparam)	::PostMessage(GetParent()->m_hWnd,WM_XRICHEDIT_MSG,\
(WPARAM)1,lparam);
/////////////////////////////////////////////////////////////////////////////
// CXRichEditCtrl

CXRichEditCtrl::CXRichEditCtrl()
{
	m_bReadOnly=0;
	UINT	uiSize			= sizeof(CHARFORMAT);
	DWORD	dwMask			= CFM_COLOR | CFM_FACE | CFM_SIZE;
	LONG	lHeight			= 180;	// 8 point => 160 * (1/20)
	m_cfText.cbSize			= uiSize;
	m_cfText.dwMask			= dwMask;
	m_cfText.dwEffects		= 0;
	m_cfText.yHeight		= lHeight;
	m_cfText.crTextColor	= RGB(0, 0, 0);
	strcpy_s(m_cfText.szFaceName,	_T("宋体"));
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

CXRichEditCtrl::~CXRichEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CXRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CXRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRichEditCtrl message handlers

BOOL CXRichEditCtrl::CreateX(CWnd *pf, const RECT &rect, UINT unID, DWORD dwStyle)
{
	return Create(dwStyle,rect, pf, unID);
}

long CXRichEditCtrl::GetLineStart(int nLineIndex)
{
	CHKLINE(nLineIndex);
	return LineIndex(nLineIndex);
}

long CXRichEditCtrl::GetLineEnd(int nLineIndex)
{
	CHKLINE(nLineIndex);
	return LineIndex(nLineIndex)+LineLength(LineIndex(nLineIndex));
}

int CXRichEditCtrl::GetCursorCol()
{
	long ns=0,ne=0;
	GetSel(ns,ne);
	if(ns!=ne)
	{
		ns=LineFromChar(ne);
		ns=GetLineStart(ns);
		return ne-ns;
	}
	return ne-LineIndex(-1);
}

int CXRichEditCtrl::GetLineStr(CString &str, int nLineIndex)
{
	CHKLINE(nLineIndex);
	int nli=LineIndex(nLineIndex);
	long ns,ne;
	GetSel(ns,ne);
	SetSel(nli,nli+LineLength(nli));
	str=GetSelText();
	SetSel(ns,ne);
	return str.GetLength();
}

CString CXRichEditCtrl::GetLineStr(int nLineIndex)
{
	CHKLINE(nLineIndex);
	int nli=LineIndex(nLineIndex);
	long ns,ne;
	GetSel(ns,ne);
	SetSel(nli,nli+LineLength(nli));
	CString str=GetSelText();
	SetSel(ns,ne);
//	nRead= GetLine(nLineIndex, str.GetBuffer(nLineLength+3),nLineLength + 1);
//	str.ReleaseBuffer(nRead);
	return str;
}

void CXRichEditCtrl::SetDefaultFont()
{
	CFont FontTmp;
	FontTmp.CreateFont(12,0,0,0,650,FALSE,FALSE,0,
						ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,VARIABLE_PITCH&FF_SWISS,"宋体");
	SetFont(&FontTmp);
	FontTmp.DeleteObject();
}

void CXRichEditCtrl::ReplaceStr(CString str, int nFlag, int nLineIndex)
{
	CHKLINE(nLineIndex);
	int nli=LineIndex(nLineIndex);
	if((nFlag==4)||(nFlag==5))
	{
		if(GetLineStr(nLineIndex).IsEmpty())	
			nFlag=0;
	}
	if(nFlag==0)	
		SetSel(nli,nli+LineLength(nli));//整行覆盖
	else if(nFlag==1)	//当前光标处
	{
		long nStart,nEnd;
		GetSel(nStart,nEnd);
		SetSel(nEnd,nEnd);
	}
	else if((nFlag==2)||(nFlag==5))//
	{
		if(nFlag==5)	
			str="\n"+str;
		SetSel(nli+LineLength(nli),nli+LineLength(nli));//行尾或下行
	}
	else if((nFlag==3)||(nFlag==4))
	{
		if(nFlag==4)	
			str+="\n";
		SetSel(nli,nli);	//行首或上行
	}
	ReplaceSel(str,1);
}

void CXRichEditCtrl::MakeUpperLower(BOOL bUpper,int nLineIndex)
{
	CHKLINE(nLineIndex);
	long lst,lse;
	GetSel(lst,lse);
	CString str;
	if(lst==lse)	
	{	
		GetLineStr(str,nLineIndex);	
		SetSel(GetLineStart(nLineIndex),GetLineEnd(nLineIndex));
	}
	else
	{
		str=GetSelText();
		SetSel(lst,lse);
	}
	if(bUpper)	
		str.MakeUpper();
	else		
		str.MakeLower();
	ReplaceSel(str,1);
	str.Empty();
//	SetSel(lst,lse);
}

BOOL CXRichEditCtrl::IsHexFormat(CString str)
{
	int nStrlength=str.GetLength();
	if(nStrlength<2)	
		return FALSE;
	for(int i=0;i<nStrlength;i+=3)
	{
		if(((i+1)==nStrlength)||((i+3)==nStrlength))	
			return FALSE;
		TCHAR chr=' ';
		if((i+2)!=nStrlength)	
			chr=str.GetAt(i+2);
		if(chr!=' ')	
			return FALSE;
		for(int j=0;j<2;j++)
		{
			chr=str.GetAt(i+j);
			BOOL bHex=(((chr>='0')&&(chr<='9'))||((chr>='A')&&(chr<='F'))||((chr>='a')&&(chr<='f')));
			if(!bHex)	
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CXRichEditCtrl::MakeAscHex(CString str, CString &strNew, BOOL bToAsc)
{
	int nRead=str.GetLength();
	if(nRead==0)	
		return 0;
	if(bToAsc)
	{		
		if(!IsHexFormat(str))	
			return 0;
		BYTE btTmp[4096];
		for(int i=0;i<nRead;i+=3)	
			_stscanf_s(str.Mid(i,2),"%02X",&btTmp[i/3]);
		strNew=btTmp;
	}
	else
	{
		nRead-=1;
		CString strTmp;
		for(int i=0;i<nRead;i++)
		{
			strTmp.Format("%2X ",(BYTE)str.GetAt(i));
			strNew+=strTmp;
		}
		strTmp.Format("%2X",(BYTE)str.GetAt(nRead));
		strNew+=strTmp;	
		strTmp.Empty();
	}
	return 1;
}

void CXRichEditCtrl::MakeAscHex(BOOL bToAsc, int nLineIndex)
{
	CHKLINE(nLineIndex);
	CString str,strNew;
	GetLineStr(str,nLineIndex);
	if(MakeAscHex(str,strNew,bToAsc))	
		ReplaceStr(strNew,0,nLineIndex);	
	str.Empty();
	strNew.Empty();
}

BOOL CXRichEditCtrl::IsHexFormat(CString str, CString &strError, int &nErrorPos)
{
	int nStrlength=str.GetLength();
	if(nStrlength<2)	
	{
		strError="长度不能小于 1 个字节！";
		nErrorPos=0;
		return 0;
	}
	int i=0;
	BOOL bError=FALSE;
	for(i=0;i<nStrlength;i+=3)
	{
		if((i+1)==nStrlength)
		{
			strError="最后不能为半个字节！";	
			nErrorPos=i+1;
			return 0;
		}
		if((i+3)==nStrlength)
		{
			TCHAR lastchar=str.GetAt(i+2);
			if(isascii(lastchar)==0)	
				strError.Format("最后多出一个非ASCII字符，其16进制为：  %02X H",(BYTE)lastchar);
			else	strError.Format("最后多出一个字符：  \' %c \'",lastchar);
			nErrorPos=i+3;
			return 0;
		}
		TCHAR chr=' ';
		if((i+2)!=nStrlength)	chr=str.GetAt(i+2);
		if(chr!=' ')	
		{
			if(isascii(chr)==0)
				 strError.Format("第 %d 个字节后为非ASCII字符，其16进制为：  %02X H。必须改为  空格",i/3+1,(BYTE)chr);
			else strError.Format("第 %d 个字节后的字符  \' %c \'  必须改为  空格",i/3+1,chr);
			nErrorPos=i+3;
			return 0;
		}
		BOOL bHex=FALSE;
		for(int j=0;j<2;j++)
		{
			chr=str.GetAt(i+j);
			if(chr==' ')	
			{
				if(j==0)	strError.Format("第 %d 个字节的高位不能为  空格，必须是16进制数",i/3+1);
				else		strError.Format("第 %d 个字节的低位不能为  空格，必须是16进制数",i/3+1);
				nErrorPos=i+j+1;
				return 0;
			}
			bHex=(((chr>='0')&&(chr<='9'))||((chr>='A')&&(chr<='F'))||((chr>='a')&&(chr<='f')));
			if(!bHex)
			{
				if(j==0)	
				{
					if(isascii(chr)==0)
						 strError.Format("第 %d 个字节的高位为非ASCII字符，其16进制为：  %02X H。必须是16进制数",i/3+1,(BYTE)chr);
					else strError.Format("第 %d 个字节的高位不能为  \' %c \'，必须是16进制数",i/3+1,chr);
				}
				else	
				{
					if(isascii(chr)==0)
						 strError.Format("第 %d 个字节的低位为非ASCII字符，其16进制为：  %2X H。必须是16进制数",i/3+1,(BYTE)chr);
					else strError.Format("第 %d 个字节的低位不能为  \' %c \'，必须是16进制数",i/3+1,chr);
				}
				nErrorPos=i+j+1;
				return 0;
			}
		}
	}
	return 1;
}

BOOL CXRichEditCtrl::IsHexFormat(int nLineIndex)
{
	CHKLINE(nLineIndex);
	CString str,strError;
	int nErrorPos;
	GetLineStr(str,nLineIndex);
	if(IsHexFormat(str,strError,nErrorPos))	
	{
		str.Empty();
		strError.Empty();
		return 1;
	}
	CString strtmp;
	strtmp.Format("第 %d 行    报文格式出错：\n\n",nLineIndex+1);
	strtmp+=strError;
	strtmp+="\n\n报文格式：";
	strtmp+="\n1、每 2 个字节间有一个空格；";
	strtmp+="\n2、每个字节必须为16进制数：0~9，a~f，A~F；";
	strtmp+="\n3、最后一个字节后不能有任何字符。";
	strtmp+="\n\n示例报文(共5个字节)：EB  90  02  34  0D";
	AfxMessageBox(strtmp);
	strtmp.Empty();
	str.Empty();
	strError.Empty();
	SetFocus();
	int nError=LineIndex(nLineIndex)+nErrorPos;
	SetSel(nError,nError);
	return 0;
}

int CXRichEditCtrl::GetLineLength(int nLineIndex)
{
	CHKLINE(nLineIndex);
	return LineLength(LineIndex(nLineIndex));
}

static DWORD CALLBACK 
CXRichEditCtrlStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	pFile->Write(pbBuff, cb);
	*pcb = cb;

	return 0;
}

BOOL CXRichEditCtrl::SaveFile(LPCTSTR str)
{
	CFile fld;
	if(!fld.Open( str, CFile::modeCreate|CFile::modeWrite))
	{
		if(!fld.Open( GetFilePath(1), CFile::modeCreate|CFile::modeWrite))
			return 0;
	}
	EDITSTREAM strm;
	strm.dwCookie = (DWORD) &fld;
	strm.pfnCallback = CXRichEditCtrlStreamOutCallback;
	StreamOut(SF_RTF, strm);
	SetModify(FALSE);
	AfxGetApp()->WriteProfileString("Settings" ,"XREPath", str);
	fld.Close();
	return 1;
}
static DWORD CALLBACK 
CXRichEditCtrlStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	*pcb = pFile->Read(pbBuff, cb);

	return 0;
}

BOOL CXRichEditCtrl::LoadFile(LPCTSTR str)
{
	CFile fld;
	if(!fld.Open( str, CFile::modeRead))
	{
		if(!fld.Open( GetFilePath(1), CFile::modeRead))
			return 0;
	}
	EDITSTREAM strm;
	strm.dwCookie = (DWORD) &fld;
	strm.pfnCallback = CXRichEditCtrlStreamInCallback;
	StreamIn(SF_RTF, strm);
	SetModify(FALSE);		
	fld.Close();
	return 1;
}

void CXRichEditCtrl::HexToStr(BYTE *btData, int nLength, CString &strOutput)
{
	if((btData==NULL)||(nLength<1)) return;
	CString strCvt;   
	int nl=nLength-1;
	int i = 0;
	for(; i < nl; i++)                           
	{          
		strCvt.Format("%02X ", btData[i]);     
	   	strOutput += strCvt;                                    
	}         
	strCvt.Format("%02X", btData[i]);
	strOutput+=strCvt;
	strCvt.Empty();
}

CString CXRichEditCtrl::HexToStr(BYTE *btData, int nLength)
{
	if((btData==NULL)||(nLength<1))	return "";
	CString strCvt,strOutput;   
	int nl=nLength-1;
	int i;
	for(i = 0; i < nl; i++)                           
	{          
		strCvt.Format("%02X ", btData[i]);     
	   	strOutput += strCvt;                                    
	}         
	strCvt.Format("%02X", btData[i]);
	strOutput+=strCvt;
	strCvt.Empty();
	return strOutput;
}

int CXRichEditCtrl::StrToHex(CString str,BYTE* btData)
{
	int nLength=str.GetLength();
	if(nLength<2)	return 0;
	int i;
	for(i=0;i<nLength;i+=3)	_stscanf_s(str.Mid(i,2),"%02X",&btData[i/3]);
	return (i+1)/3;
}

int CXRichEditCtrl::GetLineIndex(int nLineIndex)
{
	CHKLINE(nLineIndex);
	return nLineIndex;
}

int CXRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (LoadLibraryA(_T("RICHED20.DLL")) == NULL)
	{
		AfxMessageBox("RICHED20.DLL 未找到", MB_OK|MB_ICONEXCLAMATION);
		PostMessage(WM_QUIT,0,0);
		return -1;
	}

	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetDefaultFont();
	SetEventChange();
	SetBackgroundColor(FALSE, RGB(240,240,240));
	// TODO: Add your specialized creation code here
	LoadFile(GetFilePath(0));
	return 0;
}

DWORD CXRichEditCtrl::SetEventChange()
{
	long lMask = GetEventMask();
	lMask |= ENM_CHANGE;
	return SetEventMask(lMask);
}

void CXRichEditCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRichEditCtrl::OnLButtonDown(nFlags, point);
	LN_COL_MSG(MAKELPARAM(GetLineIndex(),GetCursorCol()))
}

void CXRichEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	LN_COL_MSG(MAKELPARAM(GetLineIndex(),GetCursorCol()))
}

BOOL CXRichEditCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message) 
	{
	case WM_KEYDOWN:
		if(IsCASDown(4))//ctrl+alt
		{
			switch (pMsg->wParam)
			{  
			case 'U':
			case 'L':
				MakeUpperLower((pMsg->wParam=='U')?1:0);
				return TRUE;
			case 'H':
			case 'A':
				MakeAscHex((pMsg->wParam=='A')?1:0);
				return TRUE;
			case 'P':
				IsHexFormat();
				return TRUE;
			case 'B':
			case 'G':
			case 'Y':
			case 'T':
			case 'R':
				if(GetSelText().IsEmpty())	SetSel(GetLineStart(),GetLineEnd());
				if(pMsg->wParam=='B')	SetWordCharFormat(m_cfRecv);
				else if(pMsg->wParam=='T')	SetWordCharFormat(m_cfText);
				else if(pMsg->wParam=='G')	SetWordCharFormat(m_cfComment);
				else if(pMsg->wParam=='Y')	SetWordCharFormat(m_cfGray);
				else if(pMsg->wParam=='R')	SetWordCharFormat(m_cfSend);
			//	SetSel(GetLineStart(),GetLineStart());
				return TRUE;
			case ' ':
				ReplaceSel("");
			//	SetSel(GetLineStart(),GetLineEnd());
				if(!GetLineStr().IsEmpty())	ReplaceStr("",5);
				SetSel(GetLineStart(),GetLineStart());
				SetWordCharFormat(m_cfComment);
				ReplaceStr("<<--    -->>",5);
			//	HideCaret();
			//	DestroyCaret();
				/*
			   CBitmap* pBitmap = new CBitmap;
			   pBitmap->LoadBitmap(IDB_BITMAP1);
				CreateCaret(pBitmap);
			   ShowCaret();	
				*/
				return TRUE;
			}
		}
		else if(IsCASDown(3))//ctrl
		{
			switch (pMsg->wParam)
			{  
			case 'V':
				PasteSpecial(CF_TEXT);	return TRUE;
			case 'C':
				Copy();					return TRUE;
			case 'X':
				Cut();					return TRUE;
			case 'A':
				SetSel(0,-1);			return TRUE;
			case 'Z':
				Undo();					return TRUE;
			case 'R':
				m_bReadOnly=!m_bReadOnly;
				SetReadOnly(m_bReadOnly);
				if(m_bReadOnly)	SetBackgroundColor(FALSE,RGB(210,210,210));
				else			SetBackgroundColor(FALSE,RGB(230,230,230));
				return TRUE;
			}
		}
		break;
	}
	
	return CRichEditCtrl::PreTranslateMessage(pMsg);
}

void CXRichEditCtrl::OnChange() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CRichEditCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	LN_COL_MSG(MAKELPARAM(GetLineIndex(),GetCursorCol()))
	// TODO: Add your control notification handler code here
	
}

BOOL CXRichEditCtrl::IsCASDown(int nFlag)
{
	if(nFlag==0)		return (::GetKeyState(VK_CONTROL) < 0);
	else if(nFlag==1)	return (::GetKeyState(VK_MENU) < 0);
	else if(nFlag==2)	return (::GetKeyState(VK_SHIFT) < 0);
	else if(nFlag==3)	
	{
		return	(::GetKeyState(VK_CONTROL) < 0)&&(::GetKeyState(VK_MENU) >= 0)
			&&(::GetKeyState(VK_SHIFT) >= 0);
	}
	else if(nFlag==4)	
	{
		return	(::GetKeyState(VK_CONTROL)<0)&&(::GetKeyState(VK_MENU)  <  0)
			&&(::GetKeyState(VK_SHIFT) >= 0);
	}
	return 0;
}

CString CXRichEditCtrl::GetFilePath(int nFlag)
{
	CString str;
	str=AfxGetApp()->GetProfileString("Settings","XREPath");
	if(!str.IsEmpty()&&!nFlag)	return str;

	::GetModuleFileName(NULL,str.GetBufferSetLength(MAX_PATH+1), MAX_PATH);
	int position=str.ReverseFind('\\'); 
	str=str.Left(position+1)+AfxGetApp()->m_pszExeName+".pcf";
	return str;
}

int CXRichEditCtrl::GotoLine(int nFlag, int nLineIndex)
{
	CHKLINE(nLineIndex);
	if(nFlag==1)
	{
		nLineIndex+=1;
		if(nLineIndex>=GetLineCount())	nLineIndex-=1;
	}
	else if(nFlag==2)
	{
		if(nLineIndex>0)	nLineIndex-=1;
	}
	SetSel(LineIndex(nLineIndex),LineIndex(nLineIndex));
	return nLineIndex;
}

void CXRichEditCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRichEditCtrl::OnMouseMove(nFlags, point);

	if(!GetSelText().IsEmpty())	
	{
		long ns=0,ne=0;
		GetSel(ns,ne);	
		if(GetSelText().Right(1)=='\n')	ne-=1;
		ns=GetLineStart(LineFromChar(ne));
		LN_COL_MSG(MAKELPARAM(LineFromChar(ne),ne-ns))
	}
}



