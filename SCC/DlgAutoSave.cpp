// DlgAutoSave.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "SCC.h"
#include "DlgAutoSave.h"
#include "DirectoryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoSave dialog


CDlgAutoSave::CDlgAutoSave(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutoSave::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAutoSave)
	m_strPath = _T("");
	m_bAS = FALSE;
	m_nASTime = 1;
	m_bClear = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgAutoSave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAutoSave)
	DDX_Text(pDX, IDC_BROWSE_EDIT, m_strPath);
	DDX_Check(pDX, IDC_CHECK_AS, m_bAS);
	DDX_Text(pDX, IDC_EDIT_ASTIME, m_nASTime);
	DDV_MinMaxInt(pDX, m_nASTime, 1, 60000);
	DDX_Check(pDX, IDC_CHECK_CLEAR, m_bClear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAutoSave, CDialog)
	//{{AFX_MSG_MAP(CDlgAutoSave)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoSave message handlers

void CDlgAutoSave::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	CDirectoryDialog DirDlg;
	DirDlg.ShowDirectoryTree(GetDlgItem(IDC_BROWSE_EDIT));
}
