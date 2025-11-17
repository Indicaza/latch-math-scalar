// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__BA180C6D_2A2A_4A88_B4A9_146B437BF101__INCLUDED_)
#define AFX_STDAFX_H__BA180C6D_2A2A_4A88_B4A9_146B437BF101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define WINVER 0x0500 

#define _ATL_APARTMENT_THREADED
#define _ATL_USE_DDX_FLOAT

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern ATL::CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>
#include <atlsafe.h>
#include "dcomlib/dcom_utils/plugin_types.h"
#endif // !defined(AFX_STDAFX_H__BA180C6D_2A2A_4A88_B4A9_146B437BF101__INCLUDED)
