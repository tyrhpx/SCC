
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SCC.h"

#include "MainFrm.h"
#include "Transmit.h"
#include "DlgAutoSave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
#define WM_ALWAYSONTOP	WM_USER + 5

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_XSAVE, OnFileXsave)
	ON_UPDATE_COMMAND_UI(ID_FILE_XSAVE, OnUpdateFileXsave)
	ON_COMMAND(ID_VIEW_SFWND, OnViewSfWnd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SFWND, OnUpdateViewSfWnd)
	ON_COMMAND(IDR_SCC_TOOL, OnSccTool)
	ON_UPDATE_COMMAND_UI(IDR_SCC_TOOL, OnUpdateSccTool)

	ON_COMMAND_RANGE(ID_SCC_SET_COM, ID_SCC_SET_RTO, OnSccSetButton)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCC_SET_BAUD, ID_SCC_SET_RTO, OnUpdateSccSetButton)

	ON_CBN_SELCHANGE(ID_SCC_SET_COM, &CMainFrame::OnSCCComboComSelok)

	ON_COMMAND_RANGE(ID_SCC_SET_OPTION,ID_SCC_ADDECHO,OnSCCCommon)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCC_SET_OPTION,ID_SCC_ADDECHO,OnUpdateSCCCommon)

	ON_COMMAND_RANGE(ID_CS_ADDSUM,ID_CS_7,OnSCCCS)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CS_ADDSUM,ID_CS_7,OnUpdateSCCCS)
	ON_COMMAND_RANGE(ID_FILE_MRU_FILE1,ID_FILE_MRU_FILE16,OnSCCMRU)
	ON_COMMAND_RANGE(ID_VIEW_CENTER,ID_FILE_SAVESEND,OnSCCView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_CENTER,ID_FILE_SAVESEND,OnUpdateSCCView)

	 SCC_MSG_MAP
	 ON_WM_TIMER()
	 ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构
SCC_MSG_MAP_X(CMainFrame,RecvSCCMsg)

CMainFrame::CMainFrame()
{
	m_nCurCOM=0;
	m_bSPort=1,m_bSSend=1,m_bSTime=1;
	m_bAscRecv=0,m_bAscSend=0;
	m_bAddEnter=0,m_bAddStx=0,m_bAddEtx=0;
	m_bAddEcho=0;
	m_bPauseDis=0;
	m_nToolBtns=0;
	m_bToNext=0;
	m_unCycTimer=0;
	m_dwMs=::GetTickCount();
	m_unMulti=0;
	m_bTrigger=0;
	m_btTrigger=0;
	m_btTriggerCOM=0;
	Trasmt.bCOM=0;
	m_bFilter=0;
	AutoSave.bSave=0;
	AutoSave.bClear=0;
	AutoSave.nTime=1;
	AutoSave.strPath="";
	m_bAutoMenuEnable  = FALSE;
}

CMainFrame::~CMainFrame()
{
	delete m_pCbxCOM;
	delete m_pCbxBaud;
	delete m_pCbxParity;
	delete m_pCbxData;
	delete m_pCbxStop;
	delete m_pCbxRTO;
	delete m_pCbxFC;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// 设置用于绘制所有用户界面元素的视觉管理器
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	ReplaceBtnIcon(ID_SCC_OPEN, 2);

	CString strToolBarName = "工具栏";
	m_wndToolBar.SetWindowText(strToolBarName);


	if (!m_wndSCCTool.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0,0,0,0), IDR_SCC_TOOL) ||
		!m_wndSCCTool.LoadToolBar(IDR_SCC_TOOL))
	{
		TRACE0("未能创建SCC工具栏\n");
		return -1;      // 未能创建
	}
	
	m_wndSCCTool.SetWindowText(_T("串口设置"));
	CreateSCCTool();
	UINT uHis=AfxGetApp()->GetProfileInt("Settings","COMHis",0);
	m_pCbxCOM->SelectItem((int)uHis);
	CSerialPort *psp=new CSerialPort;
	psp->m_nCOMNo=uHis;
	if(CSCCTools::RWReg(psp))	SetCOMParam(psp,1);
	delete psp;
	psp=NULL;	

// 	CString strCustomize;
// 	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
// 	ASSERT(bNameValid);
// 	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSCCTool.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);
	DockPane(&m_wndSCCTool);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);

	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	CreateImagelist();

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndSF.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndSF);

	// 启用工具栏和停靠窗口菜单替换
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	//CMFCToolBar::EnableQuickCustomization();

	//LoadBarState(_T("MFW"));
	m_dlgSend.Create(IDD_DIALOG_SEND,this);
	m_dlgFilter.Create(IDD_DIALOG_FILTER,this);

	DWORD dwConfig=AfxGetApp()->GetProfileInt("Settings","Common",1);
	m_bSPort=dwConfig&0x01;m_bSSend=(dwConfig&0x02)>>1;m_bSTime=(dwConfig&0x04)>>2;
	m_bAscRecv=(dwConfig&0x08)>>3;
	m_bAscSend=(dwConfig&0x10)>>4;
	m_bAddEnter=(dwConfig&0x20)>>5;
	m_bAddStx=(dwConfig&0x40)>>6;
	m_bAddEtx=(dwConfig&0x80)>>7;
	if(dwConfig&0x100)	
		SetWindowPos(&wndTopMost,0, 0, 0, 0,SWP_NOSIZE|SWP_NOMOVE);
	AutoSave.strPath=AfxGetApp()->GetProfileString("Settings","AutoSave");
	if(AutoSave.strPath.IsEmpty())	
	{
		AutoSave.strPath=AfxGetApp()->m_pszAppName;
		int position=AutoSave.strPath.ReverseFind('\\'); 
		if(position!=-1)	
			AutoSave.strPath=AutoSave.strPath.Left(position+1);
		else	
			AutoSave.strPath="c:\\";
	}
	dwConfig=AfxGetApp()->GetProfileInt("Settings","ASTime",0x00010001);
	AutoSave.nTime=dwConfig;

	EnableToolTips();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style &=~FWS_ADDTOTITLE ;
	cs.lpszName=_T("SCC－串口监视工具");
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	// 创建发送窗口
	CString strSendFrmWnd = _T("发报窗");
	if (!m_wndSF.CreateX(strSendFrmWnd, this, ID_VIEW_SFWND, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("未能创建发送窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndSF.SetIcon(hOutputBarIcon, FALSE);

}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->Create();
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}


BOOL CMainFrame::DestroyWindow()
{
	m_ascc.SaveConfig();
	AfxGetApp()->WriteProfileInt("Settings","COMHis",m_nCurCOM);
	DWORD dwConfig=m_bSPort|(m_bSSend<<1)|(m_bSTime<<2)|
		(m_bAscRecv<<3)|(m_bAscSend<<4)|
		(m_bAddEnter<<5)|(m_bAddStx<<6)|(m_bAddEtx<<7);
	DWORD dwTmp=AfxGetApp()->GetProfileInt("Settings","Common",1);
	dwConfig|=dwTmp&0x100;
	AfxGetApp()->WriteProfileInt("Settings","Common",dwConfig);
	AfxGetApp()->WriteProfileString("Settings","AutoSave",AutoSave.strPath);
	AfxGetApp()->WriteProfileInt("Settings","ASTime",AutoSave.nTime);

	m_wndSF.SaveConfig();
	if(m_dlgSend.m_hWnd)	
		m_dlgSend.DestroyWindow();
	if(m_dlgFilter.m_hWnd)	
		m_dlgFilter.DestroyWindow();
	if(m_unCycTimer!=0)	
		KillTimer(m_unCycTimer);
	return CFrameWndEx::DestroyWindow();
}

int CMainFrame::IsTriggerLine( CString str )
{
	for(int i=0;i<TRIGGER_MAX;i++)
	{
		if(m_dlgSend.Trigger[i].bValiu)
		{
			if(str==m_wndSF.m_xrec.GetLineStr(m_dlgSend.Trigger[i].unLine-1))
				return i;
		}
	}
	return -1;
}

BOOL CMainFrame::SendLine( int nCOMIndex,int nLineIndex/*=-1*/ )
{
	if(!IsCOMOpen(nCOMIndex))	
		return 1;
	CString str;
	int nlength=m_wndSF.m_xrec.GetLineStr(str,nLineIndex);
	if(str.IsEmpty())	
	{
//		AfxMessageBox("空行！");
		m_wndSF.m_xrec.SetFocus();
		return 0;
	}
	if(m_bAscSend)
	{
		AscSendAdd(str);
		DisplayStr(str,nCOMIndex,0);
		GetCOM(nCOMIndex)->WriteData(str);
		if(m_bToNext)	
			m_wndSF.m_xrec.GotoLine(1,nLineIndex);
		return 1;
	}
	if(!m_wndSF.m_xrec.IsHexFormat(str) )
	{
//		AfxMessageBox("格式不对！组合键Ctrl+Alt+P分析错误！");
		m_wndSF.m_xrec.SetFocus();
		return 0;
	}
	BYTE btData[MAX_SCC_SEND_LEN];
	int nBytes=m_wndSF.m_xrec.StrToHex(str,btData);
	DisplayStr(str,nCOMIndex,0);
	GetCOM(nCOMIndex)->WriteData(btData,nBytes);	
	if(m_bToNext)	
		m_wndSF.m_xrec.GotoLine(1,nLineIndex);
	return 1;
}

int CMainFrame::AscSendAdd( CString &str )
{
	if(m_bAscSend)	
	{
		if(m_bAddStx)	str=(TCHAR)0x02+str;
		if(m_bAddEtx)	str+=(TCHAR)0x03;
		if(m_bAddEnter)	str+='\r';
		if(m_bAddEcho)	str+='\n';
	}
	return str.GetLength();
}

CSerialPort* CMainFrame::GetCOM( int nCOMIndex/*=-1*/ )
{
	if(nCOMIndex==-1)	
		nCOMIndex=m_nCurCOM;
	int nIndex=m_ascc.GetCOMIndex(nCOMIndex);
	if(nIndex==-1)	
		return NULL;
	return m_ascc.GetCOM(nIndex);
}

BOOL CMainFrame::IsCOMOpen( int nCOMIndex/*=-1*/ )
{
	if(nCOMIndex==-1)	
		nCOMIndex=m_nCurCOM;
	int nIndex=m_ascc.GetCOMIndex(nCOMIndex);
	if(nIndex==-1)	
		return 0;
	return m_ascc.GetCOM(nIndex)->IsOpen();
}

void CMainFrame::DisplayStr( CString str,int nCOMIndex,BOOL bRecv/*=1*/,BOOL bEnter/*=1*/ )
{
	if(m_bPauseDis)	
		return;
	if(!bRecv&&!m_bSSend)
		return;
	if(m_dlgFilter.m_nFilter==1)
	{
		if(nCOMIndex!=m_nCurCOM)	
			return;
	}
	CString strtmp,str1;
	CSCCView* pView=GetView();
	if(m_bSTime)
	{
		_strtime( strtmp.GetBuffer(9) );
		strtmp.ReleaseBuffer();
		str1.Format(" %03.0d",(::GetTickCount()-m_dwMs)%1000);
		m_dwMs=::GetTickCount();
		strtmp="["+strtmp+str1+"] ";
		pView->AddString(strtmp,5);
	}
	if(m_bSPort)//↑↓
	{
		pView->AddString("COM",5);
		if(bRecv)	
			strtmp.Format("%d",nCOMIndex+1);
		else		
			strtmp.Format("%d",nCOMIndex+1);
		pView->AddString(strtmp,1);
		if(bRecv)	
			pView->AddString("↑",5);
		else		
			pView->AddString("↓",5);
	}
	strtmp.Empty();
	str1.Empty();
	str+='\n';
	pView->AddString(str,bRecv?3:4);
	pView=NULL;
}

void CMainFrame::SetCOMParam( CSerialPort *psp, int nSet )
{
	if(nSet==0)	//以当前组合框值设置当前所选串口
	{
		psp->m_nBaudRate=CSCCTools::GetParams(m_pCbxBaud->GetCurSel(),0);
		psp->m_nParityBit=CSCCTools::GetParams(m_pCbxParity->GetCurSel(),1);
		psp->m_nDataBit=CSCCTools::GetParams(m_pCbxData->GetCurSel(),2);
		psp->m_nStopBit=CSCCTools::GetParams(m_pCbxStop->GetCurSel(),3);
		psp->m_nRTO=(m_pCbxRTO->GetCurSel()+1)*20;
	}
	else if(nSet==1)//设置当前所选串口
	{
		m_pCbxBaud->SelectItem(CSCCTools::GetParIndex(psp->m_nBaudRate,0));
		m_pCbxParity->SelectItem(CSCCTools::GetParIndex(psp->m_nParityBit,1));
		m_pCbxData->SelectItem(CSCCTools::GetParIndex(psp->m_nDataBit,2));
		m_pCbxStop->SelectItem(CSCCTools::GetParIndex(psp->m_nStopBit,3));
		m_pCbxRTO->SelectItem((psp->m_nRTO>=20)?(psp->m_nRTO/20-1):0);
	}
	else if(nSet==2)//设置组合框
	{
		m_pCbxBaud->SelectItem(CSCCTools::GetParIndex(115200,0));
		m_pCbxParity->SelectItem(CSCCTools::GetParIndex(NOPARITY,1));
		m_pCbxData->SelectItem(CSCCTools::GetParIndex(8,2));
		m_pCbxStop->SelectItem(CSCCTools::GetParIndex(ONESTOPBIT,3));
		m_pCbxRTO->SelectItem(0);
	}
	else//设置当前所选串口
	{
		psp->m_nBaudRate=115200;
		psp->m_nParityBit=NOPARITY;
		psp->m_nDataBit=8;
		psp->m_nStopBit=ONESTOPBIT;
		psp->m_nRTO=20;
	}
}

CString CMainFrame::GetRWPath( BOOL bOpen,LPCSTR lpstr,LPCSTR lpstrName/*=NULL*/ )
{
	CString strs;
	CFileDialog* dlgFileOpen=new CFileDialog(bOpen,".pcf",lpstrName); 
	dlgFileOpen->m_ofn.lStructSize=(GetVersion() < 0x80000000)?88:76; 
	dlgFileOpen->m_ofn .lpstrFilter ="PCStudio Frame Files(*.pcf)\0*.pcf\0";
	dlgFileOpen->m_ofn.lpstrTitle =lpstr;
	strs.Empty ();	
	if(dlgFileOpen->DoModal()==IDOK)	
		strs=dlgFileOpen->GetPathName();
	delete dlgFileOpen;
	dlgFileOpen=NULL;
	return strs;
}

BOOL CMainFrame::IsFilterFrame( LPBYTE lpb,int nLength )
{
	if(!m_dlgFilter.m_bFilter)	return 1;
	BOOL bFilter=1;
	for(int i=0;i<FILTER_MAX;i++)
	{
		if(m_dlgFilter.Filter[i].bFilter)
		{
			bFilter=0;
			if(m_dlgFilter.Filter[i].btNo>nLength)	
				return 0;
			if(lpb[m_dlgFilter.Filter[i].btNo-1]==m_dlgFilter.Filter[i].btValiu)
				return 1;
		}
	}
	return bFilter;
}

void CMainFrame::Transmits( int nCOM,LPBYTE lpbt,int nLength )
{
	if(!Trasmt.bCOM)	
		return;
	CString str;
	if(Trasmt.btCOM1==nCOM+1)	
		nCOM=Trasmt.btCOM2-1;
	else if(Trasmt.btCOM2==nCOM+1)	
		nCOM=Trasmt.btCOM1-1;
	if(!IsCOMOpen(nCOM))	
		return;
	if(m_bAscRecv)	
		str=((CString)lpbt).Left (nLength);
	else	
		str=CSCCTools::HexToStr(lpbt,nLength);
	DisplayStr(str,nCOM,0);
	GetCOM(nCOM)->WriteData(lpbt,nLength);
	str.Empty();
}

void CMainFrame::ReplaceBtnIcon( UINT unID, int nNew )
{
	int nIndex=m_wndToolBar.CommandToIndex(unID);
	m_wndToolBar.SetButtonInfo(nIndex,unID,TBBS_BUTTON,nNew);
}

void CMainFrame::CreateImagelist()
{
	CMFCToolBarImages* pImgs = m_wndToolBar.GetImages();
	m_nToolBtns = pImgs->GetCount();

	CMFCToolBarImages imgShell;
	imgShell.Load(IDR_TOOLBAR_SHELL);

	pImgs->AddImage(imgShell, 0);
	pImgs->AddImage(imgShell, 1);
	pImgs->AddImage(imgShell, 2);
	pImgs->AddImage(imgShell, 3);
}

void CMainFrame::RecvSCCMsg( WPARAM wParam, LPARAM lParam )
{
	LPBYTE lpbtBuffer = (LPBYTE)::GlobalLock((HGLOBAL)wParam);
	WORD lPortNo=LOWORD(lParam);
	WORD lLength=HIWORD(lParam);
	//======
	CString str;
	m_bFilter=0;
	if(m_bAscRecv)	
	{
		str=((CString)lpbtBuffer).Left (lLength);
		m_bFilter=1;
	}
	else	
	{
		if(IsFilterFrame(lpbtBuffer,lLength))
		{
			str=CSCCTools::HexToStr(lpbtBuffer,lLength);
			m_bFilter=1;
		}	
	}
	if(m_bFilter)	
		DisplayStr(str,lPortNo);

	Transmits(lPortNo,lpbtBuffer,lLength);
	if(m_bTrigger)
	{
		int nl=IsTriggerLine(str);
		if(nl!=-1)
		{
			if(m_dlgSend.m_unTrigger==0)	
				SendLine(lPortNo,m_dlgSend.Trigger[nl].unTrigger-1);
			else	
			{
				m_btTrigger=nl;
				m_btTriggerCOM=(BYTE)lPortNo;
				SetTimer(102,m_dlgSend.m_unTrigger,NULL);
			}
		}
	}
	str.Empty();
	::GlobalUnlock((HGLOBAL)wParam);
}

void CMainFrame::CreateSCCTool()
{
	CString strTmp = _T("");

	//COM
	m_pCbxCOM = new CMFCToolBarComboBoxButton(ID_SCC_SET_COM, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 70);
	for(int i=0; i<255; i++)
	{
		strTmp.Format("COM%d", i+1);
		m_pCbxCOM->AddItem(strTmp);
	}
	m_pCbxCOM->SelectItem(0);
	m_pCbxCOM->SetCenterVert();
	m_pCbxCOM->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_COM, *m_pCbxCOM);

	//BAUD
	m_pCbxBaud = new CMFCToolBarComboBoxButton(ID_SCC_SET_BAUD, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 70);
	for(int i=0; i<23; i++)
		m_pCbxBaud->AddItem(CSCCTools::GetParStr(i,0,1));
	m_pCbxBaud->SelectItem(20); //115200
	m_pCbxBaud->SetCenterVert();
	m_pCbxBaud->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_BAUD, *m_pCbxBaud);

	//PARITY
	m_pCbxParity = new CMFCToolBarComboBoxButton(ID_SCC_SET_PARITY, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 70);
	m_pCbxParity->AddItem("无校验");
	m_pCbxParity->AddItem("奇校验");
	m_pCbxParity->AddItem("偶校验");
	m_pCbxParity->AddItem("Mark");
	m_pCbxParity->AddItem("Space");
	m_pCbxParity->SelectItem(0);
	m_pCbxParity->SetCenterVert();
	m_pCbxParity->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_PARITY, *m_pCbxParity);

	//DATA
	m_pCbxData = new CMFCToolBarComboBoxButton(ID_SCC_SET_DATA, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 40);
	m_pCbxData->AddItem("8");
	m_pCbxData->AddItem("7");
	m_pCbxData->AddItem("6");
	m_pCbxData->AddItem("5");
	m_pCbxData->AddItem("4");
	m_pCbxData->SelectItem(0);
	m_pCbxData->SetCenterVert();
	m_pCbxData->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_DATA, *m_pCbxData);

	//STOP
	m_pCbxStop = new CMFCToolBarComboBoxButton(ID_SCC_SET_STOP, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 40);
	m_pCbxStop->AddItem("1");
	m_pCbxStop->AddItem("1.5");
	m_pCbxStop->AddItem("2");
	m_pCbxStop->SelectItem(0);
	m_pCbxStop->SetCenterVert();
	m_pCbxStop->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_STOP, *m_pCbxStop);

	//RTO
	m_pCbxRTO = new CMFCToolBarComboBoxButton(ID_SCC_SET_RTO, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 50);
	for(int i=0; i<15; i++)
	{
		strTmp.Format("%d",(i+1)*20);
		m_pCbxRTO->AddItem(strTmp);
	}
	m_pCbxRTO->SelectItem(0);
	m_pCbxRTO->SetCenterVert();
	m_pCbxRTO->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_RTO, *m_pCbxRTO);

	//FLOW
	m_pCbxFC = new CMFCToolBarComboBoxButton(ID_SCC_SET_FLOW, 0, WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP, 50);
	m_pCbxFC->AddItem("无");
	m_pCbxFC->AddItem("RTS/CTS");
	m_pCbxFC->AddItem("XON/XOFF");
	m_pCbxFC->AddItem("DTR/CTS");
	m_pCbxFC->AddItem("RTS/DSR");
	m_pCbxFC->AddItem("DTR/DSR");
	m_pCbxFC->SelectItem(0);
	m_pCbxFC->EnableWindow(0);
	m_pCbxFC->SetCenterVert();
	m_pCbxFC->SetFlatMode();
	m_wndSCCTool.ReplaceButton(ID_SCC_SET_FLOW, *m_pCbxFC);

	strTmp.Empty();
}

BOOL CMainFrame::IsCASDown( int nFlag )
{
	if(nFlag==0)		
		return (::GetKeyState(VK_CONTROL) < 0);
	else if(nFlag==1)	
		return (::GetKeyState(VK_MENU) < 0);
	else if(nFlag==2)	
		return (::GetKeyState(VK_SHIFT) < 0);
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


void CMainFrame::OnSccSetButton( UINT nID )
{
	//用于保证工具栏按钮可用
}

void CMainFrame::OnUpdateSccSetButton( CCmdUI* pCmdUI )
{
// 	m_pCbxBaud->EnableWindow(bEnable);
// 	CMFCToolBarComboBoxButton::GetByCmd(ID_SCC_SET_PARITY, FALSE)->EnableWindow(bEnable);
// 	CMFCToolBarComboBoxButton::GetByCmd(ID_SCC_SET_DATA)->EnableWindow(bEnable);
// 	CMFCToolBarComboBoxButton::GetByCmd(ID_SCC_SET_STOP)->EnableWindow(bEnable);
// 	CMFCToolBarComboBoxButton::GetByCmd(ID_SCC_SET_RTO)->EnableWindow(bEnable);
	pCmdUI->Enable(!IsCOMOpen());
}

void CMainFrame::OnSCCComboComSelok()
{
	CMFCToolBarComboBoxButton* pSrcCombo = CMFCToolBarComboBoxButton::GetByCmd (ID_SCC_SET_COM, TRUE);
	ASSERT(pSrcCombo);
	m_nCurCOM = pSrcCombo->GetCurSel();
	int nCurSelIndex=m_ascc.GetCOMIndex(m_nCurCOM) ;

	if(nCurSelIndex!=-1)
	{
		SetCOMParam(m_ascc.GetCOM(nCurSelIndex),1);
	}
	else
	{
		CSerialPort *psp=new CSerialPort;
		psp->m_nCOMNo=m_nCurCOM;
		if(CSCCTools::RWReg(psp))		
			SetCOMParam(psp,1);
		else SetCOMParam(NULL,2);
		delete psp;
		psp=NULL;
	}
	ReplaceBtnIcon(ID_SCC_OPEN,IsCOMOpen()?m_nToolBtns+1:2);
}

void CMainFrame::OnSCCCommon( UINT nID )
{
	if(nID==ID_VIEW_WRAP)	GetView()->SetWrap(!GetView()->GetWrapMode());
	else if(nID==ID_VIEW_SCC_PORT)	m_bSPort=!m_bSPort;
	else if(nID==ID_VIEW_SCC_SEND)	m_bSSend=!m_bSSend;
	else if(nID==ID_VIEW_SCC_TIME)	m_bSTime=!m_bSTime;
	else if(nID==ID_SCC_ASCRECV)	m_bAscRecv=!m_bAscRecv;
	else if(nID==ID_SCC_ASCSEND)	m_bAscSend=!m_bAscSend;
	else if(nID==ID_SCC_ADDENTER)	m_bAddEnter=!m_bAddEnter;
	else if(nID==ID_SCC_ADDSTX)		m_bAddStx=!m_bAddStx;
	else if(nID==ID_SCC_ADDETX)		m_bAddEtx=!m_bAddEtx;
	else if(nID==ID_SCC_ADDECHO)	m_bAddEcho=!m_bAddEcho;
	else if(nID==ID_SCC_PAUSE)		m_bPauseDis=!m_bPauseDis;
	else if(nID==ID_SCC_GONEXT)		m_bToNext=!m_bToNext;
	else if(nID==ID_SCC_READONLY)	GetView()->SetReadOnly(!GetView()->GetReadOnly());
	else if(nID==ID_SCC_SENDLINE)	
	{
		if(!SendLine(m_nCurCOM))	AfxMessageBox("报文格式不对！组合键Ctrl+Alt+P分析错误！");
	}
	else if(nID==ID_SCC_SET_OPTION)
	{
		if(IsCOMOpen())	return;
		SetCOMParam(NULL,2);
	}
	else if(nID==ID_SCC_CLEARDATA)
	{
		GetView()->ClearDatas();
		GetView()->SetMF();
	}
	else if(nID==ID_SCC_OPEN)
	{
		CMFCToolBarComboBoxButton* pSrcCombo = CMFCToolBarComboBoxButton::GetByCmd (ID_SCC_SET_COM, FALSE);
		ASSERT(pSrcCombo);
		int nCOMNo = pSrcCombo->GetCurSel();
		if(m_ascc.IsCOMOpen(nCOMNo))
		{
			if(m_ascc.CloseCOM(nCOMNo));
		}
		else
		{
			int nIndex=m_ascc.GetCOMIndex(nCOMNo);
			CSerialPort *psp;
			if(nIndex==-1)	
				psp=new CSerialPort;
			else
				psp=m_ascc.GetCOM(nIndex);
			SetCOMParam(psp,0);
			if(m_ascc.OpenCOM(psp,this,nCOMNo))
			{
				m_nCurCOM=nCOMNo;
			}
			else
			{
				if(psp!=NULL)	
					delete psp;
				psp=NULL;
			}
			if(nIndex!=-1)
				psp=NULL;
		}
		ReplaceBtnIcon(ID_SCC_OPEN,IsCOMOpen(nCOMNo)?m_nToolBtns+1:2);
	}
}

void CMainFrame::OnUpdateSCCCommon( CCmdUI* pCmdUI )
{
	if(pCmdUI->m_nID ==ID_VIEW_WRAP)	pCmdUI->SetCheck(GetView()->GetWrapMode());
	else if(pCmdUI->m_nID ==ID_VIEW_SCC_PORT)	pCmdUI->SetCheck(m_bSPort);
	else if(pCmdUI->m_nID ==ID_VIEW_SCC_SEND)	pCmdUI->SetCheck(m_bSSend);
	else if(pCmdUI->m_nID ==ID_VIEW_SCC_TIME)	pCmdUI->SetCheck(m_bSTime);
	else if(pCmdUI->m_nID ==ID_SCC_ASCRECV)		pCmdUI->SetCheck(m_bAscRecv);
	else if(pCmdUI->m_nID ==ID_SCC_ASCSEND)		pCmdUI->SetCheck(m_bAscSend);
	else if(pCmdUI->m_nID ==ID_SCC_GONEXT)		pCmdUI->SetCheck(m_bToNext);
	else if(pCmdUI->m_nID ==ID_SCC_READONLY)	pCmdUI->SetCheck(GetView()->GetReadOnly());
	else if(pCmdUI->m_nID ==ID_SCC_SENDLINE)	pCmdUI->Enable(IsCOMOpen());
	else if(pCmdUI->m_nID ==ID_SCC_SET_OPTION)	pCmdUI->Enable(!IsCOMOpen());
	else if(pCmdUI->m_nID ==ID_SCC_ADDENTER)
	{
		pCmdUI->Enable(m_bAscSend);
		pCmdUI->SetCheck(m_bAddEnter);
	}
	else if(pCmdUI->m_nID ==ID_SCC_ADDECHO)
	{
		pCmdUI->Enable(m_bAscSend);
		pCmdUI->SetCheck(m_bAddEcho);
	}
	else if(pCmdUI->m_nID ==ID_SCC_ADDSTX)		
	{
		pCmdUI->Enable(m_bAscSend);
		pCmdUI->SetCheck(m_bAddStx);
	}
	else if(pCmdUI->m_nID ==ID_SCC_ADDETX)		
	{
		pCmdUI->Enable(m_bAscSend);
		pCmdUI->SetCheck(m_bAddEtx);
	}
	else if(pCmdUI->m_nID ==ID_SCC_PAUSE)		
	{
		pCmdUI->SetCheck(m_bPauseDis);
		pCmdUI->SetText(m_bPauseDis?"继续报文监视(&P)":"暂停报文监视(&P)");
	}
	else if(pCmdUI->m_nID ==ID_SCC_OPEN)		
	{
		CString str;
		str.Format("COM%d",m_nCurCOM+1);
		if(IsCOMOpen())	str="关闭"+str+"(&O)";
		else	str="打开"+str+"(&O)";
		pCmdUI->SetText(str);
		str.Empty();
	}
}

void CMainFrame::OnSCCCS( UINT nID )
{
	if(nID==ID_CS_ADDSUM)		m_wndSF.m_xrec.CheckSum(4);
	if(nID==ID_CS_ADDSUMWORD)	m_wndSF.m_xrec.CheckSum(5);
	if(nID==ID_CS_MODBUS)		m_wndSF.m_xrec.CheckSum(0);
	if(nID==ID_CS_CRC_CCITT16)	m_wndSF.m_xrec.CheckSum(1);
	if(nID==ID_CS_CRC_CCITT8)	m_wndSF.m_xrec.CheckSum(2);
	if(nID==ID_CS_CRC_ANSI16)	m_wndSF.m_xrec.CheckSum(3);
}

void CMainFrame::OnUpdateSCCCS( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_wndSF.IsVisible());
}

void CMainFrame::OnSCCMRU( UINT nID )
{
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*(theApp.GetFileList()))[nIndex].GetLength() != 0);
	if (!GetView()->LoadFile((*(theApp.GetFileList()))[nIndex]))
		theApp.RemoveFile(nIndex);
}

void CMainFrame::OnSCCView( UINT nID )
{
	int ny=GetSystemMetrics(SM_CYSCREEN);
	int nx=GetSystemMetrics(SM_CXSCREEN);
	if(nID==ID_VIEW_CENTER)	MoveWindow(CRect(nx/4,ny/4,nx*3/4,ny*3/4));
	else if(nID==ID_VIEW_TOPHARF)		MoveWindow(CRect(0,0,nx,ny/2));
	else if(nID==ID_VIEW_BOTTOMHARF)	MoveWindow(CRect(0,ny/2,nx,ny));
	else if(nID==ID_VIEW_LEFTHARF)		MoveWindow(CRect(0,0,nx/2,ny));
	else if(nID==ID_VIEW_RIGHTHARF)		MoveWindow(CRect(nx/2,0,nx,ny));
	else if(nID==ID_ALWAYSONTOP)	
	{
		DWORD dwConfig=AfxGetApp()->GetProfileInt("Settings","Common",1);
		if(GetExStyle()&WS_EX_TOPMOST)
		{
			SetWindowPos(&wndNoTopMost,0, 0, 0, 0,SWP_NOSIZE|SWP_NOMOVE);
			dwConfig&=0xFEFF;
		}
		else
		{
			SetWindowPos(&wndTopMost,0, 0, 0, 0,SWP_NOSIZE|SWP_NOMOVE);
			dwConfig|=0x100;
		}
		AfxGetApp()->WriteProfileInt("Settings","Common",dwConfig);
	}
	else if(nID==ID_ADVANCE_SEND)	
	{
		m_dlgSend.ShowWindow(1);
		m_dlgSend.SetFocus();
	}
	else if(nID==ID_SCC_CYCSEND)
	{
		if(m_unCycTimer==0)	
		{
			m_unCycTimer=SetTimer(101,m_dlgSend.m_unTime,NULL);
			m_unMulti=m_dlgSend.m_unStart;
		}
		else
		{
			KillTimer(m_unCycTimer);
			m_unCycTimer=0;
		}
	}
	else if(nID==ID_SCC_TRIGGER)
	{
		m_bTrigger=!m_bTrigger;
	}
	else if(nID==ID_SCC_TRANSMIT)
	{
		CTransmit dlgTrans;
		if(dlgTrans.DoModal()==IDCANCEL)
		{
			Trasmt.bCOM=dlgTrans.m_bCOM;
			Trasmt.btCOM1=dlgTrans.m_btCOM1;
			Trasmt.btCOM2=dlgTrans.m_btCOM2;
		}
	}
	else if(nID==ID_FILE_AUTOSAVE)
	{
		CDlgAutoSave dlgAS;
		dlgAS.m_bAS=AutoSave.bSave;
		dlgAS.m_bClear=AutoSave.bClear;
		dlgAS.m_nASTime=AutoSave.nTime;
		dlgAS.m_strPath=AutoSave.strPath;
		if(dlgAS.DoModal()==IDOK)
		{
			KillTimer(103);
			AutoSave.strPath=dlgAS.m_strPath;
			AutoSave.bClear=dlgAS.m_bClear;
			AutoSave.bSave=dlgAS.m_bAS;
			AutoSave.nTime=dlgAS.m_nASTime;
			if(AutoSave.bSave||AutoSave.bClear)	
				SetTimer(103,AutoSave.nTime*60*1000,NULL);
		}
	}
	else if(nID==ID_SHOW_FILTER)
	{
		m_dlgFilter.ShowWindow(1);
		m_dlgFilter.SetFocus();
	}
	else if(nID==ID_FILE_OPENS)
	{
		CString strs=GetRWPath(1,"打开发送窗报文");
		if(!strs.IsEmpty())
		{
			m_wndSF.m_xrec.LoadFile(strs);
			strs.Empty();
		}
	}
	else if(nID==ID_FILE_SAVESEND)
	{
		CString strs=GetRWPath(0,"保存发送窗报文","SCC");
		if(!strs.IsEmpty())
		{
			m_wndSF.m_xrec.SaveFile(strs);
			strs.Empty();
		}
	}
}

void CMainFrame::OnUpdateSCCView( CCmdUI* pCmdUI )
{
	if(pCmdUI->m_nID ==ID_ALWAYSONTOP)	pCmdUI->SetCheck(GetExStyle()&WS_EX_TOPMOST);
	else if(pCmdUI->m_nID ==ID_ADVANCE_SEND)	
	{
		//	pCmdUI->SetCheck(m_dlgSend.IsWindowVisible());
		//	pCmdUI->SetText(m_dlgSend.IsWindowVisible()?"隐藏高级发送窗(&A)":"显示高级发送窗(&A)");
	}
	else if(pCmdUI->m_nID ==ID_SCC_CYCSEND)	
	{
		pCmdUI->SetCheck(m_unCycTimer!=0);
	}
	else if(pCmdUI->m_nID ==ID_SCC_TRIGGER)	
	{
		pCmdUI->SetCheck(m_bTrigger);
	}
	else if((pCmdUI->m_nID ==ID_FILE_OPENS)	||(pCmdUI->m_nID ==ID_FILE_SAVESEND))
	{
		pCmdUI->Enable(m_wndSF.IsVisible());
	}
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 101:
		{
			int nLine=-1;
			if(m_dlgSend.m_nCycMode==0)
			{
				if(m_dlgSend.m_nCycSel==1)	nLine=m_dlgSend.m_unLine-1;
			}
			else
			{
				nLine=m_unMulti-1;
				m_unMulti++;		
				if(m_unMulti>m_dlgSend.m_unEnd)	m_unMulti=m_dlgSend.m_unStart;
			}
			if(!SendLine(m_nCurCOM,nLine))
			{
				KillTimer(m_unCycTimer);
				m_unCycTimer=0;
				AfxMessageBox("报文格式不对！组合键Ctrl+Alt+P分析错误！");
			}
		}
		break;
	case 102:
		{
			SendLine(m_btTriggerCOM,m_dlgSend.Trigger[m_btTrigger].unTrigger-1);			
			KillTimer(102);
		}
		break;
	case 103:
		{
			if(!AutoSave.bSave&&!AutoSave.bClear)	KillTimer(103);
			if(AutoSave.bSave)
			{
				CTime ttmp=CTime::GetCurrentTime();
				CString strs;
				strs.Format ("%04d%02d%02d %02d%02d",ttmp.GetYear (),ttmp.GetMonth (),ttmp.GetDay (),
					ttmp.GetHour(),ttmp.GetMinute());
				strs=AutoSave.strPath+strs+".pcf";
				GetView()->SaveFile(strs);
				strs.Empty();
			}
			if(AutoSave.bClear)	SendMessage(WM_COMMAND,ID_SCC_CLEARDATA);
		}
		break;
	}
	CFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::OnFileOpen()
{
	CString strs=GetRWPath(1,"打开串口监视报文");
	if(!strs.IsEmpty())
	{
		if(GetView()->LoadFile(strs))	
			AfxGetApp()->AddToRecentFileList(strs);
		strs.Empty();
	}
}

void CMainFrame::OnFileXsave()
{
	CTime ttmp=CTime::GetCurrentTime();
	CString strs;
	strs.Format ("%04d%02d%02d %02d%02d",ttmp.GetYear (),ttmp.GetMonth (),ttmp.GetDay (),
		ttmp.GetHour(),ttmp.GetMinute());
	strs=GetRWPath(0,"保存串口监视报文",strs);
	if(!strs.IsEmpty())
	{
		if(GetView()->SaveFile(strs))
			AfxGetApp()->AddToRecentFileList(strs);
		strs.Empty();
	}
}

void CMainFrame::OnUpdateFileXsave( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(GetView()->IsModifyed());
}

void CMainFrame::OnViewSfWnd()
{
	ShowPane(&m_wndSF, !m_wndSF.IsPaneVisible(), 500, FALSE);
 	RecalcLayout();
}

void CMainFrame::OnUpdateViewSfWnd( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck(m_wndSF.IsPaneVisible());
}

void CMainFrame::OnSccTool()
{
	ShowPane(&m_wndSCCTool, !m_wndSCCTool.IsPaneVisible(), 0, FALSE);
}

void CMainFrame::OnUpdateSccTool( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck(m_wndSCCTool.IsPaneVisible());
}


void CMainFrame::OnClose()
{
	ReplaceBtnIcon(ID_SCC_OPEN, 2); //恢复默认图标
	CFrameWndEx::OnClose();
}
