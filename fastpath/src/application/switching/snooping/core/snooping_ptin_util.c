/*
 * snooping_ptin_util.c
 *
 *  Created on: 2 de Ago de 2012
 *      Author: Daniel Figueira
 */

#include "snooping_ptin_util.h"
#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "snooping_ptin_grouptimer.h"
#include "snooping_ptin_sourcetimer.h"

#include "ptin_debug.h"
#include "ptin_igmp.h"

#include "comm_structs.h"
#include "l7_mgmd_api.h"
#include "arpa/inet.h"


/*********************************************************************
* Static Methods
*********************************************************************/
static L7_RC_t  snoopPTinQueryFrameV3Build(L7_uint32 vlanId, L7_uint32 groupAddr, L7_uchar8 *buffer, L7_uint32 *length, snoopOperData_t *pSnoopOperEntry, L7_uint32 *sources, L7_uint8 sourcesCnt);
static L7_RC_t  snoopPTinPacketBuild      (L7_uint32 vlanId, snoop_cb_t *pSnoopCB, L7_uint32 groupAddr, L7_uchar8 *buffer, L7_uint32 *length, L7_uchar8 *igmpFrameBuffer, L7_uint32 igmpFrameLength);
static void     snoopPTinQuerySend        (L7_uint32 arg1);
static L7_RC_t  snoopPTinQueryQueuePush   (snoopPTinQueryData_t *queryData);
static L7_RC_t  snoopPTinQueryQueuePop    (L7_uint32 index);


/*********************************************************************
 * @purpose Add a new client to the L2 tables
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinL2ClientAdd()
{
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Adding client to L2");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Remove a existing client from the L2 tables
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinL2ClientRemove()
{
  LOG_WARNING(LOG_CTX_PTIN_IGMP, "Removing client from L2");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Check if a given timer is running
 *
 * @param   timerPtr  Pointer to timer
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 *********************************************************************/
L7_BOOL snoopPTinIsTimerRunning(osapiTimerDescr_t *timerPtr)
{
  if(!timerPtr || timerPtr->timer_running == 0)
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
 * @purpose Method to get the first free position in the Query buffer
 *
 * @param   queryData Pointer to the variable that will holds the
 *                    query data to save
 *
 * @note The queuePos in queryData is filled by this method
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinQueryQueuePush(snoopPTinQueryData_t *queryData)
{
  snoop_eb_t *pSnoopEB;

  /* Argument validation */
  if (queryData == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  pSnoopEB = snoopEBGet();

  osapiSemaTake(pSnoopEB->snoopPTinQueryQSema, -1);

  /* Insert query data if the queue is not full */
  if (!pSnoopEB->snoopPTinQueryQueueFreeListFull)
  {
    L7_uint32 freeIdx, i;

    /* Get the first free index on Query Queue */
    freeIdx = pSnoopEB->snoopPTinQueryQueueFreeList[pSnoopEB->snoopPTinQueryQueueFreeListPushIdx];

    /* Save Query data */
    queryData->queuePos = pSnoopEB->snoopPTinQueryQueueFreeListPushIdx;
    pSnoopEB->snoopPTinQueryQueue[freeIdx].queuePos = pSnoopEB->snoopPTinQueryQueueFreeListPushIdx;
    pSnoopEB->snoopPTinQueryQueue[freeIdx].vlanId = queryData->vlanId;
    pSnoopEB->snoopPTinQueryQueue[freeIdx].groupAddr = queryData->groupAddr;
    pSnoopEB->snoopPTinQueryQueue[freeIdx].sFlag = queryData->sFlag;
    pSnoopEB->snoopPTinQueryQueue[freeIdx].retransmissions = queryData->retransmissions;
    for (i = 0; i < queryData->sourcesCnt; ++i)
    {
      pSnoopEB->snoopPTinQueryQueue[freeIdx].sourceList[i] = queryData->sourceList[i];
    }
    pSnoopEB->snoopPTinQueryQueue[freeIdx].sourcesCnt = queryData->sourcesCnt;

    /* Increment push index */
    ++pSnoopEB->snoopPTinQueryQueueFreeListPushIdx;
    if(pSnoopEB->snoopPTinQueryQueueFreeListPushIdx == PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE)
    {
      pSnoopEB->snoopPTinQueryQueueFreeListPushIdx = 0;
    }

    /* If the buffer was empty, unset the empty flag */
    if(pSnoopEB->snoopPTinQueryQueueFreeListEmpty)
    {
      pSnoopEB->snoopPTinQueryQueueFreeListEmpty = L7_FALSE;
    }

    /* Check if the buffer is now full */
    if(pSnoopEB->snoopPTinQueryQueueFreeListPushIdx == pSnoopEB->snoopPTinQueryQueueFreeListPopIdx)
    {
      pSnoopEB->snoopPTinQueryQueueFreeListFull = L7_TRUE;
    }

    osapiSemaGive(pSnoopEB->snoopPTinQueryQSema);
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Query Buffer is full");
    osapiSemaGive(pSnoopEB->snoopPTinQueryQSema);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Method freeing a specific position in the Query buffer
 *
 * @param   index Index of the position to free
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinQueryQueuePop(L7_uint32 index)
{
  snoop_eb_t  *pSnoopEB;

  pSnoopEB = snoopEBGet();

  osapiSemaTake(pSnoopEB->snoopPTinQueryQSema, -1);

  /* Pop the first element in queue */
  if (!pSnoopEB->snoopPTinQueryQueueFreeListEmpty)
  {
    pSnoopEB->snoopPTinQueryQueueFreeList[pSnoopEB->snoopPTinQueryQueueFreeListPopIdx] = index;

    /* Increment pop index */
    ++pSnoopEB->snoopPTinQueryQueueFreeListPopIdx;
    if (pSnoopEB->snoopPTinQueryQueueFreeListPushIdx == PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE)
    {
      pSnoopEB->snoopPTinQueryQueueFreeListPopIdx = 0;
    }

    /* If the buffer was full, unset the full flag */
    if (pSnoopEB->snoopPTinQueryQueueFreeListFull)
    {
      pSnoopEB->snoopPTinQueryQueueFreeListFull = L7_FALSE;
    }

    /* Check if the buffer is now empty */
    if (pSnoopEB->snoopPTinQueryQueueFreeListPushIdx == pSnoopEB->snoopPTinQueryQueueFreeListPopIdx)
    {
      pSnoopEB->snoopPTinQueryQueueFreeListEmpty = L7_TRUE;
    }

    osapiSemaGive(pSnoopEB->snoopPTinQueryQSema);
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Query Buffer is empty");
    osapiSemaGive(pSnoopEB->snoopPTinQueryQSema);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Method responsible for scheduling Group-Specific or
 *          Group/Source-Specific Queries
 *
 * @param   vlanId      Vlan ID
 * @param   groupAddr   IGMP Group address
 * @param   sFlag       Suppress router-side processing flag
 * @param   sources     Source list
 * @param   sourcesCnt  Number of sources in source list
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @see RFC 3376 6.6.3.1/6.6.3.2
 *
 *********************************************************************/
L7_RC_t snoopPTinQuerySchedule(L7_uint16 vlanId, L7_uint32 groupAddr, L7_BOOL sFlag, L7_uint32 *sources, L7_uint8 sourcesCnt)
{
  snoop_eb_t            *pSnoopEB;
  snoopPTinQueryData_t  queryData;
  ptin_IgmpProxyCfg_t   igmpCfg;
  L7_uint32             i;

  /* Argument validation */
  if (sources == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  pSnoopEB = snoopEBGet();

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  /* Set query data and push it to the Query Queue. QueuePos is determined by the push method */
  queryData.queryTimer = L7_NULLPTR;
  queryData.queuePos = 0;
  queryData.vlanId = vlanId;
  queryData.groupAddr = groupAddr;
  queryData.sFlag = sFlag;
  queryData.retransmissions = igmpCfg.querier.last_member_query_count;
  for(i=0; i< sourcesCnt; ++i)
  {
    queryData.sourceList[i] = sources[i];
  }
  queryData.sourcesCnt = sourcesCnt;

  if (snoopPTinQueryQueuePush(&queryData) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Query buffer is full");
    return L7_FAILURE;
  }

  /* Schedule LMQC Group-Specific Query transmissions */
//snoopPTinQuerySend((L7_uint32) &pSnoopEB->snoopPTinQueryQueue[queryData.queuePos]);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a IGMPv3 Frame for Membership Query messages
*
* @param    groupAddr       IGMP Group address
* @param    sFlag           Suppress router-side processing flag
* @param    buffer          Buffer in which the IGMPv3 Frame will be
*                           placed
* @param    length          IGMPv3 Frame length
* @param    pSnoopOperEntry Snoop Operational data
* @param    sources         Source list
* @param    sourcesCnt      Number of sources in the list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @todo     Fix Max Resp Code
*
*********************************************************************/
L7_RC_t snoopPTinQueryFrameV3Build(L7_uint32 groupAddr, L7_BOOL sFlag, L7_uchar8 *buffer, L7_uint32 *length, snoopOperData_t *pSnoopOperEntry, L7_uint32 *sources, L7_uint8 sourcesCnt)
{
  L7_uchar8         *dataPtr, *chksumPtr, byteVal, i;
  L7_ushort16       shortVal;

  /* Argument validation */
  if (buffer == L7_NULLPTR || length == L7_NULLPTR || pSnoopOperEntry == L7_NULLPTR || sources == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

  /* Type = 0x11 */
  byteVal = 0x11;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Max response code */
  byteVal = 0;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  chksumPtr = dataPtr;
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Group Address */
  SNOOP_PUT_DATA(&groupAddr, L7_IP_ADDR_LEN, dataPtr);

  /* Resv | S | QRV */
  if(sFlag)
  {
    byteVal = pSnoopOperEntry->snoopQuerierInfo.sFlagQRV | 0x08;
  }
  else
  {
    byteVal = pSnoopOperEntry->snoopQuerierInfo.sFlagQRV;
  }
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* QQIC */
  byteVal = pSnoopOperEntry->snoopQuerierInfo.qqic;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Number of Sources */
  shortVal = sourcesCnt;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  for(i=0; i<sourcesCnt; ++i)
  {
    /* Source Address */
    SNOOP_PUT_DATA(&sources[i], L7_IP_ADDR_LEN, dataPtr);
  }

  /* Determine Checksum */
  shortVal = snoopCheckSum((L7_ushort16 *) buffer, SNOOP_IGMPV3_HEADER_MIN_LENGTH + (L7_IP_ADDR_LEN * sourcesCnt), 0);
  SNOOP_PUT_SHORT(shortVal, chksumPtr);

  /* Update frame length */
  *length = SNOOP_IGMPV3_HEADER_MIN_LENGTH + (L7_IP_ADDR_LEN * sourcesCnt);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Create Mac and IP Frames and place the provided IGMP
 *          frame at the end of the new packet
 *
 * @param    vlanId           Vlan ID
 * @param    pSnoopCB         Snooping Control block
 * @param    groupAddr        IGMP Group address
 * @param    buffer           Buffer in which the entire packet will be
 *                            placed
 * @param    length           Packet length
 * @param    igmpFrameBuffer  IGMPv3 Frame
 * @param    igmpFrameLength  IGMPv3 Frame length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinPacketBuild(L7_uint32 vlanId, snoop_cb_t *pSnoopCB, L7_uint32 groupAddr, L7_uchar8 *buffer, L7_uint32 *length, L7_uchar8 *igmpFrameBuffer, L7_uint32 igmpFrameLength)
{
  L7_uchar8           *dataPtr, *macHdrStartPtr, *ipHdrStartPtr, *chksumPtr, baseMac[L7_MAC_ADDR_LEN], destMac[L7_MAC_ADDR_LEN];
  L7_inet_addr_t      querierAddr, destIp;
  static L7_ushort16  iph_ident = 1;
  L7_ushort16         shortVal;
  L7_uint32           ipv4Addr;
  L7_uchar8           byteVal;

  /* Argument validation */
  if (pSnoopCB == L7_NULLPTR || buffer == L7_NULLPTR || length == L7_NULLPTR || igmpFrameBuffer == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

  /* Build MAC Header */
  {
    macHdrStartPtr = dataPtr;

    destIp.family = L7_AF_INET;
    destIp.addr.ipv4.s_addr = (groupAddr == 0)? L7_IP_ALL_HOSTS_ADDR : groupAddr;

    /* Validate the group address being reported */
    snoopMulticastMacFromIpAddr(&destIp, destMac);

    /* Validate MAC address */
    if (snoopMacAddrCheck(destMac, L7_AF_INET) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* Get base MAC address (could be BIA or LAA) and use it as src MAC */
    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    {
      simGetSystemIPBurnedInMac(baseMac);
    }
    else
    {
      simGetSystemIPLocalAdminMac(baseMac);
    }

    /* Set source and dest MAC */
    SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);
    SNOOP_PUT_DATA(baseMac, L7_MAC_ADDR_LEN, dataPtr);

    /* OuterVlan */
    shortVal = L7_ETYPE_8021Q;
    SNOOP_PUT_SHORT(shortVal, dataPtr);
    shortVal = vlanId;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* IP Ether type */
    shortVal = L7_ETYPE_IP;
    SNOOP_PUT_SHORT(shortVal, dataPtr);
  }

  /* Build IP Header */
  {
    ipHdrStartPtr = dataPtr;

    /* IP Version */
    byteVal = (L7_IP_VERSION << 4) | (L7_IP_HDR_VER_LEN + (IGMP_IP_ROUTER_ALERT_LENGTH / 4));
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* TOS */
    byteVal = SNOOP_IP_TOS;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Payload Length */
    shortVal = L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* Identified */
    shortVal = iph_ident++;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* Fragment flags */
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* TTL */
    byteVal = SNOOP_IP_TTL;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Protocol */
    byteVal = IGMP_PROT;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Checksum = 0*/
    chksumPtr = dataPtr;
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* Source Address - Snooping Switches send it with null source address */
    if (snoopQuerierAddressReady(vlanId, pSnoopCB, &querierAddr) == L7_FALSE)
    {
      inetAddressZeroSet(L7_AF_INET, &querierAddr);
    }
    if (inetAddressGet(L7_AF_INET, &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress, &ipv4Addr) != L7_SUCCESS)
    {
      ipv4Addr = 0;
    }
    SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

    /* Destination Address */
    ipv4Addr = (groupAddr == 0)? L7_IP_ALL_HOSTS_ADDR : groupAddr;
    SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

    /* IP Options */
    byteVal = IGMP_IP_ROUTER_ALERT_TYPE;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    byteVal = IGMP_IP_ROUTER_ALERT_LENGTH;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    byteVal = 0;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Determine Checksum */
    shortVal = snoopCheckSum((L7_ushort16 *) ipHdrStartPtr, L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, chksumPtr);
  }

  /* Update frame length */
  *length = L7_ENET_HDR_SIZE + 4 + L7_ENET_ENCAPS_HDR_SIZE + L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;

  /* Verify packet size */
  if (*length > L7_MAX_FRAME_SIZE)
  {
    return L7_FAILURE;
  }

  /* Add IGMP Frame to the end of the new MAC+IP Frame */
  SNOOP_PUT_DATA(igmpFrameBuffer, igmpFrameLength, dataPtr);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Send LMQC Group or Group/Source Speficic Queries
 *
 * @param   arg1  Pointer to a snoopPTinQueryData_t structure
 *
 *********************************************************************/
void snoopPTinQuerySend(L7_uint32 arg1)
{
  L7_uchar8             igmpFrame[L7_MAX_FRAME_SIZE]={0};
  L7_uint32             igmpFrameLength=0;
  snoopOperData_t       *pSnoopOperEntry;
  L7_RC_t               rc = L7_SUCCESS;
  mgmdSnoopControlPkt_t mcastPacket;
  snoopPTinQueryData_t  *queryData;
  snoop_cb_t            *pSnoopCB;
  ptin_IgmpProxyCfg_t   igmpCfg;

  queryData = (snoopPTinQueryData_t *) arg1;

  /* Validate arguments */
  if (queryData->vlanId < PTIN_VLAN_MIN || queryData->vlanId > PTIN_VLAN_MAX)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting pSnoopCB");
    return;
  }

  /* Only allow IPv4 for now */
  if (pSnoopCB->family != L7_AF_INET)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Not IPv4 packet");
    return;
  }

  pSnoopOperEntry = snoopOperEntryGet(queryData->vlanId, pSnoopCB, L7_MATCH_EXACT);

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return;
  }

  /* Free timer */
  if(queryData->queryTimer != L7_NULLPTR)
  {
    osapiTimerFree(queryData->queryTimer);
  }
  queryData->queryTimer = L7_NULLPTR;

  /* Initialize mcastPacket structure */
  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId = queryData->vlanId;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx = (L7_uint32) -1;
  mcastPacket.msgType = IP_PROT_IGMP;
  mcastPacket.srcAddr.family = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;
  mcastPacket.destAddr.family = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;

  /* Build header frame for IGMPv3 Query with no sources */
  rc = snoopPTinQueryFrameV3Build(queryData->groupAddr, queryData->sFlag, igmpFrame, &igmpFrameLength, pSnoopOperEntry, queryData->sourceList, queryData->sourcesCnt);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Build MAC+IP frames and add the IGMP frame to the same packet */
  rc = snoopPTinPacketBuild(mcastPacket.vlanId, pSnoopCB, queryData->groupAddr, mcastPacket.payLoad, &mcastPacket.length, igmpFrame, igmpFrameLength);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Send Packet to client interfaces */
  rc = snoopPacketClientIntfsForward(&mcastPacket, L7_IGMP_MEMBERSHIP_QUERY);
  if (rc == L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Packet transmitted to client interfaces. (queue idx %d)", queryData->queuePos);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error transmitting to client interfaces");
  }
  --queryData->retransmissions;

  /* If retransmissions > 0, schedule another Group-Specific Query message. Otherwise free the position in the query buffer */
  if (queryData->retransmissions > 0)
  {
    osapiTimerAdd((void *) snoopPTinQuerySend, (L7_uint32) queryData, 0, igmpCfg.querier.last_member_query_interval * 1000, &queryData->queryTimer);
  }
  else
  {
    snoopPTinQueryQueuePop(queryData->queuePos);
  }
}

/*************************************************************************
 * @purpose Convert an IP address in integer format to string
 *
 * @param   ip      IP address to convert
 * @param   buffer  Buffer in which the converted string will be placed.
 *                  Must have at least 15 bytes.
 *
 * @return  String with IP address
 *
 *************************************************************************/
char* snoopPTinIPv4AddrPrint(L7_uint32 ip, char* buffer)
{
  L7_uchar8 bytes[4];

  if (buffer == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;
  sprintf(buffer, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);

  return buffer;
}

/*************************************************************************
 * @purpose Debug method that prints stored information for a specific
 *          multicast group
 *
 * @param   groupAddr   Multicast group address
 * @param   vlanId      Vlan Id
 *
 * @return  none
 *
 *************************************************************************/
void snoopPTinMcastgroupPrint(L7_uint32 groupAddr, L7_uint32 vlanId)
{
  char                  debug_buf[46];
  snoopPTinL3InfoData_t *snoopEntry;

  /* Search for the requested multicast group */
  if (L7_NULLPTR != (snoopEntry = snoopPTinL3EntryFind(groupAddr, vlanId, L7_MATCH_EXACT)))
  {
    L7_uint32 ifIdx;

    printf("Group: %s       Vlan ID: %u\n", snoopPTinIPv4AddrPrint(snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf), snoopEntry->snoopPTinL3InfoDataKey.vlanId);
    printf("-----------------------------------------\n");

    for (ifIdx=0; ifIdx<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; ++ifIdx)
    {
      if (snoopEntry->interfaces[ifIdx].active == L7_TRUE)
      {
        L7_uint32 sourceIdx; 

        printf("Interface: %02u |\n", ifIdx);
        printf("              |Filter-Mode:    %s\n", snoopEntry->interfaces[ifIdx].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE?"Include":"Exclude");
        printf("              |Nbr of Sources: %u\n", snoopEntry->interfaces[ifIdx].numberOfSources);
        printf("              |Group-Timer:    %u\n", snoop_ptin_grouptimer_timeleft(&snoopEntry->interfaces[ifIdx].groupTimer));
        for (sourceIdx=0; sourceIdx<PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++sourceIdx)
        {
          if (snoopEntry->interfaces[ifIdx].sources[sourceIdx].status == PTIN_SNOOP_SOURCESTATE_ACTIVE)
          {
            L7_uint32 clientIdx;

            printf("                       |Source: %s\n", snoopPTinIPv4AddrPrint(snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceAddr, debug_buf));
            printf("                                |Source-Timer:   %u\n", snoop_ptin_sourcetimer_timeleft(&snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceTimer));
            printf("                                |Nbr of Clients: %u\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].numberOfClients);
            printf("                                |Clients: ");
            for (clientIdx=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_SOURCE; clientIdx>0; --clientIdx)
            {
              printf("%02X", snoopEntry->interfaces[ifIdx].sources[sourceIdx].clients[clientIdx]);
            }
            printf("\n");
          }
        }
      }
    }
  }
  else
  {
    printf("Unknown Group %s VlanId %u\n", snoopPTinIPv4AddrPrint(groupAddr, debug_buf), vlanId);
  }
}
