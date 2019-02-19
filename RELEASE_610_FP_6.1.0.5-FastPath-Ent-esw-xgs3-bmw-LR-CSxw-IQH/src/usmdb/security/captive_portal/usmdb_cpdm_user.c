/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  usmdb_cpdm_user.c
*
* @purpose   Captive Portal Data Manager (CPDM) USMDB API functions
*
* @component CPDM
*
* @comments  none
*
* @create    07/26/2007
*
* @author    rjindal,darsen
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "usmdb_cpdm_user_api.h"
#include "osapi.h"
#include "captive_portal_commdefs.h"
#include "cpdm_user_api.h"


/*********************************************************************
*
* @purpose  Add a new user entry
*
* @param    uId_t uId @b{(input)} user id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ALREADY_CONFIGURED
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryAdd(uId_t uId)
{
  return cpdmUserEntryAdd(uId);
}

/*********************************************************************
*
* @purpose  Delete existing user entry
*
* @param    uId_t uId @b{(input)} user id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Also de-authenticate the user if connected 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryDelete(uId_t uId)
{
  return cpdmUserEntryDelete(uId);
}

/*********************************************************************
*
* @purpose  Retrieve current count of user entries
*
* @param    L7_uint32 *count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryCountGet(L7_uint32 *count)
{
  return cpdmUserEntryCountGet(count);
}

/*********************************************************************
*
* @purpose  Check if user exists
*
* @param    uId_t uId @b{(input)} user Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryGet(uId_t uId)
{
  return cpdmUserEntryGet(uId);
}

/*********************************************************************
*
* @purpose  Get the next user entry
*
* @param    uId_t uId @b{(input)} current user id 
* @param    uId_t * nextUId @b{(output)} next user id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryNextGet(uId_t uId, uId_t *nextUId)
{
  return cpdmUserEntryNextGet(uId, nextUId);
}

/*********************************************************************
*
* @purpose  Check if user exists
*
* @param    L7_char8 *loginName @b{(input)} user name
* @param    uId_t uId @b{(output)} assigned user id 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryByNameGet(L7_char8 *loginName, uId_t *uId)
{
  return cpdmUserEntryByNameGet(loginName, uId);
}

/*********************************************************************
*
* @purpose  Set a new user/login name
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *userName @b{(input)} the new login name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryLoginNameSet(uId_t uId, L7_char8 *loginName)
{
  return cpdmUserEntryLoginNameSet(uId, loginName);
}

/*********************************************************************
*
* @purpose  Set a new user password
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *password @b{(input)} the new password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryPasswordSet(uId_t uId, L7_char8 *password)
{
  return cpdmUserEntryPasswordSet(uId, password);
}

/*********************************************************************
* @purpose  Set a new user encryption password
*
* @param    uId_t uid                   (input) user id table index 
* @param    L7_char8 *EncryptedPassword (input) encrypted password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryEncryptedPasswordSet(uId_t uId, L7_char8 *encryptedPassword)
{
  return cpdmUserEntryEncryptedPasswordSet(uId, encryptedPassword);
}


/*********************************************************************
*
* @purpose  Set the session timeout. This is the total session time
*           allowed before being logged out
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 sessionTimeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntrySessionTimeoutSet(uId_t uId, L7_uint32 sessionTimeout)
{
  return cpdmUserEntrySessionTimeoutSet(uId, sessionTimeout);
}

/*********************************************************************
*
* @purpose  Set the idle timeout. The session is logged out if the
*           session idle time is met.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 idleTimeout @b{(input)} idle in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryIdleTimeoutSet(uId_t uId, L7_uint32 idleTimeout)
{
  return cpdmUserEntryIdleTimeoutSet(uId, idleTimeout);
}

/*********************************************************************
*
* @purpose  Set maximum client transmit rate. This limits the bandwidth
*           at which the client can send data into the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxBandwidthUp @b{(input)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxBandwidthUpSet(uId_t uId, L7_uint32 maxBandwidthUp)
{
  return cpdmUserEntryMaxBandwidthUpSet(uId, maxBandwidthUp);
}

/*********************************************************************
*
* @purpose  Set maximum client receive rate. This limits the bandwidth
*           at which the client can receive data from the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxBandwidthDown @b{(input)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxBandwidthDownSet(uId_t uId, L7_uint32 maxBandwidthDown)
{
  return cpdmUserEntryMaxBandwidthDownSet(uId, maxBandwidthDown);
}

/*********************************************************************
*
* @purpose  Set maximum number of octets the user is allowed to
*           transmit. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxInputOctets @b{(input)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxInputOctetsSet(uId_t uId, L7_uint32 maxInputOctets)
{
  return cpdmUserEntryMaxInputOctetsSet(uId, maxInputOctets);
}

/*********************************************************************
*
* @purpose  Set maximum number of octets the user is allowed to
*           receive. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxOutputOctets @b{(input)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxOutputOctetsSet(uId_t uId, L7_uint32 maxOutputOctets)
{
  return cpdmUserEntryMaxOutputOctetsSet(uId, maxOutputOctets);
}

/*********************************************************************
*
* @purpose  Set the maximum number of octets the user is allowed to
*           transmit and receieve. After this limit has been reached,
*           the user will be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxTotalOctets @b{(input)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxTotalOctetsSet(uId_t uId, L7_uint32 maxTotalOctets)
{
  return cpdmUserEntryMaxTotalOctetsSet(uId, maxTotalOctets);
}

/*********************************************************************
*
* @purpose  Get the login name
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *loginName  @b{(output)} the login name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryLoginNameGet(uId_t uId, L7_char8 *loginName)
{
  return cpdmUserEntryLoginNameGet(uId, loginName);
}

/*********************************************************************
*
* @purpose  Get the user password
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *password  @b{(output)} the password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryPasswordGet(uId_t uId, L7_char8 *password)
{
  return cpdmUserEntryPasswordGet(uId, password);
}

/*********************************************************************
* @purpose  Get the user password in encrypted format
*
* @param    uId_t    uId            @b{(input)} user id
* @param    L7_char8 *encryptedPwd  @b{(output)} encrypted password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryEncryptedPasswordGet(uId_t uId, L7_char8 *encryptedPwd)
{
  return cpdmUserEntryEncryptedPasswordGet(uId, encryptedPwd);
}

/*********************************************************************
*
* @purpose  Get the session timeout. This is the total session time
*           allowed before being logged out
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *sessionTimeout @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntrySessionTimeoutGet(uId_t uId, L7_uint32 *sessionTimeout)
{
  return cpdmUserEntrySessionTimeoutGet(uId, sessionTimeout);
}

/*********************************************************************
*
* @purpose  Get the idle timeout. The session is logged out if the
*           session idle time is met.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *idleTimeout @b{(output)} idle in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryIdleTimeoutGet(uId_t uId, L7_uint32 *idleTimeout)
{
  return cpdmUserEntryIdleTimeoutGet(uId, idleTimeout);
}

/*********************************************************************
*
* @purpose  Get maximum client transmit rate. This limits the bandwidth
*           at which the client can send data into the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxBandwidthUp @b{(output)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxBandwidthUpGet(uId_t uId, L7_uint32 *maxBandwidthUp)
{
  return cpdmUserEntryMaxBandwidthUpGet(uId, maxBandwidthUp);
}

/*********************************************************************
*
* @purpose  Get maximum client receive rate. This limits the bandwidth
*           at which the client can receive data from the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxBandwidthDown @b{(output)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxBandwidthDownGet(uId_t uId, L7_uint32 *maxBandwidthDown)
{
  return cpdmUserEntryMaxBandwidthDownGet(uId, maxBandwidthDown);
}

/*********************************************************************
*
* @purpose  Get maximum number of octets the user is allowed to
*           transmit. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxInputOctets @b{(output)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxInputOctetsGet(uId_t uId, L7_uint32 *maxInputOctets)
{
  return cpdmUserEntryMaxInputOctetsGet(uId, maxInputOctets);
}

/*********************************************************************
*
* @purpose  Get maximum number of octets the user is allowed to
*           receive. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxOutputOctets @b{(output)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxOutputOctetsGet(uId_t uId, L7_uint32 *maxOutputOctets)
{
  return cpdmUserEntryMaxOutputOctetsGet(uId, maxOutputOctets);
}

/*********************************************************************
*
* @purpose  Get the maximum number of octets the user is allowed to
*           transmit and receive. After this limit has been reached,
*           the user will be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxTotalOctets @b{(output)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryMaxTotalOctetsGet(uId_t uId, L7_uint32 *maxTotalOctets)
{
  return cpdmUserEntryMaxTotalOctetsGet(uId, maxTotalOctets);
}

/*********************************************************************
*
* @purpose  Delete all user entries with extreme prejudice
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Will also wipe out all active connections 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserEntryPurge(void)
{
  return cpdmUserEntryPurge();
}


/*********************************************************************
* USER GROUP ASSOCIATION RELATED APIs
*********************************************************************/

/*********************************************************************
*
* @purpose  Add a user-group association to the assoc table.
*
* @param    uId_t uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryAdd(uId_t uId, gpId_t gpId)
{
  return cpdmUserGroupAssocEntryAdd(uId, gpId);
}

/*********************************************************************
*
* @purpose  Delete a user-group association from the assoc table.
*
* @param    uId_t uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryDelete(uId_t uId, gpId_t gpId)
{
  return cpdmUserGroupAssocEntryDelete(uId, gpId);
}

/*********************************************************************
*
* @purpose  Delete all user group associations from the assoc
*           table. This function basically loops through the table and
*           deletes all the entries for the given user id
*
* @param    uId_t uId @b{(input)} User Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(uId_t uId)
{
  return cpdmUserGroupAssocEntryByUIdDeleteAll(uId);
}

/*********************************************************************
*
* @purpose  Delete all user group associations from the assoc
*           table. This function basically loops through the table and
*           deletes all the entries for the given group id
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryByGpIdDeleteAll(gpId_t gpId)
{
  return cpdmUserGroupAssocEntryByGpIdDeleteAll(gpId);
}

/*********************************************************************
*
* @purpose  Check if group id exists.
*
* @param    uId_t uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryGet(uId_t uId, gpId_t gpId)
{
  return cpdmUserGroupAssocEntryGet(uId, gpId);
}

/*********************************************************************
*
* @purpose  Check if user and group association exists.
*
* @param    uId_t  uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
* @param    uId_t  *nextUId @b{(output)} Next user Id iteration
* @param    gpId_t *mextGpId @b{(output)} Next group Id iteration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryNextGet(uId_t uId, gpId_t gpId,
                                       uId_t *nextUId, gpId_t *nextGpId)
{
    return cpdmUserGroupAssocEntryNextGet(uId, gpId, nextUId, nextGpId);
}

/*********************************************************************
*
* @purpose  Rename all matching group ids.
*
* @param    gpId_t oldGpId @b{(input)} existing group Id
* @param    gpId_t newGpId @b{(input)} replacement group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupAssocEntryGroupRename(gpId_t oldGpId, gpId_t newGpId)
{
  return cpdmUserGroupAssocEntryGroupRename(oldGpId, newGpId);
}

/*********************************************************************
* USER GROUP RELATED APIs
*********************************************************************/


/*********************************************************************
*
* @purpose  Add an entry to the Group table
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryAdd(gpId_t gpId)
{
  return cpdmUserGroupEntryAdd(gpId);
}

/*********************************************************************
*
* @purpose  Delete an entry from the Group table
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Although we'll also remove the associations from the
*           user group association table, we don't de-authenticate
*           existing users.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryDelete(gpId_t gpId)  
{
  return cpdmUserGroupEntryDelete(gpId);
}

/*********************************************************************
*
* @purpose  Check if group exists
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryGet(gpId_t gpId)  
{
  return cpdmUserGroupEntryGet(gpId);
}

/*********************************************************************
*
* @purpose  Get the associated group id for given name
*
* @param    L7_char8 *groupName @b{(input)} Group name
* @param    gpId_t *gpId @b{(output)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryByNameGet(L7_char8 *groupName, gpId_t *gpId)
{
  return cpdmUserGroupEntryByNameGet(groupName, gpId);
}

/*********************************************************************
*
* @purpose  Get the next group entry
*
* @param    gpId_t gpId @b{(input)} current GP ID 
* @param    gpId_t * nextGpId @b{(output)} next GP ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryNextGet(gpId_t gpId, gpId_t *nextGpId) 
{
  return cpdmUserGroupEntryNextGet(gpId, nextGpId);
}

/*********************************************************************
*
* @purpose  Get group name using the GP ID
*
* @param    gpId_t gpId @b{(input)} current GP ID 
* @param    L7_char8 *groupName @b{(output)} group name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryNameGet(gpId_t gpId, L7_char8 *groupName)
{
  return cpdmUserGroupEntryNameGet(gpId, groupName);
}

/*********************************************************************
*
* @purpose  Set the group name for the given GP ID
*
* @param    gpId_t gpId @b{(input)} current GP ID 
* @param    gL7_char8 *groupName @b{(input)} group name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmUserGroupEntryNameSet(gpId_t gpId, L7_char8 *groupName)
{
  return cpdmUserGroupEntryNameSet(gpId, groupName);
}

