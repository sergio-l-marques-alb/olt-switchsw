/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name:       sysnet_util.c
*
* Purpose:    
*
* Component:  sysnet
*
* Comments: 
*
* Created by: asuthan 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/


#ifndef SYSNET_UTIL_H
#define SYSNET_UTIL_H

#define SYSNET_DOT1Q_CFI_BIT    0x10 
#define SYSNET_PROTOCOL_ID_LEN  2     /*in bytes*/
/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the
*           of reason code with which it is received by the CPU
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores reasonCode
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetFindReasonCodeMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                                  L7_char8 *data);
/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the 
*           of a mac address entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*           
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetFindMacAddrMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                               L7_char8 *data);

/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the 
*           of a protocol type entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*           
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetFindProtocolTypeMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                                    L7_char8 *data);

/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the 
*           of a dsap entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*           
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetFindDsapEntryMatch(sysnet_pdu_info_t *pduInfo,L7_netBufHandle bufHandle,
                                 L7_char8 *data);

/*********************************************************************
* @purpose  Iterate through the list of registered interceptors for the specified hook ID
*
* @param    interceptList[]  @b{(input)} List of interceptors for this hook ID
* @param    sysnetAFIndex    @b{(input)} Index into hook ID table
* @param    hookdId          @b{(input)} Hook ID being called (i.e. SYSNET_INET_IN)
* @param    bufHandle        @b{(input)} Buffer handle to the frame
* @param    *pduInfo         @b{(input)} Pointer to the PDU info (intIfNum, VLAN ID, etc.)
* @param    continueFunc     @b{(input)} Function to be called to continue processing the frame
*
* @returns  SYSNET_PDU_DISCARD    Discard this frame
* @returns  SYSNET_PDU_CONSUMED   Frame has been consumed by interceptor
* @returns  SYSNET_PDU_COPIED     Frame has been copied by interceptor
* @returns  SYSNET_PDU_IGNORED    Frame ignored by interceptor
* @returns  SYSNET_PDU_PROCESSED  Frame has been processed by interceptor
* @returns  SYSNET_PDU_MODIFIED   Frame has been modified by interceptor
*
* @notes    The hook table is semaphore protected in the calling function, sysNetPduIntercept
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t sysNetPduInterceptIterate(SYSNET_INTERCEPT_FUNC_t interceptList[],
                                          L7_uint32 sysnetAFIndex,
                                          L7_uint32 hookId,
                                          L7_netBufHandle bufHandle,
                                          sysnet_pdu_info_t *pduInfo,
                                          L7_FUNCPTR_t continueFunc);

/*********************************************************************
* @purpose  Convert an address family to an index into the SYSNET PDU hook table
*
* @param    af                @b{(input)} Address Family
* @param    *sysnetAFIndex    @b{(output)} Sysnet AF index into hook table
*
* @returns  L7_SUCCESS    If address family was found in the table
* @returns  L7_SUCCESS    If address family is not in the table
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetPduAFMapGet(L7_uint32 af, L7_uint32 *sysnetAFIndex);

/*********************************************************************
* @purpose  Get the next available slot in the sysnet AF map table
*
* @param    *sysnetAFIndex    @b{(output)} Sysnet AF index into hook table
*
* @returns  L7_SUCCESS    If address family was found in the table
* @returns  L7_SUCCESS    If address family is not in the table
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetPduAFMapNextAvailGet(L7_uint32 *sysnetAFIndex);

#endif /* SYSNET_UTIL_H */
