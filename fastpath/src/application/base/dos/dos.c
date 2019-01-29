/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename src\application\base\dos\dos.c
*
* @purpose Contains definitions to support Denial of Service functions
*
* @component DOSCONTROL
*
* @comments
*
* @create 04/04/2005
*
* @author esmiley
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "log.h"
#include "dtlapi.h"
#include "platform_config.h"
#include "l7_product.h"
#include "dos_api.h"
#include "dos.h"
#include "dos_sid.h"
#include "dos_util.h"
#include "l7_cnfgr_api.h"
#include "l7_icmp.h"
extern doSCfgData_t *doSCfgData;
extern doSCnfgrState_t doSCnfgrState;
osapiRWLock_t       doSCfgRWLock;
L7_int32            dosTaskId;
void *dosQueue = L7_NULLPTR;
L7_uint32 *dosMapTbl = L7_NULLPTR;

/*********************************************************************
* @purpose  Build default DoS config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void doSBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_DOS_MAX_INTF];


  /* save the config id's */
  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_DOS_MAX_INTF);

  for (cfgIndex = 1; cfgIndex < L7_DOS_MAX_INTF; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &doSCfgData->dosIntfCfgData[cfgIndex].configId);

  memset((char*)doSCfgData, 0, sizeof(doSCfgData_t));

  doSCfgData->systemFirstFragMode =   FD_DOS_DEFAULT_FIRSTFRAG_MODE;
  doSCfgData->systemICMPMode =        FD_DOS_DEFAULT_ICMP_MODE;
  doSCfgData->systemICMPFragMode =    FD_DOS_DEFAULT_ICMPFRAG_MODE;
  doSCfgData->systemL4PortMode =      FD_DOS_DEFAULT_L4PORT_MODE;
  doSCfgData->systemUDPPortMode =     FD_DOS_DEFAULT_UDPPORT_MODE;
  doSCfgData->systemSIPDIPMode =      FD_DOS_DEFAULT_SIPDIP_MODE;
  doSCfgData->systemSMACDMACMode =    FD_DOS_DEFAULT_SMACDMAC_MODE;
  doSCfgData->systemTCPFlagMode =     FD_DOS_DEFAULT_TCPFLAG_MODE;
  doSCfgData->systemTCPFragMode =     FD_DOS_DEFAULT_TCPFRAG_MODE;
  doSCfgData->systemTCPFinUrgPshMode =FD_DOS_DEFAULT_TCPFINURGPSH_MODE;
  doSCfgData->systemTCPSynFinMode =   FD_DOS_DEFAULT_TCPSYNFIN_MODE;
  doSCfgData->systemTCPOffsetMode =   FD_DOS_DEFAULT_TCPOFFSET_MODE;
  doSCfgData->systemMaxICMPSize =     FD_DOS_DEFAULT_MAXICMP_SIZE + ICMP_MINLEN;
  doSCfgData->systemMaxICMPv6Size =   FD_DOS_DEFAULT_MAXICMP_SIZE + ICMP_MINLEN;
  doSCfgData->systemMinTCPHdrLength = FD_DOS_DEFAULT_MINTCPHDR_SIZE;
  doSCfgData->synAckFlooding        = FD_DOS_DEFAULT_SYNACK_MODE;
  doSCfgData->pingFlooding          = FD_DOS_DEFAULT_PINGFLOODING_MODE;
  doSCfgData->smurfAttack           = FD_DOS_DEFAULT_SMURFATTACK_MODE;
  doSCfgData->params[DOSINTFPINGPARAM] = FD_DOS_DEFAULT_PING_PARAM;
  doSCfgData->params[DOSINTFSYNACKPARAM] = FD_DOS_DEFAULT_SYNACK_PARAM;

   for (cfgIndex = 1; cfgIndex < L7_DOS_MAX_INTF; cfgIndex++)
    dosBuildDefaultIntfConfigData(&configId[cfgIndex], &doSCfgData->dosIntfCfgData[cfgIndex]);


  /* Build header */
  strcpy(doSCfgData->cfgHdr.filename, DOSCONTROL_CFG_FILENAME);
  doSCfgData->cfgHdr.version = ver;
  doSCfgData->cfgHdr.componentID = L7_DOSCONTROL_COMPONENT_ID;
  doSCfgData->cfgHdr.type = L7_CFG_DATA;
  doSCfgData->cfgHdr.length = sizeof(doSCfgData_t);
  doSCfgData->cfgHdr.dataChanged = L7_FALSE;

}


/*********************************************************************
* @purpose  Applies std policy config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSApplyConfigData(void)
{
    L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;


  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSSIPDIPSet(doSCfgData->systemSIPDIPMode) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system SIP=DIP DoS mode to %d\n", doSCfgData->systemSIPDIPMode);
        rc = L7_FAILURE;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_SMACDMAC_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSSMACDMACSet(doSCfgData->systemSMACDMACMode) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system SMAC=DMAC DoS mode to %d\n", doSCfgData->systemSMACDMACMode);
        rc = L7_FAILURE;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSFirstFragSet(doSCfgData->systemFirstFragMode, doSCfgData->systemMinTCPHdrLength) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system First Fragment DoS mode to %d\n", doSCfgData->systemFirstFragMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPFragSet(doSCfgData->systemTCPFragMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP Fragment DoS mode to %d\n", doSCfgData->systemTCPFragMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPFlagSet(doSCfgData->systemTCPFlagMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP Flag DoS mode to %d\n", doSCfgData->systemTCPFlagMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPFlagSeqSet(doSCfgData->systemTCPFlagMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP Flag&Sequence DoS mode to %d\n", doSCfgData->systemTCPFlagMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPFinUrgPshSet(doSCfgData->systemTCPFinUrgPshMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP FIN/URG/PSH DoS mode to %d\n", doSCfgData->systemTCPFinUrgPshMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPSynFinSet(doSCfgData->systemTCPSynFinMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP SYN/FIN DoS mode to %d\n", doSCfgData->systemTCPSynFinMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPSYN_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPSynSet(doSCfgData->systemTCPSynMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP SYN DoS mode to %d\n", doSCfgData->systemTCPSynMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPOFFSET_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPOffsetSet(doSCfgData->systemTCPOffsetMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system Offset DoS mode to %d\n", doSCfgData->systemTCPOffsetMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSL4PortSet(doSCfgData->systemL4PortMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system L4 Port DoS mode to %d\n", doSCfgData->systemL4PortMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSTCPPortSet(doSCfgData->systemL4PortMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system TCP Port DoS mode to %d\n", doSCfgData->systemL4PortMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_UDPPORT_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSUDPPortSet(doSCfgData->systemUDPPortMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system UDP Port DoS mode to %d\n", doSCfgData->systemUDPPortMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMPFRAG_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSICMPFragSet(doSCfgData->systemICMPFragMode) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system ICMP Fragment DoS mode to %d\n", doSCfgData->systemICMPFragMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSICMPSet(doSCfgData->systemICMPMode, doSCfgData->systemMaxICMPSize) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system ICMP DoS mode to %d\n", doSCfgData->systemICMPMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMPV4_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSICMPSet(doSCfgData->systemICMPMode, doSCfgData->systemMaxICMPSize) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system ICMPv4 DoS mode to %d\n", doSCfgData->systemICMPMode);
        rc = L7_FAILURE;
     }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMPV6_FEATURE_ID) == L7_TRUE)
  {
     if (dtlDoSICMPv6Set(doSCfgData->systemICMPMode, doSCfgData->systemMaxICMPv6Size) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                "Failure setting system ICMPv6 DoS mode to %d\n", doSCfgData->systemICMPMode);
        rc = L7_FAILURE;
     }
  }

    memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Now check for interfaces that have been enabled for Snooping.  All non-zero
     values in the Cfg structure contain an external interface number that is
     configured for Snooping.  Convert this to internal interface number and
     attempt to enable it.  The internal interface number is used as an index
     into the operational interface data */
  if (DOSCONTROL_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_DOS_MAX_INTF; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&doSCfgData->dosIntfCfgData[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(doSCfgData->dosIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
     rc = dosApplyIntfConfigData(intIfNum);
    }
  }

  return rc;

}


/*********************************************************************
* @purpose  Checks if DoS user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL doSHasDataChanged(void)
{
  return doSCfgData->cfgHdr.dataChanged;
}
void doSResetDataChanged(void)
{
  doSCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Saves policy user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSSave(void)
{
  L7_RC_t rc=L7_SUCCESS;

  if (doSCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    doSCfgData->cfgHdr.dataChanged = L7_FALSE;
    doSCfgData->checkSum = nvStoreCrc32((L7_char8 *)doSCfgData,
                                           sizeof(doSCfgData_t) - sizeof(doSCfgData->checkSum));

    if (sysapiCfgFileWrite(L7_DOSCONTROL_COMPONENT_ID,
                           DOSCONTROL_CFG_FILENAME,
                           (L7_char8 *)doSCfgData,
                           sizeof(doSCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",DOSCONTROL_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Restores policy user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 doSRestore(void)
{

  doSBuildDefaultConfigData(doSCfgData->cfgHdr.version);
  (void)doSApplyConfigData();

  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}
/*********************************************************************
*
* @purpose task to handle all Port DOS management messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void dosTask()
{
  dosMgmtMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_DOS_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(dosQueue, (void *)&msg, DOS_MSG_SIZE, L7_WAIT_FOREVER);
    dosIntfChangeProcess(msg.intIfNum, msg.dosIntfChangeParms.event, msg.dosIntfChangeParms.correlator);
  } while (1);

}
/*********************************************************************
* @purpose  Start Dos TASk
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosStartTask()
{
  dosTaskId = osapiTaskCreate("dosTask", dosTask, 0, 0,
                           dosSidDefaultStackSize(),
                           dosSidDefaultTaskPriority(),
                           dosSidDefaultTaskSlice());

  if (dosTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
            "Could not create task dosTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_DOS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
            "Unable to initialize dosTask()\n");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);

}
/*********************************************************************
* @purpose  Build default DoS config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dosBuildDefaultIntfConfigData(nimConfigID_t *configId, dosIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->intfPingFlooding   = FD_DOS_DEFAULT_PINGFLOODING_MODE;
  pCfg->intfSmurfAttack    = FD_DOS_DEFAULT_SMURFATTACK_MODE;
  pCfg->intfSynAckFlooding = FD_DOS_DEFAULT_SYNACK_MODE;

  pCfg->intfparams [DOSINTFPINGPARAM]   = FD_DOS_DEFAULT_PING_PARAM;
  pCfg->intfparams [DOSINTFSYNACKPARAM] = FD_DOS_DEFAULT_SYNACK_PARAM;

}

