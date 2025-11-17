

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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



#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_LatchMathLib,0x8742b33b,0x4573,0x44e4,0x8e,0xa2,0xf1,0xb2,0xd1,0x7c,0x6c,0xce);


MIDL_DEFINE_GUID(CLSID, CLSID_Module,0x9da0ce26,0x7baf,0x4a1b,0x81,0x86,0xd7,0x82,0xf6,0x4c,0xdd,0x5a);


MIDL_DEFINE_GUID(CLSID, CLSID_Context,0x2c8515d8,0xe91f,0x49c1,0xbe,0x2d,0x26,0x1c,0x8b,0x93,0x6f,0x1a);


MIDL_DEFINE_GUID(CLSID, CLSID_SetupFrame,0xcd4ac1f9,0x6491,0x479f,0x91,0xe7,0xf4,0x3d,0x5b,0xb0,0x40,0x22);


MIDL_DEFINE_GUID(CLSID, CLSID_SettingsFrame,0x9e9febdb,0x03e1,0x432f,0x8e,0x36,0x7c,0x28,0xe4,0xad,0xa5,0x98);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



