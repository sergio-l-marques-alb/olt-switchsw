/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2008
 *
 * *********************************************************************
 *
 * @filename  spgrc.c
 *
 * @purpose   Code related to graceful restart
 *
 * @component  Routing OSPF Component
 *
 * @create     12/18/2008
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
#include "osapi_trace.h"
#include "ospf_vend_ctrl.h"
#ifdef L7_NSF_PACKAGE
#include "ospf_ckpt.h"
#endif

extern struct ospfMapCtrl_s  ospfMapCtrl_g;


static e_Err o2CkptNeighborKill(t_NBO *p_NBO);


/*********************************************************************
* @purpose  Print graceful restart debug statistics 
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void o2GracefulRestartStatsPrint(void)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if (p_RTO == NULL)
    return;

  printf("\nHelpful neighbor stats:");

  printf("\nNumber of grace LSAs received............................%u", 
         p_RTO->grStats.helpfulNbrStats.graceLsasReceived);

  printf("\nNumber of times this router entered helper mode..........%u", 
         p_RTO->grStats.helpfulNbrStats.enterHelperMode);

  printf("\nNumber of times this router exited helper mode...........%u", 
         p_RTO->grStats.helpfulNbrStats.exitHelperMode);

  printf("\nUpdated the grace period for a neighbor..................%u", 
         p_RTO->grStats.helpfulNbrStats.gracePeriodUpdate);

  printf("\nNumber of grace LSAs rx'd from nbr not in FULL state.....%u", 
         p_RTO->grStats.helpfulNbrStats.graceLsaNbrNotFull);

  printf("\nTopology change pending when grace LSA received..........%u", 
         p_RTO->grStats.helpfulNbrStats.topoChangePending);

  printf("\nGrace LSA received after grace period already expired....%u", 
         p_RTO->grStats.helpfulNbrStats.staleGraceLsa);

  printf("\nGrace LSA received while own restart in progress.........%u", 
         p_RTO->grStats.helpfulNbrStats.ownRestartInProgress);

  printf("\nNo restart reason included in grace LSA..................%u", 
         p_RTO->grStats.helpfulNbrStats.noRestartReason);

  printf("\nLocal policy prohibited this router from being helpful...%u", 
         p_RTO->grStats.helpfulNbrStats.localPolicyConflict);

  printf("\nFailure entering helper mode for a neighbor..............%u", 
         p_RTO->grStats.helpfulNbrStats.failedToEnterHelperMode);

  printf("\nHelper mode exited because the grace period expired......%u", 
         p_RTO->grStats.helpfulNbrStats.gracePeriodExpire);

  printf("\nTried to exit helper mode for neighbor not restarting....%u", 
         p_RTO->grStats.helpfulNbrStats.unexpectedExit);

  printf("\n\nRestarting router stats:");

  printf("\nNumber of planned restarts...............................%u",
         p_RTO->grStats.restartingRtrStats.plannedRestarts);

  printf("\nNumber of unplanned restarts.............................%u",
         p_RTO->grStats.restartingRtrStats.unplannedRestarts);

  printf("\nNumber of grace LSAs sent................................%u",
         p_RTO->grStats.restartingRtrStats.graceLsasSent);

  printf("\nError building grace LSA.................................%u",
         p_RTO->grStats.restartingRtrStats.graceLsaBuildFail);

  printf("\nError sending grace LSA..................................%u",
         p_RTO->grStats.restartingRtrStats.graceLsaSendFail);

  printf("\nGrace period expired.....................................%u",
         p_RTO->grStats.restartingRtrStats.gracePeriodExpire);

  printf("\nSuccessfully completed graceful restart..................%u",
         p_RTO->grStats.restartingRtrStats.successfulGracefulRestart);

  printf("\nUnsuccessful graceful restart............................%u",
         p_RTO->grStats.restartingRtrStats.failedGracefulRestart);
}

/*********************************************************************
* @purpose  Reset graceful restart debug statistics 
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void o2GracefulRestartStatsReset(void)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if (p_RTO == NULL)
    return;

  p_RTO->grStats.helpfulNbrStats.graceLsasReceived = 0;
  p_RTO->grStats.helpfulNbrStats.graceLsaNbrNotFull = 0;
  p_RTO->grStats.helpfulNbrStats.topoChangePending = 0;
  p_RTO->grStats.helpfulNbrStats.staleGraceLsa = 0;
  p_RTO->grStats.helpfulNbrStats.ownRestartInProgress = 0;
  p_RTO->grStats.helpfulNbrStats.noRestartReason = 0;
  p_RTO->grStats.helpfulNbrStats.localPolicyConflict = 0;
  p_RTO->grStats.helpfulNbrStats.gracePeriodUpdate = 0;
  p_RTO->grStats.helpfulNbrStats.failedToEnterHelperMode = 0;
  p_RTO->grStats.helpfulNbrStats.enterHelperMode = 0;
  p_RTO->grStats.helpfulNbrStats.exitHelperMode = 0;
  p_RTO->grStats.helpfulNbrStats.gracePeriodExpire = 0;
  p_RTO->grStats.helpfulNbrStats.unexpectedExit = 0;

  p_RTO->grStats.restartingRtrStats.plannedRestarts = 0;
  p_RTO->grStats.restartingRtrStats.unplannedRestarts = 0;
  p_RTO->grStats.restartingRtrStats.graceLsasSent = 0;
  p_RTO->grStats.restartingRtrStats.graceLsaBuildFail = 0;
  p_RTO->grStats.restartingRtrStats.graceLsaSendFail = 0;
  p_RTO->grStats.restartingRtrStats.gracePeriodExpire = 0;
  p_RTO->grStats.restartingRtrStats.successfulGracefulRestart = 0;
  p_RTO->grStats.restartingRtrStats.failedGracefulRestart = 0;
}

/*********************************************************************
* @purpose  graceful restart debug info
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ospfDebugGracefulRestart(void)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  e_Err e;
  t_IFO *p_IFO;
  t_NBO *p_NBO;
  t_ARO *p_ARO;

  if (p_RTO == NULL)
    return;

  if (p_RTO->restartStatus == OSPF_GR_UNPLANNED_RESTART)
    printf("\nOSPF in unplanned graceful restart");
  else if (p_RTO->restartStatus == OSPF_GR_PLANNED_RESTART)
    printf("\nOSPF in planned graceful restart");
  else
    printf("\nOSPF not restarting");

  if (p_RTO->Cfg.NsfSupport == OSPF_NSF_NONE)
    printf("\nGraceful restart disabled");
  else if (p_RTO->Cfg.NsfSupport == OSPF_NSF_PLANNED_ONLY)
    printf("\nGraceful restart for planned restarts only.");
  else if (p_RTO->Cfg.NsfSupport == OSPF_NSF_ALWAYS)
    printf("\nGraceful restart for planned and unplanned restarts");

  printf("\nGraceful restart interval:  %u seconds", (L7_uint32) p_RTO->Cfg.RestartInterval);

  printf("\nHelper support:  ");
  switch (p_RTO->Cfg.HelperSupport)
  {
    case OSPF_HELPER_NONE:          printf("Never");         break;
    case OSPF_HELPER_PLANNED_ONLY:  printf("Planned only");  break;
    case OSPF_HELPER_ALWAYS:        printf("Always");        break;
    default:                        printf("Invalid");
  }

  if (p_RTO->Cfg.StrictLsaChecking)
    printf("\nStrict LSA checking enabled");
  else
    printf("\nStrict LSA checking disabled");

  e = HL_GetFirst(p_RTO->AroHl, (void*) &p_ARO);
  while ((e == E_OK) && p_ARO)
  {
    e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(e == E_OK && p_IFO)
    {
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(e == E_OK && p_NBO)
      {
        if (o2NeighborIsRestarting(p_NBO))
        {
          L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
          printf("\n\nNeighbor at %s is restarting", nbrAddrStr);
          printf("\n  Restart reason:  %u", p_NBO->restartReason);
          printf("\n  Grace period:  %u seconds", p_NBO->gracePeriod);
          printf("\n  Expires in %u seconds", 
                 (unsigned int) TIMER_ToExpireSec(p_NBO->gracePeriodTimer));
        }
        else if (p_NBO->helperExitReason != OSPF_HELPER_EXIT_NONE)
        {
          L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
          printf("\n\nNeighbor at %s last exited helper mode with reason %s",
                 nbrAddrStr, o2HelperExitReason(p_NBO->helperExitReason));
        }
        e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
  e = HL_GetNext(p_RTO->AroHl, (void*) &p_ARO, p_ARO);
  }
}

/*********************************************************************
 * @purpose      Do full checkpoint
 *
 * @param        RTO_Id   @b{(input)}  RTO Object handle
 *
 * @returns      E_OK
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
e_Err o2FullCheckpoint(t_Handle RTO_Id)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  e_Err ea, ei, en;
  t_IFO *p_IFO;
  t_ARO *p_ARO;
  t_NBO *p_NBO;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_CKPT)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, "Initiating full checkpoint.");
    RTO_TraceWrite(traceBuf);
  } 

  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      /* Checkpoint DR and BDR on broadcast interfaces */
      if ((p_IFO->Cfg.Type == IFO_BRDC) && p_RTO->Clbk.f_DrElected)
      {
        (p_RTO->Clbk.f_DrElected)(p_IFO);
      }
      en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
      while (en == E_OK)
      {
        if (p_NBO->State == NBO_FULL)
        {
          /* Checkpoint full neighbors */
          if (p_RTO->Clbk.f_NeighborFullInd)
          {
            (p_RTO->Clbk.f_NeighborFullInd)(p_NBO);
          }   
        }
        en = HL_GetNext(p_IFO->NboHl, (void**)&p_NBO, p_NBO);
      }
      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }
  return E_OK;
}

/*********************************************************************
 * @purpose      Send grace LSAs in preparation for a planned restart
 *
 * @param        RTO_Id   @b{(input)}  RTO Object handle
 *
 * @returns      E_OK
 *
 * @notes        Send a grace LSA on each interface where there is at least
 *               one fully adjacency neighbor.
 *
 * @end
 * ********************************************************************/
e_Err o2RestartInitiate(t_Handle RTO_Id)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  e_Err ea, ei, en;
  t_IFO *p_IFO;
  t_ARO *p_ARO;
  t_NBO *p_NBO;

  p_RTO->grStats.restartingRtrStats.plannedRestarts++;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, "Sending grace LSAs prior to planned restart.");
    RTO_TraceWrite(traceBuf);
  } 

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
          "Starting planned OSPFv2 graceful restart");

  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
      while (en == E_OK)
      {
        if (p_NBO->State == NBO_FULL)
        {
          o2GraceLsaOriginate(p_IFO, O2_GR_REASON_SW_RESTART);
          break;   /* next interface */
        }
        en = HL_GetNext(p_IFO->NboHl, (void**)&p_NBO, p_NBO);
      }
      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }
  OSAPI_TRACE_EVENT(L7_TRACE_EVENT_OSPF2_GRACE_LSAS_SENT,
                    (L7_uchar8 *)L7_NULLPTR, 0);
  return E_OK;
}

/*********************************************************************
 * @purpose      Originate a grace LSA on a given interface with a given
 *               restart reason.
 *
 * @param        p_IFO           @b{(input)}  interface
 * @param        restartReason   @b{(input)}   reason for the restart
 *
 * @returns      E_OK
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
e_Err o2GraceLsaOriginate(t_IFO *p_IFO, e_o2RestartReason restartReason)
{
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  L7_uint32 opaqueId = 0;
  L7_uchar8 data[O2_GRACE_LSA_LEN];
  L7_uchar8 *pos = data;
  L7_uint32 bytesWritten = 0;
  e_Err e;

  if (!p_RTO->Cfg.AdminStat)
  {
    return E_FAILED;
  }

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    char ifName[L7_NIM_IFNAME_SIZE + 1];
    RTO_InterfaceName(p_IFO, ifName);
    sprintf(traceBuf, "Sending grace LSA on interface %s.", ifName);
    RTO_TraceWrite(traceBuf);
  } 

  memset(data, 0, O2_GRACE_LSA_LEN);

  bytesWritten = o2GraceLsaTlvAdd(pos, O2_GR_TLV_GRACE_PERIOD, O2_GRACE_PERIOD_TLV_LEN, 
                                  p_RTO->Cfg.RestartInterval); 
  pos += bytesWritten;

  bytesWritten = o2GraceLsaTlvAdd(pos, O2_GR_TLV_RESTART_REASON, O2_RESTART_REASON_TLV_LEN,
                                  restartReason);
  pos += bytesWritten;

  bytesWritten = o2GraceLsaTlvAdd(pos, O2_GR_TLV_IP_ADDRESS, O2_IP_ADDRESS_TLV_LEN,
                                  p_IFO->Cfg.IpAdr);
  pos += bytesWritten;

  /* Sanity check */
  if ((pos - data) != O2_GRACE_LSA_LEN)
  {
    char ifName[L7_NIM_IFNAME_SIZE + 1];
    RTO_InterfaceName(p_IFO, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Wrote %u bytes to grace LSA for interface %s. Expected to write %u bytes.",
            (L7_uint32) (pos - data), ifName, O2_GRACE_LSA_LEN);
    p_RTO->grStats.restartingRtrStats.graceLsaBuildFail++;
    return E_FAILED;
  }

  e = IFO_OpaqueGenerate(p_IFO, OPAQUE_LSA_TYPE_GRACE, opaqueId, data, O2_GRACE_LSA_LEN);
  if (e != E_OK)
  {
    p_RTO->grStats.restartingRtrStats.graceLsaSendFail++;
    return E_FAILED;
  }

  p_RTO->grStats.restartingRtrStats.graceLsasSent++;
  return E_OK;
}

/*********************************************************************
 * @purpose      Add a TLV to a grace LSA
 *
 * @param        pos        @b{(input)}  location in grace LSA to write
 * @param        tlvType    @b{(input)}  type code for TLV
 * @param        tlvLen     @b{(input)}  length of TLV in bytes, not incl. type and len fields
 * @param        tlvValue   @b{(input)}  data
 * 
 *
 * @returns      number of bytes written to LSA
 *
 * @notes        For now, all TLVs have integer value
 *
 * @end
 * ********************************************************************/
L7_uint32 o2GraceLsaTlvAdd(L7_uchar8 *pos, L7_ushort16 tlvType, L7_ushort16 tlvLen,
                           L7_uint32 tlvValue)
{
  L7_uchar8 *start = pos;
  L7_uint32 bytesWritten;

  A_SET_2B(tlvType, pos);
  pos += 2;
  A_SET_2B(tlvLen, pos);
  pos += 2;

  switch (tlvLen)
  {
    case 1:  *pos = (L7_uchar8) tlvValue; break;
    case 2:  A_SET_2B(tlvValue, pos); break;
    case 3:  A_SET_3B(tlvValue, pos); break;
    case 4:  A_SET_4B(tlvValue, pos); break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Unexpected TLV length %u for grace LSA.", tlvLen);
      return 0;
  }
  pos += tlvLen;

  /* Maintain 4 byte alignment */
  bytesWritten = (L7_uint32) (pos - start);
  if (bytesWritten % 4) 
  {
    bytesWritten += (4 - (bytesWritten % 4));
  }
  return bytesWritten;
}

/*********************************************************************
 * @purpose      Determine whether OSPF is waiting for ACK of any grace LSAs.
 *
 * @param        RTO_Id              @b{(input)}  OSPF instance
 * @param        graceLsasPending    @b{(output)} the answer
 * 
 * @returns      E_OK
 *
 * @notes        For now, all TLVs have integer value
 *
 * @end
 * ********************************************************************/
e_Err o2GraceLsasPending(t_Handle RTO_Id, L7_BOOL *graceLsasPending)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;

  e_Err ea, ei, en;
  t_IFO *p_IFO;
  t_ARO *p_ARO;
  t_NBO *p_NBO;

  *graceLsasPending = L7_FALSE;
  
  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
      while (en == E_OK)
      {
        if ((p_NBO->State == NBO_FULL) && o2GraceLsaOnRetxList(p_NBO))
        {
          if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
          {
            char traceBuf[OSPF_MAX_TRACE_STR_LEN];
            char ifName[L7_NIM_IFNAME_SIZE + 1];
            L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
            osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
            RTO_InterfaceName(p_IFO, ifName);
            sprintf(traceBuf, "Grace LSA on retx list for neighbor %s on interface %s.",
                    nbrAddrStr, ifName);
            RTO_TraceWrite(traceBuf);
          } 
          *graceLsasPending = L7_TRUE;
          return E_OK;
        }
        en = HL_GetNext(p_IFO->NboHl, (void**)&p_NBO, p_NBO);
      }
      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }
  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    RTO_TraceWrite("All neighbors have acknowledged grace LSA");
  } 
  return E_OK;
}

/*********************************************************************
 * @purpose      This function gets invoked when OSPF is starting a
 *               graceful restart. Determine whether the restart is planned
 *               or unplanned and record the type of restart being done.
 *
 * @param        p_RTO    OSPF instance
 * 
 * @returns      E_OK
 *
 * @notes        If a restart interval end is checkpointed, this is a planned
 *               restart. Otherwise, this is an unplanned restart. The system
 *               could be doing a planned restart and for some reason our 
 *               checkpoint table doesn't have a grace period end. In that case
 *               if configured to only do GR on planned restarts, don't do GR.
 *               
 *
 * @end
 * ********************************************************************/
e_Err o2BeginGracefulRestart(t_RTO *p_RTO)
{
#ifdef L7_NSF_PACKAGE
  L7_RC_t rc = L7_FAILURE;

  /* If there are no checkpointed neighbors, then skip GR. */
  if (o2CkptNeighborsCount() > 0)
  {
    if ((p_RTO->Cfg.NsfSupport != OSPF_NSF_NONE) && o2CkptGracePeriodPending())
    {
      rc = o2PlannedGrEnter(p_RTO);
    }
    else if (p_RTO->Cfg.NsfSupport == OSPF_NSF_ALWAYS)
    {
      rc = o2UnplannedGrEnter(p_RTO);
    }
  }

  /* If no GR or there was an error entering GR, clear the checkpoint table */
  if (rc != L7_SUCCESS)
  {
    o2CkptTableClear();
  }
  return rc;
#else
  return E_OK; 
#endif
}

/*********************************************************************
 * @purpose      Enter planned graceful restart.
 *
 * @param        p_RTO - OSPF instance
 * 
 * @returns      E_OK
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
e_Err o2PlannedGrEnter(t_RTO *p_RTO)
{
  L7_uint32 gracePeriodEnd = 0;
  L7_uint32 timeRemaining;

  /* set a timer for the end of the grace period */
#ifdef L7_NSF_PACKAGE
  gracePeriodEnd = o2CkptGracePeriodGet();
#endif
  if (gracePeriodEnd == 0)
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
    {
      RTO_TraceWrite("OSPFv2 aborting graceful restart. "
                     "Grace period end not checkpointed.");
    } 
    return E_FAILED;
  }

  timeRemaining = gracePeriodEnd - simSystemUpTimeGet();
  if (timeRemaining <= 0)
  {
    /* missed the window */
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
    {
      RTO_TraceWrite("OSPFv2 grace interval already expired on restart");
    } 
    return E_OK;
  }

  p_RTO->grStats.restartingRtrStats.plannedRestarts++;

  p_RTO->restartStatus = OSPF_GR_PLANNED_RESTART;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, "OSPFv2 initializing with a planned graceful restart. "
            "Restart interval expires in %u seconds.",
            timeRemaining);
    RTO_TraceWrite(traceBuf);
  } 

  if (TIMER_StartSec(p_RTO->gracePeriodTimer, (L7_uint32) timeRemaining,
                     L7_FALSE, o2GracePeriodTimerExp, 
                     p_RTO->OspfSysLabel.threadHndle) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to set grace period timer");
    return E_FAILED;
  }

  return E_OK;
}

/*********************************************************************
 * @purpose      Enter unplanned graceful restart.
 *
 * @param        p_RTO - OSPF instance
 * 
 * @returns      E_OK
 *
 * @notes        If we get here, we know there is checkpointed data (so GR must
 *               have been configured prior to restart). Since the  
 *               checkpoint table does not contain a grace period, we assume 
 *               OSPF did not send grace LSAs before the restart.  
 *
 * @end
 * ********************************************************************/
e_Err o2UnplannedGrEnter(t_RTO *p_RTO)
{
  p_RTO->grStats.restartingRtrStats.unplannedRestarts++;
  p_RTO->restartStatus = OSPF_GR_UNPLANNED_RESTART;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, 
            "OSPFv2 initializing with an unplanned graceful restart "
            "with grace period %u seconds.", 
            (L7_uint32) p_RTO->Cfg.RestartInterval);
    RTO_TraceWrite(traceBuf);
  } 

  if (TIMER_StartSec(p_RTO->gracePeriodTimer, (L7_uint32) p_RTO->Cfg.RestartInterval,
                     L7_FALSE, o2GracePeriodTimerExp, 
                     p_RTO->OspfSysLabel.threadHndle) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to set grace period timer");
    return E_FAILED;
  }

  return E_OK;
}

/*********************************************************************
 * @purpose      Once all OSPF interfaces are up, check if any pre-restart 
 *               neighbors were on an interface that did not come back. If 
 *               so and we are in a graceful restart, terminate the restart
 *               with a topology change event.
 *
 * @param        p_RTO - RTO instance
 * 
 * @returns      E_OK
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
e_Err o2StartupDone(t_RTO *p_RTO)
{
#ifdef L7_NSF_PACKAGE
  o2CkptNeighbor_t nbr;
  L7_RC_t rc;
  L7_uint32 intIfNum;    /* internal interface number of neighbor */
  t_IFO *p_IFO = NULL;
  t_ARO *p_ARO = NULL;
  L7_uint32 transitArea;

  if (p_RTO->restartStatus == OSPF_GR_NOT_RESTARTING)
    return E_OK;

  rc = o2CkptNeighborGetFirst(&nbr);
  while (rc == L7_SUCCESS)
  {
    /* Terminate GR if normal neighbor's interface did not come back */
    if (nbr.neighborType == O2_CKPT_NORMAL_NBR)
    {
      intIfNum = nbr.data.intIfNum;
      if ((HL_FindFirst(p_RTO->IfoIndexHl, (byte*) &intIfNum, 
                        (void**) &p_IFO) != E_OK) ||
          (p_IFO->Cfg.State == IFO_DOWN))
      {
        if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
        {
          char traceBuf[OSPF_MAX_TRACE_STR_LEN];
          char ifName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(nbr.neighborAddr, nbrAddrStr);
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          sprintf(traceBuf, "Pre-restart neighbor %s on interface %s, which is down. "
                  "Terminating graceful restart.", nbrAddrStr, ifName);
          RTO_TraceWrite(traceBuf);
        } 
        o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
        return E_OK;
      }
    }

    else if (nbr.neighborType == O2_CKPT_VIRTUAL_NBR)
    {
      Bool upIfFound = FALSE;
      e_Err e;

      transitArea = nbr.data.transitAreaId;
      if (HL_FindFirst(p_RTO->AroHl, (byte*) &transitArea, 
                       (void**) &p_ARO) == E_OK)
      {
        if (p_ARO->OperationState)
        {
          e = HL_GetFirst(p_ARO->IfoHl, (void **)&p_IFO);
          while ((e == E_OK) && !upIfFound)
          {
            if ((p_IFO->Cfg.State != IFO_DOWN) && (p_IFO->Cfg.Type != IFO_VRTL))
            {
              upIfFound = TRUE;
            }
            e = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
          }
        }
      }
      if (!upIfFound)
      {
        if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
        {
          char traceBuf[OSPF_MAX_TRACE_STR_LEN];
          L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(nbr.routerId, nbrIdStr);
          osapiInetNtoa(transitArea, areaIdStr);
          sprintf(traceBuf, "Pre-restart virtual neighbor %s on transit area %s."
                  "No interfaces in transit area. Terminating graceful restart", 
                  nbrIdStr, areaIdStr);
          RTO_TraceWrite(traceBuf);
        }
        o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
        return E_OK;
      }
    }
    rc = o2CkptNeighborGetNext(&nbr, &nbr);
  }
  return E_OK;
#else
  return E_FAILED;
#endif
}

/*********************************************************************
 * @purpose      Send grace LSAs on unplanned graceful restart
 *
 * @param        p_IFO - interface that has just come up
 * 
 * @returns      E_OK
 *
 * @notes        Called when interface comes up while in an unplanned GR.
 *               When all neighbors on an interface have ACK'd grace LSA,
 *               we bounce the interface. When this bounce occurs, 
 *               sendGraceLsas is L7_FALSE.
 *
 * @end
 * ********************************************************************/
e_Err o2GrIfoUp(t_IFO *p_IFO)
{
#ifdef L7_NSF_PACKAGE
   t_RTO *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   o2CkptNeighbor_t nbr;
   t_NBO *p_NBO = NULL;
   L7_RC_t rc;

   if (p_IFO->graceLsaSent)
     return E_OK;

   /* Only send grace LSAs if there were neighbors before the restart */
   if (((p_IFO->Cfg.Type == IFO_VRTL) &&
        !o2CkptVirtualNeighbor(p_IFO->Cfg.VirtIfNeighbor, p_IFO->Cfg.VirtTransitAreaId)) || 
       ((p_IFO->Cfg.Type != IFO_VRTL) && 
        (o2CkptNeighborsIntfCount(p_IFO->Cfg.IfIndex) == 0)))
   {
     if (TIMER_Active(p_IFO->grSuppressHelloTimer))
     {
        TIMER_Stop(p_IFO->grSuppressHelloTimer);
     }
     p_IFO->grSuppressHellos = L7_FALSE;
     return E_OK;
   }

   /* If the wait timer is running, kill it. */
   if (TIMER_Active(p_IFO->WaitTimer))
   {
     TIMER_Stop(p_IFO->WaitTimer);
   }

   /* Create a temporary neighbor for each checkpointed neighbor on 
    * this interface. We'll use the neighbors to tx and retx grace LSAs. */
   rc = o2CkptNeighborGetFirst(&nbr);
   while (rc == L7_SUCCESS)
   {
     if (((p_IFO->Cfg.Type == IFO_VRTL) && 
          (nbr.neighborType == O2_CKPT_VIRTUAL_NBR) &&
          (nbr.data.transitAreaId == p_IFO->Cfg.VirtTransitAreaId) &&
          (nbr.routerId == p_IFO->Cfg.VirtIfNeighbor)) ||
         ((nbr.neighborType == O2_CKPT_NORMAL_NBR) &&
           (nbr.data.intIfNum == p_IFO->Cfg.IfIndex)))
     {
       if (NBO_Init(p_IFO, nbr.routerId, nbr.neighborAddr, &p_IFO->Cfg, 
                    (t_Handle*) &p_NBO) == E_OK)
       {
         if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
         {
           char traceBuf[OSPF_MAX_TRACE_STR_LEN];
           L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 neighborAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
           char ifName[L7_NIM_IFNAME_SIZE + 1];
           RTO_InterfaceName(p_IFO, ifName);
           osapiInetNtoa(nbr.routerId, nbrIdStr);
           osapiInetNtoa(nbr.neighborAddr, neighborAddrStr);
           sprintf(traceBuf, "Created checkpoint neighbor %s at %s on interface %s", 
                   nbrIdStr, neighborAddrStr, ifName);
           RTO_TraceWrite(traceBuf);
         } 

         p_NBO->ckptNeighbor = TRUE;
         /* Assume neighbor supports opaque LSAs. No harm done if he doesn't. */
         p_NBO->Options |= OSPF_OPT_O_BIT;
         p_NBO->State = NBO_FULL;
       }
     }
     rc = o2CkptNeighborGetNext(&nbr, &nbr);
   }

   /* Set the IFO state based on checkpointed DR info. This affects destination
    * address of grace LSAs.  */
   if (o2CkptInterfaceDrGet(p_IFO->Cfg.IfIndex, (L7_uint32*) &p_IFO->DrId, NULL, 
                            (L7_uint32*) &p_IFO->BackupId, NULL))
   {
     if (p_IFO->DrId == p_RTO->Cfg.RouterId)
       p_IFO->Cfg.State = IFO_DR;
     else if (p_IFO->BackupId == p_RTO->Cfg.RouterId)
       p_IFO->Cfg.State = IFO_BACKUP;
     else
       p_IFO->Cfg.State = IFO_DROTHER;
   }
   else
   {
     /* Send grace LSA to all neighbors by default */
     p_IFO->DrId = p_RTO->Cfg.RouterId;
     p_IFO->Cfg.State = IFO_DR;
   }

   /* Finally send grace LSA */
   o2GraceLsaOriginate(p_IFO, O2_GR_REASON_UNKNOWN);
   p_IFO->graceLsaSent = L7_TRUE;

#endif
   return E_OK;
}

/*********************************************************************
 * @purpose      Allow hellos to be sent and received on interface once
 *               we have finished sending grace LSAs on an unplanned restart.
 *
 * @param        Id - interface 
 * 
 * @returns      E_OK
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
e_Err o2SuppressHelloTimerExp(t_Handle Id, t_Handle TimerId, word Flag)
{
  t_IFO *p_IFO = (t_IFO *) Id;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  e_Err en;
  t_NBO *p_NBO;
  t_NBO *nextNbo;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    char ifName[L7_NIM_IFNAME_SIZE + 1];
    RTO_InterfaceName(p_IFO, ifName);
    sprintf(traceBuf, "Hello suppression timer expired on interface %s", 
            ifName);
    RTO_TraceWrite(traceBuf);
  } 

  en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
  while (en == E_OK)
  {
    en = HL_GetNext(p_IFO->NboHl, (void**)&nextNbo, p_NBO);
    if (p_NBO->ckptNeighbor)
    {
      /* This neighbor hasn't ACK'd grace LSA. Stop waiting for ACK. */
      o2CkptNeighborKill(p_NBO);
    }
    p_NBO = nextNbo;
  }

  IFO_Down(p_IFO);
  p_IFO->grSuppressHellos = FALSE;
  IFO_Up(p_IFO);
      
  return E_OK;
  
}

/*********************************************************************
 * @purpose      Kill a temporary neighbor created from checkpoint data.
 *
 * @param        p_NBO - temporary neighbor
 * 
 * @returns      E_OK
 *
 * @notes        Don't do NBO_KILL. Want to avoid some things done on a 
 *               normal NBO_KILL through the state transition table.
 *
 * @end
 * ********************************************************************/
static e_Err o2CkptNeighborKill(t_NBO *p_NBO)
{
  NBO_Delete(p_NBO, 1);
  return E_OK;
}

/*********************************************************************
 * @purpose      When a neighbor ACKs a grace LSA, delete the temporary 
 *               neighbor. If there are no more neighbors on the interface,
 *               return the interface to normal operation.
 *
 * @param        p_IFO - interface that has just come up
 * 
 * @returns      E_OK
 *
 * @notes        Since OSPF is discarding received hellos on this neighbor's 
 *               interface, the only neighbors on the interface will be 
 *               neighbors created from the checkpoint table.
 *
 * @end
 * ********************************************************************/
e_Err o2GrAckReceived(t_NBO *p_NBO)
{
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  word numNbrs = 0;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 neighborAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->RouterId, nbrIdStr);
    osapiInetNtoa(p_NBO->IpAdr, neighborAddrStr);
    sprintf(traceBuf, "Neighbor %s at %s acknowledged grace LSA", 
            nbrIdStr, neighborAddrStr);
    RTO_TraceWrite(traceBuf);
  } 

  o2CkptNeighborKill(p_NBO);

  if ((HL_GetEntriesNmb(p_IFO->NboHl, &numNbrs) == E_OK) &&
      (numNbrs == 0))
  {
    /* All neighbors on this interface have ack'd grace LSA */
    if (TIMER_Active(p_IFO->grSuppressHelloTimer))
      TIMER_Stop(p_IFO->grSuppressHelloTimer);

    IFO_Down(p_IFO);
    p_IFO->grSuppressHellos = FALSE;
    IFO_Up(p_IFO);

    if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      char ifName[L7_NIM_IFNAME_SIZE + 1];
      RTO_InterfaceName(p_IFO, ifName);
      sprintf(traceBuf, "All neighbors on interface %s have acknowledged grace LSA", 
              ifName);
      RTO_TraceWrite(traceBuf);
    } 
  }
  return E_OK;
}

/*********************************************************************
 * @purpose      Handle expiration of the grace period timer.
 *
 * @param        Id - OSPF instance
 * @param        TimerId - grace period timer
 * @param        Flag - unused
 * 
 * @returns      E_OK
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
e_Err o2GracePeriodTimerExp(t_Handle Id, t_Handle TimerId, word Flag)
{
   t_RTO *p_RTO = (t_RTO *) Id;
   
   p_RTO->grStats.restartingRtrStats.gracePeriodExpire++;
   o2GracefulRestartExit(p_RTO, OSPF_GR_TIMED_OUT);

   return E_OK;
}

/*********************************************************************
* @purpose  Exit graceful restart
*
* @param    p_RTO - OSPF instance
* @param    reason - why we are exiting graceful restart
*
* @returns  E_OK
*           E_NOT_FOUND - router not restarting
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2GracefulRestartExit(t_RTO *p_RTO, OSPF_GR_EXIT_REASON_t reason)
{
  t_ARO *p_ARO;
  t_IFO *p_IFO;
  t_RTB *p_RTB;
  t_NBO *p_NBO;
  t_NBO *nextNbo;
  e_Err e, ei, ea, en;
  t_S_AsExternalCfg *p_Entry = L7_NULLPTR;
  OSPF_GR_RESTART_STATUS_t restartStatus = p_RTO->restartStatus;

  if (!o2GracefulRestartInProgress(p_RTO))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Attempting to exit graceful restart, but no restart in progress. "
            "Exit reason: %s.", o2GrExitReason(reason));
    return E_NOT_FOUND; 
  }

  p_RTO->restartStatus = OSPF_GR_NOT_RESTARTING;
  p_RTO->grExitReason = reason;

  if (reason == OSPF_GR_COMPLETED)
    p_RTO->grStats.restartingRtrStats.successfulGracefulRestart++;
  else
    p_RTO->grStats.restartingRtrStats.failedGracefulRestart++;

  if (TIMER_Active(p_RTO->gracePeriodTimer))
  {
    TIMER_Stop(p_RTO->gracePeriodTimer);
  }

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    sprintf(traceBuf, "Exiting graceful restart. %s.", o2GrExitReason(reason));
    RTO_TraceWrite(traceBuf);
  } 

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
          "Exiting OSPFv2 graceful restart. %s.",
          o2GrExitReason(reason));

  /* If this was an unplanned restart, delete any leftover checkpoint 
   * neighbors and make sure hellos are no longer suppressed on any
   * interface. */
  if (restartStatus == OSPF_GR_UNPLANNED_RESTART)
  {
    ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
    while (ea == E_OK)
    {
      ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
      while (ei == E_OK)
      {
        en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
        while (en == E_OK)
        {
          en = HL_GetNext(p_IFO->NboHl, (void**) &nextNbo, p_NBO);
          if (p_NBO->ckptNeighbor)
          {
            if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
            {
              char traceBuf[OSPF_MAX_TRACE_STR_LEN];
              L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
              L7_uchar8 neighborAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
              char ifName[L7_NIM_IFNAME_SIZE + 1];
              RTO_InterfaceName(p_IFO, ifName);
              osapiInetNtoa(p_NBO->RouterId, nbrIdStr);
              osapiInetNtoa(p_NBO->IpAdr, neighborAddrStr);
              sprintf(traceBuf, "Neighbor %s at %s on interface %s never acknowledged grace LSA", 
                      nbrIdStr, neighborAddrStr, ifName);
              RTO_TraceWrite(traceBuf);
            } 
            o2CkptNeighborKill(p_NBO);
          }
          p_NBO = nextNbo;
        }
        if (p_IFO->grSuppressHellos)
        {
          IFO_Down(p_IFO);
          p_IFO->grSuppressHellos = FALSE;
          IFO_Up(p_IFO);
        }
        ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
      }
      ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
    }
  }

#ifdef L7_NSF_PACKAGE
  if (reason == OSPF_GR_TIMED_OUT)
  {
      /* Log the adjacencies that didn't come back. Doesn't matter if
       * we delete remaining checkpoint neighbors here or when we clear 
       * the checkpoint table. */
      o2CkptNeighbor_t nbr;
      while (o2CkptNeighborPop(&nbr) == L7_SUCCESS)
      {
        L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 *nbrTypeStr = "";
        osapiInetNtoa(nbr.routerId, nbrIdStr);
        osapiInetNtoa(nbr.neighborAddr, nbrAddrStr);
        if (nbr.neighborType == O2_CKPT_VIRTUAL_NBR)
          nbrTypeStr = "virtual ";
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "Pre-restart %sneighbor %s at %s not reacquired",
                nbrTypeStr, nbrIdStr, nbrAddrStr);
      }
    }
#endif

  /* Make sure self-originated LSAs are up to date */
  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    /* Reoriginate router LSA for each active area */
    LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      /* for each interface, reoriginate network LSA. If we are DR, 
       * this will reorig network LSA. If not DR, and we have originated
       * a network LSA, possibly before the restart, then this will flush it. */
      LsaReOriginate[S_NETWORK_LSA](p_ARO, (ulng) p_IFO);

      /* if virtual neighbor, reoriginate router LSA for transit area */
      if (p_IFO->Cfg.Type == IFO_VRTL)
      {
        LsaReOriginate[S_ROUTER_LSA]((t_ARO*) p_IFO->TransitARO, 0);
      }

      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }

    /* Originate default route into stub areas */
    if (routerIsBorder(p_RTO))
    {
      if (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL)
      {
        StubDefaultLsaOriginate(p_ARO);
      }
      else if (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA)
      {
        NssaDefaultLsaOriginate(p_ARO);
      }
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }

  /* Originate T5 and T7 LSAs from route redistribution */
  e = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_Entry);
  while ((e == E_OK) && !p_RTO->IsOverflowed)
  {
    p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
    LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);
    LsaOrgNssaBulk(p_RTO, p_Entry);
    e = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_Entry, p_Entry);
  }

  /* At this point, our T1 and T2 LSAs should be correct. Run SPF to 
   * trigger reorigination of T3, T4, and translated T7 LSAs. We want to do a 
   * complete update to RTO. So clear existing routes. If an RTO update was 
   * in progress, terminate it. */
  e = HL_GetFirst(p_RTO->RtbHl,  (void**)&p_RTB);
  while (e == E_OK)
  {
    o2ClearRoutes(p_RTB, p_RTB->RtbRtBt);
    o2ClearRoutes(p_RTB, p_RTB->RtbNtBt);
    p_RTB->RtbUpdateState = RTB_UPD_DONE;
    if (TIMER_Active(p_RTB->RecalcTimer))
    {
      TIMER_Stop(p_RTB->RecalcTimer);
    }
    e = RTB_EntireTableCalc(p_RTB);
    if (e != E_OK)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failure %d computing OSPFv2 routes after exiting graceful restart", e);
    }

    e = HL_GetNext(p_RTO->RtbHl, (void**)&p_RTB, p_RTB);
  }

  /* Flushing unwanted pre-restart LSAs is deferred until after the RTO update 
   * is done in RTB_RtoUpdate() so that LSAs triggered by SPF are accounted for. */

#ifdef L7_NSF_PACKAGE
  /* Clear the checkpoint table */
  o2CkptTableClear();
#endif

  /* We suppressed updates to the checkpoint table during GR. So on exit
   * do a full checkpoint. */
  o2FullCheckpoint(p_RTO);

  return e;
}

/* Get string corresponding to graceful restart exit reason */
L7_uchar8 *o2GrExitReason(OSPF_GR_EXIT_REASON_t reason)
{
  switch (reason)
  {
    case OSPF_GR_EXIT_NONE:    return "Not attempted";
    case OSPF_GR_IN_PROGRESS:  return "In progress";
    case OSPF_GR_COMPLETED:    return "Completed";
    case OSPF_GR_TIMED_OUT:    return "Timed out";
    case OSPF_GR_TOPO_CHANGE:  return "Topology change";
    case OSPF_GR_MANUAL_CLEAR: return "Manual clear";
    default:                   return "Unknown";
  }
}

/*********************************************************************
* @purpose  Process new fully-adjacent neighbor during GR.
*
* @param    p_NBO - neighbor that has just reached FULL state while this
*                   router is in graceful restart
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2GrNeighborFull(t_NBO *p_NBO)
{
#ifdef L7_NSF_PACKAGE
  o2CkptNeighbor_t ckptNbr;
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;

  if (o2CkptNeighborsCount() == 0)
  {
    /* Perhaps this is a new neighbor, not known before restart */
    return E_OK;
  }

  ckptNbr.routerId = p_NBO->RouterId;
  ckptNbr.neighborAddr = p_NBO->IpAdr;
  if (p_IFO->Cfg.Type == IFO_VRTL)
  {
    ckptNbr.neighborType = O2_CKPT_VIRTUAL_NBR;
    ckptNbr.data.transitAreaId = p_IFO->TransitAreaId;
  }
  else
  {
    ckptNbr.neighborType = O2_CKPT_NORMAL_NBR;
    ckptNbr.data.intIfNum = p_IFO->Cfg.IfIndex;
  }
  if (o2CkptNeighborDelete(&ckptNbr) == L7_SUCCESS)
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 *nbrType = "";
      if (ckptNbr.neighborType == O2_CKPT_VIRTUAL_NBR)
        nbrType = "virtual ";
      osapiInetNtoa(p_NBO->RouterId, nbrIdStr);
      osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
      sprintf(traceBuf, "Pre-restart %sneighbor %s at %s returned to FULL state",
              nbrType, nbrIdStr, nbrAddrStr);
      RTO_TraceWrite(traceBuf);
    } 
  }

  if (o2CkptNeighborsCount() == 0)
  {
    /* All checkpointed neighbors have been reacquired. Exit GR successfully. */
    o2GracefulRestartExit(p_RTO, OSPF_GR_COMPLETED);
  }
#endif
  return E_OK;
}

/*********************************************************************
* @purpose  Flush pre-restart LSAs we no longer want to originate. 
*
* @param    p_RTO - OSPF instance
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err o2FlushAllPreRestartLsas(t_RTO *p_RTO)
{
  t_ARO *p_ARO;
  t_IFO *p_IFO;
  e_Err ei, ea;

  AVLH_Browse(p_RTO->AsExternalLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_RTO);
  AVLH_Browse(p_RTO->AsOpaqueLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_RTO);

  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    AVLH_Browse(p_ARO->RouterLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);
    AVLH_Browse(p_ARO->NetworkLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);
    AVLH_Browse(p_ARO->NetSummaryLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);
    AVLH_Browse(p_ARO->AsBoundSummaryLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);
    AVLH_Browse(p_ARO->NssaLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);
    AVLH_Browse(p_ARO->AreaOpaqueLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);

    /* Here's where we flush grace LSAs when gr is done */
    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      AVLH_Browse(p_IFO->LinkOpaqueLsaHl, (BROWSEFUNC)o2FlushPreRestartLsas, (ulng)p_ARO);
      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }

  return E_OK;
}


/*********************************************************************
 * @purpose         Flush pre-restart self-originated LSAs.
 *
 * @param  db       @b{(input)}  database entry
 * @param aroId     @b{(input)}  area. NULL for LSAs with AS flooding scope.
 *
 * @returns         TRUE
 *                  FALSE if function is called in error
 *
 * @notes           Assumes only one OSPF instance because of need to 
 *                  get instance pointer. Could pass p_RTO, but too many
 *                  calls to change for now.
 *
 * @end
 * ********************************************************************/
Bool o2FlushPreRestartLsas(t_Handle db, ulng prm)
{
  /* Assume a single OSPF instance for now */
  t_RTO       *p_RTO = (t_RTO *)RTO_List;
  t_A_DbEntry *p_DbEntry = (t_A_DbEntry *) db;

  if (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_PRE_RESTART))
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_GR)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsIdStr);
      osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), advRouterStr);
      sprintf(traceBuf, "Flushing OSPFv2 pre-restart %s. "
              "LSID: %s, Adv Router: %s, Age: %d, SeqNo: 0x%lx",
              lsaTypeNames[p_DbEntry->Lsa.LsType], lsIdStr, advRouterStr, 
              A_GET_2B(p_DbEntry->Lsa.LsAge), A_GET_4B(p_DbEntry->Lsa.SeqNum));
      RTO_TraceWrite(traceBuf);    
    }
    if (HasAsFloodingScope(p_DbEntry->Lsa.LsType))
      FlushAsLsa(p_DbEntry, prm);
    else
      FlushLsa(p_DbEntry, prm, FALSE);
  }
  return TRUE;
}

/*********************************************************************
 * @purpose         Process a received router LSA while in graceful restart.
 *                  If router LSA is inconsistent with pre-restart topology,
 *                  terminate GR.
 *
 * @param  p_DbEntry  @b{(input)}  database entry
 *
 * @returns         E_OK
 *
 * @notes           
 *
 * @end
 * ********************************************************************/
e_Err o2GrRouterLsaProcess(t_RTO *p_RTO, t_A_DbEntry *p_DbEntry)
{
#ifdef L7_NSF_PACKAGE
  t_S_RouterLsa *p_RouterLsa;
  t_ARO *p_ARO = (t_ARO*) p_DbEntry->ARO_Id;
  t_IFO *p_IFO;
  L7_uint32 areaId = p_ARO->AreaId;
  L7_uint32 advRouter = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);
  L7_uint32 linkType;
  L7_uint32 linkId;
  L7_uint32 linkData;
  L7_uint32 ifIndex;
  L7_uint32 drIpAddr;

  /* L7 TRUE if we had a virtual adj with adv router before restart. */
  L7_BOOL preRestartVirtAdj = L7_FALSE; 

  /* Don't do anything with our own router LSAs */
  if (p_RTO->Cfg.RouterId == advRouter)
    return E_OK;

  /* If we had a pre-restart adjacency with this router, but his router LSA 
   * no longer includes a link to us, then he is not being helpful. Terminate
   * graceful restart. */
  if (o2CkptNeighborInArea(advRouter, areaId, &ifIndex))
  {
    /* pre-restart adj with adv router on interface with ifIndex */
    if (HL_FindFirst(p_RTO->IfoIndexHl, (byte*) &ifIndex, (void**) &p_IFO) == E_OK)
    {
      if (p_IFO->Cfg.Type == IFO_BRDC)
      {
        /* Adjacency was via a broadcast interface */
        if (o2CkptInterfaceDrGet(ifIndex, NULL, &drIpAddr, NULL, NULL) == L7_SUCCESS)
        {
          /* We know the pre-restart DR on this interface */
          if (!o2RtrLsaHasLink(p_DbEntry, S_TRANSIT_NET, drIpAddr, 0))
          {
            /* Pre-restart neighbor's router LSA does not include this router
             * as a fully-adjacent neighbor. Neighbor is not being helpful. 
             * Terminate GR. */
            L7_uchar8 routerIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            char ifName[L7_NIM_IFNAME_SIZE + 1];
            RTO_InterfaceName(p_IFO, ifName);
            osapiInetNtoa(advRouter, routerIdStr);
            osapiInetNtoa(areaId, areaIdStr);
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                    "Lost pre-restart area %s interface %s adjacency with %s. "
                    "Exiting graceful restart.", 
                    areaIdStr, ifName, routerIdStr);
            o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
          }
        }
      }
      else if (p_IFO->Cfg.Type == IFO_PTP)
      {
        /* Adjacency was via a p2p interface */
        if (!o2RtrLsaHasLink(p_DbEntry, S_POINT_TO_POINT, p_RTO->Cfg.RouterId, 0))
        {
          /* Pre-restart neighbor's router LSA does not include this router
           * as a fully-adjacent neighbor. Neighbor is not being helpful. 
           * Terminate GR. */
          L7_uchar8 routerIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(advRouter, routerIdStr);
          osapiInetNtoa(areaId, areaIdStr);
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                  "Lost pre-restart area %s point-to-point adjacency with %s. "
                  "Exiting graceful restart.", 
                  areaIdStr, routerIdStr);
          o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
        }
      }
    }
  }

  if (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
  {
    /* Verify that backbone area router LSA has a virtual link for each
     * pre-restart virtual neighbor. */
    o2CkptNeighbor_t nbr;
    L7_RC_t rc = o2CkptNeighborGetFirst(&nbr);
    while (rc == L7_SUCCESS)
    {
      if ((nbr.routerId == advRouter) && (nbr.neighborType == O2_CKPT_VIRTUAL_NBR))
      {
        linkType = S_VIRTUAL_LINK;
        linkId = p_RTO->Cfg.RouterId;
        linkData = nbr.neighborAddr;
        if (!o2RtrLsaHasLink(p_DbEntry, linkType, linkId, linkData))
        {
          {
            L7_uchar8 routerIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 linkIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 linkDataStr[OSAPI_INET_NTOA_BUF_SIZE];
            osapiInetNtoa(advRouter, routerIdStr);
            osapiInetNtoa(linkId, linkIdStr);
            osapiInetNtoa(linkData, linkDataStr);
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                    "Lost pre-restart virtual adjacency with %s. "
                    "Backbone area router LSA has no virtual link with link ID %s "
                    "and link data %s. Exiting graceful restart.", 
                    routerIdStr, linkIdStr, linkDataStr);
            o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
          }
        }
      }
      rc = o2CkptNeighborGetNext(&nbr, &nbr);
    }
  }
  else
  {
    /* If the router LSA is for a non-backbone area, see if it is a transit
     * area for a pre-restart virtual neighbor. */
    preRestartVirtAdj = o2CkptVirtualNeighbor(advRouter, areaId);
    if (preRestartVirtAdj)
    {
      /* Verify that router LSA has bit V set */
      p_RouterLsa = (t_S_RouterLsa *) p_DbEntry->p_Lsa;
      if ((p_RouterLsa->Flags & S_VIRTUAL_FLAG) == 0)
      {
        L7_uchar8 routerIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(advRouter, routerIdStr);
        osapiInetNtoa(areaId, areaIdStr);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                "Lost pre-restart virtual adjacency with %s via transit area %s. "
                "Transit area router LSA bit V not set. Exiting graceful restart.", 
                routerIdStr, areaIdStr);
        o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
      }
    }
  }
  return E_OK;
#endif
  return E_OK;
}

/*********************************************************************
 * @purpose         Process a single link from a received router LSA.
 *                  If link is inconsistent with pre-restart topology,
 *                  terminate GR.
 *
 * @param  p_RTO      @b{(input)}  OSPF instance
 * @param  advRtrId   @b{(input)}  advertising router ID
 * @param  linkType   @b{(input)}  
 * @param  linkId     @b{(input)}  
 * @param  linkData   @b{(input)}  
 *
 * @returns         E_OK
 *
 * @notes      There are probably other cases I could check. The checks
 *             may allow us to terminate GR w/o waiting for the grace
 *             period to time out and so make recovery faster in the 
 *             event of a failure. But if failure is not detected here,
 *             then we'll still recover when the grace period ends.   
 *
 * @end
 * ********************************************************************/
e_Err o2GrRouterLinkProcess(t_RTO *p_RTO, L7_uint32 advRtrId, L7_uint32 linkType, 
                            L7_uint32 linkId, L7_uint32 linkData)
{
#ifdef L7_NSF_PACKAGE
  L7_uint32 intIfNum;
  L7_uint32 preRestartDrIpAddr;

  if (linkType == S_STUB_NET)
  {
    /* If we had pre-restart full adjacency with this router on this network,
     * the neighbor must not be honoring our graceful restart. Exit GR. */
    if (o2CkptNeighborNetMatch(advRtrId, linkId, linkData))
    {
      L7_uchar8 routerIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 networkStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 netmaskStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(advRtrId, routerIdStr);
      osapiInetNtoa(linkId, networkStr);
      osapiInetNtoa(linkData, netmaskStr);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
              "Neighbor %s on network %s/%s no longer fully adjacent. "
              "Exiting graceful restart.", 
              routerIdStr, networkStr, netmaskStr);
      o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
    }
  }
  else if (linkType == S_TRANSIT_NET)
  {
    if (o2CkptNeighborAddrMatch(advRtrId, linkData, &intIfNum))
    {
      /* Restarting router has an adjacency with this router at this 
       * address prior to restart. If restarting router was the
       * pre-restart DR and neighbor no longer considers us DR, 
       * then neighbor is not helping. */
      if (o2CkptInterfaceDrGet(intIfNum, NULL, &preRestartDrIpAddr, 
                               NULL, NULL) == L7_SUCCESS)
      {
        if (preRestartDrIpAddr && (preRestartDrIpAddr != linkId))
        {
          L7_uchar8 routerIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 drAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(advRtrId, routerIdStr);
          osapiInetNtoa(linkData, nbrAddrStr);
          osapiInetNtoa(linkId, drAddrStr);
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                  "Neighbor %s at %s no longer fully adjacent. "
                  "Restarting router was pre-restart DR. Neighbor considers %s DR. "
                  "Exiting graceful restart.", 
                  routerIdStr, nbrAddrStr, drAddrStr);
          o2GracefulRestartExit(p_RTO, OSPF_GR_TOPO_CHANGE);
        }
      }
    }
  }
#endif
  return E_OK;
}
