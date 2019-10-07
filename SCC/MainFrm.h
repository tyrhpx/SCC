
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "SerialPort.h"
#include "SCCView.h"
#include "SendFrameWnd.h"	// Added by ClassView
#include "DlgSend.h"	// Added by ClassView
#include "DlgFilter.h"	// Added by ClassView

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	BOOL m_bSPort,m_bSSend,m_bSTime;
	BOOL m_bPauseDis,m_bFilter;

// 特性
public:
	BOOL m_bAscRecv,m_bAscSend;
	BOOL m_bAddEnter,m_bAddEcho,m_bAddStx,m_bAddEtx;
	BOOL m_bToNext;
	struct  _Transmit{
		BOOL bCOM;
		BYTE btCOM1;
		BYTE btCOM2;
	}Trasmt;
	struct  _AutoSave{
		BOOL bSave;
		BOOL bClear;
		int nTime;
		CString strPath;
	}AutoSave;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();

// 实现
public:
	CSCCView* GetView(){return (CSCCView *)GetActiveView();};
	int IsTriggerLine(CString str);
	UINT m_unCycTimer;
	UINT m_unMulti;
	CDlgSend m_dlgSend;
	BOOL m_bTrigger;
	BOOL SendLine(int nCOMIndex,int nLineIndex=-1);
	int AscSendAdd(CString &str);
	CSerialPort* GetCOM(int nCOMIndex=-1);
	BOOL IsCOMOpen(int nCOMIndex=-1);
	void DisplayStr(CString str,int nCOMIndex,BOOL bRecv=1,BOOL bEnter=1);
	void SetCOMParam(CSerialPort *psp,int nSet);
	CSCCArray m_ascc;
	int m_nCurCOM,m_nToolBtns;
	
	CMFCToolBarComboBoxButton* m_pCbxCOM;
	CMFCToolBarComboBoxButton* m_pCbxBaud;
	CMFCToolBarComboBoxButton* m_pCbxParity;
	CMFCToolBarComboBoxButton* m_pCbxData;
	CMFCToolBarComboBoxButton* m_pCbxStop;
	CMFCToolBarComboBoxButton* m_pCbxRTO;
	CMFCToolBarComboBoxButton* m_pCbxFC;

	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBar       m_wndSCCTool;
	CMFCStatusBar     m_wndStatusBar;
	CSendFrameWnd	  m_wndSF;

// 生成的消息映射函数
protected:
	CString GetRWPath(BOOL bOpen,LPCSTR lpstr,LPCSTR lpstrName=NULL);
	BOOL IsFilterFrame(LPBYTE lpb,int nLength);
	CDlgFilter m_dlgFilter;
	void Transmits(int nCOM,LPBYTE lpbt,int nLength);
	void ReplaceBtnIcon(UINT unID, int nNew);
	void CreateImagelist();

	BYTE m_btTrigger,m_btTriggerCOM;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnFileOpen();
	afx_msg void OnFileXsave();
	afx_msg void OnUpdateFileXsave(CCmdUI* pCmdUI);
	afx_msg void OnViewSfWnd();
	afx_msg void OnUpdateViewSfWnd(CCmdUI* pCmdUI);
	afx_msg void OnSccTool();
	afx_msg void OnUpdateSccTool(CCmdUI* pCmdUI);
	DECLARE_SCC_MSG
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

private:
	void RecvSCCMsg(WPARAM wParam, LPARAM lParam);
	void CreateSCCTool();
	BOOL IsCASDown(int nFlag);
	CMFCToolBarComboBoxButton* GetSCCComboCtrl(UINT nID)
	{
		return CMFCToolBarComboBoxButton::GetByCmd(nID);
	}
	
public:
	afx_msg void OnSccSetButton(UINT nID);
	afx_msg void OnUpdateSccSetButton(CCmdUI* pCmdUI);
	afx_msg void OnSCCComboComSelok();
	afx_msg void OnSCCCommon(UINT nID);
	afx_msg void OnUpdateSCCCommon(CCmdUI* pCmdUI);
	afx_msg void OnSCCCS(UINT nID);
	afx_msg void OnUpdateSCCCS(CCmdUI* pCmdUI);
	afx_msg void OnSCCMRU(UINT nID);
	afx_msg void OnSCCView(UINT nID);
	afx_msg void OnUpdateSCCView(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};


