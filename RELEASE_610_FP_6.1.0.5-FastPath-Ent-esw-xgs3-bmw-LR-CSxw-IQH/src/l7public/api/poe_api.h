/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  poe_api.h
*
* @purpose   Externs for power over ethernet.
*
* @component
*
* @comments  none
*
* @create    10/11/2003
*
* @author    bpongracz
*
* @end
**********************************************************************/
#ifndef INCLUDE_POE_API_H
#define INCLUDE_POE_API_H

#include "poe_exports.h"
#include "poe_exports.h"

/*********************************************************************
* @purpose  ReSets the Poe operational state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortReset(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface is capable of supporting poe
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t poeIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface is capable of supporting poe PSE
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t poeIsValidPSEIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface is capable of supporting poe PD
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t poeIsValidPDIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Return Internal Interface Number of next interface
*           capable of supporting poe.
*
* @param    intIfNum  @b{(input)}   Internal Interface Number
* @param    pNextintIfNum @b{(output)}  pointer to Next Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t poeNextValidIntf(L7_uint32 intIfNum, L7_uint32 *pNextIntIfNum);

/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface
*           capable of supporting poe.
*
* @param    pFirstIntIfNum @b{(output)}  pointer to first internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t poeFirstValidIntfNumber(L7_uint32 *pFirstIntIfNum);

/*********************************************************************
* @purpose  Determine if the unit is valid and capable of supporting poe
*
* @param    unit                  @b{(input)} unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t poeIsValidUnit(L7_uint32 unit);

/*********************************************************************
* @purpose  Sets the administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortAdminEnableStateSet(L7_uint32 intIfNum,
                                                 L7_uint32 adminState);

/*********************************************************************
* @purpose  Sets the type string of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    typeString  (@b{  Ptr to string to set})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortTypeStringSet(L7_uint32 intIfNum, L7_char8 *typeString);

/*********************************************************************
* @purpose  Gets the administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortAdminEnableStateGet(L7_uint32 intIfNum,
                                                 L7_uint32 *adminState);

/*********************************************************************
* @purpose  Gets the power pairs ability of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerPairsControlAbilityGet(L7_uint32 intIfNum,
                                                         L7_uint32 *val);

/*********************************************************************
* @purpose Sets Interface power pairs in use.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerPairsSet(L7_uint32 intIfNum,
                                           L7_uint32 val);

/*********************************************************************
* @purpose Gets Interface power pairs in use.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerPairsGet(L7_uint32 intIfNum,
                                           L7_uint32 *val);

/*********************************************************************
* @purpose Gets operational status of the port PD detection.
*
* @param    intIfNum Internal Interface Number
*
* @param    val  pointer to Detection status,
*                       (@b{Returns: POE_DISABLED,
*                                    POE_SEARCHING,
*                                    POE_DELIVERING_POWER,
*                                    POE_FAULT,
*                                    POE_TEST,
*                                    POE_OTHER_FAULT})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortDetectionStatusGet(L7_uint32 intIfNum,
                                                L7_uint32 *val);

/*********************************************************************
* @purpose Sets power management algorithm port priority.
*
* @param    intIfNum Internal Interface Number
*
* @param    val  port priority,
*                       (@b{  POE_CRITICAL,
*                             POE_HIGH,
*                             POE_LOW})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerPrioritySet(L7_uint32 intIfNum,
                                              L7_uint32 val);

/*********************************************************************
* @purpose Gets power management algorithm port priority.
*
* @param    intIfNum Internal Interface Number
*
* @param    val  pointer to port priority,
*                       (@b{Returns: POE_CRITICAL,
*                                    POE_HIGH,
*                                    POE_LOW})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerPriorityGet(L7_uint32 intIfNum,
                                              L7_uint32 *val);

/*********************************************************************
* @purpose Gets the requested power source on a given interface
*
* @param    intIfNum Internal Interface Number
*
* @param    val  power source,
*                       (@b{  L7_POE_PSE_SOURCE_PRIMARY,
*                             L7_POE_PSE_SOURCE_BACKUP,
*                             L7_POE_PD_SOURCE_FROM_PSE,
*                             L7_POE_PD_SOURCE_LOCAL,
*                             L7_POE_PD_SOURCE_LOCAL_AND_PSE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPoePortReqPowerSourceGet(L7_uint32 intIfNum,
                                                  L7_uint32 *val);

/*********************************************************************
* @purpose Gets power management algorithm requested port priority.
*
* @param    intIfNum Internal Interface Number
*
* @param    val  pointer to port priority,
*                       (@b{Returns: L7_POE_PRIORITY_CRITICAL,
*                                    L7_POE_PRIORITY_HIGH,
*                                    L7_POE_PRIORITY_LOW})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPoePortReqPowerPriorityGet(L7_uint32 intIfNum,
                                                  L7_uint32 *val);

/*********************************************************************
* @purpose Gets min watt limit requested on a port.
*
* @param    intIfNum Internal Interface Number
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPoePortMaxReqPowerLimitGet(L7_uint32 intIfNum,
                                                    L7_uint32 *val);

/*********************************************************************
* @purpose Gets power management algorithm power acknowledge.
*
* @param    intIfNum Internal Interface Number
*
* @param    val  pointer to port priority,
*                       (@b{Returns: L7_POE_ACK_IGNORE,
*                                    L7_POE_ACK_YES,
*                                    L7_POE_ACK_NO})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPoePortLocPowerAckGet(L7_uint32 intIfNum,
                                                  L7_uint32 *val);

/*********************************************************************
* @purpose Gets counter of PSE state transitions from POWER_ON
*          to IDLE due to tmpdo_timer_done being asserted.
*
* @param    intIfNum Internal Interface Number
*
* @param    val  pointer to MSP Absent Counter,
*                       (@b{Returns: count of transitions})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortMPSAbsentCounterGet(L7_uint32 intIfNum,
                                                 L7_uint32 *val);

/*********************************************************************
* @purpose Gets Interface port type.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port priority,
*                       (@b{Returns: type string})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortTypeStringGet(L7_uint32 intIfNum,
                                           L7_char8  *val);

/*********************************************************************
* @purpose Gets Interface port terminal classification, eg. IP phones,
*          WLAN access points, ...
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port classification,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerClassificationsGet(L7_uint32 intIfNum,
                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port output power drawn by device.
*
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port output,
*                       (@b{Returns: classification})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poePethPsePortPowerOutputGet(L7_uint32 intIfNum,
                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port output current(milliAmps) drawn by device.
*
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port output,
*                       (@b{Returns: classification})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poePethPsePortCurrentOutputGet(L7_uint32 intIfNum,
                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port output voltage drawn by device.
*
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port output,
*                       (@b{Returns: classification})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poePethPsePortVoltageOutputGet(L7_uint32 intIfNum,
                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port temperature measured at the port.
*
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port temperature
*                       (@b{Returns: temperature})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poePethPsePortTemperatureGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port error code
*
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port error code
*                       (@b{Returns: error code})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poePethPsePortErrorCodeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose Gets counter of PSE state enters SIGNATURE_INVALID.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to SIGNATURE_INVALID counter,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortInvalidSignatureCounterGet(L7_uint32 intIfNum,
                                                        L7_uint32 *val);

/*********************************************************************
* @purpose Gets counter of PSE state enters POWER_DENIED.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to POWER DENIED counter,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortPowerDeniedCounterGet(L7_uint32 intIfNum,
                                                   L7_uint32 *val);

/*********************************************************************
* @purpose Gets counter of PSE state enters ERROR_DELAY_OVER.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to overload counter,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortOverLoadCounterGet(L7_uint32 intIfNum,
                                                L7_uint32 *val);

/*********************************************************************
* @purpose Gets counter of PSE state enters ERROR_DELAY_SHORT.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to short counter,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortShortCounterGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose Gets nominal power of PSE in watts.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to counter,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainPowerGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Gets operational of PSE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainOperStatusGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Gets measured usage power of PSE in watts.
*
* @param    val         pointer to value,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainConsumptionPowerGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Gets usage threshold in percent.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainUsageThresholdGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Sets usage threshold in percent.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainUsageThresholdSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose Gets ability of agent to send notifications.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainNotificationControlEnableGet(L7_uint32 UnitIndex,
                                                          L7_uint32 *val);

/*********************************************************************
* @purpose Sets ability of agent to send notifications.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainNotificationControlEnableSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose Gets min watt limit configurable for a port.
*
* @param    intIfNum Internal Interface Number
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortMinPowerLimitGet(L7_uint32 intIfNum,
                                              L7_uint32 *val);

/*********************************************************************
* @purpose Gets max watt limit configurable for a port.
*
* @param    intIfNum Internal Interface Number
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortMaxPowerLimitGet(L7_uint32 intIfNum,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets the current watt limit the PSE can deliver to a port.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    This is not in the PoE MIB
*
* @end
*
*********************************************************************/
extern L7_RC_t poePethPsePortCurPowerLimitSet(L7_uint32 intIfNum,
                                              L7_uint32 val);

/*********************************************************************
*
* @purpose Gets the current watt limit the PSE can deliver to a port.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    This is not in the PoE MIB
*
* @end
*
*********************************************************************/
extern L7_RC_t poePethPsePortCurPowerLimitGet(L7_uint32 intIfNum,
                                              L7_uint32 *val);

/*********************************************************************
* @purpose  Clear PoE statistics
*
* @param
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t poeStatsClearAll(void);

/*********************************************************************
*
* @purpose Clears port statistics.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
extern L7_RC_t poeStatsClearPort(L7_uint32 intIfNum);

/*********************************************************************
* @purpose Gets power management mode of the PSE unit
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainPowerMgmtModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets power management mode of the PSE unit
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         L7_uint32   power management mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainPowerMgmtModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose Gets port violation type of a PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortViolationTypeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets port violation type of the PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         L7_uint32   violation type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortViolationTypeSet(L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose Gets port high power mode of a PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortHighPowerModeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets port high power mode of a PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         L7_uint32   high power mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortHighPowerModeSet(L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose Gets Logical Port Mapping mode of a PSE Unit
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainLogicalPortMapModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets Logical Port Mapping mode of the PSE unit
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         L7_uint32   auto reset mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainLogicalPortMapModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose Gets Auto Reset mode of a PSE Unit
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainResetAutoModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets Auto Reset  mode of the PSE unit
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         L7_uint32   auto reset mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPseMainResetAutoModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Sets port detection mode of the PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         L7_uint32   detection mode L7_POE_DETECTION_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortDetectionModeSet(L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose Gets port detection  mode of a PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPsePortDetectionModeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose Gets the requested device type on a given interface
*
* @param    intIfNum Internal Interface Number
*
* @param    val  device type,
*                       (@b{  L7_POE_DEVICE_TYPE_PSE,
*                             L7_POE_DEVICE_TYPE_PD})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t poePethPoePortReqPowerDeviceTypeGet(L7_uint32 intIfNum,
                                                  L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets detection mode of a given unit.
*
* @param    UnitIndex   L7_uint32            Unit Number
* @param    val         L7_POE_DETECTION_t * port detection mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if unit does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poePethPseMainDetectionModeGet(L7_uint32           UnitIndex,
                                       L7_POE_DETECTION_t *val);

/*********************************************************************
*
* @purpose Sets detection mode on a given unit.
*
* @param UnitIndex L7_uint32            the unit for this operation
* @param val       L7_POE_DETECTION_t * port detection mode
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if unit does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t poePethPseMainDetectionModeSet(L7_uint32          UnitIndex,
                                       L7_POE_DETECTION_t val);

/*********************************************************************
* @purpose Gets port disconnect mode of a PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         pointer to value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t poePethPsePortDisconnectTypeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets port Disconnect mode of the PSE port
*
* @param    intIfNum    L7_uint32   Internal interface number
* @param    val         L7_uint32   detection mode L7_POE_DETECTION_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t poePethPsePortDisconnectTypeSet(L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose Gets all counters of PSE state machine.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t poePethPortCountersGet(L7_uint32 intIfNum,
                               L7_uint32 *MPSAbsentCounter,
                               L7_uint32 *invalidSignatureCounter,
                               L7_uint32 *powerDeniedCounter,
                               L7_uint32 *overLoadCounter,
                               L7_uint32 *shortCounter);

/*********************************************************************
*
* @purpose Gets the version of POE controller's FW image.
*
* @param    UnitIndex   L7_uint32      Unit Number
* @param    L7_char8    *imageVersion  image version details
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t poeFWImageVersionGet(L7_char8 *imageVersion);

#endif /* INCLUDE_POE_API_H */
