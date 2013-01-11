/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_ch_pwr_api.h
*
* @purpose      Wireless Data Manager (WDM) API header
*
* @component    WDM
*
* @comments     none
*
* @create       01/30/06
*
* @author       dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_CH_PWR_API_H
#define INCLUDE_WDM_CH_PWR_API_H

#include "datatypes.h"
#include "platform_config.h"
#include "wireless_commdefs.h"

/****************************************
*   AP Radio Power Algorithm Info
****************************************/
typedef struct wdmManagedAPRadioPowerAlgo_s
{
  L7_uint32               lastRetryCount;    /* Power Algo Tx retry count */
  L7_uint32               lastFrameDup; /* Power Algo Duplicate frame count */

} wdmManagedAPRadioPowerAlgo_t;

/*********************************************************************
*
* @purpose  Set the channel plan mode for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t type  @b{(input)}  channel plan type (a, bg)
* @param    L7_WDM_CHANNEL_PLAN_MODE_t mode  @b{(input)}  mode (manual, interval, time)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanModeSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                              L7_WDM_CHANNEL_PLAN_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the channel plan mode for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type  @b{(input)}  channel plan type (a, bg)
* @param    L7_WDM_CHANNEL_PLAN_MODE_t *mode  @b{(output)}  mode (manual, interval, time)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanModeGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                              L7_WDM_CHANNEL_PLAN_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the channel plan interval for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t type     @b{(input)}  channel plan type (a, bg)
* @param    L7_uchar8                  interval @b{(input)}  interval in hours
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanIntervalSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                  L7_uchar8                  interval);

/*********************************************************************
*
* @purpose  Get the channel plan interval for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type     @b{(input)}  channel plan type (a, bg)
* @param    L7_uchar8                  *interval @b{(output)}  interval in hours
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanIntervalGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                  L7_uchar8                  *interval);

/*********************************************************************
*
* @purpose  Set the channel plan time for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t type     @b{(input)}  channel plan type (a, bg)
* @param    L7_ushort16                time     @b{(input)}  24 hour time in minutes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanTimeSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                              L7_ushort16                time);

/*********************************************************************
*
* @purpose  Get the channel plan time for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type @b{(input)}  channel plan type (a, bg)
* @param    L7_ushort16                *time @b{(output)}  24 hour time in minutes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanTimeGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                              L7_ushort16                *time);

/*********************************************************************
*
* @purpose  Set the channel plan history depth for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t type     @b{(input)}  channel plan type (a, bg)
* @param    L7_uchar8                  depth    @b{(input)}  number of history entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryDepthSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                      L7_uchar8                  depth);

/*********************************************************************
*
* @purpose  Get the channel plan history depth for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type   @b{(input)}  channel plan type (a, bg)
* @param    L7_uchar8                  *depth  @b{(output)} number of history entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryDepthGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                      L7_uchar8                  *depth);

/*********************************************************************
*
* @purpose  Set the operating status for a channel plan.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t    type    @b{(input)}  channel plan type (a, bg)
* @param    L7_WDM_CHANNEL_PLAN_STATUS_t  status  @b{(input)} active or inactive
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanOperatingStatusSet(L7_WDM_CHANNEL_PLAN_TYPE_t    type,
                                         L7_WDM_CHANNEL_PLAN_STATUS_t  status);

/*********************************************************************
*
* @purpose  Get the operating status for a channel plan.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t    type    @b{(input)}  channel plan type (a, bg)
* @param    L7_WDM_CHANNEL_PLAN_STATUS_t *status  @b{(output)} active or inactive
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanOperatingStatusGet(L7_WDM_CHANNEL_PLAN_TYPE_t    type,
                                         L7_WDM_CHANNEL_PLAN_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the current iteration for a channel plan.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t type      @b{(input)}  channel plan type (a, bg)
* @param    L7_uint32                  iteration @b{(input)}  current iteration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanCurrentIterationSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                          L7_uint32                  iteration);

/*********************************************************************
*
* @purpose  Get the current iteration for a channel plan.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t type      @b{(input)}   channel plan type (a, bg)
* @param    L7_uint32                 *iteration @b{(output)}  current iteration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanCurrentIterationGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                          L7_uint32                  *iteration);

/*********************************************************************
*
* @purpose  Set the manual channel adjustment status for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t    type    @b{(input)} channel plan type (a, bg)
* @param    L7_WDM_CHANNEL_ADJ_STATUS_t   status  @b{(input)} manual channel adjustment status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelManualAdjStatusSet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                     L7_WDM_CHANNEL_ADJ_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the manual channel adjustment status for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t    type    @b{(input)}  channel plan type (a, bg)
* @param    L7_WDM_CHANNEL_ADJ_STATUS_t  *status  @b{(output)} manual channel adjustment status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelManualAdjStatusGet(L7_WDM_CHANNEL_PLAN_TYPE_t   type,
                                     L7_WDM_CHANNEL_ADJ_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the last time the channel algorithm ran.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t    type       @b{(input)} channel plan type (a, bg)
* @param    L7_uint32                     timestamp  @b{(input)} last time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanLastAlgorithmTimeSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                           L7_uint32                  timestamp);

/*********************************************************************
*
* @purpose  Get the last time the channel algorithm ran.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t    type      @b{(input)}  channel plan type (a, bg)
* @param    L7_uint32                    *timestamp @b{(output)} last time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanLastAlgorithmTimeGet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                           L7_uint32                 *timestamp);
/*********************************************************************
*
* @purpose  Add an entry in the channel plan history for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIf     @b{(input)}  AP radio interface
* @param    L7_uint32                   iteration   @b{(input)}  iteration of channel algorithm
* @param    L7_uchar8                   channel     @b{(input)}  assigned channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryEntryAdd(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                      L7_enetMacAddr_t            macAddr,
                                      L7_uchar8                   radioIf,
                                      L7_uint32                   iteration,
                                      L7_uchar8                   channel);

/*********************************************************************
*
* @purpose  Verify an entry in the channel plan history for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryEntryGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                      L7_enetMacAddr_t            macAddr,
                                      L7_uchar8                   radioIf);

/*********************************************************************
*
* @purpose  Get next entry in the channel plan history for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_enetMacAddr_t           *macAddr     @b{(output)} next AP MAC address
* @param    L7_uchar8                  *radioIndex  @b{(output)} next AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryEntryNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                          L7_enetMacAddr_t            macAddr,
                                          L7_uchar8                   radioIf,
                                          L7_enetMacAddr_t           *nextMacAddr,
                                          L7_uchar8                  *nextRadioIf);

/*********************************************************************
*
* @purpose  Delete an entire iteration of the channel history.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_uint32                   iteration   @b{(input)}  channel plan iteration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, no entries found for specified iteration.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryIterationDelete(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                             L7_uint32                  iteration);

/*********************************************************************
*
* @purpose  Get assigned channel for a channel plan history entry.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_uchar8                  *channel     @b{(output)}  assigned channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryChannelGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                        L7_enetMacAddr_t            macAddr,
                                        L7_uchar8                   radioIf,
                                        L7_uchar8                  *channel);

/*********************************************************************
*
* @purpose  Get iteration for a channel plan history entry.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_uint32                  *iteration   @b{(output)} channel algorithm iteration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryIterationGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                          L7_enetMacAddr_t            macAddr,
                                          L7_uchar8                   radioIf,
                                          L7_uint32                  *iteration);

/*********************************************************************
*
* @purpose  Clear the entire channel plan history for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type   @b{(input)}  channel plan type (a, bg)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryClear(L7_WDM_CHANNEL_PLAN_TYPE_t type);


/*********************************************************************
*
* @purpose  Add an entry in the proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type            @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr         @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIf         @b{(input)}  AP radio interface
* @param    L7_uchar8                   currentChannel  @b{(input)}  current radio channel
* @param    L7_uchar8                   newChannel      @b{(input)}  proposed new channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjEntryAdd(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                      L7_enetMacAddr_t            macAddr,
                                      L7_uchar8                   radioIf,
                                      L7_uchar8                   currentChannel,
                                      L7_uchar8                   newChannel);

/*********************************************************************
*
* @purpose  Verify an entry in the proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjEntryGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                      L7_enetMacAddr_t            macAddr,
                                      L7_uchar8                   radioIf);

/*********************************************************************
*
* @purpose  Get next entry in the proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_enetMacAddr_t           *macAddr     @b{(output)} next AP MAC address
* @param    L7_uchar8                  *radioIndex  @b{(output)} next AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjEntryNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                          L7_enetMacAddr_t            macAddr,
                                          L7_uchar8                   radioIf,
                                          L7_enetMacAddr_t           *nextMacAddr,
                                          L7_uchar8                  *nextRadioIf);

/*********************************************************************
*
* @purpose  Get current channel for a proposed channel adjustment entry.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_uchar8                  *channel     @b{(output)}  current radio channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjCurrentChannelGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_enetMacAddr_t            macAddr,
                                               L7_uchar8                   radioIf,
                                               L7_uchar8                  *channel);

/*********************************************************************
*
* @purpose  Get new channel for a proposed channel adjustment entry.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_uchar8                  *channel     @b{(output)}  proposed new channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjNewChannelGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                           L7_enetMacAddr_t            macAddr,
                                           L7_uchar8                   radioIf,
                                           L7_uchar8                  *channel);

/*********************************************************************
*
* @purpose  Start a manual proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type  @b{(input)}  channel plan type (a, bg)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, algorithm or apply already in progress
*
* @comments This also purges any previous channel adjustment entries.
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjStart(L7_WDM_CHANNEL_PLAN_TYPE_t type);

/*********************************************************************
*
* @purpose  Apply an entire manual proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type  @b{(input)}  channel plan type (a, bg)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, in progress or no proposed channel adjustments to apply
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjApply(L7_WDM_CHANNEL_PLAN_TYPE_t type);


/*********************************************************************
*
* @purpose  Clear entire manual proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type  @b{(input)}  channel plan type (a, bg)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, in progress or no proposed channel adjustments.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjClear(L7_WDM_CHANNEL_PLAN_TYPE_t type);
                                   
/*********************************************************************
*
* @purpose  Set the global power adjustment mode.
*
* @param    L7_WDM_POWER_ADJ_MODE_t mode  @b{(input)}  mode (manual/interval)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerAdjModeSet(L7_WDM_POWER_ADJ_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the global power adjustment mode.
*
* @param    L7_WDM_POWER_ADJ_MODE_t *mode  @b{(output)}  mode (manual/interval)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerAdjModeGet(L7_WDM_POWER_ADJ_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the global power adjustment interval.
*
* @param    L7_ushort16     interval @b{(input)}  interval in minutes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerAdjIntervalSet(L7_ushort16 interval);

/*********************************************************************
*
* @purpose  Get the global power adjustment interval.
*
* @param    L7_ushort16   *interval @b{(output)}  interval in minutes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerAdjIntervalGet(L7_ushort16 *interval);

/*********************************************************************
*
* @purpose  Set the manual power adjustment status.
*
* @param    L7_WDM_POWER_ADJ_STATUS_t status  @b{(input)} manual power adjustment status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerManualAdjStatusSet(L7_WDM_POWER_ADJ_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the manual power adjustment status.
*
* @param    L7_WDM_POWER_ADJ_STATUS_t *status  @b{(output)} manual power adjustment status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerManualAdjStatusGet(L7_WDM_POWER_ADJ_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the manual power proposed adjustment tree count.
*
* @param    L7_uint32 *count  @b{(output)} proposed adjustment count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerManualPropAdjCountGet(L7_uint32 *count);

/*********************************************************************
*
* @purpose  Add an entry in the proposed power adjustment table.
*
* @param    L7_enetMacAddr_t            macAddr         @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIf         @b{(input)}  AP radio interface
* @param    L7_uchar8                   currentPower    @b{(input)}  current radio power
* @param    L7_uchar8                   newPower        @b{(input)}  proposed new power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjEntryAdd(L7_enetMacAddr_t            macAddr,
                                    L7_uchar8                   radioIf,
                                    L7_uchar8                   currentPower,
                                    L7_uchar8                   newPower);

/*********************************************************************
*
* @purpose  Verify an entry in the proposed power adjustment table.
*
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjEntryGet(L7_enetMacAddr_t            macAddr,
                                    L7_uchar8                   radioIf);

/*********************************************************************
*
* @purpose  Get next entry in the proposed power adjustment table.
*
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_enetMacAddr_t           *macAddr     @b{(output)} next AP MAC address
* @param    L7_uchar8                  *radioIndex  @b{(output)} next AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjEntryNextGet(L7_enetMacAddr_t            macAddr,
                                        L7_uchar8                   radioIf,
                                        L7_enetMacAddr_t           *nextMacAddr,
                                        L7_uchar8                  *nextRadioIf);

/*********************************************************************
*
* @purpose  Get current power setting for a proposed power adjustment entry.
*
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_uchar8                  *power       @b{(output)}  current radio power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjCurrentPowerGet(L7_enetMacAddr_t            macAddr,
                                           L7_uchar8                   radioIf,
                                           L7_uchar8                  *power);

/*********************************************************************
*
* @purpose  Get new power setting for a proposed power adjustment entry.
*
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_uchar8                  *power       @b{(output)}  proposed new power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjNewPowerGet(L7_enetMacAddr_t            macAddr,
                                       L7_uchar8                   radioIf,
                                       L7_uchar8                  *power);

/*********************************************************************
*
* @purpose  Start a manual proposed power adjustment.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, algorithm or apply already in progress
*
* @comments This also purges any previous power adjustment entries.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjStart();

/*********************************************************************
*
* @purpose  Apply an entire manual proposed power adjustment.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, in progress or no proposed power adjustments to apply
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjApply();

/*********************************************************************
*
* @purpose  Clear the manual proposed power adjustment table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, in progress or no proposed power adjustments.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerProposedAdjClear();

/*********************************************************************
*
* @purpose  Get Power Algorithm info per AP Radio
*
* @param    L7_enetMacAddr_t              macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                     radioIndex  @b{(input)}  AP radio interface
* @param    wdmManagedAPRadioPowerAlgo_t *radioPowerAlgo @b{(output)} APRadio Power Algo Info 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerAlgoAPRadioInfoGet(L7_enetMacAddr_t              macAddr,
                                   L7_uchar8                     radioIf,
                                   wdmManagedAPRadioPowerAlgo_t *radioPowerAlgo);

/*********************************************************************
*
* @purpose  Set Power Algorithm info per AP Radio
*
* @param    L7_enetMacAddr_t              macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                     radioIndex  @b{(input)}  AP radio interface
* @param    wdmManagedAPRadioPowerAlgo_t *radioPowerAlgo @b{(output)} APRadio Power Algo Info 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPowerAlgoAPRadioInfoSet(L7_enetMacAddr_t              macAddr,
                                   L7_uchar8                     radioIf,
                                   wdmManagedAPRadioPowerAlgo_t *radioPowerAlgo);

/*********************************************************************
*
* @purpose  Verify valid Channel type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t   type   @b{(input)} Channel type
*
* @returns  L7_SUCCESS  : if vaild channel type 
* @returns  L7_FAILURE  : if invalid channel type
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanTypeGet(L7_WDM_CHANNEL_PLAN_TYPE_t   chType);

/*********************************************************************
*
* @purpose  Verify valid Channel type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t   type   @b{(input)} Channel type
*
* @returns  L7_SUCCESS  : if vaild channel type 
* @returns  L7_FAILURE  : if invalid channel type
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanTypeNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t chType, L7_WDM_CHANNEL_PLAN_TYPE_t *chTypeNext);

/*********************************************************************
*
* @purpose  Get table entries in the channel plan history for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t *nextType    @b{(input)}  next channel plan type (a, bg)
* @param    L7_enetMacAddr_t           *macAddr     @b{(output)} next AP MAC address
* @param    L7_uchar8                  *radioIndex  @b{(output)} next AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelPlanHistoryTableIndexNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_enetMacAddr_t            macAddr,
                                               L7_uchar8                   radioIf,
                                               L7_WDM_CHANNEL_PLAN_TYPE_t *nextType,
                                               L7_enetMacAddr_t           *nextMacAddr,
                                               L7_uchar8                  *nextRadioIf);

/*********************************************************************
* @purpose  Get all table entries in the proposed channel adjustment for a radio type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t  type        @b{(input)}  channel plan type (a, bg)
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  AP MAC address
* @param    L7_uchar8                   radioIndex  @b{(input)}  AP radio interface
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t *nextType    @b{(input)}  next channel plan type (a, bg)
* @param    L7_enetMacAddr_t           *macAddr     @b{(output)} next AP MAC address
* @param    L7_uchar8                  *radioIndex  @b{(output)} next AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmChannelProposedAdjTableIndexNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_enetMacAddr_t            macAddr,
                                               L7_uchar8                   radioIf,
                                               L7_WDM_CHANNEL_PLAN_TYPE_t *nextType,
                                               L7_enetMacAddr_t           *nextMacAddr,
                                               L7_uchar8                  *nextRadioIf);

#endif /* INCLUDE_WDM_CH_PWR_API_H */
