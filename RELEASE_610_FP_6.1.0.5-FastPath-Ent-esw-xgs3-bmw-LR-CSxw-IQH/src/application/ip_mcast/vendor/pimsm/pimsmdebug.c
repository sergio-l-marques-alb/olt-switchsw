/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDebug.c
*
* @purpose  debug and statistics related structs and APIs.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/

#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmintf.h"
#include "pimsmbsr.h"
#include "pimsmcandbsrfsm.h"
#include "pimsmnoncandbsrfsm.h"
#include "pimsm_vend_ctrl.h"

pimsmTraceFlags_t   pimsmCurrTraceLevel = PIMSM_TRACE_DISABLE;
pimsmDebugPathFlags_t pimsmCurrDebugPathBits = PIMSM_DEBUG_BIT_NONE;
L7_uchar8 pimsmAddr1[IPV6_DISP_ADDR_LEN];
L7_uchar8 pimsmAddr2[IPV6_DISP_ADDR_LEN];
L7_uchar8 pimsmAddr3[IPV6_DISP_ADDR_LEN];
L7_uchar8 pimsmAddr4[IPV6_DISP_ADDR_LEN];

L7_uint32 pimsmDebugMsgCnt = 0 ;
#define PIMSM_DEBUG_PRINTF sysapiPrintf

#define PIMSM_PRINT_ADDR(xaddr)           \
   {                                                     \
      L7_char8  __buf1__[PIMSM_MAX_MSG_SIZE];      \
           (void)inetAddrHtop((xaddr),__buf1__);              \
        PIMSM_DEBUG_PRINTF("%s \n",__buf1__);  \
   }  



/* for debugging purpose, store as strings */
L7_uchar8           pimsmDebugFlagName[PIMSM_DEBUG_MAX]
[PIMSM_STR_LEN] =
{
  "NONE", 
  "MACROS",
  "REGISTER RX/TX AND FSM", 
  "HELLO RX/TX",
  "JOIN RX/TX", 
  "(S,G,RPT)",
  "(S,G)", 
  "(*,G)",
  "(*,*,RP)", 
  "ASSERT FSM",
  "CAND-RP/MAPPINg", 
  "CAND-BSR/ELECTED-BSR",
  "BSR FSM", 
  "MFC INTERACTION",
  "MGMD INTERACTION", 
  "RTP INTERACTION",
  "UI INTERACTION", 
  "INTERFACE AND NEIGHBOR",  
  "MISC/ NONE OF THE ABOVE", 
  "ALL OF THE ABOVE"  
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmUpStrmSGStateName[PIMSM_UPSTRM_S_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NOT_JOINED", 
  "JOINED"
};

L7_uchar8           pimsmUpStrmSGEventName[PIMSM_UPSTRM_S_G_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE",
  "PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE",
  "PIMSM_UPSTRM_S_G_SM_EVENT_TIMER_EXPIRES",
  "PIMSM_UPSTRM_S_G_SM_EVENT_SEE_JOIN",
  "PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_S_G",
  "PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_S_G_RPT",
  "PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_STAR_G",
  "PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT",
  "PIMSM_UPSTRM_S_G_SM_EVENT_GEN_ID_CHANGED",
  "PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_DUE_TO_ASSERT"
};


/* for debugging purpose, store as strings */
L7_uchar8           pimsmUpStrmSGRptStateName
[PIMSM_UPSTRM_S_G_RPT_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "RPT_NOT_JOINED",
  "PRUNED_SG_RPT",
  "NOT_PRUNED_SG_RPT"
};

L7_uchar8           pimsmUpStrmSGRptEventName
[PIMSM_UPSTRM_S_G_RPT_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_RPT_JOIN_DESIRED_FALSE",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_INHERITED_OLIST_NOT_NULL",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_OVERRIDE_TIMER_EXPIRES",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_PRUNE",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_JOIN",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_PRUNE_S_G",
  "PIMSM_UPSTRM_S_G_RPT_SM_EVENT_RPF_CHANGED"
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmUpStrmStarGStateName
[PIMSM_UPSTRM_STAR_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NOT_JOINED",
  "JOINED"
};


L7_uchar8           pimsmUpStrmStarGEventName
[PIMSM_UPSTRM_STAR_G_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_TIMER_EXPIRES",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_SEE_JOIN",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_SEE_PRUNE",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_DUE_TO_ASSERT",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT",
  "PIMSM_UPSTRM_STAR_G_SM_EVENT_GEN_ID_CHANGED"
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmUpStrmStarStarRPStateName
[PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NOT_JOINED",
  "JOINED"
};
L7_uchar8           pimsmUpStrmStarStarRPEventName
[PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE",
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE",
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_TIMER_EXPIRES",
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_SEE_JOIN",
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_SEE_PRUNE",
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_NBR_CHANGED",
  "PIMSM_UP_STAR_STAR_RP_SM_EVENT_GEN_ID_CHANGED"
};  



/* for debugging purpose, store as strings */
L7_uchar8           pimsmPerScopeZoneCandBSRStateName
[PIMSM_CANDIDATE_BSR_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "CANDIDATE_BSR",
  "PENDING_BSR",
  "ELECTED_BSR"
};

L7_uchar8           pimsmPerScopeZoneCandBSREventName
[PIMSM_CANDIDATE_BSR_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_BSR_PERSCP_ZONE_CAND_RECV_PREFERRED_BSM",
  "PIMSM_BSR_PERSCP_ZONE_CAND_BOOTSTRAP_TIMER_EXPIRES",
  "PIMSM_BSR_PERSCP_ZONE_CAND_RECV_NON_PREFERRED_BSM",
  "PIMSM_BSR_PERSCP_ZONE_CAND_RECV_NON_PREFERRED_BSM_FROM_ELECTED_BSR"
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmPerScopeZoneNonCandBSRStateName    
[PIM_NON_CANDIDATE_BSR_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "ACCEPT_ANY",
  "ACCEPT_PREFERRED"
};

L7_uchar8           pimsmPerScopeZoneNonCandBSREventName
[PIMSM_NON_CANDIDATE_BSR_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_BSR_PER_SCOPE_ZONE_NON_CAND_SM_EVENT_RECV_BSM",
  "PIMSM_BSR_PER_SCOPE_ZONE_NON_CAND_SM_EVENT_SCOPEZONE_TIMER_EXPIRES",
  "PIMSM_BSR_PER_SCOPE_ZONE_NON_CAND_SM_EVENT_RECV_PREFERRED_BSM",
  "PIMSM_BSR_PER_SCOPE_ZONE_NON_CAND_SM_EVENT_BOOTSTRAP_TIMER_EXPIRES",
  "PIMSM_BSR_PER_SCOPE_ZONE_NON_CAND_SM_EVENT_RECV_NON_PREFERRED_BSM"
};

/* for debugging purpose, store as strings */

L7_uchar8           pimsmDnStrmPerIntfSGStateName
[PIMSM_DNSTRM_S_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "JOIN",
  "PRUNE_PENDING"
};

L7_uchar8           pimsmDnStrmPerIntfSGEventName
[PIMSM_DNSTRM_S_G_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_DNSTRM_PER_INTF_S_G_SM_EVENT_RECV_JOIN",
  "PIMSM_DNSTRM_PER_INTF_S_G_SM_EVENT_RECV_PRUNE",
  "PIMSM_DNSTRM_PER_INTF_S_G_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES",
  "PIMSM_DNSTRM_PER_INTF_S_G_SM_EVENT_EXPIRY_TIMER_EXPIRES"
};


/* for debugging purpose, store as strings */
L7_uchar8           pimsmDnStrmPerIntfSGRptStateName
[PIMSM_DNSTRM_S_G_RPT_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "PRUNE",
  "PRUNE_PENDING",
  "PRUNE_TMP",
  "PRUNE_PENDING_TMP"
};

L7_uchar8           pimsmDnStrmPerIntfSGRptEventName
[PIMSM_DNSTRM_S_G_RPT_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_DNSTRM_PER_INTF_S_G_RPT_SM_EVENT_RECV_JOIN_STAR_G",
  "PIMSM_DNSTRM_PER_INTF_S_G_RPT_SM_EVENT_RECV_JOIN_SG_RPT",
  "PIMSM_DNSTRM_PER_INTF_S_G_RPT_SM_EVENT_RECV_PRUNE_SG_RPT",
  "PIMSM_DNSTRM_PER_INTF_S_G_RPT_SM_EVENT_END_OF_MSG",
  "PIMSM_DNSTRM_PER_INTF_S_G_RPT_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES",
  "PIMSM_DNSTRM_PER_INTF_S_G_RPT_SM_EVENT_EXPIRY_TIMER_EXPIRES"
};


/* for debugging purpose, store as strings */
L7_uchar8           pimsmDnStrmPerIntfStarGStateName
[PIMSM_DNSTRM_STAR_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "JOIN",
  "PRUNE_PENDING"
};

L7_uchar8           pimsmDnStrmPerIntfStarGEventName
[PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_DNSTRM_PER_INTF_STAR_G_SM_EVENT_RECV_JOIN",
  "PIMSM_DNSTRM_PER_INTF_STAR_G_SM_EVENT_RECV_PRUNE",
  "PIMSM_DNSTRM_PER_INTF_STAR_G_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES",
  "PIMSM_DNSTRM_PER_INTF_STAR_G_SM_EVENT_EXPIRY_TIMER_EXPIRES"
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmDnStrmPerIntfStarStarRPStateName
[PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "JOIN",
  "PRUNE_PENDING"
};
L7_uchar8           pimsmDnStrmPerIntfStarStarRPEventName
[PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_DOWN_PER_INTF_STAR_STAR_RP_SM_EVENT_RECV_JOIN",
  "PIMSM_DOWN_PER_INTF_STAR_STAR_RP_SM_EVENT_RECV_PRUNE",
  "PIMSM_DOWN_PER_INTF_STAR_STAR_RP_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES",
  "PIMSM_DOWN_PER_INTF_STAR_STAR_RP_SM_EVENT_EXPIRY_TIMER_EXPIRES"
};
/* for debugging purpose, store as strings */
L7_uchar8           pimsmPerIntfSGAssertStateName
[PIMSM_ASSERT_S_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "ASSERT_WINNER",
  "ASSERT_LOSER"
};

L7_uchar8           pimsmPerIntfSGAssertEventName
[PIMSM_ASSERT_S_G_SM_EVENT_MAX][PIMSM_STR_LEN] =
{
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_ASSERT_RPT",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_DATA_PKT",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT",
  "PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TIMER_EXPIRES",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_PREF_ASSERT",
  "PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER",
  "PIMSM_ASSERT_S_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED",
  "PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TRACKING",
  "PIMSM_ASSERT_S_G_SM_EVENT_METRIC_BETTER_THAN_WINNER",
  "PIMSM_ASSERT_S_G_SM_EVENT_STOP_BEING_RPF_IFACE",
  "PIMSM_ASSERT_S_G_SM_EVENT_RECV_JOIN"
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmRegPerSGStateName
[PIMSM_REG_PER_S_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "JOIN",
  "JOIN_PENDING",
  "PRUNE"
};

L7_uchar8           pimsmRegPerSGEventName
[PIMSM_REG_PER_S_G_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_REG_PER_S_G_SM_EVENT_REG_STOP_TIMER_EXPIRES",
  "PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_TRUE",
  "PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_FALSE",
  "PIMSM_REG_PER_S_G_SM_EVENT_RECV_REG_STOP",
  "PIMSM_REG_PER_S_G_SM_EVENT_RP_CHANGED"
};

/* for debugging purpose, store as strings */
L7_uchar8           pimsmPerIntfStarGAssertStateName
[PIMSM_ASSERT_STAR_G_SM_STATE_MAX]
[PIMSM_STR_LEN] =
{
  "NO_INFO",
  "ASSERT_WINNER",
  "ASSERT_LOSER"
};

L7_uchar8           pimsmPerIntfStarGAssertEventName
[PIMSM_ASSERT_STAR_G_SM_EVENT_MAX]
[PIMSM_STR_LEN] =
{
  "PIMSM_ASSERT_STAR_G_RECV_INFERIOR_ASSERT_RPTBIT",
  "PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_DATA_PKT",
  "PIMSM_ASSERT_STAR_G_RECV_ACCEPTABLE_ASSERT_RPTBIT",
  "PIMSM_ASSERT_STAR_G_ASSERT_TIMER_EXPIRES",
  "PIMSM_ASSERT_STAR_G_RECV_INFERIOR_ASSERT",
  "PIMSM_ASSERT_STAR_G_RECV_PREFERRED_ASSERT",
  "PIMSM_ASSERT_STAR_G_COULD_ASSERT_FALSE",
  "PIMSM_ASSERT_STAR_G_RECV_ACCEPTABLE_ASSERT_FROM_CURR_WINNER",
  "PIMSM_ASSERT_STAR_G_RECV_INFERIOR_ASSERT_FROM_CURR_WINNER",
  "PIMSM_ASSERT_STAR_G_CURR_WINNER_GEN_ID_CHANGED_OR_NLT_EXPIRES",
  "PIMSM_ASSERT_STAR_G_ASSERT_TRACKING_DESIRED_FALSE",
  "PIMSM_ASSERT_STAR_G_MY_METRIC_BETTER_THAN_WINNER_METRIC",
  "PIMSM_ASSERT_STAR_G_SM_EVENT_STOP_BEING_RPF_IFACE",
  "PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_JOIN"
};          

#if 0
static void pimsmDebugBitSetToString(interface_bitset_t *pOif,
        L7_uchar8 *pStr)
{
  L7_uint32 index;
  L7_BOOL isEmpty = L7_TRUE;
  L7_uchar8 tempBuff[10];
  memset(pStr,0, PIMSM_STR_LEN);
  for (index = 0; index <= sizeof(interface_bitset_t); index++)
  {
    if ((BITX_TEST (pOif, index)) !=L7_NULL)
    {
      osapiSnprintf(tempBuff, 10, "%d ", index);
      osapiStrncat(pStr, tempBuff, sizeof(tempBuff));
      isEmpty = L7_FALSE;
    }
  }
  if (isEmpty == L7_TRUE)
  {
    osapiStrncpy(pStr, "NULL", sizeof("NULL"));
  }
}
#endif
static void pimsmDebugFlagsToString(L7_uchar8 flags,
        L7_uchar8 *pStr)
{
  L7_uchar8 tempBuff[10];
   memset(pStr, 0, MCAST_STRING_SIZE_MAX);  
   if ((flags & PIMSM_ADDED_TO_MFC) != L7_NULL)
   {
     osapiSnprintf(tempBuff, 10, "MFC ");
     osapiStrncat(pStr, tempBuff, sizeof(tempBuff));
   }
   if ((flags & PIMSM_NEW) != L7_NULL)
   {
     osapiSnprintf(tempBuff, 10, "NEW ");
     osapiStrncat(pStr, tempBuff, sizeof(tempBuff));
    }
   if ((flags & PIMSM_NULL_OIF) != L7_NULL)
   {
     osapiSnprintf(tempBuff, 10, "NULL_OIF ");
     osapiStrncat(pStr, tempBuff, sizeof(tempBuff));
   }
   if ((flags & PIMSM_NODE_DELETE) != L7_NULL)
   {
     osapiSnprintf(tempBuff, 10, "DELETED ");
     osapiStrncat(pStr, tempBuff, sizeof(tempBuff));
   }
}
/******************************************************************************
* @purpose  Display given neighbor list.
*
* @param        pimsmCb @b{(input)}
* @param    pimsmNbrList @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
static void pimsmDebugNbrShow (pimsmCB_t * pimsmCb,
                                    pimsmNeighborEntry_t *    currNbr)
{
  L7_uint32                 now, i;
  L7_uint32                 uptime;
  L7_uint32                 timeLeft = 0;

  PIMSM_DEBUG_PRINTF( "Interface Index : %d\n",
                      currNbr->pimsmNeighborIfIndex );        
  PIMSM_DEBUG_PRINTF( "Neighbor Address : " );
  PIMSM_PRINT_ADDR( &currNbr->pimsmNeighborAddrList.pimsmPrimaryAddress );
  now = osapiUpTimeRaw();
  uptime = now - currNbr->pimsmNeighborCreateTime;
  PIMSM_DEBUG_PRINTF( "Up Time: %d\n", uptime );
  appTimerTimeLeftGet( pimsmCb->timerCb,
                       currNbr->pimsmNeighborExpiryTimer,
                       &timeLeft );        
  PIMSM_DEBUG_PRINTF( "Expiry Time: %d\n", timeLeft );
  PIMSM_DEBUG_PRINTF( "Neighbor Generation Id: %d\n", 
                      currNbr->pimsmNeighborGenerationIDValue );
  PIMSM_DEBUG_PRINTF( "Neighbor DR Priority: %d\n", 
                      currNbr->pimsmNeighborDRPriority );
  PIMSM_DEBUG_PRINTF( "LAN Prune Delay Support: %s\n",
                      currNbr->pimsmNeighborLANPruneDelayPresent ? "TRUE":"FALSE");
  PIMSM_DEBUG_PRINTF( "Propagation Delay: %d\n",
  	                  currNbr->pimsmNeighborPropagationDelay);
  PIMSM_DEBUG_PRINTF( "Override Interval: %d\n",
  	                  currNbr->pimsmNeighborOverrideInterval);
  PIMSM_DEBUG_PRINTF( "Secondary Addr list: \n");

  for (i = 0;  i < PIMSM_MAX_NBR_SECONDARY_ADDR &&
             inetIsAddressZero(&currNbr->pimsmNeighborAddrList.pimsmSecondaryAddrList[i]) == L7_FALSE;
             i++)
  {
    if (currNbr->pimsmNeighborAddrList.pimsmSecondaryAddrActive[i] == L7_FALSE)
    {
        continue;
    }
    PIMSM_PRINT_ADDR(
      &currNbr->pimsmNeighborAddrList.pimsmSecondaryAddrList[i]);
  }
}
/******************************************************************************
* @purpose  Display given neighbor list.
*
* @param        pimsmCb @b{(input)}
* @param    pimsmNbrList @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
static void pimsmDebugNbrListShow ( pimsmCB_t * pimsmCb,
                                         pimsmInterfaceEntry_t * intfEntry )
{
  pimsmNeighborEntry_t *    currNbr;
  L7_RC_t rc;

  rc = pimsmNeighborGetFirst( pimsmCb, intfEntry, &currNbr );
  while(rc == L7_SUCCESS)
  {
    pimsmDebugNbrShow(pimsmCb, currNbr);
    PIMSM_DEBUG_PRINTF( "\n");
      rc = pimsmNeighborNextGet( pimsmCb, intfEntry, currNbr, &currNbr );
  }
}
/******************************************************************************
* @purpose  Display  neighbor list for the interface
*
* @param
* @param        rtrIfNum          @b{(input)}
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfNeighborListShow (L7_uchar8 family , L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t *   intfEntry = L7_NULLPTR;

  pimsmCB_t *               pimsmCb;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "failed to get control block \n" );
    return;
  }

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "interface entry is NULL \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Neighbor List : \n\n" );
  pimsmDebugNbrListShow( pimsmCb, intfEntry );
}


/******************************************************************************
* @purpose 
*
* @param
* @param        rtrIfNum          @b{(input)}
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmDebugRpCandidateShow(L7_uchar8 family)
{
  pimsmCB_t  *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t  grpAddr;
  L7_uchar8  strDump[40];
  L7_uint32  cnt, grpMask;
  L7_uint32  timeLeft;

  if ((pimsmCb = (pimsmCB_t*)pimsmMapProtocolCtrlBlockGet(family)) == L7_NULLPTR)
  {
     PIMSM_DEBUG_PRINTF("\n PIMSM Control Block Get Failed for Family - %d", family);
     return L7_FAILURE;
  }
 
  PIMSM_DEBUG_PRINTF("\n********  Candidate RP Info ********\n");
 
  if (inetAddrHtop(&(pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAddr),strDump) == L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n C-RP Address    : %s \n",strDump);

  PIMSM_DEBUG_PRINTF("\n C-RP RtrIfNum   : %d \n", pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpRtrIfNum);
  PIMSM_DEBUG_PRINTF("\n C-RP Priority   : %d \n", pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpPriority);
  PIMSM_DEBUG_PRINTF("\n C-RP HoldTime   : %d \n", pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpHoldTime);
  PIMSM_DEBUG_PRINTF("\n C-RP HashMakLen : %d \n", pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpHashMaskLen);
  if (pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer != L7_NULLPTR)
  {
    if (appTimerTimeLeftGet(pimsmCb->timerCb,
                            (pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer),
                            &timeLeft) == L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("\n C-RP Adv Time : %d \n", timeLeft);
    }
    else
    {
      PIMSM_DEBUG_PRINTF("\n Failed to get CandRpAdvTimer \n");
    }
  }
  else
  {
    PIMSM_DEBUG_PRINTF("\n CandRpAdvTimer is not running\n");
  }
  

  PIMSM_DEBUG_PRINTF("\n -------------------------- \n");
  PIMSM_DEBUG_PRINTF("\n GrpAddr   GrpMask   Origin \n");
  
  for (cnt = 0; cnt < PIMSM_CANDIDATE_RP_GRP_MAX; cnt++)         
  {
    if (inetIsAddressZero(
         &pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr) 
         != L7_TRUE)
    {
      inetCopy(&grpAddr, 
        &pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpAddr);
      grpMask = 
         pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpGrpNode[cnt].grpMask;
  
      if (inetAddrHtop(&grpAddr,strDump) == L7_SUCCESS)
        PIMSM_DEBUG_PRINTF(" %s",strDump);
      PIMSM_DEBUG_PRINTF ("     %x", grpMask);
      /*PIMSM_DEBUG_PRINTF("     %d",temp_addr_node->pimsmMaskLen);
      PIMSM_DEBUG_PRINTF("     %d",temp_addr_node->pimsmOrigin); */
      PIMSM_DEBUG_PRINTF("\n");
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Display entire RP-Set (all Group-to-RP Mapping)
*
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugRPSetShow ( L7_uchar8 family )
{
  pimsmRpGrpNode_t *rp_node = L7_NULL;
  pimsmAddrList_t *rp_grp_node = L7_NULL;
  L7_uchar8 strDump[40];
  L7_uint32 timeLeft = 0;
  pimsmCB_t *   pimsmCB = L7_NULLPTR;

  if ((pimsmCB = (pimsmCB_t*)pimsmMapProtocolCtrlBlockGet(family)) == L7_NULLPTR)
  {
     PIMSM_DEBUG_PRINTF("\n PIMSM Control Block Get Failed for Family - %d", family);
     return;
  }

  PIMSM_DEBUG_PRINTF("\n *********** RP-GRP Mapping ************** \n"); 
  for (rp_node = (pimsmRpGrpNode_t *)SLLFirstGet(&(pimsmCB->pimsmRpInfo->pimsmRpGrpList));
       rp_node != L7_NULL;
       rp_node = (pimsmRpGrpNode_t *)SLLNextGet(&(pimsmCB->pimsmRpInfo->pimsmRpGrpList), (L7_sll_member_t*)rp_node))
  {
    if (inetAddrHtop(&(rp_node->pimsmRpAddr),strDump) == L7_SUCCESS)
      PIMSM_DEBUG_PRINTF("\n RP-address : %s",strDump);
    PIMSM_DEBUG_PRINTF("\n origin Type = %d",rp_node->pimsmOrigin);
    PIMSM_DEBUG_PRINTF("\n rpPriority = %d",rp_node->pimsmRpPriority);
    PIMSM_DEBUG_PRINTF("\n rpHoldTime = %d",rp_node->pimsmRpHoldTime);
    PIMSM_DEBUG_PRINTF("\n hashMaskLen = %d",rp_node->pimsmRpHashMaskLen);
    PIMSM_DEBUG_PRINTF("\n hashValue = %d",rp_node->pimsmHashValue);
  
    PIMSM_DEBUG_PRINTF("\n Grps under this RP : \n");
    for (rp_grp_node = (pimsmAddrList_t *)SLLFirstGet(&(rp_node->pimsmGrpList));
       rp_grp_node != L7_NULL;
       rp_grp_node = (pimsmAddrList_t *)SLLNextGet(&(rp_node->pimsmGrpList), (L7_sll_member_t*)rp_grp_node))
    {
      if (inetAddrHtop(&(rp_grp_node->pimsmIpAddr),strDump) == L7_SUCCESS)
        PIMSM_DEBUG_PRINTF (" %s  , ",strDump);
      PIMSM_DEBUG_PRINTF("  %d  ; ", rp_grp_node->pimsmMaskLen);
      if (rp_grp_node->pimsmRpGrpExpiryTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(pimsmCB->timerCb, rp_grp_node->pimsmRpGrpExpiryTimer, &timeLeft) != L7_SUCCESS)
        {
          timeLeft = 0;
        }
        PIMSM_DEBUG_PRINTF("\n Expire time : %d \n", timeLeft);
      }
    }
    PIMSM_DEBUG_PRINTF("\n ---------------------\n");
  }

}
/******************************************************************************
* @purpose  Display entire GRP-RP Map (Preffered RP is shown)
*
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugGrpRPMapShow ( L7_uchar8 family )
{
  pimsmRpSetNode_t *rp_set_node = L7_NULL;
  pimsmAddrList_t *rp_entry = L7_NULL;
  L7_uchar8 strDump[40];
  pimsmCB_t *   pimsmCb = L7_NULLPTR;

  if ((pimsmCb = (pimsmCB_t*)pimsmMapProtocolCtrlBlockGet(family)) == L7_NULLPTR)
  {
     PIMSM_DEBUG_PRINTF("\n PIMSM Control Block Get Failed for Family - %d", family);
     return;
  }
  
  PIMSM_DEBUG_PRINTF ("\n ********* GRP-RP  MAPPING **********\n");
  for (rp_set_node = (pimsmRpSetNode_t *)radixGetNextEntry(&pimsmCb->pimsmRpInfo->pimsmRpSetTree, L7_NULL);
       rp_set_node != L7_NULLPTR;
       rp_set_node = (pimsmRpSetNode_t *)radixGetNextEntry(&pimsmCb->pimsmRpInfo->pimsmRpSetTree, (void *)rp_set_node))
  { 
    if (inetAddrHtop(&(rp_set_node->pimsmGrpAddr),strDump) == L7_SUCCESS)
      PIMSM_DEBUG_PRINTF("\n GRP-address : %s",strDump);
    PIMSM_DEBUG_PRINTF("\n GRP-mask Len : %d",rp_set_node->pimsmGrpMaskLen);

    PIMSM_DEBUG_PRINTF("\n RP List for this grp :\n");   
    PIMSM_DEBUG_PRINTF("\n RP-address  RP-Origin  RP-priority  RP-holdtime  OriginateFlag  \n");
    for (rp_entry = (pimsmAddrList_t*)SLLFirstGet(&(rp_set_node->pimsmRpList));
         rp_entry != L7_NULLPTR;
         rp_entry = (pimsmAddrList_t*)SLLNextGet(&(rp_set_node->pimsmRpList),(L7_sll_member_t*)rp_entry))
    {
      pimsmRpGrpNode_t  rp_search_node;
      pimsmRpGrpNode_t     *rp_grp_node = L7_NULL; 
      /* get the values from the rp nodes */
      memset(&rp_search_node, 0, sizeof(pimsmRpGrpNode_t));
      inetCopy(&rp_search_node.pimsmRpAddr,&rp_entry->pimsmIpAddr);
      rp_search_node.pimsmOrigin = rp_entry->pimsmOrigin;
 
      if ((rp_grp_node = (pimsmRpGrpNode_t *)SLLFind(&(pimsmCb->pimsmRpInfo->pimsmRpGrpList),(void*)&rp_search_node)) != L7_NULLPTR)
      {
        if(inetAddrHtop(&(rp_grp_node->pimsmRpAddr), strDump) == L7_SUCCESS)
           PIMSM_DEBUG_PRINTF("  %s ",strDump);
        else
           PIMSM_DEBUG_PRINTF("\n failed to get rpAddr \n");
        PIMSM_DEBUG_PRINTF("\t      %d",rp_grp_node->pimsmOrigin);
        PIMSM_DEBUG_PRINTF("\t      %d ",rp_grp_node->pimsmRpPriority);
        PIMSM_DEBUG_PRINTF("\t      %d ",rp_grp_node->pimsmRpHoldTime);
        PIMSM_DEBUG_PRINTF("\t      %s ",rp_entry->pimsmOriginFlag ? "TRUE" : "FALSE");
      }
      PIMSM_DEBUG_PRINTF("\n");
    }
    PIMSM_DEBUG_PRINTF ("\n Preferred Rp  for this grp :\n");
    if (rp_set_node->pimsmPrefRpGrpNode != L7_NULLPTR)
    {
      if(inetAddrHtop(&(rp_set_node->pimsmPrefRpGrpNode->pimsmRpAddr), strDump) == L7_SUCCESS)
        PIMSM_DEBUG_PRINTF("  %s ",strDump);
      PIMSM_DEBUG_PRINTF("\t     %d ",rp_set_node->pimsmPrefRpGrpNode->pimsmOrigin);
      PIMSM_DEBUG_PRINTF("\t     %d ",rp_set_node->pimsmPrefRpGrpNode->pimsmRpPriority);
      PIMSM_DEBUG_PRINTF("\t    %d ",(rp_set_node->pimsmPrefRpGrpNode->pimsmRpHoldTime));
      PIMSM_DEBUG_PRINTF("\n");
    }
    else
      PIMSM_DEBUG_PRINTF("\n no preferred RP \n");
           
    PIMSM_DEBUG_PRINTF ("\n -------------------------\n");
  }

}

/******************************************************************************
* @purpose  Display the cand-BSR and Elected-BSR
*
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugBsrShow (L7_uchar8 family )
{
  pimsmBsrPerScopeZone_t    *entry = L7_NULL;
  L7_uchar8 strDump[40], i;
  L7_uint32 now, timeLeft = 0;
  pimsmCB_t *   pimsmCB = L7_NULLPTR;

  if ((pimsmCB = (pimsmCB_t*)pimsmMapProtocolCtrlBlockGet(family)) == L7_NULLPTR)
  {
     PIMSM_DEBUG_PRINTF("\n PIMSM Control Block Get Failed for Family - %d", family);
     return;
  }
  
  if (pimsmCB->pimsmBsrInfo == L7_NULL)
  {
     PIMSM_DEBUG_PRINTF("\n pimsmBsrInfo is not initialised \n");
     return;
  }

  PIMSM_DEBUG_PRINTF("\n *******  Candidate BSR Info  *********\n");
  if (inetAddrHtop(&(pimsmCB->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRAddress),strDump) == L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n C-BSR address : %s",strDump);
  PIMSM_DEBUG_PRINTF("\n C-BSR rtrIfNum : %d",pimsmCB->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRrtrIfNum);
  PIMSM_DEBUG_PRINTF("\n C-BSR priority : %d",pimsmCB->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRPriority);
  PIMSM_DEBUG_PRINTF("\n C-BSR HashMaskLen : %d",pimsmCB->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRHashMaskLength);
  PIMSM_DEBUG_PRINTF("\n Elected BSR : %s",(pimsmCB->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRElectedBSR)? "TRUE":"FALSE");

  PIMSM_DEBUG_PRINTF("\n *******  BSR List  ********* \n" );

  now = osapiUpTimeRaw ();
  for (i  = 0; i < PIMSM_BSR_SCOPE_NODES_MAX; i++)
  {
    entry = pimsmCB->pimsmBsrInfo->pimsmBsrInfoList + i;
    if (inetIsAddressZero(&entry->pimsmBSRAddr) != L7_TRUE)
    {
      PIMSM_DEBUG_PRINTF("\n BSR Type : %d", entry->pimsmBSRType);
      PIMSM_DEBUG_PRINTF("\n CBSR State : %d",entry->zone.cbsr.pimsmCandBSRState);
      PIMSM_DEBUG_PRINTF("\n NONBSR State : %d",entry->zone.nbsr.pimsmNonCandBSRState);
      /* ipaddr of the scoped/nonscoped zone */
      if (inetAddrHtop(&(entry->pimsmBSRGroupRange),strDump) == L7_SUCCESS)
        PIMSM_DEBUG_PRINTF("\n group_range_addr : %s", strDump);  
      PIMSM_DEBUG_PRINTF("\n grp Mask len : %d", entry->pimsmBSRGroupMask);
      if (inetAddrHtop(&(entry->pimsmBSRAddr),strDump) == L7_SUCCESS)
         PIMSM_DEBUG_PRINTF("\n bsr_addr : %s", strDump);
      PIMSM_DEBUG_PRINTF("\n bsr_priority : %d", entry->pimsmBSRPriority);
      PIMSM_DEBUG_PRINTF("\n bsr_hash_masklen : %d", entry->pimsmBSRHashMasklen);
      if (entry->pimsmBSRTimer != L7_NULLPTR)
      {
        if (appTimerTimeLeftGet(pimsmCB->timerCb, entry->pimsmBSRTimer, &timeLeft) != L7_SUCCESS)
        {
          timeLeft = 0;
        }
        PIMSM_DEBUG_PRINTF("\n BSR Expire time : %d ", timeLeft);
      }
      PIMSM_DEBUG_PRINTF("\n BSR FragTag : %d ", entry->pimsmBSRFragInfo.pimsmFragTag);
      
      PIMSM_DEBUG_PRINTF("\n  Elected BSR state : %d",entry->zone.cbsr.pimsmCandBSRState);
      PIMSM_DEBUG_PRINTF("\n  Elected BSR priority : %d",entry->zone.cbsr.pimsmElectedBSRPriority);
      PIMSM_DEBUG_PRINTF("\n  Elected BSR hashMaskLen : %d",entry->zone.cbsr.pimsmElectedBSRHashMaskLen);
      if (inetAddrHtop(&(entry->zone.cbsr.pimsmElectedBSRAddress), strDump) == L7_SUCCESS)
        PIMSM_DEBUG_PRINTF("\n  Elected BSR address : %s",strDump);

      PIMSM_DEBUG_PRINTF("\n -----------------------------\n");
    }
    
  }
}
/******************************************************************************
* @purpose  Set pimsm trace level
*
* @param        level
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugEnable ( pimsmTraceFlags_t level )
{
  if(level >= PIMSM_TRACE_DISABLE && level < PIMSM_TRACE_MAX)
  {
    pimsmCurrTraceLevel = level;
  } 
  else
  {
    pimsmCurrTraceLevel = PIMSM_TRACE_NORMAL;
  }
}
/******************************************************************************
* @purpose  Set pimsm trace level
*
* @param        level
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugDisable()
{
   pimsmCurrTraceLevel = PIMSM_TRACE_DISABLE;
}
/******************************************************************************
* @purpose  Set pimsm trace level
*
* @param        level
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugPathBitsSet ( pimsmDebugPathFlags_t bits )
{
  if(bits <= PIMSM_DEBUG_BIT_ALL)
  {
    pimsmCurrDebugPathBits = bits;
  }
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for all Trace Levels
*
* @param    None.
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimsmDebugAllSet (void)
{
  pimsmCurrDebugPathBits =PIMSM_DEBUG_BIT_ALL;
  PIMSM_DEBUG_PRINTF ("PIM-SM Debug Tracing is Enabled for All Trace Levels.\n");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for all Trace Levels
*
* @param    None.
*
* @returns  L7_SUCCESS if Debug trace was successfully disabled.
* @returns  L7_FAILURE if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimsmDebugAllReset (void)
{
  pimsmCurrDebugPathBits =PIMSM_DEBUG_BIT_NONE;
  PIMSM_DEBUG_PRINTF ("PIM-SM Debug Tracing is Disabled for All Trace Levels.\n");
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Set pimsm flag
*
* @param        level
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmDebugFlagSet ( pimsmDebugFlags_t flag )
{
  if(flag  <= PIMSM_DEBUG_ALL)
  {
    switch(flag)
    {
      case PIMSM_DEBUG_NONE:
          pimsmCurrDebugPathBits = PIMSM_DEBUG_BIT_NONE;       
          break;    
      case PIMSM_DEBUG_MACROS:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_MACROS;       
          break;
      case PIMSM_DEBUG_REG_RX_TX_FSM:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_REG_RX_TX_FSM;               
          break;          
      case PIMSM_DEBUG_HELLO_RX_TX:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_HELLO_RX_TX;               
          break;          
      case PIMSM_DEBUG_JOIN_RX_TX:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_JOIN_RX_TX;               
          break;          
      case PIMSM_DEBUG_S_G_RPT:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_S_G_RPT;               
          break;          
      case PIMSM_DEBUG_S_G:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_S_G;               
          break;          
      case PIMSM_DEBUG_STAR_G:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_STAR_G;               
          break;          
      case PIMSM_DEBUG_STAR_STAR_RP:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_STAR_STAR_RP;               
          break;          
      case PIMSM_DEBUG_ASSERT_FSM:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_ASSERT_FSM;               
          break;          
      case PIMSM_DEBUG_RP:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_RP;               
          break;          
      case PIMSM_DEBUG_BSR:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_BSR;               
          break;          
      case PIMSM_DEBUG_BSR_FSM:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_BSR_FSM;               
          break;          
      case PIMSM_DEBUG_MFC_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_MFC_INTERACTION;               
          break;          
      case PIMSM_DEBUG_MGMD_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_MGMD_INTERACTION;               
          break;          
      case PIMSM_DEBUG_RTO_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_RTO_INTERACTION;               
          break;          
      case PIMSM_DEBUG_UI_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_UI_INTERACTION;               
          break;          
      case PIMSM_DEBUG_INTF_NEIGHBOR:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_INTF_NEIGHBOR;               
          break;          
      case PIMSM_DEBUG_MISC:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits |PIMSM_DEBUG_BIT_MISC;               
          break;       
      case PIMSM_DEBUG_ALL:        
          pimsmCurrDebugPathBits = PIMSM_DEBUG_BIT_ALL;               
          break;           
      default:    
      PIMSM_DEBUG_PRINTF("\n Not a valid flag");
      break;
    }
  }
  return L7_SUCCESS;  
}


/******************************************************************************
* @purpose  Set pimsm flag
*
* @param        level
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
L7_BOOL pimsmDebugFlagCheck ( pimsmDebugFlags_t flag )
{
 L7_int32 flagBitSet =0;
 
  if(flag  >= PIMSM_DEBUG_MAX)
  {
    return L7_FALSE;
  }
  switch(flag)
  {
    case PIMSM_DEBUG_NONE:
        flagBitSet = PIMSM_DEBUG_BIT_NONE;       
        break;    
    case PIMSM_DEBUG_MACROS:
        flagBitSet = PIMSM_DEBUG_BIT_MACROS;       
        break;
    case PIMSM_DEBUG_REG_RX_TX_FSM:
        flagBitSet = PIMSM_DEBUG_BIT_REG_RX_TX_FSM;               
        break;          
    case PIMSM_DEBUG_HELLO_RX_TX:
        flagBitSet = PIMSM_DEBUG_BIT_HELLO_RX_TX;               
        break;          
    case PIMSM_DEBUG_JOIN_RX_TX:        
        flagBitSet = PIMSM_DEBUG_BIT_JOIN_RX_TX;               
        break;          
    case PIMSM_DEBUG_S_G_RPT:
        flagBitSet = PIMSM_DEBUG_BIT_S_G_RPT;               
        break;          
    case PIMSM_DEBUG_S_G:        
        flagBitSet = PIMSM_DEBUG_BIT_S_G;               
        break;          
    case PIMSM_DEBUG_STAR_G:
        flagBitSet = PIMSM_DEBUG_BIT_STAR_G;               
        break;          
    case PIMSM_DEBUG_STAR_STAR_RP:
        flagBitSet = PIMSM_DEBUG_BIT_STAR_STAR_RP;               
        break;          
    case PIMSM_DEBUG_ASSERT_FSM:
        flagBitSet = PIMSM_DEBUG_BIT_ASSERT_FSM;               
        break;          
    case PIMSM_DEBUG_RP:        
        flagBitSet = PIMSM_DEBUG_BIT_RP;               
        break;          
    case PIMSM_DEBUG_BSR:
        flagBitSet = PIMSM_DEBUG_BIT_BSR;               
        break;          
    case PIMSM_DEBUG_BSR_FSM:
        flagBitSet = PIMSM_DEBUG_BIT_BSR_FSM;               
        break;          
    case PIMSM_DEBUG_MFC_INTERACTION:
        flagBitSet = PIMSM_DEBUG_BIT_MFC_INTERACTION;               
        break;          
    case PIMSM_DEBUG_MGMD_INTERACTION:
        flagBitSet = PIMSM_DEBUG_BIT_MGMD_INTERACTION;               
        break;          
    case PIMSM_DEBUG_RTO_INTERACTION:
        flagBitSet = PIMSM_DEBUG_BIT_RTO_INTERACTION;               
        break;          
    case PIMSM_DEBUG_UI_INTERACTION:
        flagBitSet = PIMSM_DEBUG_BIT_UI_INTERACTION;               
        break;          
    case PIMSM_DEBUG_INTF_NEIGHBOR:        
        flagBitSet = PIMSM_DEBUG_BIT_INTF_NEIGHBOR;               
        break;          
    case PIMSM_DEBUG_MISC:        
        flagBitSet = PIMSM_DEBUG_BIT_MISC;               
        break;       
    case PIMSM_DEBUG_ALL:        
        flagBitSet = PIMSM_DEBUG_BIT_ALL;               
        break;           
    default:    
        PIMSM_DEBUG_PRINTF("\n Not a valid flag");
        return L7_FALSE;
  }
  if ( (pimsmCurrDebugPathBits & flagBitSet) != L7_NULL )
  {
    return L7_TRUE;
  }
  return L7_FALSE;  
}
/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific Trace Level
*
* @param    traceLevel @b{ (input) } Trace Level to disable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
* @returns  L7_FAILURE   if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimsmDebugFlagReset (pimsmDebugFlags_t flag)
{
  if(flag <= PIMSM_DEBUG_ALL)
  {
    switch(flag)
    {
      case PIMSM_DEBUG_MACROS:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits & (~PIMSM_DEBUG_BIT_MACROS);       
          break;
      case PIMSM_DEBUG_REG_RX_TX_FSM:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_REG_RX_TX_FSM);               
          break;          
      case PIMSM_DEBUG_HELLO_RX_TX:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_HELLO_RX_TX);               
          break;          
      case PIMSM_DEBUG_JOIN_RX_TX:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_JOIN_RX_TX);               
          break;          
      case PIMSM_DEBUG_S_G_RPT:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_S_G_RPT);               
          break;          
      case PIMSM_DEBUG_S_G:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_S_G);               
          break;          
      case PIMSM_DEBUG_STAR_G:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_STAR_G);               
          break;          
      case PIMSM_DEBUG_STAR_STAR_RP:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_STAR_STAR_RP);               
          break;          
      case PIMSM_DEBUG_ASSERT_FSM:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_ASSERT_FSM);               
          break;          
      case PIMSM_DEBUG_RP:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_RP);               
          break;          
      case PIMSM_DEBUG_BSR:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_BSR);               
          break;          
      case PIMSM_DEBUG_BSR_FSM:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_BSR_FSM);               
          break;          
      case PIMSM_DEBUG_MFC_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_MFC_INTERACTION);               
          break;          
      case PIMSM_DEBUG_MGMD_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_MGMD_INTERACTION);               
          break;          
      case PIMSM_DEBUG_RTO_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_RTO_INTERACTION);               
          break;          
      case PIMSM_DEBUG_UI_INTERACTION:
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_UI_INTERACTION);               
          break;          
      case PIMSM_DEBUG_INTF_NEIGHBOR:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_INTF_NEIGHBOR);               
          break;          
      case PIMSM_DEBUG_MISC:        
          pimsmCurrDebugPathBits = pimsmCurrDebugPathBits &(~PIMSM_DEBUG_BIT_MISC);               
          break;          
      default:    
      PIMSM_DEBUG_PRINTF("\n Not a valid flag");
      break;
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Show current pimsm trace level
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugFlagShow ()
{ 
  L7_uint32 i;
  
  PIMSM_DEBUG_PRINTF( "PIMSM Current Trace Level = %d\n",
                      pimsmCurrTraceLevel );
  PIMSM_DEBUG_PRINTF( "PIMSM Current Debug PathBits = 0x%x\n",
                      pimsmCurrDebugPathBits );    
  PIMSM_DEBUG_PRINTF( "\nPIMSM Current Flag Status :\n");
                     
  for (i = PIMSM_DEBUG_NONE;
       i < PIMSM_DEBUG_MAX;
       i++)
  {
    if(pimsmDebugFlagCheck (i) == L7_TRUE)  
    {
      PIMSM_DEBUG_PRINTF ("%50s : Enabled.\n", pimsmDebugFlagName[i]);
    }
    else
    {
      PIMSM_DEBUG_PRINTF ("%50s : Disabled.\n", pimsmDebugFlagName[i]);
    }

  }
}
/******************************************************************************
* @purpose  show control block
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugCtrlBlockShow ( pimsmCB_t * pimsmCb )
{

  PIMSM_DEBUG_PRINTF( "StarStarRPList: %p \n", pimsmCb->pimsmStarStarRpList );
  PIMSM_DEBUG_PRINTF( "StarStarRPList Sema: %p \n",
                      pimsmCb->pimsmStarStarRpListLock );
  PIMSM_DEBUG_PRINTF( "StarGTree: %p \n", &pimsmCb->pimsmStarGTree );
  PIMSM_DEBUG_PRINTF( "StarGDataHeap: %p \n", pimsmCb->pimsmStarGDataHeap );
  PIMSM_DEBUG_PRINTF( "StarGTreeHeap: %p \n", pimsmCb->pimsmStarGTreeHeap );
  PIMSM_DEBUG_PRINTF( "SGTree: %p \n", &pimsmCb->pimsmSGTree );
  PIMSM_DEBUG_PRINTF( "SGDataHeap: %p \n", pimsmCb->pimsmSGDataHeap );
  PIMSM_DEBUG_PRINTF( "SGTreeHeap: %p \n", pimsmCb->pimsmSGTreeHeap );

  PIMSM_DEBUG_PRINTF( "SGRptTree: %p \n", &pimsmCb->pimsmSGRptTree );
  PIMSM_DEBUG_PRINTF( "SGRptDataHeap: %p \n", pimsmCb->pimsmSGRptDataHeap );
  PIMSM_DEBUG_PRINTF( "SGRptTreeHeap: %p \n", pimsmCb->pimsmSGRptTreeHeap );
  PIMSM_DEBUG_PRINTF("allPimRoutersGrpAddr :");
  PIMSM_PRINT_ADDR(&pimsmCb->allPimRoutersGrpAddr);
  PIMSM_DEBUG_PRINTF( "sockFd: %d \n", pimsmCb->sockFd );
  PIMSM_DEBUG_PRINTF( "pimsmDataRateBytes: %d \n", pimsmCb->pimsmDataRateBytes );  
  PIMSM_DEBUG_PRINTF( "pimsmRegRateBytes: %d \n", pimsmCb->pimsmRegRateBytes );
  PIMSM_DEBUG_PRINTF( "pimsmRateCheckInterval: %d \n", pimsmCb->pimsmRateCheckInterval );
  PIMSM_DEBUG_PRINTF( "family: %d \n", pimsmCb->family );  
  PIMSM_DEBUG_PRINTF( "isPimsmEnabled: %d \n", pimsmCb->isPimsmEnabled );  
  PIMSM_DEBUG_PRINTF( "pimsmSPTTimerHandle: %d \n", 
                      pimsmCb->pimsmSPTTimerHandle );  
  PIMSM_DEBUG_PRINTF( "pimsmSPTTimerParam Addr: %p \n", 
                      &pimsmCb->pimsmSPTTimerParam );
  PIMSM_DEBUG_PRINTF( "pimsmJPBundleTimerHandle: %d \n", 
                      pimsmCb->pimsmJPBundleTimerHandle );  
  PIMSM_DEBUG_PRINTF( "pimsmJPBundleTimerParam Addr: %p \n", 
                      &pimsmCb->pimsmJPBundleTimerParam );
  PIMSM_DEBUG_PRINTF( "timerCb: %p \n", 
                      (pimsmCb->timerCb));  
  PIMSM_DEBUG_PRINTF( "handleList: %p \n", 
                      *(pimsmCb->handleList) );  
  PIMSM_DEBUG_PRINTF( "Neighbor Sema: 0x%x\n", pimsmCb->pimsmNbrLock);
  PIMSM_DEBUG_PRINTF( "pimsmStarStarRpNextHopUpdateCnt: %d\n",
                pimsmCb->pimsmStarStarRpNextHopUpdateCnt);
  PIMSM_DEBUG_PRINTF( "pimsmSGNextHopUpdateCnt: %d\n",
                pimsmCb->pimsmSGNextHopUpdateCnt);
  PIMSM_DEBUG_PRINTF( "pimsmStarGNextHopUpdateCnt: %d\n",
                pimsmCb->pimsmStarGNextHopUpdateCnt);
  PIMSM_DEBUG_PRINTF( "pimsmSGRptNextHopUpdateCnt: %d\n",
                pimsmCb->pimsmSGRptNextHopUpdateCnt);
  PIMSM_DEBUG_PRINTF( "maxNumOfTimers: %d\n",
                pimsmCb->maxNumOfTimers);

  PIMSM_DEBUG_PRINTF( "mcastHeapId: 0x%x \n", mcastMapHeapIdGet(pimsmCb->family));
}

/******************************************************************************
* @purpose  Help command: show all debug APIs
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugHelp ()
{
   L7_uint32 i;
   
   PIMSM_DEBUG_PRINTF("\npimsmDebugEnable (level ) ");
   PIMSM_DEBUG_PRINTF("\n                         level = 0..7 ");   
   PIMSM_DEBUG_PRINTF("\npimsmDebugDisable () ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugFlagSet (flag)");    
   PIMSM_DEBUG_PRINTF("\npimsmDebugFlagReset (flag)");    
   PIMSM_DEBUG_PRINTF("\npimsmDebugAllSet ()");    
   PIMSM_DEBUG_PRINTF("\npimsmDebugAllReset ()");    
   PIMSM_DEBUG_PRINTF("\nFollowing flags are to be used:");    
   for (i = PIMSM_DEBUG_NONE;
        i < PIMSM_DEBUG_MAX;
        i++)
   {
     PIMSM_DEBUG_PRINTF ("\n\t\t%d\t%s",i, pimsmDebugFlagName[i]);
   }   
   PIMSM_DEBUG_PRINTF("\npimsmDebugFlagShow()");    
   PIMSM_DEBUG_PRINTF("\npimsmDebugCBShow ( family) ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugTimersShow ( family) ");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugMRTTableShow (  family,count ) ");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugSGShow (  family,count  ) ");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugSGRptShow (  family,count  )");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugStarGShow (  family,count  )");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugStarStarRpShow (  family,count  ) ");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugSGIShow (  family,count  ) ");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugSGRptIShow (  family,count  )");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugStarGIShow (  family,count  )");
   PIMSM_DEBUG_PRINTF("\n pimsmDebugStarStarRpIShow (  family,count  ) ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugIntfShow (  family,  rtrIfNum ) ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugIntfStatsShow ( family,  rtrIfNum ) "
                      "\n          [Use rtrIfNum=0 to display Stats for all PIM-SM Enabled interfaces]");
   PIMSM_DEBUG_PRINTF("\npimsmDebugIntfAllShow (  family ) ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugIntfNeighborListShow (family, rtrIfNum ) ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugRpCandidateShow( family)");
   PIMSM_DEBUG_PRINTF("\npimsmDebugRPSetShow (  family )");
   PIMSM_DEBUG_PRINTF("\npimsmDebugGrpRPMapShow (  family ) ");
   PIMSM_DEBUG_PRINTF("\npimsmDebugBsrShow ( family ) ");
   PIMSM_DEBUG_PRINTF("\n                      family = 1 or 2 ");   
   PIMSM_DEBUG_PRINTF("\n                  rtrIfNum = 1 or 128 ");   
}
/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugDummy ()
{
  pimsmDebugMsgCnt = 0;
  /*pimsmDebugUTDummy();*/
}

#if 0
/******************************************************************************
* @purpose  Display per-interface stats
*
* @param        rtrIfNum         @b{(input)}
* @param        family               @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfStatsShow (L7_uchar8 family, L7_uint32 rtrIfNum )
{
  pimsmInterfaceEntry_t *   intfEntry = L7_NULLPTR;
  pimsmCB_t *               pimsmCb;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  if(L7_NULLPTR == pimsmCb)
  {
    return;
  }

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry )!=L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "interface entry is NULL\r\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF(
                    "---------------------------PerIntf Stats----------------------------\n" );  
  PIMSM_DEBUG_PRINTF( "If Index: %d\n", rtrIfNum );
  PIMSM_DEBUG_PRINTF( "Interface Address : " );
  PIMSM_PRINT_ADDR( &intfEntry->pimsmInterfaceAddr );
  PIMSM_DEBUG_PRINTF( "Hello Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmHelloPktRx );
  PIMSM_DEBUG_PRINTF( "Hello Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmHelloPktTx );
  PIMSM_DEBUG_PRINTF( "Register Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmRegisterPktRx );
  PIMSM_DEBUG_PRINTF( "Register Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmRegisterPktTx );
  PIMSM_DEBUG_PRINTF( "Register-Stop Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmRegisterStopPktRx );
  PIMSM_DEBUG_PRINTF( "Register-Stop Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmRegisterStopPktTx );
  PIMSM_DEBUG_PRINTF( "Join/Prune Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmJoinPrunePktRx );
  PIMSM_DEBUG_PRINTF( "Join/Prune Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmJoinPrunePktTx );
  PIMSM_DEBUG_PRINTF( "Bootstrap Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmBsrPktRx );
  PIMSM_DEBUG_PRINTF( "Bootstrap Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmBsrPktTx );
  PIMSM_DEBUG_PRINTF( "Cand-Rp Advt Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmCandRpPktRx );
  PIMSM_DEBUG_PRINTF( "Cand-Rp Advt Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmCandRpPktTx );
  PIMSM_DEBUG_PRINTF( "Assert Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmAssertPktRx );
  PIMSM_DEBUG_PRINTF( "Assert Packet Tx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmAssertPktTx );
  PIMSM_DEBUG_PRINTF( "No Cache Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmNoCacheRx );
  PIMSM_DEBUG_PRINTF( "Wrong Interface Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmWrongiifRx );

  PIMSM_DEBUG_PRINTF( "Invalid Packet Rx Count: %d\n",
                      intfEntry->pimsmPerIntfStats.pimsmInvalidPktRx );
}
#else
/*********************************************************************
*
* @purpose  To display the PIM-SM Statistics in detail
*
* @param    pimsmCb    @b{ (input) } Pointer to the PIM-SM Control Block
* @param    rtrIfNum   @b{ (input) } Index of the Router Interface
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimsmIntfStatsDisplay (pimsmCB_t *pimsmCb,
                       L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t *intfEntry = L7_NULLPTR;
  L7_uchar8 emptyStr[] = "     ";
  L7_uint32 tempInt = 0;

  if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &intfEntry ) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_INTF_NEIGHBOR, PIMSM_TRACE_DEBUG,
                 "Interface-%d Entry is NULL", rtrIfNum);
    return;
  }

  PIMSM_DEBUG_PRINTF ("%4d ", rtrIfNum);
  PIMSM_DEBUG_PRINTF ("Rx   ");
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmHelloPktRx);
  PIMSM_DEBUG_PRINTF ("%8d ", intfEntry->pimsmPerIntfStats.pimsmRegisterPktRx);
  PIMSM_DEBUG_PRINTF ("%8d ", intfEntry->pimsmPerIntfStats.pimsmRegisterStopPktRx);
  PIMSM_DEBUG_PRINTF ("%8d ", intfEntry->pimsmPerIntfStats.pimsmJoinPrunePktRx);
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmBsrPktRx);
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmAssertPktRx);
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmCandRpPktRx);
  PIMSM_DEBUG_PRINTF ("%5d ", intfEntry->pimsmPerIntfStats.pimsmNoCacheRx);
  PIMSM_DEBUG_PRINTF ("%5d ", intfEntry->pimsmPerIntfStats.pimsmWrongiifRx);
  PIMSM_DEBUG_PRINTF ("\n");

  PIMSM_DEBUG_PRINTF ("%s", emptyStr);
  PIMSM_DEBUG_PRINTF ("Tx   ");
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmHelloPktTx);
  PIMSM_DEBUG_PRINTF ("%8d ", intfEntry->pimsmPerIntfStats.pimsmRegisterPktTx);
  PIMSM_DEBUG_PRINTF ("%8d ", intfEntry->pimsmPerIntfStats.pimsmRegisterStopPktTx);
  PIMSM_DEBUG_PRINTF ("%8d ", intfEntry->pimsmPerIntfStats.pimsmJoinPrunePktTx);
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmBsrPktTx);
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmAssertPktTx);
  PIMSM_DEBUG_PRINTF ("%7d ", intfEntry->pimsmPerIntfStats.pimsmCandRpPktTx);
  PIMSM_DEBUG_PRINTF ("%5d ", tempInt);
  PIMSM_DEBUG_PRINTF ("%5d ", tempInt);
  PIMSM_DEBUG_PRINTF ("\n");

  PIMSM_DEBUG_PRINTF ("\n");
  PIMSM_DEBUG_PRINTF ("%s", emptyStr);
  PIMSM_DEBUG_PRINTF ("Invalid Packets Received - %d", intfEntry->pimsmPerIntfStats.pimsmInvalidPktRx);
  PIMSM_DEBUG_PRINTF ("\n");

  PIMSM_DEBUG_PRINTF ("--------------------------------------------------------------------------------\n");

  return;
 }

/******************************************************************************
* @purpose  Display per-interface stats
*
* @param        rtrIfNum         @b{(input)}
* @param        family           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void
pimsmDebugIntfStatsShow (L7_uchar8 addrFamily,
                         L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_uint32 ifIndex = 0;
  pimsmInterfaceEntry_t *intfEntry = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMSM_DEBUG_PRINTF ("Bad Address Family - %d Specified.\n", addrFamily);
    return;
  }
  if ((pimsmCb = pimsmMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF ("Error in getting the Control Block for family - %d.\n", addrFamily);
    return;
  }
  if (rtrIfNum >= MCAST_MAX_INTERFACES)
  {
    PIMSM_DEBUG_PRINTF ("Bad rtrIfNum - %d Specified.\n", rtrIfNum);
    return;
  }
  if (rtrIfNum != 0)
  {
    if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &intfEntry ) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF ("PIM-SM is not Enabled on Intf - %d.\n", rtrIfNum);
      return;
    }
  }

  PIMSM_DEBUG_PRINTF ("\nRx      - Packet Received in Protocol.\n");
  PIMSM_DEBUG_PRINTF ("Tx      - Packet Sent from Protocol.\n");
  PIMSM_DEBUG_PRINTF ("Invalid - Packet Received in Protocol, Basic Validations Failed.\n");

  PIMSM_DEBUG_PRINTF ("\n================================================================================\n");
  PIMSM_DEBUG_PRINTF ("Intf Stat   Hello Register Reg-Stop Join/Pru     BSR  Assert     CRP NoCac WroIF\n");
  PIMSM_DEBUG_PRINTF ("================================================================================\n");

  if (rtrIfNum == 0)
  {
    for (ifIndex = 1; ifIndex < MCAST_MAX_INTERFACES; ifIndex++)
    {
      pimsmIntfStatsDisplay (pimsmCb, ifIndex);
    }
  }
  else
  {
    pimsmIntfStatsDisplay (pimsmCb, rtrIfNum);
  }

  return;
}
#endif

/******************************************************************************
* @purpose  clear the per-interface stats
*
* @param        rtrIfNum         @b{(input)}
* @param        family               @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfStatsClear (L7_uchar8 family, L7_uint32 rtrIfNum )
{
  pimsmInterfaceEntry_t *   intfEntry = L7_NULLPTR;
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  L7_uint32                 tempRtrIfNum = 0;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Invalid Address Family - %d\n", family);
    return;
  }

  if (rtrIfNum == 0) /* Clear for All Interfaces */
  {
    for (tempRtrIfNum = 0; tempRtrIfNum < MCAST_MAX_INTERFACES; tempRtrIfNum++)
    {
      if (pimsmIntfEntryGet (pimsmCb, tempRtrIfNum, &intfEntry) != L7_SUCCESS)
      {
        continue;
      }
      if (intfEntry == L7_NULLPTR)
      {
        continue;
      }

      memset (&intfEntry->pimsmPerIntfStats, 0 , sizeof (pimsmPerIntfStats_t));
    }
  }
  else /* Clear for the specified Interface */
  {
    if (pimsmIntfEntryGet (pimsmCb, rtrIfNum, &intfEntry) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF ("Intf Entry Get Failed for rtrIfNum-%d.\n", rtrIfNum);
      return;
    }
    if (intfEntry == L7_NULLPTR)
    {
      PIMSM_DEBUG_PRINTF ("Intf Entry is NULL for rtrIfNum-%d.\n", rtrIfNum);
      return;
    }

    memset (&intfEntry->pimsmPerIntfStats, 0 , sizeof (pimsmPerIntfStats_t));
  }

  return;
}

/******************************************************************************
* @purpose  Display a given interface information
*
* @param
* @param        rtrIfNum          @b{(input)}
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfShow ( L7_uchar8 family, L7_uint32 rtrIfNum )
{
  pimsmInterfaceEntry_t *   intfEntry = L7_NULLPTR;
  L7_uint32 timeLeft = 0;
  pimsmCB_t *               pimsmCb;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "failed to get control block \n" );
    return;
  }

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry )!=L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "interface entry is NULL \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Interface Index : %d\n",
                      intfEntry->pimsmInterfaceIfIndex );
  PIMSM_DEBUG_PRINTF("Hello Interval Value : %d\n",
    intfEntry->pimsmInterfaceHelloInterval);
  PIMSM_DEBUG_PRINTF("Join/Prune Interval Value : %d\n",
    intfEntry->pimsmInterfaceJoinPruneInterval);
  PIMSM_DEBUG_PRINTF("Hello Holdtime Value : %d\n",
    intfEntry->pimsmInterfaceHelloHoldtime);
  PIMSM_DEBUG_PRINTF("Join/Prune Holdtime Value : %d\n",
    intfEntry->pimsmInterfaceJoinPruneHoldtime);
  PIMSM_DEBUG_PRINTF( "Interface Address : " );
  PIMSM_PRINT_ADDR( &intfEntry->pimsmInterfaceAddr );
  PIMSM_DEBUG_PRINTF( "DR Address : " );
  PIMSM_PRINT_ADDR( &intfEntry->pimsmInterfaceDR );
  PIMSM_DEBUG_PRINTF( "I_am_DR : %d\n", intfEntry->pimsmIamDR );
  PIMSM_DEBUG_PRINTF( "DR Priority: %d\n", 
                      intfEntry->pimsmInterfaceDRPriority);
  PIMSM_DEBUG_PRINTF( "Generation Id : %d\n", 
                      intfEntry->pimsmInterfaceGenerationIDValue);
  appTimerTimeLeftGet( pimsmCb->timerCb,
                       intfEntry->pimsmHelloTimer,
                       &timeLeft );   
  PIMSM_DEBUG_PRINTF( "Hello Timer Value: %d\n", timeLeft );
  PIMSM_DEBUG_PRINTF( "Neighbor Count: %d\n", intfEntry->pimsmNbrCount );
  PIMSM_DEBUG_PRINTF( "\nNeighbor List : \n" );
  pimsmDebugNbrListShow( pimsmCb, intfEntry );
}
/******************************************************************************
* @purpose  Display all interfaces information
*
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfAllShow ( L7_uchar8 family )
{
  L7_uint32     rtrIfNum;
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    pimsmDebugIntfShow( family, rtrIfNum );
  }
}
/******************************************************************************
* @purpose  Display all interfaces information
*
* @param        family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfStatsAllShow ( L7_uchar8 family )
{
  L7_uint32     rtrIfNum;
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    pimsmDebugIntfStatsShow( family, rtrIfNum );
  }
}
/******************************************************************************
* @purpose  Display all global configuration
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugCBShow (L7_uchar8 family)
{
  pimsmCB_t *pimsmCb;
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);
  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  pimsmDebugCtrlBlockShow(pimsmCb);
}
/******************************************************************************
* @purpose  
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugTimersShow (L7_uchar8 family)
{
  pimsmCB_t *pimsmCb;
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);
  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  appTimerDebugShow(pimsmCb->timerCb);
}
/******************************************************************************
* @purpose  Display mem usage
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugMemUsageShow ()
{
 L7_uint32 a,t1,t2;
 L7_uint64 sum = 0, sum1 = 0 ;  
 pimsmCB_t *pimsmCb;

  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(L7_AF_INET);
  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  a = MCAST_MAX_INTERFACES  * sizeof(pimsmInterfaceEntry_t);
  PIMSM_DEBUG_PRINTF("Max number of Interfaces = %d\n", MCAST_MAX_INTERFACES); 
  PIMSM_DEBUG_PRINTF("Interface Table size = %d\n\n", a); 
  
  a =  sizeof(pimsmCB_t) + 
   sizeof(pimsmRpBlock_t) + sizeof(pimsmBSRBlock_t);
  PIMSM_DEBUG_PRINTF("PIMSM Control block size = %d\n\n", a);

  PIMSM_DEBUG_PRINTF("Optimal number of outgoing interfaces per MRT entry = %d\n", 
            PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES); 

  sum += a;
  t1 = sizeof(pimsmSGEntry_t) + 
   PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * sizeof(pimsmSGIEntry_t);
  a = PIMSM_S_G_IPV4_TBL_SIZE_TOTAL * t1;
  PIMSM_DEBUG_PRINTF("(S,G) Table Size = %d\n", PIMSM_S_G_IPV4_TBL_SIZE_TOTAL); 
  PIMSM_DEBUG_PRINTF("(S,G) Entry Size = %d\n", sizeof(pimsmSGEntry_t)); 
  PIMSM_DEBUG_PRINTF("(S,G,I) Entry Size = %d\n", sizeof(pimsmSGIEntry_t)); 
  PIMSM_DEBUG_PRINTF("(S,G) Node Size = %d\n", t1); 
  PIMSM_DEBUG_PRINTF("(S,G) Total Tree size = %d\n\n", a); 
  sum += a;
  
  t1 = sizeof(pimsmSGRptEntry_t) + 
   PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * sizeof(pimsmSGRptIEntry_t);
  a = PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL * t1;
  PIMSM_DEBUG_PRINTF("(S,G,Rpt) Table Size = %d\n", PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL); 
  PIMSM_DEBUG_PRINTF("(S,G,Rpt) Entry Size = %d\n", sizeof(pimsmSGRptEntry_t)); 
  PIMSM_DEBUG_PRINTF("(S,G,Rpt,I) Entry Size = %d\n", sizeof(pimsmSGRptIEntry_t)); 
  PIMSM_DEBUG_PRINTF("(S,G,Rpt) Node Size = %d\n", t1); 
  PIMSM_DEBUG_PRINTF("(S,G,Rpt) Total Tree size = %d\n\n", a); 
  sum += a;
  
  t1 = sizeof(pimsmStarGEntry_t) + 
   PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * sizeof(pimsmStarGIEntry_t);
  a = PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL * t1;
  PIMSM_DEBUG_PRINTF("(*,G) Table Size = %d\n", PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL); 
  PIMSM_DEBUG_PRINTF("(*,G) Entry Size = %d\n", sizeof(pimsmStarGEntry_t)); 
  PIMSM_DEBUG_PRINTF("(*,G,I) Entry Size = %d\n", sizeof(pimsmStarGIEntry_t)); 
  PIMSM_DEBUG_PRINTF("(*,G) Node Size = %d\n", t1); 
  PIMSM_DEBUG_PRINTF("(*,G) Total Tree size = %d\n\n", a); 
  sum += a;


  PIMSM_DEBUG_PRINTF("Optimal number of Kernel-Cache entries per (*,G) entry = %d\n", 
            PIMSM_MAX_KERNEL_CACHE_ENTRIES_PER_STAR_G_ENTRY);
  a = PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL * PIMSM_MAX_KERNEL_CACHE_ENTRIES_PER_STAR_G_ENTRY *
         sizeof(pimsmCache_t);
  PIMSM_DEBUG_PRINTF("(*,G) Kernel Cache Entires size = %d\n\n", a); 
  sum += a;
  
  t1 = sizeof(pimsmStarStarRpEntry_t) + 
   PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * sizeof(pimsmStarStarRpIEntry_t);
  a = PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL * t1;
  PIMSM_DEBUG_PRINTF("(*,*,RP) Table Size = %d\n", PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL); 
  PIMSM_DEBUG_PRINTF("(*,*,RP) Entry Size = %d\n", sizeof(pimsmStarStarRpEntry_t)); 
  PIMSM_DEBUG_PRINTF("(*,*,RP,I) Entry Size = %d\n", sizeof(pimsmStarStarRpIEntry_t)); 
  PIMSM_DEBUG_PRINTF("(*,*,RP) Node Size = %d\n", t1); 
  PIMSM_DEBUG_PRINTF("(*,*,RP) Total Tree size = %d\n\n", a); 
  sum += a;
  
  
  PIMSM_DEBUG_PRINTF("Optimal number of neighbors per interface = %d\n", 
            10);
  a = MCAST_MAX_INTERFACES * 10 * sizeof(pimsmNeighborEntry_t);
  PIMSM_DEBUG_PRINTF("Neighbor Table Total size = %d\n\n", a); 
  sum += a;

  a = PIMSM_MAX_PROTOCOL_PACKETS * PIMSM_PKT_SIZE_MAX;
  PIMSM_DEBUG_PRINTF("Max protocol packets = %d\n", 
            PIMSM_MAX_PROTOCOL_PACKETS );
  PIMSM_DEBUG_PRINTF("Max packet size = %d\n", 
            PIMSM_PKT_SIZE_MAX);
  PIMSM_DEBUG_PRINTF("Packets Send/Recv Buffer Pool Size= %d\n\n", 
            a );
  sum += a;

  a = PIMSM_MAX_JP_PACKETS * (sizeof(pim_encod_grp_addr_t) + 4 +
                (pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * sizeof(pim_encod_src_addr_t))  +
      PIMSM_MAX_PROTOCOL_PACKETS * 
      ((pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * sizeof(pim_encod_src_addr_t));
  PIMSM_DEBUG_PRINTF("Join/Prune message building Buffer Pool Size= %d\n\n", 
            a );
  sum += a;

  t1 = RADIX_TREE_HEAP_SIZE(PIMSM_RP_GRP_ENTRIES_MAX, sizeof(rpSetTreeKey_t));
  t2 = PIMSM_RP_GRP_ENTRIES_MAX * sizeof (pimsmRpSetNode_t);
  a = t1+t2;
  PIMSM_DEBUG_PRINTF("Max RP-GRP entries = %d\n", 
            PIMSM_RP_GRP_ENTRIES_MAX );
  PIMSM_DEBUG_PRINTF("RP radix tree size = %d\n\n", a);
  sum += a;

  a = PIMSM_RP_GRP_ENTRIES_MAX * sizeof(pimsmRpGrpNode_t);
  PIMSM_DEBUG_PRINTF("Max RP entries = %d\n", 
            PIMSM_RP_GRP_ENTRIES_MAX );
  PIMSM_DEBUG_PRINTF("RP-GRP Mapping Buffer Pool = %d\n\n", a);
  sum += a;

  a = PIMSM_RP_GRP_ADDR_LIST_NODES_MAX * sizeof(pimsmAddrList_t);
  PIMSM_DEBUG_PRINTF("Max RP-GRP Addr List Nodes = %d\n", 
     PIMSM_RP_GRP_ADDR_LIST_NODES_MAX);
  PIMSM_DEBUG_PRINTF("RP-GRP Addr List Buffer Pool = %d\n\n", a);
  sum += a;


  a = PIMSM_BSR_FRAGMENT_GRP_ENTRIES_MAX * sizeof(pimsmFragGrpRpNode_t);
  PIMSM_DEBUG_PRINTF("Max BSR Frag GRP Entries = %d\n", 
     PIMSM_BSR_FRAGMENT_GRP_ENTRIES_MAX);
  PIMSM_DEBUG_PRINTF("Frag GRP-RP Mapping buffer Pool-1 = %d\n\n", a);
  sum += a;

  a = PIMSM_BSR_FRAGMENT_RP_ENTRIES_MAX * sizeof(pimsmFragRpNode_t);
  PIMSM_DEBUG_PRINTF("Max BSR Frag RP Entries = %d\n", 
     PIMSM_BSR_FRAGMENT_RP_ENTRIES_MAX);
  PIMSM_DEBUG_PRINTF("Frag GRP-RP Mapping buffer Pool-2 = %d\n\n", a);
  sum += a;

  a = PIMSM_BSM_PKT_FRAGMENT_COUNT_MAX * sizeof(pimsmBSMPacketNode_t);
  PIMSM_DEBUG_PRINTF("BSM packets storage buffer Pool = %d\n\n", a);
  sum += a;

  a = PIMSM_BEST_ROUTE_CHANGES_MAX * sizeof(rtoRouteChange_t);
  PIMSM_DEBUG_PRINTF("Max RTO Best Route changes = %d\n", 
     PIMSM_BEST_ROUTE_CHANGES_MAX );
  PIMSM_DEBUG_PRINTF("RTO Route Change Buffer size  = %d\n\n", a);
  sum += a;

  PIMSM_DEBUG_PRINTF("PIMSM IPV4 /IPV6 Total Memory Used Size  = %lld\n\n", sum);  
  
  a = appTimerCbMemSizeGet(pimsmCb->maxNumOfTimers);
  PIMSM_DEBUG_PRINTF("Timer Control Block size  = %d\n\n", a);
  sum1 += a;
  /* allocate 10% more */
  a = sizeof(handle_list_t)+ ((pimsmCb->maxNumOfTimers * 11)/10) * sizeof(handle_member_t);
  PIMSM_DEBUG_PRINTF("Timer handle list size  = %d\n\n", a);
  sum1 += a;

  PIMSM_DEBUG_PRINTF("PIMSM Total Memory Used for ipv4 and ipv6  = %lld\n\n",
   sum * 2 + sum1);  
}

/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGShow ( L7_uchar8 family,L7_int32 count)
{
  pimsmSGNode_t         * pimsmSGNode;
  pimsmSGEntry_t *      pimsmSGEntry;
  L7_RC_t               rc;
  L7_uint32             pimsmSGEntryCount;
  interface_bitset_t oifList;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftJT = 0 , timeLeftRST = 0, timeLeftKAT = 0;
  L7_int32 localcount = 0;  
  
  L7_uchar8 tmpStr[MCAST_STRING_SIZE_MAX];
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }

  pimsmSGEntryCount = 0;     

  pimsmSGEntryCount = avlTreeCount(&pimsmCb->pimsmSGTree);
  
  PIMSM_DEBUG_PRINTF( " Number of Entries (S,G)  = %d\n", pimsmSGEntryCount );  
  /* print info */
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("SrcAddr, GrpAddr, SPT-bit, IIF, UpNbrAddr, OIFList\n" );
  PIMSM_DEBUG_PRINTF("UpStrmFSM, RegFSM, Flags\n" );
  PIMSM_DEBUG_PRINTF("JT, RST, KAT\n" );
  PIMSM_DEBUG_PRINTF("UpTime, NextHop, Metric, MetricPref\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  rc = pimsmSGFirstGet( pimsmCb, &pimsmSGNode );
  while(rc == L7_SUCCESS)
  {

    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }
    pimsmSGEntry = &pimsmSGNode->pimsmSGEntry;
    memset(&oifList,0,sizeof(interface_bitset_t));
    pimsmSGInhrtdOlist(pimsmCb,pimsmSGNode,&oifList);
    mcastIntfBitSetToString(&oifList, tmpStr, MCAST_STRING_SIZE_MAX);
    PIMSM_DEBUG_PRINTF( "%s, %s, %s, %d, %s, %s\n",
        inetAddrPrint( &pimsmSGEntry->pimsmSGSrcAddress, pimsmAddr1),
        inetAddrPrint( &pimsmSGEntry->pimsmSGGrpAddress, pimsmAddr2),
        (pimsmSGEntry->pimsmSGSPTBit == L7_TRUE) ? "TRUE" : "FALSE",
        pimsmSGEntry->pimsmSGRPFIfIndex,
        inetAddrPrint( &pimsmSGEntry->pimsmSGUpstreamNeighbor,  pimsmAddr3),
        tmpStr);
    pimsmDebugFlagsToString(pimsmSGNode->flags, tmpStr);     
    PIMSM_DEBUG_PRINTF( "%s, %s, %s\n",
        pimsmUpStrmSGStateName[pimsmSGEntry->pimsmSGUpstreamJoinState],
        pimsmRegPerSGStateName[pimsmSGEntry->pimsmSGDRRegisterState],
        tmpStr);
    appTimerTimeLeftGet( pimsmCb->timerCb,
                         pimsmSGEntry->pimsmSGUpstreamJoinTimer,
                         &timeLeftJT );
    appTimerTimeLeftGet( pimsmCb->timerCb,
                         pimsmSGEntry->pimsmSGDRRegisterStopTimer,
                         &timeLeftRST );
    appTimerTimeLeftGet( pimsmCb->timerCb,
                         pimsmSGEntry->pimsmSGKeepaliveTimer,
                         &timeLeftKAT );
        
    PIMSM_DEBUG_PRINTF( "%d, %d, %d\n",
          timeLeftJT, timeLeftRST, timeLeftKAT);
    now = osapiUpTimeRaw();
    uptime = now - pimsmSGEntry->pimsmSGCreateTime;
    PIMSM_DEBUG_PRINTF( "%d, %s, 0x%x, 0x%x\n",    
        uptime,
        inetAddrPrint( &pimsmSGEntry->pimsmSGRPFNextHop, pimsmAddr1),
        pimsmSGEntry->pimsmSGRPFRouteMetric,
        pimsmSGEntry->pimsmSGRPFRouteMetricPref);
    PIMSM_DEBUG_PRINTF( "\n");    
    localcount++;
    rc = pimsmSGNextGet( pimsmCb, pimsmSGNode, &pimsmSGNode );
  }
}

/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGIShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmSGNode_t         * pimsmSGNode;
  pimsmSGEntry_t *      pimsmSGEntry;
  pimsmSGIEntry_t *     pimsmSGIEntry;
  L7_RC_t               rc;
  L7_uint32             rtrIfNum;
  L7_uint32             pimsmSGIEntryCount;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftPPT = 0 , timeLeftJET = 0, timeLeftAT = 0;
  L7_int32 localcount = 0;    
  
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }
  pimsmSGIEntryCount = 0;
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("SrcAddr, GrpAddr\n" );
  PIMSM_DEBUG_PRINTF("DnStrmIndex, LCMem, DnStrmFSM, AssertFSM\n" );
  PIMSM_DEBUG_PRINTF("PPT, JET, AT \n" );
  PIMSM_DEBUG_PRINTF("UpTime, WinAddr, WinMetric, WinPref\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );  
  rc = pimsmSGFirstGet( pimsmCb, &pimsmSGNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }

    pimsmSGEntry = &pimsmSGNode->pimsmSGEntry;
    PIMSM_DEBUG_PRINTF( "%s, %s\n",
      inetAddrPrint( &pimsmSGEntry->pimsmSGSrcAddress, pimsmAddr1),
      inetAddrPrint( &pimsmSGEntry->pimsmSGGrpAddress, pimsmAddr2));
    
    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (rtrIfNum == pimsmSGEntry->pimsmSGRPFIfIndex)
      {
        continue;
      }
      pimsmSGIEntry = pimsmSGNode->pimsmSGIEntry[rtrIfNum];
      if(pimsmSGIEntry == L7_NULLPTR)
      {
        continue;
      }
      ++pimsmSGIEntryCount;    
      PIMSM_DEBUG_PRINTF( "%d, %s, %s, %s\n",
        pimsmSGIEntry->pimsmSGIIfIndex,
        (pimsmSGIEntry->pimsmSGILocalMembership == L7_TRUE) ? "TRUE" : "FALSE",        
        pimsmDnStrmPerIntfSGStateName[pimsmSGIEntry->pimsmSGIJoinPruneState],
        pimsmPerIntfSGAssertStateName[pimsmSGIEntry->pimsmSGIAssertState]);

      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmSGIEntry->pimsmSGIPrunePendingTimer,
                           &timeLeftPPT );     
      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmSGIEntry->pimsmSGIJoinExpiryTimer,
                           &timeLeftJET ); 
      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmSGIEntry->pimsmSGIAssertTimer,
                           &timeLeftAT );  
      PIMSM_DEBUG_PRINTF( "%d, %d, %d\n",
           timeLeftPPT, timeLeftJET, timeLeftAT);    
      now = osapiUpTimeRaw();
      uptime = now - pimsmSGIEntry->pimsmSGICreateTime;
      PIMSM_DEBUG_PRINTF( "%d, %s, 0x%x, 0x%x\n",    
          uptime,
          inetAddrPrint( &pimsmSGIEntry->pimsmSGIAssertWinnerAddress, pimsmAddr1),
          pimsmSGIEntry->pimsmSGIAssertWinnerMetric,
          pimsmSGIEntry->pimsmSGIAssertWinnerMetricPref);
      PIMSM_DEBUG_PRINTF( "\n");
    }
    localcount++;
    rc = pimsmSGNextGet( pimsmCb, pimsmSGNode, &pimsmSGNode );
  }
  PIMSM_DEBUG_PRINTF( " Number of Total Entries (S,G,I)  = %d\n",
                        pimsmSGIEntryCount );
}
/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGRptShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmSGRptNode_t         * pimsmSGRptNode;
  pimsmSGRptEntry_t *      pimsmSGRptEntry;
  L7_RC_t               rc;
  L7_uint32             pimsmSGRptEntryCount;
  interface_bitset_t oifList;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftOT = 0;
  L7_int32 localcount = 0;    
  
  L7_uchar8 tmpStr[MCAST_STRING_SIZE_MAX];
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }

  pimsmSGRptEntryCount = 0;     
  pimsmSGRptEntryCount = avlTreeCount(&pimsmCb->pimsmSGRptTree);  
  PIMSM_DEBUG_PRINTF( " Number of Entries (S,G,rpt)  = %d\n", pimsmSGRptEntryCount );
  
  /* print info */
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("SrcAddr, GrpAddr, IIF, OIFList\n" );
  PIMSM_DEBUG_PRINTF("UpStrmFSM, Flags\n" );
  PIMSM_DEBUG_PRINTF("OT\n" );
  PIMSM_DEBUG_PRINTF("UpTime\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  rc = pimsmSGRptFirstGet( pimsmCb, &pimsmSGRptNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }

    ++pimsmSGRptEntryCount;     
    pimsmSGRptEntry = &pimsmSGRptNode->pimsmSGRptEntry;
    memset(&oifList,0,sizeof(interface_bitset_t));
    pimsmSGRptInhrtdOlist(pimsmCb,pimsmSGRptNode,&oifList);
    mcastIntfBitSetToString(&oifList, tmpStr, MCAST_STRING_SIZE_MAX);
    PIMSM_DEBUG_PRINTF( "%s, %s, %d, %s\n",
        inetAddrPrint( &pimsmSGRptEntry->pimsmSGRptSrcAddress, pimsmAddr1),
        inetAddrPrint( &pimsmSGRptEntry->pimsmSGRptGrpAddress, pimsmAddr2),
        pimsmSGRptEntry->pimsmSGRptRPFIfIndex,
        tmpStr);
    pimsmDebugFlagsToString(pimsmSGRptNode->flags, tmpStr);     
    PIMSM_DEBUG_PRINTF( "%s, %s\n",
        pimsmUpStrmSGRptStateName[pimsmSGRptEntry->pimsmSGRptUpstreamPruneState],
        tmpStr);
    appTimerTimeLeftGet( pimsmCb->timerCb,
                         pimsmSGRptEntry->pimsmSGRptUpstreamOverrideTimer,
                         &timeLeftOT );
    PIMSM_DEBUG_PRINTF( "%d \n",timeLeftOT);
    now = osapiUpTimeRaw();
    uptime = now - pimsmSGRptEntry->pimsmSGRptCreateTime;
    PIMSM_DEBUG_PRINTF( "%d\n",uptime);
    PIMSM_DEBUG_PRINTF( "\n");   
    localcount++;
    rc = pimsmSGRptNextGet( pimsmCb, pimsmSGRptNode, &pimsmSGRptNode );
  }

}

/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGRptIShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmSGRptNode_t         * pimsmSGRptNode;
  pimsmSGRptEntry_t *      pimsmSGRptEntry;
  pimsmSGRptIEntry_t *     pimsmSGRptIEntry;
  L7_RC_t               rc;
  L7_uint32             rtrIfNum;
  L7_uint32             pimsmSGRptIEntryCount;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftPPT = 0 , timeLeftPET = 0;
  L7_int32 localcount = 0;    
  
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }
  pimsmSGRptIEntryCount = 0;
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("SrcAddr, GrpAddr\n" );
  PIMSM_DEBUG_PRINTF("DnStrmIndex, LCMem, DnStrmFSM\n" );
  PIMSM_DEBUG_PRINTF("PPT, PET \n" );
  PIMSM_DEBUG_PRINTF("UpTime\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );  
  rc = pimsmSGRptFirstGet( pimsmCb, &pimsmSGRptNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }

    pimsmSGRptEntry = &pimsmSGRptNode->pimsmSGRptEntry;
    PIMSM_DEBUG_PRINTF( "%s, %s\n",
      inetAddrPrint( &pimsmSGRptEntry->pimsmSGRptSrcAddress, pimsmAddr1),
      inetAddrPrint( &pimsmSGRptEntry->pimsmSGRptGrpAddress, pimsmAddr2));
    
    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (rtrIfNum == pimsmSGRptEntry->pimsmSGRptRPFIfIndex)
      {
        continue;
      }
      pimsmSGRptIEntry = pimsmSGRptNode->pimsmSGRptIEntry[rtrIfNum];
      if(pimsmSGRptIEntry == L7_NULLPTR)
      {
        continue;
      }
      ++pimsmSGRptIEntryCount;    
      PIMSM_DEBUG_PRINTF( "%d, %s, %s\n",
        pimsmSGRptIEntry->pimsmSGRptIIfIndex,  
        (pimsmSGRptIEntry->pimsmSGRptILocalMembership == L7_TRUE) ? "TRUE" : "FALSE",             
        pimsmDnStrmPerIntfSGRptStateName[pimsmSGRptIEntry->pimsmSGRptIJoinPruneState]);

      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmSGRptIEntry->pimsmSGRptIPrunePendingTimer,
                           &timeLeftPPT );     
      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmSGRptIEntry->pimsmSGRptIPruneExpiryTimer,
                           &timeLeftPET ); 
      PIMSM_DEBUG_PRINTF( "%d, %d\n",
           timeLeftPPT, timeLeftPET);    
      now = osapiUpTimeRaw();
      uptime = now - pimsmSGRptIEntry->pimsmSGRptICreateTime;
      PIMSM_DEBUG_PRINTF( "%d\n", uptime);
      PIMSM_DEBUG_PRINTF( "\n");      
    }
    localcount++;
    rc = pimsmSGRptNextGet( pimsmCb, pimsmSGRptNode, &pimsmSGRptNode );
  }
  PIMSM_DEBUG_PRINTF( " Number of Total Entries (S,G,rpt,I)  = %d\n",
                        pimsmSGRptIEntryCount );
}
/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarGShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmStarGNode_t         * pimsmStarGNode;
  pimsmStarGEntry_t *      pimsmStarGEntry;
  L7_RC_t               rc;
  L7_uint32             pimsmStarGEntryCount;
  interface_bitset_t oifList;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftJT = 0 ;
  L7_RC_t                   rc1;
  pimsmCache_t *      kernelCacheEntry;
  L7_uint32                 kerCachCount = 0;  
  L7_uchar8 tmpStr[MCAST_STRING_SIZE_MAX];
  L7_int32 localcount = 0;    
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }

  pimsmStarGEntryCount = 0;     
  pimsmStarGEntryCount = avlTreeCount(&pimsmCb->pimsmStarGTree);    
  PIMSM_DEBUG_PRINTF( " Number of Entries (*,G)  = %d\n", pimsmStarGEntryCount );  
  /* print info */
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("GrpAddr, RPAddr, RPLocal, IIF, UpNbrAddr, OIFList\n" );
  PIMSM_DEBUG_PRINTF("UpStrmFSM, Flags\n" );
  PIMSM_DEBUG_PRINTF("JT\n" );
  PIMSM_DEBUG_PRINTF("UpTime, NextHop, Metric, MetricPref\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  rc = pimsmStarGFirstGet( pimsmCb, &pimsmStarGNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }

    pimsmStarGEntry = &pimsmStarGNode->pimsmStarGEntry;
    memset(&oifList,0,sizeof(interface_bitset_t));
    pimsmStarGImdtOlist(pimsmCb,pimsmStarGNode,&oifList);
    mcastIntfBitSetToString(&oifList, tmpStr, MCAST_STRING_SIZE_MAX);
    PIMSM_DEBUG_PRINTF( "%s, %s, %s, %d, %s, %s\n",
        inetAddrPrint( &pimsmStarGEntry->pimsmStarGGrpAddress, pimsmAddr1),
        inetAddrPrint( &pimsmStarGEntry->pimsmStarGRPAddress, pimsmAddr2),
        (pimsmStarGEntry->pimsmStarGRPIsLocal == L7_TRUE) ? "TRUE" : "FALSE",
        pimsmStarGEntry->pimsmStarGRPFIfIndex,
        inetAddrPrint( &pimsmStarGEntry->pimsmStarGUpstreamNeighbor,  pimsmAddr3),
        tmpStr);
    pimsmDebugFlagsToString(pimsmStarGNode->flags, tmpStr);     
    PIMSM_DEBUG_PRINTF( "%s, %s\n",
        pimsmUpStrmStarGStateName[pimsmStarGEntry->pimsmStarGUpstreamJoinState],
        tmpStr);
    appTimerTimeLeftGet( pimsmCb->timerCb,
                         pimsmStarGEntry->pimsmStarGUpstreamJoinTimer,
                         &timeLeftJT );
    PIMSM_DEBUG_PRINTF( "%d\n",
          timeLeftJT);
    now = osapiUpTimeRaw();
    uptime = now - pimsmStarGEntry->pimsmStarGCreateTime;
    PIMSM_DEBUG_PRINTF( "%d, %s, 0x%x, 0x%x\n",    
        uptime,
        inetAddrPrint( &pimsmStarGEntry->pimsmStarGRPFNextHop, pimsmAddr1),
        pimsmStarGEntry->pimsmStarGRPFRouteMetric,
        pimsmStarGEntry->pimsmStarGRPFRouteMetricPref);

    PIMSM_DEBUG_PRINTF("-----:CacheList -START:-----\n" );
    PIMSM_DEBUG_PRINTF("SrcAddr, GrpAddr, ByteCnt, PktCnt, SGByteCnt\n" );
    kerCachCount = 0;
    rc1 = pimsmStarGCacheGetFirst( pimsmCb, pimsmStarGNode, &kernelCacheEntry );
    while(rc1 == L7_SUCCESS)
    {
      ++kerCachCount;  
      PIMSM_DEBUG_PRINTF( "%s, %s, %d, %d, %d\n",
        inetAddrPrint( &kernelCacheEntry->pimsmSrcAddr, pimsmAddr1),
        inetAddrPrint( &kernelCacheEntry->pimsmGrpAddr, pimsmAddr2),
        kernelCacheEntry->pimsmSGRealtimeByteCount,
        kernelCacheEntry->pimsmSGRealtimePktcnt,
         kernelCacheEntry->pimsmSGByteCount);      
      rc1 = pimsmStarGCacheNextGet( pimsmCb, pimsmStarGNode,
              kernelCacheEntry, &kernelCacheEntry );
    }  
    PIMSM_DEBUG_PRINTF("-----:CacheList -END: Count = %d-----\n",  
        kerCachCount);
    PIMSM_DEBUG_PRINTF( "\n");
    localcount++;
    rc = pimsmStarGNextGet( pimsmCb, pimsmStarGNode, &pimsmStarGNode );
    
  }
}

/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarGIShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmStarGNode_t         * pimsmStarGNode;
  pimsmStarGEntry_t *      pimsmStarGEntry;
  pimsmStarGIEntry_t *     pimsmStarGIEntry;
  L7_RC_t               rc;
  L7_uint32             rtrIfNum;
  L7_uint32             pimsmStarGIEntryCount;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftPPT = 0 , timeLeftJET = 0, timeLeftAT = 0;
  L7_int32 localcount = 0;  
  
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }
  pimsmStarGIEntryCount = 0;
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("GrpAddr\n" );
  PIMSM_DEBUG_PRINTF("DnStrmIndex, LCMem, DnStrmFSM, AssertFSM\n" );
  PIMSM_DEBUG_PRINTF("PPT, JET, AT \n" );
  PIMSM_DEBUG_PRINTF("UpTime, WinAddr, WinMetric, WinPref\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );  
  rc = pimsmStarGFirstGet( pimsmCb, &pimsmStarGNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }

    pimsmStarGEntry = &pimsmStarGNode->pimsmStarGEntry;
    PIMSM_DEBUG_PRINTF( "%s\n",
      inetAddrPrint( &pimsmStarGEntry->pimsmStarGGrpAddress, pimsmAddr2));
    
    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (rtrIfNum == pimsmStarGEntry->pimsmStarGRPFIfIndex)
      {
        continue;
      }
    
      pimsmStarGIEntry = pimsmStarGNode->pimsmStarGIEntry[rtrIfNum];
      if(pimsmStarGIEntry == L7_NULLPTR)
      {
        continue;
      }
      ++pimsmStarGIEntryCount;    
      PIMSM_DEBUG_PRINTF( "%d, %s, %s, %s\n",
        pimsmStarGIEntry->pimsmStarGIIfIndex,
        (pimsmStarGIEntry->pimsmStarGILocalMembership == L7_TRUE) ? "TRUE" : "FALSE",        
        pimsmDnStrmPerIntfStarGStateName[pimsmStarGIEntry->pimsmStarGIJoinPruneState],
        pimsmPerIntfStarGAssertStateName[pimsmStarGIEntry->pimsmStarGIAssertState]);

      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmStarGIEntry->pimsmStarGIPrunePendingTimer,
                           &timeLeftPPT );     
      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmStarGIEntry->pimsmStarGIJoinExpiryTimer,
                           &timeLeftJET ); 
      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmStarGIEntry->pimsmStarGIAssertTimer,
                           &timeLeftAT );  
      PIMSM_DEBUG_PRINTF( "%d, %d, %d\n",
           timeLeftPPT, timeLeftJET, timeLeftAT);    
      now = osapiUpTimeRaw();
      uptime = now - pimsmStarGIEntry->pimsmStarGICreateTime;
      PIMSM_DEBUG_PRINTF( "%d, %s,  0x%x, 0x%x\n",    
          uptime,
          inetAddrPrint( &pimsmStarGIEntry->pimsmStarGIAssertWinnerAddress, 
                pimsmAddr1),
          pimsmStarGIEntry->pimsmStarGIAssertWinnerMetric,
          pimsmStarGIEntry->pimsmStarGIAssertWinnerMetricPref);
      PIMSM_DEBUG_PRINTF( "\n");    
    }
    localcount++;
    rc = pimsmStarGNextGet( pimsmCb, pimsmStarGNode, &pimsmStarGNode );
  }
  PIMSM_DEBUG_PRINTF( " Number of Total Entries (*,G,I)  = %d\n",
                        pimsmStarGIEntryCount );
}
/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarStarRpShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmStarStarRpNode_t         * pimsmStarStarRpNode;
  pimsmStarStarRpEntry_t *      pimsmStarStarRpEntry;
  L7_RC_t               rc;
  L7_uint32             pimsmStarStarRpEntryCount;
  interface_bitset_t oifList;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftJT = 0 ;
  L7_RC_t                   rc1;
  pimsmCache_t *      kernelCacheEntry;
  L7_uint32                 kerCachCount = 0;  
  
  L7_uchar8 tmpStr[MCAST_STRING_SIZE_MAX];
  L7_int32 localcount = 0;    
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }

  pimsmStarStarRpEntryCount = 0;     
  pimsmStarStarRpEntryCount = SLLNumMembersGet(&pimsmCb->pimsmStarStarRpList);

  PIMSM_DEBUG_PRINTF( " Number of Entries (*,*,RP)  = %d\n",
        pimsmStarStarRpEntryCount );
  
  /* print info */
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("RPAddr, IIF, UpNbrAddr, OIFList\n" );
  PIMSM_DEBUG_PRINTF("UpStrmFSM, Flags\n" );
  PIMSM_DEBUG_PRINTF("JT\n" );
  PIMSM_DEBUG_PRINTF("UpTime, NextHop, Metric, MetricPref\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  rc = pimsmStarStarRpFirstGet( pimsmCb, &pimsmStarStarRpNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }
    pimsmStarStarRpEntry = &pimsmStarStarRpNode->pimsmStarStarRpEntry;
    memset(&oifList,0,sizeof(interface_bitset_t));
    pimsmStarStarRpImdtOlist(pimsmCb,pimsmStarStarRpNode,&oifList);
    mcastIntfBitSetToString(&oifList, tmpStr, MCAST_STRING_SIZE_MAX);
    PIMSM_DEBUG_PRINTF( "%s, %d, %s, %s\n",
        inetAddrPrint( &pimsmStarStarRpEntry->pimsmStarStarRpRPAddress, pimsmAddr2),
        pimsmStarStarRpEntry->pimsmStarStarRpRPFIfIndex,
        inetAddrPrint( &pimsmStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor,
                      pimsmAddr3),
        tmpStr);
    pimsmDebugFlagsToString(pimsmStarStarRpNode->flags, tmpStr);     
    PIMSM_DEBUG_PRINTF( "%s, %s\n",
        pimsmUpStrmStarStarRPStateName[pimsmStarStarRpEntry->pimsmStarStarRpUpstreamJoinState],
        tmpStr);
    appTimerTimeLeftGet( pimsmCb->timerCb,
                         pimsmStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer,
                         &timeLeftJT );
    PIMSM_DEBUG_PRINTF( "%d\n",
          timeLeftJT);
    now = osapiUpTimeRaw();
    uptime = now - pimsmStarStarRpEntry->pimsmStarStarRpCreateTime;
    PIMSM_DEBUG_PRINTF( "%d, %s,  0x%x, 0x%x\n",    
        uptime,
        inetAddrPrint( &pimsmStarStarRpEntry->pimsmStarStarRpRPFNextHop, 
              pimsmAddr1),
        pimsmStarStarRpEntry->pimsmStarStarRpRPFRouteMetric,
        pimsmStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref);
    PIMSM_DEBUG_PRINTF("-----:CacheList -START:-----\n" );
    PIMSM_DEBUG_PRINTF("SrcAddr, GrpAddr, ByteCnt, PktCnt, SGByteCnt\n" );
    kerCachCount = 0;
    rc1 = pimsmStarStarRpCacheGetFirst( pimsmCb, pimsmStarStarRpNode, 
            &kernelCacheEntry );
    while(rc1 == L7_SUCCESS)
    {
      ++kerCachCount;  
      PIMSM_DEBUG_PRINTF( "%s, %s, %d, %d, %d\n",
        inetAddrPrint( &kernelCacheEntry->pimsmSrcAddr, pimsmAddr1),
        inetAddrPrint( &kernelCacheEntry->pimsmGrpAddr, pimsmAddr2),
        kernelCacheEntry->pimsmSGRealtimeByteCount,
        kernelCacheEntry->pimsmSGRealtimePktcnt,
         kernelCacheEntry->pimsmSGByteCount);      
      rc1 = pimsmStarStarRpCacheNextGet( pimsmCb, pimsmStarStarRpNode,
              kernelCacheEntry, &kernelCacheEntry );
    }  
    PIMSM_DEBUG_PRINTF("-----:CacheList -END: Count = %d-----\n",  
        kerCachCount);      
    PIMSM_DEBUG_PRINTF( "\n");
    localcount++;     
    rc = pimsmStarStarRpNextGet( pimsmCb, pimsmStarStarRpNode, 
          &pimsmStarStarRpNode );
  }
}

/******************************************************************************
* @purpose  Display entire (S,G) tree
*
* @param    pimsmCb @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarStarRpIShow ( L7_uchar8 family,L7_int32 count )
{
  pimsmStarStarRpNode_t         * pimsmStarStarRpNode;
  pimsmStarStarRpEntry_t *      pimsmStarStarRpEntry;
  pimsmStarStarRpIEntry_t *     pimsmStarStarRpIEntry;
  L7_RC_t               rc;
  L7_uint32             rtrIfNum;
  L7_uint32             pimsmStarStarRpIEntryCount;
  pimsmCB_t *   pimsmCb;
  L7_uint32     now;
  L7_uint32     uptime;
  L7_uint32    timeLeftPPT = 0 , timeLeftJET = 0;
  L7_int32 localcount = 0;    
  
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Control Block is NULL\r\n" );
    return;
  }
  pimsmStarStarRpIEntryCount = 0;
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMSM_DEBUG_PRINTF("RPAddr\n" );
  PIMSM_DEBUG_PRINTF("DnStrmIndex, DnStrmFSM \n" );
  PIMSM_DEBUG_PRINTF("PPT, JET \n" );
  PIMSM_DEBUG_PRINTF("UpTime\n" );
  PIMSM_DEBUG_PRINTF("-------------------------------------------------------\n" );  
  rc = pimsmStarStarRpFirstGet( pimsmCb, &pimsmStarStarRpNode );
  while(rc == L7_SUCCESS)
  {
    if((count!= L7_NULL)&&(localcount >= count))
    {
      break;
    }

    pimsmStarStarRpEntry = &pimsmStarStarRpNode->pimsmStarStarRpEntry;
    PIMSM_DEBUG_PRINTF( "%s\n",
        inetAddrPrint( &pimsmStarStarRpEntry->pimsmStarStarRpRPAddress, pimsmAddr2));

    for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (rtrIfNum == pimsmStarStarRpEntry->pimsmStarStarRpRPFIfIndex)
      {
        continue;
      }    
      pimsmStarStarRpIEntry = pimsmStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
      
      if(pimsmStarStarRpIEntry == L7_NULLPTR)
      {
        continue;
      }
      ++pimsmStarStarRpIEntryCount;    
      PIMSM_DEBUG_PRINTF( "%d, %s\n",
        pimsmStarStarRpIEntry->pimsmStarStarRpIIfIndex,        
        pimsmDnStrmPerIntfStarStarRPStateName[pimsmStarStarRpIEntry->pimsmStarStarRpIJoinPruneState]);

      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmStarStarRpIEntry->pimsmStarStarRpIPrunePendingTimer,
                           &timeLeftPPT );     
      appTimerTimeLeftGet( pimsmCb->timerCb,
                           pimsmStarStarRpIEntry->pimsmStarStarRpIJoinExpiryTimer,
                           &timeLeftJET ); 
      PIMSM_DEBUG_PRINTF( "%d, %d\n",
           timeLeftPPT, timeLeftJET);    
      now = osapiUpTimeRaw();
      uptime = now - pimsmStarStarRpIEntry->pimsmStarStarRpICreateTime;
      PIMSM_DEBUG_PRINTF( "%d \n", uptime);
      PIMSM_DEBUG_PRINTF( "\n");
    }
    localcount++;
    rc = pimsmStarStarRpNextGet( pimsmCb, pimsmStarStarRpNode, &pimsmStarStarRpNode );
  }
  PIMSM_DEBUG_PRINTF( " Number of Total Entries (*,*,RP,I)  = %d\n",
                        pimsmStarStarRpIEntryCount );
}

/******************************************************************************
* @purpose  Display PIMSM Mcast Routing Table (MRT)
*
* @param    family @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugMRTTableShow ( L7_uchar8 family,L7_int32 count)
{
  pimsmDebugSGShow( family,count );
  pimsmDebugSGRptShow( family,count  );
  pimsmDebugStarGShow( family,count  );
  pimsmDebugStarStarRpShow( family,count  );
}

/******************************************************************************
* @purpose  Display PIMSM Mcast Routing Table (MRT)
*
* @param    family @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugMRTITableShow ( L7_uchar8 family, L7_int32 count )
{
  pimsmDebugSGIShow( family,count  );
  pimsmDebugSGRptIShow( family,count  );
  pimsmDebugStarGIShow( family,count  );
  pimsmDebugStarStarRpIShow( family,count  );
}
