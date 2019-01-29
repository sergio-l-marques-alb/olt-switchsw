/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_migrate.c
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

#include "vlan_mac_include.h"

/* Begin Function Declarations: vlan_mac_migrate.h */

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
void vlanMacMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer)
{

    DUMMY_MIGRATE_FUNCTION (oldVer, ver, sizeof (vlanMacCfg_t));
    vlanMacBuildDefaultConfigData(ver);
    return;
}

/* End Function Declarations */
