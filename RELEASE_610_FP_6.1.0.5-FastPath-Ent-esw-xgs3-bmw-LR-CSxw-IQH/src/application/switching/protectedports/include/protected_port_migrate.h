/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_migrate.h
*
* @purpose   Protected Port Migration file
*
* @component protectedPort
*
* @comments 
*
* @create    6/5/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/
#ifndef PROTECTED_PORT_MIGRATE_H
#define PROTECTED_PORT_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

/* Begin Function Prototypes */

void protectedPortMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver,
                                    L7_char8 * pCfgBuffer);

/* End Function Prototypes */
#endif /* PROTECTED_PORT_MIGRATE_H */
