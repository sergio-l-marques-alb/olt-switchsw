/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_migrate.c
*
* @purpose   ISCSI Configuration Migration
*
* @component ISCSI
*
* @comments  none
*
* @create    04/18/2008
* @end
*
**********************************************************************/

#include "iscsi_migrate.h"

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
iscsiMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  DUMMY_MIGRATE_FUNCTION (oldVer, ver, sizeof (iscsiCfgData_t));
  iscsiBuildDefaultConfigData(ver);
  return;
}
