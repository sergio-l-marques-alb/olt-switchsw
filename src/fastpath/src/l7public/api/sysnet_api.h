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


#ifndef SYSNET_API_H
#define SYSNET_API_H

#include "l7_common.h"
#include "dot1q_exports.h"
#include "commdefs.h"
#include "default_cnfgr.h"
#include "osapi_sockdefs.h"  /* L7_AF_xx for register */

#define SYSNET_MAX_FUNC_NAME          40

typedef enum 
{
  SYSNET_INVALID_ENTRY = 0,
  SYSNET_SUBTYPE_ENTRY,
  SYSNET_MAC_ENTRY, 
  SYSNET_ETHERTYPE_ENTRY,
  SYSNET_DSAP_ENTRY,
  SYSNET_PKT_RX_REASON
} sysnet_pdu_entry_t;

/**********************************************************
*
* Sysnet Registrar Pdu Receive                   
*
**********************************************************/

typedef struct sysnet_pdu_info_s
{
  /* Originally, this structure contained only intIfNum and vlanId on the
   * receive side. Unfortunately, the meaning of intIfNum depends on context. 
   * intIfNum is originally set to the ingress physical port. 
   * Once IP MAP processes an incoming packet, it resets intIfNum to 
   * the logical (e.g., VLAN routing interface) ingress interface. See ipMapPduRcv().
   * All the software forwarding code and sysnet interceptors depend on 
   * this behavior. DHCP relay needs to know the ingress physical port to
   * set option 82 correctly. So we add a new member, rxPort, to this
   * structure as a place to put this. If we were starting over, intIfNum
   * would always have the same meaning and we'd add a field for the 
   * logical ingress interface, but I'm afraid we're stuck with 
   * a bunch of code that depends on the current behavior. NOTE:  rxPort is 
   * only set in IP MAP. So if packet hasn't been handled by IP MAP, you can't
   * use rxPort. */
  L7_uint32 intIfNum;          /* Receiving intIfNum */
  L7_uint32 rxPort;            /* Physical port where packet was received */
  L7_uint32 vlanId;            /* Receiving VLAN */
  L7_uint32 innerVlanId;        /* Receiving inner VLAN if the packet is double tagged.*/

  L7_uint32 destIntIfNum;      /* Destination intIfNum */
  L7_uint32 destVlanId;        /* Destination VLAN */

  L7_uint64 ts;                //PTIN added: SDK's timestamp
}sysnet_pdu_info_t;

typedef struct
{
  L7_char8           funcName[SYSNET_MAX_FUNC_NAME];
  sysnet_pdu_entry_t type;  
  union 
  {
    L7_uchar8   macAddr[L7_MAC_ADDR_LEN]; 
    L7_ushort16 protocol_type;
    L7_uchar8  subType;
    L7_uchar8   dsap;
    L7_uint32   rxReason;
  } u;
  L7_RC_t (*notify_pdu_receive)(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
  L7_BOOL           inUse;

} sysnetNotifyEntry_t;

typedef struct
{
  sysnetNotifyEntry_t sysnetNotifyEntries[FD_CNFGR_SYSNET_MAX_REGISTRATIONS];
  L7_uint32           curr;  /* Keeps track of the current number of entries in
                                the sysnetNotifyEntries array */

} sysnetNotifyList_t;


/************************
 * Sysnet PDU Intercept *
 ************************/

#define SYSNET_AF_MAX   FD_SYSNET_PDU_MAX_AF /* Max address families supported by SYSNET PDU Hooks */

/* Sysnet intercept return codes */
typedef enum
{
  SYSNET_PDU_RC_DISCARD = 0,  /* Discard this frame */
  SYSNET_PDU_RC_CONSUMED,     /* Frame has been consumed by interceptor */
  SYSNET_PDU_RC_COPIED,       /* Frame has been copied by interceptor */
  SYSNET_PDU_RC_IGNORED,      /* Frame ignored by interceptor */
  SYSNET_PDU_RC_PROCESSED,    /* Frame has been processed by interceptor */
  SYSNET_PDU_RC_MODIFIED,     /* Frame has been modified by interceptor */
  SYSNET_PDU_RC_LAST
} SYSNET_PDU_RC_t;

typedef struct sysnetPduRcCounters_s
{
  L7_uint32 sysnetPduRcDiscard;
  L7_uint32 sysnetPduRcConsumed;
  L7_uint32 sysnetPduRcCopied;
  L7_uint32 sysnetPduRcIgnored;
  L7_uint32 sysnetPduRcProcessed;
  L7_uint32 sysnetPduRcModified;
} sysnetPduRcCounters_t;

/* Function to be registered at specified hook point */
typedef SYSNET_PDU_RC_t (*SYSNET_INTERCEPT_FUNC_t) (L7_uint32 hookId,
                                                    L7_netBufHandle bufHandle,
                                                    sysnet_pdu_info_t *pduInfo,
                                                    L7_FUNCPTR_t continueFunc);

/* Structure passed by interceptor to register the specified address family/hook ID combination*/
typedef struct sysnetPduIntercept_s
{
  L7_uint32 addressFamily;  /* Address family, i.e. PF_INET */
  L7_uint32 hookId;         /* Associated with address family, i.e. SYSNET_INET_HOOKS_t */
  L7_uint32 hookPrecedence; /* Lowest number takes precedence */
  SYSNET_INTERCEPT_FUNC_t interceptFunc; /* Function to be called at specified hook */
  L7_uchar8 interceptFuncName[SYSNET_MAX_FUNC_NAME]; /* Intercept function name */
} sysnetPduIntercept_t;

typedef struct sysnetPduHooks_s
{
  L7_uint32 numHooks;
  SYSNET_INTERCEPT_FUNC_t interceptFuncList[L7_SYSNET_HOOK_PRECEDENCE_LAST];
  L7_uchar8 interceptFuncNameList[L7_SYSNET_HOOK_PRECEDENCE_LAST][SYSNET_MAX_FUNC_NAME];
} sysnetPduHooks_t;

#define SYSNET_PDU_INTERCEPT(af, hook, bufHdl, pduInf, contFunc, action) \
    sysNetPduIntercept((af), (hook), (bufHdl), (pduInf), (contFunc), (action))

/*
typedef L7_RC_t (*sysnetProtocol_pdu_receive)(L7_uint32 intIfNum, L7_netBufHandle bufHandle, L7_uint32 domainId);
typedef L7_RC_t (*sysnetGeneric_pdu_receive)(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
*/

/*********************************************************************
* @purpose  Determines if the specified vlan id is valid
*
* @param    vlanId    @b{(input)} The vlan id to be checked for validity
*
* @returns  L7_TRUE   If the specified vlan id is valid
* @returns  L7_FALSE  If the specified vlan id is invalid
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL sysNetIsValid8021qVlanId( L7_ushort16 vlanId );

/*********************************************************************
* @purpose  Determines the exit port & the destination vlan id
*
* @param    vlanId        @b{(input)}   Destination vlan id
* @param    intIfNum      @b{(input)}   Destination intIfNum
* @param    destMac       @b{(input)}   Pointer to the Destination mac addr
* @param    pExitPort     @b{(output)}  Pointer to the Exit Port
* @param    pDestVlanId   @b{(output)}  Pointer to the Destination Vlan
* @param    pflag         @b{(output)}  Pointer to the transmit Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Destination vlan id is set only if the entry has a
* @notes    valid vlan id else it is set to L7_DOT1Q_NULL_VLAN_ID 
*
* @end
*********************************************************************/
L7_RC_t sysNetExitPortGet(L7_ushort16 vlanId, L7_uint32 intIfNum, 
                          L7_uchar8* destMac, L7_uint32 *pExitPort,
                          L7_ushort16 *pDestVlanId, L7_ushort16 *pflag);

/*********************************************************************
* @purpose  Determines the encapsulation type
*
* @param    vlanId    @b{(input)}  Destination vlan Id
* @param    intIfNum  @b{(input)}  Destination Internal Interface Num
* @param    exitPort  @b{(input)}  Exit Port
*
* @returns  L7_TRUE   If the specified vlan id is valid
* @returns  L7_FALSE  If the specified vlan id is invalid
*
* @notes    none 
*
* @end
*********************************************************************/
L7_ushort16 sysNetEncapsulationTypeGet(L7_ushort16 vlanId, L7_uint32 intIfNum,
                                       L7_uint32 exitPort);

/*********************************************************************
* @purpose  Encapsulates the pdu with an ethernet or 
* @purpose  SNAP header depending on the specified flag
*
* @param    pflag     @b{(input)}   Pointer to the transmit flag
* @param    pdataStart @b{(output)} Pointer to location containing the 
*                                   starting address of the pdu
* @param    plen      @b{(output)}  Current length of the pdu  
* @param    datalen   @b{(input)}   Size of the ip packet
* @param    etype     @b{(input)}   Ethertype
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t sysNetPduEnetOrSnapHdrEncapsulate(L7_ushort16 *pflag, L7_char8 **pdataStart,
                                          L7_uint32 *plen, L7_uint32 datalen,
                                          L7_ushort16 etype);

/*********************************************************************
* @purpose  Allocates buffer space for a pdu and creates its 
* @purpose  header
*
* @param    destMac     @b{(input)}   Destination mac Address
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    pflag       @b{(output)}   Pointer to the transmit flag
* @param    datalen     @b{(input)}   Length of the ip packet
* @param    destVlanId  @b{(input)}   Destination vlan id
* @param    etype       @b{(input)}   Ethernet type
* @param    data        @b{(input)}   pointer to the start of the pdu
*                                     to be transmitted
* @param    pBufHandle  @b{(output)}  Stores the buffer handle returned
*                                     by sysapiNetMbufGet()
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t sysNetPduHdrEncapsulate(L7_uchar8* destMac, L7_uint32 intIfNum, 
                                L7_ushort16 *pflag, L7_uint32 datalen,
                                L7_uint32 destVlanId, L7_ushort16 etype,
                                L7_netBufHandle *pBufHandle, L7_char8 *data);

/*********************************************************************
* @purpose  Creates a vlan tagged pdu header
*
* @param    pdataStart  @b{(output)}  Pointer to location containing the
*                                       starting address of the pdu
* @param    plen        @b{(output)}  Current length of the pdu
* @param    destVlanId  @b{(input)}   Destination vlan id
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void sysNetPdu8021qHdrEncapsulate(L7_char8 **pdataStart, L7_uint32 *plen, 
                                  L7_ushort16 destVlanId);

/*********************************************************************
* @purpose  Forward frame to be transmitted to dtlPduTransmit
*
* @param    data        @b{(input)} Pointer to pdu to be xmitted
* @param    destMac     @b{(input)} Destination Mac Address
* @param    vlanId      @b{(input)} Destination vlan id
* @param    intIfNum    @b{(input)} Destination internal interface num
* @param    etype       @b{(input)} Ether type
* @param    datalen     @b{(input)} Total length of the pdu to be
*                                   transmitted
*
* @returns  none
*
* @notes    
*       
* @end
*********************************************************************/
void sysNetForward( L7_char8 *data, L7_uchar8* destMac, 
                    L7_ushort16 vlanId, L7_uint32 intIfNum,
                    L7_ushort16 etype, L7_uint32 datalen);

/*********************************************************************
* @purpose  Register a routine to be called when a pdu is received
*
* @param    *snEntry  @b{(input)} Registering routines need to provide
* @param              a) funcName: Name of the registered pdu receive
* @param              b) entry type: if SYSNET_MAC_ENTRY
* @param                                SYSNET_ETHERTYPE_ENTRY
* @param                                SYSNET_DSAP_ENTRY      
* @param              c) macAddr, protocol_type or dsap value depending
* @param                 on the type of entry (b)
* @param              d) Pointer to the associated pduReceive function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*                                 
* @end
*********************************************************************/
L7_RC_t sysNetRegisterPduReceive(sysnetNotifyEntry_t *snEntry);

/*********************************************************************
* @purpose  Deregister a routine from being called when a pdu is received
*
* @param    *snEntry  @b{(input)} Deregistering routines need to provide
* @param              a) funcName: Name of the registered pdu receive
* @param              b) entry type: if SYSNET_MAC_ENTRY
* @param                                SYSNET_ETHERTYPE_ENTRY
* @param                                SYSNET_DSAP_ENTRY      
* @param              c) macAddr, protocol_type or dsap value depending
* @param                 on the type of entry (b)
* @param              d) Pointer to the associated pduReceive function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*                                 
* @end
*********************************************************************/
L7_RC_t sysNetDeregisterPduReceive(sysnetNotifyEntry_t *snEntry);

/*********************************************************************
* @purpose  Notifies registered routines on receipt of the associated
*           pdu.
*
* @param    bufHandle   @b{(input)}pdu handle
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
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
L7_RC_t sysNetNotifyPduReceive (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/*********************************************************************
* @purpose  calculate where the actual data pcakets header starts devoid of L2 headers
*
* @param    *data       @b{(input)} Pointer to pdu that is received
*
* @returns  offset      @b{(output)} Number of bytes of header
*
* @notes    check if packet is lan tagged or LLC&SNAP or ethernet enacpsulations
* @notes                offset is dst(6)+src(6)+encaphrd(x)+protocoltype(2)
* @notes    vlan tagged  offset is 6+6+4+2 = 18
* @notes    LLC&SNAP    offset is 6+6+8+2 = 22
* @notes    Ether encap offset is 6+6+0+2 = 14    
*       
* @end
*********************************************************************/
L7_uint32 sysNetDataOffsetGet(L7_uchar8 *data);

/*********************************************************************
* @purpose  Removes the offset after the 802.1q VLAN tag if present 
*           from the pdu
*
* @param    *data       @b{(input)}pointer to the start of the pdu
*           
*
* @returns  *offset     @b{(output)}pointer to the point in the pdu
*                           following the VLAN tag
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uchar8* sysNetPost8021qVlanTagOffsetGet(L7_char8 *data);

/*********************************************************************
* @purpose  Return start of 802.1q VLAN tag if present 
*
* @param    *data       @b{(input)}pointer to the start of the pdu
*           
*
* @returns  *tag        @b{(output)}pointer to the vlan tag
*
* @notes    none
*       
* @end
*********************************************************************/
L7_8021QTag_t* sysNet8021qVlanTagGet(L7_char8 *data);

/*********************************************************************
* @purpose  Checks for the presence of 802.1Q E-RIF field in a packet
*
* @param    bufHandle   @b{(input)}pdu handle
*
* @param    *data       @b{(input)}pointer to the start of the pdu
*           
*
* @returns  flag       @b{(output)}flag indicating L7_TRUE if E-RIF 
*                           present else L7_FALSE if absent
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL sysNetHas8021qVlanERIF(L7_char8 *data);

/*********************************************************************
* @purpose  Call the registered intercept functions for the specified address family and hook ID
*
* @param    addressFamily    @b{(input)}  Address Family of hook (i.e. AF_INET)
* @param    hookdId          @b{(input)}  Hook ID being called (i.e. SYSNET_INET_IN)
* @param    bufHandle        @b{(input)}  Buffer handle to the frame
* @param    *pduInfo         @b{(input)}  Pointer to the PDU info (intIfNum, VLAN ID, etc.)
* @param    continueFunc     @b{(input)}  Function to be called to continue processing the frame
* @param    *hookVerdict     @b{(output)} Action taken by the hook intercept call
*
* @returns  L7_TRUE   Frame has either been discarded or consumed
* @returns  L7_FALSE  Caller should continue processing the frame
*
* @notes    If the frame is discarded or consumed, no further processing will be done
* @notes    on the frame.  In the case of discard, the network buffer is freed here.
* @notes    In the case of consumed, the interceptor should free the frame.  If the frame
* @notes    is neither discarded nor consumed, the continueFunc is called, which will continue
* @notes    processing the frame.  If the continueFunc is NULL, control will return to the
* @notes    caller (hook point) and frame processing will continue there.
*       
* @end
*********************************************************************/
L7_BOOL sysNetPduIntercept(L7_uint32 addressFamily,
                           L7_uint32 hookId,
                           L7_netBufHandle bufHandle,
                           sysnet_pdu_info_t *pduInfo,
                           L7_FUNCPTR_t continueFunc,
                           SYSNET_PDU_RC_t *hookVerdict);

/*********************************************************************
* @purpose  Initialize the Sysnet PDU Intercept hook structure
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetPduHooksInit();

/*********************************************************************
* @purpose  Register for a PDU hook 
*
* @param    *sysnetPduIntercept  @b{(input)} Pointer to intercept registration info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetPduInterceptRegister(sysnetPduIntercept_t *sysnetPduIntercept);

/*********************************************************************
* @purpose  De-Register a PDU hook 
*
* @param    *sysnetPduIntercept  @b{(input)} Pointer to intercept registration info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t sysNetPduInterceptDeregister(sysnetPduIntercept_t *sysnetPduIntercept);

/*********************************************************************
* @purpose  Strip off the VLan Tagging to the Layer2 header.
*
*
* @param    bufHandle      @b{(input)} Pointer to pdu that is received
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_uint32 sysNetDataStripVlanTag(L7_netBufHandle bufHandle);


#endif /* SYSNET_API_H */
