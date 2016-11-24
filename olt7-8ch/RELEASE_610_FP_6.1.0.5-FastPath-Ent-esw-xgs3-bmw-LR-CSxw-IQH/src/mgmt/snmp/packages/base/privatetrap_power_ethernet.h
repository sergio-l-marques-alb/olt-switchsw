/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: privatetrap_power_ethernet.h
*
* Purpose: Power Ethernet trap functions
*
* Created by: cpverne
*
* Component: SNMP
*
*********************************************************************/

#ifndef _PRIVATETRAP_POWER_ETHERNET_H
#define _PRIVATETRAP_POWER_ETHERNET_H

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
* @param    unitIndex     The PseGroupIndex
* @param    intIfNum      The Pse port
* @param    pethPsePortDetectionStatus      The PD detection status
*                                           of the PSE port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_pethPsePortOnOffNotificationTrapSend(L7_uint32 unitIndex,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32 pethPsePortDetectionStatus);

/*********************************************************************
*
* @purpose  This Notification indicate PSE Threshold usage
*           indication is on, the usage power is above the
*           threshold. At least 500 msec must elapse between
*           notifications being emitted by the same object
*           instance.
*
* @param    unitIndex     The PseGroupIndex
* @param    pethMainPseConsumptionPower     Allocated power by the PSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_pethMainPowerUsageOnNotificationTrapSend(L7_uint32 unitIndex,
                                                      L7_uint32 pethMainPseConsumptionPower);

/*********************************************************************
*
* @purpose  This Notification indicates PSE Threshold usage indication
*           off, the usage power is below the threshold.
*           At least 500 msec must elapse between notifications being
*           emitted by the same object instance.
*
* @param    unitIndex     The PseGroupIndex
* @param    pethMainPseConsumptionPower     Allocated power by the PSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_pethMainPowerUsageOffNotificationTrapSend(L7_uint32 unitIndex, 
                                                       L7_uint32 pethMainPseConsumptionPower);

/* End Function Definitions*/

#endif /* _PRIVATETRAP_POWER_ETHERNET_H */
