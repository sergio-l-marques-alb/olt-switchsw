/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename usmdb_poe_api.h
*
* @purpose USMDB API's for PoE
*
* @component poe
*
* @create 10/14/2003
*
* @author bpongracz
*
* @end
**********************************************************************/

#ifndef USMDB_POE_API_H
#define USMDB_POE_API_H

#include "poe_exports.h"

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
L7_RC_t usmDbPoeIsValidIntf(L7_uint32 intIfNum);
L7_RC_t usmDbPoeIsValidPSEIntf(L7_uint32 intIfNum);
L7_RC_t usmDbPoeIsValidPDIntf(L7_uint32 intIfNum);
/*********************************************************************
*
* @purpose  ReSets Interface operational status to provide POE.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortReset(L7_uint32 UnitIndex,
                              L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Gets Interface ability to provide POE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    adminState  pointer to Admin State,
*                       (@b{Returns: NIM_DISABLE,
*                                    NIM_ENABLE
*                                    or NIM_DIAG_DISABLE})
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
L7_RC_t usmDbPethPsePortAdminEnableGet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose  Sets Interface ability to provide POE.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortAdminEnableSet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 val);

/*********************************************************************
*
* @purpose  Sets Interface(s) type string.
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum   L7_uint32 the internal interface number
*                   L7_ALL_INTERFACES to apply to all
* @param typeString L7_char8* Ptr to string
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortTypeStringSet(L7_uint32 UnitIndex,
                                      L7_uint32 intIfNum,
                                      L7_char8 *typeString);

/*********************************************************************
*
* @purpose  Gets the power pairs ability of the specified interface.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    adminState  pointer to Admin State,
*                       (@b{Returns: L7_ENABLE,
*                                    L7_DISABLE})
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
L7_RC_t usmDbPethPsePortPowerPairsControlAbilityGet(L7_uint32 UnitIndex,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32   *val);

/*********************************************************************
*
* @purpose Gets Interface power pairs in use.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    adminState  pointer to power pair,
*                       (@b{Returns: signal(1),
*                                    spare(2)})
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
L7_RC_t usmDbPethPsePortPowerPairsGet(L7_uint32 UnitIndex,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets Interface power pairs in use.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortPowerPairsSet(L7_uint32 UnitIndex,
                                      L7_uint32 intIfNum,
                                      L7_uint32 val);

/*********************************************************************
*
* @purpose Gets operational status of the port PD detection.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param    val  pointer to Detection status,
*                       (@b{Returns: POE_DISABLED,
*                                    POE_SEARCHING,
*                                    POE_DELIVERING_POWER,
*                                    POE_FAULT,
*                                    POE_TEST,
*                                    POE_OTHER_FAULT})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortDetectionStatusGet(L7_uint32 UnitIndex,
                                           L7_uint32 intIfNum,
                                           L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port priority value used in managment algorithm.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port priority,
*                       (@b{Returns: POE_CRITICAL,
*                                    POE_HIGH,
*                                    POE_LOW})
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
L7_RC_t usmDbPethPsePortPowerPriorityGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets Interface port priority value used in managment algorithm.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  port priority
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Port Priorities:
* @table{@row{@cell{POE_CRITICAL}}
*        @row{@cell{POE_HIGH}}
*        @row{@cell{POE_LOW}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortPowerPrioritySet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 val);

/*********************************************************************
*
* @purpose Gets counter of PSE state transitions from POWER_ON
*          to IDLE due to tmpdo_timer_done being asserted.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param    val  pointer to MSP Absent Counter,
*                       (@b{Returns: count of transitions})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortMPSAbsentCounterGet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum,
                                            L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port type.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to place string
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
L7_RC_t usmDbPethPsePortTypeStringGet(L7_uint32 UnitIndex,
                                      L7_uint32 intIfNum,
                                      L7_char8  *val);

/*********************************************************************
*
* @purpose Gets Interface port terminal classification, eg. IP phones,
*          WLAN access points, ...
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port classification,
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
L7_RC_t usmDbPethPsePortPowerClassificationsGet(L7_uint32 UnitIndex,
                                                L7_uint32 intIfNum,
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
L7_RC_t usmDbPethPsePortPowerOutputGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port output current(milliAmp) drawn by device.
*
* @param    UnitIndex   L7_uint32   Unit Number
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
L7_RC_t usmDbPethPsePortCurrentOutputGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface port output voltage drawn by device.
*
* @param    UnitIndex   L7_uint32   Unit Number
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
L7_RC_t usmDbPethPsePortVoltageOutputGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets Interface temperature (degree celsius) measured
*          at the poe controller hardware
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port temperature
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
L7_RC_t usmDbPethPsePortTemperatureGet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *val);

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
L7_RC_t usmDbPethPsePortErrorCodeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets counter of PSE state enters SIGNATURE_INVALID.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to inv sig counter,
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
L7_RC_t usmDbPethPsePortInvalidSignatureCounterGet(L7_uint32 UnitIndex,
                                                   L7_uint32 intIfNum,
                                                   L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets counter of PSE state enters POWER_DENIED.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to power denied,
*                       (@b{Returns: power denied counter})
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
L7_RC_t usmDbPethPsePortPowerDeniedCounterGet(L7_uint32 UnitIndex,
                                              L7_uint32 intIfNum,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets counter of PSE state enters ERROR_DELAY_OVER.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to overload,
*                       (@b{Returns: overload counter})
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
L7_RC_t usmDbPethPsePortOverLoadCounterGet(L7_uint32 UnitIndex,
                                           L7_uint32 intIfNum,
                                           L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets counter of PSE state enters ERROR_DELAY_SHORT.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to short,
*                       (@b{Returns: short counter})
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
L7_RC_t usmDbPethPsePortShortCounterGet(L7_uint32 UnitIndex,
                                        L7_uint32 intIfNum,
                                        L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets nominal power of PSE in watts.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to counter,
*                       (@b{Returns:  counter})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMainPowerGet(L7_uint32 UnitIndex,
                                 L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets operational of PSE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns:  on(1),
*                                     off(2),
*                                     faulty(3)})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMainOperStatusGet(L7_uint32 UnitIndex,
                                      L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets measured usage power of PSE in watts.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns:  counter})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMainConsumptionPowerGet(L7_uint32 UnitIndex,
                                            L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets usage threshold in percent.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMainUsageThresholdGet(L7_uint32 UnitIndex,
                                          L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets usage threshold in percent.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param val      L7_uint32  value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainUsageThresholdSet(L7_uint32 UnitIndex,
                                          L7_uint32 val);

/*********************************************************************
*
* @purpose Gets ability of agent to send notifications.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMainNotificationControlEnableGet(L7_uint32 UnitIndex,
                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets ability of agent to send notifications.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param val       L7_uint32  value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainNotificationControlEnableSet(L7_uint32 UnitIndex,
                                                     L7_uint32 val);

/*********************************************************************
*
* @purpose Gets min watt limit configurable for a port.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPsePortMinPowerLimitGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets max watt limit configurable for a port.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPsePortMaxPowerLimitGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
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
L7_RC_t usmDbPethPsePortCurPowerLimitSet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
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
L7_RC_t usmDbPethPsePortCurPowerLimitGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Clears all stats.
*
* @param
*
* @returns  L7_SUCCESS  if success
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethStatsClearAll(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Clears port statistics.
*
* @param    UnitIndex   L7_uint32   Unit Number
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
L7_RC_t usmDbPethStatsClearPort(L7_uint32 UnitIndex,
                                L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Get first interface supporting PoE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
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
L7_RC_t usmDbPethFirstInterfaceGet(L7_uint32 UnitIndex,
                                   L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose Get next interface supporting PoE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
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
L7_RC_t usmDbPethNextValidIntfGet(L7_uint32 UnitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *nextIntIfNum);

/*********************************************************************
*
* @purpose Get first interface supporting PSE PoE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if no interfaces support PoE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseFirstInterfaceGet(L7_uint32 UnitIndex,
                                   L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose Get next interface supporting PSE PoE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
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
L7_RC_t usmDbPethNextValidPseIntfGet(L7_uint32 UnitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *nextIntIfNum);

/*********************************************************************
*
* @purpose Get first interface supporting PD PoE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if no interfaces support PoE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPdFirstInterfaceGet(L7_uint32 UnitIndex,
                                   L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose Get next interface supporting PD PoE.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
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
L7_RC_t usmDbPethPdNextValidIntfGet(L7_uint32 UnitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *nextIntIfNum);

/*********************************************************************
*
* @purpose Check to see if PoE is supported on this platform.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
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
L7_RC_t usmDbIsPoeSupported(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Obtain this box's minimum power supported.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    min_power   L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMinPowerGet(L7_uint32 UnitIndex, L7_uint32 *min_power);

/*********************************************************************
*
* @purpose Obtain this box's maximum power supported.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    max_power   L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMaxPowerGet(L7_uint32 UnitIndex, L7_uint32 *max_power);

/*********************************************************************
*
* @purpose Gets Power Management mode of the POE system
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    val         pointer to value,
*                       (@b{Returns: value})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPethPseMainPowerMgmtModeGet(L7_uint32 UnitIndex,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets Power Management mode of the POE system
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param val      L7_uint32  value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainPowerMgmtModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Gets violation type of a given interface.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to port priority,
*                       (@b{Returns: POE_VIOLATION_NONE,
*                                    POE_VIOLATION_CLASS_BASED,
*                                    POE_VIOLATION_USER_DEFINED})
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
L7_RC_t usmDbPethPsePortViolationTypeGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets violation type on a given interface
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  port violation type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Port Violation Type:
* @table{@row{@cell{POE_VIOLATION_NONE}}
*        @row{@cell{POE_VIOLATION_CLASS_BASED}}
*        @row{@cell{POE_VIOLATION_USER_DEFINED}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortViolationTypeSet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 val);

/*********************************************************************
*
* @purpose Gets Interface ability to provide POE high power.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    adminState  pointer to highpower mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
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
L7_RC_t usmDbPethPsePortHighPowerModeGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose  Sets Interface ability to provide POE.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortHighPowerModeSet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 val);

/*********************************************************************
*
* @purpose Gets Powerup mode of a given interface.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to powerup mode,
*                       (@b{Returns: POE_POWERUP_MODE_NORMAL,
*                                    POE_POWERUP_MODE_HIGHPOWER})
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
L7_RC_t usmDbPethPsePortPowerupModeGet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets Powerup mode on a given interface
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  port powerup mode
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Port Violation Type:
* @table{@row{@cell{POE_POWERUP_MODE_NORMAL}}
*        @row{@cell{POE_POWERUP_MODE_HIGHPOWER}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortPowerupModeSet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 val);

/*********************************************************************
*
* @purpose  When enabled resets PoE port incase of any error automatically.
*
* @param UnitIndex L7_uint32 the unit for this operation L7_ALL_UINTS
*                  to apply to all units
* @param mode      L7_ENABLE to enable the mode
*                  L7_DISABLE to disable the mode
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainResetAutoSet(L7_uint32 UnitIndex,
                                     L7_uint32 mode);

/*********************************************************************
*
* @purpose  When enabled resets PoE port incase of any error automatically.
*
* @param UnitIndex L7_uint32 the unit for this operation L7_ALL_UINTS
*                  to apply to all units
* @param mode      L7_ENABLE to enable the mode
*                  L7_DISABLE to disable the mode
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainResetAutoGet(L7_uint32 UnitIndex,
                                     L7_uint32 *mode);

/*********************************************************************
*
* @purpose Gets port detection mode of a given interface.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number
* @param    val         pointer to  port detection mode
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
L7_RC_t usmDbPethPsePortDetectionModeGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets port detection  mode on a given interface
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  port detection mode
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortDetectionModeSet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 val);

/*********************************************************************
* @purpose Sets the unit's POE admin mode.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    admin        L7_uint32 admin
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbPethPseMainAdminModeSet(L7_uint32 UnitIndex, L7_uint32 admin);

/*********************************************************************
* @purpose Gets the unit's POE admin mode.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    admin        L7_uint32 *admin
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbPethPseMainAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *admin);

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
L7_RC_t usmDbPoeFWImageVersionGet(L7_uint32 UnitIndex, L7_char8 *imageVersion);

/*********************************************************************
* @purpose Gets all counters of PSE state machine .
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
L7_RC_t usmDbPethPsePortCountersGet(L7_uint32 intIfNum,
                                    L7_uint32 *MPSAbsentCounter,
                                    L7_uint32 *invalidSignatureCounter,
                                    L7_uint32 *powerDeniedCounter,
                                    L7_uint32 *overLoadCounter,
                                    L7_uint32 *shortCounter);

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
L7_RC_t usmDbPethPseMainDetectionModeGet(L7_uint32           UnitIndex,
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
L7_RC_t usmDbPethPseMainDetectionModeSet(L7_uint32          UnitIndex,
                                         L7_POE_DETECTION_t val);

/*********************************************************************
*
* @purpose maps logical port to physical port. 
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  logical port value 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseInterfacePortMapSet(L7_uint32 UnitIndex, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32 val);

/*********************************************************************
*
* @purpose  Enables/Disables PoE auto power up mode on an interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if atleast one interface operation failed
*                       or any other failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortAutoPowerup(L7_uint32 UnitIndex,
                              L7_uint32 intIfNum, L7_uint32 status);

/*********************************************************************
*
* @purpose Gets port disconnect type on a given interface
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32  port disconnect type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortDisconnectTypeGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose Sets port disconnect type on a given interface
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val      L7_uint32  port disconnect type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortDisconnectTypeSet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 val);

/*********************************************************************
*
* @purpose  Enable/Disable Logical port mapping on a PoE subsystem.
*
* @param UnitIndex L7_uint32 the unit for this operation
*
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortMapSet(L7_uint32 UnitIndex,
                                         L7_uint32 val);

/*********************************************************************
*
* @purpose  Enable/Disable Logical port mapping on a PoE subsystem.
*
* @param UnitIndex L7_uint32 the unit for this operation
*
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainLogicalPortMapModeSet(L7_uint32 UnitIndex,
                                         L7_uint32 val);

/*********************************************************************
*
* @purpose  Gets Logical port mapping mode on a PoE subsystem.
*
* @param UnitIndex L7_uint32 the unit for this operation
*
* @param val      L7_uint32  State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseMainLogicalPortMapModeGet(L7_uint32 UnitIndex,
                                         L7_uint32 *val);
/********************************************************************
*
* @purpose  Switch to high power.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPsePortSwitchHighPower( L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_uint32 val);

/********************************************************************
*
* @purpose  Firmware Version on the Card
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param val      L7_uint8 Buffer to fill version
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPseFirmwareVersion( L7_uint32 UnitIndex, L7_uint8* val);

/*********************************************************************
*
* @purpose  Gets Power MDI Power Class string.
*
* @param mode      L7_uint32 Power MDI Power class mode
*
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerPortClassString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power MDI Power Support
*
* @param mode      L7_uint32 Power MDI Support
*
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerMDISupportedString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power Admin Enable State
*
* @param mode      L7_uint32 Power Admin Enable State
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerMDIEnabledString( L7_uint32 mode);
/*********************************************************************
*
* @purpose  Gets Power Pair Controlable String
*
* @param mode      L7_uint32 Power Pair Controlable String
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerPairControlableString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power Pair String
*
* @param mode      L7_uint32 Power Pair String
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerPairsString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power Class String
*
* @param mode      L7_uint32 Power Pair String
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerClassString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power Type String
*
* @param mode      L7_uint32 Power Pair Controlable String
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerTypeString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power Pair Controlable String
*
* @param mode      L7_uint32 Power Pair Controlable String
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerSourceString( L7_uint32 mode,L7_uint32 addrlen);

/*********************************************************************
*
* @purpose  Gets Power Pair String
*
* @param mode      L7_uint32 Power Pair String
* @returns  uchar8 *, if success
* @returns  L7_NULL, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
const L7_uchar8 *usmDbPoEPowerPriorityString( L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets Power Negotiated Via LLDP
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val       L7_uint32  Pointer to Value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @end
*********************************************************************/
L7_RC_t usmDbPoEPortLldpNegotiatiedPowerGet(L7_uint32 uintIndex,L7_uint32 intIfNum, L7_uint32* val);

/*********************************************************************
*
* @purpose  Sets the power requested by PD
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param val       L7_uint32  Power Requested Value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_SUPPORTED, if the interface is no PD type interface
* @returns  L7_FAILURE, if other failure
*
* @end
*********************************************************************/
L7_RC_t usmDbPethPdRequestingPowerSet(L7_uint32 uintIndex,L7_uint32 intIfNum, L7_uint32 powerRequested);
#endif /* USMDB_POE_API_H */
