/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mfc_api.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the Multicast Forwarding Cache component/module.
*
* @component Multicast Forwarding Cache (MFC)
*
* @comments  All other components MUST use the functionalities exported
*            by this file only. ie. they should not include any other
*            header files within the MFC component directory structure.
*
* @create    January 18, 2006
*
* @author    ddevi
* @end
*
**********************************************************************/
#ifndef _MFC_API_H
#define _MFC_API_H

/**********************************************************************
  Include Files (only those absolutely necessary to compile this file)
**********************************************************************/

#include "l3_addrdefs.h" 
#include "l3_mcast_commdefs.h"
#include "intf_bitset.h" 

/**********************************************************************
                  Typedefs, Defines and Enums
**********************************************************************/

typedef enum
{
  MFC_MIN = 0,
  MFC_UPDATE_ENTRY,
  MFC_DELETE_ENTRY,
  MFC_INTERFACE_STATUS_CHNG,
  MFC_MAX,
}mfcOperationType_t;



typedef struct mfcEntry_s
{
  L7_uchar8          family;
  L7_inet_addr_t     source;
  L7_inet_addr_t     group;
  L7_uint32          iif;      /* Incoming Router Interface Index */
  interface_bitset_t oif;
  L7_uint32          dataTTL;  /* TTL of the last data pkt received */
  struct rtmbuf      *m;
  L7_uint32          mcastProtocol; /* Protocol Id from L7_MRP_TYPE */
} mfcEntry_t;

typedef struct mfcEntryStats_s
{
  L7_inet_addr_t source;
  L7_inet_addr_t group;
  L7_uint32      pktCount;
  L7_uint32      byteCount;
  L7_uint32      wrongIf;
} mfcEntryStats_t;


/* interface structure to be passed when interface mode changes */
typedef struct mfcInterfaceChng_s
{
   L7_uint32 rtrIfNum;      /* routing interface on which the mode changed */
   L7_uchar8 family;        /* IP family - L7_AF_INET, etc. */
   L7_BOOL   mode;          /* L7_ENABLE or L7_DISABLE */
} mfcInterfaceChng_t;


typedef L7_RC_t (*mfcNotifyFn_t)(L7_uchar8 family, L7_uint32 eventType, 
                                 L7_uint32 msgLen, void *pData);

/**********************************************************************
                         Function Prototypes
**********************************************************************/

/*********************************************************************
*
* @purpose  API function used by various modules to post messages to the
*           Multicast Forwarding Cache module.
*
* @param    eventType @b{(input)}Event Type to be posted.
* @param    msgLen      @b{(input)} Message Length.
* @param    pData     @b{(input)}Pointer to a buffer containing the message.
*
* @returns  L7_SUCCESS, if successful.
*           L7_FAILURE, if operation failed.
*
* @notes
*
* @end
*
*********************************************************************/
extern L7_RC_t mfcMessagePost(mfcOperationType_t eventType, void *pData, 
                              L7_uint32 msgLen);

/*********************************************************************
* @purpose  Register a routine to be called when changes occur within
*           the Multicast Forwarding Cache module that necessitates
*           communication with other modules for the given protocol.
*
*  @param    protocolId    @b{(input)}Protocol ID (A identified constant in L7_MRP_TYPE_t )
* @param    *notifyFn     @b{(input)}pointer to a routine to be invoked for MFC
*                                    changes.
* @param    *notifyFn     @b{(input)} wrongIfRateLimitEnable @b{(input)} Flag indicating
*                                                        whether Enable/Disable
* @param    *notifyFn     @b{(input)} wrongIfRateLimitThreshold @b{(input)} Rate Limit 
*                                                        Threshold  value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The registration from MRPs is reentrant, yet the notify list is not 
*            protected as function pointer location is fixed for each 
* @comments  The wrongIfRateLimitEnable has to be Set only by the MRPs which
*            processes the Wrong Interface Events.
*
* @end
*
*********************************************************************/
L7_RC_t mfcCacheEventRegister(L7_MRP_TYPE_t protocolId,
                              mfcNotifyFn_t notifyFn,
                              L7_BOOL wrongIfRateLimitEnable,
                              L7_uint32 wrongIfRateLimitThreshold);

/*********************************************************************
* @purpose  De-register an already registered notify function from the
*           Multicast Forwarding Cache module for the given protocol.
*
* @param    protocolId   @b{(input)}Protocol ID  (This MUST be from L7_MRP_TYPE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
extern L7_RC_t mfcCacheEventDeregister(L7_MRP_TYPE_t  protocolId);

/*********************************************************************
* @purpose  Retrieve the route details associated with a given Multicast
*           Forwarding Cache entry
*
* @param    pRtEntry @b{(inout)} Pointer to a route information structure
*                                Input  : The source and group addresses
*                                Output : Various route info associated with the
*                                given <source,group> cache entry
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
extern L7_BOOL mfcIsEntryInUse(mfcEntry_t *pRtEntry);

/*********************************************************************
* @purpose  Transmit the given IPv4 packet on a specific outgoing interface
*
* @param    rtrIfNum  @b{(input)}router interface number of the outgoing interface
* @param    pBuf      @b{(input)}buffer containing the packet to be sent
*
* @returns  L7_SUCCESS, Packet forwarded successfully.
* @returns  L7_FAILURE, Packet forwarding failed.
*
*
* @end
*
*********************************************************************/
extern L7_RC_t mfcIpPktDtlTransmitIntf(L7_uchar8 family, L7_uint32 rtrIfNum,
                        L7_uchar8 *pIpPkt, L7_uint32 ipPktLen);
/***********************************************************************
* @purpose Get Expiry time for a particular multicast entry.
* 
* params  ipMRtSrc    {(input)}  multicast source address.
*         ipMRtGrp    {(input)}  multicast group address.
*         expiryTime  {(output)} expiry time for multicast entry.
*         
* returns L7_SUCCESS : for suceessful get operation.
*         L7_FAILURE : for error in the operation.
*
* @comments
*
* @end
*
*************************************************************************/ 
extern L7_RC_t mfcEntryExpiryTimeGet(L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *ipMRtGrp,
                                     L7_uint32 *expiryTime);
#endif /*_MFC_API_H */
