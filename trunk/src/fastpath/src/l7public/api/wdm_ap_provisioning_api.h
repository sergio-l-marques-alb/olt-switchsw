/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2009
*
*
* @filename   wdm_ap_provisioning_api.h
*
* @purpose    Wireless Data Manager Access Point (WDM AP) API header
*
* @component  WDM
*
* @comments   none
*
* @create     
*
* @author   
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_WDM_AP_PROVISIONING_API_H
#define INCLUDE_WDM_AP_PROVISIONING_API_H

#include "datatypes.h"
#include "osapi.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "captive_portal_commdefs.h"
#include "intf_cb_api.h"

/* Following APIs can be called via usmDb and wireless application components. */

/*********************************************************************
*
* @purpose  Update a AP Provisioning  entry.
*
* @param    L7_enetMacAddr_t      macAddr    @b{(input)} AP mac address
* @param    L7_IP_ADDR_t          ipAddr     @b{(input)} ip address
*                                                                     address
* @param    L7_IP_ADDR_t          cfgPrimaryIPAddr @b{(input)} primary ip
*                                                                     address
* @param    L7_IP_ADDR_t          cfgBackupIPAddr  @b{(input)} backup ip
*                                                                     address
* @param    L7_BOOL               cfgMutualAuthMode @b{(input)} mutual
*                                                                     auth mode
* @param    L7_BOOL               cfgUnmanagedReprovMode @b{(input)} unmanaged
*                                                                     auth mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function adds a new entry or updates an existing entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningEntryAddUpdate(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t ipAddr,
                                        L7_IP_ADDR_t cfgPrimaryIPAddr,
                                        L7_IP_ADDR_t cfgBackupIPAddr,
                                        L7_BOOL cfgMutualAuthMode,
                                        L7_BOOL cfgUnmanagedReprovMode);
/*********************************************************************
*
* @purpose  Delete an AP Provisioning entry.
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
L7_RC_t wdmAPProvisioningEntryDelete(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete all AP entries from the AP Provisioning Table.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only Unmanaged AP entries can be deleted from the table.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningEntriesPurge ();

/*********************************************************************
*
* @purpose  Delete all unmanaged AP entries from the AP Provisioning Table.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only Unmanaged AP entries can be deleted from the table.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningUnmanagedEntriesPurge ();

/*********************************************************************
*
* @purpose  Verify presence of a AP Provisioning entry.
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
L7_RC_t wdmAPProvisioningEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next provisioned AP entry.
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
L7_RC_t wdmAPProvisioningEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the Mutual Authentication Mode.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_BOOL           *mode @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t wdmAPProvisioningMutualAuthModeGet (L7_enetMacAddr_t  macAddr,
                                            L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Get the Reprovisioning in Unmanaged Mode.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_BOOL           *mode @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningReprovModeGet (L7_enetMacAddr_t  macAddr,
                                            L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Get the X509 certificate for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8        *X509Cert  @b{(output)} Pointer to X509 cert.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningX509CertGet (L7_enetMacAddr_t macAddr,
                            L7_uchar8 *x509Cert);

/*********************************************************************
*
* @purpose  Set the X509 certificate for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8        *X509Cert  @b{(output)}  pointer to X509 cert.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningX509CertSet (L7_enetMacAddr_t macAddr,
                            L7_uchar8 *x509Cert);

/*********************************************************************
*
* @purpose  Set the profile ID applied to Provisioned AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32         profileId  @b{(output)} profile id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningProfileIdSet(L7_enetMacAddr_t macAddr, L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Get the profile ID applied to Provisioned AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32        *profileId  @b{(output)} pointer to profile id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningProfileIdGet(L7_enetMacAddr_t macAddr, L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Set the primary IP address of the switch.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      newPriIPAddr @b{(input)}Primary IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningNewPriIPAddrSet (L7_enetMacAddr_t  macAddr,
                                          L7_IP_ADDR_t      newPriIPAddr);

/*********************************************************************
*
* @purpose  Get the primary IP address of the switch.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *newPriIPAddr @b{(output)} pointer to Primary IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningNewPriIPAddrGet (L7_enetMacAddr_t  macAddr,
                                          L7_IP_ADDR_t     *newPriIPAddr);

/*********************************************************************
*
* @purpose  Set the backup IP address of the switch.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      newBackupIPAddr @b{(input)}Backup IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningNewBackupIPAddrSet (L7_enetMacAddr_t  macAddr,
                                             L7_IP_ADDR_t      newBackupIPAddr);

/*********************************************************************
*
* @purpose  Get the backup IP address of the switch.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t     *newBackupIPAddr @b{(output)} pointer to backup switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningNewBackupIPAddrGet (L7_enetMacAddr_t  macAddr,
                                          L7_IP_ADDR_t     *newBackupIPAddr);

/*********************************************************************
*
* @purpose  Set status for AP Provisioning entry.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  status  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningStatusSet(L7_enetMacAddr_t macAddr,L7_uint32 status);

/*********************************************************************
*
* @purpose  Get status for AP Provisioning entry.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  *status  @b{(input)}pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningStatusGet(L7_enetMacAddr_t macAddr,L7_uint32 *status);

/*********************************************************************
*
* @purpose  Set status for Credential transimission.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  txStatus  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCredTxSet(L7_enetMacAddr_t macAddr,L7_uint32 txStatus);

/*********************************************************************
*
* @purpose  Get status for Credential transmission.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  *txStatus  @b{(input)}pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCredTxGet(L7_enetMacAddr_t macAddr,L7_uint32 *txStatus);

/*********************************************************************
*
* @purpose  Get the AP IP address of provisioning entry.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *cfgPriIPAddr @b{(output)} pointer to IP address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningIPAddrGet (L7_enetMacAddr_t  macAddr,
                            L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the configured primary IP address of the switch.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *cfgPriIPAddr @b{(output)} pointer to Primary IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCfgPriIPAddrGet (L7_enetMacAddr_t  macAddr,
                                          L7_IP_ADDR_t     *cfgPriIPAddr);

/*********************************************************************
*
* @purpose  Get the configured Backup IP address of the switch.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *cfgBackIPAddr @b{(output)} pointer to Backup IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCfgBackupIPAddrGet (L7_enetMacAddr_t  macAddr,
                                             L7_IP_ADDR_t     *cfgBackIPAddr);

/*********************************************************************
*
* @purpose  Get the time since last communication for this AP.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} AP MAC Address
* @param    L7_uint32 *age     @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningAgeGet (L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Set AP Provisioning comand send time stamp to AP.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  timeStamp  @b{(input)} Time stamp 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCommandSendTimeSet(L7_enetMacAddr_t macAddr, 
                                            L7_uint32 timeStamp);  

/*********************************************************************
*
* @purpose  Get AP Provisioning comand send time stamp to AP.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  timeStamp  @b{(output)} Time stamp 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCommandSendTimeGet(L7_enetMacAddr_t macAddr, 
                                            L7_uint32 *timeStamp);
/*********************************************************************
*
* @purpose  Set AP Provisioning start time stamp to AP.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  timeStamp  @b{(input)} Time stamp 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningStartTimeSet(L7_enetMacAddr_t macAddr, 
                                      L7_uint32 timeStamp);

/*********************************************************************
*
* @purpose  Get AP Provisioning start time stamp to AP.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_uint32                  timeStamp  @b{(output)} Time stamp 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningStartTimeGet(L7_enetMacAddr_t macAddr, 
                                      L7_uint32 *timeStamp);


/*********************************************************************
*
* @purpose  Start the provisioning of the AP.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningStart (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Start provisioning of all APs.
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
L7_RC_t wdmAPProvisioningAllStart ();

/*********************************************************************
*
* @purpose  Get the number of entries in the AP Provisioning database.
*
* @param    L7_uint32 *count @b{(output)} pointer to count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningCountGet (L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get the maximum entries in the AP Provisioning database
*
* @param    L7_uint32 *value @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningMaxNumOfEntriesGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the next entry from AP provision database flagged for a pending action.
*
* @param    L7_enetMacAddr_t                macAddr     @b{(input)}  last AP MAC flagged
* @param    L7_WDM_AP_PROVISIONING_FLAG_t   flag        @b((input))  type of pending entry to lookup
* @param    L7_enetMacAddr_t                *nextMacAddr @b{(output)} next AP MAC flagged
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first pending entry for the 
*           L7_WDM_AP_PROVISIONING_FLAG_START flag. The entry pending flag for 
*           the returned macAddr is cleared.
*
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningPendingEntryNextGet(L7_enetMacAddr_t    macAddr,
                                             L7_WDM_AP_PROVISIONING_FLAG_t    flag,
                                             L7_enetMacAddr_t     *nextMacAddr);
/*********************************************************************
*
* @purpose  Set global AP provisioning status.
*
* @param    L7_uint32                  status  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmGlobalAPProvisioningStatusSet(L7_uint32 status);
    
/*********************************************************************
*
* @purpose  Get global AP Provisioning status.
*
* @param    L7_uint32                  *status  @b{(output)}pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmGlobalAPProvisioningStatusGet(L7_uint32 *status);

/*********************************************************************
*
* @purpose  Get AP count pending in provisioning progress state.
*
* @param    L7_uint32       apCount  @b{(input)} AP count 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningInProgresssCountGet(L7_uint32 *apCount);

#endif /*INCLUDE_WDM_AP_PROVISIONING_API_H */

