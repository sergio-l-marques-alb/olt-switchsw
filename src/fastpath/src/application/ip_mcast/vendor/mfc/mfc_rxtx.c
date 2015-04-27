/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mfc_rxtx.c
*
* @purpose    Implements the Packet Reception/Transmission mechanisms
*             of the Multicast Forwarding Cache (MFC) module for IPv4
*
* @component  Multicast Forwarding Cache (MFC)
*
* @comments   none
*
* @create     24-Jan-06
*
* @author     ddevi.
* @end
*
**********************************************************************/
/**********************************************************************
                  Includes
***********************************************************************/
#include "log.h"
#include "buff_api.h"
#include "mfc_api.h"
#include "sysapi.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"
#include "l3end_api.h"
#include "l7_ip_api.h"
#include "datatypes.h"
#include "rtip.h"
#include "rtiprecv.h"
#include "osapi.h"
#include "dtl_l3_mcast_api.h"
#include "l7_packet.h"
#include "mcast_defs.h"
#include "mcast_rtmbuf.h"
#include "mcast_wrap.h"
#include "mcast_debug.h"
#include "mfc_rxtx.h"
#include "mfc.h"
#include "mfc_debug.h"
#include "mcast_rtmbuf.h"
#include "l7_mcast_api.h"
#include "mfc_v6.h"
#include "snooping_api.h"
#include "dot1q_api.h"

#ifdef L7_NSF_PACKAGE
#include "mcast_ckpt.h"
#endif

#define MFC_IPV4_TUNNEL_LEN  12 /* bytes of IP option for tunnel encapsulation*/

/*****************************************************************
    Function Definitions
******************************************************************/
/*********************************************************************
*
* @purpose  Update the hardware with appropriate forwarding information
*           for the given multicast entry
*
* @param    pSource       @b{(input)}pointer to the source IP address
* @param    pGroup        @b{(input)}pointer to the multicast group IP address
* @param    iif           @b{(input)}incoming interface index (router ifIndex)
* @param    oif           @b{(input)}bitmap of the outgoing router interfaces 
* @param    mcastProtocol @b{(input)}multicast protocol associated with the entry
*
* @returns  L7_SUCCESS, if operation successful
* @returns  L7_FAILURE, if operation failed
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t mfcHwEntryUpdate(mfcCacheEntry_t* pData,
                         L7_BOOL forceDrop)
{
  L7_inet_addr_t *pSource;
  L7_inet_addr_t *pGroup;
  L7_uint32 rtrIfNumIncoming;
  interface_bitset_t *oif;
  L7_uint32 mcastProtocol;
  L7_uint32              intIfNum;
  L7_uint32              rxPort;
  L7_multicastForwList_t mfl;
  L7_uint32              i;
  L7_uchar8              grp[IPV6_DISP_ADDR_LEN];
  L7_uchar8              src[IPV6_DISP_ADDR_LEN];
  L7_VLAN_MASK_t         outVlanRtrIntfMask;
  L7_uint32              vlanId;
  L7_BOOL                snoopNotify = L7_FALSE;
  mfcCacheEntry_t  *pEntry = L7_NULLPTR;

  /* Basic parameter validations */
  if ((pData == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mfcEntry_t for Hardware mroute updation");
    return L7_FAILURE;
  }
  pSource = &pData->source;
  pGroup =  &pData->group;
  rtrIfNumIncoming = pData->iif;
  rxPort = pData->rxPort;
  oif = &pData->oif;
  mcastProtocol =pData->mcastProtocol;
  if (rtrIfNumIncoming < 1 ||  rtrIfNumIncoming >= MAX_INTERFACES || oif ==L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," invalid input interface number = %d", rtrIfNumIncoming);
    return L7_FAILURE;
  }

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return L7_FAILURE;
  }
  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pData->source), &(pData->group), AVL_EXACT);

  /* When this mfcEntryUpdate function is called by MRP,if the mfc entry is not there either 
     because the entry itself not got created at all or entry was created but it got deleted 
     because of the mfc timer expiry, then for the corresponding source and group entry should 
     not be created here.Mfc entry creation trigger should always be the first data packet arrival
     only.Creatin of entry here if the entrry is not found raised lot of problems for PIMSM.
   */

  if (pEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n No Entry So return from here ");  
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return L7_FAILURE;
  }

  if ((pEntry->iif == pData->iif) &&
      (memcmp(&pEntry->oif, &pData->oif,sizeof(interface_bitset_t)) == 0)&&
      (pEntry->mcastProtocol == pData->mcastProtocol) &&
      (pEntry->rpfAction == forceDrop))
  {
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    MFC_DEBUG (MFC_DEBUG_EVENTS, "Not allowing Duplicate Entry Addition to the H/W");
    MFC_DEBUG_ADDR (MFC_DEBUG_EVENTS, "Source : ", &pData->source);
    MFC_DEBUG_ADDR (MFC_DEBUG_EVENTS, "Group  : ", &pData->group);
    return L7_SUCCESS;
  }
  
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",pSource);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",pGroup);
  MFC_DEBUG(MFC_DEBUG_APIS," incoming interface : %d", rtrIfNumIncoming);
  MFC_DEBUG(MFC_DEBUG_APIS," mcast protocol : %d", mcastProtocol);

  if (mcastIpMapRtrIntfToIntIfNum(pSource->family, rtrIfNumIncoming, &intIfNum)
                                  != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n Failed to convert to internal interface number for rtrIfNum : %d", rtrIfNumIncoming);
    return L7_FAILURE;
  }

  /* Clean-up the hardware structure and fill it up with the appropriate
     parameters */
  memset(&mfl, 0, sizeof(L7_multicastForwList_t));
  memset(&outVlanRtrIntfMask, 0, sizeof(L7_VLAN_MASK_t));
  if (forceDrop == L7_TRUE)
  {
    mfl.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
  }
  else 
  {
    switch (mcastProtocol)
    {
      case L7_MRP_DVMRP:
      case L7_MRP_MGMD_PROXY:
        /* If packet fails rpf check, drop it in Hardware */
        mfl.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
        break;
      case L7_MRP_PIMDM:
      case L7_MRP_PIMSM:
        /* If packet fails rpf check, send it up to the CPU for further processing */
        mfl.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
        break;
      default:
        MFC_DEBUG(MFC_DEBUG_FAILURES," Unknown protocol identifier %d  for hardware updation ", mcastProtocol);
        return L7_FAILURE;
    }
  }
  /* For all Multicast packets, always perform RPF by matching with the Incoming
     Interface in the route entry */
  mfl.typeOfReversePathCheck = L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH;

  /* Convert the given rtrIfNumIncoming and oif parameters to appropriate Internal Interface
     numbers and bitmaps. The given ones are based on Router Interface indices */
  mfl.inIntIfNum = intIfNum;
  mfl.inPhysicalIntIfNum = rxPort;
  if (oif != L7_NULLPTR)
  {
    for (i = 0; i < MAX_INTERFACES; i++)
    {
      if (BITX_TEST(oif, i))
      {
        snoopNotify = L7_TRUE;
        if (mcastIpMapRtrIntfToIntIfNum(pSource->family, i, &intIfNum)
            != L7_SUCCESS)
        {
          MFC_DEBUG(MFC_DEBUG_FAILURES,
                    "\n Failed to convert to internal interface number for rtrIfNum : %d",
                    i);
          return L7_FAILURE;
        }
        L7_INTF_SETMASKBIT(mfl.outIntIfMask, intIfNum);

        if (dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
        {
          /* set out vlan routing interfaces mask */
          L7_VLAN_SETMASKBIT(outVlanRtrIntfMask, vlanId);
        }
      }
    }
  }
  /* Group and Source address setup */
  inetCopy(&(mfl.mcastGroupDestAddress), pGroup);
  inetCopy(&(mfl.sourceIpAddress), pSource);
  /* Enable RPF checks for packets on this <S,G> */
  mfl.reversePathCheck      = L7_TRUE;

  /* Below Block is added only for Debugging */
  if (mfcDebugFlagCheck (MFC_DEBUG_HW_UPDATES) == L7_TRUE)
  {
    L7_uchar8 src[IPV6_DISP_ADDR_LEN];
    L7_uchar8 grp[IPV6_DISP_ADDR_LEN];
    L7_BOOL oifIsEmpty = L7_TRUE;
    L7_uint32 i=0;

    MFC_DEBUG_PRINTF ("\n----------MFC->HW ... UPDATE----------\n");
    MFC_DEBUG_PRINTF ("S- %s, ", inetAddrPrint (pSource, src));
    MFC_DEBUG_PRINTF ("G- %s, ", inetAddrPrint (pGroup, grp));
    MFC_DEBUG_PRINTF ("I- %d, ", rtrIfNumIncoming);
    MFC_DEBUG_PRINTF ("O- ");
    BITX_IS_EMPTY (oif, oifIsEmpty);
    if (oifIsEmpty == L7_TRUE)
    {
      MFC_DEBUG_PRINTF ("None");
    }
    else
    {
      for (i = 0; i < MCAST_MAX_INTERFACES; i++)
    {
        if (BITX_TEST(oif, i) != L7_NULL)
        MFC_DEBUG_PRINTF ("%d ", i);
    }
    }
    MFC_DEBUG_PRINTF (".\n");
  }

  /* Update the hardware */
  if (L7_SUCCESS != dtlMulticastAddrAdd(&mfl))
  {
    MFC_LOG_MSG (pSource->family,"MFC Entry (src,grp)=(%s, %s) Add in DTL Failed",
                 inetAddrPrint(&mfl.sourceIpAddress,src),
                 inetAddrPrint(&mfl.mcastGroupDestAddress,grp));
    MFC_DEBUG_ADDR(MFC_DEBUG_HW_UPDATES,
                   "\n Failed to update mroute in Hardware of source :",
                   &(mfl.sourceIpAddress));
    MFC_DEBUG_ADDR(MFC_DEBUG_HW_UPDATES,
                   "\n Failed to update mroute in Hardware of group :",
                   &(mfl.mcastGroupDestAddress));
    return L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  mcastCkptEntrySync(pSource, pGroup, rtrIfNumIncoming, rxPort, L7_TRUE);
#endif

  /* Source Routing interface notification */
  if (dot1qVlanIntfIntIfNumToVlanId(mfl.inIntIfNum, &vlanId) == L7_SUCCESS)
  {
    snoopNotify = L7_TRUE;
  }
  else
  {
    vlanId = L7_NULL;
  }

  if (snoopNotify == L7_TRUE)
  {
    /* Notify snooping component of entry addition */
    snoopMcastNotify(pGroup, pSource, vlanId, &outVlanRtrIntfMask, L7_TRUE);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete the given multicast entry from hardware
*
* @param    pSource       @b{(input)}pointer to the source IP address
* @param    Group         @b{(input)}pointer to the multicast group IP address
* @param    iif           @b{(input)}incoming interface index (router ifIndex)
* @param    mcastProtocol @b{(input)}multicast protocol associated with the entry
*
* @returns  L7_SUCCESS, if operation successful
* @returns  L7_FAILURE, if operation failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcHwEntryDelete(L7_inet_addr_t *pSource,
                         L7_inet_addr_t *pGroup,
                         L7_uint32 rtrIfNumIncoming,
                         L7_uint32 mcastProtocol)
{
  L7_uint32              intIfNum;
  L7_multicastForwList_t mfl;
  L7_uchar8              addrBufStr[IPV6_DISP_ADDR_LEN];


  /* Basic parameter validations */
  if ((pSource == L7_NULLPTR) || (pGroup == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL source or group for Hardware mroute updation");
    return L7_FAILURE;
  }

  if (rtrIfNumIncoming < 1 ||  rtrIfNumIncoming >= MAX_INTERFACES)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," invalid input interface number = %d", rtrIfNumIncoming);
    return L7_FAILURE;
  }


  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",pSource);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",pGroup);
  MFC_DEBUG(MFC_DEBUG_APIS," incoming interface : %d", rtrIfNumIncoming);
  MFC_DEBUG(MFC_DEBUG_APIS," mcast protocol : %d", mcastProtocol);

  if (mcastIpMapRtrIntfToIntIfNum(pSource->family, rtrIfNumIncoming, &intIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n Failed to convert to internal interface number for rtrIfNum : %d",
              rtrIfNumIncoming);
    return L7_FAILURE;
  }

  /* Clean-up the hardware structure and fill it up with the appropriate
     parameters */
  memset(&mfl, 0, sizeof(L7_multicastForwList_t));
  switch (mcastProtocol)
  {
    case L7_MRP_DVMRP:
    case L7_MRP_MGMD_PROXY:
      /* If packet fails rpf check, drop it in Hardware */
      mfl.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
      break;
    case L7_MRP_PIMDM:
    case L7_MRP_PIMSM:
      /* If packet fails rpf check, send it up to the CPU for further processing */
      mfl.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
      break;
    default:
      MFC_DEBUG(MFC_DEBUG_FAILURES," Unknown protocol identifier %d  for hardware updation ", mcastProtocol);
      return L7_FAILURE;
  }
  /* For all Multicast packets, always perform RPF by matching with the Incoming
     Interface in the route entry */
  mfl.typeOfReversePathCheck = L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH;

  /* Convert the given rtrIfNumIncoming to its corresponding Internal Interface number */
  mfl.inIntIfNum = intIfNum;
  /* Group and Source address setup */
  inetCopy(&(mfl.mcastGroupDestAddress), pGroup);
  inetCopy(&(mfl.sourceIpAddress), pSource);
  /* Enable RPF checks for packets on this <S,G> */
  mfl.reversePathCheck = L7_TRUE;

  /* Below Block is added only for Debugging */
  if (mfcDebugFlagCheck (MFC_DEBUG_HW_UPDATES) == L7_TRUE)
  {
    L7_uchar8 src[IPV6_DISP_ADDR_LEN];
    L7_uchar8 grp[IPV6_DISP_ADDR_LEN];

    MFC_DEBUG_PRINTF ("\n----------MFC->HW ... DELETE----------\n");
    MFC_DEBUG_PRINTF ("S- %s, ", inetAddrPrint (pSource, src));
    MFC_DEBUG_PRINTF ("G- %s, ", inetAddrPrint (pGroup, grp));
    MFC_DEBUG_PRINTF ("I- %d", rtrIfNumIncoming);
    MFC_DEBUG_PRINTF (".\n");
  }

  /* Update the hardware */
  if (L7_SUCCESS != dtlMulticastAddrDelete(&mfl))
  {
    MFC_LOG_MSG (pSource->family, "MFC Entry (src,grp)=(%s, %s) Delete in DTL Failed",
                inetAddrPrint(&mfl.sourceIpAddress,addrBufStr),
                inetAddrPrint(&mfl.mcastGroupDestAddress,addrBufStr));    
    MFC_DEBUG_ADDR(MFC_DEBUG_HW_UPDATES,
                   "\n Failed to delete mroute in Hardware of source :",
                   &(mfl.sourceIpAddress));
    MFC_DEBUG_ADDR(MFC_DEBUG_HW_UPDATES,
                   "\n Failed to delete mroute in Hardware of group :",
                   &(mfl.mcastGroupDestAddress));
    return L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  mcastCkptEntryDelete(pSource, pGroup);
#endif

    /* Notify snooping component of entry deletion */
  snoopMcastNotify(pGroup, pSource, L7_NULL, L7_NULLPTR, L7_FALSE);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Change the mode of the given interface in hardware
*
* @param    pInterfaceChange    @b{(input)} pointer to a structure containing 
*                                           the mode change info
*
* @returns  L7_SUCCESS, if operation successful
* @returns  L7_FAILURE, if operation failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcHwInterfaceStatusChange(mfcInterfaceChng_t *pInterfaceChange)
{
  /* Basic parameter validations */
  if (pInterfaceChange == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL interface info for Hardware interface status updation");
    return L7_FAILURE;
  }

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry, family = %x", pInterfaceChange->family);
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry, rtrIfNum = %x",pInterfaceChange->rtrIfNum);
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry, mode = %x", pInterfaceChange->mode);

  /* Enable/Disable the interface on the underlying IP stack */
  if (pInterfaceChange->family == L7_AF_INET)
  {
    /* Do nothing as of now */
  }
  else if (pInterfaceChange->family == L7_AF_INET6)
  {
    return(mfcIpv6HwInterfaceStatusChange(pInterfaceChange->rtrIfNum,
                                          pInterfaceChange->mode));
  }
  else
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Unsupported Address Family ");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Checks for the usage of the given multicast entry in hardware
*
* @param    pSource        @b{(input)}pointer to the source IP address
* @param    pGroup         @b{(input)}pointer to the multicast group IP address
* @param    iif            @b{(input)}incoming interface index (router ifIndex)
* @param    mcastProtocol  @b{(input)}multicast protocol associated with the entry
*
* @returns  L7_TRUE, if entry is in use.
*                    (ie. packets were forwarded since last check)
* @returns  L7_FALSE,if entry not in use or if any errors in verifying
*
* @comments Entry usage by hardware is determined by checking the entry
*           statistics in hardware for any forwarded packets since last check
*
* @end
*
*********************************************************************/
L7_BOOL mfcHwUseCheck(L7_inet_addr_t *pSource,
                      L7_inet_addr_t *pGroup,
                      L7_uint32 rtrIfNumIncoming,
                      L7_uint32 mcastProtocol)
{
  L7_uint32 intIfNum;
  L7_multicastForwEntryUseQuery_t stats;
  L7_uchar8              grp[IPV6_DISP_ADDR_LEN];
  L7_uchar8              src[IPV6_DISP_ADDR_LEN];

  /* Basic parameter validations */
  if ((pSource == L7_NULLPTR) || (pGroup == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL source or group for Hardware mroute updation");
    return L7_FAILURE;
  }

  if (rtrIfNumIncoming < 1 ||  rtrIfNumIncoming >= MAX_INTERFACES)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," invalid input interface number = %d", rtrIfNumIncoming);
    return L7_FAILURE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",pSource);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",pGroup);
  MFC_DEBUG(MFC_DEBUG_APIS," incoming interface : %d", rtrIfNumIncoming);
  MFC_DEBUG(MFC_DEBUG_APIS," mcast protocol : %d", mcastProtocol);

  if (mcastIpMapRtrIntfToIntIfNum(pSource->family, rtrIfNumIncoming, &intIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n Failed to convert to internal interface number for rtrIfNum : %d",
              rtrIfNumIncoming);
    return L7_FAILURE;
  }

  /* Clean-up the hardware structure and fill it up with the appropriate
       parameters */
  memset(&stats, 0, sizeof(L7_multicastForwEntryUseQuery_t));
  switch (mcastProtocol)
  {
    case L7_MRP_DVMRP:
    case L7_MRP_MGMD_PROXY:
      /* If packet fails rpf check, drop it in Hardware */
      stats.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
      break;
    case L7_MRP_PIMDM:
    case L7_MRP_PIMSM:
      /* If packet fails rpf check, send it up to the CPU for further processing */
      stats.reversePathCheckFailAction = L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
      break;
    default:
      MFC_DEBUG(MFC_DEBUG_FAILURES," Unknown protocol identifier %d  for hardware updation ", mcastProtocol);
      return L7_FALSE;
  }
  /* For all Multicast packets, always perform RPF by matching with the Incoming
     Interface in the route entry */
  stats.typeOfReversePathCheck = L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH;

  /* Convert the given rtrIfNumIncoming to its corresponding Internal Interface number */
  stats.inIntIfNum = intIfNum;
  /* Group and Source address setup */
  inetCopy(&(stats.mcastGroupDestAddress), pGroup);
  inetCopy(&(stats.sourceIpAddress), pSource);
  /* Enable RPF checks for packets on this <S,G> */
  stats.reversePathCheck      = L7_TRUE;

  /* Retrieve hardware statistics */
  if (L7_SUCCESS != dtlMulticastSGUseGet(&stats))
  {
    MFC_LOG_MSG(pSource->family,"MFC Entry (src,grp)=(%s, %s) Hardware inUseGet Failed",
                inetAddrPrint(&stats.sourceIpAddress,src),
                inetAddrPrint(&stats.mcastGroupDestAddress,grp));
    MFC_DEBUG_ADDR(MFC_DEBUG_APIS,
                   "\n Failed to get mroute Hardware use  of source :",
                   &(stats.sourceIpAddress));
    MFC_DEBUG_ADDR(MFC_DEBUG_APIS,
                   "\n Failed to get mroute Hardware use of group :",
                   &(stats.mcastGroupDestAddress));
    return L7_FALSE;
  }

  /* Is the entry in use by Hardware? */
  if (stats.entry_used == L7_TRUE)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}
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
L7_RC_t mfcIpPktDtlTransmitIntf(L7_uchar8 family, L7_uint32 rtrIfNum,
                        L7_uchar8 *pIpPkt, L7_uint32 ipPktLen)
{
  struct ip         *pIpSend = L7_NULLPTR;
  struct ip ipSend;
  L7_ip6Header_t    *pIp6Hdr = L7_NULLPTR;
  L7_ip6Header_t ip6Hdr;
  L7_uint32         intIfNum;
  L7_uchar8 destMac[6], *pData;
  L7_netBufHandle tmpHandle;
  sysnet_pdu_info_t sysnetPduInfo;
  SYSNET_PDU_RC_t   hookVerdict;
  L7_ushort16 flag = 0;
  L7_ushort16 etype;
  L7_uint32 dataLen;  
  L7_uint32 innerGrp; 
  L7_ulong32 tmp;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , rtrIfNum = %d, pIpPkt = %x",
            rtrIfNum, pIpPkt);
  MFC_DEBUG(MFC_DEBUG_TX,"\n Transmission of Ip pkt ");

  /* Basic parameter validations */
  if (pIpPkt == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL packet to forward");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n  invalid input parameters");
    return L7_FAILURE;
  }
  /* Compute the internal interface number for the given outgoing interface */
  if (mcastIpMapRtrIntfToIntIfNum(family,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "Failed to convert to internal interface number for rtrIfNum:%d", 
              rtrIfNum);
    return L7_FAILURE;
  }
  /* Prepare packet to be sent */
  if(family == L7_AF_INET)
  {
     memcpy (&ipSend, pIpPkt, sizeof(struct ip));
     pIpSend = &ipSend;
     pIpSend->ip_ttl --;
     if (pIpSend->ip_ttl == 0)
     {
       MFC_DEBUG(MFC_DEBUG_FAILURES, "Invalid TTL");
       return L7_FAILURE;
     }     
     pIpSend->ip_sum = 0;
     pIpSend->ip_sum = inetChecksum((L7_short16 *)pIpSend, pIpSend->ip_hl * 4);
     etype = L7_ETYPE_IP;
     memcpy(&tmp,&(pIpSend->ip_dst.s_addr),sizeof(tmp));
     innerGrp = osapiNtohl(tmp);

     destMac[0] = 0x01;
     destMac[1] = 0x00;     
     destMac[2] = 0x5e; 
     destMac[3] = (innerGrp & 0x007f0000) >> 16;
     destMac[4] = (innerGrp & 0x0000ff00) >> 8;
     destMac[5] = innerGrp & 0x000000ff;
 }
  else if(family == L7_AF_INET6)
  {
    memcpy (&ip6Hdr, pIpPkt, sizeof(L7_ip6Header_t));
    pIp6Hdr = &ip6Hdr;
    pIp6Hdr->hoplim --;
    if (pIp6Hdr->hoplim == 0)
    {
      MFC_DEBUG(MFC_DEBUG_FAILURES, "Invalid Hop-Limit");
      return L7_FAILURE;
    }      
    destMac[0] = 0x33;
    destMac[1] = 0x33;
    destMac[2] = pIp6Hdr->dst[L7_IP6_ADDR_LEN - 4];
    destMac[3] = pIp6Hdr->dst[L7_IP6_ADDR_LEN - 3];
    destMac[4] = pIp6Hdr->dst[L7_IP6_ADDR_LEN - 2];
    destMac[5] = pIp6Hdr->dst[L7_IP6_ADDR_LEN - 1];  
    etype = L7_ETYPE_IPV6;
  }  
  else
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, "Wrong family type");
    return L7_FAILURE;
  }
  flag = L7_LL_ENCAP_ENET | L7_LL_USE_L3_SRC_MAC;   
  dataLen = ipPktLen; 
  if(sysNetPduHdrEncapsulate(destMac, intIfNum, &flag, dataLen,
                               0, etype,(L7_netBufHandle*)&tmpHandle, 
                               (L7_char8 *)pIpPkt) 
                               == L7_SUCCESS)
  { 
    SYSAPI_NET_MBUF_GET_DATASTART(tmpHandle, pData);  
    /* Copy the Source MAC address to the data */
    ipMapRouterIfMacAddressGet(intIfNum, &(pData[6]));
    MFC_DEBUG(MFC_DEBUG_FAILURES," sysNetPduHdrEncapsulate() success");
    /* remember to free tmpHandle below*/
  }
  else 
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," sysNetPduHdrEncapsulate() failed");
    return L7_FAILURE;      
  }

    /* Call interceptors who are interested in outgoing IP multcast frames to be
     forwarded.If L7_TRUE is returned, the frame was either discarded or consumed,
     which means that the network buffer has been freed by the intercept call, 
     or has been copied by the consumer.

     Note that for this intercept point, the buffer is referenced through the
     rtmbuf which is freed here if the caller consumes it.  So, the interceptor
     should make a copy of the frame if processing is done on another task. */
  memset(&sysnetPduInfo, 0, sizeof(sysnet_pdu_info_t));
  sysnetPduInfo.destIntIfNum = intIfNum;
  if (SYSNET_PDU_INTERCEPT(family,
                           SYSNET_INET_MFORWARD_OUT,
                           (L7_netBufHandle)(tmpHandle),
                           &sysnetPduInfo,
                           L7_NULLPTR,
                           &hookVerdict) == L7_TRUE)
  {
    return L7_SUCCESS;
  }
  /* If not intercepted, send the packet */
  if (ipmRouterIfBufSend(intIfNum, (L7_netBufHandle)(tmpHandle))
      != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"failed to tranmit packet over oif = %d ", rtrIfNum); 
    MFC_DEBUG(MFC_DEBUG_TX,
              "\n failed to tranmit packet through DTL over oif = %d",
              intIfNum);
    return L7_FAILURE;
  }
  else 
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," ipmRouterIfBufSend() success");
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Transmit the given IPv4 packet on a specific outgoing interface
*
* @param    rtrIfNum  @b{(input)}router interface number of the outgoing interface
* @param    pBuf      @b{(input)}buffer containing the packet to be sent
*
* @returns  L7_SUCCESS, Packet forwarded successfully.
* @returns  L7_FAILURE, Packet forwarding failed.
*
* @comments CAUTION : 1. The given buffer is in-line modified to suit
*                        transmission.
*                     2. Both the given frame-buffer and the rtm-buffer would
*                        be freed-up by this function irrespective of the return value.
*
* @end
*
*********************************************************************/

static L7_RC_t mfcIpPktDtlTransmit(L7_uchar8 family, L7_uint32 rtrIfNum, struct rtmbuf *pBuf)
{
  L7_uchar8         *ipPkt = L7_NULLPTR;
  L7_uint32         pktLen;
  
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , rtrIfNum = %d, pBuf = %x",
            rtrIfNum, pBuf);
  MFC_DEBUG(MFC_DEBUG_TX,"\n Transmission of Ip pkt ");

  /* Basic parameter validations */
  if (pBuf == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL packet to forward");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n  invalid input parameters");
    return L7_FAILURE;
  }

  if ((family != L7_AF_INET) && (family != L7_AF_INET6))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, "Wrong family type");
    if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    }
    rtm_freem(pBuf);
    return L7_FAILURE;
  }

  /* Prepare packet to be sent */
  ipPkt = rtmtod(pBuf, L7_uchar8 *);
  pktLen = pBuf->rtm_len;

  /* after transmission, free up both pktBuffer and netMbuf irrespective of 
     success or failure. The call mfcIpPktDtlTransmitIntf constructs another 
     netBuf for packet transmission  which will anyway be freed by the dtl send call*/
  if (mfcIpPktDtlTransmitIntf(family,rtrIfNum , ipPkt, pktLen) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"failed to tranmit packet over oif = %d ", rtrIfNum); 
    MFC_DEBUG(MFC_DEBUG_TX,
              "\n failed to tranmit packet through DTL over oif = %d",
              rtrIfNum);
    if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    }
    rtm_freem(pBuf);
    return L7_FAILURE;

  }
  if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
  {
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
  }
  rtm_freem(pBuf);
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Transmit the given IPv4 packet on a specific outgoing interface
*
* @param    rtrIfNum  @b{(input)}router interface number of the outgoing interface
* @param    pBuf      @b{(input)}buffer containing the packet to be sent
*
* @returns  L7_SUCCESS, Packet forwarded successfully.
* @returns  L7_FAILURE, Packet forwarding failed.
*
* @comments CAUTION : 1. The given buffer is in-line modified to suit
*                        transmission.
*                     2. Both the given frame-buffer and the rtm-buffer would
*                        be freed-up by this function irrespective of the return value.
*
* @end
*
*********************************************************************/
static L7_RC_t mfcIpPktTransmit(L7_uchar8 family, L7_uint32 rtrIfNum, struct rtmbuf *pBuf)
{
  L7_uchar8 *ipStart;
  ipStart = rtmtod(pBuf, L7_uchar8 *);
  mcastDebugPacketRxTxTrace(family, L7_FALSE, rtrIfNum, ipStart, pBuf->rtm_len);
    if ( family == L7_AF_INET)
    {
      return mfcIpPktDtlTransmit(family,rtrIfNum, pBuf);
    }
    else if ( family == L7_AF_INET6)
    {
#ifdef _L7_OS_LINUX_       
    /* for linux */
      return mfcIpPktDtlTransmit(family,rtrIfNum, pBuf);
#else
    /* for vxworks */
      return mfcIpv6PktTransmit(rtrIfNum, pBuf);
#endif
    }
   MFC_DEBUG(MFC_DEBUG_FAILURES," mfcIpPktTransmit() failed. family(%d)", family);
   return L7_FAILURE;
}
/*********************************************************************
* @purpose  Transmit the given packet on all the outgoing interfaces
*           based on the cache entry information provided
*
* @param    pBuf   @b{(input)}buffer containing the packet to be forwarded
* @param    pEntry @b{(input)}pointer to the cache entry with forwarding details
*
* @returns  L7_SUCCESS, Packet forwarded successfully or if the buffer should be freed.
* @returns  L7_FAILURE, Packet forwarding failed, may make sense to save the buffer.
*
* @comments Be it success or failure, the given buffer is preserved
*           intact. It is upto the caller to free-up the buffer as
*           appropriate.
*
* @end
*
*********************************************************************/
L7_RC_t mfcPktTransmit(struct rtmbuf *pBuf,
                       mfcEntry_t    *pEntry)
{
  L7_uint32      ipVer, i,intIfNum, ttlThreshold;
  struct ip      *pIp = L7_NULLPTR;
  struct rtmbuf  *pBufCopy = L7_NULLPTR;
  L7_ip6Header_t *pIp6 = L7_NULLPTR;  
  L7_BOOL         foundOutIf = L7_FALSE;
  L7_uint32 checkSum = 0;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , pBuf = %x, pEntry = %x", pBuf, pEntry);
  MFC_DEBUG(MFC_DEBUG_TX,"\n Transmission of pkt "); 

  /* Basic parameter validations */
  if (pBuf == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n invalid input packet buffer");
    return L7_SUCCESS; /* Invalid data. Let the caller free-up the data resources */
  }
  if (pEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info to forward packet");
    return L7_FAILURE; /* Invalid route. Let's wait for the route to be made */
  }

  /* Identify the version of IP packet we are given */
  /* Since both IPv4 & IPv6 packets have the version fields identical in their
     headers (and at the same location within), we can cast the given packet header
     to a IPv4 only for determining the version information. Subsequent processing
     must use the appropriate header structures for processing. */

  pIp = rtmtod(pBuf, struct ip *);
  pIp6 = rtmtod(pBuf, L7_ip6Header_t *);
  if ((pIp == L7_NULLPTR) || (pIp6 == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Failed to extract Ip header");
    return L7_SUCCESS;
  }
  if (pIp->ip_v == 4)
  {
    ipVer = L7_AF_INET;
  }
  else if (pIp->ip_v == 6)
  {
    ipVer = L7_AF_INET6;
  }
  else
  {
    return L7_SUCCESS;  /* Unknown version of IP or not an IP packet */
  }

  /* Decrement the TTL and Re-calculate the Checksum */
  if (ipVer == L7_AF_INET)
  {
    pIp->ip_ttl -= 1;

    pIp->ip_sum = 0;
    checkSum = inetChecksum (pIp, (pIp->ip_hl * 4));
    pIp->ip_sum = checkSum;
  }
  else if (ipVer == L7_AF_INET6)
  {
    /* TODO ... Decrement Hop-count and Checksum Calculation */
  }
  else
  {
     /* Do Nothing */
  }

  /* Process the outgoing bitmap and send the packet out as appropriate */
  for (i = 1; i < MAX_INTERFACES; i++)
  {
    if (BITX_TEST(&(pEntry->oif), i))
    {
      if (mcastIpMapRtrIntfToIntIfNum(ipVer, i, &intIfNum) != L7_SUCCESS)
      {
        MFC_DEBUG(MFC_DEBUG_FAILURES,
                  "\n Failed to convert to internal interface number for rtrIfNum : %d",
                  i);
        continue;
      }

      foundOutIf = L7_TRUE;
      ttlThreshold = 0;
      if (mcastMapIntfTtlThresholdGet(intIfNum,&ttlThreshold) != L7_SUCCESS)
      {
        MFC_DEBUG(MFC_DEBUG_TX,
                  "\n unable to obtain ttl for (internal) Interface = %d",intIfNum);
        MFC_DEBUG(MFC_DEBUG_FAILURES,
                  "\n unable to obtain ttl for (internal) Interface = %d",intIfNum);       
        ttlThreshold = 0;
      }
      if (ttlThreshold != 0)
      {
        if (ipVer == L7_AF_INET)
        {
          if (pIp->ip_ttl > ttlThreshold)
          {
            pBufCopy = rtm_dup(pBuf); /* Copy MUST be made for every outgoing
                                         interface, since mfcIpv4PktTransmit()
                                         always frees-up the given buffer */
            if (pBufCopy != L7_NULLPTR)
            {
              mfcIpPktTransmit(L7_AF_INET, i, pBufCopy);
            }
            else
            {
              MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to generate a packet buffer");
              MFC_DEBUG(MFC_DEBUG_TX,
                        "\n failed to generate copy of rtmbuf for (IPv4)");

            }
          }
        }
        else
        {
          if (pIp6->hoplim > ttlThreshold)
          {
            pBufCopy = rtm_dup(pBuf); /* Copy MUST be made for every outgoing
                                         interface, since mfcIpv6PktTransmit()
                                         always frees-up the given buffer */
            if (pBufCopy != L7_NULLPTR)
            {
              mfcIpPktTransmit(L7_AF_INET6, i, pBufCopy);
            }
            else
            {
              MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to generate a packet buffer");
              MFC_DEBUG(MFC_DEBUG_TX,
                        "\n failed to generate copy of rtmbuf (IPv6)");

            }
          }
        }
      }
    }
  }

  /* All done... return */
  if (foundOutIf != L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_TX,"\n Outgoing Ifaces not resolved for entry ");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Outgoing Ifaces not resolved for entry ");
    return L7_FAILURE;  /* Possibly route not resolved as yet */
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a received IPv4 Multicast Data packet
*
* @param    pIp   @b{(input)}pointer to the IP structure containing the 
*                            IPv4 header of the incoming packet
* @param    pIf   @b{(input)}pointer to the incoming interface structure
* @param    pBuf  @b{(input)}pointer to the routing buffer containing 
*                            the packet itself
*
* @returns  L7_FALSE Packet successfully processed.
*                    Caller may free-up the packet buffer
* @returns  L7_TRUE  Packet processing failed.
*                    Caller may want to further process it.
*
* @comments  Be it success or failure, the given buffer is preserved
*            intact. It is upto the caller to free-up the buffer as
*            appropriate.
*
* @end
*
*********************************************************************/
L7_BOOL mfcIpv4PktRecv(struct ip *pIp,
                       struct rtm_ifnet *pIf,
                       struct rtmbuf *pBuf)
{
  L7_uchar8         *pData = L7_NULLPTR;
  L7_uint32         rtrIfNum,sourceAddr, groupAddr;
  L7_inet_addr_t    ipAddr;
  sysnet_pdu_info_t sysnetPduInfo;
  mfcEntry_t        mfcEntry;
  SYSNET_PDU_RC_t   hookVerdict;
  L7_uchar8         addrBufStr1[IPV6_DISP_ADDR_LEN];
  L7_uchar8         addrBufStr2[IPV6_DISP_ADDR_LEN];
  L7_uint32 currentMcastProtocol = L7_NULL;
    
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , pIp = %x, pIf = %x", pIp, pIf, pBuf);
  MFC_DEBUG(MFC_DEBUG_RX, "\n pkt Received from ip_input ");

  /* Basic parameter validations */
  if ((pIp == L7_NULLPTR) || (pIf == L7_NULLPTR) || (pBuf == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL interface info or NULL packet buffer received");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n  invalid input parameters");
    return L7_TRUE;
  }
  /* Verify if the incoming interface is a valid router interface */
  if (mcastIpMapIntIfNumToRtrIntf(L7_AF_INET,(L7_uint32)(pIf->if_index),
                                  &rtrIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n incoming interfaces is not valid routing interface , iif = %d", 
              pIf->if_index);
    return L7_TRUE; /* Invalid interface index. Let the caller free-up the packet */
  }

  mcastDebugPacketRxTxTrace(L7_AF_INET, L7_TRUE, rtrIfNum, (L7_uchar8 *) pIp, pBuf->rtm_len);

/* Don't forward a packet with time-to-live of zero or one,
     or a packet destined to a local-only group. */
  sourceAddr = osapiNtohl(pIp->ip_src.s_addr);
  groupAddr = osapiNtohl(pIp->ip_dst.s_addr);
  if ((pIp->ip_ttl <= 1) || (groupAddr <= INADDR_MAX_LOCAL_GROUP))
  {
    /* MFC_LOG_MSG(L7_AF_INET,"pkt ttl is less than 1 or dest may be a local group ");
    MFC_DEBUG(MFC_DEBUG_RX,
              "\n pkt ttl is less than 1 or dest may be a local group ");
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n pkt ttl is less than 1 or dest may be a local group ");  */
    return L7_FALSE;
  }

  /* If multicast is not enabled, do nothing */
  if (mfcIsEnabled(L7_AF_INET) != L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n MFC is disabled");
    return L7_FALSE;
  }

  if ((mcastMapIpCurrentMcastProtocolGet (L7_AF_INET, &currentMcastProtocol)
                                       == L7_SUCCESS) &&
      currentMcastProtocol == L7_MCAST_IANA_MROUTE_UNASSIGNED)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n No MRP is configured");
    return L7_FALSE;
  }

  if (mcastMapMcastProtocolIsOperational (L7_AF_INET, currentMcastProtocol)
                                       != L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n MRP (%d) is not operational", 
            currentMcastProtocol);
    return L7_FALSE;
  }
        
  /* Verify that MAC address in the packet is correctly formatted
     with respect to the destination multicast IP address.
     If the packet is malformed, then discard it. */
  SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)pBuf->rtm_bufhandle, pData);

  if(mcastMacAddressCheck(L7_AF_INET,((L7_uchar8 *)(&(pIp->ip_dst))),pData)== L7_FAILURE)
  {
    return L7_TRUE;
  }


  /* Verify if the incoming interface is up and running */
  if (mcastIpMapRtrIntfIpAddressGet(L7_AF_INET,(L7_uint32)(pIf->if_index), 
                                    &ipAddr) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n incoming interface is not up , iif = %d", 
              pIf->if_index);
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n incoming interface is not up , iif = %d", 
              pIf->if_index);
    return L7_TRUE;
  }

  /* Give an opportunity for all the registered interceptors to process the
     received Multicast packet before we process it.

     If the call returns L7_TRUE, the frame was processed by an interceptor
     and marked as either discard or consumed. This indicates that the network
     buffer has either been freed by the interceptor, or has been copied-out
     by the interceptor.

     Note that for this intercept point, the buffer is referenced through the 
     rtmbuf which is freed here if the caller consumes it.So, the interceptor 
     should make a copy of the frame if processing is done on another task. 
     Note also that an action of 'discard' here is invalid since the buffer 
     and rtmbuf will be freed by the caller of this function. */

  memset(&sysnetPduInfo, 0, sizeof(sysnet_pdu_info_t));
  sysnetPduInfo.intIfNum = pBuf->rtm_pkthdr.rcvif->if_index;
  if (SYSNET_PDU_INTERCEPT(AF_INET,
                           SYSNET_INET_MFORWARD_IN,
                           (L7_netBufHandle)(pBuf->rtm_bufhandle),
                           &sysnetPduInfo,
                           L7_NULLPTR,
                           &hookVerdict) == L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n SYSNET_PDU_INTERCEPT failed");
    return L7_FALSE;
  }

  /* Make sure that the packet did not arrive via a Source Route Tunnel.
     We do not support Source Route tunnels anymore */
  if ((pIp->ip_hl < ((sizeof(struct ip) + MFC_IPV4_TUNNEL_LEN) >> 2)) ||
      (((L7_uchar8 *)(pIp + 1))[1] != IPOPT_LSRR))
  {
  }
  else
  {
    /* Packet is not accepted; packet will be freed by the caller */
    return L7_TRUE;
  }


  /* Try and route the packet to the destination group, if possible */
  memset(&mfcEntry, 0, sizeof(mfcEntry_t));
  if (inetAddressSet(L7_AF_INET, &sourceAddr, &(mfcEntry.source)) != L7_SUCCESS)
  {
    return L7_TRUE;
  }
  if (inetAddressSet(L7_AF_INET, &groupAddr, &(mfcEntry.group)) != L7_SUCCESS)
  {
    return L7_TRUE;
  }

  mfcEntry.iif = rtrIfNum;
  mfcEntry.mcastProtocol = L7_MRP_UNKNOWN;
  mfcEntry.dataTTL = (L7_uint32)(pIp->ip_ttl);
  mfcEntry.m   = pBuf;
  if (mfcMroutePktForward(&mfcEntry) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to forward packet of (src,grp) = (%s, %s)",
                inetAddrPrint(&mfcEntry.source,addrBufStr1),
                inetAddrPrint(&mfcEntry.group,addrBufStr2)); 
    MFC_DEBUG(MFC_DEBUG_RX,"\n Failed to forward packet ");
    return L7_TRUE;
  }
  return L7_TRUE;
}


