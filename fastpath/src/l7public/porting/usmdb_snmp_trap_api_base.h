/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\l7public\porting\usmdb_snmp_trap_base.c
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


#ifndef _USMDB_SNMP_TRAP_BASE_API_H
#define _USMDB_SNMP_TRAP_BASE_API_H

#include "l7_common.h"

/* Begin Function Declarations: usmdb_snmp_trap_api.h */

/*********************************************************************
*
* @purpose  This Notification indicates if Pse Port is delivering or
*           not power to the PD. This Notification SHOULD be sent on
*           every status change except in the searching mode.
*           At least 500 msec must elapse between notifications
*           being emitted by the same object instance.
*
* @param    unitIndex     The Pse group index
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
                                                      L7_uint32 detection);

/*********************************************************************
*
* @purpose  This Notification indicate PSE Threshold usage
*           indication is on, the usage power is above the
*           threshold. At least 500 msec must elapse between
*           notifications being emitted by the same object
*           instance.
*
* @param    unitIndex     The Pse group index
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
                                                          L7_uint32 allocPower);

/*********************************************************************
*
* @purpose  This Notification indicates PSE Threshold usage indication
*           off, the usage power is below the threshold.
*           At least 500 msec must elapse between notifications being
*           emitted by the same object instance.
*
* @param    unitIndex     The Pse group index
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
                                                           L7_uint32 allocPower);

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
                                                        L7_uint32 lldpXMedRemDeviceClass);

/* End Function Declarations */

#endif /* _USMDB_SNMP_TRAP_BASE_API_H */
