

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for VoukoderTypeLib.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#include "ndr64types.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "VoukoderTypeLib_h.h"

#define TYPE_FORMAT_STRING_SIZE   285                               
#define PROC_FORMAT_STRING_SIZE   457                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _VoukoderTypeLib_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } VoukoderTypeLib_MIDL_TYPE_FORMAT_STRING;

typedef struct _VoukoderTypeLib_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } VoukoderTypeLib_MIDL_PROC_FORMAT_STRING;

typedef struct _VoukoderTypeLib_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } VoukoderTypeLib_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

static const RPC_SYNTAX_IDENTIFIER  _NDR64_RpcTransferSyntax = 
{{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}};



extern const VoukoderTypeLib_MIDL_TYPE_FORMAT_STRING VoukoderTypeLib__MIDL_TypeFormatString;
extern const VoukoderTypeLib_MIDL_PROC_FORMAT_STRING VoukoderTypeLib__MIDL_ProcFormatString;
extern const VoukoderTypeLib_MIDL_EXPR_FORMAT_STRING VoukoderTypeLib__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IVoukoder_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IVoukoder_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const VoukoderTypeLib_MIDL_PROC_FORMAT_STRING VoukoderTypeLib__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Close */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter finalize */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 32 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetConfig */

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x4 ),	/* 4 */
/* 46 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0xc124 ),	/* -16092 */
/* 52 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 54 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter config */

/* 64 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
/* 66 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 68 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 70 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 72 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFileExtension */

/* 76 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 78 */	NdrFcLong( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x5 ),	/* 5 */
/* 84 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 92 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 94 */	NdrFcShort( 0x1 ),	/* 1 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter extension */

/* 102 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 104 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 106 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 110 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMaxPasses */

/* 114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x6 ),	/* 6 */
/* 122 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x24 ),	/* 36 */
/* 128 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 130 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter passes */

/* 140 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 142 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 148 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsAudioActive */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x7 ),	/* 7 */
/* 160 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x24 ),	/* 36 */
/* 166 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 168 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isActive */

/* 178 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 180 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 184 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 186 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsAudioWaiting */

/* 190 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 192 */	NdrFcLong( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 198 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x24 ),	/* 36 */
/* 204 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 206 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isWaiting */

/* 216 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 218 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 222 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 224 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsVideoActive */

/* 228 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 230 */	NdrFcLong( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x9 ),	/* 9 */
/* 236 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x24 ),	/* 36 */
/* 242 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 244 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isActive */

/* 254 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 256 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 260 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 262 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Log */

/* 266 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0xa ),	/* 10 */
/* 274 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x8 ),	/* 8 */
/* 280 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 282 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x1 ),	/* 1 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter text */

/* 292 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 294 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 296 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Return value */

/* 298 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 300 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Open */

/* 304 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 306 */	NdrFcLong( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0xb ),	/* 11 */
/* 312 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x8 ),	/* 8 */
/* 318 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 320 */	0xa,		/* 10 */
			0x81,		/* Ext Flags:  new corr desc, has big byval param */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter info */

/* 330 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 332 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 334 */	NdrFcShort( 0xb2 ),	/* Type Offset=178 */

	/* Return value */

/* 336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 338 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SendAudioSampleChunk */

/* 342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0xc ),	/* 12 */
/* 350 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 356 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 358 */	0xa,		/* 10 */
			0x81,		/* Ext Flags:  new corr desc, has big byval param */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter chunk */

/* 368 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 370 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 372 */	NdrFcShort( 0xd4 ),	/* Type Offset=212 */

	/* Return value */

/* 374 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 376 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SendVideoFrame */

/* 380 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 382 */	NdrFcLong( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0xd ),	/* 13 */
/* 388 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 394 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 396 */	0xa,		/* 10 */
			0x81,		/* Ext Flags:  new corr desc, has big byval param */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter frame */

/* 406 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 408 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 410 */	NdrFcShort( 0xf0 ),	/* Type Offset=240 */

	/* Return value */

/* 412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 414 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetConfig */

/* 418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0xf ),	/* 15 */
/* 426 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 428 */	NdrFcShort( 0xc11c ),	/* -16100 */
/* 430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 432 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 434 */	0xa,		/* 10 */
			0x81,		/* Ext Flags:  new corr desc, has big byval param */
/* 436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter config */

/* 444 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 446 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 448 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 450 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 452 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const VoukoderTypeLib_MIDL_TYPE_FORMAT_STRING VoukoderTypeLib__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x2e ),	/* Offset= 46 (50) */
/*  6 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/*  8 */	NdrFcShort( 0x10 ),	/* 16 */
/* 10 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 12 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 14 */	NdrFcShort( 0x2000 ),	/* 8192 */
/* 16 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 18 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 20 */	NdrFcShort( 0x6010 ),	/* 24592 */
/* 22 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 24 */	NdrFcShort( 0xffee ),	/* Offset= -18 (6) */
/* 26 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 28 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (12) */
/* 30 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 32 */	NdrFcShort( 0xffec ),	/* Offset= -20 (12) */
/* 34 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 36 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (12) */
/* 38 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 40 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 42 */	NdrFcShort( 0x14 ),	/* 20 */
/* 44 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 46 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (6) */
/* 48 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 50 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 52 */	NdrFcShort( 0xc038 ),	/* -16328 */
/* 54 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 56 */	0x0,		/* 0 */
			NdrFcShort( 0xffd9 ),	/* Offset= -39 (18) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 60 */	0x0,		/* 0 */
			NdrFcShort( 0xffd5 ),	/* Offset= -43 (18) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 64 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (40) */
			0x5b,		/* FC_END */
/* 68 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 70 */	NdrFcShort( 0x1c ),	/* Offset= 28 (98) */
/* 72 */	
			0x13, 0x0,	/* FC_OP */
/* 74 */	NdrFcShort( 0xe ),	/* Offset= 14 (88) */
/* 76 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 78 */	NdrFcShort( 0x2 ),	/* 2 */
/* 80 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 82 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 84 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 86 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 88 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 90 */	NdrFcShort( 0x8 ),	/* 8 */
/* 92 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (76) */
/* 94 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 96 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 98 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0xffde ),	/* Offset= -34 (72) */
/* 108 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 110 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 112 */	
			0x12, 0x0,	/* FC_UP */
/* 114 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (88) */
/* 116 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (112) */
/* 126 */	
			0x11, 0x0,	/* FC_RP */
/* 128 */	NdrFcShort( 0x32 ),	/* Offset= 50 (178) */
/* 130 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 132 */	NdrFcShort( 0x1fe ),	/* 510 */
/* 134 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 136 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 138 */	NdrFcShort( 0x40 ),	/* 64 */
/* 140 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 142 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 146 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 148 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 150 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 152 */	NdrFcShort( 0x20 ),	/* 32 */
/* 154 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 156 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 158 */	0x0,		/* 0 */
			NdrFcShort( 0xffef ),	/* Offset= -17 (142) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 162 */	0x0,		/* 0 */
			NdrFcShort( 0xffeb ),	/* Offset= -21 (142) */
			0xe,		/* FC_ENUM32 */
/* 166 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 168 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 170 */	NdrFcShort( 0x10 ),	/* 16 */
/* 172 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 174 */	0xe,		/* FC_ENUM32 */
			0x8,		/* FC_LONG */
/* 176 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 178 */	0xb1,		/* FC_FORCED_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 180 */	NdrFcShort( 0x270 ),	/* 624 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	NdrFcShort( 0x0 ),	/* Offset= 0 (184) */
/* 186 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 188 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (130) */
/* 190 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 192 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (136) */
/* 194 */	0x3e,		/* FC_STRUCTPAD2 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 196 */	0x0,		/* 0 */
			NdrFcShort( 0xffd1 ),	/* Offset= -47 (150) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 200 */	0x0,		/* 0 */
			NdrFcShort( 0xffdf ),	/* Offset= -33 (168) */
			0x5b,		/* FC_END */
/* 204 */	
			0x11, 0x0,	/* FC_RP */
/* 206 */	NdrFcShort( 0x6 ),	/* Offset= 6 (212) */
/* 208 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 210 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 212 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 214 */	NdrFcShort( 0x30 ),	/* 48 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0xe ),	/* Offset= 14 (232) */
/* 220 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 222 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 224 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 226 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 228 */	NdrFcShort( 0xff22 ),	/* Offset= -222 (6) */
/* 230 */	0x40,		/* FC_STRUCTPAD4 */
			0x5b,		/* FC_END */
/* 232 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 234 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (208) */
/* 236 */	
			0x11, 0x0,	/* FC_RP */
/* 238 */	NdrFcShort( 0x2 ),	/* Offset= 2 (240) */
/* 240 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 242 */	NdrFcShort( 0x70 ),	/* 112 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x1e ),	/* Offset= 30 (276) */
/* 248 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 250 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 252 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 254 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 256 */	NdrFcShort( 0xff06 ),	/* Offset= -250 (6) */
/* 258 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 260 */	NdrFcShort( 0xff02 ),	/* Offset= -254 (6) */
/* 262 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 264 */	NdrFcShort( 0xfefe ),	/* Offset= -258 (6) */
/* 266 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 268 */	NdrFcShort( 0xfefa ),	/* Offset= -262 (6) */
/* 270 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 272 */	NdrFcShort( 0xfef6 ),	/* Offset= -266 (6) */
/* 274 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 276 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 278 */	NdrFcShort( 0xffba ),	/* Offset= -70 (208) */
/* 280 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 282 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Standard interface: __MIDL_itf_VoukoderTypeLib_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IVoukoder, ver. 0.0,
   GUID={0x57DAA67D,0xA35F,0x4737,{0x85,0x6A,0xF2,0x6A,0x46,0x0C,0x9C,0x7F}} */

#pragma code_seg(".orpc")
static const unsigned short IVoukoder_FormatStringOffsetTable[] =
    {
    0,
    38,
    76,
    114,
    152,
    190,
    228,
    266,
    304,
    342,
    380,
    (unsigned short) -1,
    418
    };



#endif /* defined(_M_AMD64)*/



/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for VoukoderTypeLib.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)



extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif


#include "ndr64types.h"
#include "pshpack8.h"


typedef 
NDR64_FORMAT_CHAR
__midl_frag59_t;
extern const __midl_frag59_t __midl_frag59;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag58_t;
extern const __midl_frag58_t __midl_frag58;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag57_t;
extern const __midl_frag57_t __midl_frag57;

typedef 
NDR64_FORMAT_CHAR
__midl_frag54_t;
extern const __midl_frag54_t __midl_frag54;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag53_t;
extern const __midl_frag53_t __midl_frag53;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_NO_REPEAT_FORMAT frag1;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag2;
        struct _NDR64_POINTER_FORMAT frag3;
        struct _NDR64_NO_REPEAT_FORMAT frag4;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag5;
        struct _NDR64_POINTER_FORMAT frag6;
        NDR64_FORMAT_CHAR frag7;
    } frag2;
}
__midl_frag52_t;
extern const __midl_frag52_t __midl_frag52;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag51_t;
extern const __midl_frag51_t __midl_frag51;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag50_t;
extern const __midl_frag50_t __midl_frag50;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_NO_REPEAT_FORMAT frag1;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag2;
        struct _NDR64_POINTER_FORMAT frag3;
        NDR64_FORMAT_CHAR frag4;
    } frag2;
}
__midl_frag46_t;
extern const __midl_frag46_t __midl_frag46;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag45_t;
extern const __midl_frag45_t __midl_frag45;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag44_t;
extern const __midl_frag44_t __midl_frag44;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag42_t;
extern const __midl_frag42_t __midl_frag42;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag41_t;
extern const __midl_frag41_t __midl_frag41;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag1;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag2;
        struct _NDR64_MEMPAD_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag5;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag6;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag7;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag8;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag9;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag10;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag11;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag12;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag13;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag14;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag15;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag16;
    } frag2;
}
__midl_frag40_t;
extern const __midl_frag40_t __midl_frag40;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag39_t;
extern const __midl_frag39_t __midl_frag39;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag38_t;
extern const __midl_frag38_t __midl_frag38;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag36_t;
extern const __midl_frag36_t __midl_frag36;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag35_t;
extern const __midl_frag35_t __midl_frag35;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag34_t;
extern const __midl_frag34_t __midl_frag34;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag31_t;
extern const __midl_frag31_t __midl_frag31;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag30_t;
extern const __midl_frag30_t __midl_frag30;

typedef 
NDR64_FORMAT_CHAR
__midl_frag16_t;
extern const __midl_frag16_t __midl_frag16;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag15_t;
extern const __midl_frag15_t __midl_frag15;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag14_t;
extern const __midl_frag14_t __midl_frag14;

typedef 
struct 
{
    struct _NDR64_CONF_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag13_t;
extern const __midl_frag13_t __midl_frag13;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag12_t;
extern const __midl_frag12_t __midl_frag12;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag11_t;
extern const __midl_frag11_t __midl_frag11;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag10_t;
extern const __midl_frag10_t __midl_frag10;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag9_t;
extern const __midl_frag9_t __midl_frag9;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag7_t;
extern const __midl_frag7_t __midl_frag7;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag5_t;
extern const __midl_frag5_t __midl_frag5;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag2_t;
extern const __midl_frag2_t __midl_frag2;

typedef 
NDR64_FORMAT_UINT32
__midl_frag1_t;
extern const __midl_frag1_t __midl_frag1;

static const __midl_frag59_t __midl_frag59 =
0x5    /* FC64_INT32 */;

static const __midl_frag58_t __midl_frag58 =
{ 
/* *VKENCODERCONFIG */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag7
};

static const __midl_frag57_t __midl_frag57 =
{ 
/* SetConfig */
    { 
    /* SetConfig */      /* procedure SetConfig */
        (NDR64_UINT32) 34079043 /* 0x2080143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn, actual guaranteed */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 49392 /* 0xc0f0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* config */      /* parameter config */
        &__midl_frag7,
        { 
        /* config */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag54_t __midl_frag54 =
0x2    /* FC64_INT8 */;

static const __midl_frag53_t __midl_frag53 =
{ 
/* *BYTE */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag54
};

static const __midl_frag52_t __midl_frag52 =
{ 
/* VKVIDEOFRAME */
    { 
    /* VKVIDEOFRAME */
        0x31,    /* FC64_PSTRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* VKVIDEOFRAME */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 112 /* 0x70 */
    },
    { 
    /*  */
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* **BYTE */
            0x21,    /* FC64_UP */
            (NDR64_UINT8) 16 /* 0x10 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag53
        },
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *INT */
            0x21,    /* FC64_UP */
            (NDR64_UINT8) 8 /* 0x8 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag59
        },
        0x93    /* FC64_END */
    }
};

static const __midl_frag51_t __midl_frag51 =
{ 
/* *VKVIDEOFRAME */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag52
};

static const __midl_frag50_t __midl_frag50 =
{ 
/* SendVideoFrame */
    { 
    /* SendVideoFrame */      /* procedure SendVideoFrame */
        (NDR64_UINT32) 34079043 /* 0x2080143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn, actual guaranteed */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 301 /* 0x12d */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* frame */      /* parameter frame */
        &__midl_frag52,
        { 
        /* frame */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag46_t __midl_frag46 =
{ 
/* VKAUDIOCHUNK */
    { 
    /* VKAUDIOCHUNK */
        0x31,    /* FC64_PSTRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* VKAUDIOCHUNK */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 48 /* 0x30 */
    },
    { 
    /*  */
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* **BYTE */
            0x21,    /* FC64_UP */
            (NDR64_UINT8) 16 /* 0x10 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag53
        },
        0x93    /* FC64_END */
    }
};

static const __midl_frag45_t __midl_frag45 =
{ 
/* *VKAUDIOCHUNK */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag46
};

static const __midl_frag44_t __midl_frag44 =
{ 
/* SendAudioSampleChunk */
    { 
    /* SendAudioSampleChunk */      /* procedure SendAudioSampleChunk */
        (NDR64_UINT32) 34079043 /* 0x2080143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn, actual guaranteed */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 149 /* 0x95 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* chunk */      /* parameter chunk */
        &__midl_frag46,
        { 
        /* chunk */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag42_t __midl_frag42 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */
    }
};

static const __midl_frag41_t __midl_frag41 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 510 /* 0x1fe */
    }
};

static const __midl_frag40_t __midl_frag40 =
{ 
/* VKENCODERINFO */
    { 
    /* VKENCODERINFO */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* VKENCODERINFO */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 624 /* 0x270 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag41
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag42
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 2 /* 0x2 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag39_t __midl_frag39 =
{ 
/* *VKENCODERINFO */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag40
};

static const __midl_frag38_t __midl_frag38 =
{ 
/* Open */
    { 
    /* Open */      /* procedure Open */
        (NDR64_UINT32) 34341187 /* 0x20c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, actual guaranteed */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* info */      /* parameter info */
        &__midl_frag40,
        { 
        /* info */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag36_t __midl_frag36 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag13
};

static const __midl_frag35_t __midl_frag35 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag36
};

static const __midl_frag34_t __midl_frag34 =
{ 
/* Log */
    { 
    /* Log */      /* procedure Log */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* text */      /* parameter text */
        &__midl_frag35,
        { 
        /* text */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag31_t __midl_frag31 =
{ 
/* *BOOL */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag59
};

static const __midl_frag30_t __midl_frag30 =
{ 
/* IsVideoActive */
    { 
    /* IsVideoActive */      /* procedure IsVideoActive */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* isActive */      /* parameter isActive */
        &__midl_frag59,
        { 
        /* isActive */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag16_t __midl_frag16 =
0x4    /* FC64_INT16 */;

static const __midl_frag15_t __midl_frag15 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */
    }
};

static const __midl_frag14_t __midl_frag14 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag15
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag16
    }
};

static const __midl_frag13_t __midl_frag13 =
{ 
/* FLAGGED_WORD_BLOB */
    { 
    /* FLAGGED_WORD_BLOB */
        0x32,    /* FC64_CONF_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* FLAGGED_WORD_BLOB */
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag14
    }
};

static const __midl_frag12_t __midl_frag12 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag13
};

static const __midl_frag11_t __midl_frag11 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag12
};

static const __midl_frag10_t __midl_frag10 =
{ 
/* *wireBSTR */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag11
};

static const __midl_frag9_t __midl_frag9 =
{ 
/* GetFileExtension */
    { 
    /* GetFileExtension */      /* procedure GetFileExtension */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* extension */      /* parameter extension */
        &__midl_frag11,
        { 
        /* extension */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag7_t __midl_frag7 =
{ 
/* VKENCODERCONFIG */
    { 
    /* VKENCODERCONFIG */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* VKENCODERCONFIG */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 49208 /* 0xc038 */
    }
};

static const __midl_frag5_t __midl_frag5 =
{ 
/* GetConfig */
    { 
    /* GetConfig */      /* procedure GetConfig */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 49400 /* 0xc0f8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* config */      /* parameter config */
        &__midl_frag7,
        { 
        /* config */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag2_t __midl_frag2 =
{ 
/* Close */
    { 
    /* Close */      /* procedure Close */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* finalize */      /* parameter finalize */
        &__midl_frag59,
        { 
        /* finalize */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag59,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1_t __midl_frag1 =
(NDR64_UINT32) 0 /* 0x0 */;


#include "poppack.h"


static const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize64
            ,BSTR_UserMarshal64
            ,BSTR_UserUnmarshal64
            ,BSTR_UserFree64
            }

        };



/* Standard interface: __MIDL_itf_VoukoderTypeLib_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IVoukoder, ver. 0.0,
   GUID={0x57DAA67D,0xA35F,0x4737,{0x85,0x6A,0xF2,0x6A,0x46,0x0C,0x9C,0x7F}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IVoukoder_Ndr64ProcTable[] =
    {
    &__midl_frag2,
    &__midl_frag5,
    &__midl_frag9,
    &__midl_frag30,
    &__midl_frag30,
    &__midl_frag30,
    &__midl_frag30,
    &__midl_frag34,
    &__midl_frag38,
    &__midl_frag44,
    &__midl_frag50,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag57
    };


static const MIDL_SYNTAX_INFO IVoukoder_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    VoukoderTypeLib__MIDL_ProcFormatString.Format,
    &IVoukoder_FormatStringOffsetTable[-3],
    VoukoderTypeLib__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IVoukoder_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IVoukoder_ProxyInfo =
    {
    &Object_StubDesc,
    VoukoderTypeLib__MIDL_ProcFormatString.Format,
    &IVoukoder_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IVoukoder_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IVoukoder_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    VoukoderTypeLib__MIDL_ProcFormatString.Format,
    (unsigned short *) &IVoukoder_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IVoukoder_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(16) _IVoukoderProxyVtbl = 
{
    &IVoukoder_ProxyInfo,
    &IID_IVoukoder,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IVoukoder::Close */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::GetConfig */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::GetFileExtension */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::GetMaxPasses */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::IsAudioActive */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::IsAudioWaiting */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::IsVideoActive */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::Log */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::Open */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::SendAudioSampleChunk */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::SendVideoFrame */ ,
    0 /* IVoukoder::ShowVoukoderDialog */ ,
    (void *) (INT_PTR) -1 /* IVoukoder::SetConfig */
};

const CInterfaceStubVtbl _IVoukoderStubVtbl =
{
    &IID_IVoukoder,
    &IVoukoder_ServerInfo,
    16,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    VoukoderTypeLib__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x60001, /* Ndr library version */
    0,
    0x801026e, /* MIDL Version 8.1.622 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x2000001, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _VoukoderTypeLib_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IVoukoderProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _VoukoderTypeLib_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IVoukoderStubVtbl,
    0
};

PCInterfaceName const _VoukoderTypeLib_InterfaceNamesList[] = 
{
    "IVoukoder",
    0
};


#define _VoukoderTypeLib_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _VoukoderTypeLib, pIID, n)

int __stdcall _VoukoderTypeLib_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_VoukoderTypeLib_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo VoukoderTypeLib_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _VoukoderTypeLib_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _VoukoderTypeLib_StubVtblList,
    (const PCInterfaceName * ) & _VoukoderTypeLib_InterfaceNamesList,
    0, /* no delegation */
    & _VoukoderTypeLib_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

