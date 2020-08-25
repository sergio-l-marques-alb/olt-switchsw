/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd.c
*
* @purpose   To support IGMP (v1, v2, v3) and MLD (v1, v2)
*
* @component MGMD (IGMP+MLD)
*
* @comments  none
*
* @create    03/10/2005
*
* @author    Kamlesh Agrawal/ ddevi
* @end
*
**********************************************************************/

/**********************************************************************
                  Includes
***********************************************************************/

#include <l7_socket.h>
#include "l7_mgmdmap_include.h"
#include "l7_mgmd_api.h"
#include "mcast_defs.h"
#include "mcast_inet.h"
#include "mgmd_mrp.h"
#include "mgmd_v6.h"
#include "mgmdmap_vend_exten.h"
#include "mgmd.h"
#include "mgmd_proxy.h"

/**********************************************************************
                  Globals
***********************************************************************/

/**********************************************************************
                  Local Function Prototypes
**********************************************************************/

static L7_BOOL mgmd_src_records_check(L7_inet_addr_t *srcAddr,
                                      mgmd_group_t *mgmd_group);
static L7_BOOL mgmd_src_list_check(L7_inet_addr_t *srcAddr,
                                   L7_inet_addr_t *sourceList, L7_uint32 numSrcs);
static void mgmd_src_lists_build(mgmd_info_t *mgmd_info, mgmd_group_t *mgmd_group,
                                 L7_inet_addr_t *qSrcs[], L7_uint32 numSrcs,
                                 L7_inet_addr_t **srcsGtLmqt, L7_uint32 *numGtLmqt,
                                 L7_inet_addr_t **srcsLeLmqt, L7_uint32 *numLeLmqt);
static void mgmd_fp_encode(L7_uchar8 family,L7_int32 num, void *code);

static void mgmd_v3_is_in_process(mgmd_info_t *mgmd_info, mgmd_group_t *mgmd_group,
                                  L7_ushort16 numSrcs, L7_inet_addr_t *sourceList);

static void mgmd_v3_is_ex_process(mgmd_info_t *mgmd_info,
                                  mgmd_group_t *mgmd_group, L7_ushort16 numSrcs,
                                  L7_inet_addr_t *sourceList);

static void mgmd_v3_to_in_process(mgmd_info_t *mgmd_info,
                                  mgmd_group_t *mgmd_group, L7_ushort16 numSrcs,
                                  L7_inet_addr_t *sourceList);

static void mgmd_v3_to_ex_process(mgmd_info_t *mgmd_info,
                                  mgmd_group_t *mgmd_group, L7_ushort16 numSrcs,
                                  L7_inet_addr_t *sourceList);

static void mgmd_v3_allow_process(mgmd_info_t *mgmd_info,
                                  mgmd_group_t *mgmd_group, L7_ushort16 numSrcs,
                                  L7_inet_addr_t *sourceList);

static void mgmd_v3_block_process(mgmd_info_t *mgmd_info,
                                  mgmd_group_t *mgmd_group, L7_ushort16 numSrcs,
                                  L7_inet_addr_t *sourceList);

static L7_RC_t mgmd_source_record_destroy(L7_sll_member_t *srcRec);

static L7_int32 mgmd_source_record_comp(void *p, void *q, L7_uint32 key);

static void mgmd_v1membership_report(mgmd_info_t    *mgmd_info,
                                     L7_inet_addr_t  group,
                                     L7_inet_addr_t  reporter);

static void mgmd_group_src_specific_q_send(mgmd_info_t *mgmd_info,
                                           mgmd_group_t *mgmd_group,
                                           L7_inet_addr_t *sourceSet, L7_uint32 numSrcs);
static void mgmd_group_specific_q_send (mgmd_info_t *mgmd_info,
                                        mgmd_group_t *mgmd_group);
static L7_RC_t mgmd_group_query_send(mgmd_info_t *mgmd_info, L7_inet_addr_t group,
                                     L7_inet_addr_t dest, L7_inet_addr_t **qSrcs,
                                     L7_uint32 numSrcs, L7_int32 sflag, L7_int32 responseInterval);

static L7_RC_t mgmd_query_req_destroy (L7_sll_member_t *qreq);

static void mgmd_group_query_timeout_event_handler(void *info);

void mgmd_mrp_timeout_handler(void *param);

static void mgmd_process_v4_packet(mgmd_cb_t *mgmdCB, mcastControlPkt_t * message,
                                   L7_BOOL proxyInterface);

static void mgmd_timeout_groups_event_handler (void *mgmdCB);

static void mgmd_timeout_sources_event_handler (void* mgmdCB);

static L7_RC_t mgmd_query_send (mgmd_info_t *mgmd_info);

static void mgmd_querier_timeout_event_handler(void *mgmd_info);
static void mgmd_query_timeout_event_handler (void *mgmd_info);
static L7_RC_t mgmd_router_interface(mgmd_cb_t *mgmdCB, L7_uint32 rtrIntf, L7_uint32 mode,
                                     mgmdIntfConfig_t *mgmdIntfInfo);

static mgmd_group_t*  mgmd_group_create(mgmd_cb_t *mgmdCB,L7_inet_addr_t group,L7_uint32 rtrIfIndex);
static mgmd_source_record_t*  mgmd_source_create(mgmd_cb_t *mgmdCB,mgmd_group_t *mgmd_group,L7_inet_addr_t source);

static void mgmd_v1host_timeout_event_handler (void *mgmd_group);
static void mgmd_v2host_timeout_event_handler (void *mgmd_group);

static void mgmd_group_delete(mgmd_cb_t *mgmdCB, mgmd_group_t *mgmd_group);

static L7_RC_t mgmdUtilAppTimerLower (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 lowerTimeOut,
                      MGMD_TIMER_t timerType);
static L7_RC_t  mgmdUtilAppTimerHandleDelete(mgmd_cb_t *mgmdCB,
                                             L7_APP_TMR_HNDL_t *timer,
                                             L7_uint32 *handle);
static L7_RC_t  mgmdUtilAppTimerDelete(mgmd_cb_t *mgmdCB,
                                       L7_APP_TMR_HNDL_t *timer);

static
L7_RC_t mgmd_global_mode_set(mgmd_cb_t *mgmdCB, L7_uint32 gblAdminMode);

static L7_RC_t mgmd_intf_mode_process(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum,
                                      L7_uint32 intfType,L7_uint32  mode,
                                      mgmdIntfConfig_t *mgmdIntfInfo);
static L7_RC_t mgmd_interface_version_set(mgmd_cb_t  *mgmdCB,
                                   L7_uint32  rtrIfNum,
                                   L7_uint32  version);

/*****************************************************************
    Function Definitions
******************************************************************/
/*********************************************************************
*
* @purpose  Supresses the source timers to LMQT
*
* @param    mgmdCB               @b{ (input) } MGMD Control Block
* @param    srcRec               @b{ (input) } source record
* @param    qSrcs                @b{ (input) } List of sources to be
*                                              transmitted on grp-src-specific query
* @param    srcCount             @b{ (input) } number of sources.
* @param    lastMemQueryInterval @b{ (input) } Last member query interval
* @param    lastMemQueryCount    @b{ (input) } Last member query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes
*
* @end
*********************************************************************/
static
void mgmdSourceTimerUpdate(mgmd_cb_t *mgmdCB,
                           mgmd_source_record_t *srcRec,L7_inet_addr_t *qSrcs,
                           L7_uint32 *srcCount, L7_uint32 lastMemQueryInterval,
                           L7_uint32 lastMemQueryCount)
{
  qSrcs[*srcCount] = srcRec->sourceAddress;
  (*srcCount)++;
  srcRec->sourceInterval = lastMemQueryCount * lastMemQueryInterval;

  if ((srcRec->sourceTimer != L7_NULLPTR) &&
      (appTimerUpdate(mgmdCB->timerHandle,&srcRec->sourceTimer, mgmd_timeout_sources_event_handler,
                     (void*)srcRec->src_timer_handle, srcRec->sourceInterval,
                     "MGMD Source Timer2") != L7_SUCCESS))
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, "Src Node Timer Updation Failed for src = \n",
                    &srcRec->sourceAddress);
    return;
  }
}

/*********************************************************************
*
* @purpose To determine if the given router interface is a MGMD
*          querier
*
* @param   mgmdCB     @b{ (input) } MGMD Control Block
* @param   rtrIfNum   @b{ (input) } Router Interface Number
*
* @returns TRUE -  The given interface is an MGMD querier
*          FALSE -  The give interface is NOT an MGMD querier
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL mgmd_is_querier (mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum)
{
  mgmd_info_t *mgmd_info;

  mgmd_info =  (mgmd_info_t*)&(mgmdCB->mgmd_info[rtrIfNum]);

  if (MCAST_BITX_TEST(mgmd_info->flags.bits, IGMP_QUERIER))
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}

/*********************************************************************
*
* @purpose  Send IGMP / MLD packet to McastMap
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
* @param    dst       @b{ (input) }   Destination Address
* @param    data      @b{ (input) }   Data (Payload)
* @param    dalalen   @b{ (input) }   Data Length
* @param    rtrIntf   @b{ (input) }   Interface on which packet is to be sent out
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_send(mgmd_cb_t *mgmdCB, L7_inet_addr_t dst, L7_uchar8 *data,
                  L7_uint32 datalen, L7_uint32 rtrIntf,
                  L7_inet_addr_t *ipAddr)
{
  L7_uint32       family;
  L7_inet_addr_t  source;
  mcastSendPkt_t  sendPkt;
  L7_RC_t         retVal;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  memset (&sendPkt, 0, sizeof(mcastSendPkt_t));
  /* Input Data Verification */
  if (data == L7_NULLPTR || datalen == 0)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Input Data verification Failed \n");
    return L7_FAILURE;
  }

  family = mgmdCB->proto;

  if (mcastIpMapIpAddressGet(family, rtrIntf, &source) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:mcastIpMapIpAddressGet returned FAILURE \n");
    return L7_FAILURE;

  }
  if (ipAddr != L7_NULLPTR)
  {
    inetCopy(ipAddr, &source);
  }

  if (family == L7_AF_INET)
  {
    L7_short16      checksum;
    L7_uchar8      *checksumPtr;
    L7_uchar8       dummy;

    checksum = inetChecksum (data, datalen);
    checksumPtr = data;

    /* Checksum location is 2 bytes OFF the start of the data */
    /* To forward the checksumPtr, do a MCAST_GET_BYTE twice OR
       we can also do checksumPtr = data + 2 */
    MCAST_GET_BYTE(dummy, checksumPtr);
    MCAST_GET_BYTE(dummy, checksumPtr);
    MCAST_PUT_SHORT(checksum, checksumPtr);

    sendPkt.protoType = IPPROTO_IGMP;
  }
  else if (family == L7_AF_INET6)
  {
    sendPkt.protoType = IPPROTO_ICMPV6;
  }

  MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, "Dst =", &dst);
  sendPkt.payLoad = data;
  sendPkt.srcAddr = source;
  sendPkt.destAddr = dst;
  sendPkt.flags = 0;
  sendPkt.length = datalen;
  sendPkt.rtrIfNum = rtrIntf;
  sendPkt.ttl = MRT_IGMP_TTL;
  sendPkt.sockFd = mgmdCB->sockfd;
  sendPkt.family = family;
  sendPkt.rtrAlert = L7_TRUE;

  retVal = mcastMapPacketSend(&sendPkt);

  if (retVal == L7_SUCCESS)
  {
    mgmdCB->counters.TotalPacketsSent++;
    mgmdDebugPacketRxTxTrace(sendPkt.family, L7_FALSE, sendPkt.rtrIfNum,
                             &sendPkt.srcAddr, &sendPkt.destAddr,
                             sendPkt.payLoad, sendPkt.length);
  }
  else
  {
    MGMD_DEBUG(MGMD_DEBUG_APIS, "retVal = %d", retVal);
  }

  return retVal;
}


/*********************************************************************
* @purpose  Check to see if specified source address is in the current
*           source record list for this group.
*
* @param    srcAddr     @b{ (input) }   Source Address
* @param    mgmd_group  @b{ (input) }   MGMD Group
*
* @returns  L7_TRUE
* @return   L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_BOOL
mgmd_src_records_check(L7_inet_addr_t *srcAddr, mgmd_group_t *mgmd_group)
{
  mgmd_source_record_t srcRec;
  srcRec.sourceAddress = *srcAddr;

  if (SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&srcRec) != L7_NULL)
    return L7_TRUE;
  else
    return L7_FALSE;

}


/*********************************************************************
* @purpose  Check to see if specified source address is in the list of sources.
*
* @param    srcAddr     @b{ (input) }   Source Address
* @param    sourceList  @b{ (input) }   Source List
* @param    numSrcs     @b{ (input) }   Number of sources
*
* @returns  L7_TRUE
* @return   L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_BOOL mgmd_src_list_check(L7_inet_addr_t *srcAddr,
                                   L7_inet_addr_t *sourceList, L7_uint32 numSrcs)
{
  L7_uint32       index;
  L7_inet_addr_t *temp_src_ptr;

  for (index=0, temp_src_ptr=&sourceList[0];
      index < numSrcs;
      index++, temp_src_ptr++)
  {
    if (L7_INET_IS_ADDR_EQUAL(srcAddr, temp_src_ptr))
      return L7_TRUE;
  }
  return L7_FALSE;
}


/*********************************************************************
* @purpose  Build 2 Source list based on the given input Source List
*           1. A list of all sources with Ctime > LMQT
*           2. A list of all sources with Ctime < LMQT
*
* @param    mgmd_info     @b{ (input) }   MGMD Info
* @param    mgmd_group    @b{ (input) }   MGMD Group
* @param    qSrcs         @b{ (input) }   Source List
* @param    numSrcs       @b{ (input) }   Number of sources
* @param    srcsGtLmqt    @b{ (output) }  Source List > LMQT
* @param    numGtLmqt     @b{ (output) }  Number of sources >LMQT
* @param    srcsLeLmqt    @b{ (output) }  Source List < LMQT
* @param    numLeLmqt     @b{ (output) }  Number of sources <LMQT
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_src_lists_build(mgmd_info_t *mgmd_info, mgmd_group_t *mgmd_group,
                                 L7_inet_addr_t *qSrcs[], L7_uint32 numSrcs,
                                 L7_inet_addr_t **srcsGtLmqt, L7_uint32 *numGtLmqt,
                                 L7_inet_addr_t **srcsLeLmqt, L7_uint32 *numLeLmqt)
{
  time_t                now;
  L7_int32              index, numGt, numLe, lmqt, timeLeft;
  mgmd_source_record_t *srcRec;
  L7_uint32             lastMemQueryInterval;
  L7_uint32             lastMemQueryCount;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered, numSrcs = %d", numSrcs);


  for (index = 0; index < numSrcs; index++)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, " src = ", qSrcs[index] );
  }

  now = osapiUpTimeRaw();
  numGt = numLe = 0;

  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  lmqt= ((lastMemQueryCount * lastMemQueryInterval));

  for (index = 0; index < numSrcs; index++)
  {
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), (qSrcs[index])))
        break;
    }

    if (srcRec != L7_NULLPTR)
    {
      if ((srcRec->sourceTimer != L7_NULLPTR) &&
          (appTimerTimeLeftGet(mgmdCB->timerHandle,srcRec->sourceTimer, &timeLeft) == L7_SUCCESS) &&
          (timeLeft > lmqt))
      {
        srcsGtLmqt[numGt] = &(srcRec->sourceAddress);
        numGt++;
      }
      else
      {
        srcsLeLmqt[numLe] = &(srcRec->sourceAddress);
        numLe++;
      }
    }
  }/*End-of-For-Loop*/

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Exit, numLe = %d", numLe );
  *numGtLmqt = numGt;
  *numLeLmqt = numLe;
}



/*********************************************************************
* @purpose  This function is used exclusively for encoding the floating
*           point representation as described in RFC 3376 section 4.1.1
*           (Max Resp Code) and section 4.1.7 (Querier's * Query Interval Code).
*           An out of range parameter causes the output parm "code" to
*           be set to 0.
*
* @param    num   @b{ (input) }    Number to be encoded
* @param    code  @b{ (output) }   Coded value
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_fp_encode(L7_uchar8 family,L7_int32 num, void *code)
{
  L7_int32 exp, mant;
  L7_uchar8 *codev4;
  L7_ushort16 *codev6;

  if (family == L7_AF_INET)
  {
    codev4 = (L7_uchar8 *)code;
    if (num < 128)
    {
      *codev4 = num;
    }
    else
    {
      mant = num >> 3;
      exp = 0;
      for (;;)
      {
        if ((mant & 0xfffffff0) == 0x00000010)
          break;
        mant = mant >> 1;
        exp++;
        /* Check for out of range */
        if (exp > 7)
        {
          *codev4 = 0;
          return;
        }
      }

      mant = mant & 0x0f;
      *codev4 = (L7_uchar8)(0x80 | (exp<<4) | mant);
    }
  }
  else if (family == L7_AF_INET6)
  {
    codev6 = (L7_ushort16 *)code;
    if (num < 32768)
    {
      *codev6 = num;
    }
    else
    {
      mant = num >> 3;
      exp = 0;
      for (;;)
      {
        if ((mant & 0xfffffff0) == 0x00000010)
          break;
        mant = mant >> 1;
        exp++;
        /* Check for out of range */
        if (exp > 7)
        {
          *codev6 = 0;
          return;
        }
      }

      mant = mant & 0x0f;
      *codev6 = (L7_ushort16)(0x80 | (exp<<4) | mant);
    }
  }
}

/*********************************************************************
* @purpose  Process v3 MODE_IS_INCLUDE group record
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    mgmd_group  @b{ (input) }   MGMD Group
* @param    numSrcs     @b{ (input) }   Number of sources
* @param    sourceList  @b{ (input) }   Source List
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v3_is_in_process (mgmd_info_t    *mgmd_info,
                                   mgmd_group_t   *mgmd_group,
                                   L7_ushort16     numSrcs,
                                   L7_inet_addr_t *sourceList)
{
  L7_uint32             index;
  mgmdGroupInfo_t       group_info;
  mgmd_source_record_t *srcRec = L7_NULLPTR, temp_src_ptr;
  mgmd_host_info_t     *mgmd_host_info = mgmd_info->mgmdCB->mgmd_host_info;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32             robustnessvar;
  L7_uint32             queryInterval;
  L7_uint32             responseInterval;
  L7_uint32             proxy_status = mgmd_info->mgmdCB->mgmd_proxy_status;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;


  if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
  {
    queryInterval = mgmd_info->querierQueryInterval;
  }
  /* 6.4.1: State = Incl(A),   Report = IS_IN(B), New State = Incl(A+B) */
  /* 6.4.1: State = Excl(X,Y), Report = IS_IN(A), New State = Excl(X+A,Y-A) */
  group_info.numSrcs = 0;
  for (index = 0; index < numSrcs; index++)
  {
    temp_src_ptr.sourceAddress = sourceList[index];
    srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);

    /* If this source is not in the list, add it and notify cache */
    if (srcRec == L7_NULLPTR)
    {
      if ((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
        return;
      }
      /* Copy event sources to be Added in INCLUDE list if group in INCLUDE  or EXCLUDE mode. */
      mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
    }
    else
    {  /* This source is in the list. */
      /* Copy event sources to be added in INCLUDE list if group is in EXCLUDE mode. */
      L7_uint32 timeLeft = 0;
      if (srcRec->sourceTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
        }
      }
      if ((mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) &&
          (timeLeft == 0))
      {
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      }

    }
    /* 6.4.1: (A)=GMI */
    srcRec->sourceInterval = ((robustnessvar * queryInterval) + (responseInterval));
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)srcRec->src_timer_handle,
                            &srcRec->sourceTimer,srcRec->sourceInterval,
                            L7_MGMD_SRC_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Start the Source timer \n");
      return;
    }
  }/*End-of-For-Loop*/

  if (proxy_status == L7_ENABLE)
  {
    /* Proxy is initialized */
    mgmd_host_info->router_report_event(mgmd_info->mgmdCB,
                                        L7_IGMP_MODE_IS_INCLUDE, mgmd_info->ifIndex,
                                        mgmd_group->group, numSrcs, sourceList);
  }
  /* Send Membership Event to Registerted MRP protocols.*/
  else if ((mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
      ((mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) && (group_info.numSrcs)))
  {
    /* NOTE: For GROUP_ADD event if (eventNumSrcs==0) then it is Group_Add
     * event else it is a Src_Add event. */
    mgmd_frame_event_info_and_notify (mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      mgmd_info->ifIndex, mgmd_group, &group_info);
  }
}


/*********************************************************************
* @purpose  Process v3 MODE_IS_EXCLUDE group record
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    mgmd_group  @b{ (input) }   MGMD Group
* @param    numSrcs     @b{ (input) }   Number of sources
* @param    sourceList  @b{ (input) }   Source List
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v3_is_ex_process(mgmd_info_t *mgmd_info,
                                  mgmd_group_t *mgmd_group, L7_ushort16 numSrcs,
                                  L7_inet_addr_t *sourceList)
{
  L7_uint32             index;
  mgmd_source_record_t *srcRec, temp_src_ptr;
  mgmd_source_record_t *prev = L7_NULL;
  mgmdGroupInfo_t       group_info;
  mgmd_host_info_t     *mgmd_host_info = mgmd_info->mgmdCB->mgmd_host_info;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32             robustnessvar;
  L7_uint32             queryInterval;
  L7_uint32             responseInterval;
  L7_uint32             proxy_status = mgmd_info->mgmdCB->mgmd_proxy_status;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;


  if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
  {
    queryInterval = mgmd_info->querierQueryInterval;
  }

  /* Initialize */
  group_info.numSrcs = 0;
  /* 6.4.1: Group Timer=GMI */
  mgmd_group->interval = ((robustnessvar * queryInterval) + (responseInterval));

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Grp timer = %p", mgmd_group->groupTimer);
  if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                          &mgmd_group->groupTimer,  mgmd_group->interval,
                          L7_MGMD_GRP_TIMER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Grp Timer\n");
    return;
  }

  if (mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)
  {
    /* 6.4.1: State = Excl(X,Y), Report = IS_EX(A), New State = Excl(A-Y,Y*A) */

    /* First, add to or update current source record list */
    for (index = 0; index < numSrcs; index++)
    {
      temp_src_ptr.sourceAddress = sourceList[index];
      srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);

      /* If this source is not in the list, add it and notify cache */
      if (srcRec == L7_NULLPTR)
      {
        if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
          return;
        }
        srcRec->sourceInterval = ((robustnessvar * queryInterval) + (responseInterval));
        if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)srcRec->src_timer_handle,
                            &srcRec->sourceTimer,srcRec->sourceInterval,
                            L7_MGMD_SRC_TIMER) != L7_SUCCESS)
        {
          MGMD_FREE(mgmdCB->proto,(srcRec));
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not Start the Source timer \n");
          return;
        }
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      }
    }/*End-of-For-Loop*/

    /* Now scrub source record list... delete those sources that were not in the TO_EX
     * source list from source record list.
     */
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      if (mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_FALSE)
      {
        /* 6.4.1: Delete (X-A), Delete (Y-A) */
        L7_uchar8  sourceMode;
        L7_uint32  timeLeft = 0;

        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
          {
            MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
          }
        }
        sourceMode = (timeLeft > 0) ? MGMD_FILTER_MODE_INCLUDE : MGMD_FILTER_MODE_EXCLUDE;

        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, sourceMode, MGMD_SOURCE_DELETE);
        if (SLLDelete(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not delete the Source node from the source List\n");
          return;
        }

        continue;
      }
      prev = srcRec;
    }/*End-of-for*/

  }
  else
  { /* MGMD_FILTER_MODE_INCLUDE */
    /* 6.4.1: State = Incl(A), Report = IS_EX(B), New State = Excl(A*B,B-A) */

    for (index = 0; index < numSrcs; index++)
    {
      temp_src_ptr.sourceAddress = sourceList[index];
      srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);

      /* If this source is not in the list, add it and notify cache */
      if (srcRec == L7_NULLPTR)
      {
        if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
          return;
        }
        srcRec->sourceInterval = ((robustnessvar * queryInterval) + (responseInterval));
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_EXCLUDE, MGMD_SOURCE_ADD);
      }
    }/*End-of-For-Loop*/

    /* Now scrub source record list... delete those sources that were not in the IN_EX
     * source list from source record list. */
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      if (mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_FALSE)
      {
        /* 6.4.1: Delete (A-B) */
        L7_uchar8  sourceMode;
        L7_uint32  timeLeft = 0;

        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
          {
            MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
          }
        }
        sourceMode = (timeLeft > 0) ? MGMD_FILTER_MODE_INCLUDE : MGMD_FILTER_MODE_EXCLUDE;

        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, sourceMode, MGMD_SOURCE_DELETE);
        if (SLLDelete(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not delete the Source node from the source List\n");
          return;
        }

        continue;
      }
      prev = srcRec;
    }/*End-of-for*/

    /* Change the Group Filter-Mode*/
    mgmd_group->filterMode = MGMD_FILTER_MODE_EXCLUDE;
    mgmd_group->mrpUpdated |= MGMD_GROUP_MODE_UPDATED;
  }


  if (proxy_status == L7_ENABLE)
  {
    /* Proxy is initialized */
    mgmd_host_info->router_report_event(mgmd_info->mgmdCB,
                                        L7_IGMP_MODE_IS_EXCLUDE, mgmd_info->ifIndex,
                                        mgmd_group->group, numSrcs, sourceList);
  }
  else
  {
    /* Send Membership Event to Registerted MRP protocols.*/
    mgmd_frame_event_info_and_notify (mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      mgmd_info->ifIndex, mgmd_group, &group_info);

  }
}


/*********************************************************************
* @purpose  Process v3 CHANGE_TO_INCLUDE_MODE group record
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    mgmd_group  @b{ (input) }   MGMD Group
* @param    numSrcs     @b{ (input) }   Number of sources
* @param    sourceList  @b{ (input) }   Source List
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v3_to_in_process (mgmd_info_t    *mgmd_info,
                                   mgmd_group_t   *mgmd_group,
                                   L7_ushort16     numSrcs,
                                   L7_inet_addr_t *sourceList)
{
  L7_uint32             index;
  mgmd_source_record_t *srcRec, temp_src_ptr;
  L7_inet_addr_t       *qSrcs;
  L7_uint32             srcCount;
  mgmdGroupInfo_t       group_info;
  mgmd_host_info_t     *mgmd_host_info = mgmd_info->mgmdCB->mgmd_host_info;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32             robustnessvar, queryInterval, responseInterval;
  L7_uint32             proxy_status = mgmd_info->mgmdCB->mgmd_proxy_status;
  L7_uint32             lastMemQueryInterval, lastMemQueryCount;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;
  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
  {
    queryInterval = mgmd_info->querierQueryInterval;
  }

  qSrcs = mgmdCB->querySrcList;
  for (index=0; index < MGMD_MAX_QUERY_SOURCES; index++)
  {
    inetAddressZeroSet (mgmdCB->proto, &qSrcs[index]);
  }

  /* Initialize local variables */
  group_info.numSrcs = 0;

  /* 7.3.2: Ignore TO_IN messages when in v1 mode */
  if (mgmd_group->groupCompatMode == L7_MGMD_VERSION_1)
    return;


  for (index = 0; index < numSrcs; index++)
  {
    temp_src_ptr.sourceAddress = sourceList[index];
    srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);

    /* If this source is not in the list, add it and notify cache */
    if (srcRec == L7_NULLPTR)
    {
      if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
        return;
      }
      mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
    }
    else
    {  /* This source is in the list. */
      /* Copy event sources to be added in INCLUDE list if group is in EXCLUDE mode. */
      L7_uint32 timeLeft = 0;
      if (srcRec->sourceTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
        }
      }
      if ((mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) &&
          (timeLeft == 0))
      {
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      }
    }

    /* 6.4.2: (A/B)=GMI */
    srcRec->sourceInterval = ((robustnessvar * queryInterval) + (responseInterval));

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL,(void*) srcRec->src_timer_handle,
                            &srcRec->sourceTimer,srcRec->sourceInterval,
                            L7_MGMD_SRC_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Start the Source timer \n");
      return;
    }

  }/*End-of-For-Loop*/

  for (index=0; index<MGMD_MAX_QUERY_SOURCES; index++)
  {
    inetAddressZeroSet (mgmdCB->proto, &qSrcs[index]);
  }

  if (mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
  {
    /* 6.4.2: Send Q(G,A-B) */
    srcCount = 0;
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      if (mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_FALSE)
      {

        mgmdSourceTimerUpdate(mgmdCB, srcRec, qSrcs, &srcCount,
                              lastMemQueryInterval, lastMemQueryCount);
      }
    }
    /* If we're the querier, send the appropriate queries */
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      mgmd_group_src_specific_q_send(mgmd_info, mgmd_group, qSrcs, srcCount);
    }
  }
  else
  { /* MGMD_FILTER_MODE_EXCLUDE */
    /* If we're the querier, send the appropriate queries */

    /* Send Q(G)*/
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      mgmd_group_specific_q_send(mgmd_info, mgmd_group);
    }
    /* Send Q(G,X-A) */
    srcCount = 0;
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      L7_uint32  timeLeft = 0;
      if (srcRec->sourceTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
        }
      }
      if ((timeLeft > 0) &&
          (mgmd_src_list_check(&(srcRec->sourceAddress),sourceList, numSrcs) == L7_FALSE))
      {

        mgmdSourceTimerUpdate(mgmdCB, srcRec, qSrcs, &srcCount,
                              lastMemQueryInterval, lastMemQueryCount);
      }
    }
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      mgmd_group_src_specific_q_send(mgmd_info, mgmd_group, qSrcs, srcCount);
    }
  }

  if (proxy_status == L7_ENABLE)
  {
    /* Proxy is initialized */

    mgmd_host_info->router_report_event(mgmd_info->mgmdCB, L7_IGMP_CHANGE_TO_INCLUDE_MODE,
                                        mgmd_info->ifIndex,mgmd_group->group,
                                        numSrcs, sourceList);
  }
  /* Send Membership Event to Registerted MRP protocols.*/
  else if ((mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
      ((group_info.numSrcs > 0) &&
       (mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)))
  {
    /* NOTE: For GROUP_ADD event if(group_info.numSrcs==0) then it is Group_Add
     * event else it is a Src_Add event. */
    mgmd_frame_event_info_and_notify (mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      mgmd_info->ifIndex,
                                      mgmd_group, &group_info);
  }

}


/*********************************************************************
* @purpose  Process v3 CHANGE_TO_EXCLUDE_MODE group record
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    mgmd_group  @b{ (input) }   MGMD Group
* @param    numSrcs     @b{ (input) }   Number of sources
* @param    sourceList  @b{ (input) }   Source List
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v3_to_ex_process(mgmd_info_t    *mgmd_info,
                                  mgmd_group_t   *mgmd_group,
                                  L7_ushort16     numSrcs,
                                  L7_inet_addr_t *sourceList)
{
  L7_uint32             index, grpTimeLeft = 0;
  mgmd_source_record_t *srcRec, temp_src_ptr;
  mgmd_source_record_t *prev = L7_NULL;
  L7_inet_addr_t       *qSrcs;
  L7_uint32             srcCount;
  mgmd_host_info_t     *mgmd_host_info = mgmd_info->mgmdCB->mgmd_host_info;
  mgmdGroupInfo_t       group_info;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32             robustnessvar;
  L7_uint32             queryInterval;
  L7_uint32             responseInterval;
  L7_uint32             proxy_status = mgmd_info->mgmdCB->mgmd_proxy_status;
  L7_uint32             lastMemQueryInterval;
  L7_uint32             lastMemQueryCount;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;
  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  qSrcs = mgmdCB->querySrcList;

  if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
  {
    queryInterval = mgmd_info->querierQueryInterval;
  }

  /* Initialize local variables */
  group_info.numSrcs = 0;

  /* 6.4.2: Group Timer=GMI */
  mgmd_group->interval = ((robustnessvar * queryInterval) + (responseInterval));

  if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                          &mgmd_group->groupTimer, mgmd_group->interval,
                          L7_MGMD_GRP_TIMER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Grp Timer\n");
    return;
  }

  /* 7.3.2: Ignore source-lists in TO_EX messages when in v1 or v2 mode,
   * i.e. TO_EX() messages are treated as TO_EX( {} ).
   */
  if (mgmd_group->groupCompatMode != L7_MGMD_VERSION_3)
    numSrcs = 0;

  for (index=0; index<MGMD_MAX_QUERY_SOURCES; index++)
  {
    inetAddressZeroSet (mgmdCB->proto, &qSrcs[index]);
  }

  /* If source list is empty, treat this like an IGMPv1/v2 Membership Report */
  if (mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
  {
    /* 6.4.2: State = Incl(A), Report = TO_EX(B), New State = Excl(A*B,B-A) */
    /* First, add to or update current source record list */
    /* Check to see if this source is already in the list */
    for (index = 0; index < numSrcs; index++)
    {
      temp_src_ptr.sourceAddress = sourceList[index];
      srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);      /* If this source is not in the list, add it and notify cache */

      if (srcRec == L7_NULLPTR)
      {
        if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
          return;
        }

        srcRec->sourceInterval = ((robustnessvar * queryInterval) + (responseInterval));

        if (mgmdUtilAppTimerDelete(mgmdCB, &srcRec->sourceTimer) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE,"Could not Delete the sourcetimer for src=\n",
                          &srcRec->sourceAddress);
          return;
        }
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_EXCLUDE, MGMD_SOURCE_ADD);
      }
    }/*End-of-For-Loop*/

    /* Now scrub source record list... delete those sources that were not in the TO_EX
     * source list from source record list.
     */
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      if (mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_FALSE)
      {
        /* 6.4.2: Delete (A-B) */
        L7_uchar8  sourceMode;
        L7_uint32  timeLeft = 0;
        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
          {
            MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
          }
        }
        sourceMode = (timeLeft > 0) ? MGMD_FILTER_MODE_INCLUDE : MGMD_FILTER_MODE_EXCLUDE;

        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, sourceMode, MGMD_SOURCE_DELETE);
        if (SLLDelete(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not delete the Source node from the source List\n");
          return;
        }

        continue;
      }
      prev = srcRec;
    }/*End-of-for*/

    mgmd_group->filterMode = MGMD_FILTER_MODE_EXCLUDE;
    mgmd_group->mrpUpdated |= MGMD_GROUP_MODE_UPDATED;

    /* 6.4.2: Send Q(G,A*B) */
    srcCount = 0;

    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      L7_uint32 timeLeft = 0;
      if (srcRec->sourceTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
        }
      }
      if ((mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_TRUE) &&
          (timeLeft != 0))
      {

        mgmdSourceTimerUpdate(mgmdCB, srcRec, qSrcs, &srcCount,
                              lastMemQueryInterval, lastMemQueryCount);
      }
    }
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      mgmd_group_src_specific_q_send(mgmd_info, mgmd_group, qSrcs, srcCount);
    }
  }
  else
  { /* MGMD_FILTER_MODE_EXCLUDE */
    /* 6.4.2: State = Excl(X,Y), Report = TO_EX(A), New State = Excl(A-Y,Y*A) */
    grpTimeLeft = 0;
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,mgmd_group->groupTimer, &grpTimeLeft) != L7_SUCCESS)
    {
      MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for grp=",
                          &mgmd_group->group);
    }
    for (index = 0; index < numSrcs; index++)
    {
      temp_src_ptr.sourceAddress = sourceList[index];
      srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);

      /* If this source is not in the list, add it and notify cache */
      if (srcRec == L7_NULLPTR)
      {
        if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
          return;
        }
        srcRec->sourceInterval = grpTimeLeft;
        if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)srcRec->src_timer_handle,
                                &srcRec->sourceTimer,srcRec->sourceInterval,
                                L7_MGMD_SRC_TIMER) != L7_SUCCESS)
        {
          MGMD_FREE(mgmdCB->proto,(srcRec));
          MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Start the Source timer \n");
          return;
        }
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      }
    }/*End-of-For-Loop*/

    /* Now scrub source record list... delete those sources that were not in the TO_EX
     * source list from source record list.
     */
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      if (mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_FALSE)
      {
        /* 6.4.2 Delete (X-A), Delete (Y-A) */
        L7_uchar8  sourceMode;
        L7_uint32  timeLeft = 0;

        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
          {
            MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
          }
        }
        sourceMode = (timeLeft > 0) ? MGMD_FILTER_MODE_INCLUDE : MGMD_FILTER_MODE_EXCLUDE;

        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, sourceMode, MGMD_SOURCE_DELETE);
        if (SLLDelete(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not delete the Source node from the source List\n");
          return;
        }

        continue;
      }
      prev = srcRec;
    }/*End-of-for*/

    /* 6.4.2: Send Q(G,A-Y) */
    srcCount = 0;
    for (index=0; index < numSrcs; index++)
    {
      temp_src_ptr.sourceAddress = sourceList[index];
      srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);
      if (srcRec != L7_NULLPTR)
      {
        L7_uint32 timeLeft = 0;
        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
          {
            MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
          }
        }
        if (timeLeft > 0)
        {
          mgmdSourceTimerUpdate(mgmdCB, srcRec, qSrcs, &srcCount,
                                lastMemQueryInterval, lastMemQueryCount);
        }
      }
    }/*End-of-For-Loop*/
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      mgmd_group_src_specific_q_send(mgmd_info, mgmd_group, qSrcs, srcCount);
    }
  }


  mgmd_group->filterMode = MGMD_FILTER_MODE_EXCLUDE;

  if (proxy_status == L7_ENABLE)
  {
    /* Proxy is initialized    */
    mgmd_host_info->router_report_event(mgmd_info->mgmdCB, L7_IGMP_CHANGE_TO_EXCLUDE_MODE,
                                        mgmd_info->ifIndex, mgmd_group->group,
                                        numSrcs, sourceList);
  }
  else
  {
    /* Send Membership Event to Registerted MRP protocols.*/
    mgmd_frame_event_info_and_notify(mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                     mgmd_info->ifIndex, mgmd_group, &group_info);
  }
}


/*********************************************************************
* @purpose  Process v3 ALLOW_NEW_SOURCES group record
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    mgmd_group  @b{ (input) }   MGMD Group
* @param    numSrcs     @b{ (input) }   Number of sources
* @param    sourceList  @b{ (input) }   Source List
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v3_allow_process(mgmd_info_t    *mgmd_info,
                                  mgmd_group_t   *mgmd_group,
                                  L7_ushort16     numSrcs,
                                  L7_inet_addr_t *sourceList)
{
  L7_uint32             index;
  mgmd_source_record_t *srcRec = L7_NULLPTR, temp_src_ptr;
  mgmdGroupInfo_t       group_info;
  mgmd_host_info_t     *mgmd_host_info = mgmd_info->mgmdCB->mgmd_host_info;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32             robustnessvar;
  L7_uint32             queryInterval;
  L7_uint32             responseInterval;
  L7_uint32             proxy_status = mgmd_info->mgmdCB->mgmd_proxy_status;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;


  if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
  {
    queryInterval = mgmd_info->querierQueryInterval;
  }

  /* 6.4.2: State = Incl(A), Report = ALLOW(B), New State = Incl(A+B) */
  /* 6.4.2: State = Excl(X,Y), Report = ALLOW(A), New State = Excl(X+A,Y-A) */

  /* Initialize local variables */
  group_info.numSrcs = 0;

  for (index = 0; index < numSrcs; index++)
  {
    temp_src_ptr.sourceAddress = sourceList[index];
    srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&temp_src_ptr);      /* If this source is not in the list, add it and notify cache */

    /* If this source is not in the list, add it and notify cache */
    if (srcRec == L7_NULLPTR)
    {
      if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,temp_src_ptr.sourceAddress ))== L7_NULLPTR)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
        return;
      }
      mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
    }
    else
    {  /* This source is in the list. */
      /* Copy event sources to be deleted if group in EXCLUDE mode. */
      L7_uint32   timeLeft = 0;
      if (srcRec->sourceTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
        }
      }
      if ((mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) &&
          (timeLeft == 0))
      {
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      }
    }

    /* 6.4.2: (A/B)=GMI */
    srcRec->sourceInterval = ((robustnessvar * queryInterval) + (responseInterval));

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void *)srcRec->src_timer_handle,
                            &srcRec->sourceTimer,srcRec->sourceInterval,
                            L7_MGMD_SRC_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Start the Source timer \n");
      return;
    }
  }/*End-of-For-Loop*/

  if (proxy_status == L7_ENABLE)
  {
    /* Proxy is initialized */
    mgmd_host_info->router_report_event(mgmd_info->mgmdCB, L7_IGMP_ALLOW_NEW_SOURCES,
                                        mgmd_info->ifIndex,
                                        mgmd_group->group, numSrcs, sourceList);
  }
  /* Send Membership Event to Registerted MRP protocols.*/
  else if ((mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
      ((group_info.numSrcs) &&
       (mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)))
  {
    /* NOTE: For GROUP_ADD event if(group_info.numSrcs==0) then it is Group_Add
     * event else it is a Src_Add event. */
    mgmd_frame_event_info_and_notify (mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      mgmd_info->ifIndex,
                                      mgmd_group, &group_info);
  }

}


/*********************************************************************
* @purpose  Process v3 BLOCK_OLD_SOURCES group record
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    mgmd_group  @b{ (input) }   MGMD Group
* @param    numSrcs     @b{ (input) }   Number of sources
* @param    sourceList  @b{ (input) }   Source List
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v3_block_process (mgmd_info_t    *mgmd_info,
                                   mgmd_group_t   *mgmd_group,
                                   L7_ushort16     numSrcs,
                                   L7_inet_addr_t *sourceList)
{
  L7_uint32             index;
  mgmdGroupInfo_t       group_info;
  L7_inet_addr_t       *qSrcs;
  L7_uint32             srcCount, grpTimeLeft = 0;
  time_t                now;
  L7_inet_addr_t       *temp_src_ptr;
  mgmd_source_record_t *srcRec = L7_NULLPTR;
  mgmd_host_info_t     *mgmd_host_info = mgmd_info->mgmdCB->mgmd_host_info;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32             proxy_status = mgmd_info->mgmdCB->mgmd_proxy_status;
  L7_uint32             lastMemQueryInterval;
  L7_uint32             lastMemQueryCount;

  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  qSrcs = mgmdCB->querySrcList;

  /* 7.3.2: Ignore BLOCK messages when in v1 or v2 mode */
  if (mgmd_group->groupCompatMode != L7_MGMD_VERSION_3)
    return;

  for (index=0; index<MGMD_MAX_QUERY_SOURCES; index++)
  {
    inetAddressZeroSet (mgmdCB->proto, &qSrcs[index]);
  }

  group_info.numSrcs = 0;
  if (mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
  {
    /* 6.4.2: State = Incl(A), Report = BLOCK(B), New State = Incl(A) */
      /* 6.4.2: Send Q(G,A*B) */
      srcCount = 0;
      for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
          srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
      {
        if (mgmd_src_list_check(&(srcRec->sourceAddress), sourceList, numSrcs) == L7_TRUE)
        {

          mgmdSourceTimerUpdate(mgmdCB, srcRec, qSrcs, &srcCount,
                                lastMemQueryInterval, lastMemQueryCount);
        }
      }
      if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
      {

      mgmd_group_src_specific_q_send(mgmd_info, mgmd_group, qSrcs, srcCount);
    }
  }
  else
  { /* MGMD_FILTER_MODE_EXCLUDE */
    /* 6.4.2: State = Excl(X,Y), Report = BLOCK(A), New State = Excl(X+(A-Y),Y) */
    grpTimeLeft = 0;
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,mgmd_group->groupTimer, &grpTimeLeft) != L7_SUCCESS)
    {
      MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for grp=",
                          &mgmd_group->group);
    }

    now = osapiUpTimeRaw();
    for (index = 0, temp_src_ptr = &sourceList[0];
        index < numSrcs;
        index++, temp_src_ptr++)
    {
      /* If this source is not in the list, add it and notify cache */
      if (mgmd_src_records_check(temp_src_ptr, mgmd_group) == L7_FALSE)
      {
        if((srcRec =mgmd_source_create(mgmdCB,mgmd_group,*temp_src_ptr))== L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error in creating the Source node \n");
          return;
        }

        srcRec->sourceInterval = grpTimeLeft;
        if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)srcRec->src_timer_handle,
                                &srcRec->sourceTimer,srcRec->sourceInterval,
                                L7_MGMD_SRC_TIMER) != L7_SUCCESS)
        {
          MGMD_FREE(mgmdCB->proto,(srcRec));
          MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Start the Source timer \n");
          return;
        }
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      }/*End-of-Main-IF-Block*/
    }/*End-of-For-Loop*/

    /* 6.4.2: Send Q(G,A-Y) */
    srcCount = 0;
    for (index=0, temp_src_ptr=&sourceList[0];
        index<numSrcs;
        index++, temp_src_ptr++)
    {
      if (mgmd_src_records_check(temp_src_ptr,mgmd_group) == L7_TRUE)
      {
        for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
            srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
        {
          if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), temp_src_ptr))
          {
            L7_uint32 timeLeft = 0;
            if (srcRec->sourceTimer != L7_NULLPTR)
            {
              if (appTimerTimeLeftGet(mgmdCB->timerHandle, srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
              {
                MGMD_DEBUG_ADDR(MGMD_DEBUG_TIMERS," failed to time left for source = ", &srcRec->sourceAddress);
              }
            }
            if (timeLeft > 0)
            {
              mgmdSourceTimerUpdate(mgmdCB, srcRec, qSrcs, &srcCount,
                                    lastMemQueryInterval, lastMemQueryCount);
            }
          }
        }
      }
    }/*End-of-For-Loop*/
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      mgmd_group_src_specific_q_send(mgmd_info, mgmd_group, qSrcs, srcCount);
    }
  }
  if (proxy_status == L7_ENABLE)
  {
    /* Proxy is initialized    */
    mgmd_host_info->router_report_event(mgmd_info->mgmdCB,
                                        L7_IGMP_BLOCK_OLD_SOURCES,
                                        mgmd_info->ifIndex,
                                        mgmd_group->group, numSrcs, sourceList);
  }
  /* Send Membership Event to Registerted MRP protocols.*/
  else if ((group_info.numSrcs) &&
      (mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE))
  {
    /* NOTE: For GROUP_ADD event if(group_info.numSrcs==0) then it is Group_Add
     * event else it is a Src_Add event. */
    mgmd_frame_event_info_and_notify (mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      mgmd_info->ifIndex,
                                      mgmd_group, &group_info);
  }
}


/*********************************************************************
* @purpose  Deallocate mempory for source record
*           Destructor function for SLL
*
* @param    srcRec  @b{ (input) }  Source record
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t mgmd_source_record_destroy(L7_sll_member_t *srcRecParam)
{
  mgmd_source_record_t *srcRec = (mgmd_source_record_t *)srcRecParam;
  mgmd_cb_t *mgmdCB = srcRec->mgmdCB;

  MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting Src timer handle ");
  if (mgmdUtilAppTimerHandleDelete(mgmdCB, &srcRec->sourceTimer,
                                   &srcRec->src_timer_handle) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD:Error in Deleting the src_timer_handle.\n");
  }

  MGMD_FREE(mgmdCB->proto, (L7_uchar8 *)(srcRec));
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Compare source record addresses for SLL AutoSort
*
* @param    p   @b{ (input) } Source address
* @param    q   @b{ (input) } Destination Address
* @param    key @b{ (input) } Key - unused
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_int32 mgmd_source_record_comp(void *p, void *q, L7_uint32 key)
{
  return(L7_INET_ADDR_COMPARE( &(((mgmd_source_record_t *)p)->sourceAddress),
                               &(((mgmd_source_record_t *)q)->sourceAddress)));
}


/*********************************************************************
* @purpose  Process a V3 membership report
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    v3Report    @b{ (input) }   V3 report
* @param    reporter    @b{ (input) }   Reporter Address
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_v3membership_report(mgmd_info_t *mgmd_info,
                                     L7_mgmdv3Report_t *v3Report,
                                     L7_inet_addr_t reporter)
{

  mgmd_cb_t              *mgmdCB = mgmd_info->mgmdCB;
  mgmd_group_t           *mgmd_group = L7_NULLPTR, searchKey;
  L7_inet_addr_t          group;
  L7_int32                numGrpRecs, i;
  L7_mgmdv3GroupRecord_t *groupRec;
  L7_uint32               robustnessvar;
  L7_uint32               queryInterval;
  L7_uint32               responseInterval;
  L7_int32                scopeId;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;

  if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
  {
    queryInterval = mgmd_info->querierQueryInterval;
  }

  /* Convert the igmpv3Report-Header fields from Network to Host Byte Order */
  v3Report->mgmdReportNumGroupRecs=osapiNtohs(v3Report->mgmdReportNumGroupRecs);
  v3Report->mgmdReportChecksum=osapiNtohs(v3Report->mgmdReportChecksum);
  v3Report->mgmdReportReserved2=osapiNtohs(v3Report->mgmdReportReserved2);

  numGrpRecs = v3Report->mgmdReportNumGroupRecs;

  for (i = 0; i < numGrpRecs; i++)
  {
    /* Convert the Group-Record Header fields from Network to Host Byte Order */
    L7_inet_addr_t tempAddr;

    groupRec = &(v3Report->mgmdReportGroupRecs[i]);

    if (inetCopy(&tempAddr, &groupRec->mgmdGrpRecGroupAddr) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
      osapiSemaGive(mgmdCB->membership.semId);
      return;
    }
    inetAddrNtoh(&tempAddr, &(groupRec->mgmdGrpRecGroupAddr));

    groupRec->mgmdGrpRecNumSrcs = osapiNtohs(groupRec->mgmdGrpRecNumSrcs);

    do
    {
      /* checks for both v4 ( 224.0.0.0/8)  and v6 ( FF02:: / 64 )
         link local addresses */
      if (inetIsLANScopedAddress(&(groupRec->mgmdGrpRecGroupAddr)) == L7_TRUE)
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"Join received for link-local scoped grp addr = ",
                    &(groupRec->mgmdGrpRecGroupAddr));
        return;
      }

      MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"MLD report received for grp addr = ",
                  &(groupRec->mgmdGrpRecGroupAddr));
      /*    MLD messages are never sent for multicast addresses whose scope is 0
       (reserved) or 1 (node-local). */
      scopeId = inetAddrGetMulticastScopeId(&(groupRec->mgmdGrpRecGroupAddr));

      MGMD_DEBUG(MGMD_DEBUG_REPORTS,"MLD packet scope Id = %d ",scopeId);
      if ((scopeId == MGMD_ZERO) || (scopeId == MGMD_ONE))
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"MLD packet received for reserved/node-local grp addr = ",
                    &(groupRec->mgmdGrpRecGroupAddr));
        continue;
      }
      MGMD_DEBUG(MGMD_DEBUG_REPORTS, " MLD iface = %d, RecType = %d", mgmd_info->ifIndex,
                 groupRec->mgmdGrpRecType);
      /* Note: Stats are already updated in mgmdDebugPacketRxTxTrace().
        Hence remove from here
        mgmdIntfReportStatsUpdate(mgmdCB,
                                mgmd_info->ifIndex,
                                groupRec->mgmdGrpRecType,
                                MGMD_STATS_RX); */

      /* draft-holbrook-idmr-igmpv3-ssm-07.txt (2.2.2 & 3.1): Ignore MODE_IS_EXCLUDE and
       * CHANGE_TO_EXCLUDE_MODE group records in the SSM range
       */
      if (L7_INET_IN_SSM_RANGE (&groupRec->mgmdGrpRecGroupAddr))
      {
        if ((groupRec->mgmdGrpRecType == L7_IGMP_MODE_IS_EXCLUDE) ||
            (groupRec->mgmdGrpRecType == L7_IGMP_CHANGE_TO_EXCLUDE_MODE))
          break;
      }

      /* 4.2.12: Silently ignore unrecognized Group Record Types */
      if (groupRec->mgmdGrpRecType < L7_IGMP_MODE_IS_INCLUDE ||
          groupRec->mgmdGrpRecType > L7_IGMP_BLOCK_OLD_SOURCES)
        break;

      if (inetCopy(&group, &(groupRec->mgmdGrpRecGroupAddr)) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
        return;
      }

      if ( (groupRec->mgmdGrpRecNumSrcs == MGMD_ZERO) &&
           (groupRec->mgmdGrpRecType == L7_IGMP_MODE_IS_INCLUDE ||
            groupRec->mgmdGrpRecType == L7_IGMP_BLOCK_OLD_SOURCES ||
            groupRec->mgmdGrpRecType == L7_IGMP_ALLOW_NEW_SOURCES) )
      {
        MGMD_DEBUG(MGMD_DEBUG_RX," Rx'ed invalid reportof type = %d  of zero source list",
                   groupRec->mgmdGrpRecType );
        break;
      }

      searchKey.rtrIfNum = mgmd_info->ifIndex;
      if (inetCopy(&(searchKey.group), &group) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
        return;
      }
      if ((mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
      {
        /* Group record doesn't exist... don't create a new one if the group record type
         * in the report is an include/allow with an empty source list.
         */
        if (((groupRec->mgmdGrpRecNumSrcs == MGMD_ZERO) &&
            (groupRec->mgmdGrpRecType == L7_IGMP_MODE_IS_INCLUDE ||
             groupRec->mgmdGrpRecType == L7_IGMP_CHANGE_TO_INCLUDE_MODE ||
             groupRec->mgmdGrpRecType == L7_IGMP_ALLOW_NEW_SOURCES)) ||
             (groupRec->mgmdGrpRecType == L7_IGMP_BLOCK_OLD_SOURCES))
          break;

        if((mgmd_group = mgmd_group_create(mgmdCB,group,mgmd_info->ifIndex)) == L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failure in creating the Group Node \n");
          return;
        }

        mgmd_group->filterMode = MGMD_FILTER_MODE_INCLUDE; /* Initialize to INCLUDE */
        mgmd_group->mrpUpdated |= MGMD_NEW_GROUP;
        mgmd_group->v1HostPresentFlag = L7_FALSE;
        mgmd_group->v2HostPresentFlag = L7_FALSE;
        mgmd_group->groupCompatMode = L7_MGMD_VERSION_3;
        mgmd_group->interval = ((robustnessvar * queryInterval) + (responseInterval));
        mgmd_group->groupTimer = L7_NULLPTR;
      }

      if (mgmd_group->groupCompatMode < L7_MGMD_VERSION_3)
      {
        MGMD_DEBUG(MGMD_DEBUG_RX," Compatability mode is less = %d",mgmd_group->groupCompatMode);
        return;
      }
      mgmd_info->numOfJoins++;
      if (inetCopy(&(mgmd_group->reporter), &reporter) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
        return;
      }

      switch (groupRec->mgmdGrpRecType)
      {
        case L7_IGMP_MODE_IS_INCLUDE:
          mgmd_v3_is_in_process(mgmd_info, mgmd_group,groupRec->mgmdGrpRecNumSrcs,
                                groupRec->mgmdGrpRecSrcs);
          break;
        case L7_IGMP_MODE_IS_EXCLUDE:
          mgmd_v3_is_ex_process(mgmd_info, mgmd_group,groupRec->mgmdGrpRecNumSrcs,
                                groupRec->mgmdGrpRecSrcs);
          break;
        case L7_IGMP_CHANGE_TO_INCLUDE_MODE:
          mgmd_v3_to_in_process(mgmd_info, mgmd_group,groupRec->mgmdGrpRecNumSrcs,
                                groupRec->mgmdGrpRecSrcs);
          break;
        case L7_IGMP_CHANGE_TO_EXCLUDE_MODE:
          mgmd_v3_to_ex_process(mgmd_info, mgmd_group,groupRec->mgmdGrpRecNumSrcs,
                                groupRec->mgmdGrpRecSrcs);
          break;
        case L7_IGMP_ALLOW_NEW_SOURCES:
          mgmd_v3_allow_process(mgmd_info, mgmd_group,groupRec->mgmdGrpRecNumSrcs,
                                groupRec->mgmdGrpRecSrcs);
          break;
        case L7_IGMP_BLOCK_OLD_SOURCES:
          mgmd_v3_block_process(mgmd_info, mgmd_group,groupRec->mgmdGrpRecNumSrcs,
                                groupRec->mgmdGrpRecSrcs);
          break;
        default:
          /* 4.2.12: Silently ignore unrecognized GroupRecordTypes.. done above */
          break;
      }
    } while (0);
  }/*End-of-Main-FOR-LOOP*/
}


/*********************************************************************
* @purpose  Process a V2 membership report
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    group       @b{ (input) }   Group Reported
* @param    reporter    @b{ (input) }   Reporter Address
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_v2membership_report(mgmd_info_t*    mgmd_info,
                                     L7_inet_addr_t  group,
                                     L7_inet_addr_t  reporter)
{
  mgmd_cb_t    *mgmdCB = mgmd_info->mgmdCB;
  mgmd_group_t *mgmd_group, searchKey;
  L7_int32      mgmd_group_membership_interval1;
  L7_uchar8     groupStr[IPV6_DISP_ADDR_LEN];
  L7_uint32     robustnessvar;
  L7_uint32     queryInterval;
  L7_uint32     responseInterval;
  mgmd_query_req_t   *qreq = L7_NULLPTR, *qreq1;
  L7_uint32 grpMemshipTreeCount = 0;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  /* checks for both v4 ( 224.0.0.0 to 224.0.0.255)  and v6 ( FF02:: / 64 )
     link local addresses */
  if (inetIsLANScopedAddress(&group) == L7_TRUE)
  {
    MGMD_DEBUG_ADDR (MGMD_DEBUG_REPORTS,"Join received for link-local scoped grp addr = ",
                &group);
    return;
  }


  if (mgmdCB->proto == L7_AF_INET6 &&
       mgmd_ipv6_is_scope_id_valid(&group) ==L7_FALSE )
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,
      "Discarding MLD packet rx'ed for invalid scope grp addr = ",
                &group);
    return;
  }

  if (mgmdCB->proto == L7_AF_INET6)
  {
    if (inetIsMulticastReservedSiteLocalAddress (&group) == L7_TRUE)
    {
      MGMD_DEBUG_ADDR (MGMD_DEBUG_REPORTS, "Discarding MLD Packet received "
                       "with a Reserved Site Local Group Address - ", &group);
      return;
    }
  }

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;

  /* draft-holbrook-idmr-igmpv3-ssm-07.txt (3.5): Ignore V2 Reports in the SSM range */
  if (L7_INET_IN_SSM_RANGE(&group))
    return;

  mgmd_group_membership_interval1= ((robustnessvar * queryInterval) + responseInterval);

  searchKey.rtrIfNum = mgmd_info->ifIndex;
  if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
    return;
  }

  if ((mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    if ((grpMemshipTreeCount = (avlTreeCount (&mgmdCB->membership))) >= MGMD_MAX_GROUPS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD Group Membership Table is FULL - %d "
                  "Entries", grpMemshipTreeCount);
      return;
    }
    inetAddrHtop(&group, groupStr);
    MGMD_DEBUG (MGMD_DEBUG_REPORTS, "TR_TRACE: group %s on %d joined\n",
                groupStr, mgmd_info->ifIndex);

    if((mgmd_group = mgmd_group_create(mgmdCB,group,mgmd_info->ifIndex)) == L7_NULLPTR)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failure in creating the Group Node \n");
      return;
    }

    mgmd_group->v1HostPresentFlag = L7_FALSE;
    mgmd_group->v2HostPresentFlag = L7_TRUE;
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_2;
    mgmd_group->filterMode = MGMD_FILTER_MODE_EXCLUDE;
    mgmd_group->mrpUpdated |= MGMD_NEW_GROUP;

    if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
    {
      queryInterval = mgmd_info->querierQueryInterval;
    }

    if (inetCopy(&(mgmd_group->reporter), &reporter) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Copy operation Failed \n");
      return;
    }

    mgmd_group->interval = mgmd_group_membership_interval1;

    mgmd_info->numOfJoins++;

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                            &mgmd_group->groupTimer, mgmd_group->interval,
                            L7_MGMD_GRP_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Grp Timer\n");
      return;
    }

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->v2host_timer_handle,
                            &mgmd_group->v2HostTimer, mgmd_group_membership_interval1,
                            L7_MGMD_V2HOST_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the V2 host Timer\n");
      return;
    }

  }
  else
  {
    inetAddrHtop(&group, groupStr);
    MGMD_DEBUG (MGMD_DEBUG_REPORTS, "TR_TRACE: group %s on %d updated\n", groupStr, mgmd_info->ifIndex);

    mgmd_group->v2HostPresentFlag = L7_TRUE;

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->v2host_timer_handle,
                            &mgmd_group->v2HostTimer, mgmd_group_membership_interval1,
                            L7_MGMD_V2HOST_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the V2 host Timer\n");
      return;
    }
    mgmd_info->numOfJoins++;

    /* Delete all pending  queries for that group */
    qreq = (mgmd_query_req_t *)SLLFirstGet(&(mgmd_info->ll_query_reqs));
    while(qreq != L7_NULLPTR)
    {
     qreq1 = qreq;
     qreq = (mgmd_query_req_t *)SLLNextGet(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq1);
     if (L7_INET_ADDR_COMPARE(&qreq1->group, &mgmd_group->group) == 0)
     {
       MGMD_DEBUG (MGMD_DEBUG_REPORTS,"Deleting pending queries ");
       SLLDelete(&(mgmd_info->ll_query_reqs),(L7_sll_member_t *)qreq1);
     }
    }

    if (mgmd_group->grpQueryData != L7_NULLPTR)
    {
      MGMD_FREE(mgmdCB->proto,mgmd_group->grpQueryData);
      mgmd_group->grpQueryData = L7_NULLPTR;
    }

    if (mgmd_group->grpSrcQueryData != L7_NULLPTR)
    {
      MGMD_FREE(mgmdCB->proto,mgmd_group->grpSrcQueryData);
      mgmd_group->grpSrcQueryData = L7_NULLPTR;
    }

    if (mgmd_group->v1HostPresentFlag != L7_TRUE)
      mgmd_group->groupCompatMode = L7_MGMD_VERSION_2;

    if (inetCopy(&(mgmd_group->reporter), &reporter) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
      return;
    }
    mgmd_group->interval = mgmd_group_membership_interval1;

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                            &mgmd_group->groupTimer, mgmd_group->interval,
                            L7_MGMD_GRP_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Grp Timer\n");
      return;
    }
  }

  /* RFC 3376 7.3.2 - IGMPv2 Report is equivalent to IGMPv3 IS_EX( {} ) */
  mgmd_v3_is_ex_process(mgmd_info, mgmd_group, 0, L7_NULLPTR);

}


/*********************************************************************
* @purpose  Process a V1 membership report
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    group       @b{ (input) }   Group Reported
* @param    reporter    @b{ (input) }   Reporter Address
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_v1membership_report (mgmd_info_t *mgmd_info,
                                      L7_inet_addr_t group,
                                      L7_inet_addr_t reporter)
{
  mgmd_cb_t    *mgmdCB = mgmd_info->mgmdCB;
  mgmd_group_t *mgmd_group, searchKey;
  L7_int32      mgmd_group_membership_interval1;
  L7_uchar8     groupStr[IPV6_DISP_ADDR_LEN];
  L7_uint32     robustnessvar;
  L7_uint32     queryInterval;
  L7_uint32     responseInterval;
  mgmd_query_req_t   *qreq = L7_NULLPTR, *qreq1;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

   /* checks for both v4 ( 224.0.0.0/8)  and v6 ( FF02:: / 64 )
     link local addresses */
  if (inetIsLANScopedAddress(&group) == L7_TRUE)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"Join received for link-local scoped grp addr = ",
                &group);
    return;
  }

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;

  /* draft-holbrook-idmr-igmpv3-ssm-07.txt (3.5): Ignore V1 Reports in the SSM range */
  if (L7_INET_IN_SSM_RANGE(&group))
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, "Group is in SSM range, grp = ",&group);
    return;
  }

  /*refer 8.5 and 8.3 of RFC 2236 - Query Response interval is 1/10th units i.e (100uints = 10 secs) */
  mgmd_group_membership_interval1= ((robustnessvar * queryInterval) + responseInterval);

  searchKey.rtrIfNum = mgmd_info->ifIndex;
  if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
    return;
  }

  if ((mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    if((mgmd_group = mgmd_group_create(mgmdCB,group,mgmd_info->ifIndex)) == L7_NULLPTR)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failure in creating the Group Node \n");
      return;
    }

    mgmd_group->reporter = reporter;
    mgmd_group->interval = mgmd_group_membership_interval1;
    mgmd_group->filterMode = MGMD_FILTER_MODE_EXCLUDE;
    mgmd_group->mrpUpdated |= MGMD_NEW_GROUP;
    mgmd_group->v2HostPresentFlag = L7_FALSE;
    mgmd_group->v1HostPresentFlag = L7_TRUE;
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_1;

    inetAddrHtop(&group, groupStr);
    MGMD_DEBUG (MGMD_DEBUG_REPORTS, "TR_TRACE: group %s on %d joined\n",groupStr, mgmd_info->ifIndex);

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL,(void*) mgmd_group->grp_timer_handle,
                            &mgmd_group->groupTimer, mgmd_group->interval,
                            L7_MGMD_GRP_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Grp Timer\n");
      return;
    }

    if ((!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER)) &&
      (mgmd_info->querierQueryInterval != MGMD_ZERO))
    {
      queryInterval = mgmd_info->querierQueryInterval;
    }
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL,(void*) mgmd_group->v1host_timer_handle,
                            &mgmd_group->v1HostTimer, mgmd_group_membership_interval1,
                            L7_MGMD_V1HOST_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the V1 host Timer\n");
      return;
    }
  }
  else
  {
    /* Delete all pending  queries for that group */
    qreq = (mgmd_query_req_t *)SLLFirstGet(&(mgmd_info->ll_query_reqs));
    while(qreq != L7_NULLPTR)
    {
     qreq1 = qreq;
     qreq = (mgmd_query_req_t *)SLLNextGet(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq1);
     if (L7_INET_ADDR_COMPARE(&qreq1->group, &mgmd_group->group) == 0)
     {
       MGMD_DEBUG (MGMD_DEBUG_REPORTS,"Deleting pending queries ");
       SLLDelete(&(mgmd_info->ll_query_reqs),(L7_sll_member_t *)qreq1);
     }
    }
    if (mgmd_group->grpQueryData != L7_NULLPTR)
    {
      MGMD_FREE(mgmdCB->proto,mgmd_group->grpQueryData);
      mgmd_group->grpQueryData = L7_NULLPTR;
    }

    if (mgmd_group->grpSrcQueryData != L7_NULLPTR)
    {
      MGMD_FREE(mgmdCB->proto,mgmd_group->grpSrcQueryData);
      mgmd_group->grpSrcQueryData = L7_NULLPTR;
    }

    inetAddrHtop(&group, groupStr);
    MGMD_DEBUG (MGMD_DEBUG_REPORTS, "MGMD:TR_TRACE: group %s on %d updated\n",
                groupStr, mgmd_info->ifIndex);
    mgmd_group->v1HostPresentFlag = L7_TRUE;

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->v1host_timer_handle,
                            &mgmd_group->v1HostTimer, mgmd_group_membership_interval1,
                            L7_MGMD_V1HOST_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the V1 host Timer\n");
      return;
    }

    mgmd_group->interval = mgmd_group_membership_interval1;

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL,(void*)mgmd_group->grp_timer_handle,
                            &mgmd_group->groupTimer, mgmd_group->interval,
                            L7_MGMD_GRP_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Grp Timer\n");
      return;
    }

    mgmd_info->numOfJoins++;
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_1;
  }

  /* RFC 3376 7.3.2 - IGMPv1 Report is equivalent to IGMPv3 IS_EX( {} ) */
  mgmd_v3_is_ex_process(mgmd_info, mgmd_group, 0, L7_NULLPTR);
}


/*********************************************************************
* @purpose  Send a group-and-source-specific query - used by IGMPv3
*
* @param    mgmd_info    @b{ (input) }  MGMD Info
* @param    mgmd group   @b{ (input) }  MGMD Group
* @param    sourceSet    @b{ (input) }  List of sources
* @param    numSrcs      @b{ (input) }  Number of sources
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_group_src_specific_q_send (mgmd_info_t *mgmd_info,
                                            mgmd_group_t *mgmd_group,
                                            L7_inet_addr_t *sourceSet,
                                            L7_uint32 numSrcs)
{
  L7_int32              index, count;
  L7_inet_addr_t       *srcsGtLmqt[MGMD_MAX_QUERY_SOURCES];
  L7_inet_addr_t       *srcsLeLmqt[MGMD_MAX_QUERY_SOURCES];
  L7_uint32             numGtLmqt, numLeLmqt,i;
  L7_uint32             lastMemQueryInterval;
  L7_uint32             lastMemQueryCount;
  L7_inet_addr_t       *querySrcs[MGMD_MAX_QUERY_SOURCES];
  mgmd_cb_t    *mgmdCB = mgmd_info->mgmdCB;
  mgmd_query_req_t      qreqSrch;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Given Srcs");

  for (index = 0; index < numSrcs; index++)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, " src = ", &sourceSet[index] );
  }
  MGMD_DEBUG (MGMD_DEBUG_APIS, " -------- ");

  /* If there are no sources to query, just return */
  if (numSrcs == 0)
  {
    return;
  }
  else
  {
    for (i=0 ; i < numSrcs; i++)
    {
      querySrcs[i] = &sourceSet[i];
    }
  }

  count = lastMemQueryCount;

  /* Find if the query is already scheduled with group and same set of sources */
  memset(&qreqSrch, 0, sizeof(mgmd_query_req_t));
  inetCopy(&qreqSrch.group, &mgmd_group->group);
  qreqSrch.numSrcs = numSrcs;
  for (index = 0; index < numSrcs; index++)
  {
    qreqSrch.qSrcs[index] = &sourceSet[index];
    MGMD_DEBUG (MGMD_DEBUG_FAILURE," new Src addr = %p", qreqSrch.qSrcs[index]);
  }
  if ((SLLFind(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)&qreqSrch)) != L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_QUERY, "The Src-specific query is already scheduled for grp = ",
               &mgmd_group->group);
    MGMD_DEBUG(MGMD_DEBUG_QUERY, " and numSrcs = %d", numSrcs);
    return;
  }


  /* Group-and-source-specific query - 6.6.3.2 */
  numGtLmqt = numLeLmqt = 0;
  mgmd_src_lists_build(mgmd_info, mgmd_group, querySrcs, numSrcs,
                       srcsGtLmqt, &numGtLmqt, srcsLeLmqt, &numLeLmqt);

  if (numGtLmqt != 0)
  {
    if (mgmd_group_query_send(mgmd_info,mgmd_group->group, mgmd_group->group,srcsGtLmqt,numGtLmqt,1,lastMemQueryInterval) != L7_SUCCESS)
      return;
  }
  if (numLeLmqt != 0)
  {
    if (mgmd_group_query_send(mgmd_info,mgmd_group->group, mgmd_group->group,srcsLeLmqt,numLeLmqt,0,lastMemQueryInterval) != L7_SUCCESS)
      return;
  }

  if (--count > 0)
  {
    mgmd_query_req_t *qreq = L7_NULLPTR;
    mgmd_timer_event_info_t *info = L7_NULLPTR;
    L7_inet_addr_t   *srcAddrBuff = L7_NULLPTR;

    if ((qreq =  (mgmd_query_req_t *)MGMD_ALLOC(mgmdCB->proto, sizeof(mgmd_query_req_t))) == L7_NULLPTR)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error getting memory for Query Node\n");
      return;
    }

    memset(qreq, 0, sizeof(mgmd_query_req_t));

    if ((info = (mgmd_timer_event_info_t *)MGMD_ALLOC(mgmdCB->proto, sizeof(mgmd_timer_event_info_t))) == L7_NULLPTR)
    {
      MGMD_FREE(mgmdCB->proto,(L7_uchar8 *)(qreq));
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error getting memory for Timer Node\n");
      return;
    }
    mgmd_group->grpSrcQueryData = info;

    memset(info, 0, sizeof(mgmd_timer_event_info_t));

    info->proto = mgmd_info->mgmdCB->proto;
    info->intfNum = mgmd_info->ifIndex;
    info->qreq = qreq;

    /* group prefix has to be preserved until all queries are sent */
    qreq->group = mgmd_group->group;
    qreq->mgmdCB = mgmdCB;
    qreq->count = count;
    qreq->numSrcs = 0;
    for (index = 0; index < numSrcs; index++)
    {
      if ((srcAddrBuff = (L7_inet_addr_t *)MGMD_ALLOC(mgmdCB->proto, sizeof(L7_inet_addr_t))) == L7_NULLPTR)
      {
        MGMD_FREE(mgmdCB->proto, qreq);
        MGMD_FREE(mgmdCB->proto,(info));
        mgmd_group->grpSrcQueryData =  L7_NULLPTR;
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error getting memory for query source Node\n");
        return;
      }
      qreq->qSrcs[index] = srcAddrBuff;
      inetCopy(qreq->qSrcs[index], &sourceSet[index]);
      qreq->numSrcs++;
    }
    if (SLLAdd (&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq) != L7_SUCCESS)
    {
      for (index = 0; index < numSrcs; index++)
      {
        srcAddrBuff = qreq->qSrcs[index];
        MGMD_FREE(mgmdCB->proto,(srcAddrBuff));
      }
      MGMD_FREE(mgmdCB->proto,(qreq));
      MGMD_FREE(mgmdCB->proto,(info));
      mgmd_group->grpSrcQueryData =  L7_NULLPTR;
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not add the Query Node  to Query List\n");
      return;
    }
    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing grp-src-query handle ");
    if ((qreq->query_timer_handle = handleListNodeStore(mgmdCB->handle_list,info)) == 0)
    {
      for (index = 0; index < numSrcs; index++)
      {
        srcAddrBuff = qreq->qSrcs[index];
        MGMD_FREE(mgmdCB->proto,(srcAddrBuff));
      }
      MGMD_FREE(mgmdCB->proto,(info));
      SLLNodeDelete(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq);
      mgmd_group->grpSrcQueryData =  L7_NULLPTR;
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the handle node to store the timer data.\n");
      return;
    }

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)qreq->query_timer_handle,
                            &qreq->timer, lastMemQueryInterval,
                            L7_MGMD_GRP_QUERY_TIMER) != L7_SUCCESS)
    {
      for (index = 0; index < numSrcs; index++)
      {
        srcAddrBuff = qreq->qSrcs[index];
        MGMD_FREE(mgmdCB->proto,(srcAddrBuff));
      }
      MGMD_FREE(mgmdCB->proto,(info));
      SLLNodeDelete(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq);
      mgmd_group->grpSrcQueryData =  L7_NULLPTR;
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Grp Timer\n");
      return;
    }
  }
}

/*********************************************************************
* @purpose  Prepare to send a group-specific query
*
* @param    mgmd_info    @b{ (input) }  MGMD Info
* @param    mgmd group   @b{ (input) }  MGMD Group
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_group_specific_q_send(mgmd_info_t  *mgmd_info,
                                       mgmd_group_t *mgmd_group)
{
  L7_int32     count, lmqt, sflag, timeLeft = 0;
  L7_uint32             lastMemQueryInterval;
  L7_uint32             lastMemQueryCount;
  L7_uchar8             family = mgmd_info->mgmdCB->proto;
  mgmd_query_req_t      qreqSrch;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  count = lastMemQueryCount;
  lmqt = (count * lastMemQueryInterval) ;

  timeLeft = 0;
  if (appTimerTimeLeftGet(mgmdCB->timerHandle,mgmd_group->groupTimer, &timeLeft) != L7_SUCCESS)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for grp=",
                        &mgmd_group->group);
  }

  if (timeLeft > lmqt)
    sflag = 1;
  else
    sflag = 0;

  memset(&qreqSrch, 0 , sizeof(mgmd_query_req_t));
  inetCopy (&qreqSrch.group, &mgmd_group->group);
  qreqSrch.numSrcs = 0;
  if (SLLFind(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)&qreqSrch) != L7_NULLPTR)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, "The grp-query has previoulsly scheduled for grp=",
                    &mgmd_group->group);
    return;
  }

  /* Set the group timer to LMQT and allow it to timeout.  If a report is received for
   * this group, the group timer will be set as appropriate.
   */
  mgmd_group->interval = lmqt;

  if (mgmd_group_query_send(mgmd_info, mgmd_group->group, mgmd_group->group, L7_NULLPTR, 0, sflag,lastMemQueryInterval) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, " MGMD : failed to send grp-specific query out ");
    return;
  }

  if (--count > 0)
  {
    mgmd_query_req_t *qreq = L7_NULLPTR;
    mgmd_timer_event_info_t *info = L7_NULLPTR;

    if ((qreq = (mgmd_query_req_t *)MGMD_ALLOC(family, sizeof(mgmd_query_req_t))) == L7_NULLPTR)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error getting memory for Query Node\n");
      return;
    }

    memset(qreq, 0, sizeof(mgmd_query_req_t));

    if ((info =  (mgmd_timer_event_info_t *)MGMD_ALLOC(family, sizeof(mgmd_timer_event_info_t))) == L7_NULLPTR)
    {
      MGMD_FREE(family,(qreq));
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error getting memory for Timer Node\n");
      return;
    }
    mgmd_group->grpQueryData = info;

    memset(info, 0, sizeof(mgmd_timer_event_info_t));

    info->proto = mgmd_info->mgmdCB->proto;
    info->intfNum = mgmd_info->ifIndex;
    info->qreq = qreq;

    /* group prefix has to be preserved until all queries are sent */
    qreq->group = mgmd_group->group;
    qreq->mgmdCB = mgmd_info->mgmdCB;

    qreq->count = count;
    qreq->numSrcs = 0;

    MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, " qreq->group = ",&qreq->group);
    MGMD_DEBUG (MGMD_DEBUG_APIS, "cnt= %d ",qreq->count);
    MGMD_DEBUG (MGMD_DEBUG_APIS," CB= %d, ",  qreq->mgmdCB);
    MGMD_DEBUG (MGMD_DEBUG_APIS," numSrcs = %d ",qreq->numSrcs);
    MGMD_DEBUG (MGMD_DEBUG_APIS, "qSrcs = %d ",qreq->qSrcs);
    MGMD_DEBUG (MGMD_DEBUG_APIS, "handle = %d",qreq->query_timer_handle);
    MGMD_DEBUG (MGMD_DEBUG_APIS, " tmr = %d", qreq->timer);

    if (SLLAdd (&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq) != L7_SUCCESS)
    {
      MGMD_FREE(family,(qreq));
      MGMD_FREE(family,(info));
      mgmd_group->grpQueryData =  L7_NULLPTR;
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not add the Query Node  to Query List\n");
      return;
    }

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing grp-query handle ");
    if ((qreq->query_timer_handle = handleListNodeStore(mgmd_info->mgmdCB->handle_list,info)) == 0)
    {
      MGMD_FREE(family,(info));
      SLLNodeDelete(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq);
      mgmd_group->grpQueryData =  L7_NULLPTR;
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the handle node to store the timer data.\n");
      return;
    }

    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)qreq->query_timer_handle,
                            &qreq->timer, lastMemQueryInterval,
                            L7_MGMD_GRP_QUERY_TIMER) != L7_SUCCESS)
    {
      MGMD_FREE(family,(info));
      SLLNodeDelete(&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq);
      mgmd_group->grpQueryData =  L7_NULLPTR;
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Grp Timer\n");
      return;
    }
  }

  if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                          &mgmd_group->groupTimer, mgmd_group->interval,
                          L7_MGMD_GRP_TIMER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Grp Timer\n");
    return;
  }
}

/*********************************************************************
* @purpose  Send MGMD Query
*
* @param    mgmd_info    @b{ (input) }  MGMD Info
* @param    group        @b{ (input) }  Group Address
* @param    dest         @b{ (input) }  Dest Address
* @param    qSrcs        @b{ (input) }  List of sources
* @param    numSrcs      @b{ (input) }  Number of sources
* @param    sflag        @b{ (input) }  S Flag
* @param    responseTime @b{ (input) }  Response Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmd_group_query_send (mgmd_info_t *mgmd_info, L7_inet_addr_t group, L7_inet_addr_t dest,
                               L7_inet_addr_t **qSrcs, L7_uint32 numSrcs, L7_int32 sflag, L7_int32 responseTime)
{
  L7_uint32             responseInterval;
  L7_uint32             mgmdVersion;
  L7_uint32             robustnessvar;
  L7_uint32             queryInterval;
  L7_uchar8             queryType;
  L7_uint32             family;
  L7_uchar8            *dataStart = mgmd_info->mgmdCB->pktTxBuf;
  L7_uchar8            *data = dataStart;
  L7_ushort16           checksum = 0;
  L7_uint32             dataLen;

  family = mgmd_info->mgmdCB->proto;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;

  mgmdVersion = mgmd_info->intfConfig.version;

  memset(dataStart, 0, sizeof(MGMD_PKT_SIZE_MAX));
  queryType = (family == L7_AF_INET) ? IGMP_MEMBERSHIP_QUERY
              : MLD_LISTENER_QUERY;

  if (responseTime == 0)
  {
    mgmdMapProtocolInterfaceQueryMaxResponseTimeMilliSecsGet(family,
                                                            mgmd_info->ifIndex, &responseInterval);
  }
  else
  {
    mgmdMapProtocolInterfaceLastMembQueryIntervalMilliSecsGet(family,
                                                              mgmd_info->ifIndex, &responseInterval);
  }

  MGMD_DEBUG (MGMD_DEBUG_APIS," Family = %d ",family);
  MGMD_DEBUG (MGMD_DEBUG_APIS," version = %d ",mgmdVersion);
  MGMD_DEBUG (MGMD_DEBUG_APIS," responseInterval  = %d ",responseInterval);

  if (mgmdVersion == L7_MGMD_VERSION_1)
  {
    responseInterval= 0;
  }


  MCAST_PUT_BYTE(queryType, data);

  if (family == L7_AF_INET)
  {
    L7_uchar8             maxRespCodev4;
    /* In IGMP , maxresponse interval is in 1/10 of second */
    maxRespCodev4 =  responseInterval;
    if (mgmdVersion == L7_MGMD_VERSION_3)
    {
      mgmd_fp_encode(family, responseInterval, &maxRespCodev4);
    }
    MCAST_PUT_BYTE(maxRespCodev4, data);
    MCAST_PUT_SHORT(checksum, data);
  }
  else if (family == L7_AF_INET6)
  {
    L7_ushort16           maxRespCodev6;
    /* In MLD v1 , maxresponse interval is  in milliseconds */
    maxRespCodev6 =  responseInterval;
    if (mgmdVersion == L7_MGMD_VERSION_3)
    {
      mgmd_fp_encode(family, responseInterval, &maxRespCodev6);
    }
    MCAST_PUT_BYTE(0, data);
    MCAST_PUT_SHORT(checksum, data);
    MCAST_PUT_SHORT(maxRespCodev6, data);
    MCAST_PUT_SHORT(0,data);
  }
  MCAST_PUT_ADDR_INET(family, &group, data);

  MGMD_DEBUG(MGMD_DEBUG_APIS, "mgmdVer =", mgmdVersion);

  if (mgmdVersion != L7_MGMD_VERSION_3)
  {
    dataLen = (family == L7_AF_INET) ? IGMP_PKT_MIN_LENGTH
              : MLD_PKT_MIN_LENGTH;

    if (mgmd_send (mgmd_info->mgmdCB, dest,
                   dataStart, dataLen, mgmd_info->ifIndex,
                   L7_NULLPTR) ==L7_SUCCESS)
    {
      mgmd_info->mgmdCB->counters.QueriesSent++;
      return L7_SUCCESS;
    }
    else
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, " Failure in sending the packet out \n");
      return L7_FAILURE;
    }
  }
  else
  {
    L7_uchar8         mgmdResvSflagQRV = 0;
    L7_uchar8         mgmdQQIC = 0;
    L7_uint32         index = 0;

    /* Fill in IGMP V3 / MLD V2 query header */
    if (robustnessvar <= 0x07) /* RFC 3376 4.1.6 */
      mgmdResvSflagQRV = (L7_uchar8)(robustnessvar & 0x07);

    if (sflag)
      mgmdResvSflagQRV |= 0x08;

    /*QQIC is 1-byte only, hence hardcode the family value*/
    mgmd_fp_encode(L7_AF_INET, queryInterval, &(mgmdQQIC));

    /* Check for overflow */
    if (numSrcs > MGMD_MAX_QUERY_SOURCES)
      numSrcs = MGMD_MAX_QUERY_SOURCES;

    MCAST_PUT_BYTE(mgmdResvSflagQRV, data);
    MCAST_PUT_BYTE(mgmdQQIC, data);
    MCAST_PUT_SHORT(numSrcs, data);

    for (index = 0; index < numSrcs; index++)
    {
      L7_inet_addr_t  srcAddr;

      inetCopy(&srcAddr, qSrcs[index]);
      MCAST_PUT_ADDR_INET(family, &srcAddr, data);
    }

    if (family == L7_AF_INET)
    {
      dataLen = IGMP_V3_PKT_MIN_LENGTH + numSrcs*sizeof(L7_in_addr_t);
    }
    else
    {
      dataLen = MLD_V2_PKT_MIN_LENGTH + numSrcs*sizeof(L7_in6_addr_t);
    }

    MGMD_DEBUG(MGMD_DEBUG_APIS, "sending MLD v2", mgmd_info->ifIndex);

    if (mgmd_send (mgmd_info->mgmdCB, dest,
                   dataStart, dataLen, mgmd_info->ifIndex,
                   L7_NULLPTR) == L7_SUCCESS)
    {
      mgmd_info->mgmdCB->counters.QueriesSent++;
      return L7_SUCCESS;
    }
    else
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, " Failure in sending the packet out \n");
      return L7_FAILURE;
    }
  }
}

/*********************************************************************
* @purpose  MGMD Group Query Compare Function
*
* @param    node1    @b{ (input) } mgmd query request (as a SLL member)
* @param    node2    @b{ (input) } mgmd query request (as a SLL member)
* @param    keySize  @b{ (input) } size of SLL member node to compare.
*
* @returns  The difference between the two nodes.
*
* @notes    This is a comparision function needed by SLL.The keysize is
*           not taken into account here.
*
* @end
*********************************************************************/
L7_int32 mgmd_query_req_compare(void *qreqNode1, void *qreqNode2, L7_uint32 keysize)
{
  mgmd_query_req_t *qreq1 = (mgmd_query_req_t *)qreqNode1;
  mgmd_query_req_t *qreq2 = (mgmd_query_req_t *)qreqNode2;
  L7_int32 cmpResult = 0, i;

  cmpResult = L7_INET_ADDR_COMPARE(&(qreq1->group), &(qreq2->group));

  if (cmpResult != 0)
  {
    return (cmpResult);
  }

  if (qreq1->numSrcs != qreq2->numSrcs)
  {
    return (qreq1->numSrcs - qreq2->numSrcs);
  }

  for (i = 0; i < qreq1->numSrcs; i++)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE," qreq1 Src addr = %p", qreq1->qSrcs[i]);
    MGMD_DEBUG (MGMD_DEBUG_FAILURE," qreq2 Src addr = %p", qreq2->qSrcs[i]);
    cmpResult = L7_INET_ADDR_COMPARE((qreq1->qSrcs[i]), (qreq2->qSrcs[i]));
    if (cmpResult != 0)
    {
     return cmpResult;
    }
  }
  return 0;  /*Queries being compared are same */
}
/*********************************************************************
* @purpose  MGMD Group Query Destroy Function
*
* @param    qreq    @b{ (input) } mgmd query request (as a SLL member)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a destructor function needed by SLL
*
* @end
*********************************************************************/
static L7_RC_t mgmd_query_req_destroy (L7_sll_member_t *qreqNode)
{
  mgmd_query_req_t *qreq = (mgmd_query_req_t*)qreqNode;
  L7_uint32 i;

  if (qreq != L7_NULLPTR)
  {
    mgmd_cb_t *mgmdCB = qreq->mgmdCB;

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting Query(Grp/GrpSrc) timer handle ");
    if (mgmdUtilAppTimerHandleDelete(mgmdCB,&((mgmd_query_req_t*)qreq)->timer,&((mgmd_query_req_t*)qreq)->query_timer_handle ) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not delete query Timer ");
      return L7_FAILURE;
    }
    for (i=0 ; i < qreq->numSrcs; i++)
    {
      if (qreq->qSrcs[i] != L7_NULLPTR)
      {
        MGMD_FREE(mgmdCB->proto, (qreq->qSrcs[i]));
      }
    }
    MGMD_FREE(mgmdCB->proto,(qreq));
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  MGMD Group Query Timeout Event Handler
*
* @param    info  @b{ (input) } Timer Event Info Block
*
*
* @returns  none
*
* @notes    Called when MGMD GROUP QUERY TIMER expires
*
* @end
*********************************************************************/
static void mgmd_group_query_timeout_event_handler (void *pParam)
{
  mgmd_cb_t      *mgmdCB = L7_NULLPTR;
  L7_uint32      intfNum , timeLeft = 0;
  mgmd_query_req_t *qreq;
  L7_int32        count, lmqt, sflag, numSrcs;
  mgmd_info_t    *mgmd_info;
  mgmd_group_t   *mgmd_group = L7_NULLPTR, searchKey;
  L7_inet_addr_t *srcsGtLmqt[MGMD_MAX_QUERY_SOURCES];
  L7_inet_addr_t *srcsLeLmqt[MGMD_MAX_QUERY_SOURCES];
  L7_uint32       numGtLmqt, numLeLmqt;
  L7_uint32       lastMemQueryInterval;
  L7_uint32       lastMemQueryCount;
  L7_uchar8       family;
  void *info;
  L7_int32      handle = (L7_int32)pParam;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  info= (void*)handleListNodeRetrieve(handle);
  if(info == L7_NULLPTR)
  {
    L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_FLEX_MGMD_MAP_COMPONENT_ID,
               "mgmd_group_query_timeout_event_handler: Failed to retrieve handle \n");
    return;
  }
  if (mgmdMapProtocolCtrlBlockGet(((mgmd_timer_event_info_t *)info)->proto,
                                  (MCAST_CB_HNDL_t *)&mgmdCB)!= L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failed to get control block\n");
    return;
  }
  intfNum = ((mgmd_timer_event_info_t *)info)->intfNum;
  qreq = ((mgmd_timer_event_info_t *)info)->qreq;
  mgmd_info = (mgmd_info_t*)&(mgmdCB->mgmd_info[intfNum]);
  family = mgmd_info->mgmdCB->proto;

  MGMD_DEBUG_ADDR(MGMD_DEBUG_APIS, "group=",&qreq->group);
  MGMD_DEBUG (MGMD_DEBUG_APIS, "cnt= %d ",qreq->count);
  MGMD_DEBUG (MGMD_DEBUG_APIS," CB= %d, ",  qreq->mgmdCB);
  MGMD_DEBUG (MGMD_DEBUG_APIS," numSrcs = %d ",qreq->numSrcs);

  qreq->timer = L7_NULLPTR;

  lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

  if ((qreq == L7_NULLPTR) || (mgmdCB == L7_NULLPTR))
    return;

  searchKey.rtrIfNum = intfNum;
  if (inetCopy (&searchKey.group, &qreq->group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
    return;
  }

  if ((mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS," Failed to find the group");
    return;
  }

  count = lastMemQueryCount;

  if (--qreq->count >= 0)
  {
    if (qreq->numSrcs == 0)
    {
      /* Group-specific query */
      lmqt = (count * lastMemQueryInterval) ;

      timeLeft = 0;
      if (appTimerTimeLeftGet(mgmdCB->timerHandle,mgmd_group->groupTimer, &timeLeft) != L7_SUCCESS)
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for grp=",
                            &mgmd_group->group);
      }

      if (timeLeft >  lmqt)
        sflag = 0;
      else
        sflag = 1;

      if (mgmd_group_query_send (mgmd_info, qreq->group, qreq->group, L7_NULLPTR, 0, sflag, lastMemQueryInterval) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS," Failed to send group specific query");
      }
    }
    else
    {
      /* Group-and-source-specific query - 6.6.3.2 */
      numGtLmqt = numLeLmqt = 0;

      mgmd_src_lists_build(mgmd_info, mgmd_group, qreq->qSrcs, qreq->numSrcs,
                           srcsGtLmqt, &numGtLmqt, srcsLeLmqt, &numLeLmqt);
      if (numLeLmqt != 0)
      {
        if (mgmd_group_query_send(mgmd_info,qreq->group, qreq->group,srcsLeLmqt,numLeLmqt,1, lastMemQueryInterval) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_APIS," Failed to send group-src specific query");
        }
      }
    }
  }
  numSrcs = qreq->numSrcs;
  if (qreq->count == 0)
  {
    /* we need to remove this here from the linked list */
    if (SLLDelete (&(mgmd_info->ll_query_reqs), (L7_sll_member_t *)qreq) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not delete the query node from the query List\n");
    }
    /* We need to free this here as the memory for this was allocated while
       adding the timer */
    MGMD_FREE(mgmdCB->proto,(info));
    if (numSrcs != 0)
    {
      mgmd_group->grpSrcQueryData =  L7_NULLPTR;
    }
    else
    {
      mgmd_group->grpQueryData =  L7_NULLPTR;
    }
  }
  else
  {
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)qreq->query_timer_handle,
                            &qreq->timer, lastMemQueryInterval,
                            L7_MGMD_GRP_QUERY_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Grp Timer\n");
      return;
    }
  }
}

/*********************************************************************
* @purpose  To process a MGMD leave group packet
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
* @param    group       @b{ (input) }   Group Address
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_group_leave (mgmd_info_t *mgmd_info, L7_inet_addr_t group)
{
  L7_uchar8     groupStr[IPV6_DISP_ADDR_LEN];
  mgmd_group_t *mgmd_group, searchKey;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  searchKey.rtrIfNum = mgmd_info->ifIndex;
  if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
    return;
  }
  mgmd_group = avlSearchLVL7(&(mgmd_info->mgmdCB->membership), &searchKey, AVL_EXACT);
  if (mgmd_group == L7_NULLPTR)
  {
    inetAddrHtop(&group, groupStr);
    MGMD_DEBUG (MGMD_DEBUG_APIS, "TR_WARN: no group record for %s on %d\n",groupStr, mgmd_info->ifIndex);
    return;
  }
  /*ignore leave message if V1 Host Timer is running */
  if (mgmd_group->v1HostPresentFlag == L7_TRUE)
    return;

  /* RFC 3376 7.3.2 - IGMPv2 Leave is equivalent to IGMPv3 TO_IN( {} ) */
  mgmd_v3_to_in_process(mgmd_info, mgmd_group, 0, L7_NULLPTR);

}

/*********************************************************************
* @purpose  To process a MGMD membership V3 query packet
*
* @param    mgmd_info     @b{ (input) }   MGMD Info
* @param    group         @b{ (input) }   Group Address
* @param    source        @b{ (input) }   Source Address
* @param    max_resp_code @b{ (input) }   Max Response Code
* @param    v3Query       @b{ (input) }   V3 Query structure
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_v3membership_query(mgmd_info_t *mgmd_info, L7_inet_addr_t group,
                                    L7_inet_addr_t source, L7_uchar8 max_resp_code,
                                    L7_mgmdv3Query_t *v3Query)
{
  mgmd_group_t         *mgmd_group, searchKey;
  L7_int32              result=0, numSrcs, index;
  mgmd_source_record_t *srcRec;
  mgmd_cb_t            *mgmdCB = mgmd_info->mgmdCB;
  L7_inet_addr_t       *srcs;
  L7_uint32             robustnessvar;
  L7_uint32             queryInterval;
  L7_uint32             responseInterval;
  L7_uint32             startupQueryCount;
  L7_int32              mgmd_other_querier_present_interval;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;
  startupQueryCount = mgmd_info->intfConfig.startupQueryCount;

  mgmd_other_querier_present_interval = ((robustnessvar * queryInterval) + (responseInterval / 2));

  if (mgmd_info == L7_NULLPTR)
    return;

  mgmdCB = mgmd_info->mgmdCB;

  result = L7_INET_ADDR_COMPARE(&(mgmd_info->mgmd_querierIP), &source);
  if (result == 0)
  {
    /* Reset other querier present timer */
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_querier_timer_handle,
                            &mgmd_info->mgmd_querier_timer,
                            mgmd_other_querier_present_interval, L7_MGMD_QUERIER_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Failed to update Querier timer\n");
      return;
    }
  }
  else if (result > 0)
  {
    /* This querier has lower ip address than the current */
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_querier_timer_handle,
                            &mgmd_info->mgmd_querier_timer,
                            mgmd_other_querier_present_interval,
                            L7_MGMD_QUERIER_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Querier Timer\n");
      return;
    }

    mgmd_info->mgmd_querierIP = source;
    if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
    {
      MGMD_DEBUG (MGMD_DEBUG_QUERY, "TR_INFO: Querier Changed on %d\n", mgmd_info->ifIndex);
      MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "Old Querier", &(mgmd_info->mgmd_querierIP));
      MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "New Querier is ", &source);

      MCAST_BITX_RESET (mgmd_info->flags.bits, IGMP_QUERIER);
      /* Update the Querier filter mask. */
      BITX_RESET(&(mgmdCB->mgmd_querier_if_mask), mgmd_info->ifIndex);
      /* Send Async Querier Update Event */
      if (mgmdCB->mgmd_proxy_status == L7_ENABLE)
      {
        mgmd_proxy_querier_update_event_send(mgmdCB, mgmd_info->ifIndex, L7_FALSE);
      }

      /* Reset Querier-Up-Time */
      mgmd_info->querierUpTime = osapiUpTimeRaw();

      /* Don't turn off timer during startup stage, although the RFC may not require this */
      if (mgmd_info->mgmd_query_count >= startupQueryCount)
      {
        if (mgmdUtilAppTimerDelete(mgmdCB, &mgmd_info->mgmd_query_timer) != L7_SUCCESS)
        {
          MGMD_DEBUG(MGMD_DEBUG_FAILURE,"Could not Delete the QueryTimer on iface = %d\n",
                          mgmd_info->ifIndex);
          return;
        }
      }
    }
  }

  if (!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
  {
    /* If this query message contains non-zero values for the Querier's
     * Robustness Variable or the Query Interval Code, adopt these values.
     */
    if ((v3Query->mgmdResvSflagQRV & 0x07) != 0) /* RFC 3376 4.1.6 */
      robustnessvar = v3Query->mgmdResvSflagQRV & 0x07;

    if (v3Query->mgmdQQIC != 0) /* RFC 3376 4.1.7 */
    {
      L7_int32 qqic = (L7_int32)v3Query->mgmdQQIC;
      if ((v3Query->mgmdQQIC & 0x80) == 0)
        queryInterval = qqic;
      else
        queryInterval = ((qqic & 0x0F) | 0x10) << (((qqic & 0x70) >> 4) + 3);
      mgmd_info->querierQueryInterval = queryInterval;
    }

    /* If S Flag (Suppress Router-Side Processing) is turned off, update timers */
    if ((v3Query->mgmdResvSflagQRV & 0x08) == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_QUERY, " Supress Flag turned off");
      searchKey.rtrIfNum = mgmd_info->ifIndex;
      if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
        return;
      }
      mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT);
      if (mgmd_group != L7_NULLPTR)
      {
        L7_uint32 lastMemQueryCount, lastMemQueryInterval, lmqt;

        numSrcs = v3Query->mgmdQueryNumSrcs;

        lastMemQueryInterval = mgmd_info->intfConfig.lastMemQueryInterval;
        lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;

        lmqt= ((lastMemQueryCount * lastMemQueryInterval) );


        MGMD_DEBUG (MGMD_DEBUG_QUERY, " Group found , lmqt = %d, numSrcs = %d",
                    lmqt, numSrcs);

        if (numSrcs == 0)
        {
          if (mgmdUtilAppTimerLower(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                                    &mgmd_group->groupTimer, lmqt, L7_MGMD_GRP_TIMER) != L7_SUCCESS)
          {
            MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not lower the Group timer  \n");
            return;
          }
          mgmd_group->interval = lmqt;
          MGMD_DEBUG (MGMD_DEBUG_QUERY, " Updated grp Timer ");
        }
        else
        { /* Group-and-source-specific query */
          srcs = v3Query->mgmdQuerySrcs;
          for (index = 0; index < numSrcs; index++)
          {
            mgmd_source_record_t srcRecSrch;
            srcRecSrch.sourceAddress = srcs[index];;

            srcRec = (mgmd_source_record_t *)SLLFind(&(mgmd_group->sourceRecords),
                                                     (L7_sll_member_t *)&srcRecSrch);
            if (srcRec != L7_NULLPTR)
            {
              if (mgmdUtilAppTimerLower(mgmdCB, L7_NULL,(void*)srcRec->src_timer_handle,
                                        &srcRec->sourceTimer, lmqt, L7_MGMD_SRC_TIMER) != L7_SUCCESS)
              {
                MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not lower the Source timer  \n");
                return;
              }
              srcRec->sourceInterval = lmqt;
              MGMD_DEBUG (MGMD_DEBUG_QUERY, " Updated src Timer ");
            }
            /* Advance to next source address in query */
            srcs++;
          }
        }
      } /* If group exists */
    } /* If S Flag off */
  } /* If !Querier */
}

/*********************************************************************
* @purpose  To process a MGMD membership query packet
*
* @param    mgmd_info     @b{ (input) }   MGMD Info
* @param    group         @b{ (input) }   Group Address
* @param    source        @b{ (input) }   Source Address
* @param    max_resp_time @b{ (input) }   Max Response Time
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_membership_query (mgmd_info_t *mgmd_info, L7_inet_addr_t group,
                                   L7_inet_addr_t source, L7_int32 max_resp_time)
{
  mgmd_group_t  *mgmd_group;
  L7_int32       result;
  mgmd_cb_t     *mgmdCB = mgmd_info->mgmdCB;
  L7_uint32      robustnessvar;
  L7_uint32      queryInterval;
  L7_uint32      responseInterval;
  L7_uint32      lastMemQueryCount;
  L7_uint32      startupQueryCount;
  L7_int32       mgmd_other_querier_present_interval;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  robustnessvar = mgmd_info->intfConfig.robustness;
  queryInterval = mgmd_info->intfConfig.queryInterval;
  responseInterval = mgmd_info->intfConfig.queryResponseInterval;

  lastMemQueryCount = mgmd_info->intfConfig.lastMemQueryCount;
  startupQueryCount = mgmd_info->intfConfig.startupQueryCount;

  mgmd_other_querier_present_interval = ((robustnessvar * queryInterval) + (responseInterval / 2));

  if (mgmd_info == L7_NULLPTR)
    return;
  if (max_resp_time == 0)
    MGMD_DEBUG(MGMD_DEBUG_FAILURE,"Warning: Received IGMP Version 1 Membership Query\n");

  mgmdCB = mgmd_info->mgmdCB;

  result = L7_INET_ADDR_COMPARE(&(mgmd_info->mgmd_querierIP), &source);
  if (result == 0)
  {
    /* Reset other querier present timer */
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_querier_timer_handle,
                            &mgmd_info->mgmd_querier_timer,
                            mgmd_other_querier_present_interval,
                            L7_MGMD_QUERIER_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Querier Timer Updation Failed \n");
      return;
    }
    MGMD_DEBUG (MGMD_DEBUG_QUERY, "TR_INFO: Update querier on %d\n", mgmd_info->ifIndex);
    MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "Querier is ", &source);
  }
  else if (result > 0)
  {
    /* This querier has lower ip address than the current */
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_querier_timer_handle,
                            &mgmd_info->mgmd_querier_timer,
                            mgmd_other_querier_present_interval,
                            L7_MGMD_QUERIER_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Querier Timer\n");
      return;
    }

    /* timeout might be happened and queued XXX */
    MGMD_DEBUG (MGMD_DEBUG_QUERY, "TR_INFO: Querier Changed on %d\n", mgmd_info->ifIndex);
    MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "Old Querier", &(mgmd_info->mgmd_querierIP));
    MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "New Querier is ", &source);

    mgmd_info->mgmd_querierIP = source;
    if (MCAST_BITX_TEST(mgmd_info->flags.bits, IGMP_QUERIER))
    {
      MCAST_BITX_RESET(mgmd_info->flags.bits, IGMP_QUERIER);
      /* Update the Querier filter mask. */
      BITX_RESET(&(mgmd_info->mgmdCB->mgmd_querier_if_mask), mgmd_info->ifIndex);
      /* Send Async Querier Update Event */
      if (mgmdCB->mgmd_proxy_status == L7_ENABLE)
      {
        mgmd_proxy_querier_update_event_send(mgmd_info->mgmdCB, mgmd_info->ifIndex, L7_FALSE);
      }

      /* Reset Querier-Up-Time */
      mgmd_info->querierUpTime = osapiUpTimeRaw();
      /* don't make it off during startup stage, *
       * although I'm not sure RFC requires this */
      if (mgmd_info->mgmd_query_count >= startupQueryCount)
      {
        if (mgmdUtilAppTimerDelete(mgmdCB, &mgmd_info->mgmd_query_timer) != L7_SUCCESS)
        {
          MGMD_DEBUG(MGMD_DEBUG_FAILURE,"Could not Delete the QueryTimer on iface = %d\n",
                          mgmd_info->ifIndex);
          return;
        }
      }
    }
  }
  else
  {
    MGMD_DEBUG (MGMD_DEBUG_QUERY, "TR_INFO: neglect querier on %d\n", mgmd_info->ifIndex);
    MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "Querier is ", &source);
  }

  if (!MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
  {
    mgmd_group_t searchKey;
    searchKey.rtrIfNum = mgmd_info->ifIndex;
    if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Copy operation Failed \n");
      return;
    }
    MGMD_DEBUG (MGMD_DEBUG_QUERY, "Yes ! its not querier on %d\n", mgmd_info->ifIndex);

    mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT);
    if (mgmd_group != L7_NULLPTR)
    {
      L7_uint32 newTime;

      MGMD_DEBUG_ADDR (MGMD_DEBUG_QUERY, "Found group ", &(mgmd_group->group));

      newTime = ((lastMemQueryCount * max_resp_time) );
      if (mgmdUtilAppTimerLower(mgmdCB, L7_NULL, (void*)mgmd_group->grp_timer_handle,
                                &mgmd_group->groupTimer, newTime, L7_MGMD_GRP_TIMER)
                                != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not update the Group timer  \n");
        return;
      }
      mgmd_group->interval = newTime;

      MGMD_DEBUG(MGMD_DEBUG_QUERY, " Updated grp timer to %d ", mgmd_group->interval);

    }
  }
}
/*********************************************************************
* @purpose  MGMD receive
*
* @param    message     @b{ (input) }   Multicast control packet
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_receive (mcastControlPkt_t *message)
{
  mgmd_cb_t         *mgmdCB = L7_NULLPTR;
  L7_BOOL            proxyInterface = L7_FALSE;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  mgmdDebugPacketRxTxTrace(message->family, L7_TRUE, message->rtrIfNum,
                           &message->srcAddr, &message->destAddr,
                           message->payLoad, message->length);

  if (mgmdMapProtocolCtrlBlockGet(message->family,
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE,"MGMD:Failed to access control block\n");
    return;
  }

  /* Need to fix the following for MLD pkt reception
   * Commenting out for now

  if ((message->length < 0) || (inetIsLocalAddress(&source, message->rtrIfNum)))
  {
    return;
  }
  */

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_RX, "Packet len %d with invalid Control Block \n", message->length);
    return;
  }

  /* Either MGMD or MGMD-Proxy need to be enabled on the received interface.
   */
  if (message->rtrIfNum == mgmdCB->mgmd_proxy_ifIndex)
  {
    proxyInterface = L7_TRUE;
  }
  else if (mgmdCB->mgmd_info[message->rtrIfNum].ifIndex == 0)
  {
    MGMD_DEBUG (MGMD_DEBUG_RX, "MGMD Interface Index is Zero");
    return;
  }
  else
  {
    /* Do nothing and proceed with the processing of the packet */
  }

  if (mgmdCB->proto == L7_AF_INET)
  {
    mgmd_process_v4_packet(mgmdCB, message, proxyInterface);
  }
  else if (mgmdCB->proto == L7_AF_INET6)
  {
    mgmd_process_v6_packet(mgmdCB, message, proxyInterface);
  }
  else
  {
    MGMD_DEBUG (MGMD_DEBUG_RX, " Packet received with invalid family = %d", mgmdCB->proto);
    return;
  }
}

/*********************************************************************
* @purpose  Initialize the Group and Source Addresses in a V3 Report
*
* @param    mgmdCB   @b{ (input) } Pointer to the MGMD Control Block
*           v3Report @b{ (input) } Pointer to the L7_mgmdv3Report_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
mgmd_v3_report_init (mgmd_cb_t *mgmdCB,
                     L7_mgmdv3Report_t *v3Report)
{
  L7_mgmdv3GroupRecord_t *grpRecord = L7_NULLPTR;
  L7_uchar8 addrFamily = 0;
  L7_uint32 grpIndex = 0;
  L7_uint32 srcIndex = 0;

  if ((mgmdCB == L7_NULLPTR) || (v3Report == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  memset(v3Report, 0, sizeof(L7_mgmdv3Report_t));
  addrFamily = mgmdCB->proto;

  for (grpIndex = 0; grpIndex < MGMD_MAX_GROUP_RECORDS; grpIndex++)
  {
    grpRecord = &(v3Report->mgmdReportGroupRecs[grpIndex]);

    /* Initialize the Group and Source Addresses */
    inetAddressZeroSet (addrFamily, &grpRecord->mgmdGrpRecGroupAddr);
    for (srcIndex = 0; srcIndex < MGMD_MAX_QUERY_SOURCES; srcIndex++)
    {
      inetAddressZeroSet (addrFamily, &grpRecord->mgmdGrpRecSrcs[srcIndex]);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize the Source Addresses in a V3 Query
*
* @param    mgmdCB   @b{ (input) } Pointer to the MGMD Control Block
*           v3Query  @b{ (input) } Pointer to the L7_mgmdv3Query_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
mgmd_v3_query_init (mgmd_cb_t *mgmdCB,
                    L7_mgmdv3Query_t *v3Query)
{
  L7_uchar8 addrFamily = 0;
  L7_uint32 srcIndex = 0;

  if ((mgmdCB == L7_NULLPTR) || (v3Query == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  memset(v3Query, 0, sizeof(L7_mgmdv3Query_t));
  addrFamily = mgmdCB->proto;

  for (srcIndex = 0; srcIndex < MGMD_MAX_QUERY_SOURCES; srcIndex++)
  {
    inetAddressZeroSet (addrFamily, &v3Query->mgmdQuerySrcs[srcIndex]);
  }

  return L7_SUCCESS;
}

/*****************************************************************
* @purpose  calculates the max response time from the max response code
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns
* @notes
*


   If Max Resp Code < 128, Max Resp Time = Max Resp Code

   If Max Resp Code >= 128, Max Resp Code represents a floating-point
   value as follows:

       0 1 2 3 4 5 6 7
      +-+-+-+-+-+-+-+-+
      |1| exp | mant  |
      +-+-+-+-+-+-+-+-+

   Max Resp Time = (mant | 0x10) << (exp + 3)
* @end
*********************************************************************/

L7_int32 mgmd_decode_max_resp_code(L7_uchar8 family, L7_int32 max_resp_code)
{
  L7_int32           max_resp_time = 0;

  if (max_resp_code < 0x80)
  {
    max_resp_time= max_resp_code;
  }
  else
  {
    max_resp_time= ((max_resp_code & 0x0F) | 0x10) << (((max_resp_code & 0x70) >>4)+3);
  }

  if (family == L7_AF_INET)
  {
   max_resp_time = max_resp_time/10;
  }
  else if (family == L7_AF_INET6)
  {
    max_resp_time = max_resp_time/1000;
  }
  return max_resp_time;
}
/*********************************************************************
* @purpose  Process an IGMP (v4) packet
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_process_v4_packet(mgmd_cb_t *mgmdCB, mcastControlPkt_t *message,
                                   L7_BOOL proxyInterface)
{
  L7_uint32          type, maxRespTime, maxRespCode;
  L7_inet_addr_t     source, group;
  L7_ushort16        checksum;
  L7_in_addr_t       groupAddr;
  L7_mgmdv3Report_t  *v3Report = L7_NULLPTR;
  L7_mgmdv3Query_t   v3Query;
  L7_uint32          mgmdVersion, mgmdHostVersion = 0;
  mgmd_info_t       *mgmd_info;
  L7_uchar8         *payload = message->payLoad;
  L7_uint32          rtrIfNum = message->rtrIfNum;
  L7_BOOL            checkForTOS = L7_FALSE;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  v3Report = &mgmdCB->v3Report;
  /* TODO: Findout the right place to init the following*/
  /*
  mgmd_v3_report_init (mgmdCB, v3Report);
  mgmd_v3_query_init (mgmdCB, &v3Query);
  */
  if (inetCopy(&source, &(message->srcAddr)) != L7_SUCCESS)
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE, "Copy operation Failed \n");
    return;
  }
  inetAddressZeroSet (mgmdCB->proto, &group);

  MGMD_DEBUG_ADDR (MGMD_DEBUG_RX, "Packet from ", &source);

  /* Validate checksum of the recd. packet */
  if (inetChecksum(payload, message->length) != 0)
  {
    MGMD_DEBUG (MGMD_DEBUG_RX, "Packet len %d with invalid checksum \n", message->length);
    return;
  }
  /* Validate total length value */
  if (message->length < IGMP_PKT_MIN_LENGTH)
    return;

  MCAST_GET_BYTE(type, payload);
  v3Report->mgmdReportType = type;
  v3Query.mgmdMsg.mgmdType = type;

  if (type < IGMP_MEMBERSHIP_QUERY || type > IGMP_V3_MEMBERSHIP_REPORT)
  {
    MGMD_DEBUG (MGMD_DEBUG_RX, "Recd unsupported type %d on %d len %d,"
                " ignore!\n", type, rtrIfNum, message->length);
    return;
  }

  /* "Reference: RFC3376 - Section 4. Do not accept IGMPv3 Report or Query
   *  messages without the IP TOS set to 0xC0."
   *  However this check is performed only if IGMP is configured to.
   */
  if (mgmdCB->checkForTOS == L7_TRUE)
  {
    if (type == IGMP_V3_MEMBERSHIP_REPORT)
    {
      checkForTOS = L7_TRUE;
    }
    else if (type == IGMP_MEMBERSHIP_QUERY)
    {
      if (message->length >= IGMP_V3_PKT_MIN_LENGTH)
      {
        checkForTOS = L7_TRUE;
      }
    }
    else
    {
      /* Do Nothing */
    }
  }

  if (checkForTOS == L7_TRUE)
  {
    if (((message->ipTypeOfService) & (0xC0)) != 0xC0)
    {
      MGMD_DEBUG (MGMD_DEBUG_RX, "IGMPv3 Message received with Invalid ToS - 0x%x, "
                  "Discarding it.\n", message->ipTypeOfService);
      return;
    }
  }

  MCAST_GET_BYTE(maxRespCode, payload);
  v3Report->mgmdReportReserved1 = maxRespCode;
  /* As the packet has the max-respons-time in 1/10 of secs, convert it to seconds
     for further processing */
   maxRespTime = mgmd_decode_max_resp_code(mgmdCB->proto,maxRespCode);
   v3Query.mgmdMsg.igmpMaxRespTime = maxRespTime;

  if (mgmdCB->checkRtrAlert == L7_TRUE)
  {
    /* Router alert option is mandatory for v2 and v3 only.For V1 this is
       not mandatory.

      The following check is to identify the v2 and v3 query packets.For V1
      or V2 or V3 query packet the type value is same.So the differntiation
      of the packet is based on length of the packet and max response value.

      (message->length>= IGMP_V3_PKT_MIN_LENGTH)||
      ((message->length == IGMP_PKT_MIN_LENGTH)&&(delay != 0)))))

      For V3 query packet, minimum length should be 12 bytes.

      For V2 query packet, minimum length should be 8 bytes and max response(Here
      the value of "delay") value should be non zero.

      For V1 query packet, minimum length should be 8 bytes and max response(Here
      the value of "delay") value should be zero.

     */

    if ((type == IGMP_V3_MEMBERSHIP_REPORT) ||
        (type == IGMP_V2_MEMBERSHIP_REPORT) ||
        (type == IGMP_V2_LEAVE_GROUP)||
        ((type== IGMP_MEMBERSHIP_QUERY)&&
         ((message->length>= IGMP_V3_PKT_MIN_LENGTH)||
          ((message->length == IGMP_PKT_MIN_LENGTH)&&(maxRespTime != 0)))))
    {
      if (message->ipRtrAlert != L7_TRUE)
      {
        MGMD_DEBUG (MGMD_DEBUG_RX, "Packet does not contain router alert option \n");
        return;
      }
    }
  }
  /* Convert the igmp-Header fields from Network to Host Byte Order */
  MCAST_GET_SHORT (checksum, payload);
  checksum = osapiNtohs (checksum);
  v3Report->mgmdReportChecksum = checksum;
  v3Query.mgmdMsg.mgmdChecksum = checksum;

  if (type != IGMP_V3_MEMBERSHIP_REPORT)
  {
    MCAST_GET_ADDR(&groupAddr, payload);
    groupAddr.s_addr = osapiNtohl (groupAddr.s_addr);
    inetAddressSet(L7_AF_INET,&groupAddr, &(group));
    inetAddressSet(L7_AF_INET,&groupAddr, &(v3Query.mgmdMsg.mgmdGroupAddr));

    /* Don't process reports for non-multicast addresses */
    if ((type != IGMP_MEMBERSHIP_QUERY) || (inetIsAddressZero(&group) != L7_TRUE))
    {
      if (inetIsInMulticast(&group) != L7_TRUE)
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"MLD packet received for non-multicastaddr = ",
                  &group);
        return;
      }
    }
  }
  else
  {
    if (inetIsAddressIgmpv3Routers(&message->destAddr) != L7_TRUE)
    {
      MGMD_DEBUG (MGMD_DEBUG_RX, "Recd v3 report with dest'n not equal to all v3 reports");
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Recd v3 report with dest'n not equal to all v3 reports");
      return;
    }
    MCAST_GET_SHORT(v3Report->mgmdReportReserved2, payload);
    MCAST_GET_SHORT(v3Report->mgmdReportNumGroupRecs, payload);
  }

    mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
    mgmdVersion = mgmd_info->intfConfig.version;

  mgmdCB->counters.TotalPacketsReceived++;

  MGMD_DEBUG (MGMD_DEBUG_RX, " Type = %x, proxyInterface = %d ", type,proxyInterface );
  MGMD_DEBUG (MGMD_DEBUG_RX, " mgmdVersion = %d ",mgmdVersion);

  if (proxyInterface != L7_TRUE)
  {
    switch (type)
    {
      case IGMP_V1_MEMBERSHIP_REPORT:
        /* Discard v1-Reports if IGMP version on received interface is
         * NOT igmpV1 */
        if (mgmdVersion < L7_MGMD_VERSION_1)
          return;
        break;

      case IGMP_V2_MEMBERSHIP_REPORT:
        /* Discard v2-Reports if IGMP version on received interface is
         * NOT igmpV2. */
        if (mgmdVersion < L7_MGMD_VERSION_2)
          return;
        break;

      case IGMP_V3_MEMBERSHIP_REPORT:
        /* Discard v3-Reports if IGMP version on received interface is
         * NOT igmpV3. */
        if (mgmdVersion < L7_MGMD_VERSION_3)
          return;
        break;

      case IGMP_MEMBERSHIP_QUERY:
      case IGMP_V2_LEAVE_GROUP:
        break;
      default:
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: MGMD : Wrong packet type = %d ",type);
        mgmdCB->counters.WrongTypePackets++;
        break;
    }  /*End-of-Switch*/

    mgmdCB->mgmd_info[rtrIfNum].ifIndex = rtrIfNum;
    switch (type)
    {
      case IGMP_V1_MEMBERSHIP_REPORT:
        /* Received a V1Report while configured in V2 */
        mgmd_v1membership_report(mgmd_info,
                                 group, source);
        mgmdCB->counters.ReportsReceived++;
        break;

      case IGMP_V2_MEMBERSHIP_REPORT:
        mgmd_v2membership_report(mgmd_info,
                                 group, source);
        mgmdCB->counters.ReportsReceived++;
        break;

      case IGMP_V3_MEMBERSHIP_REPORT:
         if (mgmd_v3_report_form(mgmdCB->proto, v3Report, payload) != L7_SUCCESS)
         {
           MGMD_DEBUG (MGMD_DEBUG_RX,"IPV4 :Rx'ed invalid srcs in src lists ");
           return;
         }
         mgmd_v3membership_report(mgmd_info,
                                  v3Report, source);
          mgmdCB->counters.ReportsReceived++;
         break;

       case IGMP_MEMBERSHIP_QUERY:
         /* Determine the query version by checking the length of the query. */
         /* RFC 3376 - Section 7.1 */
          mgmdCB->counters.QueriesReceived++;

        mgmdVersion = mgmd_info->intfConfig.version;
        if (message->length == IGMP_PKT_MIN_LENGTH && maxRespTime == 0)
        {
          if (mgmdVersion != L7_MGMD_VERSION_1)
            mgmd_info->Wrongverqueries++;
        }
        else if (message->length == IGMP_PKT_MIN_LENGTH && maxRespTime != 0)
        {
          if (mgmdVersion != L7_MGMD_VERSION_2)
            mgmd_info->Wrongverqueries++;
        }
        else if (message->length >= IGMP_V3_PKT_MIN_LENGTH)
        {
          if (mgmdVersion != L7_MGMD_VERSION_3)
            mgmd_info->Wrongverqueries++;
        }

        if (message->length >= IGMP_V3_PKT_MIN_LENGTH)
        {
          mgmd_v3_query_form(mgmdCB->proto, &v3Query, payload);
          mgmd_v3membership_query (mgmd_info, group, source, maxRespCode, &v3Query);
        }
        else if (message->length == IGMP_PKT_MIN_LENGTH)
        {
          mgmd_membership_query (mgmd_info, group, source, maxRespTime);
        }
        break;

      case IGMP_V2_LEAVE_GROUP:
        /* Ignore Leave messages when -
            a) configured for v1
            b) Destination address in IP-Header is not a Multicast Address.
        */
        mgmdCB->counters.LeavesReceived++;
        mgmdVersion = mgmd_info->intfConfig.version;
        if ((mgmdVersion != L7_MGMD_VERSION_1) && inetIsInMulticast(&(message->destAddr)))
        {
          mgmd_group_leave (mgmd_info, group);
        }
        break;

      default:
        break;
    }  /*End-of-switch*/
  }
  else
  { /* (proxyInterface == L7_TRUE) */

    mgmdHostVersion = mgmdCB->mgmd_host_info->configVersion;
    switch (type)
    {
      case IGMP_V1_MEMBERSHIP_REPORT:
        mgmd_proxy_v1v2report_process(mgmdCB, group, source);
        mgmdCB->counters.ReportsReceived++;
        break;

    case IGMP_V2_MEMBERSHIP_REPORT:
        mgmd_proxy_v1v2report_process(mgmdCB, group, source);
        mgmdCB->counters.ReportsReceived++;
        break;

      case IGMP_V3_MEMBERSHIP_REPORT:
        MGMD_DEBUG (MGMD_DEBUG_RX, " V3 report rx'ed");
        /* NOT Processed */
        break;

      case IGMP_MEMBERSHIP_QUERY:
        /* Determine the query version by checking the length of the query. */
        /* RFC 3376 - Section 7.1 */
        mgmdCB->counters.QueriesReceived++;
        if (message->length >= IGMP_V3_PKT_MIN_LENGTH)
        {
          if (mgmdHostVersion < L7_MGMD_VERSION_3)
          {
            MGMD_DEBUG (MGMD_DEBUG_RX," Rx'ed Query with higher version for v= %d", mgmdHostVersion);
            return;
          }
          mgmd_v3_query_form(mgmdCB->proto, &v3Query, payload);
          mgmd_proxy_v3membership_query_process(mgmdCB, group, source,
                                                maxRespCode, &v3Query);
        }
        else if (message->length == IGMP_PKT_MIN_LENGTH)
        {
          if (maxRespTime != 0)
          {
            if (mgmdHostVersion < L7_MGMD_VERSION_2)
            {
              MGMD_DEBUG (MGMD_DEBUG_RX," Rx'ed Query with higher version for v= %d", mgmdHostVersion);
              return;
            }
          }
          mgmd_proxy_membership_query_process(mgmdCB,group,source,maxRespTime);
        }
        break;

      case IGMP_V2_LEAVE_GROUP:
        /* Ignore Leave messages when configured for v1 */
        /* Update the stats */
        mgmdCB->counters.LeavesReceived++;
        break;

      default:
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: MGMD : Wrong packet type = %d ",type);
        mgmdCB->counters.WrongTypePackets++;
        break;
    }  /*End-of-switch*/
  }  /*End-of-ELSE-Block*/
}

/*********************************************************************
* @purpose  Read V3 report from the received message
*
* @param    family    @b{ (input) }   Family
* @param    v3Report  @b{ (output) }  Pointer to V3 Report
* @param    buf       @b{ (input) }   Pointer to the recd. message (V3 Membership Report)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmd_v3_report_form(L7_uint32 family, L7_mgmdv3Report_t *v3Report, L7_uchar8 *buf)
{
  L7_uint32 numGroups = 0;
  L7_uint32 numSrcs = 0;
  L7_uint32     i,j;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  numGroups = v3Report->mgmdReportNumGroupRecs;
  if (numGroups)
  {
    for (i = 0; i<numGroups; i++)
    {
      L7_mgmdv3GroupRecord_t  *groupRecord;
      L7_inet_addr_t           grpAddr;
      groupRecord = &(v3Report->mgmdReportGroupRecs[i]);

      MCAST_GET_BYTE(groupRecord->mgmdGrpRecType, buf);
      MCAST_GET_BYTE(groupRecord->mgmdGrpRecAuxDataLen, buf);
      MCAST_GET_SHORT(groupRecord->mgmdGrpRecNumSrcs, buf);
      MCAST_GET_ADDR_INET(family, &grpAddr, buf);
      MGMD_DEBUG_ADDR(MGMD_DEBUG_RX, "grpAddr[] = ", &grpAddr);
      inetCopy( &(groupRecord->mgmdGrpRecGroupAddr), &grpAddr);

      if (inetIsInMulticast(&(groupRecord->mgmdGrpRecGroupAddr)) != L7_TRUE)
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"IGMP/MLD packet received for non-multicastaddr = ",
                        &(groupRecord->mgmdGrpRecGroupAddr));
        return L7_FAILURE;
      }

      if (family == L7_AF_INET6 &&
           mgmd_ipv6_is_scope_id_valid(&(groupRecord->mgmdGrpRecGroupAddr)) ==L7_FALSE )
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,
          "Discarding MLD packet rx'ed for invalid scope grp addr = ",
                    &(groupRecord->mgmdGrpRecGroupAddr));
        return L7_FAILURE;
      }

      if (family == L7_AF_INET6)
      {
        if (inetIsMulticastReservedSiteLocalAddress (&(groupRecord->mgmdGrpRecGroupAddr))
                                                  == L7_TRUE)
        {
          MGMD_DEBUG_ADDR (MGMD_DEBUG_REPORTS, "Discarding MLD Packet received "
                           "with a Reserved Site Local Group Address - ",
                           &groupRecord->mgmdGrpRecGroupAddr);
          return L7_FAILURE;
        }
      }

      numSrcs = groupRecord->mgmdGrpRecNumSrcs;
      if (numSrcs > MGMD_MAX_QUERY_SOURCES)
      {
        numSrcs = groupRecord->mgmdGrpRecNumSrcs = MGMD_MAX_QUERY_SOURCES;
      }
      if (numSrcs > 0)
      {
        for (j=0; j<numSrcs; j++)
        {
          L7_inet_addr_t  srcAddr;
          inetAddressZeroSet(family, &srcAddr);
          MCAST_GET_ADDR_INET(family, &srcAddr, buf);
          MGMD_DEBUG_ADDR(MGMD_DEBUG_RX, "srcAddr[] = ", &srcAddr);
          if ((inetIsValidHostAddress(&srcAddr) != L7_TRUE) ||
              (inetIsAddressZero(&srcAddr) == L7_TRUE))
          {
            MGMD_DEBUG (MGMD_DEBUG_RX," Invalid Source address in srclist ");
            return L7_FAILURE;
          }
          inetCopy(&(groupRecord->mgmdGrpRecSrcs[j]), &srcAddr);
        } /* End for (j) */
      } /* End if(numSrcs) */
    } /* End for (i) */
  } /* End if(numGrps) */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read V3 Query from the received message
*
* @param    family    @b{ (input) }   Family
* @param    v3Query   @b{ (output) }  Pointer to V3 Query
* @param    buf       @b{ (input) }   Pointer to the recd. message (V3 Membership Query)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_v3_query_form(L7_uint32 family, L7_mgmdv3Query_t *v3Query, L7_uchar8 *buf)
{
  L7_uint32 numSrcs = 0;
  int j = 0;


  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  MCAST_GET_BYTE(v3Query->mgmdResvSflagQRV, buf);
  MCAST_GET_BYTE(v3Query->mgmdQQIC, buf);
  MCAST_GET_SHORT(numSrcs, buf);

  v3Query->mgmdQueryNumSrcs = numSrcs;
  if (numSrcs > MGMD_MAX_QUERY_SOURCES)
  {
    numSrcs = v3Query->mgmdQueryNumSrcs = MGMD_MAX_QUERY_SOURCES;
  }
  if (numSrcs > 0)
  {
    for (j=0; j<numSrcs; j++)
    {
      L7_inet_addr_t  srcAddr;
      MCAST_GET_ADDR_INET(family, &srcAddr, buf);
      inetCopy(&(v3Query->mgmdQuerySrcs[j]), &srcAddr );
    } /* End for (j) */
  } /* End if(numSrcs) */
}

/*********************************************************************
* @purpose  Groups Timeout Event Handler
*
* @param    param      @b{ (input) }  mgmd Control Block
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_timeout_groups_event_handler (void *param)
{
  mgmd_cb_t            *mgmdCB;
  mgmd_source_record_t *srcRec, *prev = L7_NULL;
  L7_BOOL               delGroup;
  L7_uint32             source = 0;
  L7_inet_addr_t        tmpSrc;
  mgmd_host_info_t     *mgmd_host_info;
  mgmdGroupInfo_t       group_info;
  L7_uint32             rtrIfNum;
  mgmd_info_t  *mgmd_info;
  mgmd_group_t *mgmd_group = L7_NULLPTR;

  L7_int32      handle = (L7_int32)param;
  mgmd_group_timer_data_t *timerData;
  L7_uint32             proxy_status;

  MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Entered ");

  timerData = (void*)handleListNodeRetrieve(handle);

  if (timerData == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Handle retrieve failed ");
    return;
  }

  mgmd_group = timerData->groupRec;
  mgmdCB = timerData->mgmdCb;

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_TIMERS,"retrieved Handle node has null Control block");
    return;
  }

  mgmd_host_info    = mgmdCB->mgmd_host_info;

  mgmd_group->groupTimer = L7_NULLPTR;
  inetAddressZeroSet (mgmdCB->proto, &tmpSrc);

  proxy_status = mgmdCB->mgmd_proxy_status;
  rtrIfNum = mgmd_group->rtrIfNum;
  mgmd_info = (mgmd_info_t*)&(mgmdCB->mgmd_info[rtrIfNum]);

  /* Initialize group_info. */
  memset(&group_info, 0, sizeof(mgmdGroupInfo_t));
  group_info.numSrcs = 0;

  MGMD_DEBUG (MGMD_DEBUG_TIMERS," grp filter mode = %d", mgmd_group->filterMode);

  if (mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)
  {
    delGroup = L7_TRUE;
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      MGMD_DEBUG (MGMD_DEBUG_TIMERS," srcRec tmr = %p", srcRec->sourceTimer);

      /* Delete all source records with timeout off... these are no longer being excluded */
      if (srcRec->sourceTimer == L7_NULLPTR)
      {
        L7_uchar8  sourceMode;
        sourceMode = MGMD_FILTER_MODE_EXCLUDE;

        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, sourceMode, MGMD_SOURCE_DELETE);
        MGMD_DEBUG (MGMD_DEBUG_TIMERS," source timeleft = 0");
        if (SLLDelete(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not delete the Source node from the source List\n");
          MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Deleting group entry");
          MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Group entry chnaged to INCLUDE mode");
          return;
        }

        continue;
      }
      else
      {
        MGMD_DEBUG (MGMD_DEBUG_TIMERS," Timer > 0");
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
        /* Found a src record with > 0 timeout, which is an INCLUDE record */
        delGroup = L7_FALSE;
      }
      prev = srcRec;
    }    /*End-of-for*/

    MGMD_DEBUG (MGMD_DEBUG_TIMERS," delGroup = %d", delGroup);

    if (delGroup == L7_TRUE)
    {
      /* Send Group Timer Expiry Event to PROXY*/
      if (proxy_status == L7_ENABLE)
      {
        L7_inet_addr_t       *proxySrcList = mgmdCB->querySrcList;
        L7_uint32             proxySrcCount = 0, index;
        /* Initialize tmpSrc */
        for (index=0; index < MGMD_MAX_QUERY_SOURCES; index++)
        {
          inetAddressZeroSet (mgmdCB->proto, &proxySrcList[index]);
        }
        source = 0;
        inetAddressSet(mgmdCB->proto, &source, &tmpSrc);

        for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
            srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec ))
        {
          proxySrcList[proxySrcCount] = srcRec->sourceAddress;
          proxySrcCount++;
        }

        mgmd_host_info->router_timer_event(mgmdCB,
                                           L7_IGMP_PROXY_GRP_TIMER_EXPIRY, rtrIfNum,
                                           mgmd_group->group, tmpSrc,proxySrcCount,proxySrcList);
      }
      else
      {  /* Send Membership Event to Registerted MRP protocols.

          This event notifies all the sources which are in exclude list
          so that the MRPs can update states for these (S,G)s */
        mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                          rtrIfNum, mgmd_group, &group_info);
      }

      /* Send Membership Event to Registerted MRP protocols.
         Here the group state is notified , irrespective of sources*/
      mgmd_group_delete(mgmdCB,mgmd_group);
    }
    else
    {
      if (proxy_status == L7_ENABLE)
      {
        L7_inet_addr_t       *proxySrcList = mgmdCB->querySrcList;;
        L7_uint32             proxySrcCount = 0, index;

        for (index=0; index < MGMD_MAX_QUERY_SOURCES; index++)
        {
          inetAddressZeroSet (mgmdCB->proto, &proxySrcList[index]);
        }

        for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
            srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec ))
        {
          proxySrcList[proxySrcCount] = srcRec->sourceAddress;
          proxySrcCount++;
        }
        mgmd_host_info->router_timer_event(mgmdCB,
                                             L7_IGMP_PROXY_GRP_TIMER_EXPIRY, rtrIfNum,
                                             mgmd_group->group, tmpSrc,proxySrcCount,proxySrcList);
      }
      else
      {

      /* Send Membership Event to Registerted MRP protocols for states of
         all sources present with the existing group mode itself i.e EXCLUDE ,
         as states of the exclude sources will not handled if the group state
         is sent as new changed mode i.e INCLUDE. And send a separate event
         for group mode change. */
      mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                        rtrIfNum, mgmd_group, &group_info);
      }

      /* Switch group to INCLUDE mode and notify the same to MRPs */
      mgmd_group->filterMode = MGMD_FILTER_MODE_INCLUDE;

      mgmd_group->mrpUpdated |= MGMD_GROUP_MODE_UPDATED;

      if (proxy_status != L7_ENABLE)
      {
        memset(&group_info, 0 , sizeof(mgmdGroupInfo_t));
        /* Send a separate event for MRPs for group mode change */
        mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                        rtrIfNum, mgmd_group, &group_info);
      }


    }
  }
}

/*********************************************************************
* @purpose  Sources Timeout Event Handler
*
* @param    param      @b{ (input) }  mgmd Control Block
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_timeout_sources_event_handler(void *param)
{
  mgmd_cb_t            *mgmdCB ;
  mgmd_source_record_t *srcRec;
  L7_uint32             source = 0;
  L7_inet_addr_t        tmpSrc;
  mgmd_host_info_t     *mgmd_host_info ;
  mgmdGroupInfo_t       group_info;
  L7_uint32             rtrIfNum;
  mgmd_info_t          *mgmd_info;
  mgmd_group_t         *mgmd_group = L7_NULLPTR;
  L7_int32      handle = (L7_int32)param;
  mgmd_source_timer_data_t *timerData;
  L7_uint32             proxy_status;

  MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Entered ");


  timerData = (void*)handleListNodeRetrieve(handle);

  if (timerData == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Handle retrieve failed ");
    return;
  }
  srcRec= timerData->srcRec;
  mgmd_group = timerData->groupRec;

  mgmdCB = srcRec->mgmdCB;

  if (mgmdCB == L7_NULLPTR)
    return;

  mgmd_host_info    = mgmdCB->mgmd_host_info;
  proxy_status = mgmdCB->mgmd_proxy_status;

  rtrIfNum = mgmd_group->rtrIfNum;
  mgmd_info = (mgmd_info_t*)&(mgmdCB->mgmd_info[rtrIfNum]);

  srcRec->sourceTimer = L7_NULLPTR;

  /* Initialize group_info. */
  memset(&group_info, 0, sizeof(mgmdGroupInfo_t));
  group_info.numSrcs = 0;

  if (mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
  {
    mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_DELETE);
    if (SLLDelete(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not delete the Source node from the source List\n");
      return;
    }
  }
  else
  {
    /* 6.3: If source timed out in EXCLUDE mode, keep the record but tell cache */
    mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_EXCLUDE, MGMD_SOURCE_ADD);
  }

  if (proxy_status == L7_ENABLE)
  {
      mgmd_host_info->router_timer_event(mgmdCB,
                                       L7_IGMP_PROXY_SRC_TIMER_EXPIRY, rtrIfNum,
                                       mgmd_group->group, srcRec->sourceAddress,0,NULL);
  }
  /* Send Membership Event to Registerted MRP protocols.*/
  else if ((mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
      ((group_info.numSrcs > 0) &&
       (mgmd_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)))
  {
    /* NOTE: For GROUP_ADD event if(group_info.numSrcs==0) then it is Group_Add
     * event else it is a Src_Add event. */
    mgmd_frame_event_info_and_notify (mgmd_info->mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      rtrIfNum, mgmd_group, &group_info);
  }


  /* If filter mode is include and no source records are left, delete the group record */
  if ((mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE) &&
      (SLLNumMembersGet(&(mgmd_group->sourceRecords)) == 0))
  {
    /* deleting a avl item while looping */
    /* Initialize tmpSrc */
    source = 0;
    inetAddressSet(mgmdCB->proto, &source, &tmpSrc);

    /* Send Group Timer Expiry Event to PROXY*/
    if (proxy_status == L7_ENABLE)
      mgmd_host_info->router_timer_event(mgmdCB,
                                         L7_IGMP_PROXY_GRP_TIMER_EXPIRY,
                                         rtrIfNum, mgmd_group->group, tmpSrc,0,NULL);
    /* Delete the group which is in INCLUDE state.*/
     mgmd_group_delete(mgmdCB,mgmd_group);
  }
}

/*********************************************************************
* @purpose  Send a MGMD query
*
* @param    mgmd_info   @b{ (input) }   MGMD Info
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t mgmd_query_send (mgmd_info_t *mgmd_info)
{
  L7_inet_addr_t  zeroAddr;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  inetAddressReset(&zeroAddr);

  return(mgmd_group_query_send(mgmd_info, zeroAddr, mgmd_info->mgmdCB->all_hosts,
                               L7_NULLPTR, 0, 0, 0));
}


/*********************************************************************
* @purpose  Querier Timeout Event Handler
*
* @param    param      @b{ (input) }  MGMD Info
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_querier_timeout_event_handler(void *param)
{
  mgmd_info_t   *mgmd_info;
  mgmd_cb_t     *mgmdCB;
  L7_uint32      rtrIfNum;
  L7_uint32      timeLeft;
  L7_uchar8      inetAddrStr[IPV6_DISP_ADDR_LEN];
  L7_uint32      queryInterval;
  L7_int32      handle = (L7_int32)param;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  mgmd_info= (void*)handleListNodeRetrieve(handle);


  if ((mgmd_info == L7_NULLPTR))
  {
    return;
  }

  mgmdCB = mgmd_info->mgmdCB;
  rtrIfNum = mgmd_info->ifIndex;

  /* this happens when a querier was chosen after timer fired */
  /* timer is running */
  if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_info->mgmd_querier_timer, &timeLeft) == L7_SUCCESS)
  {
    if (timeLeft < MGMD_ZERO)
      return;
  }

  inetAddrHtop(&mgmd_info->mgmd_querierIP, inetAddrStr);
  MGMD_DEBUG (MGMD_DEBUG_QUERY, "TR_INFO: timed out querier %s on %d\n", inetAddrStr, mgmd_info->ifIndex);

  /* Get the primary IP address for the internal interface number */
  if (mcastIpMapIpAddressGet(mgmd_info->mgmdCB->proto, mgmd_info->ifIndex,
                             &(mgmd_info->mgmd_querierIP)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the Querier IP Address \n");
    return;
  }

  MGMD_DEBUG (MGMD_DEBUG_QUERY, "I M the Querier now in iface = %d\n", mgmd_info->ifIndex);

  MCAST_BITX_SET (mgmd_info->flags.bits, IGMP_QUERIER);
  /* Update the Querier filter mask. */
  BITX_SET (&mgmdCB->mgmd_querier_if_mask, rtrIfNum);
  /* Send Async Querier Update Event */
  if (mgmdCB->mgmd_proxy_status ==L7_ENABLE)
  {
    mgmd_proxy_querier_update_event_send(mgmdCB, rtrIfNum, L7_TRUE);
  }

  if (mgmd_query_send(mgmd_info) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Send the Query Message \n");
    return;
  }

  queryInterval = mgmd_info->intfConfig.queryInterval;

  mgmd_info->mgmd_query_timer = L7_NULLPTR;
  mgmd_info->querierQueryInterval = 0; /* when the router itself is querier, reset it
                                          so that the default value is used */

  if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_query_timer_handle,
                          &mgmd_info->mgmd_query_timer,
                          queryInterval, L7_MGMD_QUERY_TIMER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Query Timer\n");
    return;
  }

  ++mgmd_info->mgmd_query_count;
  mgmd_info->querierUpTime = osapiUpTimeRaw();

  inetAddrHtop(&mgmd_info->mgmd_querierIP, inetAddrStr);
  MGMD_DEBUG (MGMD_DEBUG_QUERY, "TR_INFO: new querier %s on %d\n", inetAddrStr, mgmd_info->ifIndex);
}


/*********************************************************************
* @purpose  Query Timer Expiry Handler
*
* @param    param   @b{ (input) }   MGMD Info
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mgmd_query_timeout_event_handler(void *param)
{
  mgmd_info_t   *mgmd_info;
  mgmd_cb_t   *mgmdCB;
  L7_uint32    rtrIfNum;
  L7_uint32    timeLeft;
  L7_uint32    startupQueryCount;
  L7_uint32    startupQueryInterval;
  L7_uint32    queryInterval;
  L7_int32      handle = (L7_int32)param;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  mgmd_info= (void*)handleListNodeRetrieve(handle);


  if (mgmd_info == L7_NULLPTR)
  {
    return;
  }

  mgmdCB = mgmd_info->mgmdCB;
  rtrIfNum = mgmd_info->ifIndex;

  if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_info->mgmd_query_timer, &timeLeft) == L7_SUCCESS &&
      ((mgmd_info->mgmd_query_timer == L7_NULLPTR) || (timeLeft <= 0)))
    return;

  /* send query only if it is a querier*/
  if (MCAST_BITX_TEST (mgmd_info->flags.bits, IGMP_QUERIER))
  {
    if (mgmd_query_send(mgmd_info) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Send the Query Message \n");
    }

    ++mgmd_info->mgmd_query_count;

    startupQueryCount = mgmd_info->intfConfig.startupQueryCount;

    mgmd_info->mgmd_query_timer= L7_NULLPTR;

    if (mgmd_info->mgmd_query_count >= startupQueryCount)
    {
      queryInterval = mgmd_info->intfConfig.queryInterval;

      if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_query_timer_handle,
                              &mgmd_info->mgmd_query_timer,
                              queryInterval, L7_MGMD_QUERY_TIMER) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Query Timer\n");
        return;
      }


    }
    else if (mgmd_info->mgmd_query_count < startupQueryCount)
    {
      startupQueryInterval = mgmd_info->intfConfig.startupQueryInterval;
      if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_query_timer_handle,
                              &mgmd_info->mgmd_query_timer,
                              startupQueryInterval, L7_MGMD_QUERY_TIMER) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Grp Timer\n");
        return;
      }


    }
  }
}

/*********************************************************************
* @purpose  enable/disable Global Admin Mode.
*
* @param    mgmdCB        @b{ (input) } MGMD control block
* @param    gblAdminMode  @b{ (input) } MGMD admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static
L7_RC_t mgmd_global_mode_set(mgmd_cb_t *mgmdCB, L7_uint32 gblAdminMode)
{

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD: Control Block is NULL");
    return L7_FAILURE;
  }

  MGMD_DEBUG (MGMD_DEBUG_EVENTS, "INFO : Event Global Admin Mode Set - %d", gblAdminMode);

  if (gblAdminMode == L7_ENABLE)
  {
    /* Create the MRP Timer's Handle List Node */
    /* MRP Updation Timer will will be created when Reports are received */
    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing MRP timer handle ");
    mgmdCB->mgmd_mrp_timer_handle = handleListNodeStore (mgmdCB->handle_list, mgmdCB);
    if (mgmdCB->mgmd_mrp_timer_handle == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD: Could not Allocate MRP Handle List Node");
      return L7_FAILURE;
    }
  }
  else if (gblAdminMode == L7_DISABLE)
  {
    /* Destroy the MRP Updation Timer.  Handle List Node is also released
     * as part of this.
     */

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting MRP timer handle ");
    if (mgmdUtilAppTimerHandleDelete(mgmdCB,&mgmdCB->mgmd_mrp_timer, &mgmdCB->mgmd_mrp_timer_handle) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD: Could not Delete the L7_MGMD_MRP_INFORM_TIMER Timer");
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD: Could not Delete the L7_MGMD_MRP_INFORM_TIMER Timer");
      return L7_FAILURE;
    }
  }
  else
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD: Could not Delete the L7_MGMD_MRP_INFORM_TIMER Timer");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  turn on/off the interface
*
* @param    mgmdCB    @b{ (input) }  MGMD control block
* @param    rtrIfNum  @b{ (input) }  router interface number
* @param    intfType  @b{ (input) }  MGMD/PROXY interface
* @param    mode      @b{ (input) }  Admin Mode.(L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static
L7_RC_t mgmd_intf_mode_process(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum,
                               L7_uint32 intfType,L7_uint32  mode,
                               mgmdIntfConfig_t *mgmdIntfInfo)
{
  L7_uint32          intIfNum = 0;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  if (intfType == MGMD_PROXY_INTERFACE && mode == L7_ENABLE)
    mgmdCB->mgmd_proxy_ifIndex = rtrIfNum;

  if (mgmdIntfInfo == L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE," Invalid interface config info for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  if (mgmdCB->proto == L7_AF_INET6)
  {
    if (mcastIpMapRtrIntfToIntIfNum(mgmdCB->proto, rtrIfNum, &intIfNum) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not Map the Rtr Inter face interface number \n");
      return L7_FAILURE;
    }
    if (mgmdV6SockChangeMembership(rtrIfNum,intIfNum,mode,
                                   mgmdCB->sockfd) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_FLEX_MGMD_MAP_COMPONENT_ID,
               "MLDv2 GroupAddr-[FF02::16] Enable with Interpeak Stack Failed; "
               "rtrIfNum - %d, intf - %d, %s"
               " Registration of this Group address with the Interpeak stack failed."
               " As a result of this, MLDv2 packets will not be received at the"
               " application.", rtrIfNum, intIfNum, ifName);
    }
  }

  if (intfType == MGMD_PROXY_INTERFACE)
  {
    mgmd_proxy_interface(mgmdCB, rtrIfNum, mode, mgmdIntfInfo);
  }
  else if (intfType == MGMD_ROUTER_INTERFACE)
  {
    mgmd_router_interface(mgmdCB, rtrIfNum, mode, mgmdIntfInfo);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Clear the group info when an igmp interface is down
*
* @param    mgmdCB      @b{ (input) }  mgmd Control Block
* @param    rtrIntf     @b{ (input) }  rtrIntf on which MGMD needs to turned ON / OFF
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

void mgmd_group_info_delete(mgmd_cb_t *mgmdCB, L7_uint32 rtrIntfNum)
{
  mgmd_group_t dummyGroup, *mgmdGroup;
  mgmdGroupInfo_t  groupInfo;
  mgmd_source_record_t *srcRec = L7_NULLPTR;
  L7_uint32 timeLeft = 0;

  MGMD_DEBUG (MGMD_DEBUG_EVENTS, " Entered");

  memset(&dummyGroup, 0, sizeof(mgmd_group_t));

  /* Parse through all the group entries learnt from disabled interface */

  while (L7_NULLPTR != (mgmdGroup = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership),
                                                                  (void *)(&dummyGroup), AVL_NEXT)))
  {
    memcpy(&dummyGroup, mgmdGroup, sizeof(mgmd_group_t));

    if (mgmdGroup->rtrIfNum != rtrIntfNum)
    {
      continue;
    }

    if (mgmdCB->mgmd_proxy_status != L7_ENABLE)
    {
      /* Notify the MRPs the source state for the (S,G) entries
         as the mgmd_group_delete() notifies group state   */
      memset(&groupInfo, 0 ,sizeof(mgmdGroupInfo_t));
      for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmdGroup->sourceRecords)); srcRec != L7_NULL;
          srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmdGroup->sourceRecords), (L7_sll_member_t *)srcRec ))
      {
        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if ((appTimerTimeLeftGet(mgmdCB->timerHandle,srcRec->sourceTimer, &timeLeft) == L7_SUCCESS)
              && (timeLeft > 0))
          {
            mgmd_src_info_for_mrp_add(&groupInfo, srcRec->sourceAddress,
                                      MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_DELETE);
          }
        }
        else
        {
          mgmd_src_info_for_mrp_add(&groupInfo, srcRec->sourceAddress,
                                    MGMD_FILTER_MODE_EXCLUDE, MGMD_SOURCE_DELETE);
        }
      }
      mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                        mgmdGroup->rtrIfNum, mgmdGroup, &groupInfo);

    }
   /* Delete the group */
    mgmd_group_delete(mgmdCB, mgmdGroup);

  }/*End-of-While*/

}
/*********************************************************************
* @purpose  turn on/off the router interface
*
* @param    mgmdCB      @b{ (input) }  mgmd Control Block
* @param    rtrIntf     @b{ (input) }  rtrIntf on which MGMD needs to turned ON / OFF
* @param    mode        @b{ (input) }  mode - ENABLE / DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t mgmd_router_interface(mgmd_cb_t *mgmdCB, L7_uint32 rtrIntf, L7_uint32 mode,
                                     mgmdIntfConfig_t *mgmdIntfInfo)
{
  L7_uchar8        inetAddrStr[IPV6_DISP_ADDR_LEN];

  MGMD_DEBUG (MGMD_DEBUG_EVENTS," Entered, mode =%d , rtrIfNum = %d",
              mode, rtrIntf);

  if (mode == L7_ENABLE)
  {
    mgmd_info_t*       mgmd_info;
    L7_uint32          robustnessvar;
    L7_uint32          queryInterval;
    L7_uint32          responseInterval;
    L7_uint32          startupQueryCount;
    mgmdIntfConfig_t   intfConfig;

    mgmd_info = &(mgmdCB->mgmd_info[rtrIntf]);
    memset(mgmd_info, 0 , sizeof(mgmd_info_t));
    inetAddressZeroSet(mgmdCB->proto, &mgmd_info->mgmd_querierIP);

    /* Populate the mgmd_info variables */
    mgmd_info->ifIndex = rtrIntf;
    mgmd_info->Wrongverqueries = 0;
    mgmd_info->numOfJoins = 0;
    mgmd_info->numOfGroups = 0;
    mgmd_info->querierUpTime = osapiUpTimeRaw();
    mgmd_info->mgmdCB = mgmdCB;

    /*
       mgmdMapProtocolInterfaceVersionGet function gives a version value of 1 for MLDv1 and  2 for MLDV2 if
       the protocol family type is L7_AF_INET6.But from the protocol functionality point of view IGMPv3 and MLDv2
       should result in a value of 3 and IGMPv2 and MLD1 sould result in a value of 2.And so for the
       family type L7_AF_INET6 mgmdVersion field value should be incremented before being assigned to
       mgmd_info->operatingVersion. The parameter mgmdIntfInfo already took care of this.

    */
    memcpy(&mgmd_info->intfConfig, mgmdIntfInfo, sizeof(mgmdIntfConfig_t));

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing Querier handle ");
    if ((mgmd_info->mgmd_querier_timer_handle = handleListNodeStore(
                      mgmd_info->mgmdCB->handle_list,mgmd_info)) == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS,"Could not get the handle node to store the timer data.\n");
      return L7_FAILURE;
    }
    memcpy(&intfConfig ,&mgmd_info->intfConfig, sizeof(mgmdIntfConfig_t));
    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing Query handle ");
    if ((mgmd_info->mgmd_query_timer_handle = handleListNodeStore(
                      mgmdCB->handle_list,mgmd_info)) == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS,"Could not get the handle node to store the timer data.\n");
      return L7_FAILURE;
    }

    robustnessvar = intfConfig.robustness;
    queryInterval = intfConfig.queryInterval;
    responseInterval = intfConfig.queryResponseInterval;
    startupQueryCount = intfConfig.startupQueryCount;

    MGMD_DEBUG (MGMD_DEBUG_EVENTS," 1:operating version = %d",mgmd_info->intfConfig.version);

    /*refer 8.5 and 8.3 of RFC 2236 - Query Response interval is 1/10th units i.e (100uints = 10 secs) */
    if (SLLCreate (L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_NO_ORDER, L7_NULL,
                   mgmd_query_req_compare, mgmd_query_req_destroy,
                   &(mgmd_info->ll_query_reqs)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "ll_query_reqs SLL Creation Failed");
      return L7_FAILURE;
    }

    MGMD_DEBUG (MGMD_DEBUG_QUERY, "Initially, i m Querier on iface=%d\n", mgmd_info->ifIndex);

    MCAST_BITX_SET (mgmd_info->flags.bits, IGMP_QUERIER);
    /* Update the Querier filter mask. */
    BITX_SET (&mgmdCB->mgmd_querier_if_mask, rtrIntf);
    mgmd_info->querierQueryInterval = queryInterval;
    /* Send Async Querier Update Event */
    if (mgmdCB->mgmd_proxy_status == L7_ENABLE)
    {
      mgmd_proxy_querier_update_event_send(mgmdCB, rtrIntf, L7_TRUE);
    }

    if (mcastIpMapIpAddressGet(mgmdCB->proto, rtrIntf, &(mgmd_info->mgmd_querierIP)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the Querier IP Address \n");
      return L7_FAILURE;
    }

    inetAddrHtop(&mgmd_info->mgmd_querierIP, inetAddrStr);
    MGMD_DEBUG (MGMD_DEBUG_EVENTS, "TR_INFO: new querier %s on %d", inetAddrStr, mgmd_info->ifIndex);

    mgmd_info->mgmd_query_count = 0;

    if (mgmd_info->mgmd_query_count < startupQueryCount)
    {
      if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmd_info->mgmd_query_timer_handle,
                              &mgmd_info->mgmd_query_timer,
                              MGMD_STARTUP_QUERY_DELAY, L7_MGMD_QUERY_TIMER) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not start the Query Timer\n");
        return L7_FAILURE;
      }
    }
    MGMD_DEBUG (MGMD_DEBUG_EVENTS," 2: operating version = %d",mgmd_info->intfConfig.version);
  }
  else
  { /* (mode == L7_DISABLE) */
    mgmd_info_t *mgmd_info;

    mgmd_info = &(mgmdCB->mgmd_info[rtrIntf]);
    if (mgmd_info != L7_NULLPTR)
    {
      MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting Query timer handle ");
      if (mgmdUtilAppTimerHandleDelete(mgmdCB,&mgmd_info->mgmd_query_timer,&mgmd_info->mgmd_query_timer_handle) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Delete the query Timer node \n");
      }

      MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting Querier timer handle ");
      if (mgmdUtilAppTimerHandleDelete(mgmdCB,&mgmd_info->mgmd_querier_timer,&mgmd_info->mgmd_querier_timer_handle) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Delete the Querier Timer node \n");
      }

      if (SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_info->ll_query_reqs)) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not destroy the ll_query_reqs SLL.\n");
      }

      memset(mgmd_info, 0, sizeof(mgmd_info_t));

      /* Update proxy table when an igmp-interface goes down */
      if (mgmdCB->mgmd_proxy_status == L7_ENABLE)
      {
        mgmd_proxy_intf_down_chk(mgmdCB, rtrIntf);
      }
      /* Delete all group info learnt from that interface */
      mgmd_group_info_delete(mgmdCB, rtrIntf);
      mgmdIntfStatsClear(mgmdCB->proto, rtrIntf);
      MGMD_DEBUG (MGMD_DEBUG_EVENTS," After disable : version = %d ", mgmd_info->intfConfig.version);
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Handles version change on an mgmd interface
*
* @param    mgmdCB    @b{ (input) }  MGMD control block
* @param    rtrIfNum  @b{ (input) }  router interface number
* @param    version   @b{ (input) }  interface version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Whenever a version has been changed on an interface, the
*           mgmd router on that interface is restarted with new version.
*
* @end
*********************************************************************/
L7_RC_t mgmd_interface_version_set(mgmd_cb_t  *mgmdCB,
                                   L7_uint32  rtrIfNum,
                                   L7_uint32  version)
{
  L7_RC_t   rc;
  mgmd_info_t *mgmd_info;
  mgmdIntfConfig_t mgmdIntfInfo;

  MGMD_DEBUG (MGMD_DEBUG_EVENTS,"Entered, rtrIfnum = %d",rtrIfNum );

  mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);

  MGMD_DEBUG (MGMD_DEBUG_EVENTS,"Existing version = %d , new Version =%d",
              mgmd_info->intfConfig.version, version);

  if (mgmd_info->intfConfig.version == version)
  {
    MGMD_DEBUG (MGMD_DEBUG_EVENTS,"Version = %d is already set on rtrIfNum ",
                version,rtrIfNum);
    return L7_SUCCESS;
  }
  mgmd_info->intfConfig.version = version;
  memcpy (&mgmdIntfInfo, &mgmd_info->intfConfig, sizeof(mgmdIntfConfig_t));

  rc = mgmd_router_interface(mgmdCB, rtrIfNum, L7_DISABLE, &mgmdIntfInfo);
  if (rc == L7_SUCCESS)
  {
    rc = mgmd_router_interface(mgmdCB, rtrIfNum, L7_ENABLE, &mgmdIntfInfo);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  V1 HOST TIMER expiry Handler
*
* @param    mgmd_group  @b{ (input) }   Group for which the timer expired
*
* @returns  Nothing
*
* @notes
*
* @end
*********************************************************************/
static void mgmd_v1host_timeout_event_handler(void *param)
{
  L7_int32      handle = (L7_int32)param;
  mgmd_group_t *mgmd_group;

  MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Entered ");

  mgmd_group= (mgmd_group_t *)handleListNodeRetrieve(handle);

  if (mgmd_group == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Failed to find group entry");
    return;
  }

  mgmd_group->v1HostTimer = L7_NULLPTR;

  ((mgmd_group_t *)mgmd_group)->v1HostPresentFlag = L7_FALSE;

  if (mgmd_group->v2HostPresentFlag == L7_TRUE)
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_2;
  else
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_3;
}

/*********************************************************************
*
* @purpose  V2 HOST TIMER expiry Handler
*
* @param    mgmd_group  @b{ (input) }   Group for which the timer expired
*
* @returns  Nothing
*
* @notes
*
* @end
*********************************************************************/
static void mgmd_v2host_timeout_event_handler(void *param)
{

  L7_int32      handle = (L7_int32)param;
  mgmd_group_t *mgmd_group;

  MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Entered ");

  mgmd_group= (mgmd_group_t *)handleListNodeRetrieve(handle);


  if (mgmd_group == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Failed to find group entry");
    return;
  }


  mgmd_group->v2HostTimer = L7_NULLPTR;

  mgmd_group->v2HostPresentFlag = MGMD_ZERO;

  if (mgmd_group->v1HostPresentFlag == MGMD_ONE)
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_1;
  else
    mgmd_group->groupCompatMode = L7_MGMD_VERSION_3;
}

/*********************************************************************
*
* @purpose  MGMD Event process.
*
* @param    familyType  @b{ (input) } address family.
* @param    eventType   @b{ (input) } Event.
* @param    pMsg        @b{ (input) } Message.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_event_process (L7_uchar8 familyType,
                            mcastEventTypes_t eventType, void *pMsg)
{
  mgmd_cb_t *mgmdCB = L7_NULLPTR;

  if(eventType != MCAST_EVENT_MGMD_TIMER_EXPIRY_EVENT)
  {
    if (mgmdMapProtocolCtrlBlockGet(familyType,
                                    (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
    {

      MGMD_DEBUG(MGMD_DEBUG_FAILURE,"MGMD:Invalid Control Block\r\n");
      return L7_FAILURE;
    }

  }

  /* Process Events */
  switch (eventType)
  {
    case MCAST_EVENT_IGMP_CONTROL_PKT_RECV:   /* IPv4 Control Pkt Recv Event*/
      {
        mcastControlPkt_t *ctrlPkt = (mcastControlPkt_t *)pMsg;
        /* Copy the received packet to a local buffer and Free the MCAST Buffer */
        memset (mgmdCB->pktRxBuf, 0, MGMD_PKT_SIZE_MAX);
        memcpy (mgmdCB->pktRxBuf, (L7_uchar8 *) ctrlPkt->payLoad, MGMD_PKT_SIZE_MAX);
        mcastCtrlPktBufferPoolFree(mgmdCB->proto,ctrlPkt->payLoad);
        ctrlPkt->payLoad = mgmdCB->pktRxBuf;
        mgmd_receive(ctrlPkt);
        break;
      }
    case MCAST_EVENT_MGMD_TIMER_EXPIRY_EVENT: /* MGMD Appn Timer Expiry Event */
      {
        appTimerProcess(pMsg);
        break;
      }

    case MCAST_EVENT_MLD_CTRL_PKT_RECV:       /* IPv6 Control Pkt Recv Event */
      {
        mcastControlPkt_t *ctrlPkt = (mcastControlPkt_t *)pMsg;
        if (mgmd_ipv6_pkt_receive(ctrlPkt) != L7_SUCCESS)
        {
          MGMD_DEBUG(MGMD_DEBUG_FAILURE, "MLD validation failed ");
          mcastCtrlPktBufferPoolFree(mgmdCB->proto,ctrlPkt->payLoad);
          break;
        }
        /* Copy the received packet to a local buffer and Free the MCAST Buffer */
        memset (mgmdCB->pktRxBuf, 0, MGMD_PKT_SIZE_MAX);
        memcpy (mgmdCB->pktRxBuf, (L7_uchar8 *) ctrlPkt->payLoad, MGMD_PKT_SIZE_MAX);
        mcastCtrlPktBufferPoolFree(mgmdCB->proto,ctrlPkt->payLoad);
        ctrlPkt->payLoad = mgmdCB->pktRxBuf;
        mgmd_receive(ctrlPkt);
        break;
      }

    case MCAST_MFC_NOCACHE_EVENT:             /* MFC No Cache Entry Event */
    case MCAST_MFC_ENTRY_EXPIRE_EVENT:        /* MFC Entry Expiry Event */
      {
        if (mgmdCB->mgmd_proxy_status == L7_ENABLE)
        {
          mgmdProxyMFCEventHandler (mgmdCB, eventType,(mfcEntry_t*)pMsg);
        }
        break;
      }

    case MCAST_EVENT_ADMINSCOPE_BOUNDARY:
      {
        if (mgmdCB->mgmd_proxy_status == L7_ENABLE)
        {
          mgmdProxyAdminScopeBoundaryEventHandler(mgmdCB, eventType,(mcastAdminMsgInfo_t *)pMsg);
        }
        break;
      }

    case MCAST_EVENT_MGMD_GROUP_INFO_GET:
      {

        mgmdMRPGroupInfoGet(mgmdCB, (L7_uint32*) pMsg);
        break;
      }

    default:
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:\nInvalid Event.\n");
      return L7_FAILURE;
      /* passthru */
  } /*End-of-Switch*/
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose   This function is used to send timer events
 *
 * @param     timerCtrlBlk    @b{ (input) }   Timer Control Block
 * @param     mgmdEB          @b{ (input) }   MGMD Execution Block
 *
 * @returns   None
 *
 * @notes     None
 * @end
 *********************************************************************/
void mgmdTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* mgmdEB)
{
  mgmdMapMessageQueueSend(L7_NULL, MCAST_EVENT_MGMD_TIMER_EXPIRY_EVENT,
                          sizeof(L7_APP_TMR_CTRL_BLK_t), timerCtrlBlk);
}

#if 0
/*********************************************************************
*
* @purpose  To start the  Timer
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*                 srcRec     Source record for which timer needs to be started
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t  mgmd_start_timer(mgmd_cb_t *mgmdCB,void *data, L7_int32 timerType,  L7_uint32  interval)
{

  mgmd_group_t *mgmd_group= NULL;
  mgmd_source_record_t *srcRec= NULL;
  mgmd_info_t*       mgmd_info = NULL;
  mgmd_query_req_t *qreq = NULL;
  mgmd_timer_event_info_t *info = NULL;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered, timerType = %d", timerType);

  switch (timerType)
  {
    case L7_MGMD_GRP_TIMER:
      {
        mgmd_group = data;
        mgmd_group->groupTimer= appTimerAdd(mgmdCB->timerHandle, mgmd_timeout_groups_event_handler,
                                            (void*)mgmd_group->grp_timer_handle, mgmd_group->interval);
        if (mgmd_group->groupTimer == NULL)
        {
          LOG_MSG(" MGMD:Could not Start the Group timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;
      }
      break;
    case L7_MGMD_SRC_TIMER:
      {
        srcRec = data;
        srcRec->sourceTimer = appTimerAdd(mgmdCB->timerHandle, mgmd_timeout_sources_event_handler,
                                          (void*)srcRec->src_timer_handle, srcRec->sourceInterval);

        if (srcRec->sourceTimer == NULL)
        {
          LOG_MSG(" MGMD:Could not Start the Source timer.\n");
          return L7_FAILURE;
        }

        return L7_SUCCESS;
      }
      break;
    case L7_MGMD_QUERY_TIMER:
      {
        mgmd_info =data;
        mgmd_info->mgmd_query_timer = appTimerAdd(mgmdCB->timerHandle, mgmd_query_timeout_event_handler,
                                                  (void *) mgmd_info->mgmd_query_timer_handle, interval);

        if (mgmd_info->mgmd_query_timer  == NULL)
        {
          LOG_MSG(" MGMD:Could not Start the Query timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;
      }
      break;
    case L7_MGMD_QUERIER_TIMER:
      {
        mgmd_info =data;
        mgmd_info->mgmd_querier_timer= appTimerAdd(mgmdCB->timerHandle, mgmd_querier_timeout_event_handler,
                                                   (void *) mgmd_info->mgmd_querier_timer_handle , interval);
        if (mgmd_info->mgmd_querier_timer == NULL)
        {
          LOG_MSG(" MGMD:Could not Start the Querier timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;

      }
      break;
    case L7_MGMD_V1HOST_TIMER:
      {
        mgmd_group = data;
        mgmd_group->v1HostTimer = appTimerAdd(mgmdCB->timerHandle, mgmd_v1host_timeout_event_handler,
                                              (void*)mgmd_group->v1host_timer_handle, interval);
        if (mgmd_group->v1HostTimer  == NULL)
        {

          LOG_MSG(" MGMD:Could not Start the V1HOST timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;

      }
      break;
    case L7_MGMD_V2HOST_TIMER:
      {
        mgmd_group = data;
        mgmd_group->v2HostTimer = appTimerAdd(mgmdCB->timerHandle, mgmd_v2host_timeout_event_handler,
                                              (void*)mgmd_group->v2host_timer_handle, interval);
        if (mgmd_group->v2HostTimer  == NULL)
        {
          LOG_MSG(" MGMD:Could not Start the V2HOST timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;
      }
      break;

    case L7_MGMD_GRP_QUERY_TIMER:
      {
        info = data;
        qreq = info->qreq;

        qreq->timer = appTimerAdd(mgmdCB->timerHandle, mgmd_group_query_timeout_event_handler,
                                  (void *) qreq->query_timer_handle, interval);
        if (qreq->timer  == NULL)
        {
          LOG_MSG(" MGMD:Could not Start the Grp Query timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;
      }
      break;

    case L7_MGMD_MRP_INFORM_TIMER:
      {
        mgmdCB->mgmd_mrp_timer =appTimerAdd(mgmdCB->timerHandle, mgmd_mrp_timeout_handler,
                                            (void*)mgmdCB->mgmd_mrp_timer_handle, interval);

        if (mgmdCB->mgmd_mrp_timer == L7_NULLPTR)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD:Could not Start mgmd_mrt_timer.\n");
          return L7_FAILURE;
        }
        return L7_SUCCESS;
      }
      break;
    default:
      break;

  }
  return L7_FAILURE;
}
#endif
/*********************************************************************
*
* @purpose  To Delete the  Timer
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*                 srcRec     Source record for which timer needs to be started
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t  mgmdUtilAppTimerHandleDelete(mgmd_cb_t *mgmdCB,L7_APP_TMR_HNDL_t *timer, L7_uint32 *handle)
{
  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered, timer = %d", timer);

  if (*timer != L7_NULLPTR)
  {
    if (appTimerDelete(mgmdCB->timerHandle, *timer ) != L7_SUCCESS)
    {
       MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Could not Delete the timer.\n");
      /*return L7_FAILURE;*/
    }
    *timer=L7_NULLPTR;
  }
  if (*handle != L7_NULL)
  {
    MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting Actual timer handle ");
    handleListNodeDelete(mgmdCB->handle_list,handle);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To create the group entry and add it into the membership table
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*           group     group for which entry is being added.
*           rtrIfIndex Router interface on which the group join recieved
*
* @returns  pointer to the group entry added.
*
* @notes
*
* @end
*********************************************************************/
static mgmd_group_t*  mgmd_group_create(mgmd_cb_t *mgmdCB,L7_inet_addr_t group,L7_uint32 rtrIfIndex)
{

  mgmd_group_t    mgmd_new_group;
  mgmd_group_t    *mgmd_group=L7_NULLPTR;

  memset(&mgmd_new_group,0,sizeof(mgmd_new_group));

  /* Create linked list of source records for this group */
  SLLCreate(L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_ASCEND_ORDER, sizeof(L7_inet_addr_t),
            mgmd_source_record_comp, mgmd_source_record_destroy, &(mgmd_new_group.sourceRecords));

  mgmd_new_group.rtrIfNum = rtrIfIndex;
  mgmd_new_group.group = group;
  mgmd_new_group.ctime = osapiUpTimeRaw();

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not take the semaphore \n");
    return L7_NULLPTR;
  }

  if (avlInsertEntry(&(mgmdCB->membership), &mgmd_new_group) == L7_NULL)
  {
    /* Entry was inserted. Grab the pointer to the entry just inserted
       for further processing */
    mgmd_group = avlSearchLVL7(&(mgmdCB->membership), &mgmd_new_group, AVL_EXACT);
    if (mgmd_group == NULL)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failure in retrieving the Group Node \n");
      osapiSemaGive(mgmdCB->membership.semId);
      SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_new_group.sourceRecords));
      return L7_NULLPTR;
    }

    mgmd_group->timerData.mgmdCb =mgmdCB;
    mgmd_group->timerData.groupRec = mgmd_group;

    mgmd_group->grpQueryData = L7_NULLPTR;
    mgmd_group->grpSrcQueryData = L7_NULLPTR;

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing Grp timer handle ");
    if ((mgmd_group->grp_timer_handle = handleListNodeStore(mgmdCB->handle_list,&mgmd_group->timerData)) == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the handle node to store the timer data and so deleting the created group\n");
      avlDeleteEntry (&(mgmdCB->membership), mgmd_group);
      osapiSemaGive(mgmdCB->membership.semId);
      SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_new_group.sourceRecords));
      return L7_NULLPTR;
    }

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing V1 host timer handle ");
    if ((mgmd_group->v1host_timer_handle = handleListNodeStore(mgmdCB->handle_list,mgmd_group)) == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the handle node to store the timer data and so deleting the created group\n");
      avlDeleteEntry (&(mgmdCB->membership), mgmd_group);
      osapiSemaGive(mgmdCB->membership.semId);
      SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_new_group.sourceRecords));
      return L7_NULLPTR;
    }

    MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing V2 host timer handle ");
    if ((mgmd_group->v2host_timer_handle = handleListNodeStore(mgmdCB->handle_list,mgmd_group)) == 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the handle node to store the timer data and so deleting the created group\n");
      avlDeleteEntry (&(mgmdCB->membership), mgmd_group);
      osapiSemaGive(mgmdCB->membership.semId);
      SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_new_group.sourceRecords));
      return L7_NULLPTR;
    }
    mgmdCB->mgmd_info[rtrIfIndex].numOfGroups++;
    osapiSemaGive(mgmdCB->membership.semId);
    return mgmd_group;

  }
  else
  {
    L7_uchar8 grp[IPV6_DISP_ADDR_LEN];

    L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_FLEX_MGMD_MAP_COMPONENT_ID,
             "MGMD Group Entry Creation Failed; grpAddr - %s, rtrIfNum - %d"
             "The specified Group Address registration on the specified"
             " router interface failed.", inetAddrPrint(&group,grp), rtrIfIndex);
    osapiSemaGive(mgmdCB->membership.semId);
    SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_new_group.sourceRecords));
    return L7_NULLPTR;
  }

}
/*********************************************************************
*
* @purpose  To cleanup the group entry and delete it
*
* @param    mgmdCB  @b{ (input)  MGMD Control Block
*           group   @b{ (input)  group for which entry is being deleted.
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
static void mgmd_group_delete(mgmd_cb_t *mgmdCB, mgmd_group_t *mgmd_group)
{
  mgmdGroupInfo_t  group_info;
  L7_uint32        rtrIfNum = mgmd_group->rtrIfNum;

  memset(&group_info, 0 ,sizeof(mgmdGroupInfo_t));

  /* Send Membership Event to Registerted MRP protocols.*/
  mgmd_group->filterMode = MGMD_FILTER_MODE_INCLUDE;
  mgmd_group->mrpUpdated |= MGMD_GROUP_MODE_UPDATED;

  mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                    mgmd_group->rtrIfNum, mgmd_group, &group_info);


  MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting Grp timer handle ");
  if (mgmdUtilAppTimerHandleDelete(mgmdCB, &mgmd_group->groupTimer, &mgmd_group->grp_timer_handle) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Delete the Grp Timer node \n");
  }

  MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting V1 host timer handle ");
  if (mgmdUtilAppTimerHandleDelete(mgmdCB, &mgmd_group->v1HostTimer, &mgmd_group->v1host_timer_handle) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Delete the v1 Host timer \n");
  }

  MGMD_DEBUG(MGMD_DEBUG_HANDLE," Deleting V2 host timer handle ");
  if (mgmdUtilAppTimerHandleDelete(mgmdCB, &mgmd_group->v2HostTimer, &mgmd_group->v2host_timer_handle) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Delete the v2 Host timer \n");
  }

  /* Delete the source list for group learned */
  if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_group->sourceRecords)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not destroy the sourceRecords SLL.\n");
  }

  if (mgmd_group->grpQueryData != L7_NULLPTR)
  {
    MGMD_FREE(mgmdCB->proto, mgmd_group->grpQueryData);
  }
  if (mgmd_group->grpSrcQueryData != L7_NULLPTR)
  {
    MGMD_FREE(mgmdCB->proto, mgmd_group->grpSrcQueryData);
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not take the semaphore \n");
    return;
  }
  if (avlDeleteEntry (&(mgmdCB->membership), mgmd_group) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD : AVL group Deletion failed \n");
  }
  else
  {
    mgmdCB->mgmd_info[rtrIfNum].numOfGroups--;
  }
  osapiSemaGive(mgmdCB->membership.semId);
}
/*********************************************************************
*
* @purpose  To create the source node and add it into src list
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*           mgmd_group group entry pointer into which source node  is being added.
*           source source address for which the entry is being added.
*
* @returns  pointer to the source record node.
*
* @notes
*
* @end
*********************************************************************/

static mgmd_source_record_t*  mgmd_source_create(mgmd_cb_t *mgmdCB,mgmd_group_t *mgmd_group,L7_inet_addr_t source)
{
  mgmd_source_record_t *srcRec = L7_NULLPTR;

  if ((srcRec = (mgmd_source_record_t *)MGMD_ALLOC(mgmdCB->proto, sizeof(mgmd_source_record_t))) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error getting memory for source record\n");
    return L7_NULLPTR;
  }
  memset(srcRec, 0, sizeof(mgmd_source_record_t));
  srcRec->sourceAddress = source;
  /* 6.4.1: (A)=GMI */
  srcRec->sourceCtime = osapiUpTimeRaw();
  srcRec->mgmdCB = mgmdCB;
  srcRec->timerData.srcRec = srcRec;
  srcRec->timerData.groupRec = mgmd_group;

  MGMD_DEBUG(MGMD_DEBUG_HANDLE," storing Src timer handle ");
  if ((srcRec->src_timer_handle = handleListNodeStore(mgmdCB->handle_list,&srcRec->timerData)) ==0)
  {
    MGMD_FREE(mgmdCB->proto,(srcRec));
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not get the handle node to store the timer data.\n");
    return L7_NULLPTR;
  }

  if (SLLAdd(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
  {
    MGMD_FREE(mgmdCB->proto,(srcRec));
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Could not add the Source Record to source List\n");
    return L7_NULLPTR;
  }

  return srcRec;
}
/*********************************************************************
* @purpose  Retrieve group information learned by MGMD
*
* @param    mgmdCB            @b{(input)}   MGMD Control Block
* @param    pRtrIfNum         @b{(input)}   router Interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is used by MRPs to get the group information learned
*           by MGMD whenever MRP is disabled and enabled at run-time
*
* @end
*********************************************************************/
L7_RC_t mgmdMRPGroupInfoGet(mgmd_cb_t *mgmdCB, L7_uint32 *pRtrIfNum)
{
  mgmd_group_t *mgmd_group = L7_NULLPTR;
  mgmd_group_t dummyGroup;
  L7_uint32 rtrIfNum = *pRtrIfNum;

  MGMD_DEBUG (MGMD_DEBUG_EVENTS, "MGMD Group Info Requested for intf - %d", rtrIfNum);

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD CB is NULL");
    return L7_FAILURE;
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: Could not take the samaphore\n");
    return L7_FAILURE;
  }

  dummyGroup.rtrIfNum = 0;
  if (inetAddressZeroSet(mgmdCB->proto, &dummyGroup.group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: InetCopy Failed. \n");
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }

  while ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &dummyGroup, AVL_NEXT)) != L7_NULLPTR)
  {
    mgmdGroupInfo_t group_info;
    mgmd_source_record_t *srcRec = L7_NULLPTR;

    dummyGroup.rtrIfNum = mgmd_group->rtrIfNum;
    if (inetCopy (&dummyGroup.group, &mgmd_group->group) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: InetCopy Failed. \n");
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }

    if (mgmd_group->rtrIfNum != rtrIfNum)
    {
      continue;
    }

    group_info.numSrcs =0;

    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
         srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      L7_uint32 timeLeft = 0;
      if (srcRec->sourceTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle,srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for src=",
                              &srcRec->sourceAddress);
        }
      }
      if (timeLeft > 0)
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
      else
        mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_EXCLUDE, MGMD_SOURCE_ADD);

      if(group_info.numSrcs >= MGMD_MAX_QUERY_SOURCES)
      {
        mgmd_group->mrpUpdated |= MGMD_MRP_REQUESTED_GROUP;
        mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                          rtrIfNum, mgmd_group, &group_info);
        group_info.numSrcs =0;
      }
    }

    mgmd_group->mrpUpdated |= MGMD_MRP_REQUESTED_GROUP;

    mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                      rtrIfNum, mgmd_group, &group_info);
  }

  osapiSemaGive(mgmdCB->membership.semId);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  mrp timeout handler
* @param    param  @b{ (input) } Timer Event Info Block
*
*
* @returns  none
*
* @notes    Called when MGMD MRP TIMER expires
*
* @end
*********************************************************************/
void mgmd_mrp_timeout_handler(void *param)
{
  mgmd_cb_t   *mgmdCB ;
  L7_uint32    handle = (L7_uint32)param, timeLeft = 0;
  mgmd_group_t dummyGroup, *mgmdGroup;

  mgmdCB = (void*)handleListNodeRetrieve(handle);

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_TIMERS,"Handle retrieve failed ");
    return;
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: Could not take the samaphore\n");
    return;
  }

  memset(&dummyGroup, 0, sizeof(mgmd_group_t));

  /* Parse through all the group entries learnt from disabled interface */

  while (L7_NULLPTR != (mgmdGroup = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership),
                                                                  (void *)(&dummyGroup), AVL_NEXT)))
  {

    memcpy(&dummyGroup, mgmdGroup, sizeof(mgmd_group_t));

    if((mgmdGroup->mrpUpdated & MGMD_MRP_INFORM_FAILED) != L7_NULL)
    {

      mgmdGroupInfo_t       group_info;
      mgmd_source_record_t *srcRec;
      group_info.numSrcs =0;


      for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmdGroup->sourceRecords)); srcRec != L7_NULL;
           srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmdGroup->sourceRecords), (L7_sll_member_t *)srcRec))
      {
        timeLeft = 0;
        if (srcRec->sourceTimer != L7_NULLPTR)
        {
          if (appTimerTimeLeftGet(mgmdCB->timerHandle,srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
          {
            MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for src=",
                                &srcRec->sourceAddress);
          }
        }
        if (timeLeft > 0)
        {
          mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_INCLUDE, MGMD_SOURCE_ADD);
        }
        else
        {
          mgmd_src_info_for_mrp_add(&group_info, srcRec->sourceAddress, MGMD_FILTER_MODE_EXCLUDE, MGMD_SOURCE_ADD);
        }

        if(group_info.numSrcs >= MGMD_MAX_QUERY_SOURCES)
        {
          mgmdGroup->mrpUpdated |= MGMD_MRP_INFORM_FAILED;
          mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                            mgmdGroup->rtrIfNum, mgmdGroup, &group_info);
          group_info.numSrcs =0;
        }
      }

      mgmdGroup->mrpUpdated |= MGMD_MRP_INFORM_FAILED;

      mgmd_frame_event_info_and_notify (mgmdCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                        mgmdGroup->rtrIfNum,mgmdGroup, &group_info);
    }

  }

  mgmdCB->mgmd_mrp_timer = L7_NULLPTR;

  osapiSemaGive(mgmdCB->membership.semId);

}
/*********************************************************************
* @purpose Function to Start the Timer (Both Set and update Timer)
*
* @param   mgmdCB   @b{(input)} Control Block.
*          pFunc     @b{(input)} Function Pointer.
*          pParam    @b{(input)} Function Parameter
*          timeout   @b{(input)} Timeout in seconds
*          tmrHandle     @b{(output)} Timer Handle
*
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t mgmdUtilAppTimerSet (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 timeOut,
                      MGMD_TIMER_t timerType)
{
  L7_char8 timerName[APPTIMER_STR_LEN];

  if (tmrHandle == L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Invalid tmrHandle for timer(%d) for timeout = %d",
               timerType, timeOut);
    return L7_FAILURE;
  }

  switch(timerType)
  {
  case L7_MGMD_GRP_TIMER:
       pFunc = mgmd_timeout_groups_event_handler;
       osapiStrncpySafe(timerName,"MG-GT", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_SRC_TIMER:
       pFunc = mgmd_timeout_sources_event_handler;
       osapiStrncpySafe(timerName,"MG-ST", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_QUERY_TIMER:
       pFunc = mgmd_query_timeout_event_handler;
       osapiStrncpySafe(timerName,"MG-QT", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_QUERIER_TIMER:
       pFunc = mgmd_querier_timeout_event_handler;
       osapiStrncpySafe(timerName,"MG-QRT", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_V1HOST_TIMER:
       pFunc = mgmd_v1host_timeout_event_handler;
       osapiStrncpySafe(timerName,"MG-V1H", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_V2HOST_TIMER:
       pFunc = mgmd_v2host_timeout_event_handler;
       osapiStrncpySafe(timerName,"MG-V2H", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_GRP_QUERY_TIMER:
       pFunc = mgmd_group_query_timeout_event_handler;
       osapiStrncpySafe(timerName,"MG-GQT", APPTIMER_STR_LEN);
       break;
  case L7_MGMD_MRP_INFORM_TIMER:
       pFunc = mgmd_mrp_timeout_handler;
       osapiStrncpySafe(timerName,"MG-MRP", APPTIMER_STR_LEN);
       break;
  default:
       MGMD_DEBUG(MGMD_DEBUG_TIMERS,
                  " Timer type(%d) has no pre-defined callback function", timerType);
       break;
  }
  MGMD_DEBUG(MGMD_DEBUG_TIMERS, "timerhandle = %p", tmrHandle);

  if(*tmrHandle != L7_NULL)
  {
    if (appTimerUpdate (mgmdCB->timerHandle,tmrHandle,pFunc,pParam,timeOut, timerName)
                        != L7_SUCCESS)
    {
      *tmrHandle = 0;
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to update timer(%d) for timeout = %d",
                 timerType, timeOut);
      return L7_FAILURE;
    }
  }
  else
  {
    if ((*tmrHandle = appTimerAdd (mgmdCB->timerHandle, pFunc, (void*) pParam,
                                   timeOut,timerName)) == L7_NULLPTR)
    {
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to start timer(%d) for timeout = %d",
                 timerType, timeOut);
      return L7_FAILURE;
    }
  }
  MGMD_DEBUG(MGMD_DEBUG_TIMERS, "returning success ");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Function to Start the Timer (Both Set and update Timer)
*
* @param   mgmdCB   @b{(input)} Control Block.
*          pFunc     @b{(input)} Function Pointer.
*          pParam    @b{(input)} Function Parameter
*          timeout   @b{(input)} Timeout in seconds
*          tmrHandle     @b{(output)} Timer Handle
*
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments    none
*
* @end
*********************************************************************/
static
L7_RC_t mgmdUtilAppTimerLower (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 lowerTimeOut,
                      MGMD_TIMER_t timerType)
{
  L7_uint32 timeLeft = 0;
  L7_char8 timerName[80];

  switch(timerType)
  {
  case L7_MGMD_GRP_TIMER:
       pFunc = mgmd_timeout_groups_event_handler;
       osapiStrncpySafe(timerName,"MGMD Group Timer2", 80);
       break;
  case L7_MGMD_SRC_TIMER:
       pFunc = mgmd_timeout_sources_event_handler;
       osapiStrncpySafe(timerName,"MGMD Source Timer2", 80);
       break;
  case L7_MGMD_QUERY_TIMER:
       pFunc = mgmd_query_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD Query Timer2", 80);
       break;
  case L7_MGMD_QUERIER_TIMER:
       pFunc = mgmd_querier_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD Querier Timer2", 80);
       break;
  case L7_MGMD_V1HOST_TIMER:
       pFunc = mgmd_v1host_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD V1-host Timer2", 80);
       break;
  case L7_MGMD_V2HOST_TIMER:
       pFunc = mgmd_v2host_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD V2-host Timer2", 80);
       break;
  case L7_MGMD_GRP_QUERY_TIMER:
       pFunc = mgmd_group_query_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD Group-Query Timer2", 80);
       break;
  case L7_MGMD_MRP_INFORM_TIMER:
       pFunc = mgmd_mrp_timeout_handler;
       osapiStrncpySafe(timerName,"MGMD MRP-Inform Timer2", 80);
       break;
  default:
       MGMD_DEBUG(MGMD_DEBUG_TIMERS,
                  " Timer type(%d) has no pre-defined callback function", timerType);
       break;
  }
  if(*tmrHandle != L7_NULL)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,tmrHandle, &timeLeft)
                        != L7_SUCCESS)
    {
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to get leftover time for timer(%d)",
                 timerType);
      return L7_FAILURE;
    }
    if (lowerTimeOut < timeLeft)
    {
      if (appTimerUpdate(mgmdCB->timerHandle, tmrHandle,pFunc,pParam,lowerTimeOut,
                         timerName)
                        != L7_SUCCESS)

      {
        MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to update timer(%d) for timeout = %d",
                   timerType, lowerTimeOut);
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To Delete the  AppTimer
*
* @param    mgmdCB          @b{ (input) } MGMD Control Block
*           timerHandle     @b{ (input) } time handle to be deleted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  mgmdUtilAppTimerDelete(mgmd_cb_t *mgmdCB,L7_APP_TMR_HNDL_t *timer)
{
  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered, timer = %d", timer);

  if (*timer != L7_NULLPTR)
  {
    if (appTimerDelete(mgmdCB->timerHandle, *timer ) != L7_SUCCESS)
    {
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, " Could not Delete the timer.\n");
      /*return L7_FAILURE;*/
    }
    *timer=L7_NULLPTR;
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Processes all configuration events
*
* @param    mgmd_map_events_params_t @b{ (input) } mapping event info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProtocolConfigSet(mgmdMapEventParams_t *eventParams)
{
  mgmd_cb_t *mgmdCB = L7_NULLPTR;
  L7_RC_t    rc = L7_FAILURE;
  mgmd_info_t *mgmd_info;
  L7_uint32 eventInfo;
  L7_uint32 rtrIfNum;
  L7_uint32 intfType;
  MGMD_UI_EVENT_TYPE_t eventType;

  if (eventParams == L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "Invalid eventParams \n");
    return L7_FAILURE;
  }
  eventInfo = eventParams->eventInfo;
  rtrIfNum = eventParams->rtrIfNum;
  intfType = eventParams->intfType;
  eventType = eventParams->eventType;

  MGMD_DEBUG(MGMD_DEBUG_EVENTS, " Family = %d , eventType = %d", eventParams->familyType,
             eventParams->eventType);

  MGMD_DEBUG(MGMD_DEBUG_EVENTS, " rtrIfNum = %d , intfType = %d", eventParams->rtrIfNum,
             eventParams->intfType);

  MGMD_DEBUG(MGMD_DEBUG_EVENTS, "eventInfo = %d", eventParams->eventInfo);
  if (eventParams->mgmdIntfInfo != L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : version = %d",
               eventParams->mgmdIntfInfo->version  );
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : robustness = %d",
               eventParams->mgmdIntfInfo->robustness);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : queryIntvl = %d",
               eventParams->mgmdIntfInfo->queryInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : queryMaxRespTime = %d",
               eventParams->mgmdIntfInfo->queryResponseInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : startupIntvl = %d",
               eventParams->mgmdIntfInfo->startupQueryInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : startupCount = %d",
               eventParams->mgmdIntfInfo->startupQueryCount);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : lastMemQIntvl = %d",
               eventParams->mgmdIntfInfo->lastMemQueryInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : lastMemQCount = %d",
               eventParams->mgmdIntfInfo->lastMemQueryCount);
  }

  if (mgmdMapProtocolCtrlBlockGet(eventParams->familyType,
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {

    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "Invalid Control Block\r\n");
    return L7_FAILURE;
  }
  switch(eventType)
  {
  case MGMD_ADMIN_MODE_SET:
       rc = mgmd_global_mode_set(mgmdCB, eventInfo);
       break;
  case MGMD_INTF_MODE_SET:
       rc = mgmd_intf_mode_process(mgmdCB, rtrIfNum, intfType, eventInfo,
                                   eventParams->mgmdIntfInfo);
       break;
  case MGMD_VERSION_SET:
       rc = mgmd_interface_version_set(mgmdCB,rtrIfNum, eventInfo);
       break;
  case MGMD_ROUTER_ALERT_CHECK_SET:
       mgmdCB->checkRtrAlert = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_ROBUSTNESS_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.robustness = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_QUERY_INTERVAL_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.queryInterval = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_QUERY_MAX_RESPONSE_TIME_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.queryResponseInterval = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_STARTUP_QUERY_INTERVAL_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.startupQueryInterval = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_STARTUP_QUERY_COUNT_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.startupQueryCount = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_LASTMEMBER_QUERY_INTERVAL_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.lastMemQueryInterval = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_LASTMEMBER_QUERY_COUNT_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.lastMemQueryCount = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_UNSOLICITED_REPORT_INTERVAL_SET:
       mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
       mgmd_info->intfConfig.lastMemQueryCount = eventInfo;
       rc = L7_SUCCESS;
       break;
  default:
      MGMD_DEBUG(MGMD_DEBUG_EVENTS, "Invalid config apply for %d", eventType);
      break;
  }
  return rc;
}

/*********************************************************************
* @purpose  Updates all report-specific statistics
*
* @param    mgmdCB      @b{ (input) } MGMD Control block
* @param    rtrIfNum    @b{ (input) } router interface number
* @param    grpRecType  @b{ (input) } Group Report Type
* @param    statsType   @b{ (input) } Statictics type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfReportStatsUpdate(mgmd_cb_t *mgmdCB,
                               L7_uint32 rtrIfNum,
                               MGMD_GROUP_REPORT_TYPE_t reportType,
                               MGMD_STATS_TYPE_t statsType)
{
  mgmd_info_t *mgmdIntf = L7_NULLPTR;
  MGMD_CTRL_PKT_TYPE_t msgType;

  /* Basic validations. */
  if ((statsType >= MGMD_STATS_MAX) || (reportType >= MGMD_GROUP_REPORT_TYPE_MAX) ||
      (rtrIfNum < 1) || (rtrIfNum >= MCAST_MAX_INTERFACES))
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE,
               "Invalid reportType(%d) or statsType(%d) or rtrIfNum(%d)",
               reportType, statsType, rtrIfNum);
    return L7_FAILURE;
  }
  switch (reportType)
  {
    case L7_IGMP_MODE_IS_INCLUDE:
      msgType = MGMD_REPORT_V3_ISINC;
      break;
    case L7_IGMP_MODE_IS_EXCLUDE:
      msgType = MGMD_REPORT_V3_ISEXC;
      break;
    case L7_IGMP_CHANGE_TO_INCLUDE_MODE:
      msgType = MGMD_REPORT_V3_TOINC;
      break;
    case L7_IGMP_CHANGE_TO_EXCLUDE_MODE:
      msgType = MGMD_REPORT_V3_TOEXC;
      break;
    case L7_IGMP_ALLOW_NEW_SOURCES:
      msgType = MGMD_REPORT_V3_ALLOW;
      break;
    case L7_IGMP_BLOCK_OLD_SOURCES:
      msgType = MGMD_REPORT_V3_BLOCK;
      break;
    default:
      MGMD_DEBUG(MGMD_DEBUG_FAILURE,
                 "Invalid group report Type = %d on rtrIfNum(%d)",
                 reportType, rtrIfNum);
      return L7_FAILURE;
  }

  /* Get the Interface Entry and Update the Statistic. */
  if (rtrIfNum == mgmdCB->mgmd_proxy_ifIndex)
  {
    if (mgmdCB->mgmd_host_info != L7_NULLPTR)
    {
      (mgmdCB->mgmd_host_info->proxyIntfStats[msgType][statsType])++;
    }
  }
  else
  {
    mgmdIntf = &(mgmdCB->mgmd_info[rtrIfNum]);
    (mgmdIntf->intfStats[msgType][statsType])++;
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  To Update the PIM-DM Interface Statistics
*
* @param    family       @b{ (input) } IP address family to get MGMD Ctrl Block
*           rtrIfNum     @b{ (input) } Index of the Router Interface for
*                                      Stats has to be updated
*           msgType      @b{ (input) } Type of the MGMD Control Pkt
*           statsType    @b{ (input) } Type of the Statistic
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfStatsUpdate (L7_uchar8 family,
                             L7_uint32 rtrIfNum,
                             MGMD_CTRL_PKT_TYPE_t msgType,
                             MGMD_STATS_TYPE_t statsType)
{
  mgmd_cb_t *mgmdCB = L7_NULLPTR;
  mgmd_info_t *mgmdIntf = L7_NULLPTR;

  /* Basic validations. */
  if ((msgType >= MGMD_CTRL_PKT_MAX) || (statsType >= MGMD_STATS_MAX) ||
      (rtrIfNum < 1) || (rtrIfNum >= MCAST_MAX_INTERFACES))
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE,
                "Invalid msg Type or stats Type or rtrIfNum");
    return L7_FAILURE;
  }
  if (mgmdMapProtocolCtrlBlockGet(family,
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE,"MGMD:Failed to access control block\n");
    return L7_FAILURE;
  }
  /* Get the Interface Entry and Update the Statistic. */
  if (rtrIfNum == mgmdCB->mgmd_proxy_ifIndex)
  {
    if (mgmdCB->mgmd_host_info != L7_NULLPTR)
    {
      (mgmdCB->mgmd_host_info->proxyIntfStats[msgType][statsType])++;
    }
  }
  else
  {
    mgmdIntf = &mgmdCB->mgmd_info[rtrIfNum];
    (mgmdIntf->intfStats[msgType][statsType])++;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  DevShell Debug Command to clear the interface statistics
*
* @param    family    @b{ (input) } Family (IPv4, IPv6)
* @param    rtrIfNum  @b{ (input) } Router interface number
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void mgmdIntfStatsClear(L7_uint32 family, L7_uint32 rtrIfNum)
{
  mgmd_cb_t *mgmdCB;
  mgmd_info_t *mgmdIntf;
  L7_uint32  rtrIndex;

  if (mgmdMapProtocolCtrlBlockGet (family,
                                   (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Couldn't get Control Block for family = %d", family);
    return;
  }
  if (rtrIfNum != L7_NULL)
  {
    if (rtrIfNum == mgmdCB->mgmd_proxy_ifIndex)
    {
      memset(mgmdCB->mgmd_host_info->proxyIntfStats, 0,
             sizeof(mgmdCB->mgmd_host_info->proxyIntfStats));
    }
    else
    {
      mgmdIntf = &(mgmdCB->mgmd_info[rtrIfNum]);
      memset(mgmdIntf->intfStats, 0, sizeof(mgmdIntf->intfStats));
    }
  }
  else
  {
    for (rtrIndex = 0; rtrIndex < MCAST_MAX_INTERFACES; rtrIndex++)
    {
      mgmdIntf = &(mgmdCB->mgmd_info[rtrIndex]);
      memset(mgmdIntf->intfStats, 0, sizeof(mgmdIntf->intfStats));
    }
    /* Clear Proxy stats also*/
    memset(mgmdCB->mgmd_host_info->proxyIntfStats, 0,
           sizeof(mgmdCB->mgmd_host_info->proxyIntfStats));
  }
}
