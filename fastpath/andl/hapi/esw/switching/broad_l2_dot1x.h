/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: broad_l2_dot1x.h
*
* Purpose: Implement Layer-2 Dot1x HAPI code
*
* Component: hapi
*
* Comments:
*
* Created by: colinw 4/16/07
*
*********************************************************************/
#include <commdefs.h>

#ifndef L7_DOT1X_PORT_MAX_MAC_USERS
#define L7_DOT1X_PORT_MAX_MAC_USERS 16
#endif

/* Number of bytes in mask */
#define HAPI_DOT1X_GROUP_INDICES   ((L7_DOT1X_PORT_MAX_MAC_USERS - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct
{
  L7_uchar8 value[HAPI_DOT1X_GROUP_INDICES];
} HAPI_DOT1X_GROUP_MASK_t;

/* SETMASKBIT turns on bit index # k in mask j. */
#define HAPI_DOT1X_GROUP_SETMASKBIT(j, k)                                    \
            ((j).value[((k)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k) % (8*sizeof(L7_uchar8))))   



/* CLRMASKBIT turns off bit index # k in mask j. */
#define HAPI_DOT1X_GROUP_CLRMASKBIT(j, k)                                    \
           ((j).value[((k)/(8*sizeof(L7_uchar8)))]   \
                        &= ~(1 << ((k) % (8*sizeof(L7_uchar8)))))      

/* The groupId of zero is reserved for cases where we do not
   want a client to match any policies on that port. */
#define HAPI_DOT1X_GROUP_ID_INVALID  0

typedef struct
{
  L7_BOOL                     inUse;
  L7_enetMacAddr_t            macAddr;
  L7_ushort16                 vid;     /* for VLAN assignment */
  L7_uint32                   vlanAssignmentPolicyId;
  L7_uint32                   policyId;
  L7_uint32                   groupId; /* The group ID to qualify on in this clients policy.
                                          The L2FDB entry will be written with this group ID. */
  L7_uint32                   tlvSignature;
} HAPI_DOT1X_CLIENT_t;

typedef struct
{
  L7_DOT1X_PORT_STATUS_t      dot1xStatus;
  L7_ushort16                 authorizedClientCount; /* relevant for MAC based dot1x */
  HAPI_DOT1X_CLIENT_t         client[L7_DOT1X_PORT_MAX_MAC_USERS];
  HAPI_DOT1X_GROUP_MASK_t     groupMask;
  L7_BOOL                     violationCallbackEnabled;
} HAPI_DOT1X_PORT_t;

#define HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr) \
    ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED) || \
     (hapiPortPtr->dot1x.authorizedClientCount > 0))

/*********************************************************************
*
* @purpose Determines if Dot1x VLAN assignment feature is supported.
*
* @param   
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadDot1xVlanAssignmentSupported();

/*********************************************************************
*
* @purpose Determines if Dot1x Policy feature is supported.
*
* @param   
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadDot1xPolicySupported();

/*********************************************************************
*
* @purpose Determines if any dot1x policies have been applied to this port.
*
* @param   
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadDot1xPoliciesApplied(DAPI_USP_t *usp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Determines if the client specified by macAddr is authorized
*          on this port.
*
* @param   
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadDot1xIsAuthorizedClient(DAPI_USP_t *usp, L7_uchar8 *macAddr, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Update the L2FDB as appropriate to allow authorized clients
*          access to this VLAN.
*
* @param   
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDot1xPortVlanAddNotify (DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Update the L2FDB as appropriate to disallow authorized clients
*          access to this VLAN.
*
* @param   
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDot1xPortVlanRemoveNotify (DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose Initializes Dot1x code
 *
 * @param *dapi_g          system information
 *
 * @returns L7_RC_t result
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDot1xInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Configure 802.1x by either enabling or disabling 
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_SYSTEM_DOT1X_CONFIG
* @param   *data          @b{(input)} DAPI_SYSTEM_CMD_t.cmdData.dot1xConfig 
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1xConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose  802.1x : Set the authorization status of the specified usp
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_STATUS
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xStatus 
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xStatus(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  802.1x : Authorize the specified client and possible apply
*                    a policy for that client.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_ADD
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientAdd
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  802.1x : Unauthorize the specified client and remove applicable
*                    policies for that client.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientRemove
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose  802.1x : Check if client has timeout i.e. check if the BCM_SRC_HIT flag 
*                    has been set for that client MAC Address
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientTimeout
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Sets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientTimeout(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  802.1x : Configure HW to drop packets from specified MAC/VLAN pair
*                    on the specified interface.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientTimeout
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientBlock(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  802.1x : Configure HW to allow packets from specified MAC/VLAN pair
*                    on the specified interface.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientUnblock
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientUnblock(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Take semaphore to protect dot1x resources
*
* @param   
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1xSemTake();

/*********************************************************************
*
* @purpose Give semaphore to protect dot1x resources
*
* @param   
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1xSemGive();

/*********************************************************************
 *
 * @purpose  802.1x : Create the violation policy.
 *
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDot1xViolationPolicyCreate(DAPI_t *dapi_g);
