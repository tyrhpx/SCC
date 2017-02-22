#include "stdafx.h"
#include "DirectoryDialog.h"
// Download by http://www.codefans.net
CDirectoryDialog::CDirectoryDialog()
{
	init();	
}

CDirectoryDialog::CDirectoryDialog(CString strIniKeyName)
{
	init();
	m_strIniKeyName = strIniKeyName ;
}

CDirectoryDialog::~CDirectoryDialog()
{

}

int CALLBACK CDirectoryDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch(uMsg)
	{		
	case BFFM_INITIALIZED://根据lpData选择初始文件夹		
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
		return 1 ;
	case BFFM_SELCHANGED:
		{
			char szDir[1000];
			if (SHGetPathFromIDList((LPITEMIDLIST) lParam ,szDir))
			{//将选择的文件夹显示在静态文本框上,如果太长只显示开头和结尾
				SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)(LPCTSTR)GetShortDir(szDir));
			}			
		}
		return 1 ;
	}
	return 1 ;
}

/////////////////////////////////////////////////////////////////////////
//  函数名称: CDirectoryDialog::ShowDirectoryTree
//  函数说明: 弹出系统对话框让用户选择一个合适的文件夹或文件
//  入口参数: 初始路径
//  返回值  : 用户选择的路径,取消返回空串
/////////////////////////////////////////////////////////////////////////
CString CDirectoryDialog::ShowDirectoryTree(CString strIniDir)
{
	CString strPath;//记录用户选择的文件夹
	char pszTemp[1000] = {""};
	char pszTitle[1000] = {""};//标题
	
	if(NULL == m_hwndOwner)		m_hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	if(m_strTitle.IsEmpty())	m_strTitle = GetDefaultTitle();//为空,得到默认的标题
	BROWSEINFOA bi ={0}; 	
	char pszDisplayName[MAX_PATH] = "\0";//存储用户的选择
	bi.hwndOwner=m_hwndOwner;	
	bi.pszDisplayName = pszDisplayName;	
	bi.ulFlags	 = GetStyle(m_ulFlags);	
	bi.lpszTitle = m_strTitle ;
	bi.lpfn		 = BrowseCallbackProc ;

	if(strIniDir.IsEmpty())
		strcpy_s(pszTemp,(LPCTSTR)(AfxGetApp()->GetProfileString(m_strIniAppName,m_strIniKeyName,"")));
	else	strcpy_s(pszTemp,strIniDir);
	bi.lParam   = (int)pszTemp ;	
	LPITEMIDLIST lpIL = SHBrowseForFolder(&bi);	
	if(lpIL)	SHGetPathFromIDList(lpIL,bi.pszDisplayName);
	else		return "";	
	strPath = bi.pszDisplayName;//如果不是以"\\"结尾,加上"\\"
	if(strPath.Right(1)!='\\')		strPath += "\\";
	SetDefaultDirectory(strPath);//将文件夹存进注册表或ini
	return strPath ;
}

CString CDirectoryDialog::GetDefaultDirectory()
{
	//m_strIniAppName或m_strIniKeyName为空,不存进注册表
	if(m_strIniAppName.IsEmpty() || m_strIniKeyName.IsEmpty())
		return "";
	
	return AfxGetApp()->GetProfileString(m_strIniAppName,m_strIniKeyName,"");
}

void CDirectoryDialog::SetDefaultDirectory(CString strDefaultDirectory)
{
	//m_strIniAppName或m_strIniKeyName为空,不存进注册表
	if(m_strIniAppName.IsEmpty() || m_strIniKeyName.IsEmpty())
	{
	//	ASSERT(false);
		return ;
	}
	
	AfxGetApp()->WriteProfileString(m_strIniAppName,m_strIniKeyName,strDefaultDirectory);
}

void CDirectoryDialog::init()
{		
	m_hwndOwner = NULL;    
    m_strTitle.Empty(); 
    m_ulFlags  = SD_DIR;     
    	
	m_strIniAppName = "文件夹初始路径";
	m_strIniKeyName.Empty();
}

/////////////////////////////////////////////////////////////////////////
//  函数名称: CDirectoryDialog::GetShortDir
//  函数说明: 将长路径转成短路径;短路径=盘符+1到6个"." + 最后一级文件夹或文件
//  入口参数: 长路径名,最长长度
/////////////////////////////////////////////////////////////////////////
CString CDirectoryDialog::GetShortDir(CString strDir,long nMaxLength/*=45*/)
{
	ASSERT(nMaxLength > 5);
	if(strDir.GetLength() < nMaxLength)	return strDir ;//没有超过长,不用处理
	CString strShortDir;//如果有盘符(如c:\)加上盘符
	if(':'==strDir[1] && '\\'==strDir[2])	strShortDir = strDir.Left(3);
	CString strLastDir ;//取最后一级文件夹或文件
	int nPos = strDir.ReverseFind('\\');
	if(nPos != -1)		strLastDir = strDir.Mid(nPos);
	if(strLastDir.GetLength() > nMaxLength - 5 )
	{		  
		strLastDir = strLastDir.Mid(strLastDir.GetLength() - (nMaxLength - 5));
		if('\\' != strLastDir[0])	strLastDir = '\\' + strLastDir ;
	}//中间加上若干...,最多六个
	int nSpace = nMaxLength - strShortDir.GetLength() - strLastDir.GetLength() ;
	if(nSpace > 6)		nSpace = 6;
	for(int i = 0 ; i< nSpace ; i++)		strShortDir +=".";
	strShortDir += strLastDir ;
	strLastDir.Empty();
	return strShortDir ;	
}

/////////////////////////////////////////////////////////////////////////
//  函数名称: CDirectoryDialog::ShowDirectoryTree
//  函数说明: 弹出系统对话框让用户选择一个路径
//  入口参数: 入口参数关联窗口
//  如果父句柄为空,则父句柄为关联控件的父窗口
//	如果不将选择的路径写进ini,此用关联控件的值为初始值
//	将用户选择的值显示在关联控件上,并更新其关联变量
/////////////////////////////////////////////////////////////////////////
CString CDirectoryDialog::ShowDirectoryTree(CWnd *pBuddyWnd)
{
	if(NULL == pBuddyWnd)	return "" ;
	CString strDir ;
	
	if(NULL == m_hwndOwner)//以关联控件的父窗口为父窗口
	{
		if(NULL != pBuddyWnd->GetParent())
			m_hwndOwner = pBuddyWnd->GetParent()->GetSafeHwnd();
	}		
	if(m_strIniKeyName.IsEmpty())	pBuddyWnd->GetWindowText(strDir);//如果没有将上次的路径写进注册表,初始值为关联控件的内容
	strDir = ShowDirectoryTree(strDir);		
	if(!strDir.IsEmpty())
	{//更新关联控件的显示及关联变量的值
		pBuddyWnd->SetWindowText(strDir);
		if(NULL != pBuddyWnd->GetParent())
			pBuddyWnd->GetParent()->UpdateData(true);
	}		
	return strDir ;
}

CString CDirectoryDialog::GetDefaultTitle()
{
	CString strTitle ;
	if(m_strIniKeyName.IsEmpty())		strTitle = "选择保存监视报文的目录:";	
	else
	{
		char pszTitle[255];
		sprintf_s(pszTitle,"请为%s选择合适的目录:",m_strIniKeyName);
		strTitle =  pszTitle ;
	}
	return strTitle ;
}

UINT CDirectoryDialog::GetStyle(enum Flag flag)
{
	switch(flag)
	{
//	case SD_DIR		 :	
//		return BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT ;//只显示文件夹
	case SD_DIR_FILE :	
		return BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES;//显示文件夹和文件
	}
	return BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT ;//只显示文件夹
}
