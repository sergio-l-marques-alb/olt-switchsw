/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_migrate.h
*
* @purpose   IP Subnet Vlan Migration file
*
* @component vlanIpSubnet
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

#ifndef VLAN_IPSUBNET_MIGRATE_H
#define VLAN_IPSUBNET_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

/* Function Prototypes */

void vlanIpSubnetMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer);


#endif /* VLAN_IPSUBNET_MIGRATE_H */
