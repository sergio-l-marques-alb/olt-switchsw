/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_wds_api.h
*
* @purpose    Wireless WDM WDS Managed AP USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     05/01/09
*
* @author     pmchakri
*
* @end
*             
*********************************************************************/
#ifndef INCLUDE_USMDB_WDM_WDS_GROUP_API_H
#define INCLUDE_USMDB_WDM_WDS_GROUP_API_H

#include "l7_common.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "acl_exports.h"
#include "wireless_commdefs.h"

/*********************************************************************
*
* @purpose  Add an entry to WDS AP Group Table.
*
* @param    L7_uchar8 groupId @b{(input)} WDS AP group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupEntryAdd(L7_uchar8 groupId);

/*********************************************************************
*
* @purpose  Delete an entry in the WDS Managed AP Group Table.
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupEntryDelete(L7_uchar8 groupId);

/*********************************************************************
*
* @purpose  Find if an entry existing in the WDS Managed AP Group Table
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupEntryGet(L7_uchar8 groupId);

/*********************************************************************
*
* @purpose  Get the next entry from the WDS managed AP Group Table
*
* @param    L7_uint32 *profileId @b{(input)} WDS Managed AP Group ID
* @param    L7_uint32 *profileId @b{(output)} Next WDS Managed AP Group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupEntryNextGet(L7_uchar8 *groupId);

/*********************************************************************
*
* @purpose  Set the Group Name for a WDS Managed AP Group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 *name @b{(input)} Group Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupNameSet(L7_uchar8 groupId, L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the Group Name for a WDS Managed AP Group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 *name @b{(input)} Group Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupNameGet(L7_uchar8 groupId, L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the spanning tree mode for a WDS Managed AP Group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_uchar8 status @b{(input)} Spanning tree status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupSpanningTreeModeSet (L7_uchar8 groupId, L7_uchar8 mode);

/*********************************************************************
*
* @purpose  Set the spanning tree mode for a WDS Managed AP Group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_uchar8 *status @b{(input)} Spanning tree status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupSpanningTreeModeGet (L7_uchar8 groupId, L7_uchar8 *mode);

/*********************************************************************
*
* @purpose  Set the password for a WDS Managed AP Group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 password @b{(input)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupPasswordSet(L7_uchar8 groupId, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Get the password for the WDS managed AP group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 *status @b{(input)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupPasswordGet (L7_uchar8 groupId, L7_char8 *password,L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Set the new password for a WDS Managed AP Group
*
* @param    L7_uint32 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 new password @b{(input)} password
* @param    L7_BOOL          encrypted @b{(input)} L7_TRUE if password in encrypted format
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupNewPasswordSet (L7_uint32 groupId, L7_char8 *newPassword,L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Get the new password for the WDS managed AP group
*
* @param    L7_uint32 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 *newPassword @b{(input)} new password
* @param    L7_BOOL          encrypted @b{(input)} L7_TRUE if new password in encrypted format
* @param    L7_BOOL          *pwdConfig @b{(output)} L7_TRUE if new password is configured
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupNewPasswordGet (L7_uint32 groupId, L7_char8 *newPassword, 
                                          L7_BOOL encrypted, L7_BOOL *pwdConfig);

/*********************************************************************
*
* @purpose  Set the status for password change
*
* @param    L7_uint32 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_WDM_WDS_CHANGE_PASSWORD_STATUS_t status @b{(input)} password change status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupPasswordStatusSet (L7_uint32 groupId,
                                        L7_char8 status);

/*********************************************************************
*
* @purpose  Get the password for a WDS Managed AP Group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_char8 *status @b{(input)} Spanning tree status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupPasswordStatusGet (L7_uint32 groupId, 
                                             L7_char8 *status);

/*********************************************************************
*
* @purpose  Get the number of APs configured in this group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_uchar8 *configured @b{(input)} Num of configured APs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupConfiguredApsGet(L7_uchar8 groupId, L7_uint32 *configuredAps);

/*********************************************************************
*
* @purpose  Get the number of APs connected in this group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_uchar8 *connectedAPs @b{(input)} Num of connected APs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupConnectedApsGet (L7_uchar8 groupId, L7_uint32 *connectedAps);
/*********************************************************************
*
* @purpose  Get the number of root APs in this group
*
* @param    L7_uchar8 groupId @b{(input)} WDS Managed AP Group ID
* @param    L7_uchar8 *numRootAps @b{(input)} Num of Root APs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmWdsApGroupNumRootApsGet(L7_uchar8 groupId, L7_uint32 *numRootAps);
#endif

