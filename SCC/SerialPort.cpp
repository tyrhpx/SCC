#include "stdafx.h"
#include "SerialPort.h"
// Download by http://www.codefans.net
#include <objbase.h>
#include <initguid.h>

#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4,0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif
/*
int g_nBaudrates[]={CBR_600,CBR_1200,CBR_2400,CBR_4800,CBR_9600,CBR_19200,
CBR_75,CBR_110,CBR_134,CBR_150,CBR_300,CBR_1800,CBR_7200,
CBR_12000,CBR_14400,CBR_28800,CBR_33600,CBR_38400,CBR_56000,
CBR_57600,CBR_115200,CBR_128000,CBR_256000};
*/
//==========================
CSerialPort::CSerialPort()
{
	m_hFile = NULL;
	m_pThreadRece=NULL;
	m_pWndReceMsg=NULL;
	m_nCOMNo=0;
	m_bThreadAlive=FALSE;
	m_hEventCloseThread=NULL;
	m_bValid=0;

	m_nRTO=20;
	m_nParityBit=NOPARITY;
	m_nStopBit=ONESTOPBIT;
	m_nDataBit=8;
	m_nFlowCtrl=0;
	m_nBaudRate=9600;
}

CSerialPort::~CSerialPort()
{
	Close();
}

BOOL CSerialPort::Open(CWnd *pWndReceMsg, UINT uiPortNo,BOOL bConfig,BOOL bStart)
{
	//	_ASSERTE(uiPortNo>=0 && uiPortNo<MAX_SCC_NUM);
	_ASSERTE(uiPortNo>=0 && uiPortNo<255);
	_ASSERTE(pWndReceMsg!=NULL);

	if(m_bThreadAlive||m_hFile)			return FALSE;
	char str[100];
	sprintf_s(str,"\\\\.\\COM%d",uiPortNo+1);
	m_hFile = ::CreateFile(str,GENERIC_READ|GENERIC_WRITE,
		0,0,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,0);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		switch (::GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
			strcat_s(str," 为无效端口！");
			AfxMessageBox(str);
			break;
		case ERROR_ACCESS_DENIED:
			strcat_s(str," 已被其他程序打开！");
			AfxMessageBox(str);
			break;
		default:
			strcat_s(str," 打开失败！未知错误！");
			AfxMessageBox(str);
			break;
		}
		m_hFile = NULL;
		return FALSE;
	}
	m_pWndReceMsg=pWndReceMsg;
	m_nCOMNo=uiPortNo;
	m_bValid=1;

	if (m_hEventCloseThread != NULL)
		ResetEvent(m_hEventCloseThread);
	m_hEventCloseThread = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hEventCloseThread == 0)	
	{
		::CloseHandle(m_hFile);
		m_hFile = NULL;
		return FALSE;
	}
	//====
	if(bConfig)		
		if(!Config())	return FALSE;
	if(bStart)		
		if(!StartThread())	return FALSE;

	return TRUE;
}

BOOL CSerialPort::Close()
{
	DWORD dwR=0;
	for(;;)
	{
		if(m_hFile&&m_bThreadAlive)
		{
			dwR=m_pThreadRece->ResumeThread();
			if((dwR>1)&&(dwR!=0xFFFFFFFF))
				continue;
		}
		SetEvent(m_hEventCloseThread);
		if(!m_bThreadAlive)
			break;
	}
	m_pThreadRece=NULL;
	m_pWndReceMsg=NULL;
	//	::CloseHandle(m_hEventCloseThread);
	//	m_hEventCloseThread = 0;
	if (m_hFile == 0)	
		return TRUE;
	::CloseHandle(m_hFile);
	m_hFile = 0;
	return TRUE;
}

UINT CSerialPort::CommThread(LPVOID pParam)
{
	CSerialPort *port = (CSerialPort*)pParam;

	OVERLAPPED olRead;
	::ZeroMemory(&olRead, sizeof(OVERLAPPED));
	olRead.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
	if(olRead.hEvent==NULL)	return (UINT)-1;

	port->m_bThreadAlive = TRUE;
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL  bResult = TRUE;
	DWORD length=0;

	char *RXBuffer;
	RXBuffer=new char[MAX_SCC_RECV_LEN];

	HANDLE hEventArray[2];
	hEventArray[0] = port->m_hEventCloseThread;
	hEventArray[1] = olRead.hEvent;	

	if (port->m_hFile)		// check if the port is opened
		PurgeComm(port->m_hFile, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	BOOL bError=0;
	for(;;)
	{
		bResult = WaitCommEvent(port->m_hFile, &Event, &olRead);
		if(!bResult)
		{
			if(GetLastError()!=ERROR_IO_PENDING)	
				bError=1;
		}
		else
		{
			bResult = ClearCommError(port->m_hFile, &dwError, &comstat);
			if (comstat.cbInQue == 0) 
				continue;
		}
		if(bError)	
			break;

		Event = WaitForMultipleObjects(2, hEventArray, FALSE, INFINITE);
		switch (Event)
		{
		case 0:
			port->m_bThreadAlive = FALSE;
			delete [] RXBuffer;
			RXBuffer=NULL;
			::ZeroMemory(&comstat, sizeof(COMSTAT));
			CloseHandle(hEventArray[0]);
			CloseHandle(hEventArray[1]);

			AfxEndThread(100);
			break;
		case 1:	//读数据
			GetCommMask(port->m_hFile, &CommEvent);				
			if (CommEvent & EV_RXCHAR)	
				port->ReadData(RXBuffer, &olRead);
			break;
		default:
			bError=1;
			break;
		}

		if(bError)	
			break;
	}
	port->m_bThreadAlive = FALSE;
	delete [] RXBuffer;
	RXBuffer=NULL;

	::ZeroMemory(&comstat, sizeof(COMSTAT));
	CloseHandle(hEventArray[0]);
	CloseHandle(hEventArray[1]);

	return 0;
}

BOOL CSerialPort::StartThread()
{
	if (m_hFile == 0)	
		return FALSE;
	if (!(m_pThreadRece = AfxBeginThread(CommThread, this)))	
		return FALSE;	
	return TRUE;
}

DWORD CSerialPort::ReadData(LPVOID cpRXBuffer, OVERLAPPED* olRead)
{
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD dwBytesRead = 0;
	COMSTAT comstat;

	bResult = ClearCommError(m_hFile, &dwError, &comstat);
	if (comstat.cbInQue == 0)	
		return 0;

	bResult = ReadFile(m_hFile,cpRXBuffer,MAX_SCC_RECV_LEN,&dwBytesRead,olRead); 
	if(!bResult)
	{
		if(GetLastError()==ERROR_IO_PENDING)
			GetOverlappedResult(m_hFile,olRead,&dwBytesRead,1);
		else	
			return 0;
	}
	WORD wBytesRead=(WORD)dwBytesRead;
	::PostMessage(m_pWndReceMsg->m_hWnd, WM_SCC_MSG, (WPARAM)cpRXBuffer, MAKELPARAM(m_nCOMNo,wBytesRead));
	::ZeroMemory(&comstat, sizeof(COMSTAT));
	return dwBytesRead;
}

BOOL CSerialPort::Config()
{
	if (m_hFile == 0)		
		return FALSE;
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!::GetCommState(m_hFile,&dcb))	
		return FALSE;
	dcb.BaudRate = (DWORD) m_nBaudRate;
	dcb.ByteSize = (BYTE) m_nDataBit;
	dcb.Parity   = (BYTE) m_nParityBit;
	dcb.StopBits = (BYTE) m_nStopBit;
	dcb.fParity  = (m_nParityBit != NOPARITY);
	if (!::SetCommState(m_hFile,&dcb))	
	{
		char cherror[100];
		sprintf_s(cherror,"COM%d设置错误：\n可能不支持所选波特率，或停止位，或数据位",m_nCOMNo+1);
		AfxMessageBox(cherror);
		return FALSE;
	}
	if (!::SetupComm(m_hFile,MAX_SCC_RECV_LEN,MAX_SCC_SEND_LEN))	
		return FALSE;
	if (!::SetCommMask(m_hFile,EV_RXCHAR))	
		return FALSE;
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))	
		return FALSE;
	cto.ReadIntervalTimeout = m_nRTO;
	cto.ReadTotalTimeoutConstant = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant =0;
	cto.WriteTotalTimeoutMultiplier =0;
	if (!::SetCommTimeouts(m_hFile,&cto))	
		return FALSE;
	::ZeroMemory(&dcb, sizeof(DCB));
	::ZeroMemory(&cto, sizeof(COMMTIMEOUTS));
	return TRUE;
}

DWORD CSerialPort::WriteData(LPVOID lpBuffer, DWORD dwLength)
{
	if(!dwLength)	
		return 0;
	BOOL bResult=TRUE;
	DWORD length=dwLength;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	OVERLAPPED olWrite;

	::ZeroMemory(&olWrite, sizeof(OVERLAPPED));
	olWrite.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
	if(olWrite.hEvent==NULL)	
		return 0;
	ClearCommError(m_hFile,&dwErrorFlags,&ComStat);
	bResult=WriteFile(m_hFile,lpBuffer,length,&length,&olWrite);

	if(!bResult)
	{
		if(GetLastError()==ERROR_IO_PENDING)
			GetOverlappedResult(m_hFile,&olWrite,&length,TRUE);// 等待
		else	
			length=0;
	}
	::ZeroMemory(&ComStat, sizeof(COMSTAT));
	::CloseHandle (olWrite.hEvent);
	return length;
}

//===============================
#ifdef ADDSCCENUM
CSCCEnum::CSCCEnum()
{

}
CSCCEnum::~CSCCEnum()
{

}
void CSCCEnum::EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi)
{
	CString strErr;
	// Create a device information set that will be the container for 
	// the device interfaces.
	GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

	try {
		hDevInfo = SetupDiGetClassDevs( guidDev,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
			);

		if(hDevInfo == INVALID_HANDLE_VALUE) 
		{
			strErr.Format("SetupDiGetClassDevs failed. (err=%lx)",
				GetLastError());
			throw strErr;
		}

		// Enumerate the serial ports
		BOOL bOk = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		for (DWORD ii=0; bOk; ii++) {
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				NULL, guidDev, ii, &ifcData);
			if (bOk) {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk) {
					CString strDevPath(pDetData->DevicePath);
					// Got a path to the device. Try to get some more info.
					TCHAR fname[256];
					TCHAR desc[256];
					BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(PBYTE)fname, sizeof(fname), NULL);
					bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
						(PBYTE)desc, sizeof(desc), NULL);
					BOOL bUsbDevice = FALSE;
					TCHAR locinfo[256];
					if (SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
						(PBYTE)locinfo, sizeof(locinfo), NULL))
					{
						// Just check the first three characters to determine
						// if the port is connected to the USB bus. This isn't
						// an infallible method; it would be better to use the
						// BUS GUID. Currently, Windows doesn't let you query
						// that though (SPDRP_BUSTYPEGUID seems to exist in
						// documentation only).
						bUsbDevice = (strncmp(locinfo, "USB", 3)==0);
					}
					if (bSuccess) {
						// Add an entry to the array
						SSerInfo si;
						si.strDevPath = strDevPath;
						si.strFriendlyName = fname;
						si.strPortDesc = desc;
						si.bUsbDevice = bUsbDevice;
						asi.Add(si);
					}

				}
				else {
					strErr.Format("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)",
						GetLastError());
					throw strErr;
				}
			}
			else {
				DWORD err = GetLastError();
				if (err != ERROR_NO_MORE_ITEMS) {
					strErr.Format("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
					throw strErr;
				}
			}
		}
	}
	catch (CString strCatchErr) {
		strErr = strCatchErr;
	}

	if (pDetData != NULL)
		delete [] (char*)pDetData;
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	if (!strErr.IsEmpty())
		throw strErr;
}

void CSCCEnum::EnumPortsWNt4(CArray<SSerInfo,SSerInfo&> &asi)
{
	// NT4's driver model is totally different, and not that
	// many people use NT4 anymore. Just try all the COM ports
	// between 1 and 16
	SSerInfo si;
	for (int ii=1; ii<=16; ii++) {
		CString strPort;
		strPort.Format("COM%d",ii);
		si.strDevPath = CString("\\\\.\\") + strPort;
		si.strPortName = strPort;
		asi.Add(si);
	}
}

void CSCCEnum::EnumPortsW9x(CArray<SSerInfo,SSerInfo&> &asi)
{
	// Look at all keys in HKLM\Enum, searching for subkeys named
	// *PNP0500 and *PNP0501. Within these subkeys, search for
	// sub-subkeys containing value entries with the name "PORTNAME"
	// Search all subkeys of HKLM\Enum\USBPORTS for PORTNAME entries.

	// First, open HKLM\Enum
	HKEY hkEnum = NULL;
	HKEY hkSubEnum = NULL;
	HKEY hkSubSubEnum = NULL;

	try {
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Enum", 0, KEY_READ,
			&hkEnum) != ERROR_SUCCESS)
			throw CString("Could not read from HKLM\\Enum");

		// Enumerate the subkeys of HKLM\Enum
		char acSubEnum[128];
		DWORD dwSubEnumIndex = 0;
		DWORD dwSize = sizeof(acSubEnum);
		while (RegEnumKeyEx(hkEnum, dwSubEnumIndex++, acSubEnum, &dwSize,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hkSubEnum = NULL;
			if (RegOpenKeyEx(hkEnum, acSubEnum, 0, KEY_READ,
				&hkSubEnum) != ERROR_SUCCESS)
				throw CString("Could not read from HKLM\\Enum\\")+acSubEnum;

			// Enumerate the subkeys of HKLM\Enum\*\, looking for keys
			// named *PNP0500 and *PNP0501 (or anything in USBPORTS)
			BOOL bUsbDevice = (strcmp(acSubEnum,"USBPORTS")==0);
			char acSubSubEnum[128];
			dwSize = sizeof(acSubSubEnum);  // set the buffer size
			DWORD dwSubSubEnumIndex = 0;
			while (RegEnumKeyEx(hkSubEnum, dwSubSubEnumIndex++, acSubSubEnum,
				&dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				BOOL bMatch = (strcmp(acSubSubEnum,"*PNP0500")==0 ||
					strcmp(acSubSubEnum,"*PNP0501")==0 ||
					bUsbDevice);
				if (bMatch) {
					HKEY hkSubSubEnum = NULL;
					if (RegOpenKeyEx(hkSubEnum, acSubSubEnum, 0, KEY_READ,
						&hkSubSubEnum) != ERROR_SUCCESS)
						throw CString("Could not read from HKLM\\Enum\\") + 
						acSubEnum + "\\" + acSubSubEnum;
					SearchPnpKeyW9x(hkSubSubEnum, bUsbDevice, asi);
					RegCloseKey(hkSubSubEnum);
					hkSubSubEnum = NULL;
				}

				dwSize = sizeof(acSubSubEnum);  // restore the buffer size
			}

			RegCloseKey(hkSubEnum);
			hkSubEnum = NULL;
			dwSize = sizeof(acSubEnum); // restore the buffer size
		}
	}
	catch (CString strError) {
		if (hkEnum != NULL)
			RegCloseKey(hkEnum);
		if (hkSubEnum != NULL)
			RegCloseKey(hkSubEnum);
		if (hkSubSubEnum != NULL)
			RegCloseKey(hkSubSubEnum);
		throw strError;
	}

	RegCloseKey(hkEnum);
}

void CSCCEnum::SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,CArray<SSerInfo,SSerInfo&> &asi)
{
	// Enumerate the subkeys of the given PNP key, looking for values with
	// the name "PORTNAME"
	// First, open HKLM\Enum
	HKEY hkSubPnp = NULL;

	try {
		// Enumerate the subkeys of HKLM\Enum\*\PNP050[01]
		char acSubPnp[128];
		DWORD dwSubPnpIndex = 0;
		DWORD dwSize = sizeof(acSubPnp);
		while (RegEnumKeyEx(hkPnp, dwSubPnpIndex++, acSubPnp, &dwSize,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hkSubPnp = NULL;
			if (RegOpenKeyEx(hkPnp, acSubPnp, 0, KEY_READ,
				&hkSubPnp) != ERROR_SUCCESS)
				throw CString("Could not read from HKLM\\Enum\\...\\")
				+ acSubPnp;

			// Look for the PORTNAME value
			char acValue[128];
			dwSize = sizeof(acValue);
			if (RegQueryValueEx(hkSubPnp, "PORTNAME", NULL, NULL, (BYTE*)acValue,
				&dwSize) == ERROR_SUCCESS)
			{
				CString strPortName(acValue);

				// Got the portname value. Look for a friendly name.
				CString strFriendlyName;
				dwSize = sizeof(acValue);
				if (RegQueryValueEx(hkSubPnp, "FRIENDLYNAME", NULL, NULL, (BYTE*)acValue,
					&dwSize) == ERROR_SUCCESS)
					strFriendlyName = acValue;

				// Prepare an entry for the output array.
				SSerInfo si;
				si.strDevPath = CString("\\\\.\\") + strPortName;
				si.strPortName = strPortName;
				si.strFriendlyName = strFriendlyName;
				si.bUsbDevice = bUsbDevice;

				// Overwrite duplicates.
				BOOL bDup = FALSE;
				for (int ii=0; ii<asi.GetSize() && !bDup; ii++)
				{
					if (asi[ii].strPortName == strPortName) {
						bDup = TRUE;
						asi[ii] = si;
					}
				}
				if (!bDup) {
					// Add an entry to the array
					asi.Add(si);
				}
			}

			RegCloseKey(hkSubPnp);
			hkSubPnp = NULL;
			dwSize = sizeof(acSubPnp);  // restore the buffer size
		}
	}
	catch (CString strError) {
		if (hkSubPnp != NULL)
			RegCloseKey(hkSubPnp);
		throw strError;
	}
}
void CSCCEnum::EnumSerialPorts(CArray<SSerInfo,SSerInfo&> &asi, BOOL bIgnoreBusyPorts)
{
	asi.RemoveAll();
	// Use different techniques to enumerate the available serial
	// ports, depending on the OS we're using
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	if (!::GetVersionEx(&vi)) {
		CString str;
		str.Format("Could not get OS version. (err=%lx)",
			GetLastError());
		throw str;
	}
	// Handle windows 9x and NT4 specially
	if (vi.dwMajorVersion < 5) {
		if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
			EnumPortsWNt4(asi);
		else
			EnumPortsW9x(asi);
	}
	else {
		// Win2k and later support a standard API for
		// enumerating hardware devices.
		EnumPortsWdm(asi);
	}

	for (int ii=0; ii<asi.GetSize(); ii++)
	{
		SSerInfo& rsi = asi[ii];    
		if (bIgnoreBusyPorts) {
			// Only display ports that can be opened for read/write
			HANDLE hCom = CreateFile(rsi.strDevPath,
				GENERIC_READ | GENERIC_WRITE,
				0,    /* comm devices must be opened w/exclusive-access */
				NULL, /* no security attrs */
				OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
				0,    /* not overlapped I/O */
				NULL  /* hTemplate must be NULL for comm devices */
				);
			if (hCom == INVALID_HANDLE_VALUE) {
				// It can't be opened; remove it.
				asi.RemoveAt(ii);
				ii--;               
				continue;
			}
			else {
				// It can be opened! Close it and add it to the list
				::CloseHandle(hCom);
			}
		}

		// Come up with a name for the device.
		// If there is no friendly name, use the port name.
		if (rsi.strFriendlyName.IsEmpty())
			rsi.strFriendlyName = rsi.strPortName;

		// If there is no description, try to make one up from
		// the friendly name.
		if (rsi.strPortDesc.IsEmpty()) {
			// If the port name is of the form "ACME Port (COM3)"
			// then strip off the " (COM3)"
			rsi.strPortDesc = rsi.strFriendlyName;
			int startdex = rsi.strPortDesc.Find(" (");
			int enddex = rsi.strPortDesc.Find(")");
			if (startdex > 0 && enddex == 
				(rsi.strPortDesc.GetLength()-1))
				rsi.strPortDesc = rsi.strPortDesc.Left(startdex);
		}
	}
}

CString CSCCEnum::EnumSerialPorts()
{
	CString str;
	CArray<SSerInfo,SSerInfo&> asi;
	EnumSerialPorts(asi,FALSE/*include all*/);
	for (int i=0; i<asi.GetSize(); i++) 
		str+=asi[i].strFriendlyName+"\r\n";
	str="本机所有串口：\r\n"+str;
	asi.RemoveAll();
	return str;
}
#endif
//===============================
CSCCArray::CSCCArray()
{

}
CSCCArray::~CSCCArray()
{
	//	SaveConfig();
}

int CSCCArray::GetCOMIndex(int nCOMNo)
{
	int nExeist=m_asp.GetSize();
	for(int i=0;i<nExeist;i++)
	{
		if(m_asp.GetAt(i)->m_nCOMNo==nCOMNo )	return i;
	}
	return -1;
}

BOOL CSCCArray::IsCOMOpen(int nCOMNo)
{
	int nCurIndex=GetCOMIndex(nCOMNo) ;
	if(nCurIndex==-1)	return 0; //不存在于列表

	return (m_asp.GetAt(nCurIndex))->IsOpen();
}
BOOL CSCCArray::OpenCOM(CSerialPort *pSP, CWnd *pWnd, int nCOMNo)
{
	if(pSP==NULL)	return 0;
	int nCurIndex=GetCOMIndex(nCOMNo) ;
	if(pSP->Open(pWnd,nCOMNo))
	{
		if(nCurIndex==-1)	m_asp.Add(pSP);
		CSCCTools::RWReg(pSP,0);
		return 1;
	}
	if(nCurIndex!=-1)	m_asp.RemoveAt(nCurIndex);
	return 0;
}

BOOL CSCCArray::OpenCOM(CWnd *pWnd, int nCOMNo)
{
	int nCurIndex=GetCOMIndex(nCOMNo) ;
	if(nCurIndex==-1)	return 0; //不存在于列表

	if((m_asp.GetAt(nCurIndex))->Open(pWnd,nCOMNo))
	{
		CSCCTools::RWReg(m_asp.GetAt(nCurIndex),0);
		return 1;
	}
	m_asp.RemoveAt(nCurIndex);
	return 0;
}
BOOL CSCCArray::CloseCOM(int nCOMNo)
{
	int nCurIndex=GetCOMIndex(nCOMNo) ;
	if(nCurIndex==-1)	
		return 0; //不存在于列表
	return (m_asp.GetAt(nCurIndex))->Close();
}

CSCCTools::CSCCTools()
{

}
CSCCTools::~CSCCTools()
{

}
BOOL CSCCTools::RWReg(CSerialPort *pSP,BOOL bRead)
{
	CString str;
	char strname[8];
	sprintf_s(strname,"COM%d",pSP->m_nCOMNo+1);

	if(bRead)
	{
		str = AfxGetApp()->GetProfileString("Settings",strname);
		if(!str.IsEmpty())
		{
			int cRead = _stscanf_s(str , "%i:%i:%i:%i:%i:%i" , 
				&pSP->m_nBaudRate,&pSP->m_nParityBit,
				&pSP->m_nDataBit ,&pSP->m_nStopBit,
				&pSP->m_nFlowCtrl,&pSP->m_nRTO);
			if(cRead==6) return 1;
		}
	}
	else
	{
		if(pSP->m_bValid)	
		{
			str.Format("%i:%i:%i:%i:%i:%i",
				pSP->m_nBaudRate,pSP->m_nParityBit,
				pSP->m_nDataBit ,pSP->m_nStopBit,
				pSP->m_nFlowCtrl,pSP->m_nRTO);
			AfxGetApp()->WriteProfileString("Settings" ,strname, str);	
		}
	}
	str.Empty();
	return 0;
}

void CSCCTools::OptionCOM(CSerialPort *pSP,int nBautRate,int nParityBit,int nDataBit,int nStopBit,BOOL bIndex)
{
	if(!bIndex)
	{
		pSP->m_nBaudRate=nBautRate;
		pSP->m_nParityBit=nParityBit;
		pSP->m_nDataBit=nDataBit;
		pSP->m_nStopBit=nStopBit;
	}
	else
	{
		pSP->m_nBaudRate=GetParams(nBautRate,0);
		pSP->m_nParityBit=GetParams(nParityBit,1);
		pSP->m_nDataBit=GetParams(nDataBit,2);
		pSP->m_nStopBit=GetParams(nStopBit,3);
	}
}
BOOL CSCCTools::SetupFlowCtrl(CSerialPort *pSP)
{
	if (pSP->m_hFile == NULL)	return FALSE;
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!::GetCommState(pSP->m_hFile,&dcb))	return FALSE;
	dcb.fDsrSensitivity = FALSE;
	switch (pSP->m_nFlowCtrl)
	{
	case NO_FC:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case RTS_CTS_FC:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case DTR_CTS_FC:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case RTS_DSR_FC:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case DTR_DSR_FC:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
	case XON_XOFF_FC:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = TRUE;
			dcb.fInX = TRUE;
			dcb.XonChar = ASCII_XON;
			dcb.XoffChar = ASCII_XOFF;
			dcb.XoffLim = 100;
			dcb.XonLim = 100;
			break;
		}
	default:
		{
			ASSERT(FALSE);
			break;
		}
	}

	if (!::SetCommState(pSP->m_hFile,&dcb))	return FALSE;

	return TRUE;
}

int CSCCTools::GetParams(int nIndex, int nParamItem)
{
	if(nParamItem==0)	//波特率
	{
		switch(nIndex) 
		{
		case 0:		return CBR_600;
		case 1:		return CBR_1200;
		case 2:		return CBR_2400;
		case 3:		return CBR_4800;
		case 4:		return CBR_9600;
		case 5:		return CBR_19200;  

		case 6:		return CBR_75; 
		case 7:		return CBR_110;
		case 8:		return CBR_134;
		case 9:		return CBR_150;
		case 10:	return CBR_300;
		case 11:	return CBR_1800; 
		case 12:	return CBR_7200; 
		case 13:	return CBR_12000;
		case 14:	return CBR_14400;
		case 15:	return CBR_28800;
		case 16:	return CBR_33600;
		case 17:	return CBR_38400;
		case 18:	return CBR_56000;
		case 19:	return CBR_57600;
		case 20:	return CBR_115200;
		case 21:	return CBR_128000;
		case 22:	return CBR_256000;

		default:	return CBR_9600;
		}
	}
	else if(nParamItem==1)	//检验位
	{/*NOPARITY,ODDPARITY, EVENPARITY,MARKPARITY,SPACEPARITY*/
		if((nIndex>=1)&&(nIndex<=4))	return nIndex;
		else	return 0;
	}
	else if(nParamItem==2)	//数据位
	{
		if((nIndex>=1)&&(nIndex<=4))	return 8-nIndex;
		else	return 8;
	}
	else if(nParamItem==3)	//停止位
	{/*ONESTOPBIT,ONE5STOPBITS, TWOSTOPBITS*/
		if((nIndex>=1)&&(nIndex<=2))	return nIndex;
		else	return 0;
	}
	return CBR_9600;
}

int CSCCTools::GetParIndex(int nParam, int nParamItem)
{
	if(nParamItem==0)	//波特率
	{
		switch(nParam) 
		{
		case CBR_600	:  	return 0;
		case CBR_1200	:  	return 1;
		case CBR_2400	:  	return 2;
		case CBR_4800	:  	return 3;
		case CBR_9600	:  	return 4;
		case CBR_19200	:  	return 5;

		case CBR_75		:  	return 6;
		case CBR_110	:  	return 7;
		case CBR_134	:   return 8;
		case CBR_150	:   return 9;
		case CBR_300    :	return 10;
		case CBR_1800   :	return 11;
		case CBR_7200   :	return 12;
		case CBR_12000  :	return 13;
		case CBR_14400  :	return 14;
		case CBR_28800  :	return 15;
		case CBR_33600  :	return 16;
		case CBR_38400  :	return 17;
		case CBR_56000  :	return 18;
		case CBR_57600  :	return 19;
		case CBR_115200 :	return 20;
		case CBR_128000 :	return 21;
		case CBR_256000 :	return 22;

		default:	return 4;
		}
	}
	else if(nParamItem==1)	//检验位
	{/*NOPARITY,ODDPARITY, EVENPARITY,MARKPARITY,SPACEPARITY*/
		if((nParam>=ODDPARITY)&&(nParam<=SPACEPARITY))	return nParam;
		else	return 0;
	}
	else if(nParamItem==2)	//数据位
	{
		if((nParam>=4)&&(nParam<=7))	return 8-nParam;
		else	return 0;
	}
	else if(nParamItem==3)	//停止位
	{/*ONESTOPBIT,ONE5STOPBITS, TWOSTOPBITS*/
		if((nParam>=ONE5STOPBITS)&&(nParam<=TWOSTOPBITS))	return nParam;
		else	return 0;
	}
	return 4;
}

CString CSCCTools::GetParStr(int nData, int nItem, BOOL bIndex)
{
	CString str;
	switch(nItem)
	{
	case 0:	//波特率
	case 2:	//数据位
		if(bIndex)	str.Format("%d",GetParams(nData,nItem));
		else		str.Format("%d",nData);
		return str;
	case 1:
		switch(nData)
		{
		case 1:		return "奇校验";
		case 2:		return "偶校验";
		case 3:		return "标志";	
		case 4:		return "空";
		}
		return "无校验";
	case 3:
		switch(nData)
		{
		case ONE5STOPBITS:		return "1.5";
		case TWOSTOPBITS:		return "2";	
		}
		return "1";	
	case 4:
		switch(nData)
		{
		case RTS_CTS_FC:	return "RTS/CTS";
		case XON_XOFF_FC:	return "XON/XOFF";
		case DTR_CTS_FC:	return "DTR/CTS";
		case RTS_DSR_FC:	return "RTS/DSR";	
		case DTR_DSR_FC:	return "DTR/DSR";	
		}
		return "无";	
	}
	return "";
}

DWORD CSerialPort::WriteData(CString str)
{
	int nlength=str.GetLength();
	LPTSTR lpstr=str.GetBuffer(nlength);
	str.ReleaseBuffer();
	return WriteData(lpstr,nlength);
}

CString CSCCTools::HexToStr(BYTE *btData, int nLength)
{
	if((btData==NULL)||(nLength<1))	return "";
	CString strCvt,strOutput;   
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
	return strOutput;
}

BOOL CSCCTools::IsHexFormat(CString str)
{
	int nStrlength=str.GetLength();
	if(nStrlength<2)	return FALSE;
	for(int i=0;i<nStrlength;i+=3)
	{
		if(((i+1)==nStrlength)||((i+3)==nStrlength))	return FALSE;
		TCHAR chr=' ';
		if((i+2)!=nStrlength)	chr=str.GetAt(i+2);
		if(chr!=' ')	return FALSE;
		for(int j=0;j<2;j++)
		{
			chr=str.GetAt(i+j);
			BOOL bHex=(((chr>='0')&&(chr<='9'))||((chr>='A')&&(chr<='F'))||((chr>='a')&&(chr<='f')));
			if(!bHex)	return FALSE;
		}
	}
	return TRUE;
}

int CSCCTools::StrToHex(CString str, BYTE *btData)
{
	int nLength=str.GetLength();
	if(nLength<2)	return 0;
	int i = 0;
	for(;i<nLength;i+=3)_stscanf_s(str.Mid(i,2),"%02X",&btData[i/3]);
	return (i+1)/3;
}

void CSCCTools::HexToStr(BYTE *btData, int nLength, CString &strOutput)
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

BOOL CSCCTools::MakeAscHex(CString str, CString &strNew, BOOL bToAsc)
{
	int nRead=str.GetLength();
	if(nRead==0)	return 0;
	if(bToAsc)
	{		
		if(!IsHexFormat(str))	return 0;
		BYTE btTmp[4096];
		for(int i=0;i<nRead;i+=3)	_stscanf_s(str.Mid(i,2),"%02X",&btTmp[i/3]);
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

CSerialPort* CSCCArray::GetCOM(int nArrayIndex)
{
	if((nArrayIndex<0)||(nArrayIndex>m_asp.GetSize()))	return NULL;
	return m_asp.GetAt(nArrayIndex);
}

void CSCCArray::SaveConfig()
{
	int nCOMNums=m_asp.GetSize();
	for(int i=0;i<nCOMNums;i++)
	{
		if(m_asp.GetAt(i)->IsOpen())	
			m_asp.GetAt(i)->Close();
		CSCCTools::RWReg(m_asp.GetAt(i),0);
	}
	m_asp.RemoveAll();
}

CString CSCCTools::GetCOMName(int nCOMIndex)
{
	CString str;
	str.Format("COM%d",nCOMIndex+1);
	return str;
}

BOOL CSCCTools::IsHexFormat(CString str, CString &strError, int &nErrorPos)
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
