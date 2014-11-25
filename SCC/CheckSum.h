#ifndef __CHECKSUM_H
#define __CHECKSUM_H

class CCheckSum  
{
public:
	WORD AddSum(BYTE* btData,int nLength);
	BYTE CCITT_CRC_8(BYTE* btData,int nLength);
	WORD ANSI_CRC_16(BYTE* btData,int nLength);
	WORD CCITT_CRC_16(BYTE* btData,int nLength);
	CCheckSum();
	virtual ~CCheckSum();

};

#endif
