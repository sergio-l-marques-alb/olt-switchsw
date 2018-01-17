/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_protocol.h
*
* @purpose Contains structures,constants,macros, for dvmrp protocol
*
* @component
*
* @comments
*
* @create 07/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#ifndef _DVMRP_PROTOCOL_H_
#define _DVMRP_PROTOCOL_H_
#include <dvmrp_common.h>
#include "mcast_api.h"
#define DVMRP_GRAFT_TIMEOUT_VAL       5       /* retransmission time for 
                                                 grafts   */

#define DVMRP_PRUNE_REXMIT_VAL        3       /* initial time for prune 
                                                 rexmission*/

#define inet_touchar(prefix) ((L7_char8 *)&((prefix)->addr.ipv4.s_addr))


/*********************************************************************
* @purpose  This function processes the DVMRP probe packet
*
* @param    interface       -  @b{(input)}Pointer to the Interface through which the probe 
*                                        packet has been recieved.
* @param    versionSupport  -  @b{(input)} Verison supported by the 
*                                               neighbouring router
* @param    source          -  @b{(input)}Pointer to the Source address of the packet
* @param    data            -  @b{(input)} pointer to the packet buffer
* @param    datalen         -  @b{(input)}  length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    This function parses the probe packet and sees whether the 
*           packet is correct or not, if correct and if the packet is 
*           coming for the first time from this neighbour then a neighbor 
*           node is created and added  to the neighbour list.
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_probe_recv (dvmrp_interface_t *interface, L7_ulong32 versionSupport,
                          L7_dvmrp_inet_addr_t *source, L7_uchar8 *data, L7_int32 datalen);


/*********************************************************************
* @purpose  This function processes the DVMRP report packet
*
* @param    interface       -  @b{(input)}Pointer to the Interface through which the report 
*                                       packet has been recieved.
* @param    versionSupport  -  @b{(input)} Verison supported by the 
*                                       neighbouring router
* @param    source          -  @b{(input)} Pointer to the Source address of the packet
* @param    data            -  @b{(input)}  pointer to the packet buffer
* @param    datalen         -  @b{(input)} - length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    This function parses the report packet and sees whether 
*           the packet is correct or not, if correct ,all the routes 
*           present iin it are added to the route table.
* @end
*********************************************************************/
L7_RC_t dvmrp_report_recv(dvmrp_interface_t *interface, L7_ulong32 versionSupport,
                          L7_dvmrp_inet_addr_t *source, L7_uchar8 *data, L7_int32 datalen);

/*********************************************************************
* @purpose  This function sends the report packets out on an interface. 
*
* @param     no_of_rts_advertise     -  @b{(input)} No of routes to advertise.
* @param     interface                     -  @b{(input)}Pointer the interface
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_routes_send (L7_uint32 no_of_rts_advertise,
                            dvmrp_interface_t *interface);

/*********************************************************************
* @purpose  This function processes the DVMRP prune packet
*
* @param    interface        -  @b{(input)}Pointer to the Interface through which the prune 
*                                       packet has been recieved.
* @param    versionSupport   -  @b{(input)}  Verison supported by the 
*                                       neighbouring router
* @param    source           -  @b{(input)}Pointer to the source address of the packet
* @param    data             -  @b{(input)} Pointer to the packet buffer
* @param    datalen          -  @b{(input)}  length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
* 
* @notes    Handles the prune packet and updates the cache table accordingly.
* @end
*********************************************************************/
L7_RC_t dvmrp_prune_recv (dvmrp_interface_t *interface, L7_ulong32 versionSupport,
                          L7_dvmrp_inet_addr_t *source, L7_uchar8 *data, L7_int32 datalen);


/*********************************************************************
* @purpose  This function processes the DVMRP graft packet
*
* @param    interface       - @b{(input)}Pointer to the Interface throufh which the garft 
*                                                       packet  has been recieved.
* @param    versionSupport  -  @b{(input)} Verison supported by the 
*                                         neighbouring router
* @param    source - -  @b{(input)}Pointer to the Source address of the packet
* @param    data --  @b{(input)} pointer to the packet buffer
* @param    datalen --  @b{(input)} length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    Handles the graft packet and updated the cache table accordingly.
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_graft_recv (dvmrp_interface_t *interface, 
                          L7_ulong32 versionSupport, L7_dvmrp_inet_addr_t *source, 
                          L7_uchar8 *data, L7_int32 datalen);

/*********************************************************************
* @purpose  This function processes the DVMRP graft ack packet
*
* @param    interface - -  @b{(input)}Pointer to the Interface through which the graft 
*                                      ack packet has been recieved.
* @param    versionSupport - -  @b{(input)} Verison supported by the 
*                                      neighbouring router
* @param    source - -  @b{(input)}Pointer to the Source address of the packet
* @param    data -  -  @b{(input)} pointer to the packet buffer
* @param    datalen - -  @b{(input)} length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    Handles the graft ack packet and updates the cache table 
*           accordingly.
* @end
*********************************************************************/
L7_RC_t dvmrp_graft_ack_recv(dvmrp_interface_t *interface, 
                             L7_ulong32 versionSupport, L7_dvmrp_inet_addr_t *source, 
                             L7_uchar8 *data, L7_int32 datalen);

/*********************************************************************
* @purpose  This function sends the DVMRP graft ack packet
*
* @param    interface       -  @b{(input)}Pointer to the Interface throufh which the graft 
*                                         ack packet has to be sent.
* @param    graft_src            -  @b{(input)}Pointer to the Source node
* @param    graft_grp            -  @b{(input)}Pointer to the group node
* @para       nbr                -  @b{(input)}Pointer to the neighbor to whom the packet
*                                                     needs to be sent
*
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    Send the graft ack pack to the neighbour.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_graft_ack_send(dvmrp_interface_t *interface,L7_inet_addr_t *graft_src, 
                               L7_inet_addr_t *graft_grp,dvmrp_neighbor_t *nbr);


/*********************************************************************
* @purpose  This function sends the DVMRP graft packet
* 
* @param    entry               -  @b{(input)}Pointer to the cahce entry
* @param    nbr                  -  @b{(input)}Pointer to the neighbor node 
* @param    fromTimeout    -  @b{(input)} Graft node needs to added or not
* 
* @returns  L7_SUCCESS/L7_FAILURE.
* 
* @notes    Send the graft packet to the neighbour.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_graft_send (dvmrp_cache_entry_t *entry, dvmrp_neighbor_t *nbr, 
                           L7_int32 fromTimeout);


/*********************************************************************
* @purpose  This function sends the DVMRP probe packet
*
* @param    interface    -  @b{(input)}Pointer to the interface through which probe 
*                                          packet has to be sent.
* @param    addr         -  @b{(input)} Pointer to the address of the probe packet
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Send the probe packet on to the given interface.
* @end
*********************************************************************/
L7_int32 dvmrp_probe_send(dvmrp_interface_t *interface, 
                          L7_dvmrp_inet_addr_t *addr);


/*********************************************************************
* @purpose  This function sends the DVMRP probe packet
*
* @param    pParam -void pointer to get the required info.
* @returns  None.
*
* @notes    Send the probe packet on to the given interface.
* 
* @end
*********************************************************************/
void dvmrp_probe_sendHandler(void *pTemp);

/*********************************************************************
* @purpose  This function sends the DVMRP prune packet
*
* @param    interface    -        @b{(input)}Pointer to the interface through which probe
*                                               packet has to be sent.
* @param    src          -        @b{(input)}Pointer to the  Source network being pruned
* @param    dst          -        @b{(input)}Pointer to the  Destination group being pruned
* @param    lifetime     -        @b{(input)}  lifetime of the prune
* @param    neighbor     -        @b{(input)}Pointer to the Nighbor to whom the prune 
*                                                 should be sent
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends the prune packet on the given interface.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_prune_send(dvmrp_interface_t *interface, L7_inet_addr_t *src, 
                          L7_inet_addr_t *dst, L7_int32 lifetime, 
                          dvmrp_neighbor_t *neighbor);

/*********************************************************************
*
* @purpose  To De-initialize the AdminScope Boundary Database
*
* @param    dvmrpcb  @b{ (input) } Pointer to the DVMRP Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryDeInit (dvmrp_t *dvmrpcb);
/*********************************************************************
*
* @purpose  To Initialize the AdminScope Boundary database
*
* @param    dvmrpcb  @b{ (input) } Pointer to the DVMRP Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryInit (dvmrp_t *dvmrpcb);

/*********************************************************************
*
* @purpose   Compare function for the Admin Scope List
*
* @param     pData1 @b{ (input) } Pointer to Admin Scope Entry
*            pData2 @b{ (input) } Pointer to Admin Scope Entry
*            size   @b{ (input) } Size for the comparision 
*                            
* @returns   > 0  if pData1 > pData2
*            = 0 if pData1 == pData2
*            < 0 if pData1 < pData2
*
* @comments  None
*
* @end
*********************************************************************/
L7_int32
dvmrpAdminScopeEntryCompare (void* pData1,
                             void* pData2,
                             L7_uint32 size);


/*********************************************************************
*
* @purpose  To Initialize the AdminScope Boundary database
*
* @param    asbNode  @b{ (input) } Admin Scope Node
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/


L7_RC_t
dvmrpAdminScopeBoundaryNodeDelete (L7_sll_member_t *asbNode);
/*********************************************************************
*
* @purpose  Get the Adminscope Boundary Node for a Group
*
* @param    dvmrpcb        @b{ (input) } Pointer to the DVMRP Ctrl Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
dvmrpASBNode_t*
dvmrpAdminScopeNodeGet (dvmrp_t *dvmrpcb,
                        L7_inet_addr_t *grpAddr,
                        L7_inet_addr_t *grpMask);
/*********************************************************************
*
* @purpose  Set the Adminscope Boundary for a Group on an Interface
*
* @param    dvmrpcb        @b{ (input) } Pointer to the DVMRP Ctrl Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*           rtrIfIndex     @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundarySet (dvmrp_t *dvmrpcb,
                            L7_inet_addr_t *grpAddr,
                            L7_inet_addr_t *grpMask,
                            L7_uint32 rtrIfIndex);
/*********************************************************************
*
* @purpose  Reset the Adminscope Boundary for a Group on an Interface
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*           rtrIfIndex     @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryReset (dvmrp_t *dvmrpcb,
                              L7_inet_addr_t *grpAddr,
                              L7_inet_addr_t *grpMask,
                              L7_uint32 rtrIfIndex);
/*********************************************************************
*
* @purpose  Handler for Admin Scope Boundary Events
*
* @param    dvmrpcb     @b{ (input) } Pointer to the DVMRP Ctrl Block
*           asbEventType @b{ (input) } Admin Scope Event ID
*           asbInfo      @b{ (input) } Admin Scope Boundary Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryEventHandler (dvmrp_t *dvmrpcb,
                                     mcastAdminMsgInfo_t *asbInfo);

/*********************************************************************
*
* @purpose  Apply the boundary scope information
*
* @param    dvmrpcb      @b{ (input) } Pointer to the DVMRP Ctrl Block
*                 entry            cache entry  
*
*
* @notes
*
* @end
*********************************************************************/
void
dvmrpApplyAdminScope(dvmrp_t *dvmrpcb,dvmrp_cache_entry_t *entry);

#endif
