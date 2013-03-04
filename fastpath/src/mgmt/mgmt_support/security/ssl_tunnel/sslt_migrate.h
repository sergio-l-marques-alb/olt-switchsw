
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  sslt_migrate.c
*
* @purpose   SSL Tunnel configuration Migration
*
* @component sslt
*
* @comments  none
*
* @create    8/23/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*             
**********************************************************************/

#include "sslt_exports.h"


#ifndef SSLT_MIGRATE_H
#define SSLT_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "sslt_cfg.h"


/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES CHANGED FROM RELEASE 4.3.3 TO RELEASE 4.4.4           */
/*-------------------------------------------------------------------------*/


typedef struct
{
  L7_uint32 ssltAdminMode;
  L7_uint32 ssltSecurePort;
  L7_uint32 ssltUnSecurePort;
  L7_uint32 ssltUnSecureServerAddr;
  L7_uchar8 ssltPemPassPhrase[L7_SSLT_PEM_PASS_PHRASE_MAX];
  L7_uint32 ssltProtocolSSL30;
  L7_uint32 ssltProtocolTLS10;
} ssltCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t    hdr;
  ssltCfgDataV2_t cfg;
  L7_uint32       checkSum;
} ssltCfgV2_t;


/*-------------------------------------------------------------------------*/
/*   TYPEDEFS  OF  VERSIONS                                                */
/*-------------------------------------------------------------------------*/

typedef ssltCfgV2_t ssltCfgVer2_t;
typedef ssltCfg_t   ssltCfgVer3_t;


#endif /* SSLT_MIGRATE_H */
