/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_util.c
*
* @purpose NIM utilities
*
* @component nim
*
* @comments none
*
* @create 08/30/2000
*
* @author mbaucom
*
* @end
*             
**********************************************************************/

#include "flex.h"
#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"
#include "usmdb_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nim_data.h"
#include "nim_util.h"
#include "nim_config.h"
#include "nim_outcalls.h"
#include "nim_counters.h"
#include "nim_ifindex.h"
#include "registry.h"
#include "defaultconfig.h"
#include "l7_packet.h"
#include "cardmgr_api.h"
#include "dtlapi.h"
#include "usmdb_counters_api.h"

#include "platform_config.h"
#include <string.h>
#include "unitmgr_api.h"
#include <stdarg.h>
#include "nim_exports.h"

/* local function Prototypes */
L7_RC_t nimMaskFirstUnusedIntIfNumGet(NIM_INTF_MASK_t mask,L7_uint32 *intIfNum); 

/* TBD: Need to move to global data structure */
static L7_BOOL nimConfigIdTreePopulatationComplete = L7_FALSE;

static L7_char8 *pLogMsgFmtError = "(Unable to format log message)";

/*********************************************************************
* @purpose  Log NIM Message 
*
* @param     fileName - file
* @param     lineNum - Line
* @param     format - Format of the output.
* @param     ... - Variable output list.
*
* @returns  the phase number
*
* @notes    
*       
* @end
*********************************************************************/
void nimLogMsg (L7_BOOL logError, L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * format, ...)
{
  L7_uchar8 nim_log_buf[LOG_MSG_MAX_MSG_SIZE];
  va_list ap;
  L7_int32 rc;

  va_start(ap, format);
  rc = osapiVsnprintf(nim_log_buf, sizeof (nim_log_buf), format, ap);
  va_end(ap);

  if ((nimCtlBlk_g->nimPrintLog == L7_TRUE) ||
      (logError == L7_TRUE))
  {
    if (rc < 0)
    {
      nim_log_msg_fmt(fileName,lineNum,pLogMsgFmtError);
    } else
    {
      nim_log_msg_fmt(fileName,lineNum,nim_log_buf);
    }
  }
  l7_logf(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID, fileName, lineNum, nim_log_buf);
}

/*********************************************************************
* @purpose  Trace NIM Message 
*
* @param     fileName - file
* @param     lineNum - Line
* @param     format - Format of the output.
* @param     ... - Variable output list.
*
* @returns  the phase number
*
* @notes    
*       
* @end
*********************************************************************/
void nimTraceMsg (L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * format, ...)
{
  L7_uchar8 nim_log_buf[LOG_MSG_MAX_MSG_SIZE];
  va_list ap;
  L7_int32 rc;

  va_start(ap, format);
  rc = osapiVsnprintf(nim_log_buf, sizeof (nim_log_buf), format, ap);
  va_end(ap);

  if (nimCtlBlk_g->nimPrintLog == L7_TRUE)
  {
    if (rc < 0)
    {
      nim_log_msg_fmt(fileName, lineNum, pLogMsgFmtError);
    } else
    {
      nim_log_msg_fmt(fileName, lineNum, nim_log_buf);
    }
  }
  l7_logf(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID, fileName, lineNum, nim_log_buf);
}

/*********************************************************************
* @purpose  return the configuration phase
*
* @param     none
*
* @returns  the phase number
*
* @notes    
*       
* @end
*********************************************************************/
L7_CNFGR_STATE_t nimConfigPhaseGet(void)
{
  return(nimCtlBlk_g->nimPhaseStatus );
}

/*********************************************************************
* @purpose  Determine whether NIM is in a state ready to process
*		    interface requests.
*
* @param     none
*
* @returns  L7_TRUE - Nim is ready.
*		    L7_FALSE - Nim is not ready.
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL nimPhaseStatusCheck(void)
{
  if ((nimCtlBlk_g->nimPhaseStatus == L7_CNFGR_STATE_E) ||
      (nimCtlBlk_g->nimPhaseStatus == L7_CNFGR_STATE_P3) ||
      (nimCtlBlk_g->nimPhaseStatus == L7_CNFGR_STATE_U1))
  {
    return L7_TRUE;
  }


  return L7_FALSE;
}

/*********************************************************************
* @purpose  delete a unit slot port mapping to interface number
*
* @param    usp         Pointer to nimUSP_t structure
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    This function is not re-entrant, the caller must ensure integrety of data 
*       
* @end
*********************************************************************/
L7_RC_t nimUnitSlotPortToIntfNumClear(nimUSP_t *usp)
{
  L7_RC_t rc = L7_SUCCESS;
  nimUspIntIfNumTreeData_t data;
  nimUspIntIfNumTreeData_t *pData;

  data.usp = *usp;

  pData = avlDeleteEntry(&nimCtlBlk_g->nimUspTreeData, &data);

  if (pData == L7_NULL)
  {
    NIM_LOG_MSG("NIM: %d.%d.%d not found, cannot delete it\n",usp->unit,usp->slot,usp->port);
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  create a unit slot port mapping to interface number
*
* @param    usp         Pointer to nimUSP_t structure
* @param    intIntfNum    interface number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    This function is not re-entrant, the caller must ensure integrety of data 
*       
* @end
*********************************************************************/
L7_RC_t nimUnitSlotPortToIntfNumSet(nimUSP_t *usp, L7_uint32 intIntfNum)
{
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   unit = 0, slot = 0, port = 0;
  L7_INTF_TYPES_t sysIntfType;
  nimUspIntIfNumTreeData_t data;
  nimUspIntIfNumTreeData_t *pData;

  /* check the usp */
  if (usp != L7_NULL)
  {
    unit = usp->unit;
    slot = usp->slot;
    port = usp->port;
  }
  else
  {
    rc = L7_FAILURE;
  }

  if ((rc != L7_SUCCESS) || (nimPhaseStatusCheck () != L7_TRUE))
  {
    rc = L7_FAILURE;
  }
  else if ((unit > nimCtlBlk_g->maxNumOfUnits) ||
           (slot > nimCtlBlk_g->maxNumOfSlotsPerUnit) || 
           (port == 0))
  {
    NIM_LOG_MSG("NIM: out of bounds usp used U=%d, S=%d P=%d\n",unit,slot,port); 
    rc = L7_FAILURE;
  }
  else if ( (nimGetIntfTypeFromUSP(usp, &sysIntfType) != L7_SUCCESS) ||
            ( nimNumberOfInterfaceExceeded(sysIntfType) == L7_TRUE))

  {
    /* The port number should not exceed the maximum number of interfaces for the type */
    NIM_LOG_MSG("NIM: out of bounds usp used U=%d, S=%d P=%d - port too large\n",unit,slot,port); 
    rc = L7_FAILURE;
  }

  else
  {
    data.intIfNum = intIntfNum;
    data.usp  = *usp;

    pData = avlInsertEntry(&nimCtlBlk_g->nimUspTreeData, &data);

    if (pData != L7_NULLPTR)
    {
      NIM_LOG_MSG("NIM: Usp to intIfNum not added for intIfNum %d\n",intIntfNum);
      rc = L7_FAILURE;
    }
  } 

  return(rc);
}


/*********************************************************************
* @purpose  Calculates the length in bytes of the largest payload given
*           the max frame length and encapsulation configured on this
*           interface. If the length changes from what was previously 
*           calculated, send a L7_SET_MTU_SIZE event.
*
* @param    intIfNum    Internal Interface Number
*
* @returns  MTU size of the interface
*
* @notes    Should be called whenever the max frame length or encapsulation
*           type changes on an interface. 
*
* @end
*********************************************************************/
L7_uint32 nimCalcMtuSize(L7_uint32 intIfNum)
{
  L7_uint32 encapType;
  L7_RC_t rc = L7_SUCCESS;

  /* ethernet frame overhead. Includes at least source and dest MAC addrs + FCS */
  L7_uint32 ethHdrLen = L7_ENET_HDR_SIZE + L7_ETHERNET_CHECKSUM_LEN;    

  nimIntfConfig_t *nimCfg = &nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo;
  L7_uint32 origSize = nimCfg->ipMtu;

  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc != L7_SUCCESS)
  {
    encapType = FD_NIM_ENCAPSULATION_TYPE;
  }
  else
  {
    if (nimEncapsulationTypeGet(intIfNum, &encapType) != L7_SUCCESS)
    {
      encapType = FD_NIM_ENCAPSULATION_TYPE;
    }
  }

  if (encapType == L7_ENCAP_802)
  {
    ethHdrLen += L7_802_ENCAPS_HDR_SIZE;
  }
  else
  {
    ethHdrLen += L7_ENET_ENCAPS_HDR_SIZE;
  }

  nimCfg->ipMtu = nimCfg->cfgMaxFrameSize - ethHdrLen;

  /* Notify apps of new IP MTU limit. */
  if (nimCfg->ipMtu != origSize)
  {
    nimNotifyIntfChange(intIfNum, L7_SET_MTU_SIZE);
  } 

  return nimCfg->ipMtu;
}

/*********************************************************************
* @purpose  Get a set of NIM CPU counters
*
* @param    pCounter    Pointer to a stats counter structure
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimGetCPUCounters ( counterValue_entry_t *pCounter )
{
  L7_RC_t rc = L7_SUCCESS;

  IS_INTIFNUM_PRESENT(pCounter->cKey,rc);

  if (rc == L7_SUCCESS)
  {
    pCounter->status = dtlStatsGet(pCounter->cKey, pCounter->cId, pCounter->cSize, &(pCounter->cValue));
  }


  return(rc);
}


/*********************************************************************
* @purpose  Get a set of NIM interface counters
*
* @param    pCounter    Pointer to a stats counter structure
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimGetInterfaceCounters ( counterValue_entry_t *pCounter )
{
  L7_RC_t   rc = L7_SUCCESS;

  IS_INTIFNUM_PRESENT(pCounter->cKey,rc);

  if (rc == L7_SUCCESS)
  {
    pCounter->status = dtlStatsGet(pCounter->cKey, pCounter->cId, pCounter->cSize, &(pCounter->cValue));
  }

  return( rc );
}

/*********************************************************************
* @purpose  Gets the counter size in bits
*
* @param    id
*
* @returns  counter size in bits
*
* @notes    none 
*       
* @end
*********************************************************************/
counter_size_t nimGetCounterSize(counter_id_t id)
{
  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);
  if ((id == (L7_PLATFORM_CTR_RX_TOTAL_BYTES))  ||
      (id == (L7_PLATFORM_CTR_RX_UCAST_FRAMES)) ||
      (id == (L7_PLATFORM_CTR_RX_MCAST_FRAMES)) ||
      (id == (L7_PLATFORM_CTR_RX_BCAST_FRAMES)) ||
      (id == (L7_PLATFORM_CTR_TX_TOTAL_BYTES))  ||
      (id == (L7_PLATFORM_CTR_TX_UCAST_FRAMES)) ||
      (id == (L7_PLATFORM_CTR_TX_MCAST_FRAMES)) ||
      (id == (L7_PLATFORM_CTR_TX_BCAST_FRAMES)) )
  {
    return(C64_BITS);
  }

  return(C32_BITS);
}

/*********************************************************************
* @purpose  Creates an NIM interface counters
*
* @param    intIfNum    Internal Interface Number 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimInterfaceCounters ( L7_uint32 intIfNum , L7_BOOL create)
{

  L7_uint32 CPUListSize = 0, baseListSize = 0, i;
  L7_uint32 *pCPUCounters = 0, *pBaseCounters = 0;
  statsParm_entry_t statsParm_entry;
  L7_uint32 unitIndex;
  L7_RC_t rc = L7_SUCCESS;
  

  /*
  * No stats for wireless interfaces .
  * L7_IANA_OTHER (1) and L7_IANA_OTHER_CPU (1) have the same value
  * so this check is needed to distinguish between them.
  */
  if (nimCtlBlk_g->nimPorts[intIfNum].sysIntfType == L7_WIRELESS_INTF ||
      nimCtlBlk_g->nimPorts[intIfNum].sysIntfType == L7_VLAN_PORT_INTF)     /* PTin added: virtual ports */
  {
    return L7_SUCCESS;
  }

  switch (nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType)
  {
    case L7_IANA_OTHER_CPU:
      CPUListSize = sizeof ( statsCPU_ctr_list ) / sizeof ( L7_uint32 );
      pCPUCounters = statsCPU_ctr_list;

      break;

    case L7_IANA_FAST_ETHERNET:
    case L7_IANA_GIGABIT_ETHERNET:
    case L7_IANA_2G5_ETHERNET:      /* PTin added: Speed 2.5G */
    case L7_IANA_10G_ETHERNET:
    case L7_IANA_40G_ETHERNET:      /* PTin added: Speed 40G */
    case L7_IANA_100G_ETHERNET:     /* PTin added: Speed 100G */
    case L7_IANA_CAPWAP_TUNNEL:

      baseListSize = sizeof ( statsFastEthernet_base_ctr_list ) / sizeof ( L7_uint32 );
      pBaseCounters = statsFastEthernet_base_ctr_list;

      break;

    case L7_IANA_FAST_ETHERNET_FX:

      baseListSize = 0;

      break;   

    default:
      break;
  }


  if ((rc = unitMgrNumberGet(&unitIndex)) != L7_SUCCESS)
    return(rc);

  switch (nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType)
  {
    case L7_IANA_OTHER_CPU:

      /* create CPU counters */
      statsParm_entry.cKey = intIfNum;
      statsParm_entry.cType = ExternalCtr;
      statsParm_entry.isResettable = L7_FALSE;
      statsParm_entry.pSource = (L7_VOIDFUNCPTR_t)nimGetCPUCounters;
      statsParm_entry.status = L7_FAILURE;
      statsParm_entry.pMutlingsParmList = L7_NULL;

      for (i = 0 ; i < CPUListSize ; i++)
      {
        statsParm_entry.cId = *pCPUCounters++;
        statsParm_entry.cSize = nimGetCounterSize(statsParm_entry.cId);

        if (create)
        {
          if ((rc = statsCreate ( 1, &statsParm_entry )) != L7_SUCCESS)
          {
            NIM_LOG_MSG("NIM: Failed to create CPU stat(%d) on intf(%d)\n",
                        statsParm_entry.cId,intIfNum);

            break;
          }
        }
        else
        {
          if ((rc = statsDelete ( 1, &statsParm_entry )) != L7_SUCCESS)
          {
            NIM_LOG_MSG("NIM: Failed to delete CPU stat(%d) on intf(%d)\n",
                        statsParm_entry.cId,intIfNum);

            break;
          }
        }
      }

      /* create user counters */
      if ((CPUListSize != 0) && (rc == L7_SUCCESS))
      {
        if (create)
          rc = usmDbUserStatsCreate(unitIndex, L7_USMDB_USER_STAT_CPU, intIfNum);
        else
          rc = usmDbUserStatsDelete(unitIndex, L7_USMDB_USER_STAT_CPU, intIfNum);
      }

      break;

    case L7_IANA_FAST_ETHERNET:
    case L7_IANA_FAST_ETHERNET_FX:
    case L7_IANA_GIGABIT_ETHERNET:
    case L7_IANA_2G5_ETHERNET:      /* PTin added: Speed 2.5G */
    case L7_IANA_10G_ETHERNET:
    case L7_IANA_40G_ETHERNET:      /* PTin added: Speed 40G */
    case L7_IANA_100G_ETHERNET:     /* PTin added: Speed 100G */
    case L7_IANA_CAPWAP_TUNNEL:

      /* create base counters */

      statsParm_entry.cKey = intIfNum;
      /*statsParm_entry.cSize = C32_BITS;*/
      statsParm_entry.cType = ExternalCtr;
      statsParm_entry.isResettable = L7_FALSE;
      statsParm_entry.pSource = (L7_VOIDFUNCPTR_t)nimGetInterfaceCounters;
      statsParm_entry.status = L7_FAILURE;
      statsParm_entry.pMutlingsParmList = L7_NULL;

      for (i = 0 ; i < baseListSize ; i++)
      {
        statsParm_entry.cId = *pBaseCounters++;
        statsParm_entry.cSize = nimGetCounterSize(statsParm_entry.cId);

        if (create)
        {
          if ((rc = statsCreate ( 1, &statsParm_entry )) != L7_SUCCESS)
          {
            NIM_LOG_MSG("NIM: Failed to create Intf stat(%d) on intf(%d)\n",
                        statsParm_entry.cId,intIfNum);

            break;
          }
        }
        else
        {
          if ((rc = statsDelete ( 1, &statsParm_entry )) != L7_SUCCESS)
          {
            NIM_LOG_MSG("NIM: Failed to delete Intf stat(%d) on intf(%d)\n",
                        statsParm_entry.cId,intIfNum);

            break;
          }
        }
      }

      /* create user counters */
      if (baseListSize != 0)
      {
        if (create)
        {
          if ((rc = usmDbUserStatsCreate(unitIndex, L7_USMDB_USER_STAT_FAST_ENET, intIfNum)) != L7_SUCCESS)
            NIM_LOG_MSG("NIM: Failed in call to delete usmDbUserStatsCreate for intf(%d)\n",intIfNum);
        }
        else
        {
          if ((rc = usmDbUserStatsDelete(unitIndex, L7_USMDB_USER_STAT_FAST_ENET, intIfNum)) != L7_SUCCESS)
            NIM_LOG_MSG("NIM: Failed in call to delete usmDbUserStatsDelete for intf(%d)\n",intIfNum);
        }
      }

      break;

    default:
      rc = L7_SUCCESS;
      break;
  }

  return( rc );
}

/*********************************************************************
* @purpose  Resets counters for an NIM interface
*
* @param    intIfNum    Internal Interface Number 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimResetInterfaceCounters ( L7_uint32 intIfNum )
{
  L7_uint32 CPUListSize = 0, baseListSize = 0, i;
  L7_uint32 *pCPUCounters = 0, *pBaseCounters = 0; 
  counterValue_entry_t counterValue_entry;

  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);

  switch (nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType)
  {
    case L7_IANA_OTHER_CPU:
      CPUListSize = sizeof ( statsCPU_ctr_list ) / sizeof ( L7_uint32 );
      pCPUCounters = statsCPU_ctr_list;
      break;

    case L7_IANA_FAST_ETHERNET:
      baseListSize = sizeof ( statsFastEthernet_base_ctr_list ) / sizeof ( L7_uint32 );
      pBaseCounters = statsFastEthernet_base_ctr_list;
      break;

    case L7_IANA_FAST_ETHERNET_FX:
    case L7_IANA_GIGABIT_ETHERNET:
    case L7_IANA_2G5_ETHERNET:      /* PTin added: Speed 2.5G */
    case L7_IANA_10G_ETHERNET:
    case L7_IANA_40G_ETHERNET:      /* PTin added: Speed 40G */
    case L7_IANA_100G_ETHERNET:     /* PTin added: Speed 100G */
      baseListSize = 0;
      pBaseCounters = L7_NULL;
      break;

    default:
      break;
  }


  switch (nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType)
  {
    case L7_IANA_OTHER_CPU:

      /* reset CPU counters */

      counterValue_entry.cKey = intIfNum;
      counterValue_entry.status = L7_FAILURE;
      for (i = 0 ; i < CPUListSize ; i++)
      {
        counterValue_entry.cId = *pCPUCounters++;
        statsReset ( 1, &counterValue_entry );
      }
      break;

    case L7_IANA_FAST_ETHERNET:
    case L7_IANA_FAST_ETHERNET_FX:
    case L7_IANA_GIGABIT_ETHERNET:
    case L7_IANA_2G5_ETHERNET:      /* PTin added: Speed 2.5G */
    case L7_IANA_10G_ETHERNET:
    case L7_IANA_40G_ETHERNET:      /* PTin added: Speed 40G */
    case L7_IANA_100G_ETHERNET:     /* PTin added: Speed 100G */

      /* reset base counters */

      counterValue_entry.cKey = intIfNum;
      counterValue_entry.status = L7_FAILURE;
      for (i = 0 ; i < baseListSize ; i++)
      {
        counterValue_entry.cId = *pBaseCounters++;
        statsReset ( 1, &counterValue_entry );
      }
      break;

    default:
      break;
  }

  return( L7_SUCCESS );
}

/*********************************************************************
* @purpose  Deletes the specified interface from the running and cached cfg file
*
* @param    intIfNum Internal Interface Number 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimDeleteInterface(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;
  nimCfgPort_t    *localConfigData;
  nimConfigID_t    configInterfaceId;
  nimConfigIdTreeData_t  configIdInfo;
  L7_uint32        numOfInterfaces = 0;
  nimUSP_t usp;
  L7_RC_t   rc = L7_SUCCESS;

  NIM_CRIT_SEC_WRITE_ENTER();

  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    /* delete nim created stats for this interface */
    nimInterfaceCounters(intIfNum,L7_FALSE);

    sysIntfType = nimCtlBlk_g->nimPorts[intIfNum].sysIntfType;

    localConfigData   = nimCtlBlk_g->nimConfigData->cfgPort;  
    numOfInterfaces   = nimCtlBlk_g->nimConfigData->numOfInterfaces;
    configInterfaceId = nimCtlBlk_g->nimPorts[intIfNum].configInterfaceId;


    /* Remove the config id info from the avl tree */

    memset(&configIdInfo, 0 , sizeof( configIdInfo));
    NIM_CONFIG_ID_COPY(&configIdInfo.configId, &configInterfaceId);
    configIdInfo.intIfNum = intIfNum;
    (void)nimConfigIdTreeEntryDelete( &configIdInfo);
    

    /* clear the interface to be used in the bitmap*/
    NIM_INTF_CLRMASKBIT(nimCtlBlk_g->nimConfigData->configMaskBitmap, intIfNum);

    /* Delete the interface config in the config data */
    nimConfigInterfaceDelete(configInterfaceId);

    usp.unit = nimCtlBlk_g->nimPorts[intIfNum].usp.unit;
    usp.slot = nimCtlBlk_g->nimPorts[intIfNum].usp.slot;
    usp.port = nimCtlBlk_g->nimPorts[intIfNum].usp.port;

    nimIfIndexDelete(nimCtlBlk_g->nimPorts[intIfNum].ifIndex);

    /* set the entry in the quick map to unused */
    nimUnitSlotPortToIntfNumClear(&usp);   

    /* mark this interface is not in use */
    nimCtlBlk_g->nimPorts[intIfNum].present = L7_FALSE;

    if (nimCtlBlk_g->nimNumberOfPortsPerUnit[(L7_uint32)usp.unit] > 0 )
    {
      nimCtlBlk_g->nimNumberOfPortsPerUnit[(L7_uint32)usp.unit]--;
      rc = L7_SUCCESS;
    }

    nimCtlBlk_g->numberOfInterfacesByType[nimCtlBlk_g->nimPorts[intIfNum].sysIntfType]--;
    /* reset all the information for this interface */
    memset(( void * )&nimCtlBlk_g->nimPorts[intIfNum],0, sizeof( nimIntf_t )); 

    /* mark the config file as changed */
    nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
  }

  NIM_CRIT_SEC_WRITE_EXIT();

  return(rc);
}

/*********************************************************************
* @purpose  Find the first unused bit in an interface mask
*
* @param    mask        @b{(input)}  The interface mask to evaluate
* @param    intIfNnum   @b{(output)} The unused intIfNum in the mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if there were no unused bits
*
* @notes    If there is no unused bit, intIfNum will be zero
*       
* @end
*********************************************************************/
L7_RC_t nimMaskFirstUnusedIntIfNumGet(NIM_INTF_MASK_t mask,L7_uint32 *intIfNum) 
{
  L7_uint32 ol,il;                              
  L7_uchar8 value;                              
  L7_RC_t rc = L7_ERROR;

  *intIfNum = 0; /* make the index invalid */

  for (ol = 0;ol < NIM_INTF_INDICES;ol++)
  {
    value = ~mask.value[ol] ;

    if (value > 0)
    {
      for (il = 0;il < 8 ; il++)
      {
        if (value & (1 << il))
        {
          /* make the intIfNum non-zero based */
          *(intIfNum) = (ol * 8) + il + 1;            
          rc = L7_SUCCESS;                 
          break;                                
        }
      }                                         
      break;                                    
    }
  }                                             

  return(rc);
} 

/*********************************************************************
* @purpose  Create the internal interface number for the interface
*
* @param    configId  @b{(input)}  The configuration ID to be searched
* @param    intIfNum  @b{(output)} Internal Interface Number created
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_RC_t nimIntIfNumCreate(nimConfigID_t configId, L7_uint32 *intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32    unit,slot,port;
  L7_uint32    maxUnits,maxSlotsPerUnit,maxPhysPortsPerSlot,maxPhysPortsPerUnit;
  L7_uint32    min, max;
  L7_uint32    i;
  L7_uint32    slotOffset;
  nimConfigIdTreeData_t  configIdInfo;


  /*----------------------------------------------------------------------*/
  /* Determine if an intIfNum was assigned from a previous boot and reuse */
  /*----------------------------------------------------------------------*/
   
  if (nimConfigIdTreeEntryGet(&configId, &configIdInfo) == L7_SUCCESS)
  {
      *intIfNum = configIdInfo.intIfNum;
      return L7_SUCCESS;
  }

  /*-------------------------------------------------------*/
  /* An intIfNum was not previously assigned.  Assign one. */
  /*-------------------------------------------------------*/
  
  maxUnits              = platUnitTotalMaxPerStackGet();
  maxSlotsPerUnit       = platSlotMaxPhysicalSlotsPerUnitGet();
  maxPhysPortsPerSlot   = platSlotMaxPhysicalPortsPerSlotGet();
  maxPhysPortsPerUnit   = platUnitMaxPhysicalPortsGet();
  
  switch (configId.type)
  {
    case L7_PHYSICAL_INTF:
      unit = (L7_uint32)configId.configSpecifier.usp.unit; 
      slot = (L7_uint32) configId.configSpecifier.usp.slot;  
      port = (L7_uint32) configId.configSpecifier.usp.port;  

      if ((unit > maxUnits) || (slot >= maxSlotsPerUnit) || (port > maxPhysPortsPerSlot))
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range usp (%d.%d.%d)\n",unit, slot,port ); 
      }
      else
      {
        /* calculate the last offset for ports in lower-numbered slots on this unit */
        slotOffset = 0;

        for (i=0; i < slot; i++) 
        {
           slotOffset += sysapiHpcPhysPortsInSlotGet(i);
        }

        /* intIfNum determined by intIfNums assigned to lower numbered units and slots */
        *intIfNum = ((unit - 1) * maxPhysPortsPerUnit) + slotOffset + port;
        rc = L7_SUCCESS;

      }
      break; 


    case L7_CPU_INTF:
      /* CPU interfaces are directly after the Physical interfaces */
      unit = (L7_uint32)configId.configSpecifier.usp.unit; 
      slot = (L7_uint32) configId.configSpecifier.usp.slot;  
      port = (L7_uint32) configId.configSpecifier.usp.port;  

      if ((slot != platSlotCpuSlotNumGet()) || (unit > maxUnits) ||
          (port > platIntfCpuIntfMaxCountGet()))
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range usp for CPU Intf (%d.%d.%d)\n", unit, slot, port); 
      }
      else
      {

        /* The relative port number used by the owning interface for logical ports is 1-based.
           Decrement this number by 1 so that an intIfNum used by one type of interface does
           not pour over into the range used by another */
        (void) nimIntIfNumRangeGet(L7_CPU_INTF, &min, &max);
        *intIfNum = min+ (port-1);
        rc = L7_SUCCESS;
      }

      break;

    case L7_LAG_INTF:
      /* LAG interfaces are directly after the CPU interfaces */
      port = configId.configSpecifier.dot3adIntf;
      if (port > platIntfLagIntfMaxCountGet())
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range for LAG interface (%d)\n",configId.configSpecifier.dot3adIntf);
      }
      else
      {
        /* The relative port number used by the owning interface for logical ports is 1-based.
           Decrement this number by 1 so that an intIfNum used by one type of interface does
           not pour over into the range used by another */
          (void) nimIntIfNumRangeGet(L7_LAG_INTF, &min, &max);
          *intIfNum = min+ (port-1);
          rc = L7_SUCCESS;
      }
      break;

    case L7_LOGICAL_VLAN_INTF:
      /* VLAN interfaces are directly after the LAG interfaces */
      if (configId.configSpecifier.vlanId > L7_PLATFORM_MAX_VLAN_ID)
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range vlan interface (%d)\n",configId.configSpecifier.vlanId);
      }
      else
      {
        if (nimPortInstanceNumGet(configId, &port) == L7_SUCCESS) 
        {

            /* The relative port number used by the owning interface for logical ports is 1-based.
               Decrement this number by 1 so that an intIfNum used by one type of interface does
               not pour over into the range used by another */
            (void) nimIntIfNumRangeGet(L7_LOGICAL_VLAN_INTF, &min, &max);
            *intIfNum = min+ (port-1);
            rc = L7_SUCCESS;
        }
        else
            rc = L7_FAILURE;
      }
      break;

    case L7_LOOPBACK_INTF:
      /* Loopback interfaces are directly after the VLAN interfaces */
      if (configId.configSpecifier.loopbackId >= L7_MAX_NUM_LOOPBACK_INTF)
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range loopback interface (%d)\n",
                    configId.configSpecifier.loopbackId);
      }
      else
      {
        /* loopbackId's are zero-based */
        (void) nimIntIfNumRangeGet(L7_LOOPBACK_INTF, &min, &max);
        *intIfNum = min + configId.configSpecifier.loopbackId;
        rc = L7_SUCCESS;
      }
      break;

    case L7_TUNNEL_INTF:
      /* Tunnel interfaces are directly after the Loopback interfaces */
      if (configId.configSpecifier.tunnelId >= L7_MAX_NUM_TUNNEL_INTF)
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range tunnel interface (%d)\n",
                    configId.configSpecifier.tunnelId);
      }
      else
      {
        /* tunnelId's are zero-based */
        (void) nimIntIfNumRangeGet(L7_TUNNEL_INTF, &min, &max);
        *intIfNum = min + configId.configSpecifier.tunnelId;
        rc = L7_SUCCESS;
      }
      break;

  case L7_WIRELESS_INTF:
    /* wireless network interfaces are directly after the tunnel interfaces and
       wirelessnetId is 1-based*/
    if (configId.configSpecifier.wirelessNetId < L7_MIN_NUM_WIRELESS_INTF ||
          configId.configSpecifier.wirelessNetId > L7_MAX_NUM_WIRELESS_INTF)
    {
      rc = L7_FAILURE;
      NIM_LOG_MSG("NIM: out of range wireless network interface (%d)\n",
                  configId.configSpecifier.wirelessNetId);
    }
    else
    {
      (void) nimIntIfNumRangeGet(L7_WIRELESS_INTF, &min, &max);
      *intIfNum = min + configId.configSpecifier.wirelessNetId - 1;
      rc = L7_SUCCESS;
    }
    break;

    case L7_CAPWAP_TUNNEL_INTF:
      /* L2tunnel network interfaces are directly after the wireless interfaces */
      if (configId.configSpecifier.l2tunnelId >= L7_MAX_NUM_CAPWAP_TUNNEL_INTF)
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range l2tunnel interface (%d)\n",
                    configId.configSpecifier.l2tunnelId);
      }
      else
      {
        (void) nimIntIfNumRangeGet(L7_CAPWAP_TUNNEL_INTF, &min, &max);
        *intIfNum = min + configId.configSpecifier.l2tunnelId;
        rc = L7_SUCCESS;
      }
      break;

    /* PTin added: virtual ports */
    case L7_VLAN_PORT_INTF:
      if (configId.configSpecifier.vlanportId == 0 ||
          configId.configSpecifier.vlanportId > L7_MAX_NUM_VLAN_PORT_INTF)
      {
        rc = L7_FAILURE;
        NIM_LOG_MSG("NIM: out of range vlan port interface (%d)\n",
                    configId.configSpecifier.vlanportId);
      }
      else
      {
        (void) nimIntIfNumRangeGet(L7_VLAN_PORT_INTF, &min, &max);
        *intIfNum = min + (configId.configSpecifier.vlanportId-1);
        rc = L7_SUCCESS;
      }
      break;

    case L7_STACK_INTF:
      rc = L7_FAILURE;
      NIM_LOG_MSG("NIM: unsupported type of interface L7_STACK_INTF\n");
      break;

    default:
      rc = L7_FAILURE;
      NIM_LOG_MSG("NIM: unsupported type of interface (%d)\n",configId.type);
      break;
  }

  if ((nimCtlBlk_g != L7_NULLPTR) && (nimCtlBlk_g->nimConfigData != L7_NULLPTR))
  {
    if (rc != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      *intIfNum = 0;
    }
    else
    {
      rc = L7_SUCCESS;

      /* start with known data in the port */
      memset(( void * )&nimCtlBlk_g->nimPorts[*intIfNum], 0,sizeof(nimIntf_t)); 

      /* set the interface to be used */
      NIM_INTF_SETMASKBIT(nimCtlBlk_g->nimConfigData->configMaskBitmap, *intIfNum);

      /* Store the configId/intIfNum for quick cross access */

      memset(&configIdInfo, 0 , sizeof( configIdInfo));
      NIM_CONFIG_ID_COPY(&configIdInfo.configId, &configId);
      configIdInfo.intIfNum = *intIfNum;
      (void)nimConfigIdTreeEntryAdd( &configIdInfo);
    }
  }
  else
  {
    rc = L7_FAILURE;
    *intIfNum = 0;
  }

  return(rc);
}
/*********************************************************************
* @purpose  Create the internal interface number for the interface
*
* @param    configId  @b{(input)}  The configuration ID to be searched
* @param    intIfNum  @b{(output)} Internal Interface Number created
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_RC_t nimIntIfNumDelete(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if ((intIfNum > 0) || (intIfNum <= platIntfTotalMaxCountGet()))
  {
    /* start with known data in the port */
    memset(( void * )&nimCtlBlk_g->nimPorts[intIfNum], 0,sizeof(nimIntf_t)); 

    /* set the interface to be used */
    NIM_INTF_CLRMASKBIT(nimCtlBlk_g->nimConfigData->configMaskBitmap, intIfNum);

    rc = L7_SUCCESS;
  }

  return(rc);
}


/*********************************************************************
* @purpose  Check to see if the number of interfaces per type are exceeded
*
* @param    intfType  @b{(input)} one of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_BOOL nimNumberOfInterfaceExceeded(L7_INTF_TYPES_t intfType)
{
  L7_BOOL rc = L7_FALSE;
  switch (intfType)
  {
    case L7_PHYSICAL_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_PHYSICAL_INTF] >= platIntfPhysicalIntfMaxCountGet())
      {
        rc = (L7_TRUE);
      }
      break;

    case L7_STACK_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_STACK_INTF] >= platIntfStackIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_CPU_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_CPU_INTF] >= platIntfCpuIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_LAG_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_LAG_INTF] >= platIntfLagIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_LOGICAL_VLAN_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_LOGICAL_VLAN_INTF] >= platIntfVlanIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_LOOPBACK_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_LOOPBACK_INTF] >= platIntfLoopbackIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_TUNNEL_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_TUNNEL_INTF] >= platIntfTunnelIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_WIRELESS_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_WIRELESS_INTF] >= platIntfwirelessNetIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    case L7_CAPWAP_TUNNEL_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_CAPWAP_TUNNEL_INTF] >= platIntfL2TunnelIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    /* PTin added: virtual ports */
    case L7_VLAN_PORT_INTF:
      if (nimCtlBlk_g->numberOfInterfacesByType[L7_VLAN_PORT_INTF] >= platIntfVlanPortIntfMaxCountGet())
      {
        rc = L7_TRUE;
      }
      break;

    default:
      rc = L7_TRUE;



  }
  return(rc);
}


/*********************************************************************
* @purpose  Get the maximum number of interfaces for an interface type
*
* @param    intfType  @b{(input)} one of L7_INTF_TYPES_t
*
* @returns  the maximum number of interfaces for an interface type, 0 or greater
*
* @notes    Returns zero if an interface type is unrecognized
*       
* @end
*********************************************************************/
L7_int32 nimMaxIntfForIntfTypeGet(L7_INTF_TYPES_t intfType)
{

  L7_uint32 numIntf;

  switch (intfType)
  {
    case L7_PHYSICAL_INTF:
       numIntf =  platIntfPhysicalIntfMaxCountGet();
       break;

    case L7_STACK_INTF:
      numIntf =  platIntfStackIntfMaxCountGet();
      break;

    case L7_CPU_INTF:
      numIntf =  platIntfCpuIntfMaxCountGet();
      break;

    case L7_LAG_INTF:
      numIntf =  platIntfLagIntfMaxCountGet();
      break;

    case L7_LOGICAL_VLAN_INTF:
      numIntf =  platIntfVlanIntfMaxCountGet();
      break;

    case L7_LOOPBACK_INTF:
      numIntf =  platIntfLoopbackIntfMaxCountGet();
      break;

    case L7_TUNNEL_INTF:
      numIntf =  platIntfTunnelIntfMaxCountGet();
      break;

    case L7_WIRELESS_INTF:
      numIntf = platIntfwirelessNetIntfMaxCountGet();
      break;

    case L7_CAPWAP_TUNNEL_INTF:
      numIntf = platIntfL2TunnelIntfMaxCountGet();
      break;

    /* PTin added: virtual ports */
    case L7_VLAN_PORT_INTF:
      numIntf = platIntfVlanPortIntfMaxCountGet();
      break;

    default:
      numIntf =  0;

  }

  return numIntf;
}


/*********************************************************************
* @purpose  Delete the internal interface number for the interface
*
* @param    configId  @b{(input)}  The configuration ID to be searched
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_RC_t nimConfigInterfaceDelete(nimConfigID_t configId)
{
  L7_RC_t rc = L7_FAILURE;
  nimCfgPort_t *localConfigData;
  L7_uint32     counter;
  L7_uint32     numOfInterfaces;
  L7_uint32     intIfNum = 0;

  if ((nimCtlBlk_g != L7_NULLPTR) && (nimCtlBlk_g->nimConfigData != L7_NULLPTR))
  {

    numOfInterfaces = nimCtlBlk_g->nimConfigData->numOfInterfaces;
    localConfigData   = nimCtlBlk_g->nimConfigData->cfgPort;  

    /* see if the config for the interface exists in the file */
    for (counter = 0; counter <= numOfInterfaces; counter++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&localConfigData[counter].configInterfaceId,&configId))
      {
        intIfNum = localConfigData[counter].configIdMaskOffset;
        memset(&localConfigData[counter],0x00,sizeof(nimCfgPort_t));
        rc = L7_SUCCESS;
        break;
      }
    }

    if ((rc == L7_SUCCESS) && (nimCtlBlk_g->nimPorts[intIfNum].present == L7_TRUE))
    {
      /* clear the interface being deleted */
      NIM_INTF_CLRMASKBIT(nimCtlBlk_g->nimConfigData->configMaskBitmap, intIfNum);
    }
  }

  return(rc);
}


/*********************************************************************
* @purpose  Set the state of an interface
*
* @param    intIfNum    The internal interface number 
* @param    state       The state of the interface from the L7_INTF_STATES_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimUtilIntfStateSet(L7_uint32 intIfNum, L7_INTF_STATES_t state)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: State set during incorrect CNFGR phase\n");
    rc = L7_FAILURE;
  }
  else if ((intIfNum < 1) || (intIfNum > platIntfTotalMaxCountGet()))
  {
    NIM_LOG_MSG("NIM: intIfNum out of range (max=%u)\n",platIntfTotalMaxCountGet());
    rc = L7_FAILURE;
  }
  else
  {
    nimCtlBlk_g->nimPorts[intIfNum].intfState = state;
  }

  return(rc);
}


/*********************************************************************
* @purpose  Get the state of an interface
*
* @param    intIfNum    The internal interface number 
*
* @returns  Present state of the interface
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_INTF_STATES_t nimUtilIntfStateGet(L7_uint32 intIfNum)
{
  L7_INTF_STATES_t state = L7_INTF_UNINITIALIZED;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: incorrect CNFGR phase for action\n");
  }
  else if ((intIfNum < 1) || (intIfNum > platIntfTotalMaxCountGet()))
  {
    NIM_LOG_MSG("NIM: intIfNum out of range\n");
  }
  else
  {
    state = nimCtlBlk_g->nimPorts[intIfNum].intfState;
  }

  return state;
} 

/*********************************************************************
* @purpose  Determine the next State to transition to
*          
* @param    currState    @b{(input)}    The current state
* @param    event        @b{(input)}    The event being processed
* @param    nextState    @b{(output)}   The next state to transition to
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimUtilIntfNextStateGet(L7_INTF_STATES_t currState,L7_PORT_EVENTS_t event,L7_INTF_STATES_t *nextState)
{
  L7_RC_t rc = L7_FAILURE;

  switch (currState)
  {
    case L7_INTF_UNINITIALIZED:
      if (event == L7_CREATE)
      {
        *nextState = L7_INTF_CREATING;
        rc = L7_SUCCESS;
      }
      break;
    case L7_INTF_CREATING:
      if (event == L7_CREATE_COMPLETE)
      {
        *nextState = L7_INTF_CREATED;
        rc = L7_SUCCESS;
      }
      break;
    case L7_INTF_CREATED:
      if (event == L7_ATTACH)
      {
        *nextState = L7_INTF_ATTACHING;
        rc = L7_SUCCESS;
      }
      else if (event == L7_DELETE)
      {
        *nextState = L7_INTF_DELETING;
        rc = L7_SUCCESS;
      }
      else
      {
        rc = L7_FAILURE;
      }
      break;
    case L7_INTF_ATTACHING:
      if (event == L7_ATTACH_COMPLETE)
      {
        *nextState = L7_INTF_ATTACHED;
        rc = L7_SUCCESS;
      }
      break;
    case L7_INTF_ATTACHED:
      if (event == L7_DETACH)
      {
        *nextState = L7_INTF_DETACHING;
        rc = L7_SUCCESS;
      }
      break;
    case L7_INTF_DETACHING:
      if (event == L7_DETACH_COMPLETE)
      {
        *nextState = L7_INTF_CREATED;
        rc = L7_SUCCESS;
      }
      break;
    case L7_INTF_DELETING:
      if (event == L7_DELETE_COMPLETE)
      {
        *nextState = L7_INTF_UNINITIALIZED;
        rc = L7_SUCCESS;
      }
      break;
    default:
      rc = L7_FAILURE;
  }
  return rc;
}


/*********************************************************************
* @purpose  Add an entry to the nim nimConfigIdTreeData AVL tree
*          
* @param    *pConfigIdInfo    @b{(input)} pointer to a nimConfigIdTreeData_t structure
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimConfigIdTreeEntryAdd(  nimConfigIdTreeData_t  *pConfigIdInfo)
{

   void *pData;
   L7_RC_t rc;

   rc = L7_SUCCESS;

 /* WPJ     NIM_IFINDEX_CRIT_SEC_ENTER(); */
   pData = avlInsertEntry(&nimCtlBlk_g->nimConfigIdTreeData, (void *)pConfigIdInfo);

   if (pData != L7_NULLPTR)
   {
     NIM_LOG_MSG("NIM: configId not added to tree for intIfNum %d\n", 
                 pConfigIdInfo->intIfNum);
     rc = L7_FAILURE;
   }

 /* WPJ     NIM_IFINDEX_CRIT_SEC_EXIT(); */

   return rc;

}

/*********************************************************************
* @purpose  Delete an entry from the nim nimConfigIdTreeData AVL tree
*          
* @param    *pConfigIdInfo    @b{(input)} pointer to a nimConfigIdTreeData_t structure
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimConfigIdTreeEntryDelete(  nimConfigIdTreeData_t  *pConfigIdInfo)
{

   void *pData;
   L7_RC_t rc;

   rc = L7_SUCCESS;

 /* WPJ     NIM_IFINDEX_CRIT_SEC_ENTER(); */
   pData = avlDeleteEntry(&nimCtlBlk_g->nimConfigIdTreeData, (void *)pConfigIdInfo);

   if (pData == L7_NULLPTR)
   {
     NIM_LOG_MSG("NIM: configId could not be deleted from tree for intIfNum %d\n",
                 pConfigIdInfo->intIfNum);
     rc = L7_FAILURE;
   }


 /* WPJ     NIM_IFINDEX_CRIT_SEC_EXIT(); */

   return rc;

}


/*********************************************************************
* @purpose  Get an entry from the nim nimConfigIdTreeData AVL tree
*          
* @param    *pConfigId    @b{(input)} pointer to a nimConfigID_t structure
* @param    *pConfigIdInfo    @b{(input)} pointer to a nimConfigIdTreeData_t structure
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimConfigIdTreeEntryGet(nimConfigID_t  *pConfigId, 
                                nimConfigIdTreeData_t  *pConfigIdInfo )
{

   void *pData;
   L7_RC_t rc;

   rc = L7_SUCCESS;

 /* WPJ     NIM_IFINDEX_CRIT_SEC_ENTER(); */
   pData = avlSearchLVL7 (&nimCtlBlk_g->nimConfigIdTreeData, pConfigId, AVL_EXACT);

   if (pData != L7_NULLPTR)
   {
     memcpy( pConfigIdInfo, pData, sizeof(nimConfigIdTreeData_t) );
   }
   else
   {
     rc = L7_FAILURE;
   }

 /* WPJ     NIM_IFINDEX_CRIT_SEC_EXIT(); */

   return rc;

}
  


/*********************************************************************
* @purpose  Set either the saved config or the default config in the interface
*
* @param    port        @b{(input)}   The interface being manipulated
* @param    cfgVersion  @b{(input)}   version of the config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    
*       
* @end
*********************************************************************/
void nimConfigIdTreePopulate(void)
{
  nimCfgPort_t    *localConfigData;
  nimConfigID_t   configInterfaceId;
  L7_uint32        numOfInterfaces;
  L7_uint32        counter;
  nimConfigIdTreeData_t  configIdInfo;

  if ((nimCtlBlk_g == L7_NULLPTR) || (nimCtlBlk_g->nimConfigData == L7_NULLPTR))
  {
    NIM_LOG_ERROR("NIM: Control block or config data not valid\n");
  }
  else
  {

    localConfigData = nimCtlBlk_g->nimConfigData->cfgPort;

    numOfInterfaces = nimCtlBlk_g->nimConfigData->numOfInterfaces;

    /* Set a null config ID for comparison */
    memset(&configInterfaceId, 0 , sizeof( configInterfaceId));

    /* see if the config for the interface exists in the file */
    for (counter = 0; counter <= numOfInterfaces; counter++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&localConfigData[counter].configInterfaceId,&configInterfaceId))
      {
        /* null config id means empty entry.  Do not populate */
        continue;
      }
      else
      {
          memset(&configIdInfo, 0 , sizeof( configIdInfo));
          configIdInfo.intIfNum = localConfigData[counter].configIdMaskOffset;

          NIM_CONFIG_ID_COPY(&configIdInfo.configId, &localConfigData[counter].configInterfaceId);
          if ( nimConfigIdTreeEntryAdd(&configIdInfo) != L7_SUCCESS)
          {
              NIM_LOG_MSG("Failed to add configId to avl tree for intIfNum %d\n",configIdInfo.intIfNum); 
          }
      }
    }

  }

  /* Flag this true if this phase is complete */
  nimConfigIdTreePopulatationComplete = L7_TRUE;
  return;
}



/*********************************************************************
* @purpose  Determine if nim configuration of config IDs is complete
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL nimConfigIdTreeIsPopulated(void)
{
  return (nimConfigIdTreePopulatationComplete);
}



/*********************************************************************
* @purpose  Populate valid ranges for intIfNums for intf types
*
* @param    void
*
* @returns  void
*
* @notes   
*       
* @end
*********************************************************************/
void nimIntIfNumRangePopulate(void)
{
  L7_uint32  min, max;
  L7_uint32  i;

  
  /* The relative port number used by the owning interface for logical ports is 1-based.
   Decrement this number by 1 so that an intIfNum used by one type of interface does
   not pour over into the range used by another */
  

  /* WPJ: Future - owners of interface type will populate this information */
  for ( i = 0; i < L7_MAX_INTF_TYPE_VALUE; i++) 
  {
      switch (i)
      {
      case L7_PHYSICAL_INTF:

          min = 1;
          max = platIntfPhysicalIntfMaxCountGet();
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;


        case L7_CPU_INTF:
          /* CPU interfaces are directly after the Physical interfaces */

          min = platIntfPhysicalIntfMaxCountGet() + 1;
          max = min + (platIntfCpuIntfMaxCountGet()-1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_LAG_INTF:
          /* LAG interfaces are directly after the CPU interfaces */
          min = platIntfPhysicalIntfMaxCountGet() + 
              platIntfCpuIntfMaxCountGet() + 1;

          max = min + (platIntfLagIntfMaxCountGet()-1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_LOGICAL_VLAN_INTF:
          /* VLAN interfaces are directly after the LAG interfaces */
          min = platIntfPhysicalIntfMaxCountGet() + 
              platIntfCpuIntfMaxCountGet()+ 
              platIntfLagIntfMaxCountGet() + 1;

          max = min + (platIntfVlanIntfMaxCountGet() - 1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_LOOPBACK_INTF:
          /* Loopback interfaces are directly after the VLAN interfaces */
          min = platIntfPhysicalIntfMaxCountGet() + 
              platIntfCpuIntfMaxCountGet() + 
              platIntfLagIntfMaxCountGet() + 
              platIntfVlanIntfMaxCountGet() + 1;
          max = min + (platIntfLoopbackIntfMaxCountGet() - 1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_TUNNEL_INTF:
          /* Tunnel interfaces are directly after the Loopback interfaces */
          min = platIntfPhysicalIntfMaxCountGet() 
                + platIntfCpuIntfMaxCountGet() + platIntfLagIntfMaxCountGet()
                + platIntfVlanIntfMaxCountGet()
                + platIntfLoopbackIntfMaxCountGet() + 1;

          max = min + (platIntfTunnelIntfMaxCountGet() - 1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_WIRELESS_INTF:
          /* Wireless net interfaces are directly after the Tunnel interfaces */
          min = platIntfPhysicalIntfMaxCountGet() 
                + platIntfCpuIntfMaxCountGet() + platIntfLagIntfMaxCountGet()
                + platIntfVlanIntfMaxCountGet()
                + platIntfLoopbackIntfMaxCountGet() 
                + platIntfTunnelIntfMaxCountGet() + 1;

          max = min + (platIntfwirelessNetIntfMaxCountGet() - 1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_CAPWAP_TUNNEL_INTF:
          /* L2 tunnel net interfaces are directly after the Wireless net interfaces */
          min = platIntfPhysicalIntfMaxCountGet() 
                + platIntfCpuIntfMaxCountGet() + platIntfLagIntfMaxCountGet()
                + platIntfVlanIntfMaxCountGet()
                + platIntfLoopbackIntfMaxCountGet() 
                + platIntfTunnelIntfMaxCountGet() 
                + platIntfwirelessNetIntfMaxCountGet() + 1;

          max = min + (platIntfL2TunnelIntfMaxCountGet() - 1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        /* PTin added: virtual ports */
        case L7_VLAN_PORT_INTF:
          /* Vlan port interfaces are directly after the L2 tunnel net interfaces */
          min = platIntfPhysicalIntfMaxCountGet() 
                + platIntfCpuIntfMaxCountGet() + platIntfLagIntfMaxCountGet()
                + platIntfVlanIntfMaxCountGet()
                + platIntfLoopbackIntfMaxCountGet() 
                + platIntfTunnelIntfMaxCountGet() 
                + platIntfwirelessNetIntfMaxCountGet() + 1;

          max = min + (platIntfVlanPortIntfMaxCountGet() - 1);
          nimCtlBlk_g->intfTypeData[i].minIntIfNumber = min;
          nimCtlBlk_g->intfTypeData[i].maxIntIfNumber = max;
          break;

        case L7_STACK_INTF:
          break;

        default:
          break;
      }
  }

}

/*******************************************************************************
* @purpose  to convert the new added interface speed values to that of old values
*
*
* @param    intIfNum           Internal Interface Number
* @param    newintfSpeed       the speed value as per the new added values into
*                              L7_PORT_SPEEDS_t
* @param    *intfSpeed         the pointer to place the old value that is determined
*                              from the new intf speeds values added into L7_PORT_SPEEDS_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    this function is added for the IS-CLI compliance.The newintfSpeed has the value
*           that is equivalent to the new enum values that are added into L7_PORT_SPEEDS_t
*           the new values are { L7_PORT_CAPABILITY_HALF        = 0x100,
*                                L7_PORT_CAPABILITY_FULL        = 0x200,
*                                L7_PORT_CAPABILITY_DUPLEX_AUTO = 0x400,
*                                L7_PORT_CAPABILITY_10          = 0x800,
*                                L7_PORT_CAPABILITY_100         = 0x1000,
*                                L7_PORT_CAPABILITY_1000          = 0x2000,
*                                L7_PORT_CAPABILITY_10G         = 0x4000,
*                                L7_PORT_CAPABILITY_SPEED_AUTO  = 0x8000 }
*
*
*
* @end
*********************************************************************************/
L7_RC_t nimConvertoOldSpeedvalue(L7_uint32 intIfNum, L7_uint32 newintfSpeed, L7_uint32 *intfSpeed)
{
  L7_uint32 duplex,speed;
  L7_uint32 intftype = L7_MAX_INTF_TYPE_VALUE;
  L7_RC_t rc = L7_SUCCESS;

/* since the speed value is not in the older values range we go for converting it */
  duplex = (newintfSpeed & NIM_AUTONEG_FIXED_DUPLEX_MASK);
  if (duplex == 0)
  {
    return L7_ERROR;
  }
  if ((newintfSpeed & L7_PORT_CAPABILITY_DUPLEX_AUTO) != 0)
  {
    duplex = L7_PORT_CAPABILITY_DUPLEX_FULL;
  }

  intftype = nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType;

  speed = (newintfSpeed & NIM_AUTONEG_FIXED_SPEED_MASK);
  if (speed == 0)
  {
    return L7_ERROR;
  }
  if ((newintfSpeed & L7_PORT_CAPABILITY_SPEED_AUTO) != 0)
  {
     /* get the default speed value (max value)as per the port type */
     switch (intftype)
     {
      case L7_IANA_FAST_ETHERNET:
      case L7_IANA_FAST_ETHERNET_FX:
           speed = L7_PORT_CAPABILITY_SPEED_100;
           break;

      case L7_IANA_GIGABIT_ETHERNET:
           speed = L7_PORT_CAPABILITY_SPEED_1000;
           break;

      /* PTin added: Speed 2.5G */
      case L7_IANA_2G5_ETHERNET:
           speed = L7_PORT_CAPABILITY_SPEED_2500;
           break;
      /* PTin end */

      case L7_IANA_10G_ETHERNET:
           speed = L7_PORT_CAPABILITY_SPEED_10G;
           break;

     /* PTin added: Speed 40G */
     case L7_IANA_40G_ETHERNET:
          speed = L7_PORT_CAPABILITY_SPEED_40G;
          break;

     /* PTin added: Speed 100G */
     case L7_IANA_100G_ETHERNET:
          speed = L7_PORT_CAPABILITY_SPEED_100G;
          break;
     /* PTin end */

     default:
           rc = L7_FAILURE;
           break;

     }
  }


 /* once we determine the correct speed and duplex values we map them to
    old L7_PORT_SPEEDS_t values */

   switch (speed)
   {
    case L7_PORT_CAPABILITY_SPEED_10:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL)
         {
           if (intftype == L7_IANA_FAST_ETHERNET || intftype == L7_IANA_GIGABIT_ETHERNET)
           {
             *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_10T;
           }
         }
         else
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_HALF_10T;
         }
         break;

    case L7_PORT_CAPABILITY_SPEED_100:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL)
         {
           if (intftype == L7_IANA_FAST_ETHERNET || intftype == L7_IANA_GIGABIT_ETHERNET)
           {
             *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_100TX;
           }
           else if (intftype == L7_IANA_FAST_ETHERNET_FX)
           {
             *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_100FX;
           }
         }
         else if ((intftype == L7_IANA_FAST_ETHERNET || intftype == L7_IANA_GIGABIT_ETHERNET) &&
                       duplex == L7_PORT_CAPABILITY_DUPLEX_HALF)
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_HALF_100TX;
         }
         break;

    case L7_PORT_CAPABILITY_SPEED_1000:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL && intftype == L7_IANA_GIGABIT_ETHERNET)
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_1000SX;
         }
         break;

    /* PTin added: Speed 2.5G */
    case L7_PORT_CAPABILITY_SPEED_2500:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL && intftype == L7_IANA_2G5_ETHERNET)
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_2P5FX;
         }
         break;
    /* PTin end */

    case L7_PORT_CAPABILITY_SPEED_10G:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL && intftype == L7_IANA_10G_ETHERNET)
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_10GSX;
         }
         break;

    /* PTin added: Speed 40G */
    case L7_PORT_CAPABILITY_SPEED_40G:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL && intftype == L7_IANA_40G_ETHERNET)
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_40G_KR4;
         }
         break;

    /* PTin added: Speed 100G */
    case L7_PORT_CAPABILITY_SPEED_100G:
         if (duplex == L7_PORT_CAPABILITY_DUPLEX_FULL && intftype == L7_IANA_100G_ETHERNET)
         {
           *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_100G_BKP;
         }
         break;
    /* PTin end */

    default:
         rc = L7_FAILURE;
         break;
   }

 return rc;
}

