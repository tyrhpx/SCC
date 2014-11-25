#ifndef _DIRECTORYDIALOG_H
#define _DIRECTORYDIALOG_H

enum Flag{SD_DIR=1,SD_DIR_FILE=3};

class  CDirectoryDialog  
{	
public:
	CDirectoryDialog();
	CDirectoryDialog(CString strIniKeyName);
	virtual ~CDirectoryDialog();
	
	//弹出一个对话框,让用户选择文件夹
	CString ShowDirectoryTree(CWnd *pBuddyWnd);//入口参数是相关联的窗口
	CString ShowDirectoryTree(CString strIniDir="");//入口参数是初如值

	//写或读取注册表(或ini)中的值
	void SetDefaultDirectory(CString strDefaultDirectory);
	CString GetDefaultDirectory();
		
	HWND		m_hwndOwner; //父窗口句柄    
    CString		m_strTitle;  //标题
    enum Flag	m_ulFlags;   //SD_DIR,值为1,只显示文件夹;SD_DIR_FILE,值为3显示文件和文件夹  
    	
	//注册表或ini文件的相关信息,记录上次打开的文件夹
	CString m_strIniAppName ;
	CString m_strIniKeyName ;
			
private:
	void init();//初化成员变量
	static CString GetShortDir(CString strDir,long nMaxLength =45);//将长路径名转化成短路径名
	UINT GetStyle(enum Flag flag);//将风格转化成SHBrowseForFolder的风格
	CString GetDefaultTitle();//得到默认标题
	
	//回调函数,由SHBrowseForFolder调用
	static int CALLBACK  BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};

#endif _DIRECTORYDIALOG_H
