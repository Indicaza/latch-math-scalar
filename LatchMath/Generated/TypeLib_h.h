

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for TypeLib.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __TypeLib_h_h__
#define __TypeLib_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __Module_FWD_DEFINED__
#define __Module_FWD_DEFINED__

#ifdef __cplusplus
typedef class Module Module;
#else
typedef struct Module Module;
#endif /* __cplusplus */

#endif 	/* __Module_FWD_DEFINED__ */


#ifndef __Context_FWD_DEFINED__
#define __Context_FWD_DEFINED__

#ifdef __cplusplus
typedef class Context Context;
#else
typedef struct Context Context;
#endif /* __cplusplus */

#endif 	/* __Context_FWD_DEFINED__ */


#ifndef __SetupFrame_FWD_DEFINED__
#define __SetupFrame_FWD_DEFINED__

#ifdef __cplusplus
typedef class SetupFrame SetupFrame;
#else
typedef struct SetupFrame SetupFrame;
#endif /* __cplusplus */

#endif 	/* __SetupFrame_FWD_DEFINED__ */


#ifndef __SettingsFrame_FWD_DEFINED__
#define __SettingsFrame_FWD_DEFINED__

#ifdef __cplusplus
typedef class SettingsFrame SettingsFrame;
#else
typedef struct SettingsFrame SettingsFrame;
#endif /* __cplusplus */

#endif 	/* __SettingsFrame_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __LatchMathLib_LIBRARY_DEFINED__
#define __LatchMathLib_LIBRARY_DEFINED__

/* library LatchMathLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_LatchMathLib;

EXTERN_C const CLSID CLSID_Module;

#ifdef __cplusplus

class DECLSPEC_UUID("9da0ce26-7baf-4a1b-8186-d782f64cdd5a")
Module;
#endif

EXTERN_C const CLSID CLSID_Context;

#ifdef __cplusplus

class DECLSPEC_UUID("2c8515d8-e91f-49c1-be2d-261c8b936f1a")
Context;
#endif

EXTERN_C const CLSID CLSID_SetupFrame;

#ifdef __cplusplus

class DECLSPEC_UUID("cd4ac1f9-6491-479f-91e7-f43d5bb04022")
SetupFrame;
#endif

EXTERN_C const CLSID CLSID_SettingsFrame;

#ifdef __cplusplus

class DECLSPEC_UUID("9e9febdb-03e1-432f-8e36-7c28e4ada598")
SettingsFrame;
#endif
#endif /* __LatchMathLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


