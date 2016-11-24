/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  voice_vlan_cfg.c
*
* @purpose   voice vlan configuration file
*
* @component voice vlan
*
* @comments  none
*
* @create  Jan 11, 2007
*
* @author  PKB
*
* @end
*
**********************************************************************/

#include <string.h>

#include "voice_vlan_cnfgr.h"
#include "voice_vlan.h"
#include "voice_vlan_cfg.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "voice_vlan_api.h"

voiceVlanCfg_t   *voiceVlanCfg = L7_NULLPTR;    
extern voiceVlanPortCnfgrState_t voiceVlanPortCnfgrState;
extern L7_VLAN_MASK_t voiceVlanMask;
extern L7_ushort16 * voiceVlanIntfCount;

/*********************************************************************
* @purpose  Saves voice vlan configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments voicevlanCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t voiceVlanSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if(voiceVlanHasDataChanged() == L7_TRUE)
  {
    voiceVlanCfg->cfgHdr.dataChanged = L7_FALSE;
    voiceVlanCfg->checkSum = nvStoreCrc32( (L7_uchar8 *)voiceVlanCfg,
                                       (L7_uint32)(sizeof(voiceVlanCfg_t) - sizeof(voiceVlanCfg->checkSum)) );

    if((rc = sysapiCfgFileWrite(L7_VOICE_VLAN_COMPONENT_ID, VOICE_VLAN_CFG_FILENAME, (L7_char8 *)voiceVlanCfg,
                                (L7_int32)sizeof(voiceVlanCfg_t))) != L7_SUCCESS)
    {
      rc = L7_ERROR;
      LOG_MSG("voiceVlanSave: Error on call to sysapiCfgFileWrite for config file %s\n", VOICE_VLAN_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Restores voice vlan default configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments voiceVlanCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t voiceVlanRestore(void)
{
  /* unapply the existing config */
  voiceVlanAdminModeSet(FD_VOICE_VLAN_ADMIN_MODE);
   
  /* build the default config */
  voiceVlanBuildDefaultConfigData(voiceVlanCfg->cfgHdr.version);

  voiceVlanCfg->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if voice vlan user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL voiceVlanHasDataChanged(void)
{
  return voiceVlanCfg->cfgHdr.dataChanged;

}
void voiceVlanResetDataChanged(void)
{
  voiceVlanCfg->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print the current voice vlan config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanCfgDump(void)
{
  L7_char8 buf[32];
  L7_uint32 i;
  L7_uint32 intIfNum;
  voiceVlanPortCfg_t *pCfg;
  nimConfigID_t configIdNull;
  nimUSP_t usp;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  sysapiPrintf("\n");
  sysapiPrintf("VOICE VLAN\n");
  sysapiPrintf("=====\n");

  if (voiceVlanCfg->voiceVlanAdminMode == L7_ENABLE)
    osapiSnprintf(buf, sizeof(buf),"Enable");
  else
    osapiSnprintf(buf,sizeof(buf),"Disable");
  sysapiPrintf("Admin Mode - %s\n\n", buf);

  if (voiceVlanCfg->voiceVlanTraceMode == L7_ENABLE)
    osapiSnprintf(buf, sizeof(buf),"Enable");
  else
    osapiSnprintf(buf,sizeof(buf),"Disable");
  sysapiPrintf("Log Trace Mode - %s\n\n", buf);

  sysapiPrintf("Interface configuration:\n");

  for(i = 1; i < L7_VOICE_VLAN_INTF_MAX_COUNT; i++)
  {
    if(NIM_CONFIG_ID_IS_EQUAL(&voiceVlanCfg->portCfg[i].configId, &configIdNull))
      continue;
    if(nimIntIfFromConfigIDGet(&(voiceVlanCfg->portCfg[i].configId), &intIfNum) != L7_SUCCESS)
      continue;

    if(voiceVlanIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if((nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS))
      {
        sysapiPrintf("USP: %d.%d.%d, intIfNum: %d\n", usp.unit, usp.slot, usp.port, intIfNum);
      } else
      {
        sysapiPrintf("Failed getting USP for intIfNum %d\n", intIfNum);
        continue;
      }

      if(pCfg->adminMode == L7_ENABLE)
        osapiSnprintf(buf, sizeof(buf),"Enable");
      else
        osapiSnprintf(buf,sizeof(buf),"Disable");
      sysapiPrintf("  Intf Admin Mode - %s\n\n", buf);

      sysapiPrintf("  Voice VLAN Id = %d\n",pCfg->voiceVlanId);
      sysapiPrintf("  Port Prioity  = %d\n",pCfg->portPriority);

      switch(pCfg->untaggingEnabled)
      {
        case L7_TRUE:
          osapiSnprintf(buf, sizeof(buf),"True");
          break;
        case L7_FALSE:
          osapiSnprintf(buf, sizeof(buf), "False");
          break;
        default:
          osapiSnprintf(buf,sizeof(buf), "N/A");
          break;
      }
      sysapiPrintf("  Tagging Enabled:        %s\n", buf);

      switch(pCfg->defaultCosOverride)
      {
        case L7_TRUE:
          osapiSnprintf(buf, sizeof(buf),"True");
          break;
        case L7_FALSE:
          osapiSnprintf(buf, sizeof(buf), "False");
          break;
        default:
          osapiSnprintf(buf,sizeof(buf), "N/A");
          break;
      }
      sysapiPrintf("  Default Cos Override:        %s\n", buf);

      switch (pCfg->authState)
      {
        case L7_ENABLE:
          osapiSnprintf(buf, sizeof(buf), "Enable");
          break;
        case L7_DISABLE:
          osapiSnprintf(buf, sizeof(buf), "Disable");
          break;
        default:
          osapiSnprintf(buf, sizeof(buf), "N/A");
          break;
      }
      sysapiPrintf("  Authentication Mode:        %s\r\n", buf);

      /*dscp*/
      sysapiPrintf("  Voice VLAN dscp = %d\n",pCfg->dscp);
    }
  }

  sysapiPrintf("=============\n");
  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  Build default voice vlan config data
*
* @param    ver @((input)) Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void voiceVlanBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_VOICE_VLAN_INTF_MAX_COUNT];


  /* Save the config IDs */
  memset((void *)&configId[0], 0, sizeof(nimConfigID_t) * L7_VOICE_VLAN_INTF_MAX_COUNT);
  for (cfgIndex = 1; cfgIndex < L7_VOICE_VLAN_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &voiceVlanCfg->portCfg[cfgIndex].configId);

  memset((void *)voiceVlanCfg, 0, sizeof(voiceVlanCfg_t));


  for(cfgIndex = 1; cfgIndex < L7_VOICE_VLAN_INTF_MAX_COUNT; cfgIndex++)
    voiceVlanBuildDefaultIntfConfigData(&configId[cfgIndex], &voiceVlanCfg->portCfg[cfgIndex]);

  strcpy((L7_char8 *)voiceVlanCfg->cfgHdr.filename, VOICE_VLAN_CFG_FILENAME);

  voiceVlanCfg->cfgHdr.version = ver;
  voiceVlanCfg->cfgHdr.componentID = L7_VOICE_VLAN_COMPONENT_ID;
  voiceVlanCfg->cfgHdr.type = L7_CFG_DATA;
  voiceVlanCfg->cfgHdr.length = (L7_uint32)sizeof(voiceVlanCfg_t);
  voiceVlanCfg->cfgHdr.dataChanged = L7_FALSE;

  voiceVlanCfg->voiceVlanAdminMode = FD_VOICE_VLAN_ADMIN_MODE;
  voiceVlanCfg->voiceVlanTraceMode = FD_VOICE_VLAN_TRACE_MODE;

  return;
}

/*********************************************************************
* @purpose  Build default voice vlan port config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/

void voiceVlanBuildDefaultIntfConfigData(nimConfigID_t *configId, voiceVlanPortCfg_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->adminMode           = FD_VOICE_VLAN_ADMIN_MODE;
  if (pCfg->voiceVlanId != FD_VOICE_VLAN_VLAN_ID)
  {
    voiceVlanIntfCount[pCfg->voiceVlanId]--;
    if (voiceVlanIntfCount[pCfg->voiceVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(voiceVlanMask,pCfg->voiceVlanId);
    }
  }

  pCfg->voiceVlanId         = FD_VOICE_VLAN_VLAN_ID;
  pCfg->portPriority        = FD_VOICE_VLAN_PRIORITY;
  pCfg->untaggingEnabled    = FD_VOICE_VLAN_TAGGING;
  pCfg->noneMode            = FD_VOICE_VLAN_NONE_MODE;
  pCfg->defaultCosOverride  = FD_VOICE_VLAN_OVERRIDE;
  pCfg->authState           = FD_VOICE_VLAN_AUTH_STATE;
  pCfg->dscp                = FD_VOICE_VLAN_DSCP;

  voiceVlanCfg->cfgHdr.dataChanged = L7_FALSE;
}


/*********************************************************************
* @purpose  Apply voice vlan config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanApplyConfigData(void)
{
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  /* Now check for ports that are valid for voice vlan.  All non-zero values in the
   * config structure contain a port config ID that is valid for voice vlan.
   * Convert this to internal interface number and apply the port config.
   * The internal interface number is used as an index into the operational
   * interface data.
   */
  if(VOICE_VLAN_READY)
  {
    memset(&configIdNull, 0, sizeof(nimConfigID_t));

    for(cfgIndex = 1; cfgIndex < L7_VOICE_VLAN_INTF_MAX_COUNT; cfgIndex++)
    {
      if(NIM_CONFIG_ID_IS_EQUAL(&voiceVlanCfg->portCfg[cfgIndex].configId, &configIdNull))
        continue;
      if(nimIntIfFromConfigIDGet(&(voiceVlanCfg->portCfg[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      if(voiceVlanApplyPortConfigData(intIfNum) != L7_SUCCESS)
      {
        LOG_MSG("Failure in applying config data for interface %u\n", intIfNum);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply voice vlan config data to specified interface
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments                                               
*
* @end
*********************************************************************/
L7_RC_t voiceVlanApplyPortConfigData(L7_uint32 intIfNum)
{
  voiceVlanPortCfg_t *pCfg;
  L7_RC_t rc;

  if(voiceVlanIntfIsConfigurable(intIfNum,&pCfg) != L7_TRUE)
    return L7_FAILURE;

  if(voiceVlanCfg->voiceVlanAdminMode == L7_ENABLE && 
     pCfg->adminMode == L7_ENABLE)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Interface [%d] Voice Vlan Admin Mode [%d] Port admin mode[%d]\r\n",
                       __FUNCTION__, intIfNum,voiceVlanCfg->voiceVlanAdminMode,pCfg->adminMode);
    voiceVlanPortAuthSet(intIfNum, pCfg->authState);
    rc = voiceVlanPortStatusSet(intIfNum,L7_ENABLE);
    return rc;
  } 
  else
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Interface [%d] Voice Vlan Admin Mode [%d] Port admin mode[%d]\r\n",
                       __FUNCTION__, intIfNum,voiceVlanCfg->voiceVlanAdminMode,pCfg->adminMode);
    voiceVlanPortAuthSet(intIfNum, L7_ENABLE);
    rc = voiceVlanPortStatusSet(intIfNum,L7_DISABLE);
    return rc;
  }
}

/*********************************************************************
* @purpose  Get the Configuration Data Pointer
*
* @param    None
*
* @returns  Pointer to the Voice vlan Configuration.
*
* @comments                                               
*
* @end
*********************************************************************/
voiceVlanCfg_t *voiceVlanGetCfg(void)
{
  return voiceVlanCfg;
}

