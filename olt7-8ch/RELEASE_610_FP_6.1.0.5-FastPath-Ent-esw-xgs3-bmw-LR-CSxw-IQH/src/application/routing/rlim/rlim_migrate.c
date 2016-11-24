/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename rlim_migrate.c
*
* @purpose RLIM Configuration Migration
*
* @component RLIM
*
* @comments  none
*
* @create  09/15/2005
*
* @author  Eric Berge
* @end
*
**********************************************************************/

#include "rlim_migrate.h"

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
rlimMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  DUMMY_MIGRATE_FUNCTION (oldVer, ver, sizeof (rlimCfgData_t));
  return;
}
