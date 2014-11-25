// DlgSend.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "SCC.h"
#include "DlgSend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSend dialog


CDlgSend::CDlgSend(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSend::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSend)
	m_nCycMode = 0;
	m_nCycSel = 0;
	m_unTime = 1000;
	m_unLine = 1;
	m_unStart = 1;
	m_unEnd = 2;
	m_unTrigger = 0;
	//}}AFX_DATA_INIT
}


void CDlgSend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSend)
	DDX_Control(pDX, IDC_LIST_TRIGGER, m_ClassList);
	DDX_Radio(pDX, IDC_RADIO_LINE, m_nCycMode);
	DDX_Radio(pDX, IDC_RADIO_CURLINE, m_nCycSel);
	DDX_Text(pDX, IDC_EDIT_TIME, m_unTime);
	DDV_MinMaxUInt(pDX, m_unTime, 20, 108000000);
	DDX_Text(pDX, IDC_EDIT_ONELINE, m_unLine);
	DDV_MinMaxUInt(pDX, m_unLine, 1, 60000);
	DDX_Text(pDX, IDC_EDIT_MULTILINES, m_unStart);
	DDV_MinMaxUInt(pDX, m_unStart, 1, 60000);
	DDX_Text(pDX, IDC_EDIT_MULTILINES2, m_unEnd);
	DDV_MinMaxUInt(pDX, m_unEnd, 2, 60000);
	DDX_Text(pDX, IDC_EDIT_TRIGGER, m_unTrigger);
	DDV_MinMaxUInt(pDX, m_unTrigger, 0, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSend, CDialog)
	//{{AFX_MSG_MAP(CDlgSend)
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_RADIO_LINE,IDC_BUTTON_HIDE,OnCycLine)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSend message handlers

void CDlgSend::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM (point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgSend::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CDlgSend::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

BOOL CDlgSend::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE)
		{
			GetParent()->SetFocus();
		//	SetTransper(23);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgSend::OnCycLine(UINT nID)
{
	// TODO: Add extra cleanup here
	if(nID==IDC_RADIO_LINE)
	{
		GetDlgItem(IDC_RADIO_CURLINE)->EnableWindow(1);
		GetDlgItem(IDC_RADIO_ONELINE)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_ONELINE)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_MULTILINES)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_MULTILINES2)->EnableWindow(0);
//		GetDlgItem(IDC_BUTTON_USE)->EnableWindow(1);
	}
	else if(nID==IDC_RADIO_LINES)
	{
		GetDlgItem(IDC_RADIO_CURLINE)->EnableWindow(0);
		GetDlgItem(IDC_RADIO_ONELINE)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_ONELINE)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_MULTILINES)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_MULTILINES2)->EnableWindow(1);
//		GetDlgItem(IDC_BUTTON_USE)->EnableWindow(1);
	}
	else if(nID==IDC_BUTTON_USE)
	{
		UpdateData();
		if(m_unTime<20)	return;
		if(g_mw->m_unCycTimer!=0)
		{
			KillTimer(g_mw->m_unCycTimer);
			g_mw->m_unCycTimer=g_mw->SetTimer(101,m_unTime,NULL);
			g_mw->m_unMulti=m_unStart;
		}
		CString str;
		int nLine;
		for(int i=0;i<TRIGGER_MAX;i++)
		{
			str=m_ClassList.GetItemText(i,1);
			Trigger[i].bValiu=0;
			if(!str.IsEmpty())	
			{
				_stscanf(str,"%d",&nLine);
				if(nLine>0)	
				{
					Trigger[i].unLine=nLine;
					str=m_ClassList.GetItemText(i,2);
					if(!str.IsEmpty())
					{
						_stscanf(str,"%d",&nLine);
						if(nLine>0)
						{
							Trigger[i].unTrigger=nLine;
							Trigger[i].bValiu=1;
						}
					}
				}
			}
		}
		str.Empty();
//		GetDlgItem(IDC_BUTTON_USE)->EnableWindow(0);
	}
	else if(nID==IDC_BUTTON_HIDE)
	{
		ShowWindow(0);
	}
}

BOOL CDlgSend::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend","CycTimer",m_unTime);
	AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend","CycLine",MAKELPARAM(m_unLine,m_unTrigger));
	AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend","CycLines",MAKELPARAM(m_unStart,m_unEnd));
//	AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend","CycELine",m_unEnd);
	DWORD dwConfig=0;
	if(m_nCycMode==1)	dwConfig|=0x01;
	if(m_nCycSel==1)	dwConfig|=0x02;
	AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend","Common",dwConfig);
	CString str;
	for(int i=0; i<TRIGGER_MAX; i++)
	{
		str.Format("Trigger%d",i+1);
		if(Trigger[i].bValiu)			
			AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend",str,
			MAKELPARAM(Trigger[i].unLine,Trigger[i].unTrigger));
		else	AfxGetApp()->WriteProfileInt("Settings\\AdvanceSend",str,0);
	}
	str.Empty();
	return CDialog::DestroyWindow();
}

BOOL CDlgSend::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_unTime=AfxGetApp()->GetProfileInt("Settings\\AdvanceSend","CycTimer",1000);
	m_unLine=AfxGetApp()->GetProfileInt("Settings\\AdvanceSend","CycLine",1);
	m_unTrigger=HIWORD(m_unLine);
	m_unLine=LOWORD(m_unLine);
	m_unStart=AfxGetApp()->GetProfileInt("Settings\\AdvanceSend","CycLines",0x00020001);
	m_unEnd=HIWORD(m_unStart);
	m_unStart=LOWORD(m_unStart);
	DWORD dwConfig=AfxGetApp()->GetProfileInt("Settings\\AdvanceSend","Common",0);
	m_nCycMode=dwConfig&0x01;
	m_nCycSel=(dwConfig&0x02)>>1;

//	m_btnUse.SubclassDlgItem(IDC_BUTTON_USE,this);
//	m_btnUse.ShowText();
//	m_btnUse.SetCaption("&Ok");
//	m_btnUse.SetStyleBorder(CGuiButton::STYLEXP);
//	m_btnUse.SetImage(IDR_MAINFRAME,-1,-1);
//	m_btnUse.ShowImage();
	
	GetDlgItem(IDC_RADIO_CURLINE)->EnableWindow(m_nCycMode==0);
	GetDlgItem(IDC_RADIO_ONELINE)->EnableWindow(m_nCycMode==0);
	GetDlgItem(IDC_EDIT_ONELINE)->EnableWindow(m_nCycMode==0);
	GetDlgItem(IDC_EDIT_MULTILINES)->EnableWindow(m_nCycMode!=0);
	GetDlgItem(IDC_EDIT_MULTILINES2)->EnableWindow(m_nCycMode!=0);
	GetDlgItem(IDC_EDIT_ONELINE)->EnableWindow(m_nCycMode==0);
//	GetDlgItem(IDC_BUTTON_USE)->EnableWindow(0);
	UpdateData(0);
	ListView_SetExtendedListViewStyle(m_ClassList.m_hWnd, LVS_EX_GRIDLINES); 

	m_ClassList.InsertColumn(0, "序号", LVCFMT_LEFT, 40);
	m_ClassList.InsertColumn(1, "收到行", LVCFMT_LEFT, 50);
	m_ClassList.InsertColumn(2, "触发行", LVCFMT_LEFT, 50);

	CString strName;
	for(int i=0; i<TRIGGER_MAX; i++)
	{
		strName.Format("%d", i+1);
		m_ClassList.InsertItem( i, strName);
	}
	CString str;
	for(int i=0; i<TRIGGER_MAX; i++)
	{
		Trigger[i].bValiu=0;
		str.Format("Trigger%d",i+1);
		Trigger[i].unLine = AfxGetApp()->GetProfileInt("Settings\\AdvanceSend",str,0);
		if(Trigger[i].unLine!=0)
		{
			Trigger[i].bValiu=1;
			Trigger[i].unTrigger=HIWORD(Trigger[i].unLine);
			Trigger[i].unLine=LOWORD(Trigger[i].unLine);
			strName.Format("%d", Trigger[i].unLine);
			m_ClassList.SetItemText(i, 1, strName);  // 插入第二列
			strName.Format("%d", Trigger[i].unTrigger);
			m_ClassList.SetItemText(i, 2, strName);  // 插入第三列
		}
	}
	if(Trigger[0].bValiu==0)
	{
		strName.Format("%d", 2);
		m_ClassList.SetItemText(0, 1, strName);  // 插入第二列
		strName.Format("%d", 3);
		m_ClassList.SetItemText(0, 2, strName);  // 插入第三列
		Trigger[0].bValiu=1;
		Trigger[0].unLine=2;
		Trigger[0].unTrigger=3;
	}
	str.Empty();
	strName.Empty();
	return TRUE; 
}
typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
void CDlgSend::SetTransper(BYTE btTrans)
{
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)|0x80000);
	HINSTANCE hInst = LoadLibrary("User32.DLL"); 
	if(hInst) 
	{ 
		MYFUNC fun = NULL;
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)	fun(this->GetSafeHwnd(),GetDC()->GetPixel(0,0),btTrans,2); 
		FreeLibrary(hInst); 
	}
}

void CDlgSend::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
//	SetTransper(255);
}
