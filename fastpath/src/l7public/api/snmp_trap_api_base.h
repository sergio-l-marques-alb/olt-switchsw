/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: snmp_trap_api_base.h
*
* Purpose: API functions to initiate sending of SNMP traps
*
* Created by: cpverne
*
* Component: SNMP
*
*********************************************************************/

#ifndef _SNMP_TRAP_API_BASE_H
#define _SNMP_TRAP_API_BASE_H

#include "l7_common.h"
        
/* Begin Function Definitions */

/*********************************************************************
*
* @purpose  This Notification indicates if Pse Port is delivering or
*           not power to the PD. This Notification SHOULD be sent on
*           every status change except in the searching mode.
*           At least 500 msec must elapse between notifications
*           being emitted by the same object instance.
*
* @param    unitINdex     The pse group index
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
L7_RC_t SnmpPethPsePortOnOffNotificationTrapSend(L7_uint32 unitIndex,
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
* @param    unitINdex     The pse group index
* @param    allocPower    The Power being delivered by the pse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpPethMainPowerUsageOnNotificationTrapSend(L7_uint32 unitIndex,
                                                     L7_uint32 allocPower);

/*********************************************************************
*
* @purpose  This Notification indicates PSE Threshold usage indication
*           off, the usage power is below the threshold.
*           At least 500 msec must elapse between notifications being
*           emitted by the same object instance.
*
* @param    unitINdex     The pse group index
* @param    allocPower    The Power being delivered by the pse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpPethMainPowerUsageOffNotificationTrapSend(L7_uint32 unitIndex,
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
L7_RC_t SnmpLldpXMedTopologyChangeDetectedTrapSend(L7_uint32 lldpRemChassisIdSubtype,
                                                   L7_uchar8 *lldpRemChassisId,
                                                   L7_uint32 lldpRemChassisId_len,
                                                   L7_uint32 lldpXMedRemDeviceClass);

/* End Function Definitions*/
#endif /* _SNMP_TRAP_API_BASE_H */
