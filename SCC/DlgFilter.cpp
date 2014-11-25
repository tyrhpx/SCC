// DlgFilter.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "SCC.h"
#include "DlgFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFilter dialog


CDlgFilter::CDlgFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFilter)
	m_nFilter = 0;
	m_bFilter = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFilter)
	DDX_Control(pDX, IDC_LIST_FILTER, m_ClassList);
	DDX_Radio(pDX, IDC_RADIO_ALL, m_nFilter);
	DDX_Check(pDX, IDC_CHECK_FILTER, m_bFilter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFilter, CDialog)
	//{{AFX_MSG_MAP(CDlgFilter)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_FILTER, OnButtonFilter)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFilter message handlers

void CDlgFilter::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CDlgFilter::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CDlgFilter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM (point.x,point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CDlgFilter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ListView_SetExtendedListViewStyle(m_ClassList.m_hWnd, LVS_EX_GRIDLINES);
	m_ClassList.InsertColumn(0, "序号", LVCFMT_LEFT, 40);
	m_ClassList.InsertColumn(1, "字节序号", LVCFMT_LEFT, 60);
	m_ClassList.InsertColumn(2, "字节值(HEX)", LVCFMT_LEFT, 80);
	CString strName;
	for(int i=0; i<FILTER_MAX; i++)
	{
		strName.Format("%d", i+1);
		m_ClassList.InsertItem( i, strName);
	}
	CString str;
	for(int i=0; i<FILTER_MAX; i++)
	{
		Filter[i].bFilter=0;
		str.Format("Filter%d",i+1);
		DWORD dws;
		dws = AfxGetApp()->GetProfileInt("Settings\\Filter",str,0);
		if(dws!=0)
		{
			Filter[i].bFilter=1;
			Filter[i].btNo=(BYTE)(LOWORD(dws));
			Filter[i].btValiu=(BYTE)(HIWORD(dws));
			strName.Format("%d", Filter[i].btNo);
			m_ClassList.SetItemText(i, 1, strName);  // 插入第二列
			strName.Format("%02X", Filter[i].btValiu);
			m_ClassList.SetItemText(i, 2, strName);  // 插入第三列
		}
	}
	DWORD dwConfig=AfxGetApp()->GetProfileInt("Settings\\Filter","Common",0);
	m_nFilter=dwConfig&0x01;
	UpdateData(0);
	str.Empty();
	strName.Empty();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgFilter::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwConfig=0;
	if(m_nFilter==1)	dwConfig|=0x01;
	AfxGetApp()->WriteProfileInt("Settings\\Filter","Common",dwConfig);
	CString str;
	for(int i=0; i<FILTER_MAX; i++)
	{
		str.Format("Filter%d",i+1);
		
		if(Filter[i].bFilter)
			AfxGetApp()->WriteProfileInt("Settings\\Filter",str,
			MAKELPARAM(Filter[i].btNo,Filter[i].btValiu));
		else	AfxGetApp()->WriteProfileInt("Settings\\Filter",str,0);
	}
	str.Empty();
	
	return CDialog::DestroyWindow();
}

void CDlgFilter::OnButtonFilter() 
{
	// TODO: Add your control notification handler code here
		UpdateData();
		CString str;
		int nLine;
		for(int i=0;i<FILTER_MAX;i++)
		{
			str=m_ClassList.GetItemText(i,1);
			Filter[i].bFilter=0;
			if(!str.IsEmpty())	
			{
				_stscanf(str,"%d",&nLine);
				if(nLine>0)	
				{
					Filter[i].btNo=nLine;
					str=m_ClassList.GetItemText(i,2);
					if(!str.IsEmpty())
					{
						_stscanf(str,"%02X",&nLine);
						if(nLine>=0)
						{
							Filter[i].btValiu=nLine;
							Filter[i].bFilter=1;
						}
					}
				}
			}
		}
		str.Empty();
}

void CDlgFilter::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(0);
}
