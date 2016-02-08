/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml_cfg.c
*
* @purpose Port MAC Locking configuration functions
*
* @component Port MAC Locking
*
* @comments none
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "sysapi.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "pml_api.h"
#include "pml.h"
#include "pml_util.h"

#include "platform_config.h"

pmlCfgData_t          *pmlCfgData     = L7_NULLPTR;
L7_uint32             *pmlMapTbl      = L7_NULLPTR;
void                  *pmlQueue       = L7_NULLPTR;
pmlIntfInfo_t         *pmlIntfInfo    = L7_NULLPTR;

extern pmlCnfgrState_t pmlCnfgrState;


/*********************************************************************
* @purpose  Saves Port MAC Locking file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 pmlSave(void)
{
  if (pmlCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    pmlCfgData->cfgHdr.dataChanged = L7_FALSE;

    pmlCfgData->checkSum = nvStoreCrc32((L7_char8 *)pmlCfgData, 
                                          (sizeof (pmlCfgData_t) - sizeof (pmlCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_PORT_MACLOCK_COMPONENT_ID, PML_CFG_FILENAME,
                           (L7_char8 *)pmlCfgData, sizeof(pmlCfgData_t)) != L7_SUCCESS)
    {
      LOG_MSG("pmlSave: Error on call to sysapiCfgFileWrite file %s\n", PML_CFG_FILENAME);
    }
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Checks if Port MAC Locking user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL pmlHasDataChanged(void)
{
  return pmlCfgData->cfgHdr.dataChanged;
}
void pmlResetDataChanged(void)
{
  pmlCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply Port MAC Locking Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t pmlApplyConfigData(void)
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
  if (PML_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_PML_MAX_INTF; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(pmlCfgData->pmlIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      pmlApplyIntfConfigData(intIfNum);
    }
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Build default Port MAC Locking Intf config data  
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void pmlBuildDefaultIntfConfigData(nimConfigID_t *configId, pmlIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);
  pCfg->violationTrapsSeconds = FD_PML_INTF_VIOLATION_TRAP_SECS;
  pCfg->intfLockEnabled       = FD_PML_INTF_LOCK_MODE;
  pCfg->violationTrapsEnabled = FD_PML_INTF_VIOLATION_TRAP_MODE;
  pCfg->dynamicLimit          = L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES;
  pCfg->staticLimit           = L7_MACLOCKING_MAX_STATIC_ADDRESSES;
  memset(&pCfg->staticMacEntry[0], 0, L7_MACLOCKING_MAX_STATIC_ADDRESSES * sizeof(pmlMac_t));
}

/*********************************************************************
* @purpose  Build default Port MAC Locking config data  
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    default is NO Multicast MAC entries
*
* @end
*********************************************************************/
void pmlBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_PML_MAX_INTF];

  /* save the config id's */
  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_PML_MAX_INTF);

  for (cfgIndex = 1; cfgIndex < L7_PML_MAX_INTF; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &pmlCfgData->pmlIntfCfgData[cfgIndex].configId);

  memset(( void * )pmlCfgData, 0, sizeof( pmlCfgData_t)); 

  for (cfgIndex = 1; cfgIndex < L7_PML_MAX_INTF; cfgIndex++)
    pmlBuildDefaultIntfConfigData(&configId[cfgIndex], &pmlCfgData->pmlIntfCfgData[cfgIndex]);

  for(cfgIndex = 0; cfgIndex < L7_MAX_INTERFACE_COUNT; cfgIndex++)
    memset(&pmlIntfInfo[cfgIndex].acquiredList, 0x00, sizeof(AcquiredMask));

  strcpy(pmlCfgData->cfgHdr.filename, PML_CFG_FILENAME);

  pmlCfgData->cfgHdr.version = ver;

  pmlCfgData->cfgHdr.componentID = L7_PORT_MACLOCK_COMPONENT_ID;

  pmlCfgData->cfgHdr.type = L7_CFG_DATA;

  pmlCfgData->cfgHdr.length = sizeof(pmlCfgData_t);

  pmlCfgData->cfgHdr.dataChanged = L7_FALSE;

  pmlCfgData->globalLockEnabled = FD_PML_ADMIN_MODE;

  return;
}

