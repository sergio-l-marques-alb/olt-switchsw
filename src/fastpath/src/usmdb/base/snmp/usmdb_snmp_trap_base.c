/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_snmp_trap_base.c
*
* @purpose Provide interface to SNMP Base Trap API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 11/13/2003
*
* @author cpverne
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "usmdb_snmp_trap_api_base.h"
#include "snmp_trap_api_base.h"

/* Begin Function Declarations: usmdb_snmp_trap_api.h */

/*********************************************************************
*
* @purpose  This Notification indicates if Pse Port is delivering or
*           not power to the PD. This Notification SHOULD be sent on
*           every status change except in the searching mode.
*           At least 500 msec must elapse between notifications
*           being emitted by the same object instance.
*
* @param    unitIndex     PSE Group Index
* @param    intIfNum      The Pse port
* @param    detection     The Pse port detection status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpPethPsePortOnOffNotificationTrapSend(L7_uint32 unitIndex,
                                                      L7_uint32 intIfNum, 
                                                      L7_uint32 detection)
{
  return SnmpPethPsePortOnOffNotificationTrapSend(unitIndex, intIfNum, detection);
}

/*********************************************************************
*
* @purpose  This Notification indicate PSE Threshold usage
*           indication is on, the usage power is above the
*           threshold. At least 500 msec must elapse between
*           notifications being emitted by the same object
*           instance.
*
* @param    unitIndex   PSE Group Index
* @param    allocPower    The Power being delivered by the pse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpPethMainPowerUsageOnNotificationTrapSend(L7_uint32 unitIndex,
                                                          L7_uint32 allocPower)
{
  return SnmpPethMainPowerUsageOnNotificationTrapSend(unitIndex, allocPower);
}

/*********************************************************************
*
* @purpose  This Notification indicates PSE Threshold usage indication
*           off, the usage power is below the threshold.
*           At least 500 msec must elapse between notifications being
*           emitted by the same object instance.
*
* @param    unitIndex   PSE Group Index
* @param    allocPower    The Power being delivered by the pse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpPethMainPowerUsageOffNotificationTrapSend(L7_uint32 unitIndex, 
                                                           L7_uint32 allocPower)
{
  return SnmpPethMainPowerUsageOffNotificationTrapSend(unitIndex, allocPower);
}

/*********************************************************************
*
* @purpose  Send a Cold Start Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpLldpXMedTopologyChangeDetectedTrapSend(L7_uint32 lldpRemChassisIdSubtype,
                                                        L7_uchar8 *lldpRemChassisId,
                                                        L7_uint32 lldpRemChassisId_len,
                                                        L7_uint32 lldpXMedRemDeviceClass)
{
  return SnmpLldpXMedTopologyChangeDetectedTrapSend(lldpRemChassisIdSubtype,
                                                    lldpRemChassisId,
                                                    lldpRemChassisId_len,
                                                    lldpXMedRemDeviceClass);
}

/* End Function Declarations */
