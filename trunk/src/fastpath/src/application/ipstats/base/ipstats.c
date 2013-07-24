/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ipstats.c
*
* @purpose     Ip Counter Maintenance
*
* @component   ipStats
*
* @comments    none
*
* @create      05/07/2001
*
* @author      anayar
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "platform_counters.h"
#include "statsapi.h"
#include "ip_1213_api.h"
#include "default_cnfgr.h"
#include "log.h"
#include "dtlapi.h"

/* Ip counter ID lists
 * Note: If any new counters are added to the ipMap_externCtrs
 *       please remember to update IP_MAP_EXTERNAL_COUNTERS_LIST_SIZE
 */
#define IP_STATS_EXTERNAL_COUNTERS_LIST_SIZE  42

static L7_uint32 ipstats_externCtrs[IP_STATS_EXTERNAL_COUNTERS_LIST_SIZE] =
{
  /* IP counters */
  L7_PLATFORM_CTR_RX_IP_IN_RECEIVES,
  L7_PLATFORM_CTR_RX_IP_IN_HDR_ERRORS,
  L7_PLATFORM_CTR_RX_IP_IN_ADDR_ERRORS,
  L7_PLATFORM_CTR_RX_IP_FORW_DATAGRAMS,
  L7_PLATFORM_CTR_RX_IP_IN_UNKNOWN_PROTOS,
  L7_PLATFORM_CTR_RX_IP_IN_DISCARDS,
  L7_PLATFORM_CTR_RX_IP_IN_DELIVERS,
  L7_PLATFORM_CTR_IP_REASM_TIMEOUTS,
  L7_PLATFORM_CTR_RX_IP_REASM_REQDS,
  L7_PLATFORM_CTR_RX_IP_REASM_OKS,
  L7_PLATFORM_CTR_RX_IP_REASM_FAILS,
  L7_PLATFORM_CTR_RX_IP_FRAG_OKS,
  L7_PLATFORM_CTR_RX_IP_FRAG_FAILS,
  L7_PLATFORM_CTR_RX_IP_FRAG_CREATES,
  L7_PLATFORM_CTR_TX_IP_OUT_REQUESTS,
  L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS,
  L7_PLATFORM_CTR_TX_IP_OUT_NO_ROUTES,
  L7_PLATFORM_CTR_IP_ROUTING_DISCARDS,

  /* ICMP counters */
  L7_PLATFORM_CTR_RX_ICMP_IN_ERRORS,
  L7_PLATFORM_CTR_RX_ICMP_IN_DEST_UNREACHS,
  L7_PLATFORM_CTR_RX_ICMP_IN_TIME_EXCDS,
  L7_PLATFORM_CTR_RX_ICMP_IN_PARM_PROBS,
  L7_PLATFORM_CTR_RX_ICMP_IN_SRC_QUENCHS,
  L7_PLATFORM_CTR_RX_ICMP_IN_REDIRECTS,
  L7_PLATFORM_CTR_RX_ICMP_IN_ECHOS,
  L7_PLATFORM_CTR_RX_ICMP_IN_ECHO_REPS,
  L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMPS,
  L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMP_REPS,
  L7_PLATFORM_CTR_RX_ICMP_IN_ADDR_MASKS,
  L7_PLATFORM_CTR_RX_ICMP_IN_ADDR_MASK_REPS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_ERRORS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_DEST_UNREACHS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_TIME_EXCDS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_PARM_PROBS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_SRC_QUENCHS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_REDIRECTS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_ECHOS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_ECHO_REPS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_TIMESTAMPS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_TIMESTAMP_REPS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_ADDR_MASKS,
  L7_PLATFORM_CTR_TX_ICMP_OUT_ADDR_MASK_REPS

};

static L7_uint32 ipstats_driverCtrs[] =
{
  L7_PLATFORM_CTR_RX_IP_IN_RECEIVES,
  L7_PLATFORM_CTR_RX_IP_FORW_DATAGRAMS,
  L7_PLATFORM_CTR_RX_IP_FRAG_FAILS,
  L7_PLATFORM_CTR_RX_IP_IN_HDR_ERRORS,
  L7_PLATFORM_CTR_RX_IP_IN_ADDR_ERRORS,
  L7_PLATFORM_CTR_IP_ROUTING_DISCARDS,
  L7_PLATFORM_CTR_RECEIVED_L3_MAC_MCAST_DISCARDS,
  L7_PLATFORM_CTR_RECEIVED_L3_ARP_TO_CPU,
  L7_PLATFORM_CTR_RECEIVED_L3_IP_TO_CPU
};

mutling_id_t mutlingIcmpInMsgs[] =
{
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_ERRORS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_DEST_UNREACHS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_TIME_EXCDS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_PARM_PROBS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_SRC_QUENCHS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_REDIRECTS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_ECHOS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_ECHO_REPS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMPS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMP_REPS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_ADDR_MASKS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_ADDR_MASK_REPS
  }
};

mutling_id_t mutlingIcmpOutMsgs[] =
{
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_ERRORS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_DEST_UNREACHS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_TIME_EXCDS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_PARM_PROBS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_SRC_QUENCHS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_REDIRECTS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_ECHOS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_ECHO_REPS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_TIMESTAMPS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMP_REPS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_ADDR_MASKS
  },
  {
    cPLUS,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_ADDR_MASK_REPS
  }
};

mutling_parm_list_t mutantIcmpInMsgs =
{
  sizeof( mutlingIcmpInMsgs ) / sizeof( mutling_id_t ),
  mutlingIcmpInMsgs
};

mutling_parm_list_t mutantIcmpOutMsgs =
{
  sizeof( mutlingIcmpOutMsgs ) / sizeof( mutling_id_t ),
  mutlingIcmpOutMsgs
};

statsParm_entry_t ipStatsMutants[] =
{
  {  /* counter L7_PLATFORM_CTR_RX_ICMP_IN_MSGS */
    L7_FAILURE,
    MutantCtr,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_RX_ICMP_IN_MSGS,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantIcmpInMsgs
  },
  {  /* counter L7_PLATFORM_CTR_RX_ICMP_IN_MSGS */
    L7_FAILURE,
    MutantCtr,
    FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
    L7_PLATFORM_CTR_TX_ICMP_OUT_MSGS,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantIcmpOutMsgs
  }
};


/*********************************************************************
* @purpose  Gets the value of Ip counters requested by the Statistics
*           Manager
*
* @param    *pCtr       Pointer to counter specifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is only intended to be called directly by the
*           Stats Manager component, since this function pointer was
*           provided by the ip component when the external counters
*           were created.  It is not a public API function.
*
* @end
*********************************************************************/
L7_RC_t ipStatsGet(counterValue_entry_t *pCtr)
{
  pCtr->cSize = C32_BITS;

  switch(pCtr->cId)
  {
  case L7_PLATFORM_CTR_RX_IP_IN_RECEIVES:
    pCtr->status = ipmIpInReceivesGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_IN_HDR_ERRORS:
    pCtr->status = ipmIpInHdrErrorsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_IN_ADDR_ERRORS:
    pCtr->status = ipmIpInAddrErrorsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_FORW_DATAGRAMS:
    pCtr->status = ipmIpForwDatagramsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_IP_OUT_NO_ROUTES:
    pCtr->status = ipmIpOutNoRoutesGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_IN_UNKNOWN_PROTOS:
    pCtr->status = ipmIpInUnknownProtosGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_IN_DISCARDS:
    pCtr->status = ipmIpInDiscardsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_IN_DELIVERS:
    pCtr->status = ipmIpInDeliversGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_IP_REASM_TIMEOUTS:
    pCtr->status = ipmIpReasmTimeoutGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_REASM_REQDS:
    pCtr->status = ipmIpReasmReqdsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_REASM_OKS:
    pCtr->status = ipmIpReasmOKsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_REASM_FAILS:
    pCtr->status = ipmIpReasmFailsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_FRAG_OKS:
    pCtr->status = ipmIpFragOKsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_FRAG_FAILS:
    pCtr->status = ipmIpFragFailsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_IP_FRAG_CREATES:
    pCtr->status = ipmIpFragCreatesGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_IP_OUT_REQUESTS:
    pCtr->status = ipmIpOutRequestsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS:
    pCtr->status = ipmIpOutDiscardsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_IP_ROUTING_DISCARDS:
    pCtr->status = ipmIpRoutingDiscardsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_ERRORS:
    pCtr->status = ipmIcmpInErrorsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_DEST_UNREACHS:
    pCtr->status = ipmIcmpInDestUnreachsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_TIME_EXCDS:
    pCtr->status = ipmIcmpInTimeExcdsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_PARM_PROBS:
    pCtr->status = ipmIcmpInParmProbsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_SRC_QUENCHS:
    pCtr->status = ipmIcmpInSrcQuenchsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_REDIRECTS:
    pCtr->status = ipmIcmpInRedirectsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_ECHOS:
    pCtr->status = ipmIcmpInEchosGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_ECHO_REPS:
    pCtr->status = ipmIcmpInEchoRepsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMPS:
    pCtr->status = ipmIcmpInTimestampsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_TIMESTAMP_REPS:
    pCtr->status = ipmIcmpInTimestampRepsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_ADDR_MASKS:
    pCtr->status = ipmIcmpInAddrMasksGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_RX_ICMP_IN_ADDR_MASK_REPS:
    pCtr->status = ipmIcmpInAddrMaskRepsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_ERRORS:
    pCtr->status = ipmIcmpOutErrorsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_DEST_UNREACHS:
    pCtr->status = ipmIcmpOutDestUnreachsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_TIME_EXCDS:
    pCtr->status = ipmIcmpOutTimeExcdsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_PARM_PROBS:
    pCtr->status = ipmIcmpOutParmProbsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_SRC_QUENCHS:
    pCtr->status = ipmIcmpOutSrcQuenchsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_REDIRECTS:
    pCtr->status = ipmIcmpOutRedirectsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_ECHOS:
    pCtr->status = ipmIcmpOutEchosGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_ECHO_REPS:
    pCtr->status = ipmIcmpOutEchoRepsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_TIMESTAMPS:
    pCtr->status = ipmIcmpOutTimestampsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_TIMESTAMP_REPS:
    pCtr->status = ipmIcmpOutTimestampRepsGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_ADDR_MASKS:
    pCtr->status = ipmIcmpOutAddrMasksGet(&pCtr->cValue.low);
    break;

  case L7_PLATFORM_CTR_TX_ICMP_OUT_ADDR_MASK_REPS:
    pCtr->status = ipmIcmpOutAddrMaskRepsGet(&pCtr->cValue.low);
    break;

  default:
    pCtr->status = L7_FAILURE;
    break;
  }

  return(pCtr->status);
}

/*********************************************************************
* @purpose  Creates the ip counters
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    Counters created in this function must be reset via
*           ipStatsReset and deleted via ipStatsDelete.
*
* @end
*********************************************************************/
L7_RC_t ipStatsCreate()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i, j, listSize = IP_STATS_EXTERNAL_COUNTERS_LIST_SIZE;
  L7_uint32 mlistSize = sizeof(ipStatsMutants) / sizeof(statsParm_entry_t);
  statsParm_entry_t ipCtr;

  /* Create the external counters */
  ipCtr.cKey  = FD_CNFGR_NIM_MIN_CPU_INTF_NUM;
  ipCtr.cSize = C32_BITS;
  ipCtr.cType = ExternalCtr;
  ipCtr.isResettable = L7_FALSE;
  ipCtr.pMutlingsParmList = L7_NULL;
  ipCtr.pSource = (L7_VOIDFUNCPTR_t) ipStatsGet;

  for(i=0; (i< listSize && rc == L7_SUCCESS); i++)
  {
    ipCtr.cId = ipstats_externCtrs[i];
    rc = statsCreate(1, (pStatsParm_list_t)(&ipCtr));
  }

  if(rc == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsCreate: Could not create ip external counters\n");
  }

  /* Create mutant counters */
  if (statsCreate (mlistSize, ipStatsMutants) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsCreate: Could not create ip mutant counters\n");
  }

  /* Ip counters supported by the LVL7 driver need to be created on
   * a per interface basis
   */
  listSize = sizeof(ipstats_driverCtrs) / sizeof (L7_uint32);
  ipCtr.cSize = C32_BITS;
  ipCtr.cType = ExternalCtr;
  ipCtr.isResettable = L7_FALSE;
  ipCtr.pMutlingsParmList = L7_NULL;
  ipCtr.pSource = (L7_VOIDFUNCPTR_t) dtlStatsGet;

  for (i = 0; ((i < listSize) && (rc == L7_SUCCESS)); i++)
  {
    ipCtr.cId = ipstats_driverCtrs[i];
    for (j = 0; j <= L7_MAX_INTERFACE_COUNT; j++)
    {
      /* have the intf number as the first 12 bits of CID */
      ipCtr.cKey = j + 1;
      rc = statsCreate(1, (pStatsParm_list_t)(&ipCtr));
    }
  }

  if(rc == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsCreate: Could not create ip driver counters\n");
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Reset the ip counters
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    Only resets counters that have been created in ipStatsCreate
*
* @end
*********************************************************************/
L7_RC_t ipStatsReset()
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uint32 i, j, listSize = IP_STATS_EXTERNAL_COUNTERS_LIST_SIZE;
  L7_uint32 mlistSize = sizeof(ipStatsMutants) / sizeof(statsParm_entry_t);

  counterValue_entry_t counterValue_entry;

  /* Reset the external counters */
  counterValue_entry.cKey  = FD_CNFGR_NIM_MIN_CPU_INTF_NUM;
  for(i=0; (i< listSize && rc == L7_SUCCESS); i++)
  {
    counterValue_entry.cId = ipstats_externCtrs[i];
    rc = statsReset(1, &counterValue_entry);
  }

  if(rc == L7_FAILURE)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsReset: Could not reset ip external counters\n");

  /* Reset mutant counters */
  for(i=0; (i< mlistSize && rc == L7_SUCCESS); i++)
  {
    counterValue_entry.cKey = ipStatsMutants[i].cKey;
    counterValue_entry.cId = ipStatsMutants[i].cId;
    rc = statsReset(1, &counterValue_entry);
  }

  if(rc == L7_FAILURE)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsReset: Could not reset ip mutant counters\n");

  /* Ip counters supported by the LVL7 driver (per interface) */
  listSize = sizeof(ipstats_driverCtrs) / sizeof (L7_uint32);
  for (i = 0; ((i < listSize) && (rc == L7_SUCCESS)); i++)
  {
    counterValue_entry.cId = ipstats_driverCtrs[i];
    for (j = 0; j <= L7_MAX_INTERFACE_COUNT; j++)
    {
      /* have the intf number as the first 12 bits of CID */
      counterValue_entry.cKey = j + 1;
      rc = statsReset(1, &counterValue_entry);
    }
  }

  if(rc == L7_FAILURE)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsReset: Could not reset ip driver counters\n");

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Deletes the IP counters created in ipStatsCreate().
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipStatsDelete()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i, j, listSize = IP_STATS_EXTERNAL_COUNTERS_LIST_SIZE;
  L7_uint32 mlistSize = sizeof(ipStatsMutants) / sizeof(statsParm_entry_t);
  statsParm_entry_t ipCtr;

  /* Delete the external counters */
  ipCtr.cKey  = FD_CNFGR_NIM_MIN_CPU_INTF_NUM;
  ipCtr.cSize = C32_BITS;
  ipCtr.cType = ExternalCtr;
  ipCtr.isResettable = L7_FALSE;
  ipCtr.pMutlingsParmList = L7_NULL;
  ipCtr.pSource = (L7_VOIDFUNCPTR_t) ipStatsGet;

  for(i=0; (i< listSize && rc == L7_SUCCESS); i++)
  {
    ipCtr.cId = ipstats_externCtrs[i];
    rc = statsDelete(1, (pStatsParm_list_t)(&ipCtr));
  }

  if(rc == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsDelete: Could not delete IP external counters\n");
  }

  /* Delete mutant counters */
  if (statsDelete (mlistSize, ipStatsMutants) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsDelete: Could not delete IP mutant counters\n");
  }

  /* IP counters supported by the LVL7 driver need to be created on
   * a per interface basis
   */
  listSize = sizeof(ipstats_driverCtrs) / sizeof (L7_uint32);
  ipCtr.cSize = C32_BITS;
  ipCtr.cType = ExternalCtr;
  ipCtr.isResettable = L7_FALSE;
  ipCtr.pMutlingsParmList = L7_NULL;
  ipCtr.pSource = (L7_VOIDFUNCPTR_t) dtlStatsGet;

  for (i = 0; ((i < listSize) && (rc == L7_SUCCESS)); i++)
  {
    ipCtr.cId = ipstats_driverCtrs[i];
    for (j = 0; j <= L7_MAX_INTERFACE_COUNT; j++)
    {
      /* have the intf number as the first 12 bits of CID */
      ipCtr.cKey = j + 1;
      rc = statsDelete(1, (pStatsParm_list_t)(&ipCtr));
    }
  }

  if(rc == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "ipStatsDelete: Could not delete IP driver counters\n");
  }

  return L7_SUCCESS;
}


