/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   snmp_util.h
 *
 * @purpose    Simple Network Management Protocol (SNMP) File
 *
 * @component  snmp component
 *
 * @comments   none
 *
 * @create     03/02/2001
 *
 * @author     asuthan
 *
 * @end
 *             
 **********************************************************************/

#ifndef SNMP_UTIL_H
#define SNMP_UTIL_H

#include "snmp_util_api.h"
#include "snmp_exports.h"

#define SNMP_CFG_FILENAME  "snmpCfgData.cfg"
#define SNMP_CFG_VER_1      0x1
#define SNMP_CFG_VER_2        0x2
#define SNMP_CFG_VER_CURRENT  SNMP_CFG_VER_2

void snmpStartTasks (void);
L7_RC_t snmpSave (void);
L7_RC_t snmpRestore (void);
L7_BOOL snmpHasDataChanged (void);
void snmpResetDataChanged(void);
void snmpBuildDefaultConfigData (L7_uint32 ver);
void snmpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * buffer);
void L7_snmp_cfg_task (void);
void snmpInitData (void);
void snmpUsmDbReadyCallback (void);
L7_RC_t snmpPhaseOneInit (void);
L7_RC_t snmpPhaseTwoInit (void);
L7_RC_t snmpPhaseThreeInit (void);
void snmpPhaseOneFini (void);
L7_RC_t snmpPhaseOneUnConfig (void);
L7_RC_t snmpPhaseTwoUnConfig (void);
void L7_snmp_trap_task (void);

#endif /* SNMP_UTIL_API_H */
