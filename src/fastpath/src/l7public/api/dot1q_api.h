/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dot1q_api.h
*
* @purpose dot1q APIs
*
* @component dot1Q
*
* @comments none
*
* @create 08/14/2000
*
* @author wjacobs
* @end
*
**********************************************************************/

#ifndef _DOT1Q_API_H_
#define _DOT1Q_API_H_

#include "l7_common.h"
#include "dot1q_exports.h"
#include "l7_product.h"
#include "comm_mask.h"
#include "nimapi.h"
#include "dtlapi.h"

#include "dot1q_exports.h"


#define DOT1Q_DEFAULT_VLAN        FD_DOT1Q_DEFAULT_VLAN    /* Default VLAN */

typedef enum
{
  DOT1Q_ADMIN,
  DOT1Q_GVRP,
  DOT1Q_DOT1X,
  DOT1Q_VOICE_VLAN,
  DOT1Q_WS_L2TUNNEL,
  DOT1Q_IP_INTERNAL_VLAN,
  DOT1Q_LAST_DYNAMIC_VLAN_COMPONENT  /* Keep it as last one in the enum list */
} vlanRequestor_t;

/* Number of bytes in mask */
#define DOT1Q_DYNAMIC_VLAN_COMPONENT_INDICES   ((DOT1Q_LAST_DYNAMIC_VLAN_COMPONENT- 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Component Mask Storage */
typedef struct {
        L7_uchar8 value[DOT1Q_DYNAMIC_VLAN_COMPONENT_INDICES];
} DOT1Q_DYNAMIC_VLAN_CREATIOR_MASK_t;

#define COMPONENT_ACQUIRED_DYNAMIC_VLAN_MASK_t DOT1Q_DYNAMIC_VLAN_CREATIOR_MASK_t 

typedef COMPONENT_ACQUIRED_DYNAMIC_VLAN_MASK_t dynamicVlanAcquiredMask;  /*  Mask of components which have 
                                                     acquired the dynamic vlan */


/*
 * NONZEROMASK returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define DOT1Q_DYNAMICVLAN_ACQ_NONZEROMASK(mask, result){         \
    L7_uint32 _i_;                                                    \
    DOT1Q_DYNAMIC_VLAN_CREATIOR_MASK_t *_p_;                          \
                                                                      \
    _p_ = (DOT1Q_DYNAMIC_VLAN_CREATIOR_MASK_t *)&(mask);              \
    for(_i_ = 0; _i_ < DOT1Q_DYNAMIC_VLAN_COMPONENT_INDICES; _i_++)   \
        if(_p_ -> value[_i_]){                                        \
            result = L7_TRUE;                                         \
            break;                                                    \
        }                                                             \
        else                                                          \
            result = L7_FALSE;                                        \
}

/* Least significant bit/rightmost bit is lowest interface # */

/* SETMASKBIT turns on bit index # k in mask j. */
#define DOT1Q_SETMASKBIT(j, k)                                      \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                   \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))



/* CLRMASKBIT turns off bit index # k in mask j. */
#define DOT1Q_CLRMASKBIT(j, k)                                      \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                    \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))


/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define DOT1Q_ISMASKBITSET(j, k)                                    \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                       \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )


#define DOT1Q_ACQ_SETMASKBIT  DOT1Q_SETMASKBIT
#define DOT1Q_ACQ_CLRMASKBIT DOT1Q_CLRMASKBIT
#define DOT1Q_ACQ_ISMASKBITSET  DOT1Q_ISMASKBITSET


/* VLAN outcall notification events */
typedef enum
{
  VLAN_ADD_NOTIFY = 0x00000001,     /* Create a new VLAN */
  VLAN_DELETE_PENDING_NOTIFY = 0x00000002, /* Vlan is about to be deleted */
  VLAN_DELETE_NOTIFY = 0x00000004,      /* Delete a VLAN */
  VLAN_ADD_PORT_NOTIFY = 0x00000008,    /* Add a port to a VLAN */
  VLAN_DELETE_PORT_NOTIFY = 0x00000010,  /* Delete a port from a VLAN */
  VLAN_START_TAGGING_PORT_NOTIFY = 0x00000020,  /* Start tagging on a port */
  VLAN_STOP_TAGGING_PORT_NOTIFY = 0x00000040,   /* Stop tagging on a port */
  VLAN_INITIALIZED_NOTIFY = 0x00000080,
  VLAN_RESTORE_NOTIFY = 0x00000100,
  VLAN_PVID_CHANGE_NOTIFY = 0x00000200,        /* PVID change on a port*/
  VLAN_LAST_NOTIFY = 0x00000400               /*Any time we add an event adjust this be the last in the series */
} vlanNotifyEvent_t;


/* VLAN Notification Structure */
typedef struct dot1qNotifyData_s
{
    L7_uint32 numVlans; /* If num Vlan is 1 use vlanId member of the union, else use vlanMask of the union*/
    union
    {
        L7_uint32 vlanId;
        L7_VLAN_MASK_t vlanMask;
    }data;

}dot1qNotifyData_t;

/* PTin added: ptin_evc
   Check if dot1q message queue is empty */
extern L7_BOOL dot1qQueueIsEmpty(void);

/*********************************************************************
* @purpose  Create a new VLAN
*
* @param    vid         VLAN ID
*           requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    added 10/10/00 djohnson
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCreate(L7_uint32 vid, L7_uint32 requestor);

/*********************************************************************
* @purpose  Delete a VLAN
*
* @param    vid         VLAN ID
*           requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanDelete(L7_uint32 vid, L7_uint32 requestor);

/*********************************************************************
* @purpose  Set a vlan name
*
* @param    vid         vlan ID
* @param    *name       pointer to string defining vlan name
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    Length of name is L7_MAX_VLAN_NAME characters 
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanNameSet(L7_uint32 vid, L7_uchar8 *name);

/*********************************************************************
* @purpose  Get a vlan name
*
* @param    vid         vlan ID
* param    *name       pointer to string in which to store vlan name
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    Length of name is L7_MAX_VLAN_NAME characters 
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanNameGet(L7_uint32 vid, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Get a vlan ID from a vlan name
*
* @param    *vlanName   name of a VLAN
* @param    *vlanId     VLAN ID
*
* @returns  L7_SUCCESS, if name found
* @returns  L7_FAILURE, if no VLAN matches the name
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIdFromNameGet(const L7_char8 *vlanName, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Get a VLAN's Filtering Database ID
*
* @param    vid         vlan ID
* @param    *fdbID      pointer to Filtering Database ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanFdbIDGet(L7_uint32 vid, L7_uint32 *fdbID);

/*********************************************************************
* @purpose  Set VLAN port membership
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be added
*                       or L7_ALL_INTERFACES change all interfaces
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    requestor DOT1Q_ADMIN is for all non GVRP requests
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanMemberSet(L7_uint32 vid, L7_uint32 intf, L7_uint32 mode, L7_uint32 requestor, DOT1Q_SWPORT_MODE_t swport_mode);

/*********************************************************************
* @purpose  Get VLAN port membership mode
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface in question
* @param    *mode       pointer to mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanMemberGet(L7_uint32 vid, L7_uint32 intf, L7_uint32 *mode);


/*********************************************************************
* @purpose  Set the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or 
*                             L7_DOT1Q_TAGGED})
* @param    requestor   @b{(input)} entity requesting the change
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanTaggedMemberSet(L7_uint32 vid, L7_uint32 intf, L7_uint32 tagMode,
                                   vlanRequestor_t requestor);

/*********************************************************************
* @purpose  Get the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be configured
* @param    *tagMode     Pointer to tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or 
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanTaggedMemberGet(L7_uint32 vid, L7_uint32 intf, L7_uint32 *tagMode);
L7_RC_t   dot1qVlanTaggedMemberGetNoLock(L7_uint32 vid, L7_uint32 intf, L7_uint32 *tagMode);

/*********************************************************************
* @purpose  Set the group filter configuration for a member port
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    filterMode  group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanGroupFilterSet(L7_uint32 vid, L7_uint32 intf, L7_uint32 filterType);

/*********************************************************************
* @purpose  Get the group filter configuration for a member port
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be configured
* @param    *filterMode  pointer to group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanGroupFilterGet(L7_uint32 vid, L7_uint32 intf, L7_uint32 *filterType);

/*********************************************************************
* @purpose  Convert a dynamically learned VLAN into an 
*           administratively configured one.
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qDynamicToStaticVlanConvert(L7_uint32 vid);

/*********************************************************************
* @purpose  Check to see if a VLAN exists based on a   
*           VLAN ID.                         
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCheckValid(L7_uint32 vid);

/*********************************************************************
* @purpose  Synchronize with the dot1q Queue.
*
* @param    @b{(input)} None 
*
* @returns  L7_SUCCESS  if the vlan Id is found and has been created 
* @returns  L7_FAILURE  if the vlan Id is not found
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1qApiQueueSynchronize(L7_uint32 vid);


/*********************************************************************
* @purpose  Check to see if a VLAN exists after dot1q Queue synchronization 
*           based on a VLAN ID.                         
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanQueueSyncCheckValid(L7_uint32 vid);

/*********************************************************************
* @purpose  Determine whether an interface is a member of a given VLAN.
*
* @param    vid         vlan ID
* @param    intIfNum    interface in question
*
* @returns  L7_TRUE if the interface's participation in the VLAN is 
*           L7_DOT1Q_FIXED.
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsVlanMember(L7_uint32 vid, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the operational VLAN port membership
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be add
* @param    *mode       pointer to mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanMemberGet(L7_uint32 vid, L7_uint32 intf, L7_uint32 *mode);
L7_RC_t   dot1qOperVlanMemberGetNoLock(L7_uint32 vid, L7_uint32 intf, L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the operational tagging mode for a port in a VLAN
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be configured
* @param    *tagMode     Pointer to tagging mode
*                       (@b{  DOT1Q_UNTAGGED or 
*                             DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanTaggedMemberGet(L7_uint32 vid, L7_uint32 intf, L7_uint32 *tagMode);

/*********************************************************************
* @purpose  Get the operational group filtering mode  for a port in a VLAN
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be configured
* @param    *filterMode  pointer to group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanGroupFilterGet(L7_uint32 vid, L7_uint32 intf, L7_uint32 *filterType);

/*********************************************************************
* @purpose  Get the VLAN type 
*
* @param    vid         vlan ID
* @param    *type       pointer to mode of participation
*                       L7_DOT1Q_DEFAULT,
*                       L7_DOT1Q_ADMIN
*                       L7_DOT1Q_DYNAMIC
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    This information is LVL7 status only. 
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanTypeGet(L7_uint32 vid,  L7_uint32 *type);


/*********************************************************************
* @purpose  Get the VLAN creation time
*
* @param    vid         vlan ID
* @param    *time       pointer to L7_time_t structure
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qOperVlanCreationTimeGet(L7_uint32 vid, L7_timespec *time);

/*********************************************************************
* @purpose  Get next vlan
*
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid 
*                       (@b{  1-4094,   or         
                             DOT1Q_ADMIN_VLAN})
*
* @returns  L7_FAILURE  no more
* @returns  L7_SUCCESS  next found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qNextVlanGet(L7_uint32 vid, L7_uint32 *nextvid);

/*********************************************************************
* @purpose  Get the next static vlan in the tree
*
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to storage of next vid 
*
* @returns  L7_FAILURE  reached the last static vlan or bad first vid
* @returns  L7_SUCCESS  next found
*
* @notes    If invoked with vid=0, will start at head of list
*
* @end
*********************************************************************/
L7_RC_t dot1qNextStaticVlanGet(L7_uint32 vid, L7_uint32 *nextvid);

/*********************************************************************
* @purpose  Get the next dynamic vlan in the tree
*
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid
*
* @returns  L7_FAILURE  reached the last static vlan or bad first vid
* @returns  L7_SUCCESS  next found
*
* @notes    If invoked with vid=0, will start at head of list 
*
* @end
*********************************************************************/
L7_RC_t dot1qNextDynamicVlanGet(L7_uint32 vid, L7_uint32 *nextvid);

/*********************************************************************
* @purpose  Reset Vlan configuration to default parameters
*
* @param    void 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Sets active configuration without changing config parms
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCfgClear();

/*********************************************************************
* @purpose  Set the VID configuration for a port
*
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    vid         vlan ID
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist(configuration is applied)
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVIDSet(L7_uint32 intf, L7_uint32 vid, L7_uint32 vid_type,L7_uint32 requestor);

/*********************************************************************
* @purpose  Get the PVID configuration for a port
*
* @param    intf        physical or logical interface to be configured
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
* @param    pvid        the port VLAN ID associated with this 
*                       interface and type
*
* @returns  L7_SUCCESS, if success
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsVIDGet(L7_uint32 intf,  L7_uint32 vid_type, L7_uint32 *pvid);

/*********************************************************************
* @purpose  Get the current PVID configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
* @param    pvid        the port VLAN ID associated with this 
*                       interface and type
*
* @returns  L7_SUCCESS, if success
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsCurrentVIDGet(L7_uint32 intIfNum,  L7_uint32 vid_type, L7_uint32 *pvid);

/*********************************************************************
* @purpose  Set the Acceptable Frame Type configuration for a port
*
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    type        L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsAcceptFrameTypeSet(L7_uint32 intf, L7_uint32 type);

/*********************************************************************
* @purpose  Set the switchport mode configuration for a port
*
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
* @param    mode        L7_SWPORT_MODE_GENERAL,L7_SWPORT_MODE_TRUNK,L7_SWPORT_MODE_ACCESS 
* @param    vlanId      access vlan id if mode is L7_SWPORT_MODE_ACCESS 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qSwitchPortModeSet(NIM_INTF_MASK_t *mask, L7_uint32 mode,
                                 vlanRequestor_t requestor);

/*********************************************************************
* @purpose  Get the switchport mode configuration for a port (get access vlan id in case mode is access)
*
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    mode        L7_SWPORT_MODE_GENERAL,L7_SWPORT_MODE_TRUNK,L7_SWPORT_MODE_ACCESS 
* @param    vlanId      access vlan id if mode is L7_SWPORT_MODE_ACCESS 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qSwitchPortModeGet(L7_uint32 intf, L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the Acceptable Frame Type configuration for a port
*
* @param    intf        physical or logical interface to be configured
* @param    type        L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsAcceptFrameTypeGet(L7_uint32 intf, L7_uint32 *type);

/*********************************************************************
* @purpose  Set the Ingress Filtering configuration for a port
*
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    status      L7_ENABLE  or L7_DISABLE
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsIngressFilterSet(L7_uint32 intf, L7_uint32 status,L7_uint32 requestor);

/*********************************************************************
* @purpose  Get the Ingress Filtering configuration for a port
*
* @param    intf        physical or logical interface to be configured
* @param    mode        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsIngressFilterGet(L7_uint32 intf, L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the Current Ingress Filtering configuration for a port
*
* @param    intIfNum    physical or logical interface to be configured
* @param    mode        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsCurrentIngressFilterGet(L7_uint32 intIfNum, L7_uint32 *mode);


                               
/*********************************************************************
* @purpose  Get device capabilities
*
* @param    none
*
* @returns  deviceCapabilities   bitmask mapping of functionality
*
* @notes
*        dot1dExtendedFilteringServices(0),
*                              -- can perform filtering of
*                              -- individual multicast addresses
*                              -- controlled by GMRP.
*        dot1dTrafficClasses(1),
*                              -- can map user priority to
*                              -- multiple traffic classes.
*        dot1qStaticEntryIndividualPort(2),
*                              -- dot1qStaticUnicastReceivePort &
*                              -- dot1qStaticMulticastReceivePort
*                              -- can represent non-zero entries.
*        dot1qIVLCapable(3),   -- Independent VLAN Learning.
*        dot1qSVLCapable(4),   -- Shared VLAN Learning.
*        dot1qHybridCapable(5),
*                              -- both IVL & SVL simultaneously.
*        dot1qConfigurablePvidTagging(6),
*                              -- whether the implementation
*                              -- supports the ability to
*                              -- override the default PVID
*                              -- setting and its egress status
*                              -- (VLAN-Tagged or Untagged) on
*                              -- each port.
*        dot1dLocalVlanCapable(7)
*                              -- can support multiple local
*                              -- bridges, outside of the scope
*                              -- of 802.1Q defined VLANs.
*    
*
* @end
*********************************************************************/
L7_uchar8 dot1dDeviceCapabilitiesGet();

/*********************************************************************
* @purpose  Get traffic class capability
*
* @param    *status    Ptr to status value output location (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If successful, the *status parameter indicates L7_TRUE if
*           more than one priority level is enabled, or L7_FALSE if a 
*           single priority level is enabled.
*
* @end
*********************************************************************/
L7_RC_t dot1dTrafficClassesEnabledGet(L7_BOOL *status);

/*********************************************************************
* @purpose  Set the traffic class capability
*
* @param    status    status value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A status of L7_TRUE indicates more than one priority level
*           is enabled, while L7_FALSE indicates a single priority level
*           is enabled.
*
* @end
*********************************************************************/
L7_RC_t dot1dTrafficClassesEnabledSet(L7_BOOL status);

/*********************************************************************
* @purpose  Get dot1d port capabilities 
*
* @param    intIfNum    internal interface number
*
* @returns  portCapabilities   bitmask mapping of functionality
*
* @notes     
* SYNTAX      BITS {
*        dot1qDot1qTagging(0), -- supports 802.1Q VLAN tagging of
*                              -- frames and GVRP.
*        dot1qConfigurableAcceptableFrameTypes(1),
*                              -- allows modified values of
*                              -- dot1qPortAcceptableFrameTypes.
*        dot1qIngressFiltering(2)
*                              -- supports the discarding of any
*                              -- frame received on a Port whose
*                              -- VLAN classification does not
*                              -- include that Port in its Member
*                              -- set.
*    }
*
*
* @end
*********************************************************************/
L7_uchar8 dot1dPortCapabilitiesGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get dot1d port default user priority
*
* @param    intIfNum    internal interface number
*
* @returns  priority    default user priority
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_uint32 dot1dPortDefaultUserPriorityGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set dot1d port default user priority
*
* @param    intIfNum   internal interface number
* @param   priority    default user priority, in range of 0-7
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultUserPrioritySet(L7_uint32 intIfNum, L7_uint32 priority);

/*********************************************************************
* @purpose  Set dot1d port default user priority globally for all interfaces
*
* @param    priority    default user priority, in range of 0-7
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultUserPriorityGlobalSet(L7_uint32 priority);

/*********************************************************************
* @purpose  Get dot1d port default user priority
*
* @param    intIfNum   internal interface number
*
* @returns  Number of traffic classes supported, in range of 1-8
*           optionally readonly
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_uint32 dot1dPortNumTrafficClassesGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set dot1d number of traffic classes
*
* @param    intIfNum            internal interface number
* @param    numTrafficClasses   number of traffic classes supported, in range of 1-8
*
* @returns  L7_SUCCESS
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassesSet(L7_uint32 intIfNum, L7_uint32 numTrafficClasses);

/*********************************************************************
* @purpose  Set dot1d number of traffic classes globally for all interfaces
*
* @param    numTrafficClasses   number of traffic classes supported, in range of 1-8
*
* @returns  L7_SUCCESS
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassesGlobalSet(L7_uint32 numTrafficClasses);

/*********************************************************************
* @purpose  Get dot1d traffic class priority
*
* @param    intIfNum   internal interface number
* @param    traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
*           *priority           priority associated with traffic class
*
* @returns  L7_SUCCESS          priority was retrieved
* @returns  L7_FAILURE          traffic class not within valid range
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassPriorityGet(L7_uint32 intIfNum, L7_uint32 trafficClass, L7_uint32 *priority);

/*********************************************************************
* @purpose  Get dot1d traffic class 
*
* @param    intIfNum   internal interface number
* @param    priority   traffic class priority
* @param    *traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *trafficClass);

/*********************************************************************
* @purpose  Set dot1d traffic class 
*
* @param    intIfNum   internal interface number
* @param    priority   traffic class priority
* @param    traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassSet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 trafficClass);

/*********************************************************************
* @purpose  Set dot1d traffic class globally for all interfaces 
*
* @param    priority      traffic class priority
* @param    traffic class In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassGlobalSet(L7_uint32 priority, L7_uint32 trafficClass);

/*********************************************************************
* @purpose  Get factory default dot1d traffic class 
*
* @param    intIfNum   internal interface number
* @param    priority   traffic class priority
* @param    *traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultTrafficClassGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *trafficClass);

/*********************************************************************
* @purpose  Get dot1q Vlan Version Number
*
* @param    none
*
* @returns  IEEE 802.1Q version number supported by this device
*
* @notes     
*
* @end
*********************************************************************/
L7_uint32 dot1qVlanVersionGet();

/*********************************************************************
* @purpose  Get dot1q Max VLAN ID        
*
* @param    none
*
* @returns  Maximum VLAN ID supported by this device
*
* @notes     
*
* @end
*********************************************************************/
L7_uint32 dot1qMaxVlanIDGet();

/*********************************************************************
* @purpose  Get dot1q Max Supported VLANs
*
* @param    none
*
* @returns  Maximum number of VLANs supported by this device
*
* @notes     
*
* @end
*********************************************************************/
L7_uint32 dot1qMaxSupportedVlanGet();

/*********************************************************************
* @purpose  Get dot1q Number Vlans
*
* @param    none
*
* @returns  number of VLANs currently configured on this device
*
* @notes     
*
* @end
*********************************************************************/
L7_uint32 dot1qCurrentVlanCountGet();

/*********************************************************************
* @purpose  Update the current dot1q Number Vlans
*
* @param    increment  Boolean parameter to add or delete the count
*
* @returns  none
*
* @notes     
*
* @end
*********************************************************************/
void dot1qCurrentVlanCountUpdate(L7_BOOL increment);

/*********************************************************************
* @purpose  Get dot1q Most Vlans Used
*
* @param    none
*
* @returns  number of most VLANs ever in use on this device
*
* @notes    none  
*
* @end
*********************************************************************/
L7_uint32 dot1qMostVlanCountGet();

/*********************************************************************
* @purpose  Get dot1q number Vlans deleted
*
* @param    *NumDeletes   number of times a VLAN has been deleted  
*
* @returns  L7_SUCCESS
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanNumDeletesGet(L7_uint32 *NumDeletes);

/*********************************************************************
* @purpose  Get set of ports which are members of this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Like dot1qVlanCurrentEgressPortsGet() below, but doesn't 
*           reverse the bit order in each byte of mask. 
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanEgressPortsGet(L7_uint32 vlanID, NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Get set of ports which are members of this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCurrentEgressPortsGet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask, L7_uint32 *size);

/*********************************************************************
* @purpose  Get set of ports which are transmitting untagged frames 
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure

*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanCurrentUntaggedPortsGet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask, L7_uint32 *size);

L7_RC_t   dot1qVlanCreationTimeGet(L7_uint32 vlanID, L7_timespec *time);

/*********************************************************************
* @purpose  Get the VLAN static egress ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which are members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticEgressPortsGet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask, L7_uint32 *size);

/*********************************************************************
* @purpose  Set the VLAN static egress ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which are members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticEgressPortsSet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask);

/*********************************************************************
* @purpose  Get the VLAN Forbidden Egress Ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which cannot be members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanForbiddenEgressPortsGet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask, L7_uint32 *size);

/*********************************************************************
* @purpose  Set the VLAN Forbidden Egress Ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which cannot be members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanForbiddenEgressPortsSet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask);

/*********************************************************************
* @purpose  Get set of ports which are transmitting untagged frames 
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticUntaggedPortsGet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask, L7_uint32 *size);

/*********************************************************************
* @purpose  Set the set of ports which are transmitting untagged frames 
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStaticUntaggedPortsSet(L7_uint32 vlanID, NIM_INTF_MASK_t * mask);


/*********************************************************************
* @purpose  Determine if a new VLAN can be supported
*
* @param    none
*
* @returns  0, if another VLAN cannot be supported
* @returns  >=4096 if another VLAN can be supported
*
* @notes     
*
* @end
*********************************************************************/
L7_uint32 dot1qNextFreeLocalVlanIndexGet();


/*********************************************************************
* @purpose  Get the VLAN status
*
* @param    vid         vlan ID
* @param    *type       pointer to mode of participation
*                       (@b{  Default, Admin,GVRPRegistered})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanStatusGet(L7_uint32 vid,  L7_uint32 *type);

/*********************************************************************
* @purpose  Get a list of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    dtlEvent    event
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
L7_RC_t dot1qVIDListGet(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, L7_uint32 *data, 
                        L7_uint32 *numOfElements);

/*********************************************************************
* @purpose  Get a list of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    dtlEvent    event
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
L7_RC_t dot1qVIDListCfgGet(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, L7_uint32 *data, 
                           L7_uint32 *numOfElements);

/*********************************************************************
* @purpose  Get a list of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    dtlEvent    event
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
L7_RC_t dot1qVIDListMaskGet(L7_uint32 intfNum, L7_VLAN_MASK_t  *data);

/*********************************************************************
* @purpose  Get number of VLANs an interface is a member of
*
* @param    intfNum     interface
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    
* @end
*********************************************************************/
L7_RC_t dot1qVIDnumGet(L7_uint32 intfNum, L7_uint32 *numOfElements);


/*********************************************************************
* @purpose  Get number of interface included in a VLANs
*
* @param    intfNum     interface
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    
* @end
*********************************************************************/
L7_RC_t dot1qIntfInVIDnumGet(L7_uint32 vlanId, NIM_INTF_MASK_t *mMask,
                             L7_uint32 *numOfElements);


/*********************************************************************
* @purpose  Determine if an interface has been aquired by a LAG
*
* @param    intf        interface
* @param    *status     L7_TRUE, if currently aquired
*                       L7_FALSE, if not aquired
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
*
* @notes    L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfAcquiredGet(L7_uint32 intf, L7_uint32 *status);

/*********************************************************************
* @purpose  Gets the component ID which has acquired the interface
*
* @param    intIfNum        interface
* @param    *compId         the ID of the acquiring component
*
* @returns  L7_SUCCESS     if success
* @returns  L7_NOT_EXISTS  if VLAN does not exist
*
* @notes    Storage must be allocated by the caller
*           The component ID returned is either valid or L7_FIRST_COMPONENT_ID
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfAcquiredCompGet(L7_uint32 intIfNum, L7_COMPONENT_IDS_t *compId);

/*********************************************************************
* @purpose  Register a routine to be called when a VLAN is created,
*           deleted, or modified.
*
* @param    *notify      Notification routine with the following parm
*                        @param    vlanId              VLAN ID
*                        @param    intIfNum            internal interface number
*                        @param    event               VLAN_ADD_NOTIFY,
*                                                      VLAN_DELETE_NOTIFY,
*                                                      VLAN_ADD_PORT_NOTIFY or
*                                                      VLAN_DELETE_PORT_NOTIFY
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    eventMask    combined mask of the events this component is interested in receiving.
*                                       
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise 
*
* @notes    Comnponent will not be notified of events NOT in the mask.
*       
* @end
*********************************************************************/
L7_RC_t vlanRegisterForChange(L7_RC_t (*notify)(dot1qNotifyData_t *vlanData,
                                                L7_uint32 intIfNum,
                                                L7_uint32 event),
                           L7_COMPONENT_IDS_t registrarID, L7_uint32 eventMask);



/*********************************************************************
* @purpose  Register a routine to be called when a VLAN is created,
*           deleted, or modified.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*                                       
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise 
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t vlanDeregisterForChange( L7_COMPONENT_IDS_t registrarID);



/*********************************************************************
* @purpose  Get the first member port of the vlan
*
* @param    vlanId      vlan id of the vlan
* @param    *intf       interface
*                       
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE  If there are no members in the vlan
*
* @notes    This function returns the first member of the vlan. It doesn't
* @notes    differentiate between dynamic or static vlan.
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberFirstGet(L7_uint32 vlanId, L7_uint32 *intf);

/*********************************************************************
* @purpose  Get the next member port of the vlan after the given port
*
* @param    vlanId      vlan id of the vlan
* @param    *intf       current interface
*                       
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE  If there are no more members in the vlan
*
* @notes    This function returns the next member of the vlan. It doesn't
* @notes    differentiate between dynamic or static vlan.
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberNextGet(L7_uint32 vlanId, L7_uint32 *intf);

/*********************************************************************
* @purpose  Create an interface for the vlan
*
* @param    vlanId         vlan ID
* @param    *intIfNum   pointer to internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfCreate(L7_uint32 vlanId, L7_uint32 intfId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Delete an intf for the vlan
*
* @param    vlanId         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfDelete(L7_uint32 vlanId);

/*********************************************************************
* @purpose  Set the "always up" attribute on a VLAN interface
*
* @param    vlanId        vlan ID
* @param    alwaysUp      L7_ENABLE or L7_DISABLE
* @param    componentId   component ID
*
* @returns  L7_SUCCESS  If vlanId is valid
* @returns  L7_FAILURE  Otherwise
*
* @notes    This has the effect of setting the "always up"
*           attribute of the interface and will cause a VLAN routing
*           interface to remain operationally up even when no
*           ports are participating in the VLAN.
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfAlwaysUpSet(L7_uint32 vlanId, L7_uint32 alwaysUp,
                                L7_COMPONENT_IDS_t componentId);

/*********************************************************************
* @purpose  Get the "always up" attribute of a VLAN interface
*
* @param    vlanId      vlan ID
* @param    pAlwaysUp   returns L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  If vlanId is valid
* @returns  L7_FAILURE  Otherwise
*
* @notes    none
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfAlwaysUpGet(L7_uint32 vlanId, L7_uint32 *pAlwaysUp);

/*********************************************************************
* @purpose  Get the instance number associated with a VLAN interface
*
* @param    vlanId          vlan id
* @param    pIndex          Index of this vlan in dot1qVlanIntf
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Returns a relative instance number for the VLAN interface,
*          from 1 to L7_MAX_NUM_VLAN_INTF
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfPortInstanceNumGet(L7_uint32 vlanId, L7_uint32 *pIndex);

/*********************************************************************
* @purpose  Get the interface number corresponding to this vlan
*
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of this vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
* @notes    
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfVlanIdToIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the interface ID for a given VLAN
*
* @param    vlanid      VLAN ID
* @param    intfId      Interface ID (not internal interface number)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The interface ID is an internally assigned integer in the
*           range [1, L7_MAX_NUM_VLAN_INTF]. The user may optionally 
*           specify the interface ID. The interface ID influences the
*           u/s/p and internal interface number assigned to the VLAN
*           interface. It is listed in the text configuration in order
*           to retain the internal interface number and u/s/p across
*           reboots.  
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIdGet(L7_uint32 vlanId, L7_uint32 *intfId);


/*********************************************************************
* @purpose  Get the next vlan after this vlan which is configured to be an interface
*
* @param    vlanid          id of the vlan
* @param    nextVlanId      pointer to id of next vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
* @notes    
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfNextVlanIntfGetNext(L7_uint32 vlanId, L7_uint32 *nextVlanId);

/*********************************************************************
* @purpose  Get the vlanid corresponding to this interface number
*
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of this vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
* @notes    
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qVlanIntfIntIfNumToVlanId(L7_uint32 intIfNum, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Set the direction from the base VLAN ID when looking for an
*           unused VLAN ID for internal VLAN assignment 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanPolicySet(DOT1Q_INTERNAL_VLAN_POLICY policy);

/*********************************************************************
* @purpose  Get the direction from the base VLAN ID when 
*           assigning  internal vlans 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
DOT1Q_INTERNAL_VLAN_POLICY dot1qInternalVlanPolicyGet(void);

/*********************************************************************
* @purpose  Set the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    baseVlanId  (input)   VLAN ID 
*
* @returns  L7_SUCCESS
*           L7_FAILURE if VLAN ID is out of range
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qBaseInternalVlanIdSet(L7_uint32 baseVlanId);

/*********************************************************************
* @purpose  Get the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    void
*
* @returns  VLAN ID
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_uint32 dot1qBaseInternalVlanIdGet(void);

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use
*
* @param    vlanId          @b{(output)} pointer to first internal vlan 
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally                         
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanFindFirst(L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use
*
* @param    vid      @b{(input)} previous VLAN ID
* @param    nextvid  @b{(output)} next vlanId 
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally                         
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanFindNext(L7_uint32 vid, L7_uint32 *nextvid);

/*********************************************************************
* @purpose  Find the usage description for the given internal vlan
*
* @param    vid          @b{(input)} vlan ID
* @param    *descr       @b{(output)} pointer to usage description 
*
* @returns  L7_SUCCESS if VLAN ID is found and is internal
* @returns  L7_FAILURE if the VLAN ID is not listed as internal                        
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanUsageDescrGet(L7_uint32 vid, L7_char8 *descr);

/*********************************************************************
* @purpose  Obtain an internal VLAN assignment
*
* @param    requestor   @b{(input)} one of vlanRequestor_t
* @param    *descr      @b{(input)} pointer to usage description
* @param    *vid        @b{(output)} pointer to assigned VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Currently requested only by DOT1Q_IP_INTERNAL_VLAN
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanRequest(L7_uint32 requestor,
                                 L7_char8 *descr, 
                                 L7_uint32 *vid);

/*********************************************************************
* @purpose  Release an internal VLAN 
*
* @param    vid        @b{(input)} assigned VLAN ID
* @param    requestor  @b{(input)}one of vlanRequestor_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Currently requested only by DOT1Q_IP_INTERNAL_VLAN
*
* @end
*********************************************************************/
L7_RC_t dot1qInternalVlanRelease( L7_uint32 vid, L7_uint32 requestor);

/*********************************************************************
* @purpose  Resets VLAN counters
*
* @param    void

* @returns  L7_SUCCESS, if success
*
* @notes    Invoked during clear stats switch
*
* @end
*********************************************************************/
void dot1qVlanResetStats(void);

/*********************************************************************
* @purpose  See if an interface type is of the right type for VLANs
*
* @param    intfType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsValidType(L7_uint32 intfType);

/*********************************************************************
* @purpose  See if an interface is of the right type for VLANs
*
* @param    intifNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  See if an interface mask is completely valid for dot1q
*
* @param    *intIfMask      pointer to an L7_INTF_MASK_t structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsValidMask(L7_INTF_MASK_t *portMask);

/*********************************************************************
* @purpose  Get VLAN port membership and type
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface in question
* @param    *mode       pointer to mode of participation:
*                       L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN or
*                       L7_DOT1Q_NORMAL_REGISTRATION
* @param    *type       pointer to type of participation:
*                       L7_DOT1Q_DEFAULT, L7_DOT1Q_ADMIN   or
*                       L7_DOT1Q_DYNAMIC                 
*                       
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    This function is a combination of dot1qVlanMemberGet() and 
*           dot1qOperVlanTypeGet().  In cases when we need to call the
*           mentioned two functions one after another, 
*           dot1qVlanMemberAndTypeGet() should be called instead
*           (this will save some time on calling vlanDataSearch() and
*           grabbing/releasing semaphore).
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberAndTypeGet(L7_uint32 vid,   L7_uint32 intf, 
                                  L7_uint32 *mode, L7_uint32 *type);

/******************************************************************
* @purpose  Check to see if a VLAN exists based on a  
*           VLAN ID and it is static.                         
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qStaticVlanCheckValid(L7_uint32 vid);


/*********************************************************************
* @purpose  Set VLAN port membership for a range of interfaces
*
* @param    vid         vlan ID
* @param    mask        interface Mask
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the ports,
*           whose membership is changing, in other words only the delta
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanMemberMaskSet(L7_uint32 vid, NIM_INTF_MASK_t * mask, L7_uint32 mode, DOT1Q_SWPORT_MODE_t swport_mode);

/*********************************************************************
* @purpose  Set the tagging configuration for a range of interfaces
*
* @param    vid         vlan ID
* @param    mask        interface mask
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or 
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the ports,
*           whose membership is changing, in other words only the delta
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanTaggedMemberMaskSet(L7_uint32 vid, NIM_INTF_MASK_t * mask, L7_uint32 tagMode);
/*********************************************************************
* @purpose  For a given interface set vlan membership for a range of vlans
*
* @param    intIfNum    Internal Interface Number
* @param    vidMask     Vlan ID Mask
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the vlans,
*           whose membership is changing, in other words only the delta
*           This routine will set the membership for all valid vlans and
*           not abort in the case of a non existant or dynamic vlan
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVlanMembershipMaskSet(L7_uint32 intIfNum, L7_VLAN_MASK_t *mask, L7_uint32 mode, DOT1Q_SWPORT_MODE_t swport_mode);
/*********************************************************************
* @purpose  Set the tagging configuration for a interface on a range of vlans
*
* @param    intIfNum    Internal Interface Number
* @param    vidMask     Vlan ID Mask
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or 
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the vlans,
*           whose tagging is changing, in other words only the delta
*           This routine will set the tagging for all valid vlans and
*           not abort in the case of a non existant or dynamic vlan
*
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVlanTagMaskSet(L7_uint32 intIfNum, L7_VLAN_MASK_t *mask, L7_uint32 tagMode);

/*********************************************************************
* @purpose  Revert to configured vlan settings 
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberRevert(L7_uint32 vid, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Revert to Dynamic vlan settings
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qDynamicVlanMemberRevert(L7_uint32 vid, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the vlan type by requestor
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface in question
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTypeGetByRequestor(L7_uint32 requestor, L7_uint32 *type);

/*********************************************************************
* @purpose  Set access vlanId for a port 
*
* @param    intIfNum        physical or logical interface in question
* @param    vlanId         vlan ID
* @param    requestor    vlanRequestor_t type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortAccessVlanSet(L7_uint32 intIfNum, L7_uint32 vlanId,
                                     L7_uint32 requestor);

/*********************************************************************
* @purpose  Get access vlanId for a port 
*
* @param    intIfNum        physical or logical interface in question
* @param    vlanId         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortAccessVlanGet(L7_uint32 intIfNum, L7_uint32* vlanId);

/*********************************************************************
* @purpose  Is the vlan access vlan for any port?
*
* @param    vid         vlan ID
*
* @returns  L7_TRUE, if yes L7_FALSE otherwise
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL dot1qIsAccessVlanForAnyPort(L7_uint32 vlanId );

/************************************Changes for dot1q_MASK **********************************************/

/* NOTE: Start changes which deviate from most mask macros */



/* FHMASKBIT finds the index of the most-significant bit turned-on in
   mask j and returns that index in k.

   if no bit is set, k returns an invalid value. That is, k is greater
   than the maximum number of bits in the mask */

#define L7_VLAN_FHMASKBIT(j, k) {    \
    L7_VLAN_MASK_t  *x;              \
    x = &j;                             \
    dot1qVlanMaskFHMaskBit(x,&k);       \
}



/* FLMASKBIT finds the index of the least-significant bit turned-on in
   mask j and returns that index in k.

   if no bit is set, k returns an invalid value. That is, k is greater
   than the maximum number of bits in the mask */

#define L7_VLAN_FLMASKBIT(j, k) {    \
    L7_VLAN_MASK_t  *x;              \
    x = &j;                             \
    dot1qVlanMaskFLMaskBit(x,&k);       \
}


/* Shift bits in mask j left by k bits  */
#define L7_VLAN_SHIFTLEFTMASKBITS(j, k) {    \
   L7_VLAN_MASK_t  *x;                       \
    x = &j;                                     \
    dot1qVlanMaskShiftLeftBits(x,k);            \
}


/* Fill bits left of k with ones, bit k and below with zeroes */
#define L7_VLAN_FILLLEFTMASKBITS(j, k) {     \
   L7_VLAN_MASK_t  *x;                       \
    x = &j;                                     \
    dot1qVlanMaskFillLeftMostBits(x,k);         \
}

/* Function prototypes */

void dot1qVlanMaskFillLeftMostBits(L7_VLAN_MASK_t *j, L7_int32 k);
void dot1qVlanMaskShiftLeftBits(L7_VLAN_MASK_t *j, L7_int32 k);
void dot1qVlanMaskFLMaskBit(L7_VLAN_MASK_t *j, L7_uint32 *k);
void dot1qVlanMaskFHMaskBit(L7_VLAN_MASK_t *j, L7_uint32 *k);
L7_RC_t   dot1qVlanCreateMask(L7_VLAN_MASK_t *vidMask);
L7_RC_t   dot1qVlanDeleteMask(L7_VLAN_MASK_t *vidMask);
/* NOTE:  End changes which deviate from most mask maccros */

/************************************Changes for dot1q_MASK **********************************************/

#endif
