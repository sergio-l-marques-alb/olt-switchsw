
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename dos_migrate.h
*
* @purpose Denial of service Configuration Migration
*
* @component DoS
*
* @comments none
*
* @create  08/15/2005
*
* @author  esmiley 
* @end
*
**********************************************************************/

#ifndef DOS_MIGRATE_H
#define DOS_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "dos.h"


typedef struct
{
  L7_fileHdr_t   cfgHdr; 
  L7_uint32      systemSIPDIPMode;
  L7_uint32      systemFirstFragMode;
  L7_uint32      systemMinTCPHdrLength;
  L7_uint32      systemTCPFragMode;
  L7_uint32      systemTCPFlagMode;
  L7_uint32      systemL4PortMode;
  L7_uint32      systemICMPMode;
  L7_uint32      systemMaxICMPSize;
  L7_uint32      checkSum;    /* check sum of config file NOTE: needs to be last entry */
} doSCfgDataV1_t;

typedef doSCfgData_t doSCfgDataV2_t;

#endif /* TELNET_MIGRATE_H */
