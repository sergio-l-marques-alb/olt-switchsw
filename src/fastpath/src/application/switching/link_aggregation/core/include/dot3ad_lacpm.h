/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3ad_lacpm.h                        
*
* @purpose LACPM- Link Aggregation Control Parser Multiplexer includes and prototypes
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @notes  This component is responsible to parse control frames coming into LACP
*         LACPDU and Marker Frames. It is also responsible to multiplex
*         the LACPDU and Marker Response Frames out to the lower layers 
*
* @create 8/8/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef INCLUDE_DOT3AD_LACPM_H
#define INCLUDE_DOT3AD_LACPM_H

#include "sysnet_api.h"
/*************************************************************************
                            FUNCTION PROTOTYPES
*************************************************************************/
/**************************************************************************
*
* @purpose   this routine receives a Marker Response PDU  
*
* @param     intIfNum     the internal interface number the PDU was received in
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments  it changes the subtype from marker information to marker information response
*            and send it through the same interface it arrived in 
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMarkerResponder(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
/**************************************************************************
*
* @purpose   this routine receives a PDU from DTL, it parses the PDU and 
*
* @param     intIfNum     the internal interface number the PDU was received in
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments  determines whether it is a LACPDU or a Marker Response PDU 
*            if it is a LACPDU it puts the received notification in the LAC queue if it is 
*            a Marker Response PDU it sends it to the APM/MR module 
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPduReceive(L7_netBufHandle bufHandle,sysnet_pdu_info_t *pduInfo);

/**************************************************************************
*
* @purpose   Transmit a pdu to DTL  
*
* @param     intIfNum     the internal interface number the PDU is to be sent through
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes     transmits both LACPDU and Marker Response pdu
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmTransmit(L7_uint32 intIfNum, L7_uint32 bufHandle);

/**************************************************************************
*
* @purpose   Init the LACPM  
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     register the receive function with sysnet
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmInit(void);
/**************************************************************************
*
* @purpose   Remove any tagging that may be there between the src addr and the lengthType field  
*
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     register the receive function with sysnet
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmHeaderTagRemove(L7_netBufHandle bufHandle);
/**************************************************************************
*
* @purpose   Convert the packet from host byte order to 
*			 network byte order (big endianness)
*
* @param	 dot3ad_pdu_t	the handle to the LACPDU
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Applies to LACPDU only
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNetworkConvert(dot3ad_pdu_t * pdu);
/**************************************************************************
*
* @purpose   Convert the packet from network byte order  
*			 (big endianness) to host byte order
*
* @param	 bufHandle	the handle to the LACPDU
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Applies to LACPDU only
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmHostConvert(L7_netBufHandle bufHandle);

/**************************************************************************
*
* @purpose   Notify the helper about the transmission of the PDU 
*
* @param     intIfNum     the internal interface number the PDU is to be sent through
* @param     dot3ad_pdu_t *pdu to be transmit to the helper      
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @notes     This function checks with the shared memory if the PDU to be transmit 
*            is different from the stored or if the interval is different from the stored
*            value. If it is, it will overwrite the stored pdu /interval with the new 
*            information and notify the dot3ad transport task which is responsible to send 
*            the PDU to the helper.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNotifyHelperPduStart(dot3ad_pdu_t *pdu, L7_uint32 intIfNum);

/**************************************************************************
*
* @purpose   Notify the helper about stopping the transmission of the PDU 
*
* @param     intIfNum     the internal interface number the PDU is being sent
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @notes     
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNotifyHelperPduStop(L7_uint32 intIfNum);

/**************************************************************************
*
* @purpose   Notify the helper about modifying the timer interval 
*
* @param     intIfNum     the internal interface number the PDU is being sent
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @notes     
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNotifyHelperPduModify(L7_uint32 intIfNum, L7_uint32 interval);
#endif /*INCLUDE_DOT3AD_LACPM_H*/
