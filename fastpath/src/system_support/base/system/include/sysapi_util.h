/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename sysapi_util.h
*
* @purpose  Internal header file for sysapi
*
* @component system
*
* @comments none
*
* @create 12/12/2005
*
* @author wjacobs
*
* @end
*             
**********************************************************************/
#ifndef SYSAPI_UTIL_H__
#define SYSAPI_UTIL_H__

                           

/********************************************************************/
/*             DEFINES                                              */
/********************************************************************/

                  
/* sysapi cfg file versions */
#define SYSAPI_CFG_VER_1        1              
#define SYSAPI_CFG_VER_2        2              /* Ver 2 introduces usage of sysapiCfgFileDescr_t */
#define SYSAPI_CFG_VER_CURRENT  SYSAPI_CFG_VER_2


/* sysapi cfg file descriptor parms */
#define SYSAPI_CFG_DESCR_MARKER       0xCFABA       /* Random identifier as marker */      
#define SYSAPI_SUPPORT_CFG_DESCR_MARKER 0xDEEDBACD     /* Random identifier as marker */      

#define SYSAPI_CFG_DESCR_VER_1        1              /* Version 1 was introduced in rel k */
#define SYSAPI_CFG_DESCR_VER_CURRENT  SYSAPI_CFG_DESCR_VER_1


  

/********************************************************************/
/*             DATA STRUCTURES                                      */
/********************************************************************/
typedef struct sysapiCfgFileDescr_s
{
  L7_uint32   marker;               /* Descriptor marker - defined by SYSAPI_CFG_DESCR_MARKER */
  L7_uint32   version;              /* Descriptor version - defined by SYSAPI_CFG_DESCR_VER_* */
  L7_uint32   compressedFileSize;   /* length of compressed file data plus its header */
  L7_uint32   uncompressedLength;   /* length of uncompressed config data   ????   */
} sysapiCfgFileDescr_t;

typedef struct SYSAPI_DIRECTORY_CFG_s
{
    SYSAPI_DIRECTORY_t      fileInfo;
    sysapiCfgFileDescr_t    cfgFileDescr;     /* extra info associated with file */

} SYSAPI_DIRECTORY_CFG_t;


/********************************************************************/
/*             FUNCTION PROTOTYPES                                  */
/********************************************************************/

L7_RC_t sysapiCfgFileIndexGet(const L7_char8 *fileName, L7_uint32 *pFileIndex);
L7_RC_t sysapiCompress(L7_char8 *pUncompressedData, L7_uint32 uncompressedDataSize, 
                       L7_char8 **pCompressedData,  L7_uint32 *compressedBufLen);
L7_RC_t sysapiUncompress(L7_char8 *pCompressedData, L7_uint32 compressedDataSize, 
                         L7_char8 **pUncompressedData, L7_uint32 *uncompressedBufLen);

#endif

