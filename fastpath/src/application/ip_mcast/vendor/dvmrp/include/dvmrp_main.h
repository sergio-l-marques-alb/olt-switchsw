/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_main.h
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
#ifndef _DVMRP_MAIN_H
#define _DVMRP_MAIN_H
#include "dvmrp_common.h"

#define L7_IGMP_BAD_CHKSUM        0xFF
#define DVMRP_VERSION "NOT DEFINED YET"

#define DVMRP_MAX_PDU       8192
#define DVMRP_NEIGHBORS     4       /* response to such a request */
#define DVMRP_INFO_REQUEST      10      /* information request */
#define DVMRP_INFO_REPLY        11      /* information reply */
#define DVMRP_INFO_VERSION      1       /* version string */
#define DVMRP_MASK_LEN  32
#define DVMRP_TTL 1



typedef struct  dvmtp_pkt_info_s 
{
  L7_int32     index;
  L7_ulong32 versionSupport;
  L7_inet_addr_t source;
  L7_int32 dvmrp_pkt_type; 
  L7_uchar8 *data;
  L7_int32 datalen;
}dvmtp_pkt_info_t;


/*******************FUNCTION PROTOTYPES**********************************/

/*********************************************************************
* @purpose   This function initializes all the control block parameters
*
* @param    dvmrpCB    @b{ (input) }Pointer to the  DVMRP Control Block.
*     
* @returns  L7_SUCCESS - Protocol has been initialized properly. *
* @returns  L7_ERROR   - Protocol has not been initialized properly.
*
* @notes    none.
*        
* @end
*********************************************************************/
L7_int32 dvmrpAdminModeSet(dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  This function is called to deinit DVMRP
*
* @param    dvmrpCB    @b{ (input) }Pointer to the  DVMRP Control Block.
* 
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 dvmrpAdminModeReset(dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  This function is used to handle the dvmrp ctrl pkt.
*
* @param    dvmrpcb   -  @b{(input)} DVMRP control block Handle.
* @param    dvmrpMsg -  @b{(input)} dvmrp  Msg.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCtrlPktHandler(dvmrp_t *dvmrpcb, mcastControlPkt_t * dvmrpMsg);


/*********************************************************************
* @purpose  This function process the DVMRP control packets
*
* @param    interface            -  @b{(input)} Pointer to the Interface through which the 
*                                                               packet has been recieved.
* @param    versionSupport   -  @b{(input)} Verison supported by the 
*                                                               neighbouring router
* @param    source               -  @b{(input)} Pointer to the Source address of the packet
* @param    dvmrp_pkt_type -  @b{(input)} type of the packet
* @param    data                  -  @b{(input)} pointer to the packet buffer
* @param    datalen              -  @b{(input)} length of the packet

* @returns  L7_SUCCESS - Packet has successfully been processed.
* @returns  L7_ERROR   - Packet processing encountered errors 
*
* @notes    Packet type is identified and the corresponding handler is called.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_recv (dvmrp_interface_t *interface, 
                     L7_ulong32 versionSupport, 
                     L7_dvmrp_inet_addr_t *source,
                     L7_int32 dvmrp_pkt_type, 
                     L7_uchar8 *data, L7_int32 datalen);


/*********************************************************************
* @purpose  This function sends the DVMRP packet to IGMP. 
*
* @param    code       - @b{(input)}  DVMRP packet type
* @param    dst         - @b{(input)}   Pointer to the Destination group being pruned
* @param    data       - @b{(input)}  Pointer to the packet buffer
* @param    len         - @b{(input)}   Length of the packet buffer
* @param    interface-  @b{(input)}Pointer to the Interface throufh which DVMRP packet 
*                                       has to be sent.
* 
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Sends the DVMRP packet to the IGMP.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_send (L7_int32 code, 
                     L7_dvmrp_inet_addr_t *dst, 
                     L7_uchar8 *data, L7_int32 len,
                     dvmrp_interface_t *interface);


#endif
