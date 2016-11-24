/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_wdm_api.h
*
* @purpose  Wireless Data Manager (WDM) USMDB API functions
*
* @component    WDM
*
* @comments none
*
* @create   10/20/2005
*
* @author   dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_USMDB_WDM_AP_PROFILE_API_H
#define INCLUDE_USMDB_WDM_AP_PROFILE_API_H

#include "l7_common.h"
#include "wireless_commdefs.h"

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
L7_RC_t usmDbWdmAPProfileEntryAdd (L7_uint32 profileId);

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
L7_RC_t usmDbWdmAPProfileEntryDelete (L7_uint32 profileId);

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
L7_RC_t usmDbWdmAPProfileEntryGet (L7_uint32 profileId);

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
L7_RC_t usmDbWdmAPProfileEntryNextGet (L7_uint32 *profileId);

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
L7_RC_t usmDbWdmAPProfileNameSet (L7_uint32 profileId, L7_char8 *name);

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
L7_RC_t usmDbWdmAPProfileNameGet (L7_uint32 profileId, L7_char8 *name);

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
L7_RC_t usmDbWdmAPProfileKeepAliveSet (L7_uint32 profileId, L7_uint32 interval);

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
L7_RC_t usmDbWdmAPProfileKeepAliveGet (L7_uint32 profileId, L7_uint32 *interval);

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
L7_RC_t usmDbWdmAPProfileCopy(L7_uint32 profileId, L7_uint32 toProfileId);

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
L7_RC_t usmDbWdmAPProfileClear(L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Reapply a profile configuration to all managed APs 
*               configured with this profile.
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
L7_RC_t usmDbWdmAPProfileApply(L7_uint32 profileId);

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
L7_RC_t usmDbWdmAPProfileStateSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileStateGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileManagedAPCountGet (L7_uint32 profileId,
                            L7_uint32 *value);

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
L7_RC_t usmDbWdmAPProfileValidAPCountGet (L7_uint32 profileId,L7_uint32 *value);

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
L7_RC_t usmDbWdmAPProfileRadioEntryGet (L7_uint32 profileId,
                            L7_uint32 radioIndex);

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
L7_RC_t usmDbWdmAPProfileRadioEntryNextGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioEntryClear(L7_uint32 profileId, 
                                         L7_uint32 radioIndex);

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
L7_RC_t usmDbWdmAPProfileRadioAdminModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 mode);

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
L7_RC_t usmDbWdmAPProfileRadioAdminModeGet(L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *mode);

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
L7_RC_t usmDbWdmAPProfileRadioPhyModeIsValid (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioPhyModeSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_WDM_RADIO_PHY_MODE_t mode);

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
L7_RC_t usmDbWdmAPProfileRadioPhyModeGet (L7_uint32 profileId,
                          L7_uint32 radioIndex, L7_WDM_RADIO_PHY_MODE_t *mode);

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
L7_RC_t usmDbWdmAPProfileRadioPhyModeMaskGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioValidChannelGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uchar8 channel);

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
L7_RC_t usmDbWdmAPProfileRadioSentryScanModeSet (L7_uint32 profileId,
                           L7_uint32 radioIndex, L7_uint32 mode);

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
L7_RC_t usmDbWdmAPProfileRadioSentryScanModeGet (L7_uint32 profileId,
                           L7_uint32 radioIndex, L7_uint32 *mode);




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
usmDbWdmAPProfileRadioResourceMeasEnabledGet(L7_uint32 profileId,
					     L7_uint32 radioIndex, 
					     L7_BOOL * pEnabled);

/*********************************************************************
*
* @purpose  Enable/disable Radio Resource Meas for the AP Profile Radio.
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
usmDbWdmAPProfileRadioResourceMeasEnableSet (L7_uint32 profileId,
					     L7_uint32 radioIndex,
					     L7_BOOL enable);

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
L7_RC_t usmDbWdmAPProfileRadioOtherChannelScanModeSet (L7_uint32 profileId,
                           L7_uint32 radioIndex, L7_uint32 mode);

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
L7_RC_t usmDbWdmAPProfileRadioOtherChannelScanModeGet (L7_uint32 profileId,
                           L7_uint32 radioIndex, L7_uint32 *mode);

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
L7_RC_t usmDbwdmAPProfileRadioScanFrequencySet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioScanFrequencyGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioScanIntervalSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioScanIntervalGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Set the Radio Scan Duration for the AP Profile Radio.
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
L7_RC_t usmDbWdmAPProfileRadioScanDurationSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 duration);

/*********************************************************************
*
* @purpose  Get the Radio Scan Duration for the AP Profile Radio.
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
L7_RC_t usmDbWdmAPProfileRadioScanDurationGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *duration);

/*********************************************************************
*
* @purpose  Get the Extended Range for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 *extendedRange @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioExtendedRangeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *extendedRange);

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
L7_RC_t usmDbWdmAPProfileRadioRateLimitModeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioRateLimitModeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioNormalRateLimitSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioNormalRateLimitGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioBurstRateLimitSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioBurstRateLimitGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioBeaconIntervalSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioBeaconIntervalGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioDTIMPeriodSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioDTIMPeriodGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioFragmentationThresholdSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioFragmentationThresholdGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioRTSThresholdSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioRTSThresholdGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioShortRetryLimitSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioShortRetryLimitGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioLongRetryLimitSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioLongRetryLimitGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioMaxTransmitLifetimeSet (L7_uint32 profileId,
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
L7_RC_t usmDbwdmAPProfileRadioMaxTransmitLifetimeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioMaxReceiveLifetimeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioMaxReceiveLifetimeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioChannelBandwidthSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioChannelBandwidthGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioPrimaryChannelSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioPrimaryChannelGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioProtectionSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioProtectionGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioGuardIntervalSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioGuardIntervalGet (L7_uint32 profileId,
                            L7_uint32 radioIndex,
                            L7_WDM_RADIO_GUARD_INTERVAL_t *guardInterval);


/*********************************************************************
*
* @purpose  Set the 802.11n Space Time Block Code Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 stbc @b{(input)} Space Time Block Code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Accepts L7_ENABLE or L7_DISABLE
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioSTBCSet (L7_uint32 profileId,
                                       L7_uint32 radioIndex,
                                       L7_uint32 stbc);

/*********************************************************************
*
* @purpose  Get the 802.11n Space Time Block Code Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *stbc @b{(output)} Space Time Block Code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioSTBCGet (L7_uint32 profileId,
                                       L7_uint32 radioIndex,
                                       L7_uint32 *stbc);

/*********************************************************************
*
* @purpose  Set the Multicast Tx Rate for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 raet @b{(input)} Multicast Tx Rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioMcastTxRateSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioMcastTxRateGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVoiceAcmModeSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVoiceAcmModeGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVideoAcmModeSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVideoAcmModeGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVoiceAcmLimitSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVoiceAcmLimitGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVoiceAcmLimitCheck(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVideoAcmLimitSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVideoAcmLimitGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecVideoAcmLimitCheck(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecRoamReserveLimitSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecRoamReserveLimitGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecRoamReserveLimitCheck(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecApInactivityTimeoutSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecApInactivityTimeoutGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecStaInactivityTimeoutSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecStaInactivityTimeoutGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecLegacyWmmQueueMapModeSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioTspecLegacyWmmQueueMapModeGet(L7_uint32 profileId,
                                                            L7_uint32 radioIndex,
                                                            L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the Automatic Power Save Delivery Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 apsd @b{(input)} APSD
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioAPSDSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 apsd);

/*********************************************************************
*
* @purpose  Get the Automatic Power Save Delivery Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *apsd @b{(input)} APSD
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioAPSDGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *apsd);

/*********************************************************************
*
* @purpose  Set the No ACK Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 noAck @b{(input)} No ACK
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioNoACKSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 noAck);

/*********************************************************************
*
* @purpose  Get the No ACK Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *noAck @b{(input)} No ACK
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioNoACKGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioMaxClientsSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioMaxClientsGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAutoChannelModeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAutoChannelModeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioDot11aLimitChannelSelectionSet (
                            L7_uint32 profileId, L7_uint32 radioIndex,
                            L7_uint32 limitChannelSel);

/*********************************************************************
*
* @purpose  Get the 802.11a Limit Channel Selection for the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index
* @param    L7_uint32 *limitChannelSel @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioDot11aLimitChannelSelectionGet (
                            L7_uint32 profileId, L7_uint32 radioIndex,
                            L7_uint32 *limitChannelSel);




/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} channel to query
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
L7_RC_t 
usmDbWdmAPProfileRadioChannelEligibleGet (L7_uint32 profileId,
                                           L7_uint32 radioIndex,
                                           L7_uchar8  channel);


/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
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
L7_RC_t 
usmDbWdmAPProfileRadioChannelEligibleNextGet (L7_uint32 profileId,
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
L7_RC_t 
usmDbWdmAPProfileRadioChannelSupportedGet(L7_uint32 profileId,
                                          L7_uint32 radioIndex,
                                          L7_uchar8  channel);


/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} channel to start from
* @param    L7_uchar8  *pNextChannel @b{(output)} next channel
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
L7_RC_t 
usmDbWdmAPProfileRadioChannelSupportedNextGet(L7_uint32 profileId,
                                              L7_uint32 radioIndex,
                                              L7_uchar8  channel,
                                              L7_uchar8  *pNextChannel);

/*********************************************************************
*
* @purpose  Change Channel Availability on AP Profile Radio
*
* @param    L7_uint32 profileId  @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar8  channel    @b{(input)} channel to set
* @param    L7_BOOL   toBeEnabled @b{(input)} L7_TRUE or L7_FALSE
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
L7_RC_t 
usmDbWdmAPProfileRadioChannelEligibleSet (L7_uint32 profileId,
                                           L7_uint32 radioIndex,
                                           L7_uchar8  channel,
                                           L7_BOOL   toBeEnabled);


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
L7_RC_t usmDbWdmAPProfileRadioAutoPowerModeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAutoPowerModeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioDefaultTxPowerSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioDefaultTxPowerGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *defaultTxPower);

/*********************************************************************
*
* @purpose  Verify a rate entry for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioRateEntryGet(L7_uint32  profileId,
                                           L7_uint32  radioIndex, 
                                           L7_uint32  rate);

/*********************************************************************
*
* @purpose  Get the next rate entry for the AP profile radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 *rate @b{(output)} Supported Rate
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioRateEntryNextGet(L7_uint32  profileId,
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
L7_RC_t usmDbWdmAPProfileRadioSuppRateSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioSuppRateGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAdvtRateSet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 rate,
                            L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get if Data Rate Value is enabled in Advertised List in the AP Profile Radio.
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 rate @b{(input)} Advertised Rate
* @param    L7_uint32 *mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioAdvtRateGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmPhyModeRateEntryGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
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
L7_RC_t usmDbWdmPhyModeRateEntryNextGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Get the rate entry count from the valid list of rate entries
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode @b{(input)} PHY Mode
* @param    L7_uint32 *rate @b{(output)} numRates
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmPhyModeNumRatesGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uint32 *numRates);

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
L7_RC_t usmDbWdmPhyModeSuppRateGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
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
L7_RC_t usmDbWdmPhyModeAdvtRateGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
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
L7_RC_t usmDbWdmPhyModeMcastTxRateEntryGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
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
L7_RC_t usmDbWdmPhyModeMcastTxRateEntryNextGet (L7_WDM_RADIO_PHY_MODE_t phyMode,
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
L7_RC_t usmDbWdmAPProfileRadioWMMModeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioWMMModeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioLoadBalancingModeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioLoadBalancingModeGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioUtilizationSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioUtilizationGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioDataSnoopModeGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_uint32 *dataSnoopMode);

/*********************************************************************
*
* @purpose  Set the Data Snooping Mode for the AP Profile Radio.
*
* @param    L7_uint32 profileId      @b{(input)}  AP Profile ID
* @param    L7_uint32 radioIndex     @b{(input)}  Radio Index of the AP profile
* @param    L7_uint32 dataSnoopMode  @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioDataSnoopModeSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioEdcaEntryGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioEdcaEntryNextGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaAifsSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaAifsGet (L7_uint32 profileId,
                            L7_uint32 radioIndex, L7_int32 queueId,
                            L7_ushort16 *aifs);

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
L7_RC_t usmDbWdmAPProfileRadioEdcaCWNextGet (L7_ushort16 *cw);

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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaCWMinSet (L7_uint32 profileId,
                        L7_uint32 radioIndex, L7_int32 queueId,
                        L7_ushort16 cwmin);

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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaCWMinGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaCWMaxSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaCWMaxGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaBurstLengthSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioAPEdcaBurstLengthGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaAifsSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaAifsGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaCWMinSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaCWMinGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaCWMaxSet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaCWMaxGet (L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaTxOpLimitSet (L7_uint32 profileId, 
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
L7_RC_t usmDbWdmAPProfileRadioStationEdcaTxOpLimitGet (L7_uint32 profileId, 
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
L7_RC_t usmDbWdmAPProfileVAPEntryGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileVAPEntryNextGet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileVAPModeSet(L7_uint32         profileId,
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
L7_RC_t usmDbWdmAPProfileVAPNetworkIDSet(L7_uint32 profileId,
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
L7_RC_t usmDbWdmAPProfileVAPModeGet(L7_uint32          profileId,
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
L7_RC_t usmDbWdmAPProfileVAPNetworkIDGet(L7_uint32  profileId,
                                         L7_uint32  radioId,
                                         L7_uint32  vapId,
                                         L7_uint32 *networkId);

/*********************************************************************
* @purpose  Get the AP Profile Modified Flag value
*
* @param    L7_uint32  profileId    @b{(input)}  AP Profile ID
* @param    L7_uchar8  *value       @b{(output)} AP profile Modified flag value
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmAPProfileModifiedFlagGet (L7_uint32 profileId,
                                          L7_uchar8 *value);

/*********************************************************************
* @purpose  Set the Station Isolation Mode for the AP Profile Radio
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uint32 Mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioStationIsolationSet (L7_uint32 profileId,
                                                   L7_uint32 radioIndex, 
                                                   L7_uint32 Mode);

/*********************************************************************
* @purpose  Get the Station Isolation Mode for the AP Profile Radio
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
* @param    L7_uint32 radioIndex @b{(input)} Radio Index of the AP profile
* @param    L7_uchar Mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmAPProfileRadioStationIsolationGet (L7_uint32 profileId,
                                                   L7_uint32 radioIndex, 
                                                   L7_uint32 *Mode);

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
L7_RC_t usmDbWdmAPProfileHardwareTypeSet(L7_uint32 profileId, L7_uint32 hwType);

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
L7_RC_t usmDbWdmAPProfileHardwareTypeGet(L7_uint32 profileId, L7_uint32 *hwType);

/*********************************************************************
*
* @purpose  Checks if the specified radio Index is valid for 
*           the specified profile.
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
L7_RC_t usmDbWdmAPProfileRadioIndexGet(L7_uint32  profileId,L7_uint32 radioIndex);

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
L7_RC_t usmDbWdmAPProfileVlanIdSet (L7_uint32 profileId, L7_ushort16 vlan_id);

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
L7_RC_t usmDbWdmAPProfileVlanIdGet (L7_uint32 profileId, L7_ushort16 *vlan_id);

#if 0 /* Not supported */
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
L7_RC_t usmDbWdmAPProfileRadioAntennaDivModeSet(L7_uint32 profileId,
                                        L7_uint32 radioIndex,
                                        L7_uint32 antDiv);
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
L7_RC_t usmDbWdmAPProfileRadioAntennaSelSet(L7_uint32 profileId,
                                           L7_uint32 radioIndex,
                                           L7_uint32 antSel);
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
L7_RC_t usmDbWdmAPProfileRadioAntennaDivModeGet(L7_uint32 profileId,
                                        L7_uint32 radioIndex,
                                        L7_uint32 *antDiv);

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
L7_RC_t usmDbWdmAPProfileRadioAntennaSelGet(L7_uint32 profileId,
                                           L7_uint32 radioIndex,
                                           L7_uint32 *antSel);
#endif /* Not supported */

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
L7_RC_t usmDbWdmAPProfileRadioCountrySupportForFortyMHz(L7_uint32 profileId,
                           L7_uint32 radioIndex);

#endif /* INCLUDE_USMDB_WDM_AP_PROFILE_API_H */
