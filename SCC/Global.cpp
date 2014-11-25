#include "Stdafx.h"
#define __GLOBAL_CPP
#include "GLOBAL.H"
#undef __GLOBAL_CPP

CString GetShort(UINT id)
{
	 CString str;
	 str.LoadString(id);
	 int nIndex = str.ReverseFind(_T('\n'));
	 if(nIndex!=-1)
	 {
		str=str.Mid(nIndex+1);
	 }
	 return str;
}
