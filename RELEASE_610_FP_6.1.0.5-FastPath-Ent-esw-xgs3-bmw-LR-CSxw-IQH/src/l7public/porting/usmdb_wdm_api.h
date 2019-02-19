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
#ifndef INCLUDE_USMDB_WDM_API_H
#define INCLUDE_USMDB_WDM_API_H

#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "transfer_exports.h"

/*********************************************************************
*
* @purpose  Set the global admin mode for the Wireless Switch (WS).
*
* @param    mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Set the global admin mode for the Wireless Switch (WS).
*
* @param    mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Get the global admin mode status for the Wireless Switch.
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value,
*                                        of type L7_WDM_WS_MODE_STATUS_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSModeStatusGet(L7_WDM_WS_MODE_STATUS_t *mode);

/*********************************************************************
*
* @purpose  Get the global admin mode disable reason for the Wireless Switch.
*
* @param    L7_WDM_WS_MODE_REASON_t *reason @b{(output)} location to store the reason value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSModeReasonGet(L7_WDM_WS_MODE_REASON_t *reason);


/*********************************************************************
*
* @purpose  Set the global AP code download file name.
*
* @param    L7_uint32 imageID @b{(input)} image type ID
* @param    L7_char8 *name @b{(input)} file name, L7_MAX_FILENAME
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadFileNameSet(L7_uint32 imageID, L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the global AP code download file path.
*
* @param    L7_uint32 type @b{(input)} image type ID.
* @param    L7_char8 *path @b{(input)} file path, L7_MAX_FILEPATH.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadFilePathSet(L7_uint32 imageID, L7_char8 *path);


/*********************************************************************
*
* @purpose  Check if an entry exists in AP image table.
*
* @param    L7_uint32 imageID @b{(input)} AP image Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPImageTypeIDGet(L7_uint32 imageID);


/*********************************************************************
*
* @purpose  Get next AP image type ID entry from AP image table.
*
* @param    L7_uint32  imageID @b{(input)} AP image Type ID
* @param    L7_uint32 *nextImageID @b{(input)} Next AP image Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPImageTypeIDNextGet(L7_uint32 imageID,L7_uint32 *nextImageID);

/*********************************************************************
*
* @purpose  Get image type description from AP image capability table.
*
* @param    L7_uint32 imageID @b{(input)} AP image Type ID
* @param    L7_char8  *desc @b{(input)} AP image Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPImageTypeDescGet(L7_uint32 imageID, L7_char8 *desc);


/*********************************************************************
*
* @purpose  Get the global AP code download file name.
*
* @param    L7_uint32 imageID @b{(input)} image type ID
* @param    L7_char8 *name @b{(output)} location to store file name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments name buffer must be at least L7_MAX_FILENAME+1.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadFileNameGet(L7_uint32 imageID,L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the global AP code download file path.
*
* @param    L7_uint32 type @b{(input)} image type ID
* @param    L7_char8 *path @b{(output)} location to store file path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments path buffer must be at least L7_MAX_FILEPATH+1.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadFilePathGet(L7_uint32 imageID, L7_char8 *path);

/*********************************************************************
*
* @purpose  Set the global AP code download server IP address.
*
* @param    L7_IP_ADDR_t addr @b{(input)} server IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadServerSet(L7_IP_ADDR_t addr);

/*********************************************************************
*
* @purpose  Get the global AP code download server IP address.
*
* @param    L7_IP_ADDR_t *addr @b{(output)} location to store IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadServerGet(L7_IP_ADDR_t *addr);

/*********************************************************************
*
* @purpose  Set the global AP code download group size.
*
* @param    L7_uchar8 size @b{(input)} group size 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This indicates the number of APs to update at a time.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadGroupSizeSet(L7_uchar8 size);

/*********************************************************************
*
* @purpose  Get the global AP code download group size.
*
* @param    L7_uchar8 *size @b{(output)} group size 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This indicates the number of APs to update at a time.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadGroupSizeGet(L7_uchar8 *size);

/*********************************************************************
*
* @purpose  Set the global AP code download type.
*
* @param    L7_uint32 type @b{(input)} current download type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadTypeSet(L7_uint32 type);

/*********************************************************************
*
* @purpose  Get the global AP code download type.
*
* @param    L7_uint32 *type @b{(output)} current download type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadTypeGet(L7_uint32 *type);

/*********************************************************************
*
* @purpose  Get the global AP code download status.
*
* @param    L7_WDM_ACTION_STATUS_t *status @b{(output)} current status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadStatusGet(L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Start the global AP code download.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress or there are no managed APs
*
* @comments This also resets all the status counters.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadStart();

/*********************************************************************
*
* @purpose  Start the code download on the specified managed APs.
*
* @param    L7_enetMacAddr_t  apList  @b{(input)} AP mac addresses
* @param    L7_uint32         numAPs  @b{(input)} number of APs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This also resets all the status counters.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadMultipleStart(L7_enetMacAddr_t *apList, L7_uint32 numAPs);

/*********************************************************************
*
* @purpose  Abort the code download on managed AP(s).
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadAbort(void);

/*********************************************************************
*
* @purpose  Get the total number of APs in the current download request.
*
* @param    L7_uchar8 *total @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadTotalGet(L7_uchar8 *total);

/*********************************************************************
*
* @purpose  Get the number of APs that have successfully downloaded
*             code for the current download request.
*
* @param    L7_uchar8 *count @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadSuccessCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the number of APs that have failed to download
*             code for the current download request.
*
* @param    L7_uchar8 *count @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadFailCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the number of managed APs on which the code download
*           is aborted.
*
* @param    L7_uchar8 *count  @b{(output)} pointer to the count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadAbortCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the total number of reset count in the download request.
*
* @param    L7_uchar8  *count  @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPCodeDownloadResetCountGet(L7_uchar8 *count);

L7_RC_t usmDbWdmAPAuthenticationSet(L7_BOOL flag);
L7_RC_t usmDbWdmAPValidationSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the AP client QoS mode of the Wireless switch server.
*
* @param    mode        @b{(input)} AP client QoS mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Setting the mode to L7_ENABLE enables AP client QoS in the Wireless switch.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPClientQosModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the AP client QoS mode of the Wireless switch server.
*
* @param    *mode       @b{(output)} Ptr to AP client QoS mode output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPClientQosModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Determine if phy mode is supported for configured country.
*
* @param    L7_WDM_RADIO_PHY_MODE_t  mode  physical radio mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, not supported
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSRadioPhyModeSupported(L7_WDM_RADIO_PHY_MODE_t mode);

/*********************************************************************
*
* @purpose  Determine if channel is supported based on configured country.
*
* @param    L7_uchar8    channel     @b{(input)}  channel.
* @param    L7_uint32   *phyModesMask    @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired @b{(output)} Indicates if radar detection
* @param                               @b{(output)} required or not.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, channel not supported.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSChannelEntryGet(L7_uchar8                channel,
                                  L7_uint32               *phyModesMask,
                                  L7_BOOL                 *rdrDetRequired);

/*********************************************************************
*
* @purpose  Get list of supported channels based on configured country.
*
* @param    L7_uchar8    channel     @b{(input)}  previous channel, 0 to start.
* @param    L7_uchar8   *nextChannel @b{(output)} next supported channel.
* @param    L7_uint32   *phyModesMask    @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired @b{(output)} Indicates if radar detection
* @param                               @b{(output)} Required or not.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, end of list
*
* @comments channel list is ordered based on channel number.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSChannelEntryNextGet(L7_uchar8                channel,
                                      L7_uchar8               *nextChannel,
                                      L7_uint32               *phyModesMask,
                                      L7_BOOL                 *rdrDetRequired);

/*********************************************************************
*
* @purpose  Get the first channel from the all-country aggregate channel list.
*
* @param    L7_uchar8   *channel @b{(output)} first supported channel.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSChannelListEntryGetFirst(L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the next channel from the all-country aggregate channel list.
*
* @param    L7_uchar8    channel     @b{(input)}  previous channel, 0 to start.
* @param    L7_uchar8   *nextChannel @b{(output)} next supported channel.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, end of list
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSChannelListEntryNextGet(L7_uchar8 channel, L7_uchar8 *nextChannel);

/*********************************************************************
*
* @purpose  Determine if channel is supported based on configured country for a PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode     @b{(input)} PHY Mode
* @param    L7_uchar8    channel     @b{(input)}  channel.
* @param    L7_uint32   *phyModesMask @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired @b{(output)} Indicates if radar detection
* @param                               @b{(output)} required or not.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, channel not supported.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSPhyModeChannelEntryGet(L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uchar8 channel, L7_uint32 *phyModesMask,
                            L7_BOOL *rdrDetRequired);

/*********************************************************************
*
* @purpose  Get list of supported channels based on configured country for a PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode     @b{(input)} PHY Mode
* @param    L7_uchar8    channel     @b{(input)}  previous channel, 0 to start.
* @param    L7_uchar8   *nextChannel @b{(output)} next supported channel.
* @param    L7_uint32   *phyModesMask @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired @b{(output)} Indicates if radar detection
* @param                               @b{(output)} is required or not.

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, end of list
*
* @comments channel list is ordered based on channel number for a PHY Mode.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSPhyModeChannelEntryNextGet(L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uchar8 channel, L7_uchar8 *nextChannel,
                            L7_uint32 *phyModesMask, L7_BOOL *rdrDetRequired);

/*********************************************************************
*
* @purpose  Look up a valid country entry.
*
* @param    L7_uchar8 index @b{(input)}  country code index
* @param    L7_char8 *code  @b{(output)} country code
* @param    L7_char8 *str  @b{(output)}  country string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid index
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCountryCodeEntryGet(L7_uchar8 index,
                                      L7_char8 *code,
                                      L7_char8 *str);

/*********************************************************************
*
* @purpose  Get next valid country entry.
*
* @param    L7_uchar8 index @b{(input)}  country code index
* @param    L7_char8 *next  @b{(output)} next country code index
* @param    L7_char8 *code  @b{(output)} country code
*                                        (L7_WDM_COUNTRY_CODE_SIZE + NULL byte)
* @param    L7_char8 *str   @b{(output)} country string
*                                        (L7_WDM_COUNTRY_STRING_LENGTH_MAX + NULL byte)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, no more entries
*
* @comments index == 0 returns first entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCountryCodeEntryNextGet(L7_uchar8  index,
                                          L7_uchar8 *next,
                                          L7_char8  *code,
                                          L7_char8  *str);

/*********************************************************************
*
* @purpose  Look up a country code, return entry index if valid.
*
* @param    L7_char8  *code  @b{(input)}  country code
* @param    L7_uchar8 *index @b{(output)} country code index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid index
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCountryCodeIndexGet(const L7_char8  *code,
                                      L7_uchar8 *index);

/*********************************************************************
*
* @purpose  Set the country code for the Wireless Switch (WS).
*
* @param    L7_uchar8 index @b{(input)} country code index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, wireless system is enabled
*
* @comments country configuration can only be modified when
*           the wireless system is disabled.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCountryCodeSet(L7_uchar8 index);

/*********************************************************************
*
* @purpose  Get the country code for the Wireless Switch (WS).
*
* @param    L7_uchar8 *index @b{(output)} location to store the country index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCountryCodeGet(L7_uchar8 *index);
/*********************************************************************
*
* @purpose  Get the regulatory domains for the Wireless Switch (WS).
*
* @param    L7_char8 *twoGHz @b{(output)} 2GHz regulatory domain
* @param    L7_char8 *fiveGHz @b{(output)} 5GHz regulatory domain
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Regulatory domains are returned based on the country code
* @comments configured
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSRegDomainsGet(L7_char8 *twoGHz, L7_char8* fiveGHz);

L7_RC_t usmDbWdmIPListEntryAdd(L7_IP_ADDR_t addr);
L7_RC_t usmDbWdmIPListEntryDelete(L7_IP_ADDR_t addr);
L7_RC_t usmDbWdmIPListEntryPurge();
L7_RC_t usmDbWdmWSIPAddrGet(L7_IP_ADDR_t *ipAddr);
L7_RC_t usmDbWdmL2DiscoverySet(L7_IP_ADDR_t addr);
L7_RC_t usmDbWdmVLANListEntryAdd(L7_ushort16 vlanId);
L7_RC_t usmDbWdmVLANListEntryDelete(L7_ushort16 vlanId);
L7_RC_t usmDbWdmVLANListEntryPurge();
L7_RC_t usmDbWdmIPDiscoverySet(L7_BOOL mode);
L7_RC_t usmDbWdmPeerGroupSet(L7_uint32 id);
L7_RC_t usmDbWdmTunnelMTUSet(L7_uint32 mtu);

/*********************************************************************
*
* @purpose Set the client failure age timeout value of the Wireless switch server.
*
* @param    L7_ushort16 age @b{(input)} new client failure age timeout value,
* @param                                ranges 0-168 hours
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSClientFailureAgeSet(L7_ushort16 age);

/*********************************************************************
*
* @purpose Set the client roam age timeout value of the Wireless switch server.
*
* @param    L7_ushort16 age @b{(input)} new client roam age timeout value,
* @param                                ranges 0-168 hours
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSClientRoamAgeSet(L7_ushort16 age);

/*********************************************************************
*
* @purpose Set the RF scan age timeout value of the Wireless switch server.
*
* @param    L7_ushort16 age @b{(input)} new RF scan age timeout value,
* @param                                ranges 0-168 hours
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSRFScanAgeSet(L7_ushort16 age);

/*********************************************************************
*
* @purpose Set the Ad Hoc age timeout of the Wireless switch server.
*
* @param    L7_ushort16 age @b{(input)} new Ad Hoc age timeout value,
* @param                                ranges 0-168 hours
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSAdHocAgeSet(L7_ushort16 age);

/*********************************************************************
*
* @purpose Set the AP failure age timeout value of the Wireless switch server.
*
* @param    L7_ushort16 age @b{(input)} new AP failure age timeout value,
* @param                                ranges 0-168 hours
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSAPFailureAgeSet(L7_ushort16 age);

/*********************************************************************
*
* @purpose Set the Detected Clients age timeout value of the Wireless
*          switch server.
*
* @param    L7_ushort16 age @b{(input)} new Detected clients age timeout
*                                       value. ranges 0-168 hours
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSDetectedClientsAgeSet(L7_ushort16 age);

/*********************************************************************
*
* @purpose Get the Ad Hoc age timeout of the Wireless switch server.
*
* @param    L7_ushort16 *age @b{(output)} location to store the Ad Hoc
* @param                                  age timeout value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSAdHocAgeGet(L7_ushort16 *age);

/*********************************************************************
*
* @purpose Get the AP failure age timeout value of the Wireless switch server.
*
* @param L7_ushort16 *age @b{(output)} location to store the AP failure age
* @param                               timeout value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSAPFailureAgeGet(L7_ushort16 *age);

/*********************************************************************
*
* @purpose Get the client failure age timeout value of the Wireless switch.
*
* @param L7_ushort16 *age @b{(output)} location to store the client failure age
* @param                               timeout value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSClientFailureAgeGet(L7_ushort16 *age);

/*********************************************************************
*
* @purpose Get the client roam age timeout value of the Wireless switch server.
*
* @param L7_ushort16 *age @b{(output)} location to store the client failure age
* @param                               timeout value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSClientRoamAgeGet(L7_ushort16 *age);

/*********************************************************************
*
* @purpose Get the RF scan age timeout value of the Wireless switch server.
*
* @param L7_ushort16 *age @b{(output)} location to store the RF scan age
*                                      timeout value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSRFScanAgeGet(L7_ushort16 *age);

/*********************************************************************
*
* @purpose Get the Detected Clients age timeout value of the Wireless
*          switch server.
*
* @param L7_ushort16 *age @b{(output)} location to store the detected
*                                      clients age timeout value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSDetectedClientsAgeGet(L7_ushort16 *age);


L7_RC_t usmDbWdmAPAuthenticationGet(L7_BOOL *flag);
L7_RC_t usmDbWdmAPValidationGet(L7_uint32 *value);
L7_RC_t usmDbWdmIPListEntryGet(L7_IP_ADDR_t addr);

/*********************************************************************
*
* @purpose  Get the status of the discovered IP in the list for the Wireless
*           Switch (WS).
*
* @param    L7_IP_ADDR_t        addr   @b{(input)}  IP Address
* @param    L7_WDM_IP_STATUS_t *status @b{(output)} status for configured IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIPListStatusGet(L7_IP_ADDR_t addr, L7_WDM_IP_STATUS_t *status);

L7_RC_t usmDbWdmIPListEntryNextGet(L7_IP_ADDR_t add,
                L7_IP_ADDR_t *nextAddr);
L7_RC_t usmDbWdmL2DiscoveryGet(L7_BOOL *mode);
L7_RC_t usmDbWdmRFDiscoveryGet(L7_BOOL *mode);
L7_RC_t usmDbWdmVLANListEntryGet(L7_ushort16 vlanId);
L7_RC_t usmDbWdmVLANListEntryNextGet(L7_ushort16 vlanId,
                L7_ushort16 *nextVlanId);
L7_RC_t usmDbWdmRogueAPTrapModeGet(L7_BOOL *mode);
L7_RC_t usmDbWdmAPFailureTrapModeGet(L7_BOOL *mode);
L7_RC_t usmDbWdmIPDiscoveryGet(L7_BOOL *mode);
L7_RC_t usmDbWdmAPStateTrapModeGet(L7_BOOL *mode);
L7_RC_t usmDbWdmClientFailureTrapModeGet(L7_BOOL *mode);
L7_RC_t usmDbWdmPeerGroupGet(L7_uint32 *id);
L7_RC_t usmDbWdmRFScanTrapModeGet(L7_BOOL *mode);
L7_RC_t usmDbWdmClientStateTrapModeGet(L7_BOOL *mode);
L7_RC_t usmDbWdmPeerSwitchesGet(L7_ushort16 *value);
L7_RC_t usmDbWdmStandaloneAPsGet(L7_ushort16 *value);
/*********************************************************************
*
* @purpose Get the number of clients using l2 distributed tunneling.
*
* @param    L7_uint32 *value @b{(output)} location to store the number
*                                         of wireless clients.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelClientsGet(L7_uint32 *value);
L7_RC_t usmDbWdmRogueAPsGet(L7_ushort16 *value);
L7_RC_t usmDbWdmUnknownAPsGet(L7_ushort16 *value);
L7_RC_t usmDbWdmUtilizationWLANGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose Get the TSPEC total voice TS status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTotalVoiceTsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total video TS status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTotalVideoTsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS clients status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTotalTsClientsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS roaming clients status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTotalTsRoamClientsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the total number of WS managed APs in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTotalAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are authenticated, configured,
*           and have an active connection with WS.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that were previously authenticated
*           and managed, but currently do not have connection with WS.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmConnectionFailedAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are connected to the switch,
*           but are not completely configured.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDiscoveredAPsGet(L7_ushort16 *value);


/*********************************************************************
*
* @purpose  Get the maximum number of APs WS can manage.
*
* @param    L7_ushort16  *value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmMaxAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the total number of clients in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTotalClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of authentictaed clients in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAuthenticatedClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the maximum number of APs WS can manage.
*
* @param    L7_ushort16  *value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmMaxAssocClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose Get the managed APs (with managed status) count of the Wireless switch.
*
* @param    L7_ushort16 *value @b{(output)} location to store the count
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPsWithManagedStatusGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Set the transmitted WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 bytes @b{(input)} WLAN bytes transmitted.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANBytesTxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the received WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN received
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANBytesRxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store transmitted
*                                         WLAN packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANPktsTxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the received WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store the received
*                                         WLAN packets value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANPktsRxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Set the dropped transmit WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN dropped transmit
*                                         bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANDropBytesTxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN dropped receive
*                                         bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANDropBytesRxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store WLAN dropped transmit
*                                         packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANDropPktsTxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store the WLAN dropped receive
*                                         packets value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWLANDropPktsRxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the WLAN TSPECs accepted value for the Wireless Switch (WS).
*
* @param    L7_uint32 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTsAcceptedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPECs rejected value for the Wireless Switch (WS).
*
* @param    L7_uint32 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTsRejectedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN roaming TSPECs accepted value for the Wireless Switch (WS).
*
* @param    L7_uint32 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTsRoamAcceptedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN roaming TSPECs rejected value for the Wireless Switch (WS).
*
* @param    L7_uint32 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecTsRoamRejectedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Rx packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecPktsRxGet(L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Tx packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecPktsTxGet(L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Rx bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecBytesRxGet(L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Tx bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *value @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmTspecBytesTxGet(L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose Get Tunnel MTU value for the wireless system.
*
* @param   L7_uint32           *mtu @b{(output)} Tunnel MTU
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSTunnelMTUGet(L7_uint32 *mtu);

/*********************************************************************
*
* @purpose Set the TSPEC client violation report interval value of the Wireless switch.
*
* @param L7_uint32 val @b{(input)} value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSTspecViolationReportIntervalSet(L7_uint32 val);

/*********************************************************************
*
* @purpose Get the TSPEC client violation report interval value of the Wireless switch.
*
* @param L7_uint32 *val @b{(input)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSTspecViolationReportIntervalGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Enable/Disable a trap group for the wireless system.
*
* @param   L7_WDM_TRAP_FLAGS_t  trap @b{(input)} trap group flag
* @param   L7_uint32            mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSTrapModeSet(L7_WDM_TRAP_FLAGS_t flag,
                              L7_uint32           mode);

/*********************************************************************
*
* @purpose Enable/Disable a trap group for the wireless system.
*
* @param   L7_WDM_TRAP_FLAGS_t  trap @b{(input)} trap group flag
* @param   L7_uint32           *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSTrapModeGet(L7_WDM_TRAP_FLAGS_t flag,
                              L7_uint32          *mode);

/*********************************************************************
*
* @purpose Enable/Disable a peer config push group for the wireless system.
*
* @param   L7_WDM_CONFIG_PUSH_MASK_t  mask @b{(input)} config push mask
* @param   L7_uint32  mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushSet(L7_WDM_CONFIG_PUSH_MASK_t mask,
                                L7_uint32                 mode);

/*********************************************************************
*
* @purpose function is invoked to Get mask of a config push group
*
* @param   L7_WDM_CONFIG_PUSH_MASK_t  mask @b{(input)} config group mask
* @param   L7_uint32           *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushGet(L7_WDM_CONFIG_PUSH_MASK_t mask,
                                L7_uint32                *mode);

/*********************************************************************
*
* @purpose  Start the config push to all peer switches.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress or there are no peer switches
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushStart();


/*********************************************************************
*
* @purpose  Start the config push to given peer switch.
*
* @param    L7_IP_ADDR_t peerSwitchIP   @b{(input)} Peer switch IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress or if peer switch not present
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchConfigPushStart(L7_IP_ADDR_t ipAddr);


/*********************************************************************
*
* @purpose  Start the config push to the specified peer switches.
*
* @param    L7_IP_ADDR_t *ipList   @b{(input)} Peer switch IP Address
* @param    L7_uint32    numWs    @b{(input)} number of peer switches

* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushMultipleStart(L7_IP_ADDR_t *ipList ,L7_uint32 numWs);


/*********************************************************************
*
* @purpose  Get peer switches count config push has been requested.
*
* @param    L7_uchar8 count @b{(output)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushTotalCountGet(L7_uchar8 *count);


/*********************************************************************
*
* @purpose  Get the number of peer switches that failed config
*           push request.
*
* @param    L7_uchar8 count @b{(output)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushFailCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the number of peer switches that succeded config
*           push request.
*
* @param    L7_uchar8 count @b{(output)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushSuccCountGet(L7_uchar8 *count);


/*********************************************************************
*
* @purpose function to Get last peer switch IP address that pushed config
*          to the switch
*
* @param    ipAddr    @b{(output)} Wireless system IP Address
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushRxPeerSwitchIPAddrGet(L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose function to Get time of config push from peer switch
*
* @param    rxTime    @b{(output)} config receive time
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushRxTimeGet(L7_uint32 *rxTime);

/*********************************************************************
*
* @purpose function to Get status of config push received from peer switch
*
* @param    rxStatus    @b{(output)} config push receive status
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushRxStatusGet(L7_WDM_CONFIG_PUSH_RX_STATUS_t *rxStatus);


/*********************************************************************
*
* @purpose function to Get status of config push receive from peer switch
*
* @param    rxMask    @b{(output)} config push receive mask
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushRxMaskGet(L7_uint32 *rxMask);


/*********************************************************************
*
* @purpose function to Get status of config push send to peer switch
*
* @param    txStatus    @b{(output)} config push send status
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSConfigPushReqStatusGet(L7_WDM_CONFIG_PUSH_SEND_STATUS_t *txStatus);

/*********************************************************************
*
* @purpose Reset all wireless statistics.
*
* @param
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, clear in progress.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmStatisticsClear();

/*****************************************************************************
*
* @purpose  Get the global status for a statistics clear operation.
*
* @param    L7_WDM_ACTION_STATUS_t *status @b{(output)} current clear status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*******************************************************************************/
L7_RC_t usmDbWdmStatisticsClearStatusGet(L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Add AP MAC Address to the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ALREADY_CONFIGURED
* @returns  L7_TABLE_IS_FULL
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPEntryAdd (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete AP MAC Address from the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPEntryDelete (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete all AP MAC Addresses from the Local AP Database
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPEntryDeleteAll ();

/*********************************************************************
*
* @purpose  Find if the AP MAC Address exists in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPEntryGet (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Find the next AP MAC Address in the Local AP Database
*
* @param    L7_enetMacAddr_t *macAddr @b{(input)} AP MAC Address
* @param    L7_enetMacAddr_t *macAddr @b{(output)} AP MAC Address of the next AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPEntryNextGet (L7_enetMacAddr_t *macAddr);

/*********************************************************************
*
* @purpose  Set the Location Information of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *location @b{(input)} Location String for the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPLocationSet (L7_enetMacAddr_t macAddr, L7_char8 *location);

/*********************************************************************
*
* @purpose  Get the Location Information of the AP from the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *location @b{(output)} Location String of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPLocationGet (L7_enetMacAddr_t macAddr, L7_char8 *location);

/*********************************************************************
*
* @purpose  Set the Authentication Password of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP MAC Address
* @param    L7_char8        *password    @b{(input)} Authentication Password of the AP
* @param    L7_BOOL          encrypted   @b{(input)} L7_TRUE if password in encrypted format
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPPasswordSet(L7_enetMacAddr_t macAddr, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Get the Authentication Password of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP MAC Address
* @param    L7_char8        *password    @b{(input)} Authentication Password of the AP
* @param    L7_BOOL          encrypted   @b{(input)} L7_TRUE if password in encrypted format
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPPasswordGet(L7_enetMacAddr_t macAddr, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Check if the Authentication Password is configured or not
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_BOOL *pVal @b{(input)}  Is password configured or not.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbAPPasswordConfigured (L7_enetMacAddr_t macAddr, L7_BOOL *pVal);

/*********************************************************************
*
* @purpose  Set the Managed Mode of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_WDM_AP_MANAGED_MODE mode @b{(input)} Managed Mode of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPModeSet (L7_enetMacAddr_t macAddr, L7_WDM_AP_MANAGED_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the Managed Mode of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_WDM_AP_MANAGED_MODE *mode @b{(output)} Managed Mode of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPModeGet (L7_enetMacAddr_t macAddr, L7_WDM_AP_MANAGED_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the Profile ID of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 profileId @b{(input)} Profile ID of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileIDSet (L7_enetMacAddr_t macAddr, L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Get the Profile ID of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 *profileId @b{(output)} Profile ID of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProfileIDGet (L7_enetMacAddr_t macAddr, L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Set a fixed channel per radio for an AP database entry.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8        channel    @b{(input)}  0 (Auto) or fixed channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPRadioChannelSet(L7_enetMacAddr_t macAddr,
                                  L7_uchar8        radioIndex,
                                  L7_uchar8        channel);

/*********************************************************************
*
* @purpose  Get a fixed channel per radio for an AP database entry.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8       *channel    @b{(output)} 0 (Auto) or fixed channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPRadioChannelGet(L7_enetMacAddr_t macAddr,
                                  L7_uchar8        radioIndex,
                                  L7_uchar8       *channel);

/*********************************************************************
*
* @purpose  Set fixed power for a physical AP in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8        power      @b{(input)}  0 (Auto) or fixed power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPRadioPowerSet(L7_enetMacAddr_t macAddr,
                                L7_uchar8        radioIndex,
                                L7_uchar8        power);

/*********************************************************************
*
* @purpose  Set fixed power for a physical AP in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8       *power      @b{(output)}  0 (Auto) or fixed power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPRadioPowerGet(L7_enetMacAddr_t macAddr,
                                L7_uchar8        radioIndex,
                                L7_uchar8       *power);


/*********************************************************************
*
* @purpose  En/disable RRM  for a physical AP in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8        power      @b{(input)}  0 (Auto) or fixed power
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
usmDbWdmAPRadioResourceMeasEnabledSet(L7_enetMacAddr_t macAddr,
				      L7_uchar8        radioIndex,
				      L7_BOOL          bEnable);


/*********************************************************************
*
* @purpose  Get status of RRM for a physical AP in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8       *power      @b{(output)}  0 (Auto) or fixed power
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
usmDbWdmAPRadioResourceMeasEnabledGet(L7_enetMacAddr_t macAddr,
				      L7_uchar8        radioIndex,
                                      L7_BOOL          *pEnabled);


/*********************************************************************
* @purpose  Gets the next managed peer switch IP address.
*
* @param    ipAddr       @b{(input)} IP address of the switch
* @param    nextIpAddr   @b{(output)} Next Managed switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchEntryNextGet(L7_IP_ADDR_t ipAddr,
                                       L7_IP_ADDR_t * nextIpAddr);

/*********************************************************************
* @purpose  Gets vendorId for the given peer switch.
*
* @param    ipAddr           @b{(input)} IP address of the switch
* @param    vendorId         @b{(output)} Vendor of the peer WS software
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchVendorIdGet(L7_IP_ADDR_t ipAddr,
                                      L7_ushort16 * vendorId);

/*********************************************************************
* @purpose  Gets the protocolVersion of the given peer switch.
*
* @param    ipAddr            @b{(input)} IP address of the switch
* @param    protocolVersion   @b{(output)} Version of WS software on the
*                                          peer WS
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchProtocolVersionGet(L7_IP_ADDR_t ipAddr,
                                             L7_ushort16 * protocolVersion);

/*********************************************************************
* @purpose  Gets the software version for the given peer switch.
*
* @param    L7_IP_ADDR_t ip               @b{(input)}  IP address of the switch
* @param    L7_ushort16 *softwareVersion  @b{(output)} software version of the peer.
*                                                        buffer must be L7_WDM_SOFTWARE_VERSION_MAX+1.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchSoftwareVersionGet(L7_IP_ADDR_t ip,
                                             L7_char8    *softwareVersion);

/*********************************************************************
* @purpose  Get discovery method of the given peer switch.
*
* @param    L7_IP_ADDR_t          ip      @b{(input)}  IP address of the switch
* @param    L7_WDM_DISC_REASON_t *reason  @b{(output)} discovery reason
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchDiscoveryReasonGet(L7_IP_ADDR_t ip,
                                             L7_WDM_DISC_REASON_t *reason);

/*********************************************************************
* @purpose  Gets age field for a given peer switch.
*
* @param    ipAddr       @b{(input)} IP address of the switch
* @param    age          @b{(output)} Time since last communication with
*                                     the switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAgeGet(L7_IP_ADDR_t ipAddr, L7_uint32 * age);


/*********************************************************************
* @purpose  Gets peer switch IP address that pushed config.
*
* @param    ipAddr           @b{(input)} IP address of the switch
* @param    peerIPAddr       @b{(output)} peer switch IP address that pushed config
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchConfigPushRxIPAddrGet(L7_IP_ADDR_t ipAddr,
                                                L7_IP_ADDR_t *peerIPAddr);

/*********************************************************************
* @purpose  Gets config push receive time from peer switch.
*
* @param    ipAddr         @b{(input)} IP address of the switch
* @param    rxTime         @b{(output)} config receive time stamp
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchConfigPushRxTimeGet(L7_IP_ADDR_t ipAddr,L7_uint32 *rxTime);

/*********************************************************************
* @purpose  Gets config push mask receive from peer switch.
*
* @param    ipAddr        @b{(input)} IP address of the switch
* @param    mask          @b{(output)} config receive mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchConfigPushRxMaskGet(L7_IP_ADDR_t ipAddr,L7_uint32 *mask);

/*********************************************************************
* @purpose  Gets config push send status to peer switch.
*
* @param    ipAddr        @b{(input)} IP address of the switch
* @param    status        @b{(output)} config push send status
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchConfigPushReqStatusGet(L7_IP_ADDR_t ipAddr,
                                                 L7_WDM_PEER_SWITCH_CP_SEND_STATUS_t  *status);


/*********************************************************************
* @purpose  Verifies whether an entry is present or not.
*
* @param    ipAddr          @b{(input)} IP address of the switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE    If entry doesn't exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchEntryGet(L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Verifies whether an entry is present or not.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} MAC address of AP
* @param    L7_IP_ADDR_t     ip       @b{(input)} IP address of the switch,
*                                                   ip == 0 if not specified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      If entry doesn't exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPEntryGet(L7_enetMacAddr_t macAddr,
                                     L7_IP_ADDR_t     ip);

/*********************************************************************
* @purpose  Wrapper function for peerSwitchAPEntryNextGet,
*           Gets the next managed AP from the given MAC address. This can
*           also be used to get the first MAC address index.
*
* @param   macAddr         @b{(input)}  MAC address of the AP
* @param   nextMacAddr     @b{(output)} Next managed MAC address of the AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  For getting nextMacAddr, macAddr can be either flushed with
*            all 0's or a valid one.
*
* @notes     For fetching macAddr of the first entry, macAddr must be
*            flushed with all 0's.
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPEntryNextGet(L7_enetMacAddr_t macAddr,
                                    L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Determine if an entry exists Peer switch IP-AP MAC mapping.
*
* @param    L7_IP_ADDR_t     ipAddr    @b{(input)}  Peer switch IP address
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  Peer switch managed AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the Peer switch IP-AP MAC mapping, when an entry is
*            added to the peer switch managed AP database, and are removed
*            when entry is removed from the peer switch managed AP database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchIPAPEntryGet(L7_IP_ADDR_t ipAddr,L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get next entry in the Peer switch IP-AP MAC mapping.
*
* @param    L7_IP_ADDR_t     ipAddr    @b{(input)}  Peer switch IP address
* @param    L7_enetMacAddr_t apMacAddr   @b{(input)}  Peer switch managed AP MAC address
* @param    L7_IP_ADDR_t     *nextIPAddr     @b{(output)}  next peer switch IP address
* @param    L7_enetMacAddr_t *nextAPMacAddr  @b{(output)}  next peer switch managed AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the Peer switch IP-AP MAC mapping, when an entry is
*            added to the peer switch managed AP database, and are removed
*            when entry is removed from the peer switch managed AP database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchIPAPEntryNextGet(L7_IP_ADDR_t      ipAddr,
                                           L7_enetMacAddr_t  apMacAddr,
                                           L7_IP_ADDR_t      *nextIPAddr,
                                           L7_enetMacAddr_t  *nextAPMacAddr);

/*********************************************************************
* @purpose  Gets switch IP address for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipAddr           @b{(output)} Switch IP address managing the AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPSwitchIpAddrGet(L7_enetMacAddr_t macAddr,
                                            L7_IP_ADDR_t    *ipAddr);

/*********************************************************************
* @purpose  Gets AP IP address for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipAddr           @b{(output)} AP IP address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPIPAddrGet(L7_enetMacAddr_t macAddr,
                                      L7_IP_ADDR_t    *ipAddr);

/*********************************************************************
* @purpose  Gets AP IP mask for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipAddr           @b{(output)} AP IP address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPIPMaskGet(L7_enetMacAddr_t macAddr,
                                      L7_IP_MASK_t    *ipMask);

/*********************************************************************
* @purpose  Gets AP location Name for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    location         @b{(output)} location name of managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPLocationGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8     *location);

/*********************************************************************
* @purpose  Gets AP profile Id for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    profileId        @b{(output)} profile ID applied for managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPProfileIdGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8     *profileId);


/*********************************************************************
* @purpose  Gets AP profile name for a given AP MAC address.
*
* @param    macAddr         @b{(input)} AP MAC address
* @param    profileName     @b{(output)} profile name applied for managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPProfileNameGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8     *profileName);

/*********************************************************************
* @purpose  Gets AP hardware type for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    hwType        @b{(output)} hardware type of managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPHwTypeGet(L7_enetMacAddr_t macAddr,
                                      L7_uint32     *hwType);


/*********************************************************************
* @purpose  Gets managed AP's count for the given peer switch.
*
* @param    ipAddr           @b{(input)} IP address of the switch
* @param    mapCount         @b{(output)} Managed AP's count for the peer switch
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchManagedAPsGet(L7_IP_ADDR_t ip,L7_ushort16 *mapCount);

/*********************************************************************
* @purpose  Enable Wireless Trace Option
*
* @param    debug_level   @b{(input)} Trace Option
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmDebugSet (L7_uint32 debug_level);

/*********************************************************************
* @purpose  Disable Wireless Trace Option
*
* @param    debug_level   @b{(input)} Trace Option
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmDebugClear (L7_uint32 debug_level);

/*********************************************************************
* @purpose  Disable All flags for the Wireless Component
*
* @param    none
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmDebugAllClear (void);

/*********************************************************************
* @purpose  To retrieve the Cluster Priority.
*
* @param    prio   @b{(output)} Priority
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsPriorityGet (L7_uchar8 *prio);

/*********************************************************************
* @purpose  To set the Cluster Priority.
*
* @param    prio   @b{(input)} Priority
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsPrioritySet (L7_uchar8 prio);

/*********************************************************************
* @purpose  Get the AP auto upgrade feature mode configured on the WS
*
* @param    L7_uint32  upgradeMode  @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmAPAutoUpgradeModeGet(L7_uint32 *upgradeMode);

/*********************************************************************
* @purpose  Set the AP auto upgrade feature mode on the WS
*
* @param    L7_uint32  upgradeMode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmAPAutoUpgradeModeSet(L7_uint32 upgradeMode);

/*********************************************************************
* @purpose  To get the Cluster Controller
*
* @param    ipAddr   @b{(output)} Cluster Controller
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsControllerGet (L7_IP_ADDR_t *ipAddr);

/*********************************************************************
* @purpose  To get the Cluster Controller Indicator
*
* @param    ctrlInd   @b{(output)} Cluster Controller Indicator
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsControllerIndGet (L7_BOOL *ctrlInd);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId @b{(input)} test identifier.
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectTestModeGet(L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId, L7_uint32 *mode);

/*********************************************************************
* @purpose  To retrieve the Administrator-Configured Rogue Detection mode
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_ENABLE : If local database indicates that the AP is rogue
*           then report the AP as rogue in the RF Scan.
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectAdminConfiguredGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Administrator-Configured Rogue Detection mode.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_ENABLE : If local database indicates that the AP is rogue
*           then report the AP as rogue in the RF Scan.
*           Decided that this is always enabled ... can not be disabled.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectAdminConfiguredSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from an unknown AP Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedSsidUnknownApGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Managed SSID Received from an unknown AP Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from an unknown AP Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedSsidUnknownApSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from a fake managed AP Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedSsidFakeManagedApGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Managed SSID Received from an unknown AP Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from a fake managed AP Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedSsidFakeManagedApSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP without SSID Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectFakeManagedApNoSsidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Beacon received from a fake managed AP without SSID Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP without SSID Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectFakeManagedApNoSsidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID detected with incorrect security configuration Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedSsidSecurityInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Managed SSID detected with incorrect security configuration Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID detected with incorrect security configuration Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedSsidSecurityInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP with an invalid channel Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectFakeManagedApChanInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Beacon received from a fake managed AP with an invalid channel Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP with an invalid channel Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectFakeManagedApChanInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invalid SSID Received from a managed AP Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedApSsidInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Invalid SSID Received from a managed AP Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invalid SSID Received from a managed AP Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectManagedApSsidInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP is operating on an illegal channel Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectChanIllegalGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP is operating on an illegal channel Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectChanIllegalSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Stand-alone AP is operating with unexpected channel, SSID, security, or WIDS mode Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectStandaloneConfigInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Stand-alone AP is operating with unexpected channel, SSID, security, or WIDS mode Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectStandaloneConfigInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unexpected WDS device is detected on the network Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectWdsDeviceInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unexpected WDS device is detected on the network Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectWdsDeviceInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unmanaged AP is detected on a wired network Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectWiredUnManagedGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unmanaged AP is detected on a wired network Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectWiredUnManagedSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether the AP De-authentication attack is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPDeAuthenticationAttackGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable/Disable the AP De-authentication attack.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPDeAuthenticationAttackSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Retrieve the Minimum Wired Detection Interval
*
* @param    L7_ushort16 *val @b{(output)} location to store the Minimum Wired Detection Interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Minimum number of seconds that the AP waits before starting a new wired network detection cycle.
*           Zero indicates that Network detection is done on every RF-Scan cycle.
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPWiredDetectionIntervalGet(L7_ushort16 *val);

/*********************************************************************
*
* @purpose  Set the Minimum Wired Detection Interval.
*
* @param    L7_ushort16 val @b{(input)} new Minimum Wired Detection Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Minimum number of seconds that the AP waits before starting a new wired network detection cycle.
*           Zero indicates that Network detection is done on every RF-Scan cycle.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPWiredDetectionIntervalSet(L7_ushort16 val);

/*********************************************************************
* @purpose  Retrieve the Rogue-Detected trap interval.
*
* @param    L7_ushort16 *val @b{(output)} location to store the Rogue-Detected trap interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The interval in seconds between transmissions of the trap telling the administrator that Rogues are present in the RF Scan database.
*           Zero indicates that this trap is never sent.
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectionTrapIntervalGet(L7_ushort16 *val);

/*********************************************************************
*
* @purpose  Set the Rogue-Detected trap interval.
*
* @param    L7_ushort16 val @b{(input)} new Rogue-Detected trap interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The interval in seconds between transmissions of the trap telling the administrator that Rogues are present in the RF Scan database.
*           Zero indicates that this trap is never sent.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsAPRogueDetectionTrapIntervalSet(L7_ushort16 val);

/*********************************************************************
*
* @purpose  Check if an entry exists in AP hardware capability table.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeIDGet(L7_uchar8 hwTypeID);

/*********************************************************************
*
* @purpose  Get next hardware type ID entry from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 *nextHWTypeID @b{(input)} Next AP Hardware Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeIDNextGet(L7_uchar8 hwTypeID,L7_uchar8 *nextHWTypeID);

/*********************************************************************
*
* @purpose  Get hardware type description from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_char8   *hwTypeDescription @b{(input)} AP Hardware Type Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeDescriptionGet(L7_uchar8 hwTypeID,
                                       L7_char8 *hwTypeDescription);

/*********************************************************************
*
* @purpose  Get radio count from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 *radioCnt @b{(input)} Radio Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeNumRadiosGet(L7_uchar8 hwTypeID,L7_uchar8 *radioCnt);

/*********************************************************************
*
* @purpose  Get image type ID from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 *imageID @b{(input)} image ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeImageIDGet(L7_uchar8 hwTypeID,L7_uint32 *imageID);

/*********************************************************************
*
* @purpose  Get dual boot support from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_BOOL *dual @b{(input)} dual boot support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeDualBootSupportGet(L7_uchar8 hwTypeID,L7_BOOL *dual);

/*********************************************************************
*
* @purpose  Check if an entry exists in AP hardware capability table.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 radioIndex @b{(input)} Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadioIndexGet(L7_uchar8 hwTypeID,L7_uchar8 radioIndex);

/*********************************************************************
*
* @purpose  Get next radio index entry from AP hardware capability table.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 radioIndex @b{(input)} Radio Index
* @param    L7_uchar8 *nextRadioIndex @b{(input)}Next Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadioIndexNextGet(L7_uchar8 hwTypeID,
                                          L7_uchar8 radioIndex,
                                          L7_uchar8 *nextRadioIndex);

/*********************************************************************
*
* @purpose  Get radio type ID for this radio from AP radio hardware
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_uchar8  *radioTypeID @b{(input)} Radio Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadioIDGet(L7_uchar8 hwTypeID,
                                   L7_uchar8 radioIndex,
                                   L7_uchar8 *radioTypeID);

/*********************************************************************
*
* @purpose  Get radio type description for this from AP radio hardware
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_uchar8  *radioDescription @b{(input)} Radio Type Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadioDescriptionGet(L7_uchar8 hwTypeID,
                                            L7_uchar8 radioIndex,
                                            L7_char8 *radioDescription);

/*********************************************************************
*
* @purpose  Get VAP count for this radio from AP radio hardware capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_uchar8  *radioDescription @b{(input)} Radio Type Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadioVAPCountGet(L7_uchar8 hwTypeID,
                                         L7_uchar8 radioIndex,
                                         L7_uchar8 *vapCnt);

/*********************************************************************
*
* @purpose  Get A mode support for this radio from AP radio hardware
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_BOOL    *aMode @b{(input)} A mode support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadio80211aSupportGet(L7_uchar8 hwTypeID,
                                              L7_uchar8 radioIndex,
                                              L7_BOOL *aMode);

/*********************************************************************
*
* @purpose  Get BG mode support for this radio from AP radio hardware
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_BOOL    *bgMode @b{(input)} BG mode support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadio80211bgSupportGet(L7_uchar8 hwTypeID,
                                               L7_uchar8 radioIndex,
                                               L7_BOOL *bgMode);

/*********************************************************************
*
* @purpose  Get N mode support for this radio from AP radio hardware
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_BOOL    *nMode@b{(input)} N mode support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeRadio80211nSupportGet(L7_uchar8 hwTypeID,
                                              L7_uchar8 radioIndex,
                                              L7_BOOL *nMode);

/*********************************************************************
*
* @purpose  Set the expected channel of the stand-alone AP in the Local
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 channel @b{(input)} Expected channel for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedChannelSet(L7_enetMacAddr_t macAddr, L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Get the expected channel of the stand-alone AP in the Local
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *channel @b{(output)} Expected channel for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Set the expected SSID of the stand-alone AP in the Local
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *ssid @b{(input)} Expected SSID string for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedSsidSet(L7_enetMacAddr_t macAddr, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the expected SSID of the stand-alone AP in the Local
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *ssid @b{(output)} Expected SSID string for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedSsidGet(L7_enetMacAddr_t macAddr, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Set the expected security mode of the stand-alone AP in the
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 mode @b{(input)} Expected security mode for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedSecurityModeSet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_SECURITY_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the expected security mode of the stand-alone AP in the
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *mode @b{(output)} Expected security mode for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedSecurityModeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_SECURITY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the expected WDS mode of the stand-alone AP in the
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 mode @b{(input)} Expected WDS mode for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedWdsModeSet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WDS_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the expected WDS mode of the stand-alone AP in the
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *mode @b{(output)} Expected WDS mode for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedWdsModeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WDS_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the expected Wired Network mode of the stand-alone AP in the
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 mode @b{(input)} Expected Wired Network mode for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedWiredModeSet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WIRED_NETWORK_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the expected Wired Network mode of the stand-alone AP in the
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *mode @b{(output)} Expected Wired Network mode for the
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPStandaloneExpectedWiredModeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WIRED_NETWORK_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the number of APs to which the wireless system is currently
*           sending de-authentication messages to mitigate against rogue APs
*
* @param     L7_uint32 *mitigationCount @b{(input)} WIDS Rogue AP Mitigation Count.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsRogueAPMitigationCountGet(L7_uint32 *mitigationCount);

/*********************************************************************
*
* @purpose  Get the maximum number of APs for which the system can
*           send de-authentication frames.
*
* @param     L7_uint32 *mitigationLimit @b{(input)} maximum number of APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsRogueAPMitigationLimitGet(L7_uint32 *mitigationLimit);

/*********************************************************************
*
* @purpose  Clear out all rogue AP states in the RF Scan database.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRogueAPStateClearAll(void);

/*********************************************************************
*
* @purpose  Clear out the specified rogue AP state in the RF Scan database.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRogueAPStateClear(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the total number of managed APs in the database
*           for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           AP status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTotalAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are connected to the switch,
*           but are not completely configured.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_ushort32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           AP status only when WS acts Clsuter Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchDiscoveredAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are authenticated, configured,
*           and have an active connection with the speicified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           AP status only when WS acts Clsuter Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchManagedAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that were previously
*           authenticated and managed, but currently do not
*           have connection with the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           AP status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchConnFailedAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the maximum number of APs WS can manage.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           AP status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchMaxManagedAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the total number of clients in the database.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTotalClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of authentictaed clients in the database
*           specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32 *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchAuthClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);
/*********************************************************************
*
* @purpose Get the number of clients using l2 distributed tunneling.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32 *value @b{(output)} location to store the number
*                                         of wireless clients.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchDistTunnelClientsGet(L7_IP_ADDR_t IPAddr,
                                           L7_uint32 *value);

/*********************************************************************
* @purpose  Get AP image download mode supported by the specified switch
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} switch IP address
* @param    L7_uchar8     mode    @b{(output)} AP image download mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmSwitchAPImageDnloadModeGet(L7_IP_ADDR_t ipAddr, L7_uchar8 *mode);

/*********************************************************************
* @purpose  Get AP image download mode supported by the peer switch
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} peer switch IP address
* @param    L7_uchar8     mode    @b{(output)} AP image download mode
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchAPImageDnloadModeGet(L7_IP_ADDR_t ipAddr, L7_uchar8 *mode);

/*********************************************************************
*
* @purpose Get the WLAN utilization status of the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_ushort16 *value @b{(output)} location to store the percentage
* @param                                    of WLAN unitilization.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANUtilizationGet(L7_IP_ADDR_t IPAddr, L7_ushort16 *value);

/*********************************************************************
*
* @purpose Get the TSPEC total voice TS status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecTotalVoiceTsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total video TS status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecTotalVideoTsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS clients status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecTotalTsClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS roaming clients status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecTotalTsRoamClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the received WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN received
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANBytesRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN transmitted
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANBytesTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the received WLAN packets value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *pkts @b{(output)} Location to store WLAN received
*                                        packets value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANPktsRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN packets value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *pkts @b{(output)} Location to store WLAN transmitted
*                                        packets value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN dropped receive
*                                         bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANDropBytesRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the droped transmit WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN dropped receive
*                                          bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANDropBytesTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN packets value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} Location to store WLAN dropped receive
*                                          packets value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANDropPktsRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN packets value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} Location to store WLAN dropped receive
*                                          packets value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this function
*           to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchWLANDropPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the receive TSPEC WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecPktsRxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the transmit TSPEC WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the receive TSPEC WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecBytesRxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the transmit TSPEC WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecBytesTxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the WLAN accepted TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecTsAcceptedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN rejected TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecTsRejectedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN accepted roaming TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecRoamTsAcceptedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN rejected roaming TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchTspecRoamTsRejectedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
* @purpose  Returns the Cluster Priority for a given peer switch
*
* @param    IPAddr          @b{(input)} IP address of the switch
* @param    prio            @b{(output)}Cluster Priority
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmPeerSwitchWidsPriorityGet(L7_IP_ADDR_t IPAddr,
                            L7_uchar8 *prio);

/*********************************************************************
* @purpose  Gets the next managed switch IP address which may be of WS
*           itself of or that of peer switch.
*
* @param    ipAddr       @b{(input)} IP address of the switch
* @param    nextIpAddr   @b{(output)} Next Managed switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Next higher ipAddr can be either of WS itself or may be searched
*           in peer switch entries.  To acquire the next higher IPAddr from
*           peer group, WS should be acting as Cluster Controller.
*           if ip is passed with 0, least ipAddr is returned.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmSwitchEntryNextGet(L7_IP_ADDR_t  ip,
                                  L7_IP_ADDR_t *nextIp);

/*********************************************************************
* @purpose  Verifies whether an entry is present or not.
*
* @param    L7_IP_ADDR_t ip   @b{(input)} IP address of peer switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE    If entry doesn't exist
*
* @comments IPAddr being verified may be of WS itself or that may be searched
*           in peer switch entries. To check the IPAddr from the peer group,
*           WS should be acting as Cluster Controller.
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmSwitchEntryGet(L7_IP_ADDR_t  ip);

/*********************************************************************
* @purpose  Returns the Cluster Priority for a given switch
*
* @param    IPAddr          @b{(input)} IP address of the switch
* @param    prio            @b{(output)} Cluster Priority
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmSwitchWidsPriorityGet(L7_IP_ADDR_t IPAddr,
                            L7_uchar8 *prio);

/*********************************************************************
* @purpose  Returns the de-authentication attack status for the requested index
*
* @param    index           @b{(input)} index of the entry (1-WDM_AP_DE_AUTHENTICATION_ATTACK_MAX)
* @param   *entry           @b{(output)}de-authentication attack status entry
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsDeAuthenticationAttackStatusGet(L7_uint32 index, wdmWidsDeAuthenticationAttackStatus_t *entry);

/*********************************************************************
* @purpose  Returns the de-authentication attack status for the next index
*
* @param   *index           @b{(input)} index of the previous entry
* @param   *entry           @b{(output)}de-authentication attack status entry
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmWidsDeAuthenticationAttackStatusNextGet(L7_uint32 *index, wdmWidsDeAuthenticationAttackStatus_t *entry);

/*********************************************************************
*
* @purpose  Set the global auth radius server name.
*
* @param    L7_char8 *name @b{(input)} auth radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAuthRadiusServerNameSet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the global auth radius server name.
*
* @param    L7_char8 *name @b{(output)} auth radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAuthRadiusServerNameGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the global acct radius server name.
*
* @param    L7_char8 *name @b{(input)} acct radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAcctRadiusServerNameSet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the global acct radius server name.
*
* @param    L7_char8 *name @b{(output)} auth radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAcctRadiusServerNameGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the accounting mode for wireless clients.
*
* @param    L7_uint32 mode @b{(input)} Accounting mode for wireless clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAcctModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the accounting mode for wireless clients.
*
* @param    L7_uint32 *mode @b{(output)} Accounting mode for wireless clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAcctModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the MAC authentication mode.
*
* @param    L7_uint32 mode @b{(input)} MAC authentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmMACAuthModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the MAC authentication mode.
*
* @param    L7_uint32 *mode @b{(output)} MAC authentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmMACAuthModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose Get the global authentication server configuration status.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store the status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAuthRadiusConfigurationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose Get the global accounting server configuration status.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store the status.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAcctRadiusConfigurationGet( L7_uint32 *value);

/*********************************************************************
*
* @purpose  Checks if indexed VAP entry exists for the specified hardware
*           type.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 radioIndex @b{(input)} Radio Index
* @param    L7_uchar8 vapIndex @b{(input)} VAP Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPHWTypeVAPIndexGet(L7_uchar8 hwTypeID,L7_uchar8 radioIndex,
                     L7_uchar8 vapIndex);

/*********************************************************************
*
* @purpose  Set the Detected Rogue Trap Interval Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value [0 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientDetectedRogueTrapIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Detected Rogue Trap Interval Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientDetectedRogueTrapIntervalGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Client present in OUI List test Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Not Supported in this release
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientNotInOUIListTestSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Client present in OUI List test Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Not Supported in this release
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientNotInOUIListTestGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Client present in Known Database test Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientNotInKnownDBTestSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Client present in Known Database test Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientNotInKnownDBTestGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the configured rate of
*           802.11 Authentication Requests test.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientConfiguredAuthRateSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the configured rate of
*           802.11 Authentication Requests test.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientConfiguredAuthRateGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the configured rate of
*           802.11 Probe Requests test.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientConfiguredProbeRateSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the configured rate of
*           802.11 Probe Requests test.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientConfiguredProbeRateGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the configured rate of
*           802.11 De-Authentication Requests test.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientConfiguredDeAuthRateSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the configured rate of
*           802.11 De-Authentication Requests test.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientConfiguredDeAuthRateGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the Maximum number of
*           Authentication Failures test
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientMaxAuthFailureSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the Maximum number of
*           Authentication Failures test
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientMaxAuthFailureGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the Known Client is
*           authenticated with unknown AP test
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthWithUnknownAPSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the Known Client is
*           authenticated with unknown AP test
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthWithUnknownAPGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Client Security Threat Mitigation Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientThreatMitigationSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Client Security Threat Mitigation Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientThreatMitigationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Number of seconds for counting the de-authentication
*           messages.
*
* @param    L7_uint32 value @b{(input)} value [1 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientDeAuthThresholdIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Number of seconds for counting the de-authentication
*           messages.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientDeAuthThresholdIntervalGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the DeAuth message threshold value. If switch receives
*           more deauth messages than this threshold value during the
*           threshold interval the test triggers.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientDeAuthThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the DeAuth message threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientDeAuthThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Number of seconds for counting the authentication
*           messages.
*
* @param    L7_uint32 value @b{(input)} value [1 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthThresholdIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Number of seconds for counting the authentication
*           messages.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthThresholdIntervalGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Auth message threshold value. If switch receives
*           more Auth messages than this threshold value during the
*           threshold interval the test triggers.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the Auth message threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Number of seconds for counting the probe messages.
*
* @param    L7_uint32 value @b{(input)} value [1 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientProbeThresholdIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Number of seconds for counting the Probe messages.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientProbeThresholdIntervalGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Probe message threshold value. If switch receives
*           more Probe messages than this threshold value during the
*           threshold interval the test triggers.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientProbeThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Probe message threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientProbeThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Authentication failures threshold value. This is
*           the number of 802.1X authentication failures that triggers
*           the client to be reported as rogue.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientAuthFailureThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Authentication Failure threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t usmDbWdmWidsClientAuthFailureThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the location(Local/Radius) where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_uint32 value @b{(input)} value - Local/Radius
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientKnownDBLocationSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the location where Known Client database is defined for
*           Detected Client database.
*
* @param    L7_uint32   *value  @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientKnownDBLocationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the name of the Radius server where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_char8 *name       @b{(input)} pointer storing the name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientKnownDBRadiusServerNameSet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the name of the Radius server where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_char8 *name       @b{(output)} pointer storing the name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientKnownDBRadiusServerNameGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the status of the Radius server where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_char8    *name    @b{(input)} pointer storing the name.
* @param    L7_uint32   *value   @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWidsClientKnownDBRadiusServerStatusGet(L7_char8 *name, L7_uint32 *value);


/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling idle timeout value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelIdleTimeoutGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling idle timeout value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelIdleTimeoutSet( L7_uint32 value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling max timeout value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelMaxTimeoutGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling max timeout value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelMaxTimeoutSet( L7_uint32 value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling max clients value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelMaxClientsGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling max clients value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelMaxClientsSet( L7_uint32 value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling max mcast replications
*          value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelMcastReplGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling max mcast replications
*          value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelMcastReplSet( L7_uint32 value);
/*********************************************************************
*
* @purpose  Get the transmitted Distributed tunnel packets value for
*           the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store transmitted
*                                         WLAN packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelPktsTxGet(L7_uint64 *pkts);
/*********************************************************************
*
* @purpose  Get the number of clients in roaming mode
*           the Wireless Switch (WS).
*
* @param    L7_uint32 *val @b{(output)} location to store value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelRoamedClientsGet(L7_uint32 *val);
/*********************************************************************
*
* @purpose  Get the number of clients in denied to set up tunnel by
*           the Wireless Switch (WS).
*
* @param    L7_uint32 *val @b{(output)} location to store value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelClientDenialsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Determine the default PHY Mode for a given profile
*           and radio
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*           L7_uint32 radioIndex @b{(input)} Radio Index
*
* returns   L7_WDM_RADIO_PHY_MODE_t
*
* @notes    L7_WDM_RADIO_PHY_MODE_NONE returned on error
*
* @end
***********************************************************************/

L7_WDM_RADIO_PHY_MODE_t
usmDbWdmAPProfileRadioDefaultModeGet(L7_uint32 profileId,
                                         L7_uint32 radioIndex);


/* Start Auto AP Image Download APIs */

/*********************************************************************
* @purpose  Check if the cluster switches have different AP image 
*           download modes
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessAPAutoUpdImageModeMixed(void);

/*********************************************************************
* @purpose  Get the WS stored AP image IDs
*
* @param    L7_uint32  imageIDList  @b{(output)} pointer to ID list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_NOT_EXIST
* @returns  L7_REQUEST_DENIED
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessAPAutoUpdImageIDsGet(L7_uint32 *imageIDList);

/*********************************************************************
* @purpose  Get the switch stored AP image version based on AP image ID
*
* @param    L7_uint32 imageID  @b{(input)} AP image ID/type
* @param    L7_char8  buf      @b{(output)} pointer to buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessAPAutoUpdImageVersionGet(L7_uint32 imageID, L7_char8 *buf);

/*********************************************************************
* @purpose  Delete the AP image name file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessAPAutoUpdImageNameFileDelete(void);

/* End Auto AP Image Download APIs */

/*********************************************************************
*
* @purpose  Set the auto IP address assignment mode for the Wireless Switch (WS).
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSAutoIPAssignModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the auto IP address assignment mode for the Wireless Switch (WS).
*
* @param    L7_uint32 *mode @b{(input)} pointer to return mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSAutoIPAssignModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set static IP address to wireless switch.
*
* @param    L7_IP_ADDR_t IPAddr @b{(input)} IP address of the switch.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that assigned IP address is same as network 
*           interface on a switching package or IP address of active routing 
*           interface if routing is enabled.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSSwitchStaticIPAddrSet(L7_IP_ADDR_t IPAddr );

/*********************************************************************
*
* @purpose  Get static IP address assigned to wireless switch.
*
* @param    L7_IP_ADDR_t *IPAddr @b{(input)} IP address of the switch.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSSwitchStaticIPAddrGet(L7_IP_ADDR_t *IPAddr);

/*********************************************************************
*
* @purpose  Set the Switch Provisioning mode
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchProvisioningModeSet (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Get the Switch Provisioning mode
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchProvisioningModeGet(L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Set the Network Mutual Authentication mode.
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmNetworkMutualAuthModeSet (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Get the Network Mutual Authentication mode.
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmNetworkMutualAuthModeGet (L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Get the Network Mutual Authentication status.
*
* @param    L7_uint32  *status  @b{(output)} store network mutual auth status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmNetworkMutualAuthStatusGet(L7_uint32 *status);

/*********************************************************************
*
* @purpose  Set the AP Reprovisioning mode.
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPReprovisioningModeSet (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Get the AP Reprovisioning mode.
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPReprovisioningModeGet (L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Set the AP Provisioning Age Time
*
* @param    L7_uint32 ageTime  @b{(input)} Age time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningAgeTimeSet (L7_ushort16 agetime);

/*********************************************************************
*
* @purpose  Get the AP Provisioning Age Time
*
* @param    L7_uint32 *ageTime  @b{(output)} pointer to Age time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningAgeTimeGet (L7_ushort16 *agetime);

/*********************************************************************
*
* @purpose  Start exchange of X.509 certificates
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmNetworkExchangeCertificate ();

/*********************************************************************
*
* @purpose  Initiate command to regenerate certificate and key for the Wireless Switch (WS).
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCertificateRegenerateStart(void);

/*********************************************************************
*
* @purpose  Get the status to regenerate certificate and key for the Wireless Switch (WS).
*
* @param    L7_uint32 status @b{(output)} status to indicate to regenerate certificate and keys, L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSCertificateRegenerateStatusGet(L7_uint32 *status);

/*********************************************************************
*
* @purpose  Start switch provisioning 
*
* @param    none
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSProvisioningStart(void);

/*********************************************************************
*
* @purpose  Initiate X.509 certificate request
*
* @param    none
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSX509CertificateReqCommand(void);

/*********************************************************************
*
* @purpose  Set IP address of the switch from which this switch 
*           requests a X.509 certificate
*
* @param    certIPAddr  @b{(input)} IP address of the switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSX509CertificateReqTargetSet(L7_IP_ADDR_t certIPAddr);

/*********************************************************************
*
* @purpose  Get IP address of the switch from which this switch 
*           requested a X.509 certificate
*
* @param    certIPAddr   @b{(output)} ptr to IP address of the switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSX509CertificateReqTargetGet(L7_IP_ADDR_t *certIPAddr);

/*********************************************************************
*
* @purpose  Get the X.509 certificate request status
*
* @param    certReqStatus    @b{(output)} ptr to certificate status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSX509CertificateReqStatusGet(
                L7_WDM_WS_X509_CERTIFICATE_REQ_STATUS_t *certReqStatus);

/*********************************************************************
*
* @purpose  Get switch provisioning status
*
* @param    status   @b{(output)} ptr to provisioning status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmWSProvisioningStatusGet(L7_WDM_WS_PROVISIONING_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the IP address of the switch to obtain provisioning information
*
* @param    provIPAddr    @b{(input)} IP address of switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSProvisioningSwitchIPAddrSet(L7_IP_ADDR_t provIPAddr);

/*********************************************************************
*
* @purpose  Get the IP address of the switch from which provisioning 
*           information is obtained
*
* @param    provIPAddr   @b{(output)} ptr to IP address of switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWSProvisioningSwitchIPAddrGet(L7_IP_ADDR_t *provIPAddr);

/*********************************************************************
*
* @purpose  Get the IP port for UDP Tunnel communication
*
* @param    L7_short16 *ipTnlUdpPort  @b{(output)} pointer to Tunnel UDP port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIpTnlUdpPortGet (L7_ushort16 *ipTnlUdpPort);

/*********************************************************************
*
* @purpose  Get the IP port for UDP communication
*
* @param    L7_short16 *ipUdpPort  @b{(output)} pointer to UDP port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIpUdpPortGet (L7_ushort16 *ipUdpPort);

/*********************************************************************
*
* @purpose  Get the IP port for SSL communication
*
* @param    L7_short16 *ipSslPort  @b{(output)} pointer to SSL port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIpSslPortGet (L7_ushort16 *ipSslPort);

/*********************************************************************
*
* @purpose  Get the IP port for CAPWAP Src communication
*
* @param    L7_short16 *ipCapwapSrcPort  @b{(output)} pointer to CAPWAP Src port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIpCapwapSrcPortGet (L7_ushort16 *ipCapwapSrcPort);

/*********************************************************************
*
* @purpose  Get the IP port for CAPWAP Dst communication
*
* @param    L7_short16 *ipCapwapDstPort  @b{(output)} pointer to CAPWAP Dst port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIpCapwapDstPortGet (L7_ushort16 *ipCapwapDstPort);

/*********************************************************************
*
* @purpose  Get the base IP port for WS communication
*
* @param    L7_short16 *ipBasePort  @b{(output)} pointer to base port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmIpBasePortGet (L7_ushort16 *ipBasePort);

/*********************************************************************
*
* @purpose  Get the configured base IP port for WS communication
*
* @param    L7_short16 *ipBasePort  @b{(output)} pointer to base port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCfgIpBasePortGet (L7_ushort16 *ipBasePort);

/*********************************************************************
*
* @purpose  Set the configured base IP port for WS communication
*
* @param    L7_short16 ipBasePort  @b{(input)} base port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Set to port 0 to restore default
*           Operational ports will be automatically synced by task watchdog loop
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmCfgIpBasePortSet (L7_ushort16 ipBasePort);

#endif /* INCLUDE_USMDB_WDM_API_H */
