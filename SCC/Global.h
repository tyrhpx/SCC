#ifndef __GLOBAL_H
#define __GLOBAL_H


#ifdef __GLOBAL_CPP
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN CString GetShort(UINT id);

#undef EXTERN
#endif