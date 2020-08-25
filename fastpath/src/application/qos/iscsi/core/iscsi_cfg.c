/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_cfg.c
*
* @purpose iSCSI configuration functions
*
* @component iSCSI
*
* @comments none
*
* @create 04/18/2008
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "sysapi.h"
#include "l7_product.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "iscsi.h"
#include "iscsi_api.h"
#include "iscsi_cnfgr.h"
#include "platform_config.h"
#include "iscsi_sid.h"
#include "osapi_support.h"
#include "cli_txt_cfg_api.h"
#ifdef L7_NSF_PACKAGE
#include "iscsi_ckpt.h"
#endif

iscsiCfgData_t           *iscsiCfgData = L7_NULLPTR;
osapiRWLock_t             iscsiCfgRWLock;

iscsiTargetCfgData_t      iscsiTargetDefaultCfgData[] = FD_QOS_ISCSI_TARGET_TABLE_CONTENTS;
extern L7_BOOL            iscsiWarmStart;

/*********************************************************************
* @purpose  Saves ISCSI file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiSave(void)
{
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  if (iscsiCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    iscsiCfgData->cfgHdr.dataChanged = L7_FALSE;
    iscsiCfgData->checkSum = nvStoreCrc32((L7_char8 *)iscsiCfgData,
                                          (sizeof (iscsiCfgData_t) - sizeof (iscsiCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_FLEX_QOS_ISCSI_COMPONENT_ID, ISCSI_CFG_FILENAME,
                           (L7_char8 *)iscsiCfgData, sizeof(iscsiCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
              "iscsiSave: Error on call to sysapiCfgFileWrite file %s", ISCSI_CFG_FILENAME);
    }
  }
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Restores ISCSI user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiRestore(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* disable and clean up based on current configuration */
  iscsiAdminModeApply(L7_DISABLE);

  iscsiBuildDefaultConfigData(iscsiCfgData->cfgHdr.version);
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  if (iscsiApplyConfigData() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "Failed to restore ISCSI factory defaults");
    rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
* @purpose  Checks if ISCSI user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL iscsiHasDataChanged(void)
{
  return iscsiCfgData->cfgHdr.dataChanged;
}
void iscsiResetDataChanged(void)
{
  iscsiCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Apply ISCSI Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiApplyConfigData(void)
{
  L7_RC_t rc = L7_SUCCESS;

  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  /* disable admin mode in the active config before setting things up */
  /* apply global config items */
  if ((iscsiAdminModeApply(L7_DISABLE) != L7_SUCCESS) ||
      (iscsiAdminModeApply(iscsiCfgData->adminMode) != L7_SUCCESS))
  {
    rc = L7_FAILURE;
  }
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose Function registered with Text Based configuration.  It is
*          invoked when all text-based configuration has been pushed
*          to components.
*
* @param   L7_uint32   event  indicates status of text config completion
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 iscsiTxtCfgApplyCompletionCallback(L7_uint32 event)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Ignore events types we don't care about.  */
  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }

#ifdef L7_NSF_PACKAGE
  if (iscsiWarmStart == L7_TRUE)
  {
      rc = iscsiCkptApplyData();
  }
#endif

  /* signale configurator that iSCSI is ready for driver sync. */
  cnfgrApiComponentHwUpdateDone(L7_FLEX_QOS_ISCSI_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  return(rc);
}

/*********************************************************************
* @purpose  Build default ISCSI config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void iscsiBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  memset((void *)iscsiCfgData, 0, sizeof(iscsiCfgData_t));

  iscsiCfgData->adminMode = FD_QOS_ISCSI_ADMIN_MODE;
  iscsiCfgData->sessionTimeOutInterval = FD_QOS_ISCSI_SESSION_TIME_OUT_INTERVAL;
  iscsiCfgData->tagFieldSelector = FD_QOS_ISCSI_TAG_SELECTOR;

  /* check feature support for platform */
  if ((iscsiCfgData->tagFieldSelector == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P) &&
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID, L7_ISCSI_DOT1P_MARKING_FEATURE_ID) != L7_TRUE))
  {
    iscsiCfgData->markingEnabled = L7_FALSE;
  }
  else
  {
    iscsiCfgData->markingEnabled = FD_QOS_ISCSI_MARKING_ENABLED;
  }

  iscsiCfgData->vlanPriorityValue = FD_QOS_ISCSI_VPT_VALUE;
  iscsiCfgData->dscpValue = FD_QOS_ISCSI_DSCP_VALUE;
  iscsiCfgData->precValue = FD_QOS_ISCSI_PREC_VALUE;

  for (i=0; i<sizeof(iscsiTargetDefaultCfgData)/sizeof(iscsiTargetCfgData_t); i++)
  {
    iscsiCfgData->iscsiTargetCfgData[i].portNumber = iscsiTargetDefaultCfgData[i].portNumber;
    iscsiCfgData->iscsiTargetCfgData[i].ipAddress = iscsiTargetDefaultCfgData[i].ipAddress;
    osapiStrncpySafe(iscsiCfgData->iscsiTargetCfgData[i].targetName, iscsiTargetDefaultCfgData[i].targetName,
                     sizeof(iscsiCfgData->iscsiTargetCfgData[0].targetName));
    iscsiCfgData->iscsiTargetCfgData[i].inUse = L7_TRUE;
    iscsiCfgData->iscsiTargetCfgData[i].defaultCfgEntry = L7_TRUE;
    iscsiCfgData->numActiveTargetEntries++;
  }

  /*CFG Header */
  strcpy(iscsiCfgData->cfgHdr.filename, ISCSI_CFG_FILENAME);
  iscsiCfgData->cfgHdr.version = ver;
  iscsiCfgData->cfgHdr.componentID = L7_FLEX_QOS_ISCSI_COMPONENT_ID;
  iscsiCfgData->cfgHdr.type = L7_CFG_DATA;
  iscsiCfgData->cfgHdr.length = sizeof(iscsiCfgData_t);
  iscsiCfgData->cfgHdr.dataChanged = L7_FALSE;
  (void)osapiWriteLockGive(iscsiCfgRWLock);

  return;
}
