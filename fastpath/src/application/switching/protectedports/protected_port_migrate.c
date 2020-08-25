/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_migrate.c
*
* @purpose   Protected Port Migration file
*
* @component protectedPort
*
* @comments 
*
* @create    6/6/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/

#include "protected_port_include.h"

/* Begin Function Declaration: protected_port_migrate.h */

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
void protectedPortMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver,
                                    L7_char8 * pCfgBuffer)
{         
    DUMMY_MIGRATE_FUNCTION(oldVer, ver, sizeof(protectedPortCfg_t));
    protectedPortBuildDefaultConfigData(ver);
    return;
}

/* End Function Declarations */
