
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename relaymap_migrate.c
*
* @purpose DHCP RELAY Configuration Migration
*
* @component DHCP RELAY
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "relaymap_migrate.h"
#include <string.h>

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
void
dhcpRelayMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  dhcpRelayCfg_t *pCfgCur = (dhcpRelayCfg_t *) pCfgBuffer;

  MIGRATE_ONLY_VERSION (oldVer, ver, sizeof (*pCfgCur));

  switch (oldVer)
  {
  case L7_DHCP_RELAY_CFG_VER_1:
  case L7_DHCP_RELAY_CFG_VER_2:
    break;

  default:
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dhcpRelayBuildDefaultConfigData (L7_DHCP_RELAY_CFG_VER_CURRENT);
    break;
  }                             /* endswitch */

  return;
}
