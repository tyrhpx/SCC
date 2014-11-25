// Transmit.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "SCC.h"
#include "Transmit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransmit dialog


CTransmit::CTransmit(CWnd* pParent /*=NULL*/)
	: CDialog(CTransmit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransmit)
	m_btCOM1 = 1;
	m_btCOM2 = 2;
	m_bCOM = FALSE;
	//}}AFX_DATA_INIT
}


void CTransmit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransmit)
	DDX_Text(pDX, IDC_EDIT_COMSRC, m_btCOM1);
	DDV_MinMaxByte(pDX, m_btCOM1, 1, 255);
	DDX_Text(pDX, IDC_EDIT_COMDEST, m_btCOM2);
	DDV_MinMaxByte(pDX, m_btCOM2, 1, 255);
	DDX_Check(pDX, IDC_CHECK_COM, m_bCOM);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransmit, CDialog)
	//{{AFX_MSG_MAP(CTransmit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransmit message handlers

void CTransmit::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(m_btCOM2==m_btCOM1)	
	{
		m_btCOM2=m_btCOM1+1;
		UpdateData(0);
		AfxMessageBox("需输入不同的串口号！");
		return;
	}
	DWORD dwConfig=0;
//	dwConfig|=m_bCOM;
	dwConfig|=(m_btCOM1);
	dwConfig|=(m_btCOM2<<8);
	AfxGetApp()->WriteProfileInt("Settings\\Transmit","COM",dwConfig);
//	CDialog::OnOK();
}

BOOL CTransmit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	DWORD dwConfig=AfxGetApp()->GetProfileInt("Settings\\Transmit","COM",0);
	if(dwConfig!=0)
	{
//		m_bCOM=dwConfig&0x01;
		m_btCOM1=(BYTE)((dwConfig&0x000000FF));
		m_btCOM2=(BYTE)((dwConfig&0x0000FF00)>>8);
		UpdateData(0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
