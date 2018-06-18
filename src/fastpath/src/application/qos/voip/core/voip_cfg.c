/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_cfg.c
*
* @purpose VOIP configuration functions
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "sysapi.h"
#include "l7_product.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "voip.h"
#include "voip_cnfgr.h"
#include "platform_config.h"
#include "voip_util.h"
#include "voip_sid.h"
#include "voip_parse.h"
#include "osapi_support.h"

voipCfgData_t          *voipCfgData     = L7_NULLPTR;
L7_uint32 *voipMapTbl = L7_NULLPTR;
extern voipCnfgrState_t voipCnfgrState;
osapiRWLock_t           voipCfgRWLock;
voipIntfInfo_t          *voipIntfInfo = L7_NULLPTR;


/*********************************************************************
* @purpose  Saves VOIP file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSave(void)
{
  if (voipCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    voipCfgData->cfgHdr.dataChanged = L7_FALSE;
    voipCfgData->checkSum = nvStoreCrc32((L7_char8 *)voipCfgData,
                                          (sizeof (voipCfgData_t) - sizeof (voipCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_FLEX_QOS_VOIP_COMPONENT_ID, VOIP_CFG_FILENAME,
                           (L7_char8 *)voipCfgData, sizeof(voipCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
              "voipSave: Error on call to sysapiCfgFileWrite file %s\n", VOIP_CFG_FILENAME);
    }
  }
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Restores VOIP user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipRestore(void)
{
  L7_RC_t rc = L7_SUCCESS;

  voipBuildDefaultConfigData(voipCfgData->cfgHdr.version);
  voipCfgData->cfgHdr.dataChanged = L7_TRUE;
  if (voipApplyConfigData() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "%s: Failed to restore VOIP factory defaults\n", __FUNCTION__);
    rc = L7_FAILURE;
  }
  return rc;
}
/*********************************************************************
* @purpose  Checks if VOIP user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL voipHasDataChanged(void)
{
  return voipCfgData->cfgHdr.dataChanged;
}
void voipResetDataChanged(void)
{
  voipCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply VOIP Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipApplyConfigData(void)
{
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Now check for interfaces that have been enabled for Snooping.  All non-zero
     values in the Cfg structure contain an external interface number that is
     configured for Snooping.  Convert this to internal interface number and
     attempt to enable it.  The internal interface number is used as an index
     into the operational interface data */
  if (VOIP_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_VOIP_MAX_INTF; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&voipCfgData->voipIntfCfgData[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(voipCfgData->voipIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      voipApplyIntfConfigData(intIfNum);
    }
  }
  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Build default VOIP Interface config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void voipBuildDefaultIntfConfigData(nimConfigID_t *cfgId,
                                    voipIntfCfgData_t *pCfg)
{

  NIM_CONFIG_ID_COPY(&pCfg->configId, cfgId);
  /* build per-interface config defaults
   *
   * NOTE:  Instead of calling the default build functions here, copy
   *        the current global config data to establish the interface
   *        defaults.  After initialization, changes to global config
   *        are applied to all existing interfaces.  When a new interface
   *        is created, this function will establish the same values
   *        as all other interfaces (at least those that do not have
   *        per-interface config overrides).
   */

  pCfg->voipProfile = voipCfgData->voipIntfCfgData[0].voipProfile;
  pCfg->voipBandwidth = voipCfgData->voipIntfCfgData[0].voipBandwidth;
}
/*********************************************************************
* @purpose  Build default VOIP config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void voipBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_VOIP_MAX_INTF];

  /* save the config id's */
  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_VOIP_MAX_INTF);

  for (cfgIndex = 1; cfgIndex < L7_VOIP_MAX_INTF; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &voipCfgData->voipIntfCfgData[cfgIndex].configId);

  memset(( void * )voipCfgData, 0, sizeof( voipCfgData_t));

  for (cfgIndex = 1; cfgIndex < L7_VOIP_MAX_INTF; cfgIndex++)
    voipBuildDefaultIntfConfigData(&configId[cfgIndex], &voipCfgData->voipIntfCfgData[cfgIndex]);


  /*CFG Header */
  strcpy(voipCfgData->cfgHdr.filename, VOIP_CFG_FILENAME);
  voipCfgData->cfgHdr.version = ver;
  voipCfgData->cfgHdr.componentID = L7_FLEX_QOS_VOIP_COMPONENT_ID;
  voipCfgData->cfgHdr.type = L7_CFG_DATA;
  voipCfgData->cfgHdr.length = sizeof(voipCfgData_t);
  voipCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;

}


