/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename sntp_cfg.h
*
* @purpose defines data structures and prototypes for SNTP Configuration
*
* @component SNTP (RFC 2030)
*
* @comments none
*
* @create 1/15/2004
*
* @author McLendon
* @end
*
**********************************************************************/

#ifndef __SNTP_CFG_H__
#define __SNTP_CFG_H__

#include "l7_common.h"
#include "sntp_api.h"
#include "sntp_client.h"

#include "l7_cnfgr_api.h"

#define L7_SNTP_CFG_FILENAME    "sntp.cfg"
#define L7_SNTP_CFG_VER_1       0x1
#define L7_SNTP_CFG_VER_CURRENT L7_SNTP_CFG_VER_1

typedef struct sntpCfg_s
{
  L7_fileHdr_t          hdr;
  struct sntpCfgData_s  cfg;
  L7_uint32             checkSum;
}sntpCfg_t;

#define NO_SERVER_SELECTED (-1)

/*********************************************************************
* @purpose  Retrieve a pointer to the config data.
*
* @end
*********************************************************************/
extern struct sntpCfgData_s * sntpCfgDataGet();

/*********************************************************************
* @purpose  Build default sntp config data
*
* @end
*********************************************************************/
extern void sntpBuildConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Update sntp information in the Cfg file
*
* @end
*********************************************************************/
extern L7_RC_t   sntpCfgUpdate();

/*********************************************************************
* @purpose  Initialize the configuration data header.
*
* @end
*********************************************************************/
extern void sntpCfgHeaderDefaultsSet(L7_uint32 ver);

/*********************************************************************
* @purpose  Initialize the configuration data to the default values.
*
* @end
*********************************************************************/
extern void sntpClientDefaultsSet(L7_uint32 ver);

/*********************************************************************
* @purpose  Marks cfg data as changed
*
* @end
*********************************************************************/
extern void sntpCfgMarkDataChanged();

/*********************************************************************
*  Get the checksum for the cfg data
*
*********************************************************************/
extern L7_uint32 sntpCfgChecksumGet();


/* sntp_migrate.c */

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
extern void sntpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);


#endif /* INCLUDE_SNTP_CFG_H*/
