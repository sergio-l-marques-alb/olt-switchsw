/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename llpf_exports.h
*
* @purpose  Defines constants and feature definitions that are 
*              shared by Management and the application 
*
* @component 
*
* @comments  None 
*           
*
* @Notes   
*
* @created 09/26/2009
*
* @author vijayanand k(kvijayan)
* @end
*
**********************************************************************/

#ifndef __LLPF_EXPORTS_H_
#define __LLPF_EXPORTS_H_

/*--------------------------------------*/
/*  LLPF Constants                      */
/*--------------------------------------*/

typedef enum
{
  L7_LLPF_BLOCK_TYPE_ISDP = 0,      /* ISDP */
  L7_LLPF_BLOCK_TYPE_VTP,           /* VTP  */
  L7_LLPF_BLOCK_TYPE_DTP,           /* DTP  */
  L7_LLPF_BLOCK_TYPE_UDLD,          /* UDLD */
  L7_LLPF_BLOCK_TYPE_PAGP,          /* PAGP */
  L7_LLPF_BLOCK_TYPE_SSTP,          /* SSTP */
  L7_LLPF_BLOCK_TYPE_ALL,           /* ALL  */
  L7_LLPF_BLOCK_TYPE_LAST           /* Total number of block types*/
} L7_LLPF_BLOCK_TYPE_t;

/******************** conditional Override *****************************/

/* LLPF Component Feature List */
typedef enum
{
  L7_LLPF_BLOCK_ISDP_FEATURE_ID = 0,          /* ISDP */
  L7_LLPF_BLOCK_VTP_FEATURE_ID,               /* VTP  */
  L7_LLPF_BLOCK_DTP_FEATURE_ID,               /* DTP  */
  L7_LLPF_BLOCK_UDLD_FEATURE_ID,              /* UDLD */
  L7_LLPF_BLOCK_PAGP_FEATURE_ID,              /* PAGP */
  L7_LLPF_BLOCK_SSTP_FEATURE_ID,              /* SSTP */
  L7_LLPF_BLOCK_ALL_FEATURE_ID,               /* ALL  */
  L7_LLPF_BLOCK_FEATURE_ID_TOTAL,             /* total number of enum values */
} L7_LLPF_FEATURE_IDS_t;

#if 0
static L7_uchar8  cnfgrLlpfFeaturesSupported[] =
{
  L7_LLPF_BLOCK_ISDP_FEATURE_ID,                 /* ISDP         */
  L7_LLPF_BLOCK_VTP_FEATURE_ID,                      /* VTP          */
  L7_LLPF_BLOCK_DTP_FEATURE_ID,                      /* DTP          */
  L7_LLPF_BLOCK_UDLD_FEATURE_ID,                     /* UDLD         */
  L7_LLPF_BLOCK_PAGP_FEATURE_ID,                     /* PAGP         */
  L7_LLPF_BLOCK_SSTP_FEATURE_ID,                     /* SSTP         */
  L7_LLPF_BLOCK_ALL_FEATURE_ID                       /* ALL          */
};
#endif

#ifdef INCLUDE_LLPF_EXPORTS_OVERRIDES
#include "llpf_exports_overrides.h"
#endif

#endif /* __LLPF_EXPORTS_H_*/
