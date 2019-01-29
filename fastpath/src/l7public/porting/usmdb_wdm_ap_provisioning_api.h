/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename usmdb_wdm_ap_provisioning_api.h
*
* @purpose  Wireless Data Manager (WDM) USMDB WDM AP Provisioning API functions
*
* @component    WDM
*
* @comments none
*
* @create   08-jan-09
*
* @author   Nagendra Echerlu
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_USMDB_WDM_AP_PROVISIONING_API_H
#define INCLUDE_USMDB_WDM_AP_PROVISIONING_API_H

#include "l7_common.h"
#include "wireless_commdefs.h"

/*********************************************************************
*
* @purpose  Delete an entry from the AP Provisioning Table.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only Unmanaged AP entries can be deleted from the table.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningEntryDelete (L7_enetMacAddr_t macAddr);

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
L7_RC_t usmDbWdmAPProvisioningUnmanagedEntriesPurge ();

/*********************************************************************
*
* @purpose  Find if an entry existing in the AP Provisioning Table.
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
L7_RC_t usmDbWdmAPProvisioningEntryGet (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the next entry from the AP Provisioning Table.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} Next AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningEntryNextGet (L7_enetMacAddr_t macAddr,
                            L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the Mutual Authentication Mode.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_BOOL *mode @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningMutualAuthModeGet (L7_enetMacAddr_t macAddr,
                            L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Get the Reprovisioning in Unmanaged Mode.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_BOOL *mode @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningReprovModeGet (L7_enetMacAddr_t  macAddr,
                            L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Set the profile ID applied to be provisioned for the AP.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 profileId @b{(output)} Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningProfileIdSet(L7_enetMacAddr_t macAddr,
                            L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Get the profile ID applied to Provisioned AP.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 *profileId @b{(output)} pointer to Profile ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningProfileIdGet(L7_enetMacAddr_t macAddr,
                            L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Set the primary IP address of the switch.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t newPriIPAddr @b{(input)}Primary IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningNewPriIPAddrSet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t newPriIPAddr);

/*********************************************************************
*
* @purpose  Get the primary IP address of the switch.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t *newPriIPAddr @b{(output)} pointer to Primary IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningNewPriIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *newPriIPAddr);

/*********************************************************************
*
* @purpose  Set the backup IP address of the switch.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC ADdress
* @param    L7_IP_ADDR_t newBackupIPAddr @b{(input)}Backup IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningNewBackupIPAddrSet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t newBackupIPAddr);

/*********************************************************************
*
* @purpose  Get the backup IP address of the switch.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t *newBackupIPAddr @b{(output)} pointer to backup switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningNewBackupIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *newBackupIPAddr);

/*********************************************************************
*
* @purpose  Set status for AP Provisioning entry.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 status  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningStatusSet (L7_enetMacAddr_t macAddr,
                            L7_uint32 status);

/*********************************************************************
*
* @purpose  Get status for AP Provisioning entry.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 *status  @b{(input)}pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningStatusGet (L7_enetMacAddr_t macAddr,
                            L7_uint32 *status);

/*********************************************************************
*
* @purpose  Set status for Credential transimission.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 txStatus  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningCredTxSet (L7_enetMacAddr_t macAddr,
                            L7_uint32 txStatus);

/*********************************************************************
*
* @purpose  Get status for Credential transmission.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 *txStatus  @b{(input)}pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningCredTxGet (L7_enetMacAddr_t macAddr,
                            L7_uint32 *txStatus);

/*********************************************************************
*
* @purpose  Get the IP address of the AP provisioning entry.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t *ipAddr @b{(output)} pointer to IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the configured primary IP address of the switch.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t *cfgPriIPAddr @b{(output)} pointer to Primary IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningCfgPriIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *cfgPriIPAddr);

/*********************************************************************
*
* @purpose  Get the configured Backup IP address of the switch.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t *cfgBackIPAddr @b{(output)} pointer to Backup IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPProvisioningCfgBackupIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *cfgBackIPAddr);

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
L7_RC_t usmDbWdmAPProvisioningAgeGet (L7_enetMacAddr_t macAddr, L7_uint32 *age);

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
L7_RC_t usmDbWdmAPProvisioningStart (L7_enetMacAddr_t macAddr);

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
L7_RC_t usmDbWdmAPProvisioningAllStart ();

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
L7_RC_t usmDbWdmAPProvisioningCountGet (L7_uint32 *count);

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
L7_RC_t usmDbWdmAPProvisioningMaxNumOfEntriesGet (L7_uint32 *value);

#endif /* INCLUDE_USMDB_WDM_AP_PROVISIONING_API_H */

