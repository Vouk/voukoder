

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 04:14:07 2038
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



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __VoukoderTypeLib_h_h__
#define __VoukoderTypeLib_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVoukoder_FWD_DEFINED__
#define __IVoukoder_FWD_DEFINED__
typedef interface IVoukoder IVoukoder;

#endif 	/* __IVoukoder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_VoukoderTypeLib_0000_0000 */
/* [local] */ 

typedef /* [public][public][public][public][public] */ struct __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0001
    {
    INT num;
    INT den;
    } 	Rational;

typedef /* [v1_enum][uuid] */  DECLSPEC_UUID("BD56E1A7-AB59-418E-BDF9-229F970B86D7") 
enum FieldOrder
    {
        Progressive	= 0,
        Top	= ( Progressive + 1 ) ,
        Bottom	= ( Top + 1 ) 
    } 	FieldOrder;

typedef /* [v1_enum][uuid] */  DECLSPEC_UUID("95D62B35-8A87-49A4-B2F5-A7497D112740") 
enum ChannelLayout
    {
        Mono	= 0,
        Stereo	= ( Mono + 1 ) ,
        FivePointOne	= ( Stereo + 1 ) 
    } 	ChannelLayout;

typedef /* [v1_enum][uuid] */  DECLSPEC_UUID("8A05B5F6-3335-4DFE-AC5C-9F0250C2D995") 
enum ColorRange
    {
        Limited	= 0,
        Full	= ( Limited + 1 ) 
    } 	ColorRange;

typedef /* [public][public][public][v1_enum][uuid] */  DECLSPEC_UUID("3E609B2E-7D5F-4355-8F6B-71AF7E218555") 
enum __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0002
    {
        bt601_PAL	= 0,
        bt601_NTSC	= ( bt601_PAL + 1 ) ,
        bt709	= ( bt601_NTSC + 1 ) ,
        bt2020_CL	= ( bt709 + 1 ) ,
        bt2020_NCL	= ( bt2020_CL + 1 ) 
    } 	ColorSpace;

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0003
    {
    CHAR encoder[ 16 ];
    CHAR options[ 8192 ];
    CHAR filters[ 8192 ];
    CHAR sidedata[ 8192 ];
    CHAR format[ 16 ];
    } 	VKTRACKCONFIG;

typedef /* [public][public][public] */ struct __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0004
    {
    INT version;
    VKTRACKCONFIG video;
    VKTRACKCONFIG audio;
    struct 
        {
        CHAR container[ 16 ];
        BOOL faststart;
        } 	format;
    } 	VKENCODERCONFIG;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0006
    {
    WCHAR filename[ 255 ];
    WCHAR application[ 32 ];
    struct 
        {
        BOOL enabled;
        INT width;
        INT height;
        Rational timebase;
        Rational aspectratio;
        FieldOrder fieldorder;
        ColorRange colorRange;
        ColorSpace colorSpace;
        } 	video;
    struct 
        {
        BOOL enabled;
        INT samplerate;
        ChannelLayout channellayout;
        INT numberChannels;
        } 	audio;
    } 	VKENCODERINFO;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0009
    {
    BYTE **buffer;
    INT samples;
    INT blockSize;
    INT planes;
    INT sampleRate;
    ChannelLayout layout;
    CHAR format[ 16 ];
    } 	VKAUDIOCHUNK;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_VoukoderTypeLib_0000_0000_0010
    {
    BYTE **buffer;
    INT *rowsize;
    INT planes;
    INT width;
    INT height;
    INT pass;
    CHAR format[ 16 ];
    } 	VKVIDEOFRAME;



extern RPC_IF_HANDLE __MIDL_itf_VoukoderTypeLib_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_VoukoderTypeLib_0000_0000_v0_0_s_ifspec;

#ifndef __IVoukoder_INTERFACE_DEFINED__
#define __IVoukoder_INTERFACE_DEFINED__

/* interface IVoukoder */
/* [oleautomation][uuid][object] */ 


EXTERN_C const IID IID_IVoukoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E9661BFA-4B8E-4217-BCD8-24074D75000B")
    IVoukoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ BOOL finalize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConfig( 
            /* [retval][out] */ VKENCODERCONFIG *config) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioChunkSize( 
            /* [retval][out] */ UINT *chunkSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileExtension( 
            /* [retval][out] */ BSTR *extension) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxPasses( 
            /* [retval][out] */ UINT *passes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsAudioActive( 
            /* [retval][out] */ BOOL *isActive) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsAudioWaiting( 
            /* [retval][out] */ BOOL *isWaiting) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsVideoActive( 
            /* [retval][out] */ BOOL *isActive) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Log( 
            /* [in] */ BSTR text) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ VKENCODERINFO info) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendAudioSampleChunk( 
            /* [in] */ VKAUDIOCHUNK chunk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendVideoFrame( 
            /* [in] */ VKVIDEOFRAME frame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowVoukoderDialog( 
            /* [in] */ BOOL video,
            /* [in] */ BOOL audio,
            /* [retval][out] */ BOOL *isOkay) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetConfig( 
            /* [in] */ VKENCODERCONFIG config) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVoukoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVoukoder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVoukoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVoukoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IVoukoder * This,
            /* [in] */ BOOL finalize);
        
        HRESULT ( STDMETHODCALLTYPE *GetConfig )( 
            IVoukoder * This,
            /* [retval][out] */ VKENCODERCONFIG *config);
        
        HRESULT ( STDMETHODCALLTYPE *GetAudioChunkSize )( 
            IVoukoder * This,
            /* [retval][out] */ UINT *chunkSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileExtension )( 
            IVoukoder * This,
            /* [retval][out] */ BSTR *extension);
        
        HRESULT ( STDMETHODCALLTYPE *GetMaxPasses )( 
            IVoukoder * This,
            /* [retval][out] */ UINT *passes);
        
        HRESULT ( STDMETHODCALLTYPE *IsAudioActive )( 
            IVoukoder * This,
            /* [retval][out] */ BOOL *isActive);
        
        HRESULT ( STDMETHODCALLTYPE *IsAudioWaiting )( 
            IVoukoder * This,
            /* [retval][out] */ BOOL *isWaiting);
        
        HRESULT ( STDMETHODCALLTYPE *IsVideoActive )( 
            IVoukoder * This,
            /* [retval][out] */ BOOL *isActive);
        
        HRESULT ( STDMETHODCALLTYPE *Log )( 
            IVoukoder * This,
            /* [in] */ BSTR text);
        
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IVoukoder * This,
            /* [in] */ VKENCODERINFO info);
        
        HRESULT ( STDMETHODCALLTYPE *SendAudioSampleChunk )( 
            IVoukoder * This,
            /* [in] */ VKAUDIOCHUNK chunk);
        
        HRESULT ( STDMETHODCALLTYPE *SendVideoFrame )( 
            IVoukoder * This,
            /* [in] */ VKVIDEOFRAME frame);
        
        HRESULT ( STDMETHODCALLTYPE *ShowVoukoderDialog )( 
            IVoukoder * This,
            /* [in] */ BOOL video,
            /* [in] */ BOOL audio,
            /* [retval][out] */ BOOL *isOkay);
        
        HRESULT ( STDMETHODCALLTYPE *SetConfig )( 
            IVoukoder * This,
            /* [in] */ VKENCODERCONFIG config);
        
        END_INTERFACE
    } IVoukoderVtbl;

    interface IVoukoder
    {
        CONST_VTBL struct IVoukoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVoukoder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVoukoder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVoukoder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVoukoder_Close(This,finalize)	\
    ( (This)->lpVtbl -> Close(This,finalize) ) 

#define IVoukoder_GetConfig(This,config)	\
    ( (This)->lpVtbl -> GetConfig(This,config) ) 

#define IVoukoder_GetAudioChunkSize(This,chunkSize)	\
    ( (This)->lpVtbl -> GetAudioChunkSize(This,chunkSize) ) 

#define IVoukoder_GetFileExtension(This,extension)	\
    ( (This)->lpVtbl -> GetFileExtension(This,extension) ) 

#define IVoukoder_GetMaxPasses(This,passes)	\
    ( (This)->lpVtbl -> GetMaxPasses(This,passes) ) 

#define IVoukoder_IsAudioActive(This,isActive)	\
    ( (This)->lpVtbl -> IsAudioActive(This,isActive) ) 

#define IVoukoder_IsAudioWaiting(This,isWaiting)	\
    ( (This)->lpVtbl -> IsAudioWaiting(This,isWaiting) ) 

#define IVoukoder_IsVideoActive(This,isActive)	\
    ( (This)->lpVtbl -> IsVideoActive(This,isActive) ) 

#define IVoukoder_Log(This,text)	\
    ( (This)->lpVtbl -> Log(This,text) ) 

#define IVoukoder_Open(This,info)	\
    ( (This)->lpVtbl -> Open(This,info) ) 

#define IVoukoder_SendAudioSampleChunk(This,chunk)	\
    ( (This)->lpVtbl -> SendAudioSampleChunk(This,chunk) ) 

#define IVoukoder_SendVideoFrame(This,frame)	\
    ( (This)->lpVtbl -> SendVideoFrame(This,frame) ) 

#define IVoukoder_ShowVoukoderDialog(This,video,audio,isOkay)	\
    ( (This)->lpVtbl -> ShowVoukoderDialog(This,video,audio,isOkay) ) 

#define IVoukoder_SetConfig(This,config)	\
    ( (This)->lpVtbl -> SetConfig(This,config) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVoukoder_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


