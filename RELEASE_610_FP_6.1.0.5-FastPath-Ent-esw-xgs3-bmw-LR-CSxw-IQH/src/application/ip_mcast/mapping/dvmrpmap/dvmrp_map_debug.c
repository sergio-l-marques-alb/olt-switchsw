/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dvmrpmap_debug.c
*
* @purpose   DVMRP Mapping Debug functions
*
* @component DVMRP Mapping Layer
*
* @comments  none
*
* @create    02/17/2002
*
* @author    M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/
#include "l7_common_l3.h"
#include "l3_mcast_commdefs.h"
#include "nimapi.h"
#include "l7_dvmrp_api.h"   
#include "mcast_wrap.h"
#include "dvmrp_api.h"                   
#include "l7_ip_api.h"                  
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h"
#include "dvmrp_map.h"
#include "dvmrp_map_util.h" 
#include "dvmrp_map_debug.h" 
#include "dvmrp_debug_api.h"
#include "dvmrp_common.h"
#include "support_api.h"

/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[DVMRP_MAP_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

L7_uint32 dvmrpMapTotalBytesAllocated = 0;

/* DVMRP Packet debug traces */
static dvmrpDebugTraceFlags_t dvmrpDebugTraceFlag;

extern dvmrpDebugCfg_t dvmrpDebugCfg;


/*****************************************************************
    Function Prototypes
******************************************************************/
static void dvmrpIntfBuildTestConfigData(dvmrpCfgCkt_t *pCfg, L7_uint32 seed);
static void dvmrpCfgShow(void);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the DVMRPMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the DVMRPMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of DVMRPMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugAllSet(void)
{
  memset(debugFlags, DVMRP_MAP_DEBUG_ALL_SET, sizeof(debugFlags));
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of DVMRPMAP.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugAllReset(void)
{
  memset(debugFlags, L7_NULL, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in DVMRPMAP.
*
* @param    flag      @b{(input)}  Debug Flag to set.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugFlagSet(DVMRP_MAP_DEBUG_FLAGS_t flag)
{
  if (flag >= DVMRP_MAP_DEBUG_FLAG_LAST)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid Flag \n");
    return L7_FAILURE;
  }
  debugFlags[flag/DVMRP_MAP_FLAG_BITS_MAX] |= 
  (DVMRP_MAP_FLAG_VALUE << (flag % DVMRP_MAP_FLAG_BITS_MAX));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in DVMRPMAP.
*
* @param    flag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDebugFlagReset(DVMRP_MAP_DEBUG_FLAGS_t flag)
{
  if (flag >= DVMRP_MAP_DEBUG_FLAG_LAST)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid Flag \n");
    return L7_FAILURE;
  }
  debugFlags[flag/DVMRP_MAP_FLAG_BITS_MAX] &= 
  (~(DVMRP_MAP_FLAG_VALUE << (flag % DVMRP_MAP_FLAG_BITS_MAX)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in DVMRPMAP.
*
* @param    debugType  @b{(input)}  Debug Flag to check.
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapDebugFlagCheck(DVMRP_MAP_DEBUG_FLAGS_t debugType)
{
  if (debugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  }
  if (debugType >= DVMRP_MAP_DEBUG_FLAG_LAST)
  {
    return L7_FALSE;
  }
  if ((debugFlags[debugType/DVMRP_MAP_FLAG_BITS_MAX] & 
       (DVMRP_MAP_FLAG_VALUE << (debugType % DVMRP_MAP_FLAG_BITS_MAX))) != 0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current DVMRPMAP Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapDebugFlagShow(void)
{
  L7_uint32 index;

  if (debugEnabled == L7_TRUE)
  {
    sysapiPrintf(" DVMRPMAP Debugging : Enabled\n");
    for (index = 0;  index < DVMRP_MAP_DEBUG_FLAG_LAST ; index ++)
    {
      if (dvmrpMapDebugFlagCheck(index) == L7_TRUE)
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
    sysapiPrintf (" DVMRPMAP Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows the current DVMRPMAP Debug Queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapDebugQueueStatsShow(void)
{
  L7_uint32 qIndex;
  L7_uchar8 qName[20];

  sysapiPrintf(" QueueName  CurMsgCnt HighWaterMark  SendFailCnt RxSuccessCnt MaxQSize MsgSize \n");
  sysapiPrintf ("\n----------------------------------------------------------------------------\n");
  for (qIndex = DVMRP_APP_TIMER_Q; qIndex < DVMRP_MAX_Q; qIndex++ )
  {
    dvmrpQueue_t   *dvmrpMapQueue = &dvmrpGblVar_g.dvmrpQueue[qIndex];
    osapiStrncpy(qName, (dvmrpMapQueue->QName) + 8, 20);
    sysapiPrintf("\n %-15s  %-10d %-10d %-10d %-10d %-7d %-7d  ",
                 qName, dvmrpMapQueue->QCurrentMsgCnt, dvmrpMapQueue->QMaxRx,
                 dvmrpMapQueue->QSendFailedCnt, dvmrpMapQueue->QRxSuccess,
                 dvmrpMapQueue->QCount , dvmrpMapQueue->QSize);
        
  }
  sysapiPrintf("\n");
}

/*********************************************************************
*
* @purpose  Clears the current DVMRPMAP Debug stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapDebugQueueStatsClear(void)
{
 L7_uint32 QIndex=0;

  sysapiPrintf(" Clearing the DVMRPMAP Queue Stats\n");
  for(QIndex=0;QIndex <DVMRP_MAX_Q; QIndex++)
  {
    dvmrpGblVar_g.dvmrpQueue[QIndex].QCurrentMsgCnt = 0;  
    dvmrpGblVar_g.dvmrpQueue[QIndex].QSendFailedCnt = 0;    
    dvmrpGblVar_g.dvmrpQueue[QIndex].QMaxRx = 0;    
    dvmrpGblVar_g.dvmrpQueue[QIndex].QRxSuccess = 0;    
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the DVMRPMAP Debug utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapDebugHelp(void)
{
  sysapiPrintf(" Use dvmrpMapDebugEnable()/dvmrpMapDebugDisable()"
               "to Enable/Disable Debug trace in DVMRPMAP\n");
  sysapiPrintf(" Use dvmrpMapDebugFlagSet(flag)/dvmrpMapDebugFlagReset(flag)"
               " to Enable/Disable specific functionality traces\n");
  sysapiPrintf(" Use dvmrpMapDebugFlagAllSet()/dvmrpMapDebugFlagAllReset()"
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
  sysapiPrintf("\n dvmrpMapDebugFlagShow()");
  sysapiPrintf("\n dvmrpMapDebugQueueStatsPrint() ");
  sysapiPrintf("\n dvmrpMapDebugClearStatsPrint() ");
  sysapiPrintf("\n dvmrpMapCfgDataShow() ");
  sysapiPrintf("\n dvmrpCfgShow() ");
  sysapiPrintf("\n dvmrpConfigDataTestShow() ");
  
}

/*********************************************************************
* @purpose  Dumps the DVMRP configuration information onto the
*           display screen.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapCfgDataShow(void)
{
  L7_uchar8 c = 'n';
  L7_uint32 i;

  sysapiPrintf("\nDo you wish to see all interfaces? ");
  c = getchar();

  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n|              DVMRP CONFIGURATION DATA                   |");
  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n\nHeader Information:");
  sysapiPrintf("\nfilename...............................%s", 
               dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.filename);
  sysapiPrintf("\nversion................................%d", 
               dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.version);
  sysapiPrintf("\ncomponentID............................%d", 
               dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.componentID);
  sysapiPrintf("\ntype...................................%d", 
               dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.type);
  sysapiPrintf("\nlength.................................%d", 
               dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.length);
  sysapiPrintf("\n\nDVMRP Router Information:");

  sysapiPrintf("\n\nDVMRP Routing Interfaces:");
  sysapiPrintf("\n----------------------------------");
  sysapiPrintf("\nIntf    admin       Interface    ");
  sysapiPrintf("\nNum      mode 		Metric      ");
  sysapiPrintf("\n----------------------------------");

  for (i=1; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    if (dvmrpGblVar_g.DvmrpMapCfgMapTbl[i] != 0)
    {
      if ((dvmrpGblVar_g.DvmrpMapCfgData.
           ckt[dvmrpGblVar_g.DvmrpMapCfgMapTbl[i]].intfAdminMode == 
           L7_ENABLE)|| (c == 'y') || (c == 'Y'))
      {
        sysapiPrintf("\n%d", i);

        if (dvmrpGblVar_g.DvmrpMapCfgData.
            ckt[dvmrpGblVar_g.DvmrpMapCfgMapTbl[i]].intfAdminMode == 
            L7_ENABLE)
        {
          sysapiPrintf("    E");
        }
        else
        {
          sysapiPrintf("    D");
        }
        sysapiPrintf("       %d", dvmrpGblVar_g.DvmrpMapCfgData.
                     ckt[dvmrpGblVar_g.DvmrpMapCfgMapTbl[i]].intfMetric);
      }
    }
  }

  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n|              DVMRP INFORMATION STRUCTURES               |");
  sysapiPrintf("\n+--------------------------------------------------------+");

  sysapiPrintf("\n\npDvmrpInfo structure:");
  if (dvmrpGblVar_g.DvmrpInfo.dvmrpEnabled == L7_TRUE)
  {
    sysapiPrintf("\n\tdvmrpInitialized........true");
  }
  else
  {
    sysapiPrintf("\n\tdvmrpInitialized........false");
  }


  sysapiPrintf("\n---------------------------\n");

  sysapiPrintf("\n\npDvmrpIntfInfo structure:");
  sysapiPrintf("\n-----------------------------");
  sysapiPrintf("\nintIfNum      dvmrpInitialized");
  sysapiPrintf("\n-----------------------------");
  for (i = 0; i < (MAX_INTERFACES); i++)
  {
    sysapiPrintf("\n%u", dvmrpGblVar_g.DvmrpIntfInfo[i].intIfNum);
    if (dvmrpGblVar_g.DvmrpIntfInfo[i].dvmrpEnabled == L7_TRUE)
    {
      sysapiPrintf("\t\ttrue");
    }
    else
    {
      sysapiPrintf("\t\tfalse");
    }
  }

  sysapiPrintf("\n msgQSema        = %d",dvmrpGblVar_g.msgQSema);
  sysapiPrintf("\n dvmrpMapTaskId  = %d",dvmrpGblVar_g.dvmrpMapTaskId);
  sysapiPrintf("\n dvmrpCnfgrState = %d",dvmrpGblVar_g.dvmrpCnfgrState);  
  sysapiPrintf("\n dvmrpMapCb      = %d",dvmrpGblVar_g.dvmrpCb);
  sysapiPrintf("\n family          = %d",dvmrpGblVar_g.family);  
  sysapiPrintf("\n heapAlloc       = %d",dvmrpGblVar_g.heapAlloc);    
  
  sysapiPrintf("\n-----------------------------\n");
}
/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    ver @b{(input)} version
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dvmrpBuildTestConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  /*---------------------------*/
  /* build non-default config data */
  /*---------------------------*/

  /* generic DVMRP cfg */
  dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode = 
  FD_DVMRP_DEFAULT_ADMIN_MODE + 1;

  /* DVMRP router interface configuration parameters */
  /* we are preserving any previous configId fields in the table */
  for (i=1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    (void)dvmrpIntfBuildTestConfigData(&(dvmrpGblVar_g.DvmrpMapCfgData.
                                         ckt[i]), i);
  }
  /* End of Component's Test Non-default configuration Data */


  /* Force write of config file */
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_TRUE;  
  sysapiPrintf("Built test config data\n");
}



/*********************************************************************
* @purpose  Build default router discovery data for the interface 
*
* @param    pCfg   @b{(output)} pointer to intf configuration information
* @param    seed   @b{(input)} seed
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void dvmrpIntfBuildTestConfigData(dvmrpCfgCkt_t *pCfg, L7_uint32 seed)
{
  /* NOTE FOR WRITING DEBUG TEST FUNCTIONS:
  
     Ensure at least one entry in the array is affected by the "seed" to ensure
     that we can uniquely determine that each separate entry has migrated 
     appropriately. */

  pCfg->intfAdminMode     = L7_ENABLE;
  pCfg->intfMetric        = FD_DVMRP_INTF_DEFAULT_METRIC + seed;
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
void dvmrpCfgShow(void)
{

  nimUSP_t    usp;
  L7_uint32   i;
  L7_uint32   intIfNum;


  /*--------------------------------*/
  /* cfgParms                       */
  /*                                */
  /* DO A RAW DUMP OF THE CONFIG    */
  /* FILE IF A PREVIOUSLY EXISTING  */
  /* DEBUG DUMP ROUTINE DOES NOT    */
  /* EXIST                          */
  /*--------------------------------*/

  sysapiPrintf("DvmrpMapCfgData.rtr.adminMode = %d\n", 
               dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode);


  /* Print interface information, if any */

  sysapiPrintf( "Printing interface data\n");
  sysapiPrintf( "-----------------------\n");
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    /* FOR INTERFACE STRUCTURES,  ALWAYS PRINT USP as well as intIfNUm
       TO ENSURE PROPER INSTANCE OF CONFIG MIGRATED */


    sysapiPrintf( "ENTRY: %d\n", i);
    sysapiPrintf( "----------------\n");

    intIfNum = 0;
    memset((void *)&usp, 0, sizeof(nimUSP_t));

    if (nimIntIfFromConfigIDGet(&dvmrpGblVar_g.DvmrpMapCfgData.
                                ckt[i].configId, &intIfNum) == L7_SUCCESS)
    {
      if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
      {
        sysapiPrintf( "USP = %d/%d/%d  (intIfNum  = %d \n",
                      usp.unit, usp.slot, usp.port, intIfNum);
      }
      else
      {

        sysapiPrintf("\n%50/0/0d");
        sysapiPrintf( "USP = 0/0/0  (intIfNum  = %d \n",  intIfNum);

      }
    }

    /* PRINT REST OF INTERFACE CONFIGURATION */

    sysapiPrintf("DvmrpMapCfgData.ckt[i].intfAdminMode = %d\n",  
                 dvmrpGblVar_g.DvmrpMapCfgData.ckt[i].intfAdminMode);

    sysapiPrintf("DvmrpMapCfgData.ckt[i].intfMetric = %d\n",  
                 dvmrpGblVar_g.DvmrpMapCfgData.ckt[i].intfMetric);

    sysapiPrintf("\n");

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
void dvmrpConfigDataTestShow(void)
{

  L7_fileHdr_t  *pFileHdr;

  /*-----------------------------*/
  /* Config File Header Contents */
  /*-----------------------------*/
  pFileHdr = &(dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr);

  sysapiCfgFileHeaderDump (pFileHdr);

  /*--------------------------------*/
  /* cfgParms                       */
  /*--------------------------------*/

  dvmrpCfgShow();


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

  sysapiPrintf( "Scaling Constants\n");
  sysapiPrintf( "-----------------\n");


  sysapiPrintf( "L7_IPMAP_INTF_MAX_COUNT - %d\n", L7_IPMAP_INTF_MAX_COUNT);



  /*-----------------------------*/
  /* Checksum                    */
  /*-----------------------------*/
  sysapiPrintf("DvmrpMapCfgData.checkSum : %u\n", 
               dvmrpGblVar_g.DvmrpMapCfgData.checkSum);


}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/



/*============================================================================*/
/*===================  START OF DVMRP PACKET DEBUG TRACES =====================*/
/*============================================================================*/



/*********************************************************************
*
* @purpose Trace mgmd packets received and transmitted
*
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
void dvmrpDebugPacketRxTxTrace(L7_BOOL rxTrace,
                              L7_uint32 rtrIfNum, L7_inet_addr_t *src,
                              L7_inet_addr_t *dest, L7_uchar8 *payLoad, 
                              L7_uint32 length)
{
  L7_uchar8 srcBuff[IPV6_DISP_ADDR_LEN], destBuff[IPV6_DISP_ADDR_LEN];
  L7_uchar8 str[DVMRP_PKT_TYPE_STR_LEN], pktType;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_uint32 intIfNum = 0;
  nimUSP_t usp;
  L7_uchar8 dvmrpCode;

  dvmrpDebugTraceFlagGet(DVMRP_DEBUG_PACKET_RX_TRACE, &rxFlag);
  dvmrpDebugTraceFlagGet(DVMRP_DEBUG_PACKET_TX_TRACE, &txFlag);

  if (((rxTrace == L7_TRUE) && (rxFlag != L7_TRUE)) ||
      ((rxTrace == L7_FALSE) && (txFlag != L7_TRUE)) )
  {
    return;
  }
  if (payLoad == L7_NULLPTR)
  {
    return;
  }
  if ((mcastIpMapRtrIntfToIntIfNum(L7_AF_INET, rtrIfNum, &intIfNum) != L7_SUCCESS) ||
      (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS))
  {
    return;
  }

  MCAST_GET_BYTE (dvmrpCode, payLoad);
  MCAST_GET_BYTE (pktType, payLoad);

  switch (pktType)
  {
  case DVMRP_PROBE:
    osapiStrncpySafe(str, "Probe ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  case DVMRP_REPORT:
    osapiStrncpySafe(str, "Report ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  case  DVMRP_ASK_NEIGHBORS:
    osapiStrncpySafe(str, "Ask-Neighbors ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  case DVMRP_ASK_NEIGHBORS2:
    osapiStrncpySafe(str, "Ask-Neighbors 2 ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  case DVMRP_PRUNE: 
    osapiStrncpySafe(str, "Prune ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  case DVMRP_GRAFT:  
    osapiStrncpySafe(str, "Graft ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  case DVMRP_GRAFT_ACK:
    osapiStrncpySafe(str, "Graft-Ack ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  default:
    osapiStrncpySafe(str, "Unknown Pkt type ", DVMRP_PKT_TYPE_STR_LEN);
    break;
  }

  if (rxTrace == L7_TRUE)
  {
    DVMRP_USER_TRACE("Received DVMRP %s with Src %s and Destn %s on interface %d/%d/%d of len = %d",
                     str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                     usp.unit, usp.slot, usp.port, length);
  }
  else
  {
    DVMRP_USER_TRACE("Sent DVMRP %s with Src %s and Destn %s on interface %d/%d/%d of len = %d",
                     str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                     usp.unit, usp.slot, usp.port, length);
  }
  return;
}



/*********************************************************************
* @purpose  Get the current status of displaying mgmd packet debug info
*            
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
L7_RC_t dvmrpDebugTraceFlagGet(DVMRP_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL *flag)
{
  if (((dvmrpDebugTraceFlag[traceFlag/DVMRP_DEBUG_TRACE_FLAG_BITS_MAX]) & 
       (DVMRP_DEBUG_TRACE_FLAG_VALUE << (traceFlag % DVMRP_DEBUG_TRACE_FLAG_BITS_MAX))) != L7_NULL)
  {
    *flag = L7_TRUE;
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
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
L7_RC_t dvmrpDebugTraceFlagSet(DVMRP_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag)
{
   if (flag == L7_TRUE)
   {
     (dvmrpDebugTraceFlag[traceFlag/DVMRP_DEBUG_TRACE_FLAG_BITS_MAX]) |= 
               (DVMRP_DEBUG_TRACE_FLAG_VALUE << (traceFlag % DVMRP_DEBUG_TRACE_FLAG_BITS_MAX));
   }
   else
   {
     (dvmrpDebugTraceFlag[traceFlag/DVMRP_DEBUG_TRACE_FLAG_BITS_MAX]) &= 
            (~(DVMRP_DEBUG_TRACE_FLAG_VALUE << (traceFlag % DVMRP_DEBUG_TRACE_FLAG_BITS_MAX)));
   }

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a 
     debug config save.
    */ 

  dvmrpDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for dvmrp trace data
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
L7_RC_t dvmrpDebugPacketTraceFlagSave()
{
  memcpy(dvmrpDebugCfg.cfg.dvmrpDebugTraceFlag, dvmrpDebugTraceFlag, 
         sizeof(dvmrpDebugCfg.cfg.dvmrpDebugTraceFlag));
         
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
void dvmrpDebugCfgUpdate(void)
{ 
  dvmrpDebugPacketTraceFlagSave();
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
void dvmrpDebugCfgRead(void)
{ 
      /* reset the debug flags*/
    memset((void*)&dvmrpDebugCfg, 0 ,sizeof(dvmrpDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_FLEX_DVMRP_MAP_COMPONENT_ID, DVMRP_DEBUG_CFG_FILENAME, 
                         (L7_char8 *)&dvmrpDebugCfg, (L7_uint32)sizeof(dvmrpDebugCfg_t), 
                         &dvmrpDebugCfg.checkSum, DVMRP_DEBUG_CFG_VER_CURRENT, 
                         dvmrpDebugBuildDefaultConfigData, L7_NULL);

    dvmrpDebugCfg.hdr.dataChanged = L7_FALSE;

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
void dvmrpDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_DVMRP_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = dvmrpDebugSave;
    supportDebugDescr.userControl.hasDataChanged = dvmrpDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = dvmrpDebugRestore;

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
void dvmrpDebugDeRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_DVMRP_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = dvmrpDebugSave;
    supportDebugDescr.userControl.hasDataChanged = dvmrpDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = dvmrpDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */
    (void) supportDebugDeregister(supportDebugDescr);
}

/*********************************************************************
* @purpose  Compute the memory allocated by the DVMRP Mapping layer
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void dvmrpMapDebugMemoryInfoCompute(void)
{
  sysapiPrintf ("***** Memory Allocated By DVMRP Mapping Layer *****\n");
  sysapiPrintf ("        Global Variables\n");
  sysapiPrintf ("          dvmrpGblVaribles_t - %d\n", sizeof(dvmrpGblVaribles_t));
  dvmrpMapTotalBytesAllocated += sizeof(dvmrpGblVaribles_t);
  sysapiPrintf ("          dvmrpDebugCfg_t   - %d\n", sizeof(dvmrpDebugCfg_t));
  dvmrpMapTotalBytesAllocated += sizeof(dvmrpDebugCfg_t);
  sysapiPrintf ("        Message Queues\n");
  sysapiPrintf ("          App Timer Queue  - Size[%d]*Num[%d] = %d\n", DVMRPMAP_APPTIMER_SIZE, DVMRPMAP_APPTIMER_COUNT, (DVMRPMAP_APPTIMER_SIZE*DVMRPMAP_APPTIMER_COUNT));
  dvmrpMapTotalBytesAllocated += (DVMRPMAP_APPTIMER_SIZE*DVMRPMAP_APPTIMER_COUNT);
  sysapiPrintf ("          Events Queue     - Size[%d]*Num[%d] = %d\n", DVMRPMAP_MSG_SIZE, DVMRPMAP_MSG_COUNT, (DVMRPMAP_MSG_SIZE*DVMRPMAP_MSG_COUNT));
  dvmrpMapTotalBytesAllocated += (DVMRPMAP_MSG_SIZE*DVMRPMAP_MSG_COUNT);
  sysapiPrintf ("          Ctrl Pkt Queue   - Size[%d]*Num[%d] = %d\n", DVMRPMAP_PKT_SIZE, DVMRPMAP_PKT_COUNT, (DVMRPMAP_PKT_SIZE*DVMRPMAP_PKT_COUNT));
  dvmrpMapTotalBytesAllocated += (DVMRPMAP_PKT_SIZE*DVMRPMAP_PKT_COUNT);
  sysapiPrintf ("\n");
  sysapiPrintf ("        Total Memory       - %d\n", dvmrpMapTotalBytesAllocated);
  sysapiPrintf ("\n");
  sysapiPrintf ("***************************************************\n");

  return;
}

