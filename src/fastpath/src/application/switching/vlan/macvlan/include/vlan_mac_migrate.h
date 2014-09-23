/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_migrate.h
*
* @purpose   MAC Vlan Migration file
*
* @component vlanMac
*
* @comments 
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*             
**********************************************************************/

#ifndef VLAN_MAC_MIGRATE_H
#define VLAN_MAC_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"


/* Function Prototypes */

void vlanMacMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer);


#endif /* VLAN_MAC_MIGRATE_H */
