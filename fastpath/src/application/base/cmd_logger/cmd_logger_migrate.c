
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename cmd_logger_migrate.c
*
* @purpose   command logger Configuration Migration
*
* @component cmdLogger component

* @comments  none
*
* @create  11/13/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "cmd_logger_migrate.h"

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes just place holder
*
* @end
*********************************************************************/
void
cmdLoggerMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  DUMMY_MIGRATE_FUNCTION (oldVer, ver, sizeof (cmdLoggerCfgData_t));
  return;
}
