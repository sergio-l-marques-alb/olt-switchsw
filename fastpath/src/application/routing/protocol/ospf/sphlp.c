/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2008
 *
 * *********************************************************************
 *
 * @filename  sphlp.c
 *
 * @purpose   Code related to graceful restart helpful neighbor
 *
 * @component  Routing OSPF Component
 *
 * @create     2/3/2008
 *
 * @author     Rob Rice
 *
 * @end
 *
 * ********************************************************************/

#include <stdio.h>
#include "std.h"
#include "local.h"
#include "l7_common.h"
#include "l3_commdefs.h"
#include "log.h"
#include "spobj.h"
#include "spgrc.h"
#include "osapi.h"
#include "osapi_support.h"
#include "ospf_vend_ctrl.h"

extern struct ospfMapCtrl_s  ospfMapCtrl_g;



/*********************************************************************
* @purpose  Process a received grace LSA. 
*
* @param    p_NBO - neighbor that sent the grace LSA
* @param    p_DbEntry - database entry for the grace LSA
*
* @returns  E_OK if the grace LSA was successfully processed.
*                return code does not indicate whether OSPF entered 
*                helper mode for neighbor that sent the grace LSA.
*
* @notes    none
*
* @end
*********************************************************************/
e_Err o2GraceLsaProcess(t_NBO *p_NBO, t_A_DbEntry *p_DbEntry)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  L7_BOOL alreadyRestarting = o2NeighborIsRestarting(p_NBO);

  /* Ignore our own grace LSAs. We may get these back during 
   * db exchange when we do a graceful restart. */
  if (A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) == p_RTO->Cfg.RouterId)
  {
    return E_OK;
  }

  /* Number of seconds left in grace period */
  L7_int32 timeRemaining = 0;
  L7_uint32 graceLsaAge = A_GET_2B(p_DbEntry->Lsa.LsAge);

  p_RTO->grStats.helpfulNbrStats.graceLsasReceived++;
  if (graceLsaAge >= MaxAge)
  {
    if (alreadyRestarting)
    {
      if (p_NBO->State != NBO_FULL)
      {
        /* Helper received max age grace LSA but the adjacency has not returned
         * to FULL. Postpone the exit of helper mode until helper reaches FULL. */
        p_NBO->helperExitPending = TRUE;
        return E_OK;
      }

      /* Successfully completed graceful restart */
      o2HelperModeExit(p_NBO, OSPF_HELPER_COMPLETED);
    }
    return E_OK;
  }

  if (o2GraceLsaParse(p_NBO, p_DbEntry) != E_OK)
  {
    /* Don't enter helpful neighbor mode */
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    return E_OK;
  }

  /* Check the grace period */
  timeRemaining = p_NBO->gracePeriod - graceLsaAge;
  if (timeRemaining <= 0)
  {
    /* Grace period has already expired */
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor %s sent grace LSA, but grace period %u expired before receipt of grace LSA.",
            nbrAddrStr, p_NBO->gracePeriod);
    p_RTO->grStats.helpfulNbrStats.staleGraceLsa++;
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    return E_OK;
  }

  /* If neighbor already restarting, update grace period */
  if (alreadyRestarting)
  {
    if (o2GracePeriodUpdate(p_NBO, timeRemaining) != E_OK)
    {
      p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    }
    return E_OK;
  }

  /* Only enter helper mode if neighbor is FULL */
  if (p_NBO->State != NBO_FULL)
  {
    p_RTO->grStats.helpfulNbrStats.graceLsaNbrNotFull++;
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    return E_OK;
  }

  /* Don't be helpful if this router is restarting */
  if (o2GracefulRestartInProgress(p_RTO))
  {
    p_RTO->grStats.helpfulNbrStats.ownRestartInProgress++;
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    return E_OK;
  }

  /* Don't be helpful if a topology change is pending in this neighbor's retx list */
  if (o2TopologyChangePending(p_NBO))
  {
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor %s sent grace LSA, but topology changes are pending.",
            nbrAddrStr);
    p_RTO->grStats.helpfulNbrStats.topoChangePending++;
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    return E_OK;
  }

  /* Check if local policy allows us to be helpful */
  if (!o2RestartReasonAcceptable(p_NBO))
  {
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor %s sent grace LSA, but router not configured as helpful "
            "neighbor for restart code %u.",
            nbrAddrStr, p_NBO->restartReason);
    p_RTO->grStats.helpfulNbrStats.localPolicyConflict++;
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
    return E_OK;
  }

  /* Passed all hurdles to becoming a helpful neighbor */
  if (o2HelperModeEnter(p_NBO, timeRemaining) != E_OK)
  {
    p_RTO->grStats.helpfulNbrStats.failedToEnterHelperMode++;
    p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
  }

  return E_OK;
}

/*********************************************************************
* @purpose  Ask if this router is currently in a graceful restart
*
* @param    p_RTO - OSPF instance
*
* @returns  TRUE if OSPF is in graceful restart
* @returns  FALSE otherwise
*
* @notes    none
*
* @end
*********************************************************************/
Bool o2GracefulRestartInProgress(t_RTO *p_RTO)
{
  return ((p_RTO->restartStatus == OSPF_GR_PLANNED_RESTART) || 
          (p_RTO->restartStatus == OSPF_GR_UNPLANNED_RESTART));
}

/*********************************************************************
* @purpose  Parse the TLVs in a grace LSA and store data on the 
*           neighbor object.
*
* @param    p_NBO - neighbor
* @param    p_DbEntry - grace LSA
*
* @returns  E_OK if parse succeeded
*           E_FAILED if parse failed
*
* @notes    not sure of the purpose of the neighbor address TLV. We 
*           already know who sent the grace LSA.
*
* @end
*********************************************************************/
e_Err o2GraceLsaParse(t_NBO *p_NBO, t_A_DbEntry *p_DbEntry)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  L7_uchar8 *firstTlv = (L7_uchar8*) p_DbEntry->p_Lsa;
  L7_uchar8 *pos = (L7_uchar8*) p_DbEntry->p_Lsa;
  L7_uint32 lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);
  L7_ushort16 tlvType;
  L7_ushort16 tlvLength;        /* does not include type and length fields */
  L7_ushort16 padLen;
  L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_BOOL gracePeriodTlv = L7_FALSE;
  L7_BOOL restartReasonTlv = L7_FALSE;

  while ((pos - firstTlv) < (lsaLength - sizeof(t_S_LsaHeader)))
  {
    tlvType = A_GET_2B(pos);
    pos += 2;
    tlvLength = A_GET_2B(pos);
    pos += 2;

    switch (tlvType)
    {
      case O2_GR_TLV_GRACE_PERIOD:
        if (o2GracePeriodTlvParse(p_NBO, pos, tlvLength) != E_OK)
        {
          return E_FAILED;
        }
        gracePeriodTlv = L7_TRUE;
        break;

      case O2_GR_TLV_RESTART_REASON:
        if (o2RestartReasonTlvParse(p_NBO, pos, tlvLength) != E_OK)
        {
          return E_FAILED;
        }
        restartReasonTlv = L7_TRUE;
        break;

      case O2_GR_TLV_IP_ADDRESS:
        if (o2IpAddressTlvParse(p_NBO, pos, tlvLength) != E_OK)
        {
          return E_FAILED;
        }
        break;

      default:
        osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "Received grace LSA from neighbor at %s with unrecognized TLV type %u",
                nbrAddrStr, tlvType);
        /* just skip it */
    }
    /* TLVs are padded to 4 octet alignment, but length doesn't include padding */
    if (tlvLength % 4)
      padLen = 4 - (tlvLength % 4);
    else
      padLen = 0;
    pos += (tlvLength + padLen);
  }

  /* Make sure grace LSA included all required TLVs */
  if (!gracePeriodTlv)
  {
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Grace LSA from %s did not include grace period TLV",
            nbrAddrStr);
    return E_FAILED;
  }

  if (!restartReasonTlv)
  {
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Grace LSA from %s did not include restart reason TLV",
            nbrAddrStr);
    return E_FAILED;
  }

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    sprintf(traceBuf, 
            "Received grace LSA from neighbor %s with grace period %u and restart reason %u.", 
            nbrAddrStr, p_NBO->gracePeriod, p_NBO->restartReason);
    RTO_TraceWrite(traceBuf);
  } 

  return E_OK;
}

/*********************************************************************
* @purpose  Read the grace period from a grace LSA and store the value on
*           the neighbor instance.
*
* @param    p_NBO - neighbor
* @param    pos - start of value field of the grace period TLV 
* @param    tlvLength - Value of length field in grace period TLV
*
* @returns  E_OK if parse succeeded
*           E_FAILED if parse failed
*
* @notes    none
*
* @end
*********************************************************************/
e_Err o2GracePeriodTlvParse(t_NBO *p_NBO, L7_uchar8 *pos, L7_ushort16 tlvLength)
{
  if (tlvLength != O2_GRACE_PERIOD_TLV_LEN)
  {
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor %s sent grace LSA with Grace Period TLV with wrong length %u. Expected %u.",
            nbrAddrStr, tlvLength, O2_GRACE_PERIOD_TLV_LEN);
    return E_FAILED;
  }

  p_NBO->gracePeriod = A_GET_4B(pos);
  return E_OK;
}

/*********************************************************************
* @purpose  Read the restart reason from a grace LSA and store the value on
*           the neighbor instance.
*
* @param    p_NBO - neighbor
* @param    pos - start of value field of the grace period TLV 
* @param    tlvLength - Value of length field in grace period TLV
*
* @returns  E_OK if parse succeeded
*           E_FAILED if parse failed
*
* @notes    none
*
* @end
*********************************************************************/
e_Err o2RestartReasonTlvParse(t_NBO *p_NBO, L7_uchar8 *pos, L7_ushort16 tlvLength)
{
  if (tlvLength != O2_RESTART_REASON_TLV_LEN)
  {
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor %s sent grace LSA with restart reason TLV with wrong length %u. Expected %u.",
            nbrAddrStr, tlvLength, O2_RESTART_REASON_TLV_LEN);
    return E_FAILED;
  }

  p_NBO->restartReason = (e_o2RestartReason) *pos;
  return E_OK;
}

/*********************************************************************
* @purpose  Read the neighbor's IP address from a grace LSA.
*
* @param    p_NBO - neighbor
* @param    pos - start of value field of the grace period TLV 
* @param    tlvLength - Value of length field in grace period TLV
*
* @returns  E_OK if parse succeeded
*           E_FAILED if parse failed
*
* @notes    not sure why we need this; so not putting it anywhere
*
* @end
*********************************************************************/
e_Err o2IpAddressTlvParse(t_NBO *p_NBO, L7_uchar8 *pos, L7_ushort16 tlvLength)
{
  L7_uint32 ipAddr;
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;

  if (tlvLength != O2_IP_ADDRESS_TLV_LEN)
  {
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor %s sent grace LSA with IP address TLV with wrong length %u. Expected %u.",
            nbrAddrStr, tlvLength, O2_IP_ADDRESS_TLV_LEN);
    return E_FAILED;
  }

  ipAddr = A_GET_4B(pos);

  /* Just curious if neighbor can send grace LSA with someone else's IP address */
  if (p_IFO->Cfg.Type == IFO_BRDC)
  {
    if (ipAddr != p_NBO->IpAdr)
    {
      L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
      osapiInetNtoa(ipAddr, ipAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Neighbor at %s sent grace LSA with IP address TLV set to %s.",
            nbrAddrStr, ipAddrStr);
    }
  }

  return E_OK;
}

/*********************************************************************
* @purpose  Determine whether local policy allows this router to be a 
*           helpful neighbor for a given neighbor who has announced a 
*           certain restart reason.
*
* @param    p_RTO - OSPF instance
* @param    p_NBO - neighbor
*
* @returns  L7_TRUE if configured local policy allows this router to act
*                   as a helpful neighbor for the type of restart underway
*           L7_FALSE otherwise
*
* @notes    not sure why we need this; so not putting it anywhere
*
* @end
*********************************************************************/
L7_BOOL o2RestartReasonAcceptable(t_NBO *p_NBO)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;

  /* RFC 3623 says restart is planned if restart reason is neither unknown (0) 
   * nor failover (3) */
  L7_BOOL plannedRestart = !((p_NBO->restartReason == O2_GR_REASON_UNKNOWN) || 
                             (p_NBO->restartReason == O2_GR_REASON_FAILOVER));

  if (p_RTO->Cfg.HelperSupport == OSPF_HELPER_ALWAYS)
    return L7_TRUE;

  if ((p_RTO->Cfg.HelperSupport == OSPF_HELPER_PLANNED_ONLY) && plannedRestart)
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Update the grace period for a neighbor who sent a 
*           grace LSA asking for more time.
*
* @param    p_NBO - neighbor
* @param    timeRemaining - number of seconds remaining in grace period
*
* @returns  E_OK
*
* @notes    If we get a grace LSA from a neighbor already in graceful restart,
*           we update the grace period from the new grace LSA. The update 
*           could lengthen or shorten the original grace period. RFC says,
*           "this time period began when grace-LSA's LS age was equal to 0." 
*           So the new grace period is completely independent of the former
*           grace period.
*
* @end
*********************************************************************/
e_Err o2GracePeriodUpdate(t_NBO *p_NBO, L7_int32 timeRemaining)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;

  if (timeRemaining <= 0)
    return E_FAILED;

  p_RTO->grStats.helpfulNbrStats.gracePeriodUpdate++;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    sprintf(traceBuf, "Updating grace period for neighbor %s. %d seconds remain.", 
            nbrAddrStr, timeRemaining);
    RTO_TraceWrite(traceBuf);
  } 

  return TIMER_StartSec(p_NBO->gracePeriodTimer, (L7_uint32) timeRemaining,
                        L7_FALSE, o2NbrGracePeriodTimerExp, p_NBO->OspfSysLabel.threadHndle);
}

/*********************************************************************
* @purpose  Take actions required if a neighbor's grace period expires.
*
* @param    Id - timer owner (neighbor in this case)
* @param    TimerId
* @param    Flag
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2NbrGracePeriodTimerExp(t_Handle Id, t_Handle TimerId, word Flag)
{
   t_NBO *p_NBO = (t_NBO *) Id;
   t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
   
   if (p_NBO->OperationState)
   {
     p_RTO->grStats.helpfulNbrStats.gracePeriodExpire++;
     o2HelperModeExit(p_NBO, OSPF_HELPER_TIMED_OUT);
   }

   return E_OK;
}

/*********************************************************************
* @purpose  Enter helpful neighbor mode for a neighbor.
*
* @param    p_NBO - the neighbor we are helping
* @param    timeRemaining - number of seconds until grace period expires
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2HelperModeEnter(t_NBO *p_NBO, L7_int32 timeRemaining)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
  L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  char ifName[L7_NIM_IFNAME_SIZE + 1];

  if (timeRemaining <= 0)
    return E_FAILED;
    
  osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
  RTO_InterfaceName(p_IFO, ifName);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
          "Entering graceful restart helper mode for neighbor at %s on interface %s.", 
          nbrAddrStr, ifName);

  p_RTO->grStats.helpfulNbrStats.enterHelperMode++;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, "Entering graceful restart helper mode for neighbor %s "
            "on interface %s. %d seconds remain.", 
            nbrAddrStr, ifName, timeRemaining);
    RTO_TraceWrite(traceBuf);
  } 

  p_NBO->helperExitReason = OSPF_HELPER_IN_PROGRESS;

  return TIMER_StartSec(p_NBO->gracePeriodTimer, (L7_uint32) timeRemaining,
                        L7_FALSE, o2NbrGracePeriodTimerExp, p_NBO->OspfSysLabel.threadHndle);
}

L7_uchar8 *o2HelperExitReason(OSPF_HELPER_EXIT_REASON_t reason)
{
  switch (reason)
  {
    case OSPF_HELPER_EXIT_NONE:    return "Not attempted";
    case OSPF_HELPER_IN_PROGRESS:  return "In progress";
    case OSPF_HELPER_COMPLETED:    return "Completed";
    case OSPF_HELPER_TIMED_OUT:    return "Timed out";
    case OSPF_HELPER_TOPO_CHANGE:  return "Topology change";
    case OSPF_HELPER_MANUAL_CLEAR: return "Manual clear";
    case OSPF_HELPER_LINK_DOWN:    return "Link down";
    default:                       return "Unknown";
  }
}

/* graceful restart reasons (RFC 3623) */
L7_uchar8 *o2RestartReasonStr(e_o2RestartReason restartReason)
{
  switch (restartReason)
  {
    case 0: return "Unknown";
    case 1: return "Software restart";
    case 2: return "Software reload/upgrade";
    case 3: return "Switch to redundant control processor";
    default:  return "Unrecognized";
  }
} 

/*********************************************************************
* @purpose  Exit helpful neighbor mode for a neighbor.
*
* @param    p_NBO - the neighbor we are helping
* @param    reason - why we are exiting helpful neighbor mode
*
* @returns  E_OK
*           E_NOT_FOUND if neighbor not restarting
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2HelperModeExit(t_NBO *p_NBO, OSPF_HELPER_EXIT_REASON_t reason)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
  t_ARO *p_ARO = (t_ARO*) p_NBO->ARO_Id;
  L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  char ifName[L7_NIM_IFNAME_SIZE + 1];

  /* Always reset the helperExitPending flag */
  p_NBO->helperExitPending = FALSE;

  if (!o2NeighborIsRestarting(p_NBO))
  {
    p_RTO->grStats.helpfulNbrStats.unexpectedExit++;
    return E_NOT_FOUND; 
  }

  p_RTO->grStats.helpfulNbrStats.exitHelperMode++;

  if (TIMER_Active(p_NBO->gracePeriodTimer))
  {
    TIMER_Stop(p_NBO->gracePeriodTimer);
  }

  osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
  RTO_InterfaceName(p_IFO, ifName);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
          "Exiting helper mode for neighbor at %s on interface %s. %s.",
          nbrAddrStr, ifName, o2HelperExitReason(reason));

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, "Exiting helper mode for neighbor at %s on interface %s. %s.", 
            nbrAddrStr, ifName, o2HelperExitReason(reason));
    RTO_TraceWrite(traceBuf);
  } 

  p_NBO->gracePeriod = 0;
  p_NBO->restartReason = O2_GR_REASON_NOT_RESTARTING;
  p_NBO->helperExitReason = reason;

  if (reason == OSPF_HELPER_TIMED_OUT)
  {
    /* Doesn't look like neighbor is going to flush the grace LSA. 
     * If we leave it in our db until it naturally ages out, we may not
     * accept the next grace LSA from this neighbor, if we receive it before
     * this one ages out. So delete neighbor's grace LSA from our LSDB */
    o2DeleteGraceLsa(p_NBO);

    if (p_NBO->State < NBO_2WAY)
    {
      /* We may have retained this neighbor even though we haven't been getting
       * hellos. If this adjacency is not making progress, kill it. */
      DoNboTransition(p_NBO, NBO_KILL, 0);
      p_NBO = NULL;
    }
  }

  /* recalculate DR */
  if ((p_IFO->Cfg.Type == IFO_BRDC) && (p_IFO->Cfg.State > IFO_WAIT))
  {
    DrCalculate(p_IFO);
  }

  /* reoriginate router LSA for neighbor's area */
  LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

  /* if DR, reoriginate network LSA */
  if (p_IFO->DrId == p_RTO->Cfg.RouterId)
  {
    LsaReOriginate[S_NETWORK_LSA](p_ARO, (ulng) p_IFO);
  }

  /* if virtual neighbor, reoriginate router LSA for transit area */
  if (p_IFO->Cfg.Type == IFO_VRTL)
  {
    LsaReOriginate[S_ROUTER_LSA]((t_ARO*) p_IFO->TransitARO, 0);
  }

  return E_OK;
}

/*********************************************************************
* @purpose  Ask whether this router is in helper mode for a given neighbor.
*
* @param    p_NBO - the neighbor 
*
* @returns  L7_TRUE if in helper mode for given neighbor
*           L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL o2NeighborIsRestarting(t_NBO *p_NBO)
{
  return (p_NBO->helperExitReason == OSPF_HELPER_IN_PROGRESS);
}

/*********************************************************************
* @purpose  Delete a grace LSA for a neighbor whose grace period has
*           timed out.
*
* @param    p_NBO - the neighbor 
*
* @returns  E_OK if grace LSA deleted
*           E_FAILED if grace LSA not found
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2DeleteGraceLsa(t_NBO *p_NBO)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
  t_A_DbEntry *p_DbEntry;
  t_A_DbKey dbkey;

  /* find database entry */
  A_SET_4B(p_NBO->RouterId, dbkey.AdvertisingRouter);
  dbkey.LsId[0] = OPAQUE_LSA_TYPE_GRACE;
  dbkey.LsId[1] = 0;
  dbkey.LsId[2] = 0;
  dbkey.LsId[3] = 0;
  if (AVLH_Find(p_IFO->LinkOpaqueLsaHl, (byte*) &dbkey, (void *)&p_DbEntry, 0) != E_OK)
  {
    printf("\nFailed to find grace LSA for timed out grace period");
    return E_FAILED;
  }

  if (AgingTbl_Running(p_RTO->AgingObj))
  {
    AgingTbl_DeleteEntry(p_RTO->AgingObj, p_DbEntry, 
                         p_DbEntry->AgeIndex);
    AgingTbl_AddEntry(p_RTO->AgingObj, p_DbEntry, MaxAge + 1,
                        &p_DbEntry->AgeIndex);
  }

   A_SET_2B(MaxAge, p_DbEntry->Lsa.LsAge);

   return E_OK;
}

/*********************************************************************
* @purpose  Get helpful neighbor status.
*
* @param    RTO_Id - OSPF instance
* @param    intIfNum - internal interface number where neighbor attached
* @param    nbrIpAddr - IPv4 interface address of neighbor
* @param    nbrStatus (output) helpful neighbor status
*
* @returns  E_OK if status set
*           E_FAILED if neighbor not found
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2HelpfulNbrStatusGet(t_Handle RTO_Id, L7_uint32 intIfNum, L7_uint32 nbrIpAddr, 
                            L7_ospfHelpfulNbrStatus_t *nbrStatus)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  t_IFO *p_IFO;
  t_NBO *p_NBO;
  e_Err e;

  if (!nbrStatus)
    return E_FAILED;

  if (HL_FindFirst(p_RTO->IfoIndexHl, (byte*) &intIfNum, (void**) &p_IFO) != E_OK)
  {
    return E_FAILED;
  }

  e = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
  while (e == E_OK)
  {
    if (p_NBO->IpAdr == nbrIpAddr)
    {
      nbrStatus->exitReason = p_NBO->helperExitReason;
      strncpy(nbrStatus->exitReasonStr, o2HelperExitReason(p_NBO->helperExitReason), 
              OSPF_GR_EXIT_REASON_LEN - 1);
      nbrStatus->restartCode = p_NBO->restartReason;
      strncpy(nbrStatus->restartReason, o2RestartReasonStr(p_NBO->restartReason), 
              OSPF_RESTART_REASON_LEN - 1);
      nbrStatus->restartAge = TIMER_ToExpireSec(p_NBO->gracePeriodTimer);
      return E_OK;
    }
    e = HL_GetNext(p_IFO->NboHl, (void**) &p_NBO, p_NBO);
  }
  return E_FAILED;
}



