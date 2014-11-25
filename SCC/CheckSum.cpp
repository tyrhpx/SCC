#include "stdafx.h"
#include "CheckSum.h"
// Download by http://www.codefans.net
#define CHECKVALUE(pt,nl)	if((btData==NULL)||(nLength<=0)){AfxMessageBox("无效数组！");return -1;}

CCheckSum::CCheckSum()
{

}

CCheckSum::~CCheckSum()
{

}
//CCITT V.41,x16 + x12 + x5 + 1
WORD CCheckSum::CCITT_CRC_16(BYTE* btData,int nLength)
{
	CHECKVALUE(btData,nLength);
	WORD CRC=0;
	BYTE j;
	int i;

    for(i=0;i<nLength;i++)
   	{     
       	CRC^=(btData[i]<<8);
		for (j=0;j<8;j++)
		{
			if(CRC&0x8000)CRC=((CRC<<1)^0x1021);
			else          CRC<<=1;
		}
   	}
	return ((CRC>>8)+(CRC<<8));/* 应用时低在先 */
}
//ANSI CRC-16,x16 + x15 + x2 + 1
WORD CCheckSum::ANSI_CRC_16(BYTE* btData,int nLength)
{
	CHECKVALUE(btData,nLength);
	WORD CRC=0xFFFF;
	BYTE j,Tmp=0;
	int i;

    for(i=0;i<nLength;i++)
    {     
		CRC^=btData[i];                      
		for (j=0;j<8;j++)                   	
		{                                   	
			Tmp=CRC&0x0001;             	
			CRC=CRC>>1;                 	
			if(Tmp)CRC=(CRC^0xA001);    	
		}                                   	
    }
	return ((CRC>>8)+(CRC<<8));	/* 应用时高在先 */
}

BYTE CCheckSum::CCITT_CRC_8(BYTE* btData,int nLength)
{
	CHECKVALUE(btData,nLength);
	BYTE CRC=0,j;
	int i;

    for(i=0;i<nLength;i++)                                      	
    {                                                          
       	CRC^=btData[i];                                         
       	for (j=0;j<8;j++)                                  
		{                                                  
			if(CRC&0x80)CRC=((CRC<<1)^0x07);      
			else        CRC<<=1;                       
		}                                                  
	}                                                          
    return ~CRC;                                               	
}


WORD CCheckSum::AddSum(BYTE *btData, int nLength)
{
	CHECKVALUE(btData,nLength);
	WORD wCS=0;
	for(int i=0;i<nLength;i++)	wCS+=btData[i];
	return wCS;  
}
