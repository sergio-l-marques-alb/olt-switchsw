/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  mcast_debug.c
*
* @purpose   MCAST Mapping Debug functions
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    01/24/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "nimapi.h"
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"
#include "l3_addrdefs.h"
#include "l7handle_api.h"
#include "support_api.h"
#include "osapi_support.h"
#include "nimapi.h"
#include "mcast_wrap.h"
#include "mcast_map.h"
#include "mcast_util.h"
#include "mcast_debug.h" 
#include "heap_api.h" 
#include "l7_mcast_api.h"

#ifndef MCAST_STATIC_MROUTE_DEBUG_UT

#include "usmdb_util_api.h"
#include "usmdb_mib_mcast_api.h"

#endif /* MCAST_STATIC_MROUTE_DEBUG_UT */
/*
 * Global Declarations
 */
static L7_uchar8 mcastDebugFlags[MCAST_MAP_NUM_FLAG_BYTES];
static L7_BOOL   mcastDebugEnabled = L7_FALSE;

L7_uint32 mcastMapTotalBytesAllocated = 0;
L7_uint32 mcastTotalBytesAllocated = 0;

/* MCAST Packet debug traces */
static mcastDebugTraceFlags_t mcastDebugTraceFlag[MCAST_MAP_CB_MAX];

extern mcastDebugCfg_t mcastDebugCfg;


static void mcastMapDebugRegisteredMulticastInfoDump(void);

extern void dvmrpMapDebugQueueStatsShow();
extern void mgmdMapDebugQueueStatsShow();
extern void pimdmMapDebugQueueStatsShow();
extern void pimsmMapDebugQueueStatsShow();


/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MCASTMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDebugEnable(void)
{
  mcastDebugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MCASTMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDebugDisable(void)
{
  mcastDebugEnabled = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MCASTMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDebugAllSet(void)
{
  memset(mcastDebugFlags, MCAST_MAP_DEBUG_ALL_SET, sizeof(mcastDebugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MCASTMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDebugAllReset(void)
{
  memset(mcastDebugFlags, L7_NULL, sizeof(mcastDebugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MCASTMAP.
*
* @param    flag      @b{(input)}  Debug Flag to set.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDebugFlagSet(MCAST_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= MCAST_MAP_DEBUG_FLAG_MAX)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Debug Flag not in range.\n");
    return L7_FAILURE;
  }
  mcastDebugFlags[flag/MCAST_MAP_FLAG_BITS_MAX] |= 
             (MCAST_MAP_FLAG_VALUE << (flag % MCAST_MAP_FLAG_BITS_MAX));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MCASTMAP.
*
* @param    flag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDebugFlagReset(MCAST_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= MCAST_MAP_DEBUG_FLAG_MAX)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Debug Flag not in range.\n");
    return L7_FAILURE;
  }
  mcastDebugFlags[flag/MCAST_MAP_FLAG_BITS_MAX] &= 
            (~(MCAST_MAP_FLAG_VALUE << (flag % MCAST_MAP_FLAG_BITS_MAX)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MCASTMAP.
*
* @param    debugType  @b{(input)}  Debug Flag to check.
*
* @returns  L7_TRUE    if the Debug trace flag is turned ON.
*           L7_FALSE   if the Debug trace flag is turned OFF.
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL mcastMapDebugFlagCheck(MCAST_MAP_DEBUG_FLAGS_t debugType)
{
  if(mcastDebugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  } 
  if(debugType >= MCAST_MAP_DEBUG_FLAG_MAX)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Debug Flag not in range.\n");
    return L7_FALSE;
  }
  if((mcastDebugFlags[debugType/MCAST_MAP_FLAG_BITS_MAX] & 
        (MCAST_MAP_FLAG_VALUE << (debugType % MCAST_MAP_FLAG_BITS_MAX))) != 0)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current MCASTMAP Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @comments
*
* @end
*********************************************************************/
void mcastMapDebugFlagShow(void)
{
  L7_uint32 index;

  if(mcastDebugEnabled == L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN," MCASTMAP Debugging : Enabled\n");
    for(index = 0;  index < MCAST_MAP_DEBUG_FLAG_MAX ; index ++)
    {
      if(mcastMapDebugFlagCheck(index) == L7_TRUE)
        MCAST_MAP_DEBUG_PRINTF("     debugFlag [%d] : %s\n", index , "Enabled");
      else
        MCAST_MAP_DEBUG_PRINTF("     debugFlag [%d] : %s\n", index , "Disabled");
    }
  }
  else
  {
    MCAST_MAP_DEBUG_PRINTF (" MCASTMAP Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the MCASTMAP Debug utility
*
* @param    None.
*
* @returns  None.
*
* @comments
*
* @end
*********************************************************************/
void mcastMapDebugHelp(void)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN," Use mcastMapDebugEnable()/mcastMapDebugDisable()"
                 "to Enable/Disable Debug trace in MCASTMAP\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN," Use mcastMapDebugFlagSet(flag)/mcastMapDebugFlagReset(flag)"
               " to Enable/Disable specific functionality traces\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN," Use mcastMapDebugFlagAllSet()/mcastMapDebugFlagAllReset()"
               " to Enable/Disable all traces\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"     Various Debug Trace flags and their definitions"  
                                                  "are as follows ;\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"         0  -> Trace the complete Receive Data path\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"         1  -> Trace the complete Transmission Data path\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"         2  -> Trace all the EVENT generations"
                                                       "and receptions\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"         3  -> Trace all Timer activities\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"         4  -> Trace all failures\n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"         5  -> Trace all the APIs invoked\n");

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastCfgShow() \n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastConfigDataTestShow() \n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastDebugPktRcvrInfoShow()  \n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapDebugQueueStatsShow() \n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapDebugQueueStatsClear() \n");
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapDebugSize() \n");
  
}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void mcastCfgShow(void)
{

    nimUSP_t    usp;
    L7_uint32   i, itr;
    L7_uint32   intIfNum;


   /*--------------------------------*/
   /* cfgParms                       */
   /*                                */
   /* DO A RAW DUMP OF THE CONFIG    */
   /* FILE IF A PREVIOUSLY EXISTING  */
   /* DEBUG DUMP ROUTINE DOES NOT    */
   /* EXIST                          */
   /*--------------------------------*/


    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"\n");

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"STATIC MCAST ROUTES\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"-------------------\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"-------------------\n");

    for (itr = 0; itr < L7_RTR_MAX_STATIC_MROUTES; itr++)
    {

      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"\n");
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"ENTRY %d\n", itr);
      MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FLAG_MIN,"Source  = ",
      &mcastGblVariables_g.mcastMapCfgData.rtr.mcastStaticRtsCfgData[itr].source);
      MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FLAG_MIN,"SrcMask = ",
      &mcastGblVariables_g.mcastMapCfgData.rtr.mcastStaticRtsCfgData[itr].mask);
      MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FLAG_MIN,"RpfAddr = ",
      &mcastGblVariables_g.mcastMapCfgData.rtr.mcastStaticRtsCfgData[itr].rpfAddr);
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"Metric  = %d",
      mcastGblVariables_g.mcastMapCfgData.rtr.mcastStaticRtsCfgData[itr].preference);    
    }

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"ADMIN SCOPE ENTRIES\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"-------------------\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"-------------------\n");


    for (itr = 0; itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES; itr++)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"\n");
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"ENTRY %d\n", itr);

      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"inUse  = %d\n",
      mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].inUse);
      if ((nimIntIfFromConfigIDGet(
          &mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].ifConfigId, 
          &intIfNum)) == L7_SUCCESS)
      {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"intIfNum  = %d\n",intIfNum);
      }
      MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FLAG_MIN,"GrpAddr = ",
      &mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].groupIpAddr);
      MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FLAG_MIN,"GrpMask = ",
      &mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].groupIpMask);
    }

    
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapCfgData.rtr.numStaticMRouteEntries = %d\n",mcastGblVariables_g.mcastMapCfgData.rtr.numStaticMRouteEntries);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapCfgData.rtr.numAdminScopeEntries  = %d\n",mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries);

    
    /* Print interface information, if any */

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "Printing interface data\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "-----------------------\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"\n");
    for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
    {
        /* FOR INTERFACE STRUCTURES,  ALWAYS PRINT USP as well as intIfNUm
           TO ENSURE PROPER INSTANCE OF CONFIG MIGRATED */

        intIfNum = 0;
        memset((void *)&usp, 0, sizeof(nimUSP_t));

        if (nimIntIfFromConfigIDGet(&mcastGblVariables_g.
              mcastMapCfgData.intf[i].configId, &intIfNum) == L7_SUCCESS)
        {
            if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
            {
                MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "USP = %d/%d/%d  (intIfNum  = %d \n",
                               usp.unit, usp.slot, usp.port, intIfNum);
            }
            else
            {

                MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"\n%50/0/0d");
                MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "USP = 0/0/0  (intIfNum  = %d \n",  intIfNum);

            }
        } 
    
        /* PRINT REST OF INTERFACE CONFIGURATION */
                    
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapCfgData.intf[i].ipMRouteIfTtlThresh = %d\n",  
                     mcastGblVariables_g.mcastMapCfgData.intf[i].
                     ipMRouteIfTtlThresh);

        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"\n");

    }


}



/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void mcastConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(mcastGblVariables_g.mcastMapCfgData.cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*--------------------------------*/
   /* cfgParms                       */
   /*--------------------------------*/

    mcastCfgShow();


    /*-------------------------------*/
    /* Scaling Constants             */
    /*                               */
    /*                               */
    /* NOTE:                         */
    /* Print any limits of arrays    */
    /* or anything else in the       */
    /* confi files which might       */
    /* affect the size of the config */
    /* file.                         */
    /*                               */
    /*-------------------------------*/

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "Scaling Constants\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "-----------------\n");


    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN, "L7_IPMAP_INTF_MAX_COUNT - %d\n", L7_IPMAP_INTF_MAX_COUNT);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FLAG_MIN,"mcastMapCfgData.checkSum : %u\n", 
                 mcastGblVariables_g.mcastMapCfgData.checkSum);
}

/*********************************************************************
*
* @purpose  Dump the contents of the Packet Receiver Block.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void
mcastDebugPktRcvrInfoShow (void)
{
  L7_PktRcvr_t *pktRcvr = L7_NULLPTR;
  L7_MRP_TYPE_t index = 0;

  if ((pktRcvr = mcastGblVariables_g.pktRcvr) == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG_PRINTF ("Packet Receiver is not Created.\n");
    return;
  }

  MCAST_MAP_DEBUG_PRINTF ("\nPacket Receiver Info...\n\n");
  MCAST_MAP_DEBUG_PRINTF ("componentId - %d.\n", pktRcvr->componentId);
  MCAST_MAP_DEBUG_PRINTF ("maxPktFDs   - %d.\n", pktRcvr->maxPktFds);
  MCAST_MAP_DEBUG_PRINTF ("MAX fd width   - %d.\n", pktRcvr->fds_size);
  MCAST_MAP_DEBUG_PRINTF ("FDList...\n");

  MCAST_MAP_DEBUG_PRINTF ("protoType  pktSockFd  bufPool  eventType  ReadFDIsSet\n");
  for (index = 0; index < pktRcvr->maxPktFds; index++)
  {
    MCAST_MAP_DEBUG_PRINTF ("\t %d.", pktRcvr->fdList[index].id);
    MCAST_MAP_DEBUG_PRINTF ("\t %d,", pktRcvr->fdList[index].pktSockFd);
    MCAST_MAP_DEBUG_PRINTF ("\t %d,", pktRcvr->fdList[index].bufPool);
    MCAST_MAP_DEBUG_PRINTF ("\t %d,", pktRcvr->fdList[index].evType);
    if(FD_ISSET((pktRcvr->fdList[index]).pktSockFd,&pktRcvr->evRdFds))
    {
     MCAST_MAP_DEBUG_PRINTF ("\t TRUE"); 
    } 
    else
    {
     MCAST_MAP_DEBUG_PRINTF ("\t FALSE"); 
    }
    MCAST_MAP_DEBUG_PRINTF ("\n");
  }
  return;
}

/*********************************************************************
*
* @purpose  Shows the MCASTMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void mcastMapDebugQueueStatsShow(void)
{
  L7_uint32 qIndex;
  L7_uchar8 qName[20];

  sysapiPrintf(" QueueName  CurMsgCnt HighWaterMark  SendFailCnt RxSuccessCnt MaxQSize MsgSize \n");
  sysapiPrintf ("\n----------------------------------------------------------------------------\n");
  for (qIndex = MCASTMAP_APP_TIMER_Q; qIndex < MCASTMAP_MAX_Q; qIndex++ )
  {
    mcastMapQueue_t   *mcastMapQueue = &mcastGblVariables_g.mcastMapQueue[qIndex];
    osapiStrncpy(qName, (mcastMapQueue->QName) + 8, 20);
    sysapiPrintf("\n %-15s  %-10d %-10d %-10d %-10d %-7d %-7d  ",
                 qName, mcastMapQueue->QCurrentMsgCnt, mcastMapQueue->QMaxRx,
                 mcastMapQueue->QSendFailedCnt, mcastMapQueue->QRxSuccess,
                 mcastMapQueue->QCount , mcastMapQueue->QSize);
        
  }
  sysapiPrintf("\n");
}


/*********************************************************************
*
* @purpose  Clears the MCASTMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void mcastMapDebugQueueStatsClear(void)
{
 L7_uint32 QIndex=0;

 for(QIndex=0;QIndex < MCASTMAP_MAX_Q; QIndex++)
 {
   mcastGblVariables_g.mcastMapQueue[QIndex].QCurrentMsgCnt = 0;
   mcastGblVariables_g.mcastMapQueue[QIndex].QSendFailedCnt = 0;
   mcastGblVariables_g.mcastMapQueue[QIndex].QMaxRx = 0;
   mcastGblVariables_g.mcastMapQueue[QIndex].QRxSuccess = 0;
 }
}
/*********************************************************************
*
* @purpose  Gives the memroty allocated by Mcastmap
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mcastMapDebugSize()
{
  L7_uint32 s1,s2,s3,s4, s5, sIPv4 = 0, sIPv6 = 0;

  sysapiPrintf(" \n --- Queue sizes --- \n");
  sysapiPrintf(" \n Name         MsgSize         Msg Count    Memory\n");
  sysapiPrintf(" \n %s         %d         %d       %d",
               mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QName,
                mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QSize,
               mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QCount,
               MCASTMAP_APP_TMR_MSG_SIZE * MCASTMAP_APP_TMR_MSG_COUNT);
  sysapiPrintf(" \n %s         %d         %d       %d",
               mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QName,
                mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QSize,
               mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QCount,
               MCASTMAP_MSG_SIZE * MCASTMAP_MSG_COUNT);
  sysapiPrintf(" \n %s         %d         %d       %d",
               mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QName,
                mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QSize,
               mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QCount,
               MCASTMAP_CTRL_PKT_MSG_SIZE * MCASTMAP_CTRL_PKT_MSG_COUNT);

  sysapiPrintf("\n  TOTAL Message Queue size = %d ",(s1 = ((MCASTMAP_APP_TMR_MSG_SIZE * MCASTMAP_APP_TMR_MSG_COUNT)+
               (MCASTMAP_MSG_SIZE * MCASTMAP_MSG_COUNT) + 
               (MCASTMAP_CTRL_PKT_MSG_SIZE * MCASTMAP_CTRL_PKT_MSG_COUNT))));
  sIPv4 += s1;
  
  sysapiPrintf(" \n --- Handle List size --- \n");

  sysapiPrintf("\n  HandleSize = %d ",sizeof (handle_member_t));
  sysapiPrintf("\n  NumHandles = %d ", MCAST_MAX_TIMERS);
  sysapiPrintf("\n  TOTAL handle list size = %d ", s2 = (sizeof (handle_member_t) * MCAST_MAX_TIMERS));
  sIPv4 += s2;

  sysapiPrintf(" \n --- AppTmr BufferPool size --- \n");

  sysapiPrintf("\n  appTmr Node size = %d ",L7_APP_TMR_NODE_SIZE);
  sysapiPrintf("\n  NumTimers = %d ", MCAST_MAX_TIMERS);
  sysapiPrintf("\n  TOTAL TimerNode size = %d ", s3 = (L7_APP_TMR_NODE_SIZE * MCAST_MAX_TIMERS));
  sIPv4 += s3;

  sysapiPrintf(" \n --- Heap Init size --- \n");

  sysapiPrintf("\n  TOTAL Heap Size size = %d ", s4 = (MCAST_V4_HEAP_SIZE));
  sIPv4 += s4;

  sysapiPrintf(" \n --- Control Pkt Buffer Size  --- \n");

  sysapiPrintf("\n  CtrlPkt MTU size = %d ",L7_MULTICAST_MAX_IP_MTU);
  sysapiPrintf("\n  Max Pkts = %d ", MCAST_PKT_BUF_COUNT);
  sysapiPrintf("\n  TOTAL TimerNode size = %d ", s5 = (MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU));
  sIPv4 += s5;

  sysapiPrintf(" \n ******  IPv4 Memory == %d ", sIPv4);

  s1 = s2 = s3 = 0;
  sysapiPrintf(" \n --- IPv6 Handle List size --- \n");

  sysapiPrintf("\n  HandleSize = %d ",sizeof (handle_member_t));
  sysapiPrintf("\n  NumHandles = %d ", MCAST_MAX_TIMERS);
  sysapiPrintf("\n  TOTAL handle list size = %d ", s1 = (sizeof (handle_member_t) * MCAST_MAX_TIMERS));
  sIPv6 += s1;

  sysapiPrintf(" \n --- IPv6 Heap Init size --- \n");

  sysapiPrintf("\n  TOTAL Heap Size size = %d ", s2 = (MCAST_V6_HEAP_SIZE));
  sIPv6 += s2;

  sysapiPrintf(" \n --- IPv6 Control Pkt Buffer Size  --- \n");

  sysapiPrintf("\n  CtrlPkt MTU size = %d ",L7_MULTICAST_MAX_IP_MTU);
  sysapiPrintf("\n  Max Pkts = %d ", MCAST_PKT_BUF_COUNT);
  sysapiPrintf("\n  TOTAL TimerNode size = %d ", s3 = (MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU));
  sIPv6 += s3;

  sysapiPrintf(" \n ******  IPv6 Memory == %d ", sIPv6);

  sysapiPrintf(" \n $$$$$$$$$ GRAND TOTAL MCASTMAP MEMORY == %d $$$$$$$$$$$$$$ \n", sIPv4 + sIPv6);
}


/*============================================================================*/
/*===================  START OF MCAST PACKET DEBUG TRACES =====================*/
/*============================================================================*/


/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
*
* @param   family	@b{(input)} Address Family
* @param   rxTrace	@b{(input)} Receive trace or Transmit trace
* @param   rtrIfNum	@b{(input)} router Interface Number
* @param   payLoad	@b{(input)} Protocol packet offset
* @param   length	@b{(input)} Packet length
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void mcastDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
                               L7_uint32 rtrIfNum, L7_uchar8 *payLoad,
                               L7_uint32 length)
{
  L7_uchar8 srcBuff[IPV6_DISP_ADDR_LEN], destBuff[IPV6_DISP_ADDR_LEN];
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_uint32 intIfNum = 0;
  nimUSP_t usp;
  L7_inet_addr_t srcAddr, grpAddr;


  mcastDebugTraceFlagGet(family, MCAST_DEBUG_PACKET_RX_TRACE, &rxFlag);
  mcastDebugTraceFlagGet(family, MCAST_DEBUG_PACKET_TX_TRACE, &txFlag);

  if (((rxTrace == L7_TRUE) && (rxFlag != L7_TRUE)) ||
      ((rxTrace == L7_FALSE) && (txFlag != L7_TRUE)) )
  {
    return;
  }
  if (payLoad == L7_NULLPTR)
  {
    return;
  }
  if ((mcastIpMapRtrIntfToIntIfNum(family, rtrIfNum, &intIfNum) != L7_SUCCESS) ||
      (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS))
  {
    return;
  }

  if (family == L7_AF_INET)
  {
    inetAddressSet(L7_AF_INET, &((L7_ipHeader_t *)payLoad)->iph_src, &srcAddr);
    inetAddressSet(L7_AF_INET, &((L7_ipHeader_t *)payLoad)->iph_dst, &grpAddr);
  }
  else if (family == L7_AF_INET6)
  {
    inetAddressSet(L7_AF_INET6, ((L7_ip6Header_t *)payLoad)->src, &srcAddr);
    inetAddressSet(L7_AF_INET6, ((L7_ip6Header_t *)payLoad)->dst, &grpAddr);
  }

  if (rxTrace == L7_TRUE)
  {
    MCAST_USER_TRACE("Received mcast data pkt with Src %s and Grp %s on interface %d/%d/%d of len = %d",
                     inetAddrPrint(&srcAddr, srcBuff), inetAddrPrint(&grpAddr, destBuff),
                     usp.unit, usp.slot, usp.port, length);
  }
  else
  {
    MCAST_USER_TRACE("Sent mcast data pkt %s with Src %s and Grp %s on interface %d/%d/%d of len = %d ",
                     inetAddrPrint(&srcAddr, srcBuff), inetAddrPrint(&grpAddr, destBuff),
                     usp.unit, usp.slot, usp.port, length);
  }
  return;
}


/*********************************************************************
* @purpose  Get the current status of displaying mcast packet debug info
*            
* @param    familyType   @b{(input)} Address Family type
* @param    traceFlag    @b{(input)} type of trace flag
* @param    flag         @b{(output)} trace flag value
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastDebugTraceFlagGet(L7_uchar8 family,
                              MCAST_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL *flag)
{
  mcastDebugTraceFlags_t *mcastDebugTraceFlags ;

  if (family == L7_AF_INET)
  {
    mcastDebugTraceFlags = &mcastDebugTraceFlag[MCAST_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    mcastDebugTraceFlags = &mcastDebugTraceFlag[MCAST_MAP_IPV6_CB];
  }
  else
  {
    return L7_FAILURE;
  }

  if (((*(mcastDebugTraceFlags[traceFlag/MCAST_DEBUG_TRACE_FLAG_BITS_MAX])) & 
       (MCAST_DEBUG_TRACE_FLAG_VALUE << (traceFlag % MCAST_DEBUG_TRACE_FLAG_BITS_MAX))) != L7_NULL)
  {
    *flag = L7_TRUE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
* @param    familyType    @b{(input)}  Address Family type
* @param    rxFlag        @b{(input)} Receive trace flag
* @param    txFlag        @b{(input)} Transmit trace flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastDebugTraceFlagSet(L7_uchar8 family, 
                              MCAST_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL flag)
{
  mcastDebugTraceFlags_t *mcastDebugTraceFlags = L7_NULLPTR;

  if (family == L7_AF_INET)
  {
    mcastDebugTraceFlags = &mcastDebugTraceFlag[MCAST_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    mcastDebugTraceFlags = &mcastDebugTraceFlag[MCAST_MAP_IPV6_CB];
  }
  else
  {
    return L7_FAILURE;
  }

   if (flag == L7_TRUE)
   {
     (*(mcastDebugTraceFlags[traceFlag/MCAST_DEBUG_TRACE_FLAG_BITS_MAX])) |= 
               (MCAST_DEBUG_TRACE_FLAG_VALUE << (traceFlag % MCAST_DEBUG_TRACE_FLAG_BITS_MAX));
   }
   else
   {
     (*(mcastDebugTraceFlags[traceFlag/MCAST_DEBUG_TRACE_FLAG_BITS_MAX])) &= 
            (~(MCAST_DEBUG_TRACE_FLAG_VALUE << (traceFlag % MCAST_DEBUG_TRACE_FLAG_BITS_MAX)));
   }

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a 
     debug config save.
    */ 

  mcastDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for mcast trace data
*            
* @param    void
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastDebugTraceFlagSave()
{
  memcpy(mcastDebugCfg.cfg.mcastDebugTraceFlag, mcastDebugTraceFlag, 
         sizeof(mcastDebugCfg.cfg.mcastDebugTraceFlag));
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastDebugCfgUpdate(void)
{ 
  mcastDebugTraceFlagSave();
}

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastDebugCfgRead(void)
{ 
      /* reset the debug flags*/
    memset((void*)&mcastDebugCfg, 0 ,sizeof(mcastDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_FLEX_MCAST_MAP_COMPONENT_ID, MCAST_DEBUG_CFG_FILENAME, 
                         (L7_char8 *)&mcastDebugCfg, (L7_uint32)sizeof(mcastDebugCfg_t), 
                         &mcastDebugCfg.checkSum, MCAST_DEBUG_CFG_VER_CURRENT, 
                         mcastDebugBuildDefaultConfigData, L7_NULL);

    mcastDebugCfg.hdr.dataChanged = L7_FALSE;

}

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_MCAST_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = mcastDebugSave;
    supportDebugDescr.userControl.hasDataChanged = mcastDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = mcastDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */

    (void) supportDebugRegister(supportDebugDescr);
}
/*********************************************************************
* @purpose  DeRegister to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastDebugDeRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_MCAST_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = mcastDebugSave;
    supportDebugDescr.userControl.hasDataChanged = mcastDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = mcastDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */
    (void) supportDebugDeregister(supportDebugDescr);
}



/*********************************************************************
* @purpose  Registered support debug dump routine for general multicast issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastMapDebugMulticastInfoDumpRegister(void)
{
    supportDebugCategory_t supportDebugCategory;

    memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));
    supportDebugCategory.componentId = L7_FLEX_MCAST_MAP_COMPONENT_ID;


    /*---------------------------------*/
    /* Register routing pkg debug dump */
    /*---------------------------------*/
    supportDebugCategory.category = SUPPORT_CATEGORY_IPMCAST;
    supportDebugCategory.info.supportRoutine = mcastMapDebugRegisteredMulticastInfoDump;
    osapiStrncpySafe(supportDebugCategory.info.supportRoutineName, "mcastMapDebugRegisteredMulticastInfoDump",
            SUPPORT_DEBUG_HELP_NAME_SIZE);
    (void) supportDebugCategoryRegister(supportDebugCategory);
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general routing issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void mcastMapDebugRegisteredMulticastInfoDump(void)
{
    sysapiPrintf("\r\n");
    sysapiPrintf("/*=====================================================================*/\n");
    sysapiPrintf("/*                  MULTICAST  INFORMATION                               */\n");
    sysapiPrintf("/*=====================================================================*/\n");
    
    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("/* Output of MCAST MessageQueueInfoShow():        */\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("\r\n");

    mcastMapDebugQueueStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("/* Output of MGMD MessageQueueInfoShow():        */\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("\r\n");

    mgmdMapDebugQueueStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("/* Output of DVMRP MessageQueueInfoShow():        */\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("\r\n");

    dvmrpMapDebugQueueStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("/* Output of PIMDM MessageQueueInfoShow():        */\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("\r\n");

    pimdmMapDebugQueueStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("/* Output of PIMSM MessageQueueInfoShow():         */\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("\r\n");

    pimsmMapDebugQueueStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");

}

/*********************************************************************
*
* @purpose  Display available information on a heap allocated buffer
*
* @param    
* @param    
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void mcastMapDebugHeapShow(L7_uchar8 addrFamily)
{
  L7_uint32 heapId;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    sysapiPrintf ("Bad Address Family Specified.\n");
    return;
  }  
  heapId = mcastMapHeapIdGet(addrFamily);
  if (heapId == 0)
  {
    sysapiPrintf("Heap not allocated");
    return;
  }
  heapDbgStatsDisplay(heapId);
}

#ifndef MCAST_STATIC_MROUTE_DEBUG_UT /* Unit Test Code for Static MRoutes */

L7_RC_t
mcastMapDebugStaticMRouteAdd (L7_uchar8 addrFamily,
                              L7_uchar8 srcAddr[MCAST_MAP_MAX_MSG_SIZE],
                              L7_uchar8 srcMask[MCAST_MAP_MAX_MSG_SIZE],
                              L7_uchar8 rpfAddr[MCAST_MAP_MAX_MSG_SIZE],
                              L7_uchar8 usp[MCAST_MAP_MAX_MSG_SIZE],
                              L7_uint32 preference)
{
#if 0
  L7_uint32 srcAddrInt = 0;
  L7_uint32 srcMaskInt = 0;
  L7_uint32 rpfAddrInt = 0;
  L7_inet_addr_t srcAddrInet;
  L7_inet_addr_t srcMaskInet;
  L7_inet_addr_t rpfAddrInet;
  L7_uint32 intIfNum = 0;

  /* srcAddr */
  if (usmDbInetAton(srcAddr, (L7_uint32 *)&srcAddrInt) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid srcAddr.\n");
    return L7_FAILURE;
  }
  inetAddressSet (addrFamily, &srcAddrInt, &srcAddrInet);

  /* srcMask */
  if (usmDbInetAton(srcMask, (L7_uint32 *)&srcMaskInt) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid srcMask.\n");
    return L7_FAILURE;
  }
  inetAddressSet (addrFamily, &srcMaskInt, &srcMaskInet);

  /* rpfAddr */
  if (usmDbInetAton(rpfAddr, (L7_uint32 *)&rpfAddrInt) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid rpfAddr.\n");
    return L7_FAILURE;
  }
  inetAddressSet (addrFamily, &rpfAddrInt, &rpfAddrInet);

  /* USP */
#if 0
  L7_uint32 unit = 0;
  L7_uint32 slot = 0;
  L7_uint32 port = 0;
  if (cliValidSpecificUSPCheck(usp, &unit, &slot, &port) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid USP.\n");
    return L7_FAILURE;
  }
  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid Interface.\n");
    return L7_FAILURE;
  }
#endif
  if (usmDbMcastStaticMRouteAdd (0, addrFamily, &srcAddrInet, &srcMaskInet,
                                 &rpfAddrInet, intIfNum, preference)
                              != L7_SUCCESS)
  {
    sysapiPrintf("Static MRoute Set Failed.\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
#else
  L7_inet_addr_t srcAddrInet;
  L7_inet_addr_t srcMaskInet;
  L7_inet_addr_t rpfAddrInet;
  L7_uint32 intIfNum = 0;

  switch (addrFamily)
  {
    case L7_AF_INET:
    {
      /* srcAddr */
      if (usmDbParseInetAddrFromStr (srcAddr, &srcAddrInet) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcAddr.\n");
        return L7_FAILURE;
      }
    
      /* srcMask */
      if (usmDbParseInetAddrFromStr (srcMask, &srcMaskInet) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcMask.\n");
        return L7_FAILURE;
      }
    }
    break;

    case L7_AF_INET6:
    {
      L7_char8 srcAddrStr[L7_CLI_MAX_STRING_LENGTH];
      L7_uint32 maskLen;
      L7_in6_addr_t srcAddrV6;
      L7_uint32 unit = 0;
      L7_uint32 slot = 0;
      L7_uint32 port = 0;
      extern L7_RC_t cliValidPrefixPrefixLenCheck(const L7_char8 * buf, L7_in6_addr_t * prefix, L7_uint32 * prefixLen);
      extern L7_RC_t cliValidSpecificUSPCheck(const L7_char8 * buf, L7_uint32 * unit, L7_uint32 * slot, L7_uint32 * port);

      /* srcAddr */
      memset (srcAddrStr, 0, sizeof(srcAddrStr));
      OSAPI_STRNCPY_SAFE (srcAddrStr, srcAddr);

      if (cliValidPrefixPrefixLenCheck (srcAddrStr, &srcAddrV6, &maskLen) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcAddr.\n");
        return L7_FAILURE;
      }
      inetAddressSet (addrFamily, &srcAddrV6, &srcAddrInet);
      /* srcMask */
      if (inetMaskLenToMask (addrFamily, maskLen, &srcMaskInet) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcMask.\n");
        return L7_FAILURE;
      }

      /* USP */
      if (cliValidSpecificUSPCheck(usp, &unit, &slot, &port) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid USP.\n");
        return L7_FAILURE;
      }
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid Interface.\n");
        return L7_FAILURE;
      }
    }
    break;

    default:
    {
      sysapiPrintf("Invalid addrFamily.\n");
      return L7_FAILURE;
    }
  }

  /* rpfAddr */
  if (usmDbParseInetAddrFromStr (rpfAddr, &rpfAddrInet) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid rpfAddr.\n");
    return L7_FAILURE;
  }

  /* Invoke the Application APIs */
  if (usmDbMcastStaticMRouteAdd (0, addrFamily, &srcAddrInet, &srcMaskInet,
                                 &rpfAddrInet, intIfNum, preference)
                              != L7_SUCCESS)
  {
    sysapiPrintf("Static MRoute Set Failed.\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
#endif
}

L7_RC_t
mcastMapDebugStaticMRouteDelete (L7_uchar8 addrFamily,
                              L7_uchar8 srcAddr[MCAST_MAP_MAX_MSG_SIZE])
{
  L7_inet_addr_t srcAddrInet;

#if 0
  /* srcAddr */
  if (usmDbInetAton(srcAddr, (L7_uint32 *)&srcAddrInt) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid srcAddr.\n");
    return L7_FAILURE;
  }
#endif 

  if (usmDbParseInetAddrFromStr (srcAddr, &srcAddrInet) != L7_SUCCESS)
  {
    sysapiPrintf("Invalid srcAddr.\n");
    return L7_FAILURE;
  }
  
  if (usmDbMcastStaticMRouteDelete(0, addrFamily, &srcAddrInet, L7_NULLPTR) != L7_SUCCESS)
  {
    sysapiPrintf("Static MRoute Reset Failed.\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

void
mcastMapDebugStaticMRouteShow (L7_uchar8 addrFamily)
{
  L7_uchar8 addr[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_int32  mcastStaticMRouteNumEntries = 0;
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry = L7_NULLPTR;
  L7_uint32 tableIndex = 0;
  L7_uint32 intIfNum;

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG_PRINTF ("Invalid Address Family Identifier - %d", addrFamily);
    return;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG_PRINTF ("cfgData Get Failed for addrFamily - %d", addrFamily);
    return;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG_PRINTF ("cfgData is NULL for addrFamily - %d", addrFamily);
    return;
  }

  if ((mcastStaticMRouteNumEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG_PRINTF ("Static MRoute Table is Empty for Address Family - %d", addrFamily);
    return;
  }

  MCAST_MAP_DEBUG_PRINTF ("\n __________ MCAST STATIC MROUTE TABLE ______________\n");
  MCAST_MAP_DEBUG_PRINTF ("\nEntry Count   : %d\n", mcastStaticMRouteNumEntries);

  for (tableIndex = 0; tableIndex < mcastStaticMRouteNumEntries; tableIndex++)
  {
    staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex];

    MCAST_MAP_DEBUG_PRINTF ("\n----- Index - %d -----\n", tableIndex+1);
    MCAST_MAP_DEBUG_PRINTF ("Source Address: %s\n", inetAddrPrint(&staticMRouteEntry->source,addr));
    MCAST_MAP_DEBUG_PRINTF ("Source Mask   : %s\n", inetAddrPrint(&staticMRouteEntry->mask,addr));
    MCAST_MAP_DEBUG_PRINTF ("RPF    Address: %s\n", inetAddrPrint(&staticMRouteEntry->rpfAddr,addr));

    if (addrFamily == L7_AF_INET6)
    {
      if (L7_IP6_IS_ADDR_LINK_LOCAL (&staticMRouteEntry->rpfAddr) != 0) 
      {
        intIfNum = 0;
        if (nimIntIfFromConfigIDGet (&staticMRouteEntry->ifConfigId, &intIfNum)
                                  == L7_SUCCESS)
        {
          MCAST_MAP_DEBUG_PRINTF ("RPF intIfNum  : %d\n", intIfNum);
        }
      }
    }

    MCAST_MAP_DEBUG_PRINTF ("Preference    : %d\n", staticMRouteEntry->preference);
    MCAST_MAP_DEBUG_PRINTF ("\n");
  }

  return;
}

void
mcastMapDebugStaticMRouteDisplay (L7_uchar8 addrFamily)
{
  L7_inet_addr_t srcAddrInet;
  L7_inet_addr_t srcMaskInet;
  L7_inet_addr_t rpfAddrInet;
  L7_uint32 preference;  
  L7_uint32 intIfNum;
  L7_int32  mcastStaticMRouteNumEntries = 0;  
  L7_uchar8 addr[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 srcStr[20] = "Source Address";
  L7_uchar8 mskStr[20] = "Source Mask";
  L7_uchar8 rpfStr[20] = "RPF Address";
  L7_uchar8 intStr[10] = "Interface";
  L7_uchar8 prfStr[11] = "Preference";

  mcastMapStaticMRouteEntryCountGet (addrFamily, &mcastStaticMRouteNumEntries);

  MCAST_MAP_DEBUG_PRINTF ("\n __________ MCAST STATIC MROUTE TABLE ______________\n");
  MCAST_MAP_DEBUG_PRINTF ("\nEntry Count : %d", mcastStaticMRouteNumEntries);
  MCAST_MAP_DEBUG_PRINTF ("\n");

  if (mcastStaticMRouteNumEntries <= 0)
  {
    return;
  }

  MCAST_MAP_DEBUG_PRINTF ("\n-------------------- -------------------- -------------------- --------- ----------\n");
  MCAST_MAP_DEBUG_PRINTF ("%-20s ", srcStr);
  MCAST_MAP_DEBUG_PRINTF ("%-20s ", mskStr);
  MCAST_MAP_DEBUG_PRINTF ("%-20s ", rpfStr);
  MCAST_MAP_DEBUG_PRINTF ("%-9s ", intStr);
  MCAST_MAP_DEBUG_PRINTF ("%-10s ", prfStr);
  MCAST_MAP_DEBUG_PRINTF ("\n-------------------- -------------------- -------------------- --------- ----------\n");

  inetAddressZeroSet (addrFamily, &srcAddrInet);
  while (usmDbMcastStaticMRouteEntryNextGet (0, addrFamily, &srcAddrInet, &srcMaskInet) == L7_SUCCESS)
  {
    MCAST_MAP_DEBUG_PRINTF ("%-20s ", inetAddrPrint(&srcAddrInet,addr));
    MCAST_MAP_DEBUG_PRINTF ("%-20s ", inetAddrPrint(&srcMaskInet,addr));
    if (usmDbMcastStaticMRouteRpfAddressGet (0, addrFamily, &srcAddrInet, &srcMaskInet, &rpfAddrInet) == L7_SUCCESS)
    {
      MCAST_MAP_DEBUG_PRINTF ("%-20s ", inetAddrPrint(&rpfAddrInet, addr));
    }
    if (usmDbMcastStaticMRouteInterfaceGet (0, addrFamily, &srcAddrInet, &srcMaskInet, &intIfNum) == L7_SUCCESS)
    {
      MCAST_MAP_DEBUG_PRINTF ("%-9d ", intIfNum);
    }
    if (usmDbMcastStaticMRoutePreferenceGet (0, addrFamily, &srcAddrInet, &srcMaskInet, &preference) == L7_SUCCESS)
    {
      MCAST_MAP_DEBUG_PRINTF ("%-10d", preference);
    }
    MCAST_MAP_DEBUG_PRINTF ("\n");
  } /* end of while */
}

#endif /* MCAST_STATIC_MROUTE_DEBUG_UT */

/*********************************************************************
* @purpose  Compute the memory allocated by the MCAST Mapping layer
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void mcastMapDebugMemoryInfoCompute(void)
{
  L7_uint32 mcastV4HeapSize = MCAST_V4_HEAP_SIZE;
#ifdef L7_IPV6_PACKAGE
  L7_uint32 mcastV6HeapSize = MCAST_V6_HEAP_SIZE;
#endif

  sysapiPrintf ("\n***** Memory Allocated By MCAST Mapping Layer *****\n");
  sysapiPrintf ("        Global Variables\n");
  sysapiPrintf ("          mcastGblVar_t             - %d\n", sizeof(mcastGblVar_t));
  mcastMapTotalBytesAllocated += sizeof(mcastGblVar_t);
  sysapiPrintf ("          mcastDebugCfg_t           - %d\n", sizeof(mcastDebugCfg_t));
  mcastMapTotalBytesAllocated += sizeof(mcastDebugCfg_t);
  sysapiPrintf ("        Message Queues\n");
  sysapiPrintf ("          App Timer Queue           - Size[%d]*Num[%d] = %d\n", MCASTMAP_APP_TMR_MSG_SIZE, MCASTMAP_APP_TMR_MSG_COUNT, (MCASTMAP_APP_TMR_MSG_SIZE*MCASTMAP_APP_TMR_MSG_COUNT));
  mcastMapTotalBytesAllocated += (MCASTMAP_APP_TMR_MSG_SIZE*MCASTMAP_APP_TMR_MSG_COUNT);
  sysapiPrintf ("          Events Queue              - Size[%d]*Num[%d] = %d\n", MCASTMAP_MSG_SIZE, MCASTMAP_MSG_COUNT, (MCASTMAP_MSG_SIZE*MCASTMAP_MSG_COUNT));
  mcastMapTotalBytesAllocated += (MCASTMAP_MSG_SIZE*MCASTMAP_MSG_COUNT);
  sysapiPrintf ("          Ctrl Pkt Queue            - Size[%d]*Num[%d] = %d\n", MCASTMAP_CTRL_PKT_MSG_SIZE, MCASTMAP_CTRL_PKT_MSG_COUNT, (MCASTMAP_CTRL_PKT_MSG_SIZE*MCASTMAP_CTRL_PKT_MSG_COUNT));
  mcastMapTotalBytesAllocated += (MCASTMAP_CTRL_PKT_MSG_SIZE*MCASTMAP_CTRL_PKT_MSG_COUNT);
  sysapiPrintf ("        Timers\n");
  sysapiPrintf ("          Handle List Size IPv4     - %d\n", sizeof(handle_member_t) * MCAST_MAX_TIMERS);
  mcastMapTotalBytesAllocated += sizeof(handle_member_t) * MCAST_MAX_TIMERS;
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf ("          Handle List Size IPv6     - %d\n", sizeof(handle_member_t) * MCAST_MAX_TIMERS);
  mcastMapTotalBytesAllocated += sizeof(handle_member_t) * MCAST_MAX_TIMERS;
#endif
  sysapiPrintf ("          AppTimer Buffer Size      - %d\n", MCAST_MAX_TIMERS * L7_APP_TMR_NODE_SIZE);
  mcastMapTotalBytesAllocated += MCAST_MAX_TIMERS * L7_APP_TMR_NODE_SIZE;
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf ("        Packet Receiver Buffer Size - %d\n", sizeof(L7_PktRcvr_t) + (L7_IPV6_PKT_RCVR_ID_MAX * sizeof(L7_fdList_t)));
  mcastMapTotalBytesAllocated += sizeof(L7_PktRcvr_t) + (L7_IPV6_PKT_RCVR_ID_MAX * sizeof(L7_fdList_t));
#endif
  sysapiPrintf ("        MCAST Heap Size for IPv4    - %d\n", mcastV4HeapSize);
  mcastMapTotalBytesAllocated += MCAST_V4_HEAP_SIZE;
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf ("        MCAST Heap Size for IPv6    - %d\n", mcastV6HeapSize);
  mcastMapTotalBytesAllocated += MCAST_V6_HEAP_SIZE;
#endif
  sysapiPrintf ("        Packet Buffers\n");
  sysapiPrintf ("          Ctrl Pkt Buffers IPv4     - %d\n", MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU);
  mcastMapTotalBytesAllocated += MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU;
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf ("          Ctrl Pkt Buffers IPv6     - %d\n", MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU);
  mcastMapTotalBytesAllocated += MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU;
  sysapiPrintf ("          Data Pkt Buffers IPv6     - %d\n", MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU);
  mcastMapTotalBytesAllocated += MCAST_PKT_BUF_COUNT * L7_MULTICAST_MAX_IP_MTU;
#endif
  sysapiPrintf ("        MGMD Events Buffers IPv4    - %d\n", (MCASTMAP_MGMD_EVENTS_COUNT_LIMIT * MGMD_MAX_QUERY_SOURCES) * sizeof (mrp_source_record_t));
  mcastMapTotalBytesAllocated += (MCASTMAP_MGMD_EVENTS_COUNT_LIMIT * MGMD_MAX_QUERY_SOURCES) * sizeof (mrp_source_record_t);
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf ("        MGMD Events Buffers IPv6    - %d\n", (MCASTMAP_MGMD_EVENTS_COUNT_LIMIT * MGMD_MAX_QUERY_SOURCES) * sizeof (mrp_source_record_t));
  mcastMapTotalBytesAllocated += (MCASTMAP_MGMD_EVENTS_COUNT_LIMIT * MGMD_MAX_QUERY_SOURCES) * sizeof (mrp_source_record_t);
#endif
  sysapiPrintf ("\n");
  sysapiPrintf ("        Total Memory                - %d\n", mcastMapTotalBytesAllocated);
  sysapiPrintf ("\n");
  sysapiPrintf ("***************************************************\n");

  return;
}

/*********************************************************************
* @purpose  Compute the memory allocated by the MCAST Component
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void mcastDebugMemoryInfoShow(void)
{
  extern void mfcDebugMemoryInfoCompute(void);
  extern void dvmrpMapDebugMemoryInfoCompute(void);
  extern void mgmdMapDebugMemoryInfoCompute(void);
  extern void pimdmMapDebugMemoryInfoCompute(void);
  extern void pimsmMapDebugMemoryInfoCompute(void);

  extern L7_uint32 mfcTotalBytesAllocated;
  extern L7_uint32 mgmdMapTotalBytesAllocated;
  extern L7_uint32 dvmrpMapTotalBytesAllocated;
  extern L7_uint32 pimdmMapTotalBytesAllocated;
  extern L7_uint32 pimsmMapTotalBytesAllocated;

  mcastMapTotalBytesAllocated = 0;
  mfcTotalBytesAllocated = 0;
  mgmdMapTotalBytesAllocated = 0;
  dvmrpMapTotalBytesAllocated = 0;
  pimdmMapTotalBytesAllocated = 0;
  pimsmMapTotalBytesAllocated = 0;

  mcastMapDebugMemoryInfoCompute();
  mfcDebugMemoryInfoCompute();
  dvmrpMapDebugMemoryInfoCompute();
  mgmdMapDebugMemoryInfoCompute();
  pimdmMapDebugMemoryInfoCompute();
  pimsmMapDebugMemoryInfoCompute();

  mcastTotalBytesAllocated = 0;

  sysapiPrintf ("\n\n***** Memory Allocated By MCAST Component Summary *****\n");
  sysapiPrintf ("\n");
  sysapiPrintf ("        MCAST Mapping Layer      - %d\n", mcastMapTotalBytesAllocated);
  sysapiPrintf ("        MFC Component            - %d\n", mfcTotalBytesAllocated);
  sysapiPrintf ("        DVMRP Mapping Layer      - %d\n", dvmrpMapTotalBytesAllocated);
  sysapiPrintf ("        MGMD  Mapping Layer      - %d\n", mgmdMapTotalBytesAllocated);
  sysapiPrintf ("        PIMDM Mapping Layer      - %d\n", pimdmMapTotalBytesAllocated);
  sysapiPrintf ("        PIMSM Mapping Layer      - %d\n", pimsmMapTotalBytesAllocated);
  sysapiPrintf ("\n");
  mcastTotalBytesAllocated = mcastMapTotalBytesAllocated + mfcTotalBytesAllocated + mgmdMapTotalBytesAllocated + dvmrpMapTotalBytesAllocated + pimdmMapTotalBytesAllocated + pimsmMapTotalBytesAllocated;
  sysapiPrintf ("        Total Memory (in Bytes)  - %d\n", mcastTotalBytesAllocated);
  sysapiPrintf ("        Total Memory (in MB)     - %d\n", ((mcastTotalBytesAllocated/1024)/1024));
  sysapiPrintf ("\n");
  sysapiPrintf ("***********************************************************\n");

  return;
}

