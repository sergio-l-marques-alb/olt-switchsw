/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_migrate.c
*
* @purpose   VOIP Configuration Migration
*
* @component VOIP
*
* @comments  none
*
* @create  05/12/2007
*
* @author  aprashant
* @end
*
**********************************************************************/

#include "voip_migrate.h"

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
voipMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  DUMMY_MIGRATE_FUNCTION (oldVer, ver, sizeof (voipCfgData_t));
  voipBuildDefaultConfigData(ver);
  return;
}
