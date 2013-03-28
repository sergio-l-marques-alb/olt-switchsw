
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename sshd_migrate.h
*
* @purpose sshd Configuration Migration
*
* @component sshd
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef SSHD_MIGRATE_H
#define SSHD_MIGRATE_H

#include "datatypes.h"
#include "comm_structs.h"

#include "sshd_cfg.h"


typedef struct
{
    L7_uint32 sshdAdminMode;
    L7_uint32 sshdProtoLevel;

} sshdCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t    hdr;
  sshdCfgDataV2_t cfg;
  L7_uint32       checkSum;           /* keep this as last 4 bytes */

} sshdCfgV2_t;

typedef sshdCfg_t sshdCfgV3_t;

#endif /* SSHD_MIGRATE_H */
