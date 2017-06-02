/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  pimsmmap_debug.c
*
* @purpose   PIM-SM Mapping Debug functions
*
* @component PIM-SM Mapping Layer
*
* @comments  none
*
* @create    03/11/2002
*
* @author    gkiran/dsatyanarayana
*
* @end
*
**********************************************************************/


#include "l7_pimsminclude.h"
#include "pimsm_map.h"
#include "pim_defs.h"
#include "mcast_defs.h"
#include "nimapi.h"
#include "l7_pimsm_api.h"
#include "pimsm_map_util.h"
#include "support_api.h"

/******************************************************************
                 Global Declarations
******************************************************************/

extern pimsmGblVariables_t pimsmGblVariables_g;

static L7_uchar8 debugFlags[PIMSM_MAP_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

L7_uint32 pimsmMapTotalBytesAllocated = 0;

/* PIMSM Packet debug traces */
static pimsmDebugTraceFlags_t pimsmDebugTraceFlag[PIMSM_MAP_CB_MAX];

extern pimsmDebugCfg_t pimsmDebugCfg;

/*****************************************************************
    Function Prototypes
******************************************************************/
static void pimsmIntfBuildTestConfigData(pimsmCfgCkt_t *pCfg);
static L7_RC_t pimsmMapCommonCfgDataShow(pimsmMapCB_t *pimsmMapCbPtr);

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the PIMSMMAP.
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
L7_RC_t pimsmMapDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the PIMSMMAP.
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
L7_RC_t pimsmMapDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of PIMSMMAP.
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
L7_RC_t pimsmMapDebugAllSet(void)
{
  memset(debugFlags, PIMSM_MAP_DEBUG_ALL_SET, sizeof(debugFlags));
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of PIMSMMAP.
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
L7_RC_t pimsmMapDebugAllReset(void)
{
  memset(debugFlags, L7_NULL, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in PIMSMMAP.
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
L7_RC_t pimsmMapDebugFlagSet(PIMSM_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= PIMSM_MAP_DEBUG_FLAG_LAST)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid flag %d",flag);
    return L7_FAILURE;
  }
  debugFlags[flag/PIMSM_MAP_FLAG_BITS_MAX] |= 
             (PIMSM_MAP_FLAG_VALUE << (flag % PIMSM_MAP_FLAG_BITS_MAX));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in PIMSMMAP.
*
* @param    debugFlag  @b{(input)}  Debug Flag to reset.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDebugFlagReset(PIMSM_MAP_DEBUG_FLAGS_t flag)
{
  if(flag >= PIMSM_MAP_DEBUG_FLAG_LAST)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid flag %d",flag);
    return L7_FAILURE;
  }
  debugFlags[flag/PIMSM_MAP_FLAG_BITS_MAX] &= 
            (~(PIMSM_MAP_FLAG_VALUE << (flag % PIMSM_MAP_FLAG_BITS_MAX)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in PIMSMMAP.
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
L7_BOOL pimsmMapDebugFlagCheck(PIMSM_MAP_DEBUG_FLAGS_t debugType)
{
  if(debugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  }
  if(debugType >= PIMSM_MAP_DEBUG_FLAG_LAST)
  {
    return L7_FALSE;
  }
  if((debugFlags[debugType/PIMSM_MAP_FLAG_BITS_MAX] & 
        (PIMSM_MAP_FLAG_VALUE << (debugType % PIMSM_MAP_FLAG_BITS_MAX))) != 0)
  {
     return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current PIMSMMAP Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @comments none
*
* @end
*********************************************************************/
void pimsmMapDebugFlagShow(void)
{
  L7_uint32 index;

  if(debugEnabled == L7_TRUE)
  {
    sysapiPrintf(" PIMSMMAP Debugging : Enabled\n");
    for(index = 0;  index < PIMSM_MAP_DEBUG_FLAG_LAST ; index ++)
    {
      if(pimsmMapDebugFlagCheck(index) == L7_TRUE)
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
    sysapiPrintf (" PIMSMMAP Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the PIMSMMAP Debug utility
*
* @param    None.
*
* @returns  None.
*
* @comments none
*
* @end
*********************************************************************/
void pimsmMapDebugHelp(void)
{
  sysapiPrintf(" Use pimsmMapDebugEnable()/pimsmMapDebugDisable()"
                 "to Enable/Disable Debug trace in PIMSMMAP\n");
  sysapiPrintf(" Use pimsmMapDebugFlagSet(flag)/pimsmMapDebugFlagReset(flag)"
               " to Enable/Disable specific functionality traces\n");
  sysapiPrintf(" Use pimsmMapDebugFlagAllSet()/pimsmMapDebugFlagAllReset()"
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

  sysapiPrintf("\n pimsmMapDebugFlagShow()");
  sysapiPrintf("\n pimsmMapCfgDataShow()");
  sysapiPrintf("\n pimsm6MapCfgDataShow()");
  sysapiPrintf("\n pimsmMapConfigDataTestShow(family = 1 or 2)");
  sysapiPrintf("\n pimsmMapDebugCandRPShow(family = 1 or 2)");
  sysapiPrintf("\n pimsmMapDebugStaticRPShow(family = 1 or 2)");
  sysapiPrintf("\n pimsmMapDebugCandBsrShow(family = 1 or 2)");  
  sysapiPrintf("\n pimsmMapDebugSSMTableShow(family = 1 or 2) ");
  sysapiPrintf("\n pimsmMapDebugQueueStatsPrint() ");
  sysapiPrintf("\n pimsmMapDebugQueueStatsClear() ");
  sysapiPrintf("\n pimsmMapControlBlockShow(family = 1 0r 2)");
  sysapiPrintf("\n pimsmMapDebugRPCandidateEntryShow(family = 1 0r 2)");
  sysapiPrintf("\n pimsmMapDebugStaticRPEntryShow(family = 1 0r 2)");

}


/*********************************************************************
* @purpose  Dumps the PIM-SM configuration information onto the
*           display screen.
*
* @param    none 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCfgDataShow(void)
{
  pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;

  if (pimsmMapCtrlBlockGet(L7_AF_INET, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
    return L7_FAILURE;
  }
  return pimsmMapCommonCfgDataShow(pimsmMapCbPtr);
}

/*********************************************************************
* @purpose  Dumps the PIM-SM configuration information onto the
*           display screen.
*
* @param    none 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsm6MapCfgDataShow(void)
{
  pimsmMapCB_t *pimsm6MapCbPtr = L7_NULLPTR;
  if (pimsmMapCtrlBlockGet(L7_AF_INET6, &pimsm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
    return L7_FAILURE;
  }
  return pimsmMapCommonCfgDataShow(pimsm6MapCbPtr);
}
  
L7_RC_t pimsmMapCommonCfgDataShow(pimsmMapCB_t *pimsmMapCbPtr)
{
  L7_uint32 i;
  L7_uchar8 debug_buf[30];
  L7_uint32 s, rpIpAddr, rpGrpAddr, rpGrpMask;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n Function Entered\n");

  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n|              PIMSM "
                "CONFIGURATION DATA                  |");
  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("n\nHeader Information:");
  sysapiPrintf("\nfilename...............................%s", 
                pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.filename);
  sysapiPrintf("\nversion................................%d", 
                pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.version);
  sysapiPrintf("\ncomponentID............................%d",
                pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.componentID);
  sysapiPrintf("\ntype...................................%d", 
                pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.type);
  sysapiPrintf("\nlength.................................%d", 
                pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.length);

  sysapiPrintf("\n\nPIMSM Router Information:");

  sysapiPrintf("\ntraceMode............................  ");
  sysapiPrintf("\nAdminMode............................  ");
  if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode == L7_ENABLE)
  {
     sysapiPrintf("enable");
  }
  else
  {
     sysapiPrintf("disable");
  }
  sysapiPrintf("\ndataThresholdRate....................  %d",
                pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate);
  sysapiPrintf("\nregThresholdRate.....................  %d",
                pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate);

  sysapiPrintf("\n\nStatic RP information:");
  sysapiPrintf("\n---------------------------------------------------------");
  sysapiPrintf("\nRP Address           Group                Group Mask");
  sysapiPrintf("\n--------------------------------------------------------\n");
  for (s = 0; s < L7_MAX_STATIC_RP_NUM; s++)
  {
    inetAddressGet(pimsmMapCbPtr->familyType, 
        &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpIpAddr, 
        &rpIpAddr);
    osapiInetNtoa(rpIpAddr, debug_buf);
    sysapiPrintf("%-15.15s", debug_buf);
              inetAddressGet(pimsmMapCbPtr->familyType, 
              &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpGrpAddr, 
              &rpGrpAddr);
    osapiInetNtoa(rpGrpAddr, debug_buf);
    sysapiPrintf("      %-15.15s", debug_buf);
    inetAddressGet(pimsmMapCbPtr->familyType, 
              &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpGrpMask, 
              &rpGrpMask);
    osapiInetNtoa(rpGrpMask, debug_buf);
    sysapiPrintf("      %-15.15s\n", debug_buf);
  }

  sysapiPrintf("\n\nPIMSM Routing Interfaces:");
  sysapiPrintf("\n------------------------------------------------------------"
                "---------------------");
  sysapiPrintf("\nIntf     IfName     Admin       Hello      Join/Prune   "
                "CBSR     CBSR       CRP");
  sysapiPrintf("\nNum               Mode       Interval   Interval     Pref "
                "   HashMskLen Pref");
  sysapiPrintf("\n------------------------------------------------------------"
                "---------------------");

  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    if ( pimsmMapCbPtr->pPimsmMapCfgMapTbl[i] != 0)
    {
      if (pimsmMapCbPtr->pPimsmMapCfgData->
          intf[ pimsmMapCbPtr->pPimsmMapCfgMapTbl[i]].mode == L7_ENABLE)
      {
          sysapiPrintf("\n%d ", i);

          nimGetIntfName(i, L7_SYSNAME, ifName);
          sysapiPrintf("\t%s", ifName);
          if (pimsmMapCbPtr->pPimsmMapCfgData->intf[ pimsmMapCbPtr->
              pPimsmMapCfgMapTbl[i]].mode == L7_ENABLE)
          {
             sysapiPrintf("         E");
          }
          else
          {
             sysapiPrintf("         D");
          }
          sysapiPrintf("         %d", 
            pimsmMapCbPtr->pPimsmMapCfgData->intf[ pimsmMapCbPtr->
            pPimsmMapCfgMapTbl[i]].intfHelloInterval);
          sysapiPrintf("         %d", 
            pimsmMapCbPtr->pPimsmMapCfgData->intf[ pimsmMapCbPtr->
            pPimsmMapCfgMapTbl[i]].intfJoinPruneInterval);
          sysapiPrintf("         %d", 
            pimsmMapCbPtr->pPimsmMapCfgData->intf[ pimsmMapCbPtr->
            pPimsmMapCfgMapTbl[i]].bsrBorderMode);
          sysapiPrintf("         %d", 
            pimsmMapCbPtr->pPimsmMapCfgData->intf[ pimsmMapCbPtr->
            pPimsmMapCfgMapTbl[i]].drPriority);
          
          
      }
    }
  }


  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n|              PIMSM INFORMATION STRUCTURES               |");
  sysapiPrintf("\n+--------------------------------------------------------+");

  sysapiPrintf("\n\npPimsmInfo structure:");

  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
  {
     sysapiPrintf("\n\tpimsmOperational........true");
  }
  else
  {
     sysapiPrintf("\n\tpimsmOperational........false");
  }

  sysapiPrintf("\n\nStatic RP information:");
  sysapiPrintf("\n---------------------------------------------------------");
  sysapiPrintf("\nRP Address           operational");
  sysapiPrintf("\n--------------------------------------------------------\n");
  for (s = 0; s < L7_MAX_STATIC_RP_NUM; s++)
  {
    inetAddressGet(pimsmMapCbPtr->familyType, 
        &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpIpAddr, 
        &rpIpAddr);
    osapiInetNtoa(rpIpAddr, debug_buf);
    sysapiPrintf("%-15.15s", debug_buf);
    if (pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[s] == L7_TRUE)
    {
       sysapiPrintf("\t\ttrue\n");
    }
    else
    {
       sysapiPrintf("\t\tfalse\n");
    }
  }
  
  sysapiPrintf("\n---------------------------\n");

  sysapiPrintf("\n\npPimsmIntfInfo structure:");
  sysapiPrintf("\n-----------------------------");
  sysapiPrintf("\nIntIfNum    IfName        pimsmOperational");
  sysapiPrintf("\n------------------------------------------");

  for (i=1; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    sysapiPrintf("\n%d  ", i);

    if (nimGetIntfName(i, L7_SYSNAME, ifName) != L7_SUCCESS)
      osapiSnprintf(ifName, sizeof(ifName), "n/a");
    sysapiPrintf("\t%s", ifName);
    if (pimsmMapCbPtr->pPimsmIntfInfo[i].pimsmOperational == L7_TRUE)
    {
       sysapiPrintf("\t\ttrue");
    }
    else
    {
       sysapiPrintf("\t\tfalse");
    }
  }

  sysapiPrintf("\n-----------------------------\n");




  sysapiPrintf("Scaling Constants\n");
  sysapiPrintf("-----------------\n");

                                      
  sysapiPrintf("L7_IPMAP_INTF_MAX_COUNT - %d\n", L7_IPMAP_INTF_MAX_COUNT);
  sysapiPrintf("L7_MAX_STATIC_RP_NUM - %d\n", L7_MAX_STATIC_RP_NUM);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    familyType  @b{(input)} Address Family type
*
* @returns  void
*
* @comments This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void pimsmMapBuildTestConfigData(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
   L7_uint32 i, rpIpAddr, rpGrpAddr, rpGrpMask;

   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
    /* generic PIM-SM cfg */
    pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode = L7_ENABLE;
    /*pimsmMapCbPtr->pPimsmMapCfgData->rtr.joinPruneInterval = 77;*/
    pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate = 88;
    pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate = 99;
    for (i = 0; i < L7_MAX_STATIC_RP_NUM; i++)
    {
      rpIpAddr = 0x0A0A0A01 + i;
      inetAddressSet(familyType, &rpIpAddr, &pimsmMapCbPtr->
                     pPimsmMapCfgData->rtr.pimsmStaticRP[i].rpIpAddr);
      rpGrpAddr = 0xFA000000;
      inetAddressSet(familyType, &rpGrpAddr, &pimsmMapCbPtr->
                     pPimsmMapCfgData->rtr.pimsmStaticRP[i].rpGrpAddr);
      rpGrpMask = 0xFFFF0000;
      inetAddressSet(familyType, &rpGrpMask, &pimsmMapCbPtr->
                     pPimsmMapCfgData->rtr.pimsmStaticRP[i].rpGrpMask);
    }

    /* PIM router interface configuration parameters */
    for (i=1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
    {
      pimsmIntfBuildTestConfigData(&(pimsmMapCbPtr->
                                     pPimsmMapCfgData->intf[i]));
    }
       

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}

/*********************************************************************
* @purpose  Build default PIMSM config data for an intf 
*
* @param    pCfg   @b{(input)} pointer to the DVMRP ckt configuration record
*
* @returns  void
*
* @comments none 
*
* @end
*********************************************************************/
void pimsmIntfBuildTestConfigData(pimsmCfgCkt_t *pCfg)
{

    L7_uint32 intIfNum;

    /* Ensure at least one entry in each array has a unique value to validate
       accurate migration */
    
    if (nimIntIfFromConfigIDGet(&pCfg->configId, &intIfNum) != L7_SUCCESS)
    {
      intIfNum = 0xFF;
    }

    pCfg->mode                  = L7_ENABLE;
    pCfg->intfHelloInterval     = 11 + intIfNum;
    pCfg->intfJoinPruneInterval = 12;

}



/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    familyType  @b{(input)} Address Family type
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void pimsmMapConfigDataTestShow(L7_uchar8 familyType)
{
    pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
    L7_fileHdr_t  *pFileHdr;

    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
      return;
    }

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/
    if (familyType == L7_AF_INET)
    {
      pimsmMapCfgDataShow();
    }
    else
    {
      pimsm6MapCfgDataShow();
    }


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("pPimdmMapCfgData->checkSum : %u\n", 
                  pimsmMapCbPtr->pPimsmMapCfgData->checksum);


}

/******************************************************************************
* @purpose Display the cand-RP list
*
* @param        none
*
* @returns     void
*
* @comments    none
*
* @end
******************************************************************************/
void pimsmMapDebugCandRPShow(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
   L7_uint32 s;
   L7_uchar8 debug_buf[256];
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }
  sysapiPrintf("\n\nCand-RP information:");
  sysapiPrintf("\n------------------------------------------------------------------------------");
  sysapiPrintf("\nRP Address           Group                Group Length      IntIfNum  OperMode   emptyflag");
  sysapiPrintf("\n--------------------------------------------------------------------------------------------\n");
  for (s = 0; s < L7_MAX_CAND_RP_NUM; s++)
  {
    inetAddrHtop(&pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[s].candRPAddress,
       debug_buf);
    sysapiPrintf("\n%-15.15s", debug_buf);
    inetAddrHtop(&pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[s].grpAddress,
      debug_buf);
    sysapiPrintf("      %-15.15s", debug_buf);
    sysapiPrintf("\t%d", 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[s].grpPrefixLength);
    sysapiPrintf("\t%d", 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[s].intIfNum);
    if (pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[s].operMode 
            == L7_TRUE)
    {
       sysapiPrintf("\ttrue");
    }
    else
    {
       sysapiPrintf("\tfalse");
    }   
  } 
}
/******************************************************************************
* @purpose  Display the static-RP list
*
* @param        none
*
* @returns  void
*
* @comments  none
*
* @end
******************************************************************************/
void pimsmMapDebugStaticRPShow(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
   L7_uint32 s;
   L7_uchar8 debug_buf[256];
   
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }
  sysapiPrintf("\n\nStatic RP information:");
  sysapiPrintf("\n---------------------------------------------------------------------------");
  sysapiPrintf("\nRP Address           Group                Group Mask     Override  OperMode");
  sysapiPrintf("\n---------------------------------------------------------------------------\n");
  for (s = 0; s < L7_MAX_STATIC_RP_NUM; s++)
  {
    inetAddrHtop(&pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpIpAddr,
      debug_buf);
    sysapiPrintf("%-15.15s", debug_buf);
    inetAddrHtop(&pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpGrpAddr,
      debug_buf);
    sysapiPrintf("%      -15.15s", debug_buf);
    inetAddrHtop(&pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].rpGrpMask,
      debug_buf);
    sysapiPrintf("\t      %-15.15s", debug_buf);
    if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[s].overRide == L7_TRUE)
    {
       sysapiPrintf("\ttrue");
    }
    else
    {
       sysapiPrintf("\tfalse");
    }
    if (pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[s]  == L7_TRUE)
    {
       sysapiPrintf("\ttrue\n");
    }
    else
    {
       sysapiPrintf("\tfalse\n");
    }
  }
}
/******************************************************************************
* @purpose Display the cand-BSR 
*
* @param        none
*
* @returns void
*
* @comments
*
* @end
******************************************************************************/
void pimsmMapDebugCandBsrShow(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
   L7_uchar8 debug_buf[256];
   
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }
  sysapiPrintf("\n\nCand-BSR information:");
  sysapiPrintf("\n------------------------------------------------------------------------------");
  sysapiPrintf("\nBSR Address       Priority   HashMaskLength   Scope   IntIfNum   OperMode");
  sysapiPrintf("\n-------------------------------------------------------------------------------\n");
    inetAddrHtop(&pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress, debug_buf);
    sysapiPrintf("%-15.15s", debug_buf);
    sysapiPrintf("\t%d", 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.candBSRPriority);    
    sysapiPrintf("\t%d", 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.candBSRHashMaskLength);
    sysapiPrintf("\t\t%d", 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.candBSRScope);
    sysapiPrintf("\t%d", 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.intIfNum);
    if (pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode 
            == L7_TRUE)
    {
       sysapiPrintf("\t\ttrue\n");
    }
    else
    {
       sysapiPrintf("\t\tfalse\n");
    }    

}

/*******************************************************************************************
*
*
*********************************************************************************************/
void pimsmMapDebugSSMTableShow(L7_uchar8 familyType)

{

 pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
 L7_uint32 s;
 L7_uchar8 debug_buf[256]; 

  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }

  sysapiPrintf("\n\nSSM configuration:");
  sysapiPrintf("\n------------------------------------------------------------------------------");
  sysapiPrintf("\nGRP Address       Prefix-length   SSM-mode  slot-Empty  ");
  sysapiPrintf("\n-------------------------------------------------------------------------------\n");
 for(s=0;s<L7_MAX_SSM_RANGE;s++)
 {
  inetAddrHtop(&pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[s].groupAddr,
    debug_buf);
  sysapiPrintf("%-15.15s", debug_buf);
  sysapiPrintf("\t %d",pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[s].prefixLen);
  sysapiPrintf("\t %d",pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[s].ssmMode);  
  
   if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[s] == L7_FALSE)
    {
     sysapiPrintf("\tFalse\n");
    }
    else
    {
     sysapiPrintf("\tTrue\n");
    }
 } 
}
 
/******************************************************************************
* @purpose Display the Control Block of PIM-SM Mapping Layer 
*
* @param        none
*
* @returns void
*
* @comments
*
* @end
******************************************************************************/
void pimsmMapCBShow(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;

   if ((familyType != L7_AF_INET) && (familyType != L7_AF_INET6))
   {
     sysapiPrintf ("Invalid Family Type specified - %d.\n", familyType);
     return;
   }

   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     sysapiPrintf ("Failed to get PIM-SM MAP Control Block.\n");
     return;
   }

   sysapiPrintf ("**********************************************\n");
   sysapiPrintf ("  Family Type - %d.\n", pimsmMapCbPtr->familyType);
   sysapiPrintf ("  warmRestartInProgres - %s.\n", pimsmMapCbPtr->warmRestartInProgress?"L7_TRUE":"L7_FALSE");
   sysapiPrintf ("  cbHandle           - %p.\n", pimsmMapCbPtr->cbHandle);
   sysapiPrintf ("  pPimsmMapCfgData   - %p.\n", pimsmMapCbPtr->pPimsmMapCfgData);
   sysapiPrintf ("  pPimsmMapCfgMapTbl - %p.\n", pimsmMapCbPtr->pPimsmMapCfgMapTbl);
   sysapiPrintf ("  pPimsmInfo         - %p.\n", pimsmMapCbPtr->pPimsmInfo);
   sysapiPrintf ("  pPimsmIntfInfo     - %p.\n", pimsmMapCbPtr->pPimsmIntfInfo);
   sysapiPrintf ("  cfgFilename        - %s.\n", pimsmMapCbPtr->cfgFilename);
   
   sysapiPrintf ("  gblVars     - %p.\n", pimsmMapCbPtr->gblVars);
   sysapiPrintf ("     msgQSema             - %p.\n", pimsmMapCbPtr->gblVars->msgQSema);
   sysapiPrintf ("     pimsmMapTaskId       - %d.\n", pimsmMapCbPtr->gblVars->pimsmMapTaskId);
   sysapiPrintf ("     pimsmCnfgrState      - %d.\n", pimsmMapCbPtr->gblVars->pimsmCnfgrState);
   sysapiPrintf ("     pimsmCnfgrState      - %d.\n", pimsmMapCbPtr->gblVars->pimsmCnfgrState);
   sysapiPrintf ("     warmRestart          - %s.\n", pimsmMapCbPtr->gblVars->warmRestart?"L7_TRUE":"L7_FALSE");
   sysapiPrintf ("**********************************************\n");
   return;
}



/*********************************************************************
*
* @purpose  Shows the PIMSMMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void pimsmMapDebugQueueStatsShow(void)
{
  L7_uint32 qIndex;
  L7_uchar8 qName[20];

  sysapiPrintf(" QueueName  CurMsgCnt HighWaterMark  SendFailCnt RxSuccessCnt MaxQSize MsgSize \n");
  sysapiPrintf ("\n----------------------------------------------------------------------------\n");
  for (qIndex = PIMSM_APP_TIMER_Q; qIndex < PIMSM_MAX_Q; qIndex++ )
  {
    pimsmMapQueue_t   *pimsmMapQueue = &pimsmGblVariables_g.pimsmQueue[qIndex];
    osapiStrncpy(qName, (pimsmMapQueue->QName) + 8, 20);
    sysapiPrintf("\n %-15s  %-10d %-10d %-10d %-10d %-7d %-7d  ",
                 qName, pimsmMapQueue->QCurrentMsgCnt, pimsmMapQueue->QMaxRx,
                 pimsmMapQueue->QSendFailedCnt, pimsmMapQueue->QRxSuccess,
                 pimsmMapQueue->QCount , pimsmMapQueue->QSize);
        
  }
  sysapiPrintf("\n\n");
  sysapiPrintf("No of MFC events in the Control pkt Queue : %d\n",
               pimsmMapCountersValueGet (PIMSM_MAP_NUM_MFC_EVENTS));
  sysapiPrintf("No of MGMD events in the Events Queue : %d\n",
               pimsmMapCountersValueGet (PIMSM_MAP_NUM_MGMD_EVENTS));
  sysapiPrintf("No of MFC overflows in the Control pkt Queue : %d\n",
               pimsmMapCountersValueGet (PIMSM_MAP_NUM_MFC_OVERFLOW_EVENTS));
  sysapiPrintf("No of MGMD overflows in the Events Queue : %d\n",
               pimsmMapCountersValueGet (PIMSM_MAP_NUM_MGMD_OVERFLOW_EVENTS));
  sysapiPrintf("\n");
}


/*********************************************************************
*
* @purpose  Clears the PIMSMMAP queue stats.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/

void pimsmMapDebugQueueStatsClear(void)
{
  L7_uint32 QIndex=0;

  if (osapiSemaTake (pimsmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    sysapiPrintf ("queueCountersSemId - Acquire Failed.\n");
    return;
  }

  for (QIndex = 0; QIndex < PIMSM_MAX_Q; QIndex++)
  {
    pimsmGblVariables_g.pimsmQueue[QIndex].QCurrentMsgCnt = 0;
    pimsmGblVariables_g.pimsmQueue[QIndex].QSendFailedCnt = 0;
    pimsmGblVariables_g.pimsmQueue[QIndex].QMaxRx = 0;
    pimsmGblVariables_g.pimsmQueue[QIndex].QRxSuccess = 0;
  }

  osapiSemaGive (pimsmGblVariables_g.queueCountersSemId);

  pimsmMapCountersUpdate (PIMSM_MAP_NUM_MFC_EVENTS, PIMSM_MAP_COUNTERS_RESET);
  pimsmMapCountersUpdate (PIMSM_MAP_NUM_MGMD_EVENTS, PIMSM_MAP_COUNTERS_RESET);
  pimsmMapCountersUpdate (PIMSM_MAP_NUM_MFC_OVERFLOW_EVENTS, PIMSM_MAP_COUNTERS_RESET);
  pimsmMapCountersUpdate (PIMSM_MAP_NUM_MGMD_OVERFLOW_EVENTS, PIMSM_MAP_COUNTERS_RESET); 
}

/******************************************************************************
* @purpose Display the cand-RP list
*
* @param        none
*
* @returns     void
*
* @comments    none
*
* @end
******************************************************************************/
void pimsmMapDebugRPCandidateEntryShow(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
   L7_uchar8 debug_buf[256];
   L7_inet_addr_t candRpAddress;
   L7_inet_addr_t candGroupAddress; 
   L7_uchar8 candPrefixLen;
   L7_RC_t rc;

   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }
  sysapiPrintf("\n\nCand-RP information:");
  sysapiPrintf("\n----------------------------------------------------------------");
  sysapiPrintf("\nRP Address           Group                Group Length  ");
  sysapiPrintf("\n---------------------------------------------------------------\n");

  inetAddressZeroSet(familyType, &candRpAddress);
  inetAddressZeroSet(familyType, &candGroupAddress);
  candPrefixLen = 0;
  rc = pimsmMapCandRPEntryNextGet(familyType, &candRpAddress, 
                                       &candGroupAddress, &candPrefixLen);
  while (rc == L7_SUCCESS)
  {
    inetAddrHtop(&candRpAddress,
       debug_buf);
    sysapiPrintf("%-15.15s", debug_buf);
    inetAddrHtop(&candGroupAddress,
      debug_buf);
    sysapiPrintf("      %-15.15s", debug_buf);
    sysapiPrintf("\t%d", 
      candPrefixLen);
    rc = pimsmMapCandRPEntryNextGet(familyType, &candRpAddress, 
                                         &candGroupAddress, &candPrefixLen);

  }

}




/******************************************************************************
* @purpose Display the cand-RP list
*
* @param        none
*
* @returns     void
*
* @comments    none
*
* @end
******************************************************************************/
void pimsmMapDebugStaticRPEntryShow(L7_uchar8 familyType)
{
   pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
   L7_uchar8 debug_buf[256];
   L7_inet_addr_t staticGroupAddress; 
   L7_uchar8 staticPrefixLen;
   L7_RC_t rc;

   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Control Block");
     return;
   }
  sysapiPrintf("\n\nStatic-RP information:");
  sysapiPrintf("\n----------------------------------------------------------------");
  sysapiPrintf("\n       Group                Group Length  ");
  sysapiPrintf("\n---------------------------------------------------------------\n");

  inetAddressZeroSet(familyType, &staticGroupAddress);
  staticPrefixLen = 0;
  rc = pimsmMapStaticRPNextGet(familyType, &staticGroupAddress, &staticPrefixLen);
  while (rc == L7_SUCCESS)
  {
    inetAddrHtop(&staticGroupAddress,
      debug_buf);
    sysapiPrintf("      %-15.15s", debug_buf);
    sysapiPrintf("\t%d", 
      staticPrefixLen);
    rc = pimsmMapStaticRPNextGet(familyType, &staticGroupAddress, &staticPrefixLen);

  }

}


/*============================================================================*/
/*===================  START OF PIMSM PACKET DEBUG TRACES =====================*/
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
void pimsmDebugPacketRxTxTrace(L7_uchar8 family, L7_BOOL rxTrace,
                              L7_uint32 rtrIfNum, L7_inet_addr_t *src,
                              L7_inet_addr_t *dest, L7_uchar8 *payLoad, 
                              L7_uint32 length)
{
  L7_uchar8 srcBuff[IPV6_DISP_ADDR_LEN], destBuff[IPV6_DISP_ADDR_LEN];
  L7_uchar8 str[PIMSM_PKT_TYPE_STR_LEN], pktType;
  L7_BOOL rxFlag = L7_FALSE, txFlag = L7_FALSE;
  L7_uint32 intIfNum = 0;
  nimUSP_t usp;
  L7_uchar8 version;

  pimsmDebugTraceFlagGet(family, PIMSM_DEBUG_PACKET_RX_TRACE, &rxFlag);
  pimsmDebugTraceFlagGet(family, PIMSM_DEBUG_PACKET_TX_TRACE, &txFlag);

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
    osapiStrncpySafe(str, "Hello ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_REGISTER:
    osapiStrncpySafe(str, "Register ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case  PIM_PDU_REGISTER_STOP:
    osapiStrncpySafe(str, "Register-Stop ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_JOIN_PRUNE:
    osapiStrncpySafe(str, "Join-Prune ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_BOOTSTRAP: 
    osapiStrncpySafe(str, "BootStrap ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_ASSERT:  
    osapiStrncpySafe(str, "Assert ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_GRAFT:
    osapiStrncpySafe(str, "Graft ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_GRAFT_ACK:
    osapiStrncpySafe(str, "Graft-Ack ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_CANDIDATE_RP_ADVERTISEMENT:
    osapiStrncpySafe(str, "Candidate-RP Adv ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  case PIM_PDU_STATE_REFRESH:
    osapiStrncpySafe(str, "State-Refresh ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  default:
    osapiStrncpySafe(str, "Unknown PIM Pkt type ", PIMSM_PKT_TYPE_STR_LEN);
    break;
  }

  if (rxTrace == L7_TRUE)
  {
    PIMSM_USER_TRACE("Received PIMv%d %s with Src %s and Destn %s on interface %d/%d/%d of len = %d",
                    version, str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                    usp.unit, usp.slot, usp.port, length);
  }
  else
  {
    PIMSM_USER_TRACE("Sent PIMv%d %s with Src %s and Destn %s on interface %d/%d/%d of len = %d",
                    version, str, inetAddrPrint(src, srcBuff), inetAddrPrint(dest, destBuff),
                    usp.unit, usp.slot, usp.port, length);
  }
  return;
}


/*********************************************************************
* @purpose  Get the current status of displaying pimsm packet debug info
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
L7_RC_t pimsmDebugTraceFlagGet(L7_uchar8 family,
                              PIMSM_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL *flag)
{
  pimsmDebugTraceFlags_t *pimsmDebugTraceFlags ;

  if (family == L7_AF_INET)
  {
    pimsmDebugTraceFlags = &pimsmDebugTraceFlag[PIMSM_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    pimsmDebugTraceFlags = &pimsmDebugTraceFlag[PIMSM_MAP_IPV6_CB];
  }
  else
  {
    return L7_FAILURE;
  }

  if (((*(pimsmDebugTraceFlags[traceFlag/PIMSM_DEBUG_TRACE_FLAG_BITS_MAX])) & 
       (PIMSM_DEBUG_TRACE_FLAG_VALUE << (traceFlag % PIMSM_DEBUG_TRACE_FLAG_BITS_MAX))) != L7_NULL)
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
L7_RC_t pimsmDebugTraceFlagSet(L7_uchar8 family, 
                              PIMSM_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL flag)
{
  pimsmDebugTraceFlags_t *pimsmDebugTraceFlags ;

  if (family == L7_AF_INET)
  {
    pimsmDebugTraceFlags = &pimsmDebugTraceFlag[PIMSM_MAP_IPV4_CB];
  }
  else if (family == L7_AF_INET6)
  {
    pimsmDebugTraceFlags = &pimsmDebugTraceFlag[PIMSM_MAP_IPV6_CB];
  }
  else
  {
    sysapiPrintf ("\nInvalid Address Family - %d.\n", family);
    return L7_FAILURE;
  }

   if (flag == L7_TRUE)
   {
     (*(pimsmDebugTraceFlags[traceFlag/PIMSM_DEBUG_TRACE_FLAG_BITS_MAX])) |= 
               (PIMSM_DEBUG_TRACE_FLAG_VALUE << (traceFlag % PIMSM_DEBUG_TRACE_FLAG_BITS_MAX));
   }
   else
   {
     (*(pimsmDebugTraceFlags[traceFlag/PIMSM_DEBUG_TRACE_FLAG_BITS_MAX])) &= 
            (~(PIMSM_DEBUG_TRACE_FLAG_VALUE << (traceFlag % PIMSM_DEBUG_TRACE_FLAG_BITS_MAX)));
   }

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a 
     debug config save.
    */ 

  pimsmDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Save configuration settings for pimsm trace data
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
L7_RC_t pimsmDebugTraceFlagSave()
{
  memcpy(pimsmDebugCfg.cfg.pimsmDebugTraceFlag, pimsmDebugTraceFlag, 
         sizeof(pimsmDebugCfg.cfg.pimsmDebugTraceFlag));
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
void pimsmDebugCfgUpdate(void)
{ 
  pimsmDebugTraceFlagSave();
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
void pimsmDebugCfgRead(void)
{ 
      /* reset the debug flags*/
    memset((void*)&pimsmDebugCfg, 0 ,sizeof(pimsmDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_FLEX_PIMSM_MAP_COMPONENT_ID, PIMSM_DEBUG_CFG_FILENAME, 
                         (L7_char8 *)&pimsmDebugCfg, (L7_uint32)sizeof(pimsmDebugCfg_t), 
                         &pimsmDebugCfg.checkSum, PIMSM_DEBUG_CFG_VER_CURRENT, 
                         pimsmDebugBuildDefaultConfigData, L7_NULL);

    pimsmDebugCfg.hdr.dataChanged = L7_FALSE;

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
void pimsmDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_PIMSM_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = pimsmDebugSave;
    supportDebugDescr.userControl.hasDataChanged = pimsmDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = pimsmDebugRestore;

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
void pimsmDebugDeRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_FLEX_PIMSM_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = pimsmDebugSave;
    supportDebugDescr.userControl.hasDataChanged = pimsmDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = pimsmDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */
    (void) supportDebugDeregister(supportDebugDescr);
}



void printPimsmDebug(L7_uchar8 family)
{
  L7_uint32 flagIndex;

  sysapiPrintf("\n-----------------\n");
  sysapiPrintf("\n version = %d", pimsmDebugCfg.hdr.version);
  sysapiPrintf("\n compId = %d", pimsmDebugCfg.hdr.componentID);
  sysapiPrintf("\n type = %d", pimsmDebugCfg.hdr.type);
  sysapiPrintf("\n length = %d", pimsmDebugCfg.hdr.length);
  sysapiPrintf("\n fileName = %s", pimsmDebugCfg.hdr.filename);
  sysapiPrintf("\n dataChng = %d", pimsmDebugCfg.hdr.dataChanged);
  sysapiPrintf("\n-----------------\n");

  pimsmDebugTraceFlags_t *pimsmTraceFlags = &(pimsmDebugCfg.cfg.pimsmDebugTraceFlag[family]);
  for (flagIndex = 0;  flagIndex < PIMSM_DEBUG_LAST_TRACE ; flagIndex ++)
  {
    if (((*(pimsmTraceFlags[flagIndex/PIMSM_DEBUG_TRACE_FLAG_BITS_MAX])) & 
         (PIMSM_DEBUG_TRACE_FLAG_VALUE << (flagIndex % PIMSM_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
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
* @purpose  Compute the memory allocated by the PIMSM Mapping layer
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void pimsmMapDebugMemoryInfoCompute(void)
{
  pimsmMapCB_t *pimsmMapCB = L7_NULLPTR;
  L7_uchar8 addrFamily = 0;
  L7_BOOL v6CBPresent = L7_FALSE;

  addrFamily = L7_AF_INET6;
  if (pimsmMapCtrlBlockGet(addrFamily, &pimsmMapCB) == L7_SUCCESS)
  {
    v6CBPresent = L7_TRUE;
  }

  sysapiPrintf ("\n***** Memory Allocated By PIMSM Mapping Layer *****\n");
  sysapiPrintf ("        Global Variables\n");
  sysapiPrintf ("          pimsmGblVariables_t - %d\n", sizeof(pimsmGblVariables_t));
  pimsmMapTotalBytesAllocated += sizeof(pimsmGblVariables_t);
  sysapiPrintf ("          pimsmDebugCfg_t     - %d\n", sizeof(pimsmDebugCfg_t));
  pimsmMapTotalBytesAllocated += sizeof(pimsmDebugCfg_t);
  sysapiPrintf ("          pimsmCfgDefs_t      - %d\n", sizeof(pimsmCfgDefs_t));
  pimsmMapTotalBytesAllocated += sizeof(pimsmCfgDefs_t);
  sysapiPrintf ("        Message Queues\n");
  sysapiPrintf ("          App Timer Queue     - Size[%d]*Num[%d] = %d\n", PIMSMMAP_APP_TMR_MSG_SIZE, PIMSMMAP_APP_TMR_MSG_COUNT, (PIMSMMAP_APP_TMR_MSG_SIZE*PIMSMMAP_APP_TMR_MSG_COUNT));
  pimsmMapTotalBytesAllocated += (PIMSMMAP_APP_TMR_MSG_SIZE*PIMSMMAP_APP_TMR_MSG_COUNT);
  sysapiPrintf ("          Events Queue        - Size[%d]*Num[%d] = %d\n", PIMSMMAP_MSG_SIZE, PIMSMMAP_MSG_COUNT, (PIMSMMAP_MSG_SIZE*PIMSMMAP_MSG_COUNT));
  pimsmMapTotalBytesAllocated += (PIMSMMAP_MSG_SIZE*PIMSMMAP_MSG_COUNT);
  sysapiPrintf ("          Ctrl Pkt Queue      - Size[%d]*Num[%d] = %d\n", PIMSMMAP_CTRL_PKT_MSG_SIZE, PIMSMMAP_CTRL_PKT_MSG_COUNT, (PIMSMMAP_CTRL_PKT_MSG_SIZE*PIMSMMAP_CTRL_PKT_MSG_COUNT));
  pimsmMapTotalBytesAllocated += (PIMSMMAP_CTRL_PKT_MSG_SIZE*PIMSMMAP_CTRL_PKT_MSG_COUNT);
  sysapiPrintf ("          Data Pkt Queue      - Size[%d]*Num[%d] = %d\n", PIMSMMAP_DATA_PKT_MSG_SIZE, PIMSMMAP_DATA_PKT_MSG_COUNT, (PIMSMMAP_DATA_PKT_MSG_SIZE*PIMSMMAP_DATA_PKT_MSG_COUNT));
  pimsmMapTotalBytesAllocated += (PIMSMMAP_DATA_PKT_MSG_SIZE*PIMSMMAP_DATA_PKT_MSG_COUNT);
  sysapiPrintf ("        Control Block IPv4    - %d\n", sizeof(pimsmMapCB_t));
  pimsmMapTotalBytesAllocated += sizeof(pimsmMapCB_t);
  if (v6CBPresent == L7_TRUE)
  {
    sysapiPrintf ("        Control Block IPv6    - %d\n", sizeof(pimsmMapCB_t));
    pimsmMapTotalBytesAllocated += sizeof(pimsmMapCB_t);
  }
  sysapiPrintf ("\n");
  sysapiPrintf ("        Total Memory          - %d\n", pimsmMapTotalBytesAllocated);
  sysapiPrintf ("\n");
  sysapiPrintf ("***************************************************\n");

  return;
}

