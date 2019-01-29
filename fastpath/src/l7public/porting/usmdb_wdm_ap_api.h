/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_ap_api.h
*
* @purpose    Wireless Data Manager Access Point (WDM AP) USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     1/15/2006
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_USMDB_WDM_AP_API_H
#define INCLUDE_USMDB_WDM_AP_API_H

#include "l7_common.h"
#include "wireless_comm_structs.h"

/* Start per managed AP APIs */

/*********************************************************************
*
* @purpose  Get the network IP adddress of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *ipAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPIPAddrGet(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the network IP Subnet Mask of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_MASK_t      *ipMask  @b{(output)} pointer to ip subnet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPIPMaskGet (L7_enetMacAddr_t macAddr,
                            L7_IP_MASK_t *ipMask);

/*********************************************************************
*
* @purpose  Get the IP Address of WS managing this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *siwtchIPAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPSwitchIPAddrGet(L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *switchIPAddr);

/*********************************************************************
*
* @purpose  Get the MAC Address of WS managing this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *switchMACAddr  @b{(output)} pointer to MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPSwitchMACAddrGet(L7_enetMacAddr_t macAddr,
                            L7_enetMacAddr_t *switchMACAddr);

/*********************************************************************
*
* @purpose  Is the AP managed by the local switch (Cluster Controller)
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *apLocal  @b{(output)} pointer to is AP Local
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPLocalGet(L7_enetMacAddr_t macAddr, L7_BOOL *apLocal);

/*********************************************************************
*
* @purpose  Get the configuration profile ID applied to managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32         *profileId  @b{(output)} pointer to profile id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPProfileIdGet(L7_enetMacAddr_t macAddr, L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Get the configuration profile name applied to managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_char8          *profileName  @b{(output)} pointer to profile name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPProfileNameGet(L7_enetMacAddr_t macAddr,
                            L7_char8 *profileName);

/*********************************************************************
*
* @purpose  Get the managed AP radio admin mode.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *mode  @b{(output)} pointer to radio mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioAdminModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Get the managed AP radio scan sentry mode.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *sentryMode  @b{(output)} pointer to sentry mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioScanSentryModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *sentryMode);

/*********************************************************************
*
* @purpose  Get the managed AP VAP mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uchar8         vapId  @b{(input)} VAP id
* @param    L7_uint32         *vapMode  @b{(output)} pointer to Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 vapId,
                            L7_uint32 *vapMode);

/*********************************************************************
*
* @purpose  Get the managed AP VAP SSID
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uchar8         vapId  @b{(input)} VAP id
* @param    L7_char8          *ssid  @b{(output)} pointer to SSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPSSIDGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 vapId,
                            L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the OUI of the detected network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_char8          *oui     @b{(output)} pointer to ssid
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanOUIParamGet(L7_enetMacAddr_t macAddr, L7_char8 *oui);



/*********************************************************************
*
* @purpose  Get the location of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_char8          *location  @b{(output)} pointer to location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPLocationGet(L7_enetMacAddr_t macAddr, L7_char8 *location);

/*********************************************************************
*
* @purpose  Get the vendor ID of managed AP software.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_ushort16       *vendorId  @b{(output)} pointer to vendor id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVendorIdGet(L7_enetMacAddr_t macAddr, L7_ushort16 *vendorId);

/*********************************************************************
*
* @purpose  Get the protocol version supported on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_ushort16       *prtlVersion  @b{(output)} pointer to prtl version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPProtocolVersionGet(L7_enetMacAddr_t macAddr, L7_ushort16 *prtlVersion);

/*********************************************************************
*
* @purpose  Get the software version on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         *swVersion  @b{(output)} pointer to sw version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPSoftwareVersionGet(L7_enetMacAddr_t macAddr, L7_uchar8 *swVersion);

/*********************************************************************
*
* @purpose  Get the hardware platform for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *hwType  @b{(output)} pointer to hw type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPHardwareTypeGet(L7_enetMacAddr_t macAddr, L7_uint32 *hwType);

/*********************************************************************
*
* @purpose  Get the unique serial number assigned to managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         *serialNum  @b{(output)} pointer to serial number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPSerialNumGet(L7_enetMacAddr_t macAddr, L7_uchar8 *serialNum);

/*********************************************************************
*
* @purpose  Get the hardware part number for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         *partNum  @b{(output)} pointer to part number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPPartNumGet(L7_enetMacAddr_t macAddr, L7_uchar8 *partNum);

/*********************************************************************
*
* @purpose  Get the managed AP discovery method.
*
* @param    L7_enetMacAddr_t     macAddr     @b{(input)} AP mac address
* @param    L7_WDM_DISC_REASON_t *discReason @b{(output)} pointer to method
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDiscoveryReasonGet(L7_enetMacAddr_t macAddr,
                                            L7_WDM_DISC_REASON_t *discReason);

/*********************************************************************
*
* @purpose  Get the current state of managed AP.
*
* @param    L7_enetMacAddr_t                macAddr @b{(input)} AP mac address
* @param    L7_WDM_MANAGED_AP_STATUS_t *status @b{(output)} pointer to state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_MANAGED_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the current configuration status of managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t *status     @b{(output)} pointer to status
* @param    L7_ushort16            elementId   @b{(output)} failing config element ID
* @param    L7_char8               *failureMsg @b{(output)} failure message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPCfgStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t *status,
                                      L7_ushort16 *elementId, L7_char8 *failureMsg);

/*********************************************************************
*
* @purpose  Get the current status of code download for managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t  *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDownloadStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Start the code download for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments UI calls this API via usmDb to ensure that the user request
*           is not made in the wrong state.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDownloadStart(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the number of AP authenticated clients.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *clients  @b{(output)} pointer to number of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPAuthenticatedClientsGet(L7_enetMacAddr_t macAddr, L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Get the time since last power-on reset of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32         *sysUpTime  @b{(output)} pointer to system up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPSysUptimeGet(L7_enetMacAddr_t macAddr, L7_uint32 *sysUpTime);

/*********************************************************************
*
* @purpose  Get the time since last communication with managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *age     @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Get the reset status of managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t *status @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPResetStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Reset a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPReset(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Reset all managed APs.
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
L7_RC_t usmDbWdmManagedAPResetAllStart();

/*********************************************************************
*
* @purpose  Purge a failed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPFailedEntryPurge(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Purge all managed AP entries with a L7_WDM_MANAGED_AP_FAILED status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API is used to cleanup failed entries.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPFailedEntriesPurge();

/*********************************************************************
*
* @purpose  Set password to Enable/Disable debug on a managed AP.
*
* @param    L7_enetMacAddr_t   macAddr     @b{(input)} AP mac address
* @param    L7_char8          *password    @b{(input)} debug password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDebugPasswordSet(L7_enetMacAddr_t macAddr,
                                          L7_char8        *password);

/*********************************************************************
*
* @purpose  Enable/Disable debug mode for a managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)} AP mac address
* @param    L7_uint32              mode    @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API sets the debug mode and initiates action to send command.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDebugModeSet(L7_enetMacAddr_t macAddr,
                                      L7_uint32        mode);

/*********************************************************************
*
* @purpose  Get requested debug mode for a managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)}  AP mac address
* @param    L7_uint32             *mode    @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDebugModeGet(L7_enetMacAddr_t macAddr,
                                      L7_uint32       *mode);

/*********************************************************************
*
* @purpose  Get the debug command status for a managed AP.
*
* @param    L7_enetMacAddr_t        macAddr @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDebugStatusGet(L7_enetMacAddr_t        macAddr,
                                        L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Verify presence of a managed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next managed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);


/*********************************************************************
*
* @purpose  Get the mac address of next managed AP entry with managed status.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPNextEntryWithManagedStatusGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next managed AP entry with managed status
*           of given image type.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
* @param    L7_uint32         *imageType  @b{(input)} Image type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPImageTypeEntryNextGet(L7_enetMacAddr_t macAddr,
                                               L7_enetMacAddr_t *nextMacAddr,
                                               L7_WDM_AP_IMAGE_TYPE_t imageType);

/*********************************************************************
*
* @purpose  Get the TSPEC number of active TS for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPTspecNumActiveTsGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 acindex,
                                             L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS clients for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPTspecNumTsClientsGet(L7_enetMacAddr_t macAddr,
                                              L7_uchar8 acindex,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS roaming clients for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPTspecNumTsRoamClientsGet(L7_enetMacAddr_t macAddr,
                                                  L7_uchar8 acindex,
                                                  L7_uint32 *val);
/* End per managed AP APIs */

/* Start per radio APIs */

/*********************************************************************
*
* @purpose  Get the mac address for radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr       @b{(input)} AP mac address
* @param    L7_uchar8        radioIf       @b{(input)} radio interface
* @param    L7_enetMacAddr_t *radioMacAddr @b{(output)} pointer to radio mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioMacAddrGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_enetMacAddr_t *radioMacAddr);

/*********************************************************************
*
* @purpose  Get the physical mode for radio on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr @b{(input)} AP mac address
* @param    L7_uchar8               radioIf @b{(input)} radio interface
* @param    L7_WDM_RADIO_PHY_MODE_t *mode   @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioPhyModeGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_WDM_RADIO_PHY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the transmit channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8        *channel  @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioChannelGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the channel bandwidth for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8        *bandwidth  @b{(output)} pointer to bandwidth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioBandwidthGet(L7_enetMacAddr_t macAddr,
                             L7_uchar8 radioIf, L7_uchar8 *bandwidth);

 /*********************************************************************
*
* @purpose  Verify presence of a supported channel for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8        *channel  @b{(input)} input channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioSupportedChGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf,
                                            L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Get the next supported channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_uchar8         radioIf       @b{(input)} radio interface
* @param    L7_uchar8         channel       @b{(input)} input channel
* @param    L7_uchar8        *nextChannel   @b{(output)} pointer to next channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioSupportedChNextGet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf,
                                                L7_uchar8 channel,
                                                L7_uchar8 *nextChannel);
/*********************************************************************
*
* @purpose  Get the radar detection required flag for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8*        radarDetectionRequired  @b{(output)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioSupportedChRdrDetectionRequiredGet(
                                       L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uchar8* radarDetectionRequired);
/*********************************************************************
*
* @purpose  Get the radar detected flag for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8*        radarDetected  @b{(output)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST -- If the AP Entry is not present in the Managed AP
*                           database
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioSupportedChRdrDetectedGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uchar8* radarDetected);
/*********************************************************************
*
* @purpose  Get the radar detected time for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} channel
* @param    L7_uint32         time     @b{(output)} radar det time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioSupportedChRdrDetectedTimeGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uint32* time);

/*********************************************************************
*
* @purpose  Get the fixed channel flag for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           *chInd   @b{(output)} pointer to flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioFixedChIndGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_BOOL *chInd);

/*********************************************************************
*
* @purpose  Get the channel change request status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_uchar8               radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioManChAdjStatusGet(L7_enetMacAddr_t        macAddr,
                                                L7_uchar8               radioIf,
                                                L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Start a manual channel change request for a managed AP radio.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} channel to assign
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad entry or channel change in progress
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioManChAdjStart(L7_enetMacAddr_t macAddr,
                                            L7_uchar8        radioIf,
                                            L7_uchar8        channel);

/*********************************************************************
*
* @purpose  Get the transmit power for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         *power   @b{(output)} pointer to power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTxPwrGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 *power);

/*********************************************************************
*
* @purpose  Get the fixed power flag for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           *pwrInd  @b{(output)} pointer to flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioFixedPwrIndGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 radioIf, L7_BOOL *pwrInd);

/*********************************************************************
*
* @purpose  Get the power change request status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_uchar8               radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioManPwrAdjStatusGet(L7_enetMacAddr_t        macAddr,
                                                 L7_uchar8               radioIf,
                                                 L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Start a manual power change request for a managed AP radio.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         power    @b{(input)} power to assign
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad entry or power change in progress
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioManPwrAdjStart(L7_enetMacAddr_t macAddr,
                                             L7_uchar8        radioIf,
                                             L7_uchar8        power);

/*********************************************************************
*
* @purpose  Get the number of authenticated clients on radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         *clients  @b{(output)} pointer to clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioAuthClientsGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 radioIf, L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Get the number of neighbors on radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         *nbrs    @b{(output)} pointer to neighbors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTotalNbrsGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf, L7_uint32 *nbrs);

/*********************************************************************
*
* @purpose  Get the total network utilization for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         *util    @b{(output)} pointer to utilization
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioWLANUtilGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf, L7_uint32 *util);

/*********************************************************************
*
* @purpose  Get the RRM enable status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           *enab    @b{(output)} is enabled?
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
usmDbWdmManagedAPRadioResourceMeasEnabledGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 radioIf, L7_BOOL *enab);

/*********************************************************************
*
* @purpose  Get the TSPEC operational status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecOperStatusGet(L7_enetMacAddr_t macAddr,
                                                 L7_uchar8 radioIf,
                                                 L7_uchar8 acindex,
                                                 L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of active TS for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecNumActiveTsGet(L7_enetMacAddr_t macAddr,
                                                  L7_uchar8 radioIf,
                                                  L7_uchar8 acindex,
                                                  L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecNumTsClientsGet(L7_enetMacAddr_t macAddr,
                                                   L7_uchar8 radioIf,
                                                   L7_uchar8 acindex,
                                                   L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS roaming clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecNumTsRoamClientsGet(L7_enetMacAddr_t macAddr,
                                                       L7_uchar8 radioIf,
                                                       L7_uchar8 acindex,
                                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time admitted for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecMedTimeAdmittedGet(L7_enetMacAddr_t macAddr,
                                                      L7_uchar8 radioIf,
                                                      L7_uchar8 acindex,
                                                      L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           non-roaming and roaming clients.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecMedTimeUnallocGet(L7_enetMacAddr_t macAddr,
                                                     L7_uchar8 radioIf,
                                                     L7_uchar8 acindex,
                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time roaming unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           roaming clients only.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioTspecMedTimeRoamUnallocGet(L7_enetMacAddr_t macAddr,
                                                         L7_uchar8 radioIf,
                                                         L7_uchar8 acindex,
                                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose  Verify presence of a radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioEntryGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf);


#if 0 /* May want this in the future. */
/*********************************************************************
*
* @purpose  Verifies the usage of an MCS index on a managed AP's radio
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    mcs_idx_t             idx   @b{(input)} MCS index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioMCSIndexGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf,
                                          mcs_idx_t idx);


/*********************************************************************
*
* @purpose  Finds the next MCS index in use on a managed AP's radio
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    mcs_idx_t             idx   @b{(input)} MCS index
* @param    mcs_idx_t            pIdx   @b{(output)} ptr to MCS index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmManagedAPRadioMCSIndexGetNext(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 radioIf,
                                      mcs_idx_t idx,
                                      mcs_idx_t *pIdx);
#endif

/*********************************************************************
*
* @purpose  Get the next radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_uchar8         radioIf       @b{(input)} radio interface
* @param    L7_uchar8         *nextRadioIf  @b{(output)} pointer to next radio
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioEntryNextGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf, L7_uchar8 *nextRadioIf);

/* End per radio APIs */

/* Start per vap APIs */

/*********************************************************************
*
* @purpose  Verify the entry VAP mac address.
*
* @param    L7_enetMacAddr_t vapMacAddr @b{(input)} VAP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPMACAddrEntryGet (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the VAP mac address.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP mac address
* @param    L7_uchar8        radioIf     @b{(input)} radio interface
* @param    L7_uint32        vapId       @b{(input)} vap id
* @param    L7_enetMacAddr_t *vapMacAddr @b{(output)} pointer to VAP mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPMacAddrGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                       L7_uint32 vapId, L7_enetMacAddr_t *vapMacAddr);

/*********************************************************************
*
* @purpose  Get the radio I/F and vap ID, given a VAP mac address.
*           (The inverse of usmDbWdmManagedAPVAPMacAddrGet().)
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP mac address
* @param    L7_enetMacAddr_t vapMacAddr  @b{(input)} VAP mac
* @param    L7_uchar8        *radioIf    @b{(output)} radio interface
* @param    L7_uint32        *vapId      @b{(output)} vap id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPMacAddrFind(L7_enetMacAddr_t macAddr,
                                        L7_enetMacAddr_t vapMacAddr,
                                        L7_uchar8 * pRadioIf,
                                        L7_uint32 * pVapId);

/*********************************************************************
*
* @purpose  Get the number of clients authenticated to the vap on a radio.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uint32         *clients  @b{(output)} pointer to clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPAuthClientsGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                           L7_uint32 vapId, L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Get the TSPEC operational status for the vap on a radio.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecOperStatusGet(L7_enetMacAddr_t macAddr,
                                               L7_uchar8 radioIf,
                                               L7_uint32 vapId,
                                               L7_uchar8 acindex,
                                               L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of active TS for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecNumActiveTsGet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf,
                                                L7_uint32 vapId,
                                                L7_uchar8 acindex,
                                                L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecNumTsClientsGet(L7_enetMacAddr_t macAddr,
                                                 L7_uchar8 radioIf,
                                                 L7_uint32 vapId,
                                                 L7_uchar8 acindex,
                                                 L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS roaming clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecNumTsRoamClientsGet(L7_enetMacAddr_t macAddr,
                                                     L7_uchar8 radioIf,
                                                     L7_uint32 vapId,
                                                     L7_uchar8 acindex,
                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time admitted for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecMedTimeAdmittedGet(L7_enetMacAddr_t macAddr,
                                                    L7_uchar8 radioIf,
                                                    L7_uint32 vapId,
                                                    L7_uchar8 acindex,
                                                    L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           non-roaming and roaming clients.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecMedTimeUnallocGet(L7_enetMacAddr_t macAddr,
                                                   L7_uchar8 radioIf,
                                                   L7_uint32 vapId,
                                                   L7_uchar8 acindex,
                                                   L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time roaming unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           roaming clients only.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPTspecMedTimeRoamUnallocGet(L7_enetMacAddr_t macAddr,
                                                       L7_uchar8 radioIf,
                                                       L7_uint32 vapId,
                                                       L7_uchar8 acindex,
                                                       L7_uint32 *val);

/*********************************************************************
*
* @purpose  Verify presence of a vap entry on a radio.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         vapId    @b{(input)} vap id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPEntryGet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 radioIf, L7_uint32 vapId);

/*********************************************************************
*
* @purpose  Get the next vap entry on a radio.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         radioIf     @b{(input)} radio interface
* @param    L7_int32          vapId       @b{(input)} vap id
* @param    L7_uint32         *nextVapId  @b{(output)} pointer to next id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPVAPEntryNextGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                         L7_int32 vapId, L7_uint32 *nextVapId);

/* End per vap APIs */

/* Start neighbor AP APIs */

/*********************************************************************
*
* @purpose  Get the service set ID of neighbor AP network.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_char8          *ssid     @b{(output)} pointer to ssid
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrAPSSIDGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                           L7_enetMacAddr_t nbrApMac, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the received signal strength of neighbor AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_uint32         *rssi     @b{(output)} pointer to indicator
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrAPRSSIGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                           L7_enetMacAddr_t nbrApMac, L7_uint32 *rssi);

/*********************************************************************
*
* @purpose  Get the managed status of neighbor AP.
*
* @param    L7_enetMacAddr_t    macAddr   @b{(input)} AP mac address
* @param    L7_uchar8           radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t    nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_WDM_AP_STATUS_t  *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrAPStatusGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                             L7_enetMacAddr_t nbrApMac, L7_WDM_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the time since the neighbor AP was last detected.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_uint32         *age      @b{(output)} pointer to time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrAPAgeGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                          L7_enetMacAddr_t nbrApMac, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Verify presence of a neighbor AP entry.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrAPEntryGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_enetMacAddr_t nbrApMac);

/*********************************************************************
*
* @purpose  Get the next neighbor AP entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)} AP mac address
* @param    L7_uchar8         radioIf        @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac       @b{(input)} neighbor AP mac
* @param    L7_enetMacAddr_t  *nextNbrApMac  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrAPEntryNextGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                                L7_enetMacAddr_t nbrApMac, L7_enetMacAddr_t *nextNbrApMac);

/* End neighbor AP APIs */

/* Start neighbor client APIs */

/*********************************************************************
*
* @purpose  Get the received signal strength of neighbor client.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
* @param    L7_uint32         *rssi      @b{(output)} pointer to indicator
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrClientRSSIGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                               L7_enetMacAddr_t nbrCltMac, L7_uint32 *rssi);

/*********************************************************************
*
* @purpose  Get the channel the neighbor client frame was received on.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
* @param    L7_uchar8        *channel   @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrClientChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                                  L7_enetMacAddr_t nbrCltMac, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the status of neighbor client.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} AP mac address
* @param    L7_uchar8                  radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t           nbrCltMac @b{(input)} neighbor client mac
* @param    L7_WDM_CLIENT_NBR_FLAGS_t *status    @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrClientStatusGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                                 L7_enetMacAddr_t nbrCltMac, L7_WDM_CLIENT_NBR_FLAGS_t *status);

/*********************************************************************
*
* @purpose  Get the time since the neighbor client was last detected.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
* @param    L7_uint32         *age       @b{(output)} pointer to time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrClientAgeGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                              L7_enetMacAddr_t nbrCltMac, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Verify presence of a neighbor client entry.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrClientEntryGet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf, L7_enetMacAddr_t nbrCltMac);

/*********************************************************************
*
* @purpose  Get the next neighbor client entry.
*
* @param    L7_enetMacAddr_t  macAddr         @b{(input)} AP mac address
* @param    L7_uchar8         radioIf         @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac       @b{(input)} neighbor client mac
* @param    L7_enetMacAddr_t  *nextNbrCltMac  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioNbrClientEntryNextGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                                    L7_enetMacAddr_t nbrCltMac, L7_enetMacAddr_t *nextNbrCltMac);

/* End neighbor client APIs */

/* Start AP association and authentication failure APIs */

/*********************************************************************
*
* @purpose  Get the network IP adddress of AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *ipAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureIPAddrGet(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the IP Address of the WS who detected this AP failure latest
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *switchIPAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureSwitchIPAddrGet(L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *switchIPAddr);

/*********************************************************************
*
* @purpose  Get the MAC Address of the WS who detected this AP failure latest
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *switchMACAddr  @b{(output)} pointer to MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureSwitchMACAddrGet(L7_enetMacAddr_t macAddr,
                            L7_enetMacAddr_t *switchMACAddr);

/*********************************************************************
*
* @purpose  Is the AP failure detected by the local switch or peer switch
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *apLocal  @b{(output)} pointer to is AP Local
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureAPFailLocalGet(L7_enetMacAddr_t macAddr, L7_BOOL *apFailLocal);

/*********************************************************************
*
* @purpose  Get the vendor ID of AP software.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_ushort16       *vendorId  @b{(output)} pointer to vendor id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureVendorIdGet(L7_enetMacAddr_t macAddr, L7_ushort16 *vendorId);

/*********************************************************************
*
* @purpose  Get the protocol version supported by software on AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_ushort16       *prtlVersion  @b{(output)} pointer to prtl version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureProtocolVersionGet(L7_enetMacAddr_t macAddr, L7_ushort16 *prtlVersion);

/*********************************************************************
*
* @purpose  Get the software version on AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         *swVersion  @b{(output)} pointer to sw version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureSoftwareVersionGet(L7_enetMacAddr_t macAddr, L7_uchar8 *swVersion);

/*********************************************************************
*
* @purpose  Get the hardware platform for AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *hwType  @b{(output)} pointer to hw type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureHardwareTypeGet(L7_enetMacAddr_t macAddr, L7_uint32 *hwType);

/*********************************************************************
*
* @purpose  Get the last occurred failure type.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)} AP mac address
* @param    L7_WDM_AP_FAILURE_t  *type    @b{(output)} pointer to type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureTypeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_FAILURE_t *type);

/*********************************************************************
*
* @purpose  Get the number of association failures for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *value   @b{(output)} pointer to value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureValidFailGet(L7_enetMacAddr_t macAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of authentication failures for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *value   @b{(output)} pointer to value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureAuthFailGet(L7_enetMacAddr_t macAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the time since failure occurred.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *age     @b{(output)} pointer to time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Verify presence of a failed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next failed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Purge all failed AP entries.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPFailureEntriesPurge(void);

/* End AP association and authentication failure APIs */

/* Start RF scan AP APIs */

/*********************************************************************
*
* @purpose  Get the service set ID of the detected network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_char8          *ssid    @b{(output)} pointer to ssid
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanSSIDGet(L7_enetMacAddr_t macAddr, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the BSSID for the RF scan entry.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} MAC Address
* @param    L7_enetMacAddr_t  *bssid  @b{(output)} pointer to BSSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanBSSIDGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *bssid);

/*********************************************************************
*
* @purpose  Get the 802.11 mode being used on the AP.
*
* @param    L7_enetMacAddr_t          macAddr  @b{(input)} AP mac address
* @param    L7_WDM_RADIO_PHY_MODE_t  *mode     @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanPhysicalModeGet(L7_enetMacAddr_t macAddr, L7_WDM_RADIO_PHY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the transmit channel of the AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8        *channel  @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the data transmit rate of the AP.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP mac address
* @param    L7_ushort16                  *rate    @b{(output)} pointer to rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanTxRateGet(L7_enetMacAddr_t macAddr, L7_ushort16 *rate);

/*********************************************************************
*
* @purpose  Get the beacon interval for the neighbor AP network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_ushort16       *value   @b{(output)} pointer to value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanBeaconIntervalGet(L7_enetMacAddr_t macAddr, L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the managed AP MAC address for an RF scan entry.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *apMacAddr  @b{(output)} pointer to AP MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP MAC is only set if RF scan status is managed or peer managed.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanAPMacAddrGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *apMacAddr);

/*********************************************************************
*
* @purpose  Get the managed AP MAC radio interface for an RF scan entry.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         *radioIf  @b{(output)} radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP MAC is only set if RF scan status is managed or peer managed.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanAPRadioGet(L7_enetMacAddr_t macAddr, L7_uchar8 *radioIf);

/*********************************************************************
*
* @purpose  Get the managed status of AP.
*
* @param    L7_enetMacAddr_t    macAddr  @b{(input)} AP mac address
* @param    L7_WDM_AP_STATUS_t  *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the initial status of AP.
*
* @param    L7_enetMacAddr_t    macAddr  @b{(input)} AP mac address
* @param    L7_WDM_AP_STATUS_t  *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanStatusInitialGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the time since the AP was first detected in a scan.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *discAge  @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanDiscoveredAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *discAge);

/*********************************************************************
*
* @purpose  Get the time since the AP was last detected in a scan.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *age     @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Get the security mode of the AP.
*
* @param    L7_enetMacAddr_t    macAddr  @b{(input)} AP mac address
* @param    L7_WDM_AP_SECURITY_MODE_t  *mode  @b{(output)} pointer to security mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanSecurityGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_SECURITY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the highest supported rate advertised by this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *rate     @b{(output)} pointer to highest supported rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanHighRateGet(L7_enetMacAddr_t macAddr, L7_char8 *rate);

/*********************************************************************
*
* @purpose  Get the 802.11n support for this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL          *flag     @b{(output)} pointer to 802.11n flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FALSE : Not supported
*           L7_TRUE  : Supported
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanDot11nGet(L7_enetMacAddr_t macAddr, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Was the beacon frame for this AP received from an ad hoc network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL          *flag     @b{(output)} pointer to ad hoc flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FALSE : Not Ad hoc
*           L7_TRUE  : Ad hoc
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanAdHocGet(L7_enetMacAddr_t macAddr, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Is this AP managed by a peer switch.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL          *flag     @b{(output)} pointer to peer switch flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FALSE : AP is managed by the local switch
*           L7_TRUE  : AP is managed by a peer switch
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanPeermanagedGet(L7_enetMacAddr_t macAddr, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Get the WIDS Rogue AP Mitigation Status.
*
* @param    L7_enetMacAddr_t    macAddr  @b{(input)} AP mac address
* @param    L7_WDM_WIDS_ROGUE_AP_STATUS_t  *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanRogueMitigationGet(L7_enetMacAddr_t macAddr, L7_WDM_ROGUE_MITIGATION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Determine if AP supports Radio Resource Measurement (RRM)
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *rrm     @b{(output)} flag for RRM 
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
usmDbWdmRFScanRRMSupportedGet(L7_enetMacAddr_t macAddr, L7_BOOL *rrm);

/*********************************************************************
*
* @purpose  Verify presence of an AP entry detected in an RF scan.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next AP entry detected in an RF scan.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Purge all RF scan entries.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanEntriesPurge(void);

/*********************************************************************
*
* @purpose  Get password to Enable/Disable debug on a managed AP.
*
* @param    L7_enetMacAddr_t   macAddr     @b{(input)}  AP mac address
* @param    L7_char8          *password    @b{(output)} debug password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPDebugPasswordGet(L7_enetMacAddr_t macAddr,
                                          L7_char8        *password);


/*********************************************************************
*
* @purpose  Purge client and AP neighbor lists for all managed APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbManagedAPNbrEntriesPurge();

/*********************************************************************
*
* @purpose  Get the specified RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanTriangulationSentryEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the next RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *idx                   @b{(input)} entry index
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t  usmDbWdmRFScanTriangulationSentryEntryNextGet(L7_enetMacAddr_t macAddr, L7_uint32 *idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the specified RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanTriangulationNonSentryEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the next RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *idx                   @b{(input)} entry index
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t  usmDbWdmRFScanTriangulationNonSentryEntryNextGet(L7_enetMacAddr_t macAddr, L7_uint32 *idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmRFScanTriangulationStatusGet(L7_enetMacAddr_t macAddr, wdmRFScanTriangulationStatus_t *status);

/*********************************************************************
*
* @purpose  Get the WIDS AP Rogue Classification Test Results.
*
* @param    L7_enetMacAddr_t                         macAddr @b{(input)} AP mac address
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t   testId  @b{(input)} rogue classification test identifier
* @param    wdmWidsRogueClassificationTestResults_t *result  @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPRogueClassificationGet(L7_enetMacAddr_t macAddr,
                                         L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId,
                                         wdmWidsRogueClassificationTestResults_t *result);

/*********************************************************************
*
* @purpose  Get the WIDS AP Rogue Classification Test Description.
*
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t   testId  @b{(input)} rogue classification test identifier
*
* @returns  Pointer to the test description
* @returns  L7_NULLPTR for an invalid test index
*
* @comments none
*
* @end
*
*********************************************************************/
L7_char8 *usmDbWdmAPRogueClassificationDescrGet(L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId);

/*********************************************************************
*
* @purpose  Get the next WIDS AP Rogue Classification Test Result.
*
* @param    L7_enetMacAddr_t                         macAddr @b{(input)} AP mac address
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t   testId  @b{(input)} rogue classification test identifier
* @param    wdmWidsRogueClassificationTestResults_t *result  @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPRogueClassificationNextGet(L7_enetMacAddr_t macAddr,
                                             L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t *testId,
                                             wdmWidsRogueClassificationTestResults_t *result);
/*********************************************************************
*
* @purpose  Get the number of clients currently using this AP as
*           Home AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *clients  @b{(output)} pointer to number of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelAPHomeClientsGet(L7_enetMacAddr_t macAddr,
                                           L7_uint32 *clients);
/*********************************************************************
*
* @purpose  Get the number of clients currently using this AP as
*           Associate AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *clients  @b{(output)} pointer to number of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelAPAssocClientsGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *clients);
/*********************************************************************
*
* @purpose  Get the number of tunnels currently formed in this AP
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *tunnels  @b{(output)} pointer to number of tunnels
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelAPTunnelsFormedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *tunnels);
/*********************************************************************
*
* @purpose  Get the number of multicast replications being made by the
*          AP
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *val      @b{(output)} pointer to number
*                                                    of mcast replications
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelAPMcastReplicationsGet(L7_enetMacAddr_t macAddr,
                                                 L7_uint32 *val);
/*********************************************************************
*
* @purpose  Get the vlan which has the maximum number of mutlicast
*           replications for this AP
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *val      @b{(output)} pointer to vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDistTunnelAPMaxMcastReplicationsVlanGet(L7_enetMacAddr_t macAddr,
                                                        L7_uint32 *val);
/* End of L2 Distributed Tunneling APIs */

/*********************************************************************
*
* @purpose  Get the On-Demand-Loction Status for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address.
* @param    L7_uint32  *status  @b{(output)} pointer to the status value. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPOnDemandLocStatusGet(L7_enetMacAddr_t macAddr,
                                   L7_uint32 *status);

#endif /* INCLUDE_USMDB_WDM_AP_API_H */

