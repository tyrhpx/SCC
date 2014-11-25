#ifndef __SERIALPORT_H
#define __SERIALPORT_H

#include "afxwin.h"
#include <afxtempl.h>

#define ADDSCCENUM

#define MAX_SCC_NUM			128		//串口最大数量
#define MAX_SCC_RECV_LEN	4096	//串口最大读缓冲
#define MAX_SCC_SEND_LEN	4096	//串口最大写缓冲
#define WM_SCC_MSG			0x7000	//串口事件
#define DECLARE_SCC_MSG		afx_msg LRESULT OnSCCMsg(WPARAM wParam, LPARAM lParam);
#define SCC_MSG_MAP			ON_MESSAGE(WM_SCC_MSG, OnSCCMsg)
#define BEGIN_SCC_MSG_MAP(theClass)	LRESULT theClass::OnSCCMsg(WPARAM wParam, LPARAM lParam){
#define END_SCC_MSG_MAP()	return 1;}
#define SCC_MSG_MAP_X(theClass,memberFxn)	BEGIN_SCC_MSG_MAP(theClass)memberFxn(wParam,lParam);END_SCC_MSG_MAP()

#define CBR_75		75
#define CBR_134		134
#define CBR_150		150
#define CBR_1800	1800
#define CBR_7200	7200
#define CBR_12000	12000
#define CBR_28800	28800
#define CBR_33600	33600
//=====流控制=====
#define NO_FC		0
#define RTS_CTS_FC	1
#define XON_XOFF_FC	2
#define DTR_CTS_FC	3
#define RTS_DSR_FC	4
#define DTR_DSR_FC	5

#define ASCII_XON	0x11
#define ASCII_XOFF	0x13
//=========================
class CSerialPort : public CObject  
{
public:
	CSerialPort();
	virtual ~CSerialPort();

	BOOL Config();
	DWORD ReadData(LPVOID cpRXBuffer,OVERLAPPED* olRead);
	DWORD WriteData(LPVOID lpBuffer,DWORD dwLength);
	BOOL IsOpen (void) const	{ return (m_hFile != 0); }
	BOOL Close();
	BOOL Open(CWnd* pWndReceMsg,UINT uiPortNo=0,BOOL bConfig=1,BOOL bStart=1);
	BOOL StartThread();
public:	
	DWORD WriteData(CString str);
	HANDLE	m_hFile;			//串口句柄
	BOOL	m_bValid;
	int	m_nCOMNo;			//串口号
	int	m_nBaudRate;
	int	m_nParityBit;
	int	m_nStopBit;
	int	m_nDataBit;
	int	m_nFlowCtrl;
	int	m_nRTO;
	CWnd*				m_pWndReceMsg;		//接收事件的串口句柄
protected:
	static UINT CommThread(LPVOID pParam);
	CWinThread*			m_pThreadRece;		//接收辅助线程
//	HANDLE				m_hFile;			//串口句柄
	HANDLE				m_hEventCloseThread;//关闭串口辅助线程
	BOOL				m_bThreadAlive;		//串口辅助线程活动标志
//	CWnd*				m_pWndReceMsg;		//接收事件的串口句柄
};
//===========================================
#ifdef ADDSCCENUM
#include <Setupapi.h>
#pragma comment( lib, "setupapi" )

class CSCCEnum:public CObject
{
public:
	CSCCEnum();
	virtual ~CSCCEnum();
	typedef struct _SSerInfo 
	{
		//_SSerInfo() : bUsbDevice(FALSE) {}
		CString strDevPath;          // Device path for use with CreateFile()
		CString strPortName;         // Simple name (i.e. COM1)
		CString strFriendlyName;     // Full name to be displayed to a user
		BOOL bUsbDevice;             // Provided through a USB connection?
		CString strPortDesc;         // friendly name without the COMx
	}SSerInfo;
	CString EnumSerialPorts();
	void EnumSerialPorts(CArray<SSerInfo,SSerInfo&> &asi, BOOL bIgnoreBusyPorts=TRUE);
//	CArray<SSerInfo,SSerInfo&> m_asi;
protected:
private:
	void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi);
	void EnumPortsWNt4(CArray<SSerInfo,SSerInfo&> &asi);
	void EnumPortsW9x(CArray<SSerInfo,SSerInfo&> &asi);
	void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,CArray<SSerInfo,SSerInfo&> &asi);	
};
#endif
//===========================================
class CSCCArray:public CObject
{
public:
	void SaveConfig();
	CSerialPort* GetCOM(int nArrayIndex);
	BOOL CloseCOM(int nCOMNo=0);
	BOOL OpenCOM(CSerialPort *pSP,CWnd* pWnd,int nCOMNo=0);
	BOOL OpenCOM(CWnd* pWnd,int nCOMNo=0);
	BOOL IsCOMOpen(int nCOMNo=0);
	int GetCOMIndex(int nCOMNo=0);
	CSCCArray();
	virtual ~CSCCArray();

	CTypedPtrArray<CObArray,CSerialPort*> m_asp;
protected:
private:
};

class CSCCTools:public CObject
{
public:
	static BOOL IsHexFormat(CString str,CString &strError,int &nErrorPos);
	static CString GetCOMName(int nCOMIndex=0);
	static BOOL MakeAscHex(CString str,CString& strNew,BOOL bToAsc=1);
	static void HexToStr(BYTE* btData,int nLength,CString& strOutput);
	static int StrToHex(CString str,BYTE* btData);
	static BOOL IsHexFormat(CString str);
	static CString HexToStr(BYTE* btData,int nLength);
	static CString GetParStr(int nData,int nItem,BOOL bIndex);
	static int GetParIndex(int nParam, int nParamItem);
	static int GetParams(int nIndex,int nParamItem);
	CSCCTools();
	virtual ~CSCCTools();

	static BOOL RWReg(CSerialPort *pSP,BOOL bRead=1);
	static BOOL SetupFlowCtrl(CSerialPort *pSP);
	void OptionCOM(CSerialPort *pSP,int nBautRate,int nParityBit,int nDataBit,int nStopBit,BOOL bIndex=0);
protected:
private:
};
#endif // __SERIALPORT_H
