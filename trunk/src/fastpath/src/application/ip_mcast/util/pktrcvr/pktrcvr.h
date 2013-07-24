/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pktrcvr.h
*
* @purpose    Packet Receiver library definitions
*
* @component  Multicast System Support Utilities
*
* @comments   none
*
* @create     08-Oct-06
*
* @author     Kamlesh Agrawal
* @end
*
**********************************************************************/

#ifndef _PKTRCVR_H
#define _PKTRCVR_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/

#include "l7_common.h"
#include "l3_mcast_commdefs.h"
#include "mcast_api.h"
#include "l7_socket.h"
/*******************************************************************************
**                             Definitions                                    **
*******************************************************************************/

/*******************************************************************************
**                     Structure Definitions                                  **
*******************************************************************************/

/*********************************************************************
*
* @structures  L7_fdList_t
*
* @purpose     This defines the Packet Fd  structure.
*
* @notes       Components interested in receiving control packets have to       
*              use this  structure.
*
* @end
*********************************************************************/

typedef struct L7_fdList_s
{
  /* Event Type */
  L7_uint32  evType;

  /* Packet Socket FD */
  L7_int32   pktSockFd;

  /* Buffer Pool for packet reception */
  L7_uint32  bufPool;

  /* Protocol-id to send the message to */
  L7_IPV6_PKT_RCVR_ID_TYPE_t id;

} L7_fdList_t;

/*********************************************************************
*
* @structures  L7_PktRcvr_t
*
* @purpose     This defines the Packet Receiver structure.
*
* @notes       Components interested in receiving IPv6 packets have to       
*              maintain this structure.
*
* @end
*********************************************************************/

typedef struct L7_PktRcvr_s
{
  /* Component ID of the Pkt Rcvr creator */
  L7_uint32 componentId;

 /*Maximum Number of Packet Fds*/
  L7_uint32 maxPktFds;

  fd_set            evRdFds; /*Read FD Set */

  L7_int32          fds_size; /* maxfd + 1 to watch*/

 /*Pointer to the list of Packet FD structure */
  L7_fdList_t fdList[0]; /* This should be last element in the struct */

} L7_PktRcvr_t;

/*********************************************************************
*
* @structures  L7_PktRcvrMsg_t
*
* @purpose     This defines the Packet Rcvr. generic Message structure.
*
* @notes       Components interested in receiving messages have to do    
*              do using this structure.
*
* @end
*********************************************************************/

typedef struct L7_PktRcvrMsg_s
{
  /* Event Type */
  L7_int32           event_type;

  /* Buffer Pool pointer */
  mcastControlPkt_t  recvBuf;
} L7_PktRcvrMsg_t;

/*******************************************************************************
**                 Function Prototype Declarations                            **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Creation of a Packet Receiver
*
* @param    componentId   @b{(input)}Component ID of the application calling 
*                                    this function.
* @param    maxPktFds     @b{(input)}Maximum number of FDs the application 
*                                    is intersted on listening                 
*
* @returns  pointer to the packet receiver structure, if success
* @returns  L7_NULLPTR, if failure
*
* @notes    This API is provided to create a packet receiver for a  
*           component.  This function is re-entrant.  
*
* @end
*
*********************************************************************/
extern void*
pktRcvrCreate (L7_uint32 componentId, L7_uint32 maxPktFds);

/*********************************************************************
*
* @purpose  Deletion of an Packet reciever
*
* @param    pktRcvr   @b{(input)}Data structure for this   
*                                pkt rcvr deletion request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to destroy packet receiver for a  
*           component.  This function is re-entrant.
*
* @end
*
*********************************************************************/
extern L7_RC_t
pktRcvrDestroy (L7_PktRcvr_t** pktRcvr);

/*********************************************************************
*
* @purpose  Receiving an event with the Packet Receiver                
*
* @param    L7_PktRcvr_t  pktRcvr   Data structure for this   
*                                   Packet Receiver
*                         eventMsg  The Msg to be posted
*                                   to the component
*                         blockFlag Specifies whether the
*                                   receiving from the Pipe
*                                   is a blocking call
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to receive an event and dequeue  
*           the corresponding message from the Packet Receiver's
*           Message List and invoking the function pointer
*           corresponding to the received event.  The eventMsg
*           that is returned has to be freed by the caller.
*
* @end
*********************************************************************/
extern L7_RC_t pktRcvrReceive (L7_PktRcvr_t *pktRcvr);

/*********************************************************************
*
* @purpose  Addition of a Packet FD to the Packet Receiver 
*
* @param    sockFd      @b{(input)}Socket FD on which the application wants to 
*                                  poll for control packets   
* @param    eventType   @b{(input)}Event Type the application is interested in                 
* @param    pktRcvr     @b{(input)}Data structure for the packer receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to add socket Fds in the packet receiver's structure.
            The fdlist is a sorted one.FD with highest value is the 1st element.  
*
* @end
*
*********************************************************************/
L7_RC_t
pktRcvrSocketFdRegister (L7_int32 sockFd, L7_uint32 evTypeId, L7_uint32 bufPoolId, 
                         L7_IPV6_PKT_RCVR_ID_TYPE_t id, L7_PktRcvr_t *pktRcvr);

/*********************************************************************
*
* @purpose  Deletion of a Packet FD from the Packet Receiver
*
* @param    sockFd  @b{(input)}Socket FD to be removed
* @param    pktRcvr @b{(input)}Data structure for the packet Receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to remove a socket Fd from the packet 
*           receiver structure.
*
* @end
*
*********************************************************************/
L7_RC_t
pktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_TYPE_t id, L7_PktRcvr_t *pktRcvr);

#endif /* _PKTRCVR_H */
