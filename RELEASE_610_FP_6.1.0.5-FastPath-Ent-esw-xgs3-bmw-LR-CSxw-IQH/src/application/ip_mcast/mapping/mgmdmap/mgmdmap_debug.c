/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_debug.c
*
* @purpose   MGMD Utility Functions
*
* @component MGMD Mapping Layer
*
* @comments  none
*
* @create    02/15/2002
*
* @author    gkiran
* @end
*
**********************************************************************/

#include "support_api.h"
#include "nimapi.h"
#include "mcast_defs.h"
#include "mcast_wrap.h"
#include "mgmd.h"
#include "l7_mgmdmap_include.h"

/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[MGMD_MAP_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

L7_uint32 mgmdMapTotalBytesAllocated = 0;

/* MGMD Packet debug traces */
static mgmdDebugTraceFlags_t mgmdDebugTraceFlag[MGMD_MAP_CB_MAX];
extern mgmdDebugCfg_t mgmdDebugCfg;

/*****************************************************************
    Function Prototypes
******************************************************************/
static L7_RC_t mgmdIntfBuildTestConfigData(mgmdIntfCfgData_t *pCfg);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MGMDMAP.
*
* @param    void
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MGMDMAP.
*
* @param    void
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MGMDMAP.
*
* @param    void
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugAllSet(void)
{
  memset(debugFlags, MGMD_MAP_DEBUG_ALL_SET, sizeof(debugFlags));
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MGMDMAP.
*
* @param    void
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
*           L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugAllReset(void)
{
  memset(debugFlags, L7_NULL, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MGMDMAP.
*
* @param    flag      @b{(input)}  Debug Flag to set.
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
*           L7_FAILURE   if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugFlagSet(MGMD_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= MGMD_MAP_DEBUG_FLAG_LAST)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid Flag\n");
    return L7_FAILURE;
  }
  debugFlags[flag/MGMD_MAP_FLAG_BITS_MAX] |= 
             (MGMD_MAP_FLAG_VALUE << (flag % MGMD_MAP_FLAG_BITS_MAX));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MGMDMAP.
*
* @param    flag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
*           L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDebugFlagReset(MGMD_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= MGMD_MAP_DEBUG_FLAG_LAST)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid Flag\n");
    return L7_FAILURE;
  }
  debugFlags[flag/MGMD_MAP_FLAG_BITS_MAX] &= 
            (~(MGMD_MAP_FLAG_VALUE << (flag % MGMD_MAP_FLAG_BITS_MAX)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MGMDMAP.
*
* @param    debugType  @b{(input)}  Debug Flag to check.
*
* @returns  L7_TRUE     if the Debug trace flag is turned ON.
*           L7_FALSE    if the Debug trace flag is turned OFF.
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL mgmdMapDebugFlagCheck(MGMD_MAP_DEBUG_FLAGS_t debugType)
{
  if(debugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  }
  if(debugType >= MGMD_MAP_DEBUG_FLAG_LAST)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid debug type\n");
    return L7_FALSE;
  }
  if((debugFlags[debugType/MGMD_MAP_FLAG_BITS_MAX] & 
        (MGMD_MAP_FLAG_VALUE << (debugType % MGMD_MAP_FLAG_BITS_MAX))) != 0)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current MGMDMAP Debug flag status.
*
* @param    void
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mgmdMapDebugFlagShow(void)
{
  L7_uint32 index;

  if(debugEnabled == L7_TRUE)
  {
    sysapiPrintf(" MGMDMAP Debugging : Enabled\n");
    for(index = 0;  index < MGMD_MAP_DEBUG_FLAG_LAST ; index ++)
    {
      if(mgmdMapDebugFlagCheck(index) == L7_TRUE)
      {
        sysapiPrintf("     debugFlag [%d] : %s\n", index , "Enabled");
      }
      else
      {
        sysapiPrintf("     debugFlag [%d] : %s\n", index , "Disabled");
      }
    }
  }
  else
  {
    sysapiPrintf (" MGMDMAP Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the MGMDMAP Debug utility
*
* @param    void
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mgmdMapDebugHelp(void)
{
  sysapiPrintf(" Use mgmdMapDebugEnable()/mgmdMapDebugDisable()"
                 "to Enable/Disable Debug trace in MGMDMAP\n");
  sysapiPrintf(" Use mgmdMapDebugFlagSet(flag)/mgmdMapDebugFlagReset(flag)"
               " to Enable/Disable specific functionality traces\n");
  sysapiPrintf(" Use mgmdMapDebugFlagAllSet()/mgmdMapDebugFlagAllReset()"
               " to Enable/Disable all traces\n");
  sysapiPrintf("     Various Debug Trace flags and their definitions"  
                                                  "are as follows ;\n");
  sysapiPrintf("         0  -> Trace the complete Receive Data path\n");
  sysapiPrintf("         1  -> Trace the complete Transmission Data path\n");
  sysapiPrintf("         2  -> Trace all the EVENT generations"
                                                       "and receptions\n");
  sysapiPrintf("         3  -> Trace all Timer activities\n");
  sysapiPrintf("         4  -> Trace all failures\n");
  sysapiPrintf("         5  -> Trace all the APIs invoked\n");
  sysapiPrintf("mgmdMapCfgDataShow(family = 1/2)\n");  
  sysapiPrintf("mgmdMapDebugQueueStatsShow()\n");    
  sysapiPrintf("mgmdMapDebugQueueStatsClear()\n");    
  sysapiPrintf("mgmdMapDebugControlBlockShow(family = 1/2)\n");    
}

/*********************************************************************
* @purpose  Dumps the mgmd configuration information onto the display
*           screen
*
* @param    familyType          @b{(input)} Address Family type
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmdMapCfgDataShow(L7_uchar8 familyType)
{
  L7_uint32 index;
  nimUSP_t usp;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
    "Failed in getting Control Block.\n");
    return;
  }
  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n|              MGMD CONFIGURATION DATA                    |");
  sysapiPrintf("\n+--------------------------------------------------------+");

  sysapiPrintf("\n\nHeader Information:");
  sysapiPrintf( "\nfilename...............................%s", 
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.filename);
  sysapiPrintf( "\nversion................................%d", 
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.version);
  sysapiPrintf( "\ncomponentID............................%d", 
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.componentID);
  sysapiPrintf( "\ntype...................................%d", 
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.type);
  sysapiPrintf( "\nlength.................................%d", 
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.length);

  sysapiPrintf( "\n\nMGMD specific Information:");
  if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_ENABLE)
  {
    sysapiPrintf( "\nmgmdAdminMode......................enable");
  } 
  else
  {
    sysapiPrintf( "\nmgmdAdminMode......................disable");
  }
  if (mgmdMapCbPtr->pMgmdMapCfgData->checkRtrAlert == L7_TRUE)
  {
    sysapiPrintf( "\nmgmdRouterAlertCheck......................enable");
  } 
  else
  {
    sysapiPrintf( "\nmgmdRouterAlertCheck......................disable");
  }


  sysapiPrintf("\n-------------------------------------------------------------"
               "-------------------------------------------------------");
  sysapiPrintf("\nintIf-  admin    query     version   response  robustness"
               "    startup      lastmember     lastmember  ");
  sysapiPrintf("\n num     mode     interval            interval\
              query interval query count  query interval  query count ");
  sysapiPrintf("\n-------------------------------------------------------------"
               "-------------------------------------------------------");


  for (index = 1; index < L7_MAX_INTERFACE_COUNT; index++)
  {
    if (mgmdMapCbPtr->pMgmdMapCfgMapTbl[index] != 0)
    {
        
        if (mgmdIntfIsValid(index)== L7_FALSE)
        {
           continue; 
        }

      if ((mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->
           pMgmdMapCfgMapTbl[index]].adminMode == L7_ENABLE))
      {
        (void)nimGetUnitSlotPort(index, &usp);
        sysapiPrintf(
                      "\n%d/%d/%d",usp.unit, usp.slot, usp.port);
        if (mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->
            pMgmdMapCfgMapTbl[index]].adminMode == L7_ENABLE)
        {
          sysapiPrintf("\t  E");
        } else
        {
          sysapiPrintf("\t  D");
        }
        sysapiPrintf(
        "     %d", mgmdMapCbPtr->pMgmdMapCfgData->
        mgmdIntf[mgmdMapCbPtr->pMgmdMapCfgMapTbl[index]].queryInterval);
        sysapiPrintf(
        "     %d", mgmdMapCbPtr->pMgmdMapCfgData->
        mgmdIntf[mgmdMapCbPtr->pMgmdMapCfgMapTbl[index]].version);
        sysapiPrintf(
        "     %d", mgmdMapCbPtr->pMgmdMapCfgData->
        mgmdIntf[mgmdMapCbPtr->pMgmdMapCfgMapTbl[index]].responseInterval);
        sysapiPrintf(
        "     %d", mgmdMapCbPtr->pMgmdMapCfgData->
        mgmdIntf[mgmdMapCbPtr->pMgmdMapCfgMapTbl[index]].robustness);
        sysapiPrintf(
        "     %d", 
        mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->
        pMgmdMapCfgMapTbl[index]].startupQueryInterval);
        sysapiPrintf(
        "     %d", 
        mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->
        pMgmdMapCfgMapTbl[index]].startupQueryCount);
        sysapiPrintf(
        "      %d", 
        mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->
        pMgmdMapCfgMapTbl[index]].lastMemQueryInterval);
        sysapiPrintf(
        "      %d", 
        mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->
        pMgmdMapCfgMapTbl[index]].lastMemQueryCount);
      }
    }
  }

  sysapiPrintf(
  "\n-----------------------------------------------------------------------");

  sysapiPrintf("\n");
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    familyType          @b{(input)} Address Family type
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void mgmdMapBuildTestConfigData(L7_uchar8 familyType)
{
  L7_uint32 index = L7_NULL;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    return;
  }

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  if (mgmdMapCbPtr->pMgmdMapCfgData != L7_NULL)
  {
    /*---------------------------*/
    /* Build Default Config Data */
    /*---------------------------*/
    mgmdMapCbPtr->pMgmdMapCfgData->adminMode                =   L7_ENABLE;

    /* we are preserving any previous configId fields in the table */
    for (index=1; index < L7_IPMAP_INTF_MAX_COUNT; index++)
    {
      if (mgmdIntfBuildTestConfigData(&(mgmdMapCbPtr->pMgmdMapCfgData->
          mgmdIntf[index])) != L7_SUCCESS)
      {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error building MGMD test config record for interface\n");
      }
    }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;  
  }
   sysapiPrintf("Built test config data\n");


}
/*********************************************************************
*
* @purpose  Shows the DVMRPMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void mgmdMapDebugQueueStatsShow(void)
{
  L7_uint32 qIndex;
  L7_uchar8 qName[20];

  sysapiPrintf(" QueueName  CurMsgCnt HighWaterMark  SendFailCnt RxSuccessCnt MaxQSize MsgSize \n");
  sysapiPrintf ("\n----------------------------------------------------------------------------\n");
  for (qIndex = MGMD_APP_TIMER_Q; qIndex < MGMD_MAX_Q; qIndex++ )
  {
    mgmdQueue_t   *mgmdMapQueue = &mgmdMapGblVariables_g.mgmdQueue[qIndex];
    osapiStrncpy(qName, (mgmdMapQueue->QName)+ 7, 20);
    sysapiPrintf("\n %-15s  %-10d %-10d %-10d %-10d %-7d %-7d  ",
                 qName, mgmdMapQueue->QCurrentMsgCnt, mgmdMapQueue->QMaxRx,
                 mgmdMapQueue->QSendFailedCnt, mgmdMapQueue->QRxSuccess,
                 mgmdMapQueue->QCount , mgmdMapQueue->QSize);
        
  }
  sysapiPrintf("\n");
}
/*********************************************************************
*
* @purpose  Clears the DVMRPMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void mgmdMapDebugQueueStatsClear(void)
{
 L7_uint32 QIndex=0;

 for(QIndex=0;QIndex <MGMD_MAX_Q; QIndex++)
 {
   mgmdMapGblVariables_g.mgmdQueue[QIndex].QCurrentMsgCnt = 0;
   mgmdMapGblVariables_g.mgmdQueue[QIndex].QMaxRx = 0;
   mgmdMapGblVariables_g.mgmdQueue[QIndex].QSendFailedCnt = 0;
   mgmdMapGblVariables_g.mgmdQueue[QIndex].QRxSuccess = 0;
 }
}


/*********************************************************************
* @purpose  Build test config data for an intf 
*
* @param    pCfg    @b{(output)} pointer to interface configuration structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfBuildTestConfigData(mgmdIntfCfgData_t *pCfg)
{

    L7_uint32 intIfNum = L7_NULL;
    L7_RC_t rc = L7_FAILURE;

    /* Ensure at least one entry in each array has a unique value to validate
     * accurate migration 
     */
    
    if (nimIntIfFromConfigIDGet(&pCfg->configId, &intIfNum) != L7_SUCCESS)
    { 
      intIfNum = 0xFF;
    }
   

    if (pCfg != L7_NULLPTR)
    {
      pCfg->adminMode            = L7_ENABLE;
      pCfg->queryInterval        = 5 + intIfNum;
      pCfg->version              = 1;
      pCfg->responseInterval     = 300;
      pCfg->robustness           = 10;
      pCfg->startupQueryInterval = 64;
      pCfg->startupQueryCount    = 15;
      pCfg->lastMemQueryInterval = 25;
      pCfg->lastMemQueryCount    = 30 +intIfNum;
      pCfg->entryStatus          = L7_ENTRY_INUSE;
      rc = L7_SUCCESS;
    }
    return(rc); 
}




/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    familyType          @b{(input)} Address Family type
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void mgmdMapConfigDataTestShow(L7_uchar8 familyType)
{

    L7_fileHdr_t  *pFileHdr = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      return;
    }

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    mgmdMapCfgDataShow(familyType);


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_IPMAP_INTF_MAX_COUNT - %d\n", L7_IPMAP_INTF_MAX_COUNT);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("pMgmdMapCfgData->checkSum : %u\n", 
                  mgmdMapCbPtr->pMgmdMapCfgData->checkSum);


}

/******************************************************************************
* @purpose Display the Control Block of MGMD Mapping Layer 
*
* @param        none
*
* @returns void
*
* @comments
*
* @end
******************************************************************************/
void mgmdMapDebugCBShow(L7_uchar8 familyType)
{
   mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

   if ((familyType != L7_AF_INET) && (familyType != L7_AF_INET6))
   {
     sysapiPrintf ("Invalid Family Type specified - %d.\n", familyType);
     return;
   }

   if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     sysapiPrintf ("Failed to get PIM-SM MAP Control Block.\n");
     return;
   }

   sysapiPrintf ("**********************************************\n");
   sysapiPrintf ("  Family Type       - %d.\n", mgmdMapCbPtr->familyType);

   sysapiPrintf (" MGMD  cbHandle     - %p.\n", mgmdMapCbPtr->cbHandle);
   sysapiPrintf (" PROXY cbHandle     - %p.\n", mgmdMapCbPtr->proxyCbHandle);
   sysapiPrintf ("  pMgmdMapCfgData   - %p.\n", mgmdMapCbPtr->pMgmdMapCfgData);
   sysapiPrintf ("  pMgmdMapCfgMapTbl - %p.\n", mgmdMapCbPtr->pMgmdMapCfgMapTbl);
   sysapiPrintf ("  pMgmdInfo         - %p.\n", mgmdMapCbPtr->pMgmdInfo);

   sysapiPrintf ("\n  gblVars      - %p.\n", mgmdMapCbPtr->gblVars);
   sysapiPrintf ("mgmdMapMsgQueue  - %p.\n", mgmdMapCbPtr->gblVars->mgmdQueue[MGMD_EVENT_Q].QPointer);
   sysapiPrintf ("mgmdMapPktQueue  - %p.\n", mgmdMapCbPtr->gblVars->mgmdQueue[MGMD_CTRL_PKT_Q].QPointer);
   sysapiPrintf ("mgmdAppTimerQueue- %p.\n", mgmdMapCbPtr->gblVars->mgmdQueue[MGMD_APP_TIMER_Q].QPointer);   
   sysapiPrintf ("mgmdMapMsgQSema  - %p.\n", mgmdMapCbPtr->gblVars->mgmdMapMsgQSema);
   sysapiPrintf ("mgmdMapTaskId    - %d.\n", mgmdMapCbPtr->gblVars->mgmdMapTaskId);
   sysapiPrintf ("mgmdCnfgrState   - %d.\n", mgmdMapCbPtr->gblVars->mgmdCnfgrState);
   sysapiPrintf ("**********************************************\n");
   return;
}

/*============================================================================*/
/*========================  END OF MGMD CONFIG DEBUG =========================*/
/*============================================================================*/

/*============================================================================*/
/*===================  START OF MGMD PACKET DEBUG TRACES =====================*/
/*============================================================================*/



/*********************************************************************
*
* @purpose Trace mgmd packets received
*
* @param   family	@b{(input)} Address family
* @param   rxTrace	@b{(input)} Receive trace or Transmit trace
* @param   rtrIfNum	@b{(input)} router Interface Number
* @param   src  	@b{(input)} IP Source address
* @param   dest 	@b{(input)} IP Destination address
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
void mgmdDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
                              L7_uint32 rtrIfNum, L7_inet_addr_t *src,
                              L7_inet_addr_t *dest, L7_uchar8 *payLoad, 
                              L7_uint32 length)
{
  L7_uchar8 srcBuff[IPV6_DISP_ADDR_LEN], destBuff[IPV6_DISP_ADDR_LEN];
  L7_uchar8 str[MGMD_PKT_TYPE_STR_LEN], pktType, maxRespTime = L7_NULL;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_uint32 intIfNum = 0;
  nimUSP_t usp;
  L7_uchar8 code, version = 0;
  L7_ushort16 chksum;
  MGMD_CTRL_PKT_TYPE_t ctrlPktType = MGMD_CTRL_PKT_MAX;
  MGMD_STATS_TYPE_t    statsType;  

  if (payLoad == L7_NULLPTR)
  {
    return;
  }
  MCAST_GET_BYTE(pktType, payLoad);
  if (family == L7_AF_INET)
  {
    switch (pktType)
    {
    case IGMP_MEMBERSHIP_QUERY:
      MCAST_GET_BYTE(maxRespTime, payLoad);
      osapiStrncpySafe(str, "IGMP Query", MGMD_PKT_TYPE_STR_LEN);
      if (length == IGMP_PKT_MIN_LENGTH)
      {
        if (maxRespTime == L7_NULL)
        {
          version = L7_MGMD_VERSION_1;
          ctrlPktType = MGMD_QUERY_V1;
        }
        else
        {
          version = L7_MGMD_VERSION_2;
          ctrlPktType = MGMD_QUERY_V2;
        }
      }
      else if (length > IGMP_PKT_MIN_LENGTH)
      {
        version = L7_MGMD_VERSION_3;
        ctrlPktType = MGMD_QUERY_V3;
      }
      break;
    case IGMP_V1_MEMBERSHIP_REPORT:
      version = L7_MGMD_VERSION_1;
      ctrlPktType = MGMD_REPORT_V1;
      osapiStrncpySafe(str, "IGMP Report", MGMD_PKT_TYPE_STR_LEN);
      break;
    case IGMP_V2_MEMBERSHIP_REPORT:
      version = L7_MGMD_VERSION_2;
      ctrlPktType = MGMD_REPORT_V2;
      osapiStrncpySafe(str, "IGMP Report", MGMD_PKT_TYPE_STR_LEN);
      break;
    case IGMP_V3_MEMBERSHIP_REPORT:
      version = L7_MGMD_VERSION_3;
      ctrlPktType = MGMD_REPORT_V3;
      osapiStrncpySafe(str, "IGMP Report", MGMD_PKT_TYPE_STR_LEN);
      break;
    case IGMP_V2_LEAVE_GROUP:
      version = L7_MGMD_VERSION_2;
      ctrlPktType = MGMD_LEAVE_V2;
      osapiStrncpySafe(str, "IGMP Leave", MGMD_PKT_TYPE_STR_LEN);
      break;
    default:
      osapiStrncpySafe(str, "Unknown IGMP pktType", MGMD_PKT_TYPE_STR_LEN);
    }
  }
  else if (family == L7_AF_INET6)
  {
    switch (pktType)
    {
    case MLD_LISTENER_QUERY:
      memcpy(str, "MLD Query", MGMD_PKT_TYPE_STR_LEN);
      MCAST_GET_BYTE(code, payLoad);
      MCAST_GET_SHORT(chksum, payLoad);
      MCAST_GET_BYTE(maxRespTime, payLoad);
      if (length >= MLD_V2_PKT_MIN_LENGTH)
      {
        version = L7_MGMD_VERSION_2;
        ctrlPktType = MGMD_QUERY_V3;
      }
      else if (length == MLD_PKT_MIN_LENGTH)
      {
        version = L7_MGMD_VERSION_1;
        ctrlPktType = MGMD_QUERY_V2;
      }
      break;
    case MLD_V1_LISTENER_REPORT:
      version = L7_MGMD_VERSION_1;
      ctrlPktType = MGMD_REPORT_V2;
      osapiStrncpySafe(str, "MLD Report", MGMD_PKT_TYPE_STR_LEN);
      break;
    case MLD_V2_LISTENER_REPORT:
      version = L7_MGMD_VERSION_2;
      ctrlPktType = MGMD_REPORT_V3;
      osapiStrncpySafe(str, "MLD Report", MGMD_PKT_TYPE_STR_LEN);
      break;
    case MLD_LISTENER_DONE:
      version = L7_MGMD_VERSION_1;
      ctrlPktType = MGMD_LEAVE_V2;
      osapiStrncpySafe(str, "MLD Leave", MGMD_PKT_TYPE_STR_LEN);
      break;
    default:
      osapiStrncpySafe(str, "Unknown MLD pktType", MGMD_PKT_TYPE_STR_LEN);
    }
  }

  statsType =  (rxTrace == L7_TRUE) ? MGMD_STATS_RX : MGMD_STATS_TX;
  mgmdIntfStatsUpdate(family, rtrIfNum, ctrlPktType, statsType);

  mgmdDebugTraceFlagGet(family, MGMD_DEBUG_PACKET_RX_TRACE, &rxFlag);
  mgmdDebugTraceFlagGet(family, MGMD_DEBUG_PACKET_TX_TRACE, &txFlag);

  if (((rxTrace == L7_TRUE) && (rxFlag != L7_TRUE)) ||
      ((rxTrace == L7_FALSE) && (txFlag != L7_TRUE)) )
  {
    return;
  }
  if ((mcastIpMapRtrIntfToIntIfNum(family, rtrIfNum, &intIfNum) != L7_SUCCESS) ||
      (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS))
  {
    return;
  }

  if (rxTrace == L7_TRUE)
  {
    MGMD_USER_TRACE("Received Version %d %s with Src %s and Destn %s on interface %d/%d/%d of pktlen = %d",
                    version, str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                    usp.unit, usp.slot, usp.port, length);
  }
  else
  {
    MGMD_USER_TRACE("Sent Version %d %s with Src %s and Destn %s on interface %d/%d/%d of pktlen = %d",
                    version, str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                    usp.unit, usp.slot, usp.port, length);
  }
  return;
}

/*********************************************************************
* @purpose  Get the current status of displaying mgmd packet debug info
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
L7_RC_t mgmdDebugTraceFlagGet(L7_uchar8 family,
                              MGMD_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL *flag)
{
  mgmdDebugTraceFlags_t *mgmdDebugTraceFlags ;

  if (family == L7_AF_INET)
  {
    mgmdDebugTraceFlags = &mgmdDebugTraceFlag[MGMD_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    mgmdDebugTraceFlags = &mgmdDebugTraceFlag[MGMD_MAP_IPV6_CB];
  }
  else
  {
    return L7_FAILURE;
  }

  if (((*(mgmdDebugTraceFlags[traceFlag/MGMD_DEBUG_TRACE_FLAG_BITS_MAX])) & 
       (MGMD_DEBUG_TRACE_FLAG_VALUE << (traceFlag % MGMD_DEBUG_TRACE_FLAG_BITS_MAX))) != L7_NULL)
  {
    *flag = L7_TRUE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
* @param    familyType   @b{(input)} Address Family type
* @param    traceFlag    @b{(input)} type of trace flag
* @param    flag         @b{(input)} trace flag value
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugTraceFlagSet(L7_uchar8 family, 
                              MGMD_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL flag)
{
  mgmdDebugTraceFlags_t *mgmdDebugTraceFlags = L7_NULLPTR;

  if (family == L7_AF_INET)
  {
    mgmdDebugTraceFlags = &mgmdDebugTraceFlag[MGMD_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    mgmdDebugTraceFlags = &mgmdDebugTraceFlag[MGMD_MAP_IPV6_CB];
  }
  else
  {
    sysapiPrintf("\nWrong Family - %d.\n", family);
    return L7_FAILURE;
  }

   if (flag == L7_TRUE)
   {
     (*(mgmdDebugTraceFlags[traceFlag/MGMD_DEBUG_TRACE_FLAG_BITS_MAX])) |= 
               (MGMD_DEBUG_TRACE_FLAG_VALUE << (traceFlag % MGMD_DEBUG_TRACE_FLAG_BITS_MAX));
   }
   else
   {
     (*(mgmdDebugTraceFlags[traceFlag/MGMD_DEBUG_TRACE_FLAG_BITS_MAX])) &= 
            (~(MGMD_DEBUG_TRACE_FLAG_VALUE << (traceFlag % MGMD_DEBUG_TRACE_FLAG_BITS_MAX)));
   }

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a 
     debug config save.
    */ 

  mgmdDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for mgmd trace data
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
L7_RC_t mgmdDebugTraceFlagSave()
{
  memcpy(mgmdDebugCfg.cfg.mgmdDebugTraceFlag, mgmdDebugTraceFlag, 
         sizeof(mgmdDebugCfg.cfg.mgmdDebugTraceFlag));
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
void mgmdDebugCfgUpdate(void)
{ 
  mgmdDebugTraceFlagSave();
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
void mgmdDebugCfgRead(void)
{ 
  /* reset the debug flags*/
  memset((void*)&mgmdDebugCfg, 0 ,sizeof(mgmdDebugCfg_t));

  (void)sysapiSupportCfgFileGet(L7_FLEX_MGMD_MAP_COMPONENT_ID, MGMD_DEBUG_CFG_FILENAME, 
                                (L7_char8 *)&mgmdDebugCfg, (L7_uint32)sizeof(mgmdDebugCfg_t), 
                                &mgmdDebugCfg.checkSum, MGMD_DEBUG_CFG_VER_CURRENT, 
                                mgmdDebugBuildDefaultConfigData, L7_NULL);

  mgmdDebugCfg.hdr.dataChanged = L7_FALSE;

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
void mgmdDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_MGMD_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = mgmdDebugSave;
    supportDebugDescr.userControl.hasDataChanged = mgmdDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = mgmdDebugRestore;

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
void mgmdDebugDeRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_MGMD_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = mgmdDebugSave;
    supportDebugDescr.userControl.hasDataChanged = mgmdDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = mgmdDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */
    (void) supportDebugDeregister(supportDebugDescr);
}



void mgmdDebugPrint(L7_uchar8 family)
{
  L7_uint32 flagIndex;

  sysapiPrintf("\n-----------------\n");
  sysapiPrintf("\n version = %d", mgmdDebugCfg.hdr.version);
  sysapiPrintf("\n compId = %d", mgmdDebugCfg.hdr.componentID);
  sysapiPrintf("\n type = %d", mgmdDebugCfg.hdr.type);
  sysapiPrintf("\n length = %d", mgmdDebugCfg.hdr.length);
  sysapiPrintf("\n fileName = %s", mgmdDebugCfg.hdr.filename);
  sysapiPrintf("\n dataChng = %d", mgmdDebugCfg.hdr.dataChanged);
  sysapiPrintf("\n-----------------\n");

  mgmdDebugTraceFlags_t *mgmdTraceFlags = &(mgmdDebugCfg.cfg.mgmdDebugTraceFlag[family]);
  for (flagIndex = 0;  flagIndex < MGMD_DEBUG_LAST_TRACE ; flagIndex ++)
  {
    if (((*(mgmdTraceFlags[flagIndex/MGMD_DEBUG_TRACE_FLAG_BITS_MAX])) & 
         (MGMD_DEBUG_TRACE_FLAG_VALUE << (flagIndex % MGMD_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
    {
      sysapiPrintf("\n Flag [%d] is set", flagIndex);
    }
    else
    {
      sysapiPrintf("\n Flag [%d] is clear", flagIndex);
    }
  }
  sysapiPrintf("\n-----------------\n");
}

/*********************************************************************
* @purpose  Compute the memory allocated by the MGMD Mapping layer
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void mgmdMapDebugMemoryInfoCompute(void)
{
  mgmdMapCB_t *mgmdMapCB = L7_NULLPTR;
  L7_uchar8 addrFamily = 0;
  L7_BOOL v6CBPresent = L7_FALSE;
  L7_uint32 mgmdV4HeapSize = MGMD_V4_HEAP_SIZE;
#ifdef L7_IPV6_PACKAGE
  L7_uint32 mgmdV6HeapSize = MGMD_V6_HEAP_SIZE;
#endif

  addrFamily = L7_AF_INET6;
  if (mgmdMapCtrlBlockGet(addrFamily, &mgmdMapCB) == L7_SUCCESS)
  {
    v6CBPresent = L7_TRUE;
  }

  sysapiPrintf ("\n***** Memory Allocated By MGMD Mapping Layer *****\n");
  sysapiPrintf ("        Global Variables\n");
  sysapiPrintf ("          mgmdMapGblVars_t - %d\n", sizeof(mgmdMapGblVars_t));
  mgmdMapTotalBytesAllocated += sizeof(mgmdMapGblVars_t);
  sysapiPrintf ("          mgmdDebugCfg_t   - %d\n", sizeof(mgmdDebugCfg_t));
  mgmdMapTotalBytesAllocated += sizeof(mgmdDebugCfg_t);
  sysapiPrintf ("        Message Queues\n");
  sysapiPrintf ("          App Timer Queue  - Size[%d]*Num[%d] = %d\n", MGMDMAP_APPTIMER_Q_SIZE, MGMDMAP_APPTIMER_Q_COUNT, (MGMDMAP_APPTIMER_Q_SIZE*MGMDMAP_APPTIMER_Q_COUNT));
  mgmdMapTotalBytesAllocated += (MGMDMAP_APPTIMER_Q_SIZE*MGMDMAP_APPTIMER_Q_COUNT);
  sysapiPrintf ("          Events Queue     - Size[%d]*Num[%d] = %d\n", MGMDMAP_MSG_Q_SIZE, MGMDMAP_MSG_Q_COUNT, (MGMDMAP_MSG_Q_SIZE*MGMDMAP_MSG_Q_COUNT));
  mgmdMapTotalBytesAllocated += (MGMDMAP_MSG_Q_SIZE*MGMDMAP_MSG_Q_COUNT);
  sysapiPrintf ("          Ctrl Pkt Queue   - Size[%d]*Num[%d] = %d\n", MGMDMAP_PKT_Q_SIZE, MGMDMAP_PKT_Q_COUNT, (MGMDMAP_PKT_Q_SIZE*MGMDMAP_PKT_Q_COUNT));
  mgmdMapTotalBytesAllocated += (MGMDMAP_PKT_Q_SIZE*MGMDMAP_PKT_Q_COUNT);
  sysapiPrintf ("        Control Block IPv4 - %d\n", sizeof(mgmdMapCB_t));
  mgmdMapTotalBytesAllocated += sizeof(mgmdMapCB_t);
  if (v6CBPresent == L7_TRUE)
  {
    sysapiPrintf ("        Control Block IPv6 - %d\n", sizeof(mgmdMapCB_t));
    mgmdMapTotalBytesAllocated += sizeof(mgmdMapCB_t);
  }
  sysapiPrintf ("        Heap Size IPv4     - %d\n", mgmdV4HeapSize);
  mgmdMapTotalBytesAllocated += MGMD_V4_HEAP_SIZE;
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf ("        Heap Size IPv6     - %d\n", mgmdV6HeapSize);
  mgmdMapTotalBytesAllocated += MGMD_V6_HEAP_SIZE;
#endif
  sysapiPrintf ("\n");
  sysapiPrintf ("        Total Memory       - %d\n", mgmdMapTotalBytesAllocated);
  sysapiPrintf ("\n");
  sysapiPrintf ("***************************************************\n");

  return;
}

