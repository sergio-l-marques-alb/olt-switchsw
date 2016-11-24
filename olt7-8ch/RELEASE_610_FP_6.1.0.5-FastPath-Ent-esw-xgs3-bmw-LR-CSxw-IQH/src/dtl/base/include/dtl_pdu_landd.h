/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_pdu_lan.h
*
* @purpose This header file contains definitions to support the       
*          LANDD component.      
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTLPDU_LANDD_H
#define INCLUDE_DTLPDU_LANDD_H
/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/



/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/


/*
**********************************************************************
*                           GLOBAL DATA
**********************************************************************
*/
#ifdef  DTLPDU_LANDD_GLOBALS
  #define DTLPDU_LANDD_EXT
#else 
  #define DTLPDU_LANDD_EXT extern
#endif  /*DTLPDU_LANDD_GLOBALS*/




/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/
/*********************************************************************
* @purpose  Receive the PDU puts it in a message queue and releases the interrupt thread
*
* @param    ddUSP          @b{(input)}pointer to the Unit Slot Port Structure
* @param    family         @b{(input)}a DAPI_FAMILY_t family
* @param    cmd            @b{(input)}a DAPI_CMD_t command
* @param    event          @b{(input)} A DAPI_EVENT_t event
* @param    dapiEventInfo  @b{(input)}pointer to Dapi event
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlPduReceiveCallback(DAPI_USP_t *ddusp, 
                                    DAPI_FAMILY_t family, 
                                    DAPI_CMD_t cmd, 
                                    DAPI_EVENT_t event,
                                    void *dapiEventInfo);

/*********************************************************************
* @purpose  Process a received PDU 
*
* @param    *ddUSP         @b{(input)} Pointer to the Unit Slot Port Structure
* @param    family         @b{(input)} A DAPI_FAMILY_t family
* @param    cmd            @b{(input)} A DAPI_CMD_t command
* @param    event          @b{(input)} A DAPI_EVENT_t event
* @param    *dapiEventInfo  @b{(input)} Pointer to Dapi event
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    provide traffic parameterization
*
* @end
*********************************************************************/
L7_RC_t dtlPduReceive(DAPI_USP_t *ddusp, 
                      DAPI_FAMILY_t family, 
                      DAPI_CMD_t cmd, 
                      DAPI_EVENT_t event,
                      void *dapiEventInfo);

/*********************************************************************
* @purpose  Receives IP packets
* 
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*           
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t dtlRecvIP(L7_netBufHandle netBufHandle, 
                  sysnet_pdu_info_t *pduInfo);

/*********************************************************************
* @purpose  Receives IPv6 packets
* 
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*           
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRecvIP6(L7_netBufHandle netBufHandle, 
                  sysnet_pdu_info_t *pduInfo);
/*********************************************************************
* @purpose  Receives ARP packets
* 
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*           
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlArpRecvIP(L7_netBufHandle netBufHandle, 
                     sysnet_pdu_info_t *pduInfo);

#endif        /*INCLUDE_DTLPDU_LANDD_H*/
