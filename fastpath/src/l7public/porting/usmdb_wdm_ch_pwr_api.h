/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_ch_pwr_api.h
*
* @purpose    Wireless Data Manager Channel/Power USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     02/01/06
*
* @author     dfowler
*
* @end
*             
*********************************************************************/
#ifndef INCLUDE_USMDB_WDM_CH_PWR_API_H
#define INCLUDE_USMDB_WDM_CH_PWR_API_H

#include "l7_common.h"
#include "wireless_commdefs.h"

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
L7_RC_t usmDbWdmChannelPlanModeSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
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
L7_RC_t usmDbWdmChannelPlanModeGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelPlanIntervalSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
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
L7_RC_t usmDbWdmChannelPlanIntervalGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelPlanTimeSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
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
L7_RC_t usmDbWdmChannelPlanTimeGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelPlanHistoryDepthSet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
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
L7_RC_t usmDbWdmChannelPlanHistoryDepthGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                           L7_uchar8                  *depth);

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
L7_RC_t usmDbWdmChannelPlanOperatingStatusGet(L7_WDM_CHANNEL_PLAN_TYPE_t    type,
                                              L7_WDM_CHANNEL_PLAN_STATUS_t *status);

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
L7_RC_t usmDbWdmChannelPlanCurrentIterationGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_uint32                  *iteration);

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
L7_RC_t usmDbWdmChannelManualAdjStatusGet(L7_WDM_CHANNEL_PLAN_TYPE_t   type,
                                          L7_WDM_CHANNEL_ADJ_STATUS_t *status);

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
L7_RC_t usmDbWdmChannelPlanLastAlgorithmTimeGet(L7_WDM_CHANNEL_PLAN_TYPE_t type,
                                                L7_uint32                 *timestamp);
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
L7_RC_t usmDbWdmChannelPlanHistoryEntryGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelPlanHistoryEntryNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_enetMacAddr_t            macAddr,
                                               L7_uchar8                   radioIf,
                                               L7_enetMacAddr_t           *nextMacAddr,
                                               L7_uchar8                  *nextRadioIf);

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
L7_RC_t usmDbWdmChannelPlanHistoryChannelGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelPlanHistoryIterationGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_enetMacAddr_t            macAddr,
                                               L7_uchar8                   radioIf,
                                               L7_uint32                  *iteration);

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
L7_RC_t usmDbWdmChannelProposedAdjEntryGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelProposedAdjEntryNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                               L7_enetMacAddr_t            macAddr,
                                               L7_uchar8                   radioIf,
                                               L7_enetMacAddr_t            *nextMacAddr,
                                               L7_uchar8                   *nextRadioIf);

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
L7_RC_t usmDbWdmChannelProposedAdjCurrChannelGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelProposedAdjNewChannelGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
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
L7_RC_t usmDbWdmChannelProposedAdjStart(L7_WDM_CHANNEL_PLAN_TYPE_t type);


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
L7_RC_t usmDbWdmChannelProposedAdjApply(L7_WDM_CHANNEL_PLAN_TYPE_t type);


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
L7_RC_t usmDbWdmChannelProposedAdjClear(L7_WDM_CHANNEL_PLAN_TYPE_t type);

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
L7_RC_t usmDbWdmPowerAdjModeSet(L7_WDM_POWER_ADJ_MODE_t mode);

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
L7_RC_t usmDbWdmPowerAdjModeGet(L7_WDM_POWER_ADJ_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the global power adjustment interval.
*
* @param    L7_ushort16   interval @b{(intput)}  interval in minutes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmPowerAdjIntervalSet(L7_ushort16 interval);

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
L7_RC_t usmDbWdmPowerAdjIntervalGet(L7_ushort16 *interval);

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
L7_RC_t usmDbWdmPowerManualAdjStatusGet(L7_WDM_POWER_ADJ_STATUS_t *status);

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
L7_RC_t usmDbWdmPowerManualPropAdjCountGet(L7_uint32 *count);

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
L7_RC_t usmDbWdmPowerProposedAdjEntryGet(L7_enetMacAddr_t            macAddr,
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
L7_RC_t usmDbWdmPowerProposedAdjEntryNextGet(L7_enetMacAddr_t            macAddr,
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
L7_RC_t usmDbWdmPowerProposedAdjCurrentPowerGet(L7_enetMacAddr_t            macAddr,
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
L7_RC_t usmDbWdmPowerProposedAdjNewPowerGet(L7_enetMacAddr_t            macAddr,
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
L7_RC_t usmDbWdmPowerProposedAdjStart();

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
L7_RC_t usmDbWdmPowerProposedAdjApply();

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
L7_RC_t usmDbWdmPowerProposedAdjClear();


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
L7_RC_t usmDbWdmChannelPlanTypeGet(L7_WDM_CHANNEL_PLAN_TYPE_t  chType);

/*********************************************************************
*
* @purpose  Get Next valid Channel type.
*
* @param    L7_WDM_CHANNEL_PLAN_TYPE_t   type            @b{(input)} Channel type
*           L7_WDM_CHANNEL_PLAN_TYPE_t   *chTypeNext     @b{(input)} Next Channel type
*
* @returns  L7_SUCCESS  : if vaild channel type 
* @returns  L7_FAILURE  : if invalid channel type
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmChannelPlanTypeNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t chType, L7_WDM_CHANNEL_PLAN_TYPE_t *chTypeNext);

/*********************************************************************
*
* @purpose  Get Table index for channel plan history for a radio type.
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
L7_RC_t usmDbWdmChannelPlanHistoryTableIndexNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                                    L7_enetMacAddr_t            macAddr,
                                                    L7_uchar8                   radioIf,
                                                    L7_WDM_CHANNEL_PLAN_TYPE_t *nextType,
                                                    L7_enetMacAddr_t           *nextMacAddr,
                                                    L7_uchar8                  *nextRadioIf);

/*********************************************************************
*
* @purpose  Get Table index in the proposed channel adjustment for a radio type.
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
* @comments  @ This usmDb call is all the entries related to both the 
*              channel types rather getting entries of one channel type     
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmChannelProposedAdjTableIndexNextGet(L7_WDM_CHANNEL_PLAN_TYPE_t  type,
                                                    L7_enetMacAddr_t            macAddr,
                                                    L7_uchar8                   radioIf,
                                                    L7_WDM_CHANNEL_PLAN_TYPE_t *nextType, 
                                                    L7_enetMacAddr_t           *nextMacAddr,
                                                    L7_uchar8                  *nextRadioIf);

#endif /* INCLUDE_USMDB_WDM_CH_PWR_API_H */

