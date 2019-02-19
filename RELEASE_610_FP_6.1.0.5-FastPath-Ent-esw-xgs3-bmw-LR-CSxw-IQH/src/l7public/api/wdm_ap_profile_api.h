/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_api.h
*
* @purpose      Wireless Data Manager (WDM) API header
*
* @component    WDM
*
* @comments     none
*
* @create       10/20/2005
*
* @author       dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_AP_PROFILE_API_H
#define INCLUDE_WDM_AP_PROFILE_API_H

#include "l7_cnfgr_api.h"

/*********************************************************************
  APIs for usmDb layer, these APIs can be called via usmDb and 
  by wireless application components.
*********************************************************************/

/*********************************************************************
*
* @purpose  Add an entry to the AP Profile Table.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileEntryAdd (L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Delete an entry to the AP Profile Table.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileEntryDelete (L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Find if an entry existing in the AP Profile Table.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileEntryGet (L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Get the next entry from the AP Profile Table.
*
* @param    L7_uint32 *profileId @b{(input)} AP Profile ID
* @param    L7_uint32 *profileId @b{(output)} Next AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileEntryNextGet (L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Set the Profile Name for a Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_char8 *name @b{(input)} Profile Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileNameSet (L7_uint32 profileId, L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the Profile Name for a Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_char8 *name @b{(input)} Profile Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileNameGet (L7_uint32 profileId, L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the Keep-Alive Interval for a Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 interval @b{(input)} Keep-Alive Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileKeepAliveSet (L7_uint32 profileId, L7_uint32 interval);

/*********************************************************************
*
* @purpose  Get the Keep-Alive Interval for a Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 *interval @b{(output)} Keep-Alive Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileKeepAliveGet (L7_uint32 profileId, L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Copy from one Profile to another Profile.
*
* @param    L7_uint32 sourceProfileId @b{(input)} Source AP Profile ID
* @param    L7_uint32 destProfileId @b{(input)} Destination AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileCopy(L7_uint32 profileId, L7_uint32 toProfileId);

/*********************************************************************
*
* @purpose  Clear the Profile Configuration for this Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileClear(L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Reapply a profile configuration to all managed APs with the profile
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileApply(L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Set the AP Profile State
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_WDM_AP_PROFILE_STATE_t value @b{(intput)} AP Profile State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileStateSet(L7_uint32 profileId,
                             L7_WDM_AP_PROFILE_STATE_t value);

/*********************************************************************
*
* @purpose  Get the AP Profile State
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_WDM_AP_PROFILE_STATE_t *value @b{(output)} AP Profile State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileStateGet (L7_uint32 profileId,
                            L7_WDM_AP_PROFILE_STATE_t *value);

/*********************************************************************
*
* @purpose  Get the Managed APs using the Profile
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 *value @b{(output)} Managed AP Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileManagedAPCountGet(L7_uint32 profileId, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the Valid APs using the Profile
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 *value @b{(output)} Valid AP Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileValidAPCountGet(L7_uint32 profileId, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Find if the AP Profile Radio Index exists or not.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioEntryGet (L7_uint32 profileId, L7_uint32 radioIndex);

/*********************************************************************
*
* @purpose  Find the next AP Profile Radio Index if it exists.
*
* @param    L7_uint32 profileId @b{(input)}
* @param    L7_uint32 radioIndex @b{(input/output)} Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioEntryNextGet (L7_uint32 profileId,
                            L7_uint32 *radioIndex);

/*********************************************************************
*
* @purpose  Restore default settings for a radio interface.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioEntryClear(L7_uint32 profileId, 
                                    L7_uint32 radioIndex);

/*********************************************************************
*
* @purpose  Get the descriptive name for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_char8 *description @b{(output)} Descriptive Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioNameGet (L7_uint32 profileId, L7_uint32 radioIndex,
                            L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the Admin Mode for the Radio in AP Profile
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAdminModeSet(L7_uint32 profileId, L7_uint32 radioIndex,
                            L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the admin mode for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAdminModeGet(L7_uint32 profileId, L7_uint32 radioIndex,
                            L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Check if the mode for the AP profile radio is valid.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_PHY_MODE_t mode @b{(input)} Mode Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioPhyModeIsValid (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_WDM_RADIO_PHY_MODE_t mode);

/*********************************************************************
*
* @purpose  Set the mode for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_PHY_MODE_t mode @b{(input)} Mode Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioPhyModeSet (L7_uint32 profileId, L7_uint32 radioIndex,
                                 L7_WDM_RADIO_PHY_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the PHY Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_PHY_MODE_t *mode @b{(output)} PHY Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioPhyModeGet (L7_uint32 profileId, L7_uint32 radioIndex,
                                 L7_WDM_RADIO_PHY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the PHY Mode Mask for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_PHY_MODE_MASK_t *mask @b{(output)} PHY Mode Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioPhyModeMaskGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_PHY_MODE_MASK_t *mask);

/*********************************************************************
*
* @purpose  Check if the channel is valid for the AP Profile and Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uchar8 channel @b{(output)} Channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioValidChannelGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Get the Radio Sentry Scan Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId  @b{(input)}  AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)}  Radio Index
* @param    L7_uint32 *mode      @b{(output)} Sentry Scan Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioSentryScanModeGet (L7_uint32 profileId, L7_uint32 radioIndex,
                                            L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Radio Sentry Scan Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 mode       @b{(input)} Sentry Scan Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioSentryScanModeSet (L7_uint32 profileId, L7_uint32 radioIndex,
                                            L7_uint32 mode);


/*********************************************************************
*
* @purpose  Get if resource measurement is enabled the AP Profile Radio.
*
* @param    L7_uint32 profileId      @b{(input)}  AP Profile ID
* @param    L7_uint32 radioIndex     @b{(input)}  Radio Index of the AP profile
* @param    L7_BOOL   *pEnabled      @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t
wdmAPProfileRadioResourceMeasEnableGet (L7_uint32 profileId,
					L7_uint32 radioIndex, 
					L7_BOOL * pEnabled);


/*********************************************************************
*
* @purpose  Enable/disable Radio Resource Measurmt for AP Profile Radio.
*
* @param    L7_uint32 profileId   @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_BOOL   enabled     @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t 
wdmAPProfileRadioResourceMeasEnableSet (L7_uint32 profileId,
					L7_uint32 radioIndex,
					L7_BOOL enable);


/*********************************************************************
*
* @purpose  Get the Radio Other Channel Scan Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId  @b{(input)}  AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)}  Radio Index
* @param    L7_uint32 *mode      @b{(output)} Other Channel Scan Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioOtherChannelScanModeGet (L7_uint32 profileId, L7_uint32 radioIndex,
                                                  L7_uint32 *mode);



/*********************************************************************
*
* @purpose  Set the Radio Other Channel Scan Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 mode       @b{(input)} Other Channel Scan Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioOtherChannelScanModeSet (L7_uint32 profileId, L7_uint32 radioIndex,
                                                  L7_uint32 mode);

/*********************************************************************
*
* @purpose  Set the Radio Scan Frequency mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_SCAN_FREQUENCY_t scanFrequency @b{(input)} Scan
* @param                                    Frequency value,
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioScanFrequencySet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_SCAN_FREQUENCY_t scanFrequency);

/*********************************************************************
*
* @purpose  Get the Radio Scan Frequency mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_SCAN_FREQUENCY_t *scanFrequency @b{(output)} Scan
* @param                                     Frequency
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioScanFrequencyGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_SCAN_FREQUENCY_t *scanFrequency);

/*********************************************************************
*
* @purpose  Set the Radio Scan Interval for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 interval @b{(input)} Scan Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioScanIntervalSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 interval);

/*********************************************************************
*
* @purpose  Get the Radio Scan Interval for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *interval @b{(output)} Scan Interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioScanIntervalGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Set the Radio Mode Scan Duration for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 duration @b{(input)} Duration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioScanDurationSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 duration);

/*********************************************************************
*
* @purpose  Get the Radio Mode Scan Duration for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *duration @b{(output)} Duration Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioScanDurationGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *duration);

/*********************************************************************
*
* @purpose  Set the Broadcast/Multicast Rate Limiting for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 mode @b{(input)} L7_ENABLE OR L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioRateLimitModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the Broadcast/Multicast Rate Limiting for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioRateLimitModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Broadcast/Multicast Rate Limiting value for the
* @purpose  AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Rate Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioNormalRateLimitSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate);

/*********************************************************************
*
* @purpose  Get the Broadcast/Multicast Rate Limiting value for the
* @purpose  AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *rate @b{(output)} Rate Limit value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioNormalRateLimitGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Set the Broadcast/Multicast Burst Rate Limit value for the
* @purpose  AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 rate @b{(input)} Burst Rate Limit value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioBurstRateLimitSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate);

/*********************************************************************
*
* @purpose  Get the Broadcast/Multicast Burst Rate Limit value for the
* @purpose  AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *rate @b{(output)} Burst Rate Limit Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioBurstRateLimitGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Set the Beacon Interval for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 interval @b{(input)} Beacon Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioBeaconIntervalSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 interval);

/*********************************************************************
*
* @purpose  Get the Beacon Interval for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *interval @b{(output)} Beacon Interval Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioBeaconIntervalGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Set the DTIM Period for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 period @b{(input)} DTIM Period Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDTIMPeriodSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 period);

/*********************************************************************
*
* @purpose  Get the DTIM Period for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *period  @b{(output)} DTIM Period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDTIMPeriodGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *period);

/*********************************************************************
*
* @purpose  Set the Fragmentation Threshold value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 threshold @b{(input)} Fragmentation Threshold value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioFragmentationThresholdSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 threshold);

/*********************************************************************
*
* @purpose  Get the Fragmentation Threshold value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *threshold @b{(output)} Fragment Threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioFragmentationThresholdGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *threshold);

/*********************************************************************
*
* @purpose  Set the RTS threshold value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 threshold @b{(input)} RTS Threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioRTSThresholdSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 threshold);

/*********************************************************************
*
* @purpose  Get the RTS threshold value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *rtsThreshold @b{(output)} RTS Threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioRTSThresholdGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *threshold);

/*********************************************************************
*
* @purpose  Set the Short Retry Limit value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 shortRetryLimit @b{(input)} Short Retry Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioShortRetryLimitSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 shortRetryLimit);

/*********************************************************************
*
* @purpose  Get the Short Retry Limit value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *shortRetryLimit @b{(output)} Short Retry Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioShortRetryLimitGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *shortRetryLimit);

/*********************************************************************
*
* @purpose  Set the Long Retry Limit value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 longRetryLimit @b{(input)} Long Retry Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioLongRetryLimitSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 longRetryLimit);

/*********************************************************************
*
* @purpose  Get the long retry limit value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *longRetryLimit @b{(output)} Long Retry Limit Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioLongRetryLimitGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *longRetryLimit);

/*********************************************************************
*
* @purpose  Set the Maximum Transmit Lifetime value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 maxTransmitLifetime @b{(input)} Maximum Transmit Lifetime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMaxTransmitLifetimeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 maxTransmitLifetime);

/*********************************************************************
*
* @purpose  Get the Maximum Transmit Lifetime value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *maxTransmitLifetime @b{(output)} Maximum Transmit Lifetime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMaxTransmitLifetimeGet (L7_uint32 profileId,
                          L7_uint32 radioIndex, L7_uint32 *maxTransmitLifetime);

/*********************************************************************
*
* @purpose  Set the Maximum Receive Lifetime value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 maxReceiveLifetime @b{(input)} Maximum Recieve Lifetime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMaxReceiveLifetimeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 maxReceiveLifetime);

/*********************************************************************
*
* @purpose  Get the Maximum Receive Lifetime value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *maxReceiveLifetime @b{(output)} Maximum Receive Lifetime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMaxReceiveLifetimeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *maxReceiveLifetime);

/*********************************************************************
*
* @purpose  Set the 802.11n Channel Bandwidth for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_CHANNEL_BANDWIDTH_t bandwidth @b{(input)} Channel Bandwidth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelBandwidthSet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_CHANNEL_BANDWIDTH_t bandwidth);

/*********************************************************************
*
* @purpose  Get the 802.11n Channel Bandwidth for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_CHANNEL_BANDWIDTH_t *bandwidth @b{(output)} Channel Bandwidth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelBandwidthGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_CHANNEL_BANDWIDTH_t *bandwidth);

/*********************************************************************
*
* @purpose  Set the 802.11n Primary Channel for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_PRIMARY_CHANNEL_t primaryChannel @b{(input)} Primary Channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioPrimaryChannelSet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_PRIMARY_CHANNEL_t primaryChannel);

/*********************************************************************
*
* @purpose  Get the 802.11n Primary Channel for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_PRIMARY_CHANNEL_t *primaryChannel @b{(output)} Primary Channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioPrimaryChannelGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_PRIMARY_CHANNEL_t *primaryChannel);

/*********************************************************************
*
* @purpose  Set the 802.11n Protection for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_DOT11N_PROTECTION_t protection @b{(input)} Protection
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioProtectionSet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_DOT11N_PROTECTION_t protection);

/*********************************************************************
*
* @purpose  Get the 802.11n Protection for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_DOT11N_PROTECTION_t *protection @b{(output)} Protection
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioProtectionGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_DOT11N_PROTECTION_t *protection);

/*********************************************************************
*
* @purpose  Set the 802.11n Guard Interval for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_GUARD_INTERVAL_t guardInterval @b{(input)} Guard Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioGuardIntervalSet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_GUARD_INTERVAL_t guardInterval);

/*********************************************************************
*
* @purpose  Get the 802.11n Guard Interval for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_GUARD_INTERVAL_t *guardInterval @b{(output)} Guard Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioGuardIntervalGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_GUARD_INTERVAL_t *guardInterval);


/*********************************************************************
*
* @purpose  Set the 802.11n Space Time Block Code Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_STBC_MODE_t stbc @b{(input)} Space Time Block Code Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioSTBCSet (L7_uint32 profileId,
                                  L7_uint32 radioIndex,
                                  L7_WDM_RADIO_STBC_MODE_t stbc);

/*********************************************************************
*
* @purpose  Get the 802.11n Space Time Block Code Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_WDM_RADIO_STBC_MODE_t *stbc @b{(input)} Space Time Block Code Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioSTBCGet (L7_uint32 profileId,
                                  L7_uint32 radioIndex,
                                  L7_WDM_RADIO_STBC_MODE_t *stbc);

/*********************************************************************
*
* @purpose  Set the Multicast Tx Rate for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 rate @b{(input)} Multicast Tx Rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMcastTxRateSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate);

/*********************************************************************
*
* @purpose  Get the Multicast Tx Rate for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *rate @b{(input)} Multicast Tx Rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMcastTxRateGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Set the TSPEC Voice ACM Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVoiceAcmModeSet(L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC Voice ACM Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVoiceAcmModeGet(L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the TSPEC Video ACM Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVideoAcmModeSet(L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC Video ACM Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVideoAcmModeGet(L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the TSPEC Voice ACM Limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVoiceAcmLimitSet(L7_uint32 profileId,
                                               L7_uint32 radioIndex,
                                               L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC Voice ACM Limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVoiceAcmLimitGet(L7_uint32 profileId,
                                               L7_uint32 radioIndex,
                                               L7_uint32 *val);

/*********************************************************************
*
* @purpose  Check if TSPEC Voice ACM Limit value is within limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The combined Voice ACM and Video ACM Limit values must not
*           exceed the FD_WDM_DEFAULT_WS_TSPEC_ACM_BW_MAX build default.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVoiceAcmLimitCheck(L7_uint32 profileId,
                                                 L7_uint32 radioIndex,
                                                 L7_uint32 val);

/*********************************************************************
*
* @purpose  Set the TSPEC Video ACM Limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVideoAcmLimitSet(L7_uint32 profileId,
                                               L7_uint32 radioIndex,
                                               L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC Video ACM Limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVideoAcmLimitGet(L7_uint32 profileId,
                                               L7_uint32 radioIndex,
                                               L7_uint32 *val);

/*********************************************************************
*
* @purpose  Check if TSPEC Video ACM Limit value is within limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The combined Voice ACM and Video ACM Limit values must not
*           exceed the FD_WDM_DEFAULT_WS_TSPEC_ACM_BW_MAX build default.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecVideoAcmLimitCheck(L7_uint32 profileId,
                                                 L7_uint32 radioIndex,
                                                 L7_uint32 val);

/*********************************************************************
*
* @purpose  Set the TSPEC Roam Reserve Limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Setting a value of 0 effectively disables reserving wireless
*           ACM bandwidth for use by roaming clients.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecRoamReserveLimitSet(L7_uint32 profileId,
                                                  L7_uint32 radioIndex,
                                                  L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC Roam Reserve Limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A value of 0 indicates none of the ACM bandwidth is reserved
*           for roaming clients.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecRoamReserveLimitGet(L7_uint32 profileId,
                                                  L7_uint32 radioIndex,
                                                  L7_uint32 *val);

/*********************************************************************
*
* @purpose  Check if TSPEC Roam Reserve Limit value is within limit for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecRoamReserveLimitCheck(L7_uint32 profileId,
                                                    L7_uint32 radioIndex,
                                                    L7_uint32 val);

/*********************************************************************
*
* @purpose  Set the TSPEC AP Inactivity Timeout for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecApInactivityTimeoutSet(L7_uint32 profileId,
                                                     L7_uint32 radioIndex,
                                                     L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC AP Inactivity Timeout for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecApInactivityTimeoutGet(L7_uint32 profileId,
                                                     L7_uint32 radioIndex,
                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the TSPEC STA Inactivity Timeout for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecStaInactivityTimeoutSet(L7_uint32 profileId,
                                                      L7_uint32 radioIndex,
                                                      L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC STA Inactivity Timeout for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecStaInactivityTimeoutGet(L7_uint32 profileId,
                                                      L7_uint32 radioIndex,
                                                      L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the TSPEC Legacy WMM Queue Map Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 val @b{(input)} Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecLegacyWmmQueueMapModeSet(L7_uint32 profileId,
                                                       L7_uint32 radioIndex,
                                                       L7_uint32 val);

/*********************************************************************
*
* @purpose  Get the TSPEC Legacy WMM Queue Map Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *val @b{(output)} Pointer to output value location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioTspecLegacyWmmQueueMapModeGet(L7_uint32 profileId,
                                                       L7_uint32 radioIndex,
                                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the Automatic Power Save Delivery Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_BOOL apsd @b{(input)} APSD
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPSDSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 apsd);

/*********************************************************************
*
* @purpose  Get the Automatic Power Save Delivery Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_BOOL *apsd @b{(input)} APSD
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPSDGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *apsd);

/*********************************************************************
*
* @purpose  Set the No ACK Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_BOOL noAck @b{(input)} No ACK
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioNoACKSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 noAck);

/*********************************************************************
*
* @purpose  Get the No ACK Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_BOOL *noAck @b{(input)} No ACK
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioNoACKGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *noAck);

/*********************************************************************
*
* @purpose  Set the Maximum Clients value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 maxClients @b{(input)} Maximum Clients Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMaxClientsSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 maxClients);

/*********************************************************************
*
* @purpose  Get the Maximum Clients value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *maxClients @b{(output)} Maximum Clients Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioMaxClientsGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *maxClients);

/*********************************************************************
*
* @purpose  Set the Auto Channel Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 autoChannel @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAutoChannelModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 autoChannel);

/*********************************************************************
*
* @purpose  Get the Auto Channel Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *autoChannel @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAutoChannelModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *autoChannel);

/*********************************************************************
*
* @purpose  Set the 802.11a Limit Channel Selection for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 limitChannelSel @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDot11aLimitChannelSelectionSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 limitChannelSel);

/*********************************************************************
*
* @purpose  Get the 802.11a Limit Channel Selection for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 *limitChannelSel @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDot11aLimitChannelSelectionGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *limitChannelSel);




/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} channel to verify
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the channel is NOT available for the current country
*           code, this routine will return L7_FAILURE.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelEligibleGet (L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uchar8  channel);


/*********************************************************************
*
* @purpose  Get next auto-eligible Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} current channel 
* @param    L7_uchar8  *pChannel  @b{(output)} next channel 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the channel is NOT available for the current country
*           code, this routine will return L7_FAILURE.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelEligibleNextGet (L7_uint32 profileId,
                                                  L7_uint32 radioIndex,
                                                  L7_uchar8  channel,
                                                  L7_uchar8  *pNextChannel);


/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the channel is NOT available for the current country
*           code, this routine will return L7_FAILURE.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelSupportedGet(L7_uint32 profileId,
                                             L7_uint32 radioIndex,
                                             L7_uchar8  channel);


/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} current channel
* @param    L7_uchar8 *pNextChannel @b{(output)} next channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the channel is NOT available for the current country
*           code, this routine will return L7_FAILURE.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelSupportedNextGet(L7_uint32 profileId,
                                                 L7_uint32 radioIndex,
                                                 L7_uchar8  channel,
                                                 L7_uchar8  *pNextChannel);




/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} L7_ENABLE or L7_DISABLE
* @param    L7_BOOL   isEligilble  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the channel is NOT available for the current country
*           code, this routine will return L7_FAILURE.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioChannelEligibleSet (L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uchar8  channel,
                                              L7_BOOL   toBeEligible);


/*********************************************************************
*
* @purpose  Set the Auto Power Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 autoPower @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAutoPowerModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 autoPower);

/*********************************************************************
*
* @purpose  Get the Auto Power Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *autoPower @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAutoPowerModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *autoPower);

/*********************************************************************
*
* @purpose  Set the Default Transmission Power value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 defaultTxPower @b{(input)} Default TX Power Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDefaultTxPowerSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 defaultTxPower);

/*********************************************************************
*
* @purpose  Get the Default Transmission Power value for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *defaultTxPower @b{(output)} Default Transmission Power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDefaultTxPowerGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *defaultTxPower);

/*********************************************************************
*
* @purpose  Get a rate entry for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Supported Rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioRateEntryGet(L7_uint32 profileId,
                                      L7_uint32 radioIndex, 
                                      L7_uint32 rate);

/*********************************************************************
*
* @purpose  Get the next rate entry for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 *rate @b{(input)} Supported Rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioRateEntryNextGet(L7_uint32  profileId,
                                          L7_uint32  radioIndex, 
                                          L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Set the Data Rate Value to enable/disable in Supported List in the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Supported Rate
* @param    L7_uint32 mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioSuppRateSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate,
                            L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get if Data Rate Value is enabled in Supported List in the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Supported Rate
* @param    L7_uint32 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioSuppRateGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate,
                            L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Data Rate Value to enable/disable in Advertised List in the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Advertised Rate
* @param    L7_uint32 mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAdvtRateSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate,
                            L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get if Data Rate Value is enabled in Advertised List in the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Advertised Rate
* @param    L7_uint32 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAdvtRateGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate,
                            L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Check if this is a valid rate or not for this PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 rate @b{(input)} Rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPhyModeRateEntryGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uint32 rate);

/*********************************************************************
*
* @purpose  Get the next rate entry from the valid list of rate entries for thi PHY Mode
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 *rate @b{(output)} Rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPhyModeRateEntryNextGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                                L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Get if Data Rate Value is enabled in Supported List for the PHY Mode
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 rate @b{(input)} Supported Rate
* @param    L7_uint32 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPhyModeSuppRateGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                                L7_uint32 rate, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Get if Data Rate Value is enabled in Advertised List for the PHY Mode
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 rate @b{(input)} Advertised Rate
* @param    L7_uint32 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPhyModeAdvtRateGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                                L7_uint32 rate, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Check if this is a valid multicast tx rate or not for this PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 rate @b{(input)} Rate
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPhyModeMcastTxRateEntryGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uint32 rate);

/*********************************************************************
*
* @purpose  Get the next multicast  tx rate entry from the valid list of rate entries for thi PHY Mode
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 *rate @b{(output)} Rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPhyModeMcastTxRateEntryNextGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                                L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Set the WMM mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 wmmMode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioWMMModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 wmmMode);

/*********************************************************************
*
* @purpose  Get the WMM mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *wmmMode @b{(output)} WMM Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioWMMModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *wmmMode);

/*********************************************************************
*
* @purpose  Set the Load Balancing Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 loadBalancing @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioLoadBalancingModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 loadBalancing);

/*********************************************************************
*
* @purpose  Get the Load Balancing Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 *loadBalancing @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioLoadBalancingModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *loadBalancing);

/*********************************************************************
*
* @purpose  Set the Percentage Utilization allowed, when Load Balancing enabled,
* @purpose  for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 utilization @b{(input)} Utilization in Percentage
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioUtilizationSet (L7_uint32 profileId,
                           L7_uint32 radioIndex, L7_uint32 percentUtilization);

/*********************************************************************
*
* @purpose  Get the Percentage Utilization allowed, when Load Balancing enabled,
* @purpose  for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *percentUtilization @b{(output)} Percentage Utilization
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioUtilizationGet (L7_uint32 profileId,
                          L7_uint32 radioIndex, L7_uint32 *percentUtilization);

/*********************************************************************
*
* @purpose  Get the Data Snooping Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId      @b{(input)}  AP Profile ID
* @param    L7_uint32 radioIndex     @b{(input)}  Radio Index of the AP profile
* @param    L7_uint32 *dataSnoopMode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDataSnoopModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *dataSnoopMode);

/*********************************************************************
*
* @purpose  Set the Data Snooping Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId      @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex     @b{(input)} Radio Index
* @param    L7_uint32 dataSnoopMode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioDataSnoopModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 dataSnoopMode);

/*********************************************************************
*
* @purpose  Find if AP/Station EDCA entry exists in the AP Radio Profile Table.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioEdcaEntryGet (L7_uint32 profileId,
                              L7_uint32 radioIndex, L7_int32 queueId);

/*********************************************************************
*
* @purpose  Find the next Radio Profile AP/Station EDCA Index if it exists.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile Index
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 *queueId @b{(output)} QoS Queue ID
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioEdcaEntryNextGet (L7_uint32 profileId,
                              L7_uint32 radioIndex, L7_int32 *queueId);

/*********************************************************************
* @purpose  Set AP EDCAs Arbitration Inter-frame Spacing(AIFS) 
*           value for the given AP Profile ID, Radio Index and the 
*           QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 aifs @b{(input)} Downstream traffic wait 
*                                            time for data frames
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaAifsSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 aifs);

/*********************************************************************
* @purpose  Get AP EDCA Arbitration Inter-frame Spacing(AIFS) 
*           value for the given AP Profile Id, Radio Index and the 
*           QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 *aifs  @b{(output)} Downstream traffic wait 
*                                            time for data frames
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaAifsGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 *aifs);

/*********************************************************************
* @purpose  Set AP EDCAs Contention Window Minimum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 cwmin @b{(input)} Upper limit of a range from which
*                                          the initial random backoff wait time 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaCWMinSet (L7_uint32 profileId,
                        L7_uint32 radioIndex, L7_int32 queueId,
                        L7_ushort16 cwmin);

/*********************************************************************
*
* @purpose  Get Contention Window values in the increasing order
*
* @param    L7_ushort16 *cw  @b{(output)} Next Contention Window value
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioEdcaCWNextGet (L7_ushort16 *cw);

/*********************************************************************
* @purpose  Get AP EDCAs Contention Window Minimum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 *cwmin  @b{(output)} Upper limit of a range from
*                                            which the initial random 
*                                            backoff wait time 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaCWMinGet (L7_uint32 profileId,
                        L7_uint32 radioIndex, L7_int32 queueId,
                        L7_ushort16 *cwmin);

/*********************************************************************
* @purpose  Set AP EDCAs Contention Window Maximum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 cwmax  @b{(input)} Upper limit for the doubling of 
*                                           the random backoff value, doubling
*                                           continues until either the data 
*                                           frame is sent or this value is 
*                                           reached.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaCWMaxSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 cwmax);

/*********************************************************************
* @purpose  Get AP EDCAs Contention Window Maximum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 *cwmax  @b{(output)} Upper limit for the doubling of 
*                                            the random backoff value, doubling
*                                            continues until either the data 
*                                            frame is sent or this value is 
*                                            reached.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaCWMaxGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 *cwmax);

/*********************************************************************
* @purpose  Set AP EDCAs Maximum Burst Length for the given AP Profile ID,
*           Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_uint32 burstLength  @b{(input)} Maximum burst length allowed for
*                                        packet bursts on the wireless network.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaBurstLengthSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_uint32 burstLength);

/*********************************************************************
* @purpose  Get AP EDCAs Maximum Burst Length for the given AP Profile ID,
*           Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_uint32 *burstLength  @b{(output)} Maximum burst length allowed
*                                          for packet bursts on the wireless 
*                                          network.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioAPEdcaBurstLengthGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_uint32 *burstLength);

/*********************************************************************
* @purpose  Set Station EDCAs Arbitration Inter-frame Spacing(AIFS) 
*           value for the given AP Profile ID, Radio Index and the 
*           QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 aifs @b{(input)} Upstream data traffic wait 
*                                          time for data frames.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaAifsSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 aifs);

/*********************************************************************
* @purpose  Get Station EDCAs Arbitration Inter-frame Spacing(AIFS) 
*           value for the given AP Profile ID, Radio Index and the 
*           QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 *aifs  @b{(output)} Upstream data traffic wait 
*                                            time for data frames.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaAifsGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 *aifs);

/*********************************************************************
* @purpose  Set Station EDCAs Contention Window Minimum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 cwmin @b{(input)} Upper limit of a range from which
*                                          the initial random backoff wait time 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaCWMinSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 cwmin);

/*********************************************************************
* @purpose  Get Station EDCAs Contention Window Minimum value for the given
*           AP Profile Index, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 *cwmin  @b{(output)} Upper limit of a range from
*                                            which the initial random 
*                                            backoff wait time 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaCWMinGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 *cwmin);

/*********************************************************************
* @purpose  Set Station EDCAs Contention Window Maximum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 cwmax @b{(input)} Upper limit for the doubling of 
*                                           the random backoff value, doubling
*                                           continues until either the data 
*                                           frame is sent or this value is 
*                                           reached.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaCWMaxSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 cwmax);

/*********************************************************************
* @purpose  Get Station EDCAs Contention Window Maximum value for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_ushort16 *cwmax @b{(output)} Upper limit for the doubling of 
*                                            the random backoff value, doubling
*                                            continues until either the data 
*                                            frame is sent or this value is 
*                                            reached.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaCWMaxGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 *cwmax);

/*********************************************************************
* @purpose  Set Station EDCAs Transmission Opportunity Limit for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_uint32 txOpLimit @b{(input)} An interval of time when a WME 
*                                        client station has the right to 
*                                        initiate transmissions onto the 
*                                        wireless medium.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaTxOpLimitSet (L7_uint32 profileId, 
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_uint32 txOpLimit);

/*********************************************************************
* @purpose  Get Station EDCAs Transmission Opportunity Limit for the given
*           AP Profile ID, Radio Index and the QoS queue.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex  @b{(input)} Radio Index
* @param    L7_int32 queueId @b{(input)} QoS Queue ID
* @param    L7_uint32 *txOpLimit @b{(output)} An interval of time when a WME 
*                                          client station has the right to 
*                                          initiate transmissions onto the 
*                                          wireless medium.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationEdcaTxOpLimitGet (L7_uint32 profileId, 
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_uint32 *txOpLimit);

/*********************************************************************
* @purpose  Validates AP Profile VAP entry. 
*
* @param    L7_uint32 profileId    @b{(input)} AP Profile ID 
* @param    L7_uint32 radioId      @b{(input)} Radio interface
* @param    L7_uint32 vapId        @b{(input)} VAP id
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileVAPEntryGet(L7_uint32 profileId,
                                L7_uint32 radioId,
                                L7_uint32 vapId);

/*********************************************************************
* @purpose  Get next AP Profile VAP entry. 
*
* @param    L7_uint32  profileId    @b{(input)} AP Profile ID 
* @param    L7_uint32  radioId      @b{(input)} Radio interface
* @param    L7_uint32 *vapId        @b{(input/output)} VAP id
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileVAPEntryNextGet(L7_uint32 profileId,
                                    L7_uint32 radioId,
                                    L7_int32 *vapId);

/*********************************************************************
* @purpose  Sets VAP mode.
*
* @param    L7_uint32         profileId    @b{(input)} AP Profile ID 
* @param    L7_uint32         radioId      @b{(input)} Radio interface
* @param    L7_uint32         vapId        @b{(input)} VAP id
* @param    L7_uint32         mode         @b{(input)} L7_ENABLE or L7_DISABLE 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileVAPModeSet(L7_uint32         profileId,
                               L7_uint32         radioId,
                               L7_uint32         vapId,
                               L7_uint32         mode);

/*********************************************************************
* @purpose  Set the VAP network ID.
*
* @param    L7_uint32 profileId    @b{(input)} AP Profile ID 
* @param    L7_uint32 radioId      @b{(input)} Radio interface
* @param    L7_uint32 vapId        @b{(input)} VAP id
* @param    L7_uint32 networkId    @b{(input)} Network ID 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileVAPNetworkIDSet(L7_uint32 profileId,
                                    L7_uint32 radioId,
                                    L7_uint32 vapId,
                                    L7_uint32 networkId);

/*********************************************************************
* @purpose  Get VAP mode. 
*
* @param    L7_uint32         profileId    @b{(input)}  AP Profile ID 
* @param    L7_uint32         radioId      @b{(input)}  Radio interface
* @param    L7_uint32         vapId        @b{(input)}  VAP ID
* @param    L7_uint32         mode         @b{(output)} L7_ENABLE or L7_DISABLE 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileVAPModeGet(L7_uint32          profileId,
                               L7_uint32          radioId,
                               L7_uint32          vapId,
                               L7_uint32         *mode);

/*********************************************************************
* @purpose  Get the VAP network ID. 
*
* @param    L7_uint32  profileId    @b{(input)} AP Profile ID 
* @param    L7_uint32  radioId      @b{(input)} Radio interface
* @param    L7_uint32  vapId        @b{(input)} VAP id
* @param    L7_uint32 *networkId    @b{(output)} Network ID 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmAPProfileVAPNetworkIDGet(L7_uint32  profileId,
                                    L7_uint32  radioId,
                                    L7_uint32  vapId,
                                    L7_uint32 *networkId);

/*********************************************************************
*
* @purpose  Read global flag to determine if there have been any
*            VAP configuration changes.
*
* @param    L7_BOOL flag    @b((output))  L7_TRUE/L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The following changes are only flagged if 
*                 tunneling feature is enabled. 
*            VAP changes flagged:
*              VAP enabled/disabled,
*              VAP network ID change,
*            Network changes flagged: (if network is applied to
*                                      one or more enabled VAPs)
*              Network tunneling mode enabled/disabled,
*              Network tunnel subnet/mask modified
*            
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileVAPTunnelFlagGetClear(L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Get the AP Profile modified flag value
*
* @param    L7_uint32 profileId @b{(input)}  AP Profile ID
* @param    L7_uchar8 *flag     @b{(output)} AP Profile Modified Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileModifiedFlagGet (L7_uint32 profileId,
                                     L7_uchar8 *value);


/*********************************************************************
*
* @purpose  Get the Station Isolation Mode
* @purpose  for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *Mode @b{(output)} Station Isolation Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationIsolationGet(L7_uint32 profileId,
                                             L7_uint32 radioIndex, 
                                             L7_uint32 *Mode);


/*********************************************************************
*
* @purpose  Set the Station Isolation Mode
* @purpose  for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uchar Mode @b{(output)} Station Isolation Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioStationIsolationSet(L7_uint32 profileId,
                                             L7_uint32 radioIndex, 
                                             L7_uint32 Mode);

/*********************************************************************
*
* @purpose  Set the hardware type for the AP Profile.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 hwType @b{(input)} Hardware type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileHardwareTypeSet(L7_uint32 profileId, L7_uint32 hwType);

/*********************************************************************
*
* @purpose  Get the hardware type for the AP Profile.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 hwType @b{(output)} Hardware type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileHardwareTypeGet(L7_uint32 profileId, L7_uint32 *hwType);

/*********************************************************************
*
* @purpose  Check if the specified radio exists in AP hardware type
*           configured by the specified profile.
*
*
* @param    L7_uint32 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioIndexGet(L7_uint32  profileId,L7_uint32 radioIndex);

/*********************************************************************
*
* @purpose  Set the Vlan ID for a Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_ushort16 interval @b{(input)} VLAN ID that this AP should 
*              use to send packets on the wire that enable the system to 
*              discover rogue APs attached to the wired network. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      0 - Send packets without a tag.  
*           1-4094 - VLAN ID
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileVlanIdSet (L7_uint32 profileId, L7_ushort16 vlan_id);

/*********************************************************************
*
* @purpose  Get the Vlan ID for a Profile ID
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_ushort16 *vlan_id @b{(output)} VLAN ID that this AP should 
*              use to send packets on the wire that enable the system to 
*              discover rogue APs attached to the wired network. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      0 - Send packets without a tag.  
*           1-4094 - VLAN ID
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileVlanIdGet (L7_uint32 profileId, L7_ushort16 *vlan_id);



/*********************************************************************
*
* @purpose  Set Antenna's selected for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 antSel @b{(output)} Antenna's Selected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAntennaSelSet(L7_uint32 profileId,
                                      L7_uint32 radioIndex,
                                      L7_uint32 antSel);
/*********************************************************************
*
* @purpose  Set Antenna Diversity status for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 antDiv @b{(output)} Antenna Diversity status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAntennaDivSet(L7_uint32 profileId,
                                      L7_uint32 radioIndex,
                                      L7_uint32 antDiv);

/*********************************************************************
*
* @purpose  Get Antenna's selected for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *antSel @b{(output)} Antenna's Selected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAntennaSelGet(L7_uint32 profileId,
                                      L7_uint32 radioIndex,
                                      L7_uint32 *antSel);
/*********************************************************************
*
* @purpose  Get Antenna Diversity status for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *antDiv @b{(output)} Antenna Diversity status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileRadioAntennaDivGet(L7_uint32 profileId,
                                      L7_uint32 radioIndex,
                                      L7_uint32 *antDiv);

/*********************************************************************
* @purpose  Get if the configured country code supports 40MHz bandwidth
*           operation in the phyMode of the radio with
*           given profileId and radioIndex.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*           L7_uint32 radioIndex @b{(input)} Radio Index
*
* @returns L7_SUCCESS, if supports.
* @retruns L7_FAILURE, if doesn't support.
*
* @notes    None.
*
* @end
***********************************************************************/
L7_RC_t wdmAPProfileRadioCountrySupportForFortyMHz(L7_uint32 profileId,
                           L7_uint32 radioIndex);

#endif /* INCLUDE_WDM_AP_PROFILE_API_H */
