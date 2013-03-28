
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   dtl_l3_mcast_api.h
*
* @purpose    This files contains the declarations of the necessary prototypes,
*             for the layer3 multicast specific Dtl calls.
*
* @component  Device Transformation Layer
*
* @comments   none
*
* @create     5/31/2002
*
* @author
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef INCLUDE_DTL_L3_MCAST_API_H
#define INCLUDE_DTL_L3_MCAST_API_H

#include <l3_comm_structs.h>
#include "comm_mask.h"
#include "dapi.h"

/*
********************************************************************
*                         DTL LAYER3 STRUCTURES
********************************************************************
*/

/*********************************************************************
*
* @enums L7_MCAST_RPF_CHECK_FAIL_ACTION_t
*
* @purpose Enumerates actions taken on L3 multicast Reverse Path
* @purpose Forwarding check failures.
*
* @notes none
*
*********************************************************************/
typedef enum
{
  L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP = 0,  /* if received mcast frame fails RPF check, drop it (e.g. DVMRP) */
  L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU /* if received mcast frame fails RPF check, send to CPU (e.g. PIM) */

} L7_MCAST_RPF_CHECK_FAIL_ACTION_t;
/*********************************************************************
*
* @enums L7_MCAST_RPF_CHECK_METHOD_t
*
* @purpose Enumerates the methods used for Reverse Path Forwarding
* @purpose checks.
*
* @notes none
*
*********************************************************************/
typedef enum
{
   L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH = 0,  /* compare packet incoming interface with IIF in mcast route entry*/
   L7_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE   /* look up source IP of received frame in the unicast routing table for IIF */

} L7_MCAST_RPF_CHECK_METHOD_t;
/*********************************************************************
* @purpose   This structure defines all the parameters for a
* @purpose   Multicast Forwarding Entry
*
* @notes
*
*********************************************************************/
typedef struct L7_multicastForwList_s
{
  L7_INTF_MASK_t      outIntIfMask;           /* The Outgoing Interfaces */
  L7_uint32           inIntIfNum;             /* The Incoming Interface */
  L7_uint32           inPhysicalIntIfNum;     /* Incoming physical interface - differs from the 
                                                 above for VLAN Routing interfaces */
  L7_inet_addr_t     mcastGroupDestAddress;  /* The Multicast Group Destination Address. */
  L7_inet_addr_t  sourceIpAddress;        /* The IP address of the source of the packet  */
  L7_BOOL             reversePathCheck;       /* Flag for enabling/disabling Reverse Path check.*/
  L7_MCAST_RPF_CHECK_METHOD_t typeOfReversePathCheck; /* Flag for configuring type of Reverse Path Check.*/
  L7_MCAST_RPF_CHECK_FAIL_ACTION_t  reversePathCheckFailAction;  /* Action to take if Reverse Path Check fails. */
} L7_multicastForwList_t;

/*********************************************************************
* @purpose   This structure defines all the parameters for a
* @purpose   Multicast Forwarding Table Entry count get.
*
* @notes
*
*********************************************************************/
typedef struct L7_multicastForwEntryCounterQuery_s
{
  L7_uint32           inIntIfNum;             /* The Incoming Interface */
  L7_inet_addr_t     mcastGroupDestAddress;  /* The Multicast Group Destination Address. */
  L7_inet_addr_t  sourceIpAddress;        /* The IP address of the source of the packet  */
  L7_BOOL             reversePathCheck;       /* Flag for enabling/disabling Reverse Path check.*/
  L7_MCAST_RPF_CHECK_METHOD_t typeOfReversePathCheck; /* Flag for configuring type of Reverse Path Check.*/
  L7_MCAST_RPF_CHECK_FAIL_ACTION_t  reversePathCheckFailAction;  /* Action to take if Reverse Path Check fails. */
  L7_uint32           packet_count;           /* number of packets routed via this entry */
  L7_uint32           byte_count;             /* number of bytes routed via this entry */
  L7_uint32           wrong_if_count;         /* number of packets dropped due to RPF fail using this entry */
  L7_BOOL             packet_count_supported; /* flag telling whether driver supports the corresponding counter */
  L7_BOOL             byte_count_supported;     /* " */
  L7_BOOL             wrong_if_count_supported; /* " */
} L7_multicastForwEntryCounterQuery_t;

/*********************************************************************
* @purpose   This structure defines all the parameters for a
* @purpose   Multicast Forwarding Table Entry use get.
*
* @notes
*
*********************************************************************/
typedef struct L7_multicastForwEntryUseQuery_s
{
  L7_uint32           inIntIfNum;             /* The Incoming Interface */
  L7_inet_addr_t     mcastGroupDestAddress;  /* The Multicast Group Destination Address. */
  L7_inet_addr_t  sourceIpAddress;        /* The IP address of the source of the packet  */
  L7_BOOL             reversePathCheck;       /* Flag for enabling/disabling Reverse Path check.*/
  L7_MCAST_RPF_CHECK_METHOD_t typeOfReversePathCheck; /* Flag for configuring type of Reverse Path Check.*/
  L7_MCAST_RPF_CHECK_FAIL_ACTION_t  reversePathCheckFailAction;  /* Action to take if Reverse Path Check fails. */
  L7_BOOL             entry_used;             /* flag indicating the entry has been used to route traffic since last query */
} L7_multicastForwEntryUseQuery_t;

/*
/------------------------------------------------------------------\
*                          MCAST DTL APIs                     *
\------------------------------------------------------------------/
*/

/*********************************************************************
* @purpose  Enables or disables the ability to forward multicast
* @purpose  traffic for this particular router interface.
*
* @param    mode     @b{(input)}
*           L7_ENABLE, to enable forwarding of multicast traffic.
*           L7_DISABLE, to disable forwarding of multicast traffic.
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRouterMulticastForwardModeSet(L7_uint32 mode, L7_uint32 family);

/*********************************************************************
* @purpose  Enables or disables IGMP admin mode
*
* @param    mode     @b{(input)}
*           L7_ENABLE, to enable IGMP admin mode.
*           L7_DISABLE, to disable IGMP admin mode.
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRouterMulticastIgmpModeSet(L7_uint32 mode, L7_uint32 family);
                                                          
/*********************************************************************
* @purpose  Adds or updates a Multicast Forwarding entry in the
* @purpose  Multicast Forwarding List in the network processor.
* @purpose  If the entry is new, the driver will add the forwarding
* @purpose  entry, if it was previously added, the driver updates
* @purpose  the entry (e.g. change to outgoing interfaces).
*
* @param    *pMcastForwEntry  @b{(input)} A pointer to the Multicast Entry
*                                that contains all the necessary information
*                                needed to add the entry to the Multicast
*                                Forwarding list .
*
*
* @returns  L7_SUCCESS  on successful addition
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlMulticastAddrAdd(L7_multicastForwList_t *pMcastForwEntry);

/*********************************************************************
* @purpose  Deletes the  Multicasting Forwarding Entry from
* @purpose  Multicast Forwarding List
*
* @param    *pMcastForwEntry  @b{(input)} A pointer to the Multicast Entry
*                                that contains all the necessary information
*                                needed to identify the Multicast Forwarding
*                                entry  and delete it.
*
*
*
* @returns  L7_SUCCESS  on successful addition
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlMulticastAddrDelete(L7_multicastForwList_t *pMcastForwEntry);


/*********************************************************************
* @purpose  Request counters for the  Multicasting Forwarding Entry from
* @purpose  the driver
*
* @param    *pMcastForwEntryCntQuery  @b{(input)}
*           A pointer to a structure that contains all the
*           necessary information needed to identify the Multicast
*           Forwarding entry.  This structure also has member fields
*           inwhich to return count data.
*
* @returns  L7_SUCCESS  on successful addition, or if driver has
*                       not added entry due to non-support of options
*                       requested
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlMulticastSGCountersGet(L7_multicastForwEntryCounterQuery_t *pMcastForwEntryCntQuery);

/*********************************************************************
* @purpose  Request use status for the  Multicasting Forwarding Entry from
* @purpose  the driver.  This function is to be called ONLY by the application
* @purpose  function that is aging Multicast Forwarding Entries.  The usage
* @purpose  state for the entry is reset as a side effect of calling this
* @purpose  function.  Calling this function can cause use data to be missed
* @purpose  by the aging function and cause a entry to be aged out incorrectly.
*
* @param    *pMcastForwEntryUseQuery  @b{(input)}
*           A pointer to a structure that contains all the
*           necessary information needed to identify the Multicast
*           Forwarding entry.  This structure also has member fields
*           inwhich to return count data.
*
* @returns  L7_SUCCESS  on successful addition, or if driver has
*                       not added entry due to non-support of options
*                       requested
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlMulticastSGUseGet(L7_multicastForwEntryUseQuery_t *pMcastForwEntryUseQuery);

/*********************************************************************
* @purpose  Sets the Multicast TTL scoping value for a router interface.
*
* @param    inIntIfNum    @b{(input)} The internal interface for which the
*                                     TTL scooping has to be applied .
* @param    mcastTtlValue @b{(input)} The TTL value to be applied to this
*                                     router interface
*
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlMulticastTTLScopingSet(L7_uint32 inIntIfNum, L7_uint32 mcastTtlValue);

/*********************************************************************
* @purpose  Updates port information in the IP Multicast Forwarding tables
* @purpose  regarding port vlan membership, tagging (and in the future group membership).
* @purpose  It is expected that this will be invoked in response to notification
* @purpose  events from the dot1q and mfdb components.
*
* @param    vid             @b{(input)} VLAN ID for which port data is to be updated
* @param    portIntIfNum    @b{(input)} internal interface number of the physical port
*                           whose VLAN parameters are to be updated (VLAN membership,
*                           tagging, group membership).
*
*
*
* @returns  L7_SUCCESS  on successful update
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlMcastVlanPortMemberUpdate(L7_uint32 vid, L7_uint32 portIntIfNum);

#endif /* INCLUDE_DTL_L3_MCAST_API_H */
