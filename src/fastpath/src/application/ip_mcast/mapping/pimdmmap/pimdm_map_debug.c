/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  pimdm_map_debug.c
*
* @purpose   PIMDM Mapping Debug functions
*
* @component PIMDM Mapping Layer
*
* @comments  none
*
* @create    20/04/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "sysapi.h"
#include "nimapi.h"
#include "l3_defaultconfig.h"
#include "intf_bitset.h"
#include "pimdm_map.h"
#include "pimdm_map_config.h"
#include "pimdm_map_debug.h"
#include "pimdm_map_util.h"
#include "bspapi.h"
#include "support_api.h"
#include "nimapi.h"
#include "mcast_wrap.h"

/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[PIMDM_MAP_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

/* PIMDM Packet debug traces */
static pimdmDebugTraceFlags_t pimdmDebugTraceFlag[PIMDM_MAP_CB_MAX];
extern pimdmDebugCfg_t pimdmDebugCfg;

L7_uint32 pimdmMapTotalBytesAllocated = 0;

static void pimdmIntfBuildTestConfigData(pimdmCfgCkt_t *pCfg);
static void pimdmMapCfgDataShow(pimdmMapCB_t *pimdmMapCbPtr);
/*********************************************************************
*
* @purpose  Enable Debug Tracing for the PIMDMMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the PIMDMMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of PIMDMMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapDebugAllSet(void)
{
  memset(debugFlags, PIMDM_MAP_DEBUG_ALL_SET, sizeof(debugFlags));
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of PIMDMMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapDebugAllReset(void)
{
  memset(debugFlags, L7_NULL, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in PIMDMMAP.
*
* @param    flag      @b{(input)}  Debug Flag to set.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapDebugFlagSet(PIMDM_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= PIMDM_MAP_DEBUG_FLAG_LAST)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Invalid Flag %d",flag);
    return L7_FAILURE;
  }
  debugFlags[flag/PIMDM_MAP_FLAG_BITS_MAX] |= 
             (PIMDM_MAP_FLAG_VALUE << (flag % PIMDM_MAP_FLAG_BITS_MAX));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in PIMDMMAP.
*
* @param    flag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapDebugFlagReset(PIMDM_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= PIMDM_MAP_DEBUG_FLAG_LAST)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Invalid Flag %d",flag);
    return L7_FAILURE;
  }
  debugFlags[flag/PIMDM_MAP_FLAG_BITS_MAX] &= 
            (~(PIMDM_MAP_FLAG_VALUE << (flag % PIMDM_MAP_FLAG_BITS_MAX)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in PIMDMMAP.
*
* @param    debugType  @b{(input)}  Debug Flag to check.
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmMapDebugFlagCheck(PIMDM_MAP_DEBUG_FLAGS_t debugType)
{
  if(debugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  }
  if(debugType >= PIMDM_MAP_DEBUG_FLAG_LAST)
  {
    return L7_FALSE;
  }
  if((debugFlags[debugType/PIMDM_MAP_FLAG_BITS_MAX] & 
        (PIMDM_MAP_FLAG_VALUE << (debugType % PIMDM_MAP_FLAG_BITS_MAX))) != 0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current PIMDMMAP Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapDebugFlagShow(void)
{
  L7_uint32 index;

  if(debugEnabled == L7_TRUE)
  {
    sysapiPrintf(" PIMDMMAP Debugging : Enabled\n");
    for(index = 0;  index < PIMDM_MAP_DEBUG_FLAG_LAST ; index ++)
    {
      if(pimdmMapDebugFlagCheck(index) == L7_TRUE)
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
    sysapiPrintf (" PIMDMMAP Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the PIMDMMAP Debug utility
*
* @param    None.
*
* @returns  None.
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapDebugHelp(void)
{
  sysapiPrintf(" Use pimdmMapDebugEnable()/pimdmMapDebugDisable()"
                 "to Enable/Disable Debug trace in PIMDMMAP\n");
  sysapiPrintf(" Use pimdmMapDebugFlagSet(flag)/pimdmMapDebugFlagReset(flag)"
               " to Enable/Disable specific functionality traces\n");
  sysapiPrintf(" Use pimdmMapDebugFlagAllSet()/pimdmMapDebugFlagAllReset()"
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

  sysapiPrintf("\n pimdmMapDebugFlagShow()");
  sysapiPrintf("\n pimdmMapConfigDataTestShow(family = 1 0r 2) ");
  sysapiPrintf("\n pimdmMapDebugShow(family = 1 0r 2) ");
  sysapiPrintf("\n pimdmMapDebugQueueStatsPrint() ");
  sysapiPrintf("\n pimdmMapDebugQueueStatsClear() ");
  sysapiPrintf("\n pimdmMapControlBlockShow(family = 1 0r 2)");
  
}
/*********************************************************************
* @purpose  Dumps the pimdm configuration information onto the display
*           screen
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapCfgDataShow(pimdmMapCB_t *pimdmMapCbPtr)
{
    L7_uint32 i;
    L7_uchar8 c = 'n';
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    sysapiPrintf("\nDo you wish to see all interfaces? ");
    osapiFileRead(bspapiConsoleFdGet(), &c, sizeof(c));

    sysapiPrintf("\n+--------------------------------------------------------+");
    sysapiPrintf("\n|              PIMDM CONFIGURATION DATA                    |");
    sysapiPrintf("\n+--------------------------------------------------------+");

    sysapiPrintf("\n\nHeader Information:");
    sysapiPrintf("\nfilename...............................%s", 
                 pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.filename);
    sysapiPrintf("\nversion................................%d", 
                 pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.version);
    sysapiPrintf("\ncomponentID............................%d", 
                 pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.componentID);
    sysapiPrintf("\ntype...................................%d", 
                 pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.type);
    sysapiPrintf("\nlength.................................%d", 
                 pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.length);

    sysapiPrintf("\n\nPIMDM specific Information:");
    if (pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode == L7_ENABLE)
    {
        sysapiPrintf("\n\npimdmAdminMode......................enable");
    }
    else
    {
        sysapiPrintf("\npimdmAdminMode......................disable");
    }


    sysapiPrintf("\n----------------------------------");
    sysapiPrintf("\nintIf  ifName   admin    Hello    ");
    sysapiPrintf("\nNum             mode     Interval ");
    sysapiPrintf("\n----------------------------------");

    for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
    {
      if (pimdmMapCbPtr->pPimdmMapCfgMapTbl[i] != 0)
      {
        if ((pimdmMapCbPtr->pPimdmMapCfgData->ckt[pimdmMapCbPtr->
             pPimdmMapCfgMapTbl[i]].mode == L7_ENABLE) || 
             (c == 'y') || (c == 'Y'))
        {
            sysapiPrintf("\n%d  ", i);

            nimGetIntfName(i, L7_SYSNAME, ifName);
            sysapiPrintf("\t%s", ifName);
            if (pimdmMapCbPtr->pPimdmMapCfgData->ckt[pimdmMapCbPtr->
                 pPimdmMapCfgMapTbl[i]].mode == L7_ENABLE)
            {
                sysapiPrintf("\t  E");
            }
            else
            {
                sysapiPrintf("\t  D");
            }

            sysapiPrintf("     %d", 
                pimdmMapCbPtr->pPimdmMapCfgData->ckt[pimdmMapCbPtr->
                pPimdmMapCfgMapTbl[i]].pimInterfaceHelloInterval);

        }
      }
    }

    sysapiPrintf("\n-----------------------------------------------------------------------");
    sysapiPrintf("\nKey:");
    sysapiPrintf("\nE: Enable\tD: Disable");
    sysapiPrintf("\nL7_PIMDM_CONF_DEFAULT_HELLO_INTERVAL: 30");
    sysapiPrintf("\n");


    sysapiPrintf("Scaling Constants\n");
    sysapiPrintf("-----------------\n");


    sysapiPrintf("L7_IPMAP_INTF_MAX_COUNT - %d\n", L7_IPMAP_INTF_MAX_COUNT);

    sysapiPrintf("\n Some other PIMDM  mapping layer control block fields: ");    
   
    sysapiPrintf("\n pimdmMapCbPtr->pPimdmIntfInfo            = %d",
                  pimdmMapCbPtr->pPimdmIntfInfo);
    sysapiPrintf("\n pimdmMapCbPtr->pPimdmInfo                = %d",
                  pimdmMapCbPtr->pPimdmInfo);
    sysapiPrintf("\n pimdmMapCbPtr->pPimdmMapCfgMapTbl        = %d",
                  pimdmMapCbPtr->pPimdmMapCfgMapTbl);
    sysapiPrintf("\n pimdmMapCbPtr->pPimdmMapCfgData          = %d",
                  pimdmMapCbPtr->pPimdmMapCfgData);
    sysapiPrintf("\n pimdmMapCbPtr->gblVars                   = %d",
                  pimdmMapCbPtr->gblVars);
    sysapiPrintf("\n pimdmMapCbPtr->familyType                = %d",
                  pimdmMapCbPtr->familyType);
    sysapiPrintf("\n pimdmMapCbPtr->cbHandle                  = %d",
                  pimdmMapCbPtr->cbHandle);
    sysapiPrintf("\n pimdmMapCbPtr->gblVars->msgQSema         = %d",
                  pimdmMapCbPtr->gblVars->msgQSema);
    sysapiPrintf("\n pimdmMapCbPtr->gblVars->pimdmCnfgrState  = %d",
                  pimdmMapCbPtr->gblVars->pimdmCnfgrState);
    sysapiPrintf("\n pimdmMapCbPtr->gblVars->pimdmMapTaskId   = %d",
                  pimdmMapCbPtr->gblVars->pimdmMapTaskId);
    sysapiPrintf("\n pimdmMapCbPtr->gblVars->msgQSema         = %d",
                  pimdmMapCbPtr->gblVars->msgQSema);
    
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    familyType    @b{(input)} Address Family type
*
* @returns  void
*
* @comments This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void pimdmMapBuildTestConfigData(L7_uchar8 familyType)
{

    L7_uint32     i = L7_NULL;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    if (pimdmMapCtrlBlockGet (familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      return;
    }

    /*-------------------------------*/
    /* Build Non-Default Config Data */
    /*-------------------------------*/
    /* generic PIMDM cfg */
    pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode = L7_ENABLE;

    /* PIM router interface configuration parameters */
    for (i=1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
    {
      pimdmIntfBuildTestConfigData(&(pimdmMapCbPtr->pPimdmMapCfgData->ckt[i]));
    }

    /* End of Component's Test Non-default configuration Data */

    /* Force write of config file */
    pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;  
    sysapiPrintf("Built test config data\n");
}

/*********************************************************************
* @purpose  Build default PIMDM config data for an intf 
*
* @param    pCfg   @b{(output)} pointer to the PIMDM ckt configuration record
*
* @returns  void
*
* @comments none 
*
* @end
*********************************************************************/
void pimdmIntfBuildTestConfigData(pimdmCfgCkt_t *pCfg)
{
  L7_uint32 intIfNum = L7_NULL;


  /* Ensure at least one entry in each array has a unique value to validate
     accurate migration */


  if (nimIntIfFromConfigIDGet(&pCfg->configId, &intIfNum) != L7_SUCCESS)
  {
    intIfNum = 0xFF;
  }

  pCfg->mode                      = L7_ENABLE;
  pCfg->pimInterfaceHelloInterval = 45 + intIfNum;
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
void pimdmMapConfigDataTestShow(L7_uchar8 familyType)
{

    L7_fileHdr_t *pFileHdr = L7_NULLPTR;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    if (pimdmMapCtrlBlockGet (familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      return;
    }

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    pimdmMapCfgDataShow(pimdmMapCbPtr);


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("pPimdmMapCfgData->checkSum : %u\n", 
                  pimdmMapCbPtr->pPimdmMapCfgData->checkSum);


}

/*********************************************************************
* @purpose  Display internal PIMDM information for debugging
*
* @param    familyType    @b{(input)} Address Family type
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapDebugShow(L7_uchar8 familyType)
{
  pimdmMapIntfInfo_t *pIntf = L7_NULLPTR;
  L7_uint32 index = L7_NULL;
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  if (pimdmMapCtrlBlockGet (familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    return;
  }

  sysapiPrintf("PIMDM Internal Debug Information\n");

  sysapiPrintf("    PIMDM Initialized=%d  \n",
              pimdmMapCbPtr->pPimdmInfo->pimdmInitialized );

  sysapiPrintf("  Routing interfaces:\n");
  for (index = 1, pIntf = pimdmMapCbPtr->pPimdmIntfInfo; 
                                    index < MCAST_MAX_INTERFACES; index++, pIntf++)
  {
    sysapiPrintf("    [%2.1d]: intIfNum=%d\n",
                index,  pIntf->intIfNum);
  }

  sysapiPrintf("\n");
}


/*********************************************************************
*
* @purpose  Shows the PIMDMMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void pimdmMapDebugQueueStatsShow(void)
{
  L7_uint32 qIndex;
  L7_uchar8 qName[20];

  sysapiPrintf(" QueueName   CurMsgCnt HighWaterMark  SendFailCnt RxSuccessCnt MaxQSize MsgSize \n");
  sysapiPrintf ("\n------------------------------------------------------------------------------\n");
  for (qIndex = PIMDM_APP_TIMER_Q; qIndex < PIMDM_MAX_Q; qIndex++ )
  {
    pimdmMapQueue_t   *pimdmMapQueue = &pimdmGblVariables_g.pimdmQueue[qIndex];
    osapiStrncpy(qName, (pimdmMapQueue->QName) + 8, 20);
    sysapiPrintf("\n %-15s  %-10d %-10d %-10d %-10d %-7d %-7d  ",
                 qName, pimdmMapQueue->QCurrentMsgCnt, pimdmMapQueue->QMaxRx,
                 pimdmMapQueue->QSendFailedCnt, pimdmMapQueue->QRxSuccess,
                 pimdmMapQueue->QCount , pimdmMapQueue->QSize);
        
  }
  sysapiPrintf("\n\n");

  sysapiPrintf("No of MFC events in the Control pkt Queue : %d\n",
               pimdmMapCountersValueGet (PIMDM_MAP_NUM_MFC_EVENTS));
  sysapiPrintf("No of MGMD events in the Events Queue : %d\n",
               pimdmMapCountersValueGet (PIMDM_MAP_NUM_MGMD_EVENTS));
  sysapiPrintf("No of MFC overflows in the Control pkt Queue : %d\n",
               pimdmMapCountersValueGet (PIMDM_MAP_NUM_MFC_OVERFLOW_EVENTS));
  sysapiPrintf("No of MGMD overflows in the Events Queue : %d\n",
               pimdmMapCountersValueGet (PIMDM_MAP_NUM_MGMD_OVERFLOW_EVENTS));
  sysapiPrintf("\n");  
}

/*********************************************************************
*
* @purpose  Clears the PIMDMMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void pimdmMapDebugQueueStatsClear(void)
{
  L7_uint32 QIndex=0;

  if (osapiSemaTake (pimdmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    sysapiPrintf ("queueCountersSemId - Acquire Failed.\n");
    return;
  }

  for (QIndex = 0; QIndex < PIMDM_MAX_Q; QIndex++)
  {
    pimdmGblVariables_g.pimdmQueue[QIndex].QCurrentMsgCnt = 0;
    pimdmGblVariables_g.pimdmQueue[QIndex].QSendFailedCnt = 0;
    pimdmGblVariables_g.pimdmQueue[QIndex].QMaxRx = 0;
    pimdmGblVariables_g.pimdmQueue[QIndex].QRxSuccess = 0;
  }

  osapiSemaGive (pimdmGblVariables_g.queueCountersSemId);

  pimdmMapCountersUpdate (PIMDM_MAP_NUM_MFC_EVENTS, PIMDM_MAP_COUNTERS_RESET);
  pimdmMapCountersUpdate (PIMDM_MAP_NUM_MGMD_EVENTS, PIMDM_MAP_COUNTERS_RESET);
  pimdmMapCountersUpdate (PIMDM_MAP_NUM_MFC_OVERFLOW_EVENTS, PIMDM_MAP_COUNTERS_RESET);
  pimdmMapCountersUpdate (PIMDM_MAP_NUM_MGMD_OVERFLOW_EVENTS, PIMDM_MAP_COUNTERS_RESET); 
}

/******************************************************************************
* @purpose Display the Control Block of PIM-DM Mapping Layer 
*
* @param        none
*
* @returns void
*
* @comments
*
* @end
******************************************************************************/
void pimdmMapCBShow(L7_uchar8 familyType)
{
   pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;
   L7_uint32 i = 0;

   if ((familyType != L7_AF_INET) && (familyType != L7_AF_INET6))
   {
     sysapiPrintf ("Invalid Family Type specified - %d.\n", familyType);
     return;
   }

   if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     sysapiPrintf ("Failed to get PIM-DM MAP Control Block.\n");
     return;
   }

   sysapiPrintf ("**********************************************\n");
   sysapiPrintf ("  Family Type - %d.\n", pimdmMapCbPtr->familyType);
   sysapiPrintf ("  warmRestartInProgres - %s.\n", pimdmMapCbPtr->warmRestartInProgress?"L7_TRUE":"L7_FALSE");

   sysapiPrintf ("  cbHandle           - %p.\n", pimdmMapCbPtr->cbHandle);
   sysapiPrintf ("  pPimdmMapCfgData   - %p.\n", pimdmMapCbPtr->pPimdmMapCfgData);
   sysapiPrintf ("  pPimdmMapCfgMapTbl - %p.\n", pimdmMapCbPtr->pPimdmMapCfgMapTbl);
   sysapiPrintf ("  pPimdmInfo         - %p.\n", pimdmMapCbPtr->pPimdmInfo);
   sysapiPrintf ("  pPimdmIntfInfo     - %p.\n", pimdmMapCbPtr->pPimdmIntfInfo);

   sysapiPrintf ("  \ngblVars     - %p.\n", pimdmMapCbPtr->gblVars);
   sysapiPrintf ("     msgQSema             - %p.\n", pimdmMapCbPtr->gblVars->msgQSema);
   sysapiPrintf ("     pimdmMapTaskId       - %d.\n", pimdmMapCbPtr->gblVars->pimdmMapTaskId);
   sysapiPrintf ("     pimdmCnfgrState      - %d.\n", pimdmMapCbPtr->gblVars->pimdmCnfgrState);
   sysapiPrintf ("     warmRestart          - %s.\n", pimdmMapCbPtr->gblVars->warmRestart?"L7_TRUE":"L7_FALSE");
   sysapiPrintf ("**********************************************\n");

   sysapiPrintf ("  \npimdmInfo... \n" );
   sysapiPrintf ("     pimdmInitialized     - %d.\n", pimdmMapCbPtr->pPimdmInfo->pimdmInitialized);

   sysapiPrintf ("  \npimdmIntfInfo... \n" );
   for (i=0; i<L7_MAX_INTERFACE_COUNT; i++)
   {
     if (pimdmMapCbPtr->pimdmIntfInfo[i].pimdmIsOperational == L7_TRUE)
     {
       sysapiPrintf ("     intIfNum             - %d.\n", pimdmMapCbPtr->pimdmIntfInfo[i].intIfNum);
       sysapiPrintf ("     pimdmIsOperational   - %d.\n", pimdmMapCbPtr->pimdmIntfInfo[i].pimdmIsOperational);
     }
   }

   return;
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/



/*============================================================================*/
/*===================  START OF PIMDM PACKET DEBUG TRACES =====================*/
/*============================================================================*/


/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
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
void pimdmDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
                              L7_uint32 rtrIfNum, L7_inet_addr_t *src,
                              L7_inet_addr_t *dest, L7_uchar8 *payLoad, 
                              L7_uint32 length)
{
  L7_uchar8 srcBuff[IPV6_DISP_ADDR_LEN], destBuff[IPV6_DISP_ADDR_LEN];
  L7_uchar8 str[PIMDM_PKT_TYPE_STR_LEN], pktType;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_uint32 intIfNum = 0;
  nimUSP_t usp;
  L7_uchar8 version;

  pimdmDebugTraceFlagGet(family, PIMDM_DEBUG_PACKET_RX_TRACE, &rxFlag);
  pimdmDebugTraceFlagGet(family, PIMDM_DEBUG_PACKET_TX_TRACE, &txFlag);

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

  MCAST_GET_BYTE (pktType, payLoad);
  version = pktType >> 4;
  pktType = pktType & 0x0f;

  switch (pktType)
  {
  case PIM_PDU_HELLO:
    osapiStrncpySafe(str, "Hello ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_REGISTER:
    osapiStrncpySafe(str, "Register ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case  PIM_PDU_REGISTER_STOP:
    osapiStrncpySafe(str, "Register-Stop ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_JOIN_PRUNE:
    osapiStrncpySafe(str, "Join-Prune ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_BOOTSTRAP: 
    osapiStrncpySafe(str, "BootStrap ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_ASSERT:  
    osapiStrncpySafe(str, "Assert ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_GRAFT:
    osapiStrncpySafe(str, "Graft ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_GRAFT_ACK:
    osapiStrncpySafe(str, "Graft-Ack ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_CANDIDATE_RP_ADVERTISEMENT:
    osapiStrncpySafe(str, "Candidate-RP Adv ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_STATE_REFRESH:
    osapiStrncpySafe(str, "State-Refresh ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  default:
    osapiStrncpySafe(str, "Unknown PIM Pkt type ", PIMDM_PKT_TYPE_STR_LEN);
    break;
  }

  if (rxTrace == L7_TRUE)
  {
    PIMDM_USER_TRACE("Received PIMv%d %s with Src %s and Destn %s on interface %d/%d/%d of len = %d",
                    version, str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                    usp.unit, usp.slot, usp.port, length);
  }
  else
  {
    PIMDM_USER_TRACE("Sent PIMv%d %s with Src %s and Destn %s on interface %d/%d/%d of len = %d",
                    version, str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                    usp.unit, usp.slot, usp.port, length);
  }
  return;
}


/*********************************************************************
* @purpose  Get the current status of displaying pimdm packet debug info
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
L7_RC_t pimdmDebugTraceFlagGet(L7_uchar8 family,
                              PIMDM_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL *flag)
{
  pimdmDebugTraceFlags_t *pimdmDebugTraceFlags ;

  if (family == L7_AF_INET)
  {
    pimdmDebugTraceFlags = &pimdmDebugTraceFlag[PIMDM_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    pimdmDebugTraceFlags = &pimdmDebugTraceFlag[PIMDM_MAP_IPV6_CB];
  }
  else
  {
    sysapiPrintf ("\nInvalid Address Family - %d.\n", family);
    return L7_FAILURE;
  }

  if (((*(pimdmDebugTraceFlags[traceFlag/PIMDM_DEBUG_TRACE_FLAG_BITS_MAX])) & 
       (PIMDM_DEBUG_TRACE_FLAG_VALUE << (traceFlag % PIMDM_DEBUG_TRACE_FLAG_BITS_MAX))) != L7_NULL)
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
L7_RC_t pimdmDebugTraceFlagSet(L7_uchar8 family, 
                              PIMDM_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL flag)
{
  pimdmDebugTraceFlags_t *pimdmDebugTraceFlags = L7_NULLPTR;

  if (family == L7_AF_INET)
  {
    pimdmDebugTraceFlags = &pimdmDebugTraceFlag[PIMDM_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    pimdmDebugTraceFlags = &pimdmDebugTraceFlag[PIMDM_MAP_IPV6_CB];
  }
  else
  {
    sysapiPrintf ("\nInvalid Address Family - %d.\n", family);
    return L7_FAILURE;
  }

   if (flag == L7_TRUE)
   {
     (*(pimdmDebugTraceFlags[traceFlag/PIMDM_DEBUG_TRACE_FLAG_BITS_MAX])) |= 
               (PIMDM_DEBUG_TRACE_FLAG_VALUE << (traceFlag % PIMDM_DEBUG_TRACE_FLAG_BITS_MAX));
   }
   else
   {
     (*(pimdmDebugTraceFlags[traceFlag/PIMDM_DEBUG_TRACE_FLAG_BITS_MAX])) &= 
            (~(PIMDM_DEBUG_TRACE_FLAG_VALUE << (traceFlag % PIMDM_DEBUG_TRACE_FLAG_BITS_MAX)));
   }

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a 
     debug config save.
    */ 

  pimdmDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for pimdm trace data
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
L7_RC_t pimdmDebugTraceFlagSave()
{
  memcpy(pimdmDebugCfg.cfg.pimdmDebugTraceFlag, pimdmDebugTraceFlag, 
         sizeof(pimdmDebugCfg.cfg.pimdmDebugTraceFlag));
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
void pimdmDebugCfgUpdate(void)
{ 
  pimdmDebugTraceFlagSave();
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
void pimdmDebugCfgRead(void)
{ 
      /* reset the debug flags*/
    memset((void*)&pimdmDebugCfg, 0 ,sizeof(pimdmDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_FLEX_PIMDM_MAP_COMPONENT_ID, PIMDM_DEBUG_CFG_FILENAME, 
                         (L7_char8 *)&pimdmDebugCfg, (L7_uint32)sizeof(pimdmDebugCfg_t), 
                         &pimdmDebugCfg.checkSum, PIMDM_DEBUG_CFG_VER_CURRENT, 
                         pimdmDebugBuildDefaultConfigData, L7_NULL);

    pimdmDebugCfg.hdr.dataChanged = L7_FALSE;

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
void pimdmDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_PIMDM_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = pimdmDebugSave;
    supportDebugDescr.userControl.hasDataChanged = pimdmDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = pimdmDebugRestore;

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
void pimdmDebugDeRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_PIMDM_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = pimdmDebugSave;
    supportDebugDescr.userControl.hasDataChanged = pimdmDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = pimdmDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */
    (void) supportDebugDeregister(supportDebugDescr);
}

void printPimdmDebug(L7_uchar8 family)
{
  L7_uint32 flagIndex;

  sysapiPrintf("\n-----------------\n");
  sysapiPrintf("\n version = %d", pimdmDebugCfg.hdr.version);
  sysapiPrintf("\n compId = %d", pimdmDebugCfg.hdr.componentID);
  sysapiPrintf("\n type = %d", pimdmDebugCfg.hdr.type);
  sysapiPrintf("\n length = %d", pimdmDebugCfg.hdr.length);
  sysapiPrintf("\n fileName = %s", pimdmDebugCfg.hdr.filename);
  sysapiPrintf("\n dataChng = %d", pimdmDebugCfg.hdr.dataChanged);
  sysapiPrintf("\n-----------------\n");

  pimdmDebugTraceFlags_t *pimdmTraceFlags = &(pimdmDebugCfg.cfg.pimdmDebugTraceFlag[family]);
  for (flagIndex = 0;  flagIndex < PIMDM_DEBUG_LAST_TRACE ; flagIndex ++)
  {
    if (((*(pimdmTraceFlags[flagIndex/PIMDM_DEBUG_TRACE_FLAG_BITS_MAX])) & 
         (PIMDM_DEBUG_TRACE_FLAG_VALUE << (flagIndex % PIMDM_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
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
* @purpose  Compute the memory allocated by the PIMDM Mapping layer
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void pimdmMapDebugMemoryInfoCompute(void)
{
  pimdmMapCB_t *pimdmMapCB = L7_NULLPTR;
  L7_uchar8 addrFamily = 0;
  L7_BOOL v6CBPresent = L7_FALSE;

  addrFamily = L7_AF_INET6;
  if (pimdmMapCtrlBlockGet(addrFamily, &pimdmMapCB) == L7_SUCCESS)
  {
    v6CBPresent = L7_TRUE;
  }

  sysapiPrintf ("\n***** Memory Allocated By PIMDM Mapping Layer *****\n");
  sysapiPrintf ("        Global Variables\n");
  sysapiPrintf ("          pimdmGblVars_t   - %d\n", sizeof(pimdmGblVars_t));
  pimdmMapTotalBytesAllocated += sizeof(pimdmGblVars_t);
  sysapiPrintf ("          pimdmDebugCfg_t  - %d\n", sizeof(pimdmDebugCfg_t));
  pimdmMapTotalBytesAllocated += sizeof(pimdmDebugCfg_t);
  sysapiPrintf ("        Message Queues\n");
  sysapiPrintf ("          App Timer Queue  - Size[%d]*Num[%d] = %d\n", PIMDMMAP_APP_TMR_MSG_SIZE, PIMDMMAP_APP_TMR_MSG_COUNT, (PIMDMMAP_APP_TMR_MSG_SIZE*PIMDMMAP_APP_TMR_MSG_COUNT));
  pimdmMapTotalBytesAllocated += (PIMDMMAP_APP_TMR_MSG_SIZE*PIMDMMAP_APP_TMR_MSG_COUNT);
  sysapiPrintf ("          Events Queue     - Size[%d]*Num[%d] = %d\n", PIMDMMAP_MSG_SIZE, PIMDMMAP_MSG_COUNT, (PIMDMMAP_MSG_SIZE*PIMDMMAP_MSG_COUNT));
  pimdmMapTotalBytesAllocated += (PIMDMMAP_MSG_SIZE*PIMDMMAP_MSG_COUNT);
  sysapiPrintf ("          Ctrl Pkt Queue   - Size[%d]*Num[%d] = %d\n", PIMDMMAP_CTRL_PKT_MSG_SIZE, PIMDMMAP_CTRL_PKT_MSG_COUNT, (PIMDMMAP_CTRL_PKT_MSG_SIZE*PIMDMMAP_CTRL_PKT_MSG_COUNT));
  pimdmMapTotalBytesAllocated += (PIMDMMAP_CTRL_PKT_MSG_SIZE*PIMDMMAP_CTRL_PKT_MSG_COUNT);
  sysapiPrintf ("        Control Block IPv4 - %d\n", sizeof(pimdmMapCB_t));
  pimdmMapTotalBytesAllocated += sizeof(pimdmMapCB_t);
  if (v6CBPresent == L7_TRUE)
  {
    sysapiPrintf ("        Control Block IPv6 - %d\n", sizeof(pimdmMapCB_t));
    pimdmMapTotalBytesAllocated += sizeof(pimdmMapCB_t);
  }
  sysapiPrintf ("\n");
  sysapiPrintf ("        Total Memory       - %d\n", pimdmMapTotalBytesAllocated);
  sysapiPrintf ("\n");
  sysapiPrintf ("***************************************************\n");

  return;
}





