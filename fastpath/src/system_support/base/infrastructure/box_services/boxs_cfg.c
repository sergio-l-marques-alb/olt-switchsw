/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  boxs_cfg.c
*
* @purpose   Box Services configuration file
*
* @component boxs
*
* @comments
*
* @create    01/15/2008
*
* @author    vkozlov
*
* @end
*
**********************************************************************/

#include "commdefs.h"

#include "boxs.h"
#include "support_api.h"
#include "defaultconfig.h"
#include "boxs_defaultconfig.h"
#include "nvstoreapi.h"
#include "boxs_cfg.h"

extern boxsCfg_t * boxsCfg;

void  *boxsSemaphore = L7_NULLPTR;

/*********************************************************************
* @purpose  Saves boxs configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments boxsCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t boxsSave(void)
{
    L7_RC_t rc;

    rc = L7_SUCCESS;

    if (boxsHasDataChanged() == L7_TRUE)
    {
      boxsCfg->hdr.dataChanged = L7_FALSE;
      boxsCfg->checkSum = nvStoreCrc32((L7_uchar8 *)boxsCfg, (L7_uint32)(sizeof(boxsCfg_t) - sizeof(boxsCfg->checkSum)));

      if (sysapiCfgFileWrite(L7_BOX_SERVICES_COMPONENT_ID, BOXS_CFG_FILENAME, (L7_char8 *)boxsCfg,
                             (L7_int32)sizeof(boxsCfg_t)) == L7_ERROR)
      {
        rc = L7_ERROR;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
                "Error on call to sysapiCfgFileWrite routine on config file %s\n", BOXS_CFG_FILENAME);
      }
    }
    return(rc);
}

/*********************************************************************
* @purpose  Checks if boxs user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL boxsHasDataChanged(void)
{
  return boxsCfg->hdr.dataChanged;
}

void boxsResetDataChanged(void)
{
  boxsCfg->hdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Build default boxs config data
*
* @param    ver @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void boxsBuildDefaultConfigData(L7_uint32 ver)
{
  memset((void *)&boxsCfg->cfg, 0, sizeof(boxsCfgData_t));

  /* setup file header */
  boxsCfg->hdr.version                       = ver;
  boxsCfg->hdr.componentID                   = L7_BOX_SERVICES_COMPONENT_ID;
  boxsCfg->hdr.type                          = L7_CFG_DATA;
  boxsCfg->hdr.length                        = (L7_uint32)sizeof(boxsCfg_t);
  boxsCfg->hdr.dataChanged                   = L7_FALSE;


  boxsCfg->cfg.globalCfgData.minTemp         = FD_BOXS_DEFAULT_MIN_ALLOWED_TEMP;
  boxsCfg->cfg.globalCfgData.maxTemp         = FD_BOXS_DEFAULT_MAX_ALLOWED_TEMP;
  boxsCfg->cfg.globalCfgData.trapTempStatusEnabled      = FD_BOXS_DEFAULT_TEMPSTATUS_TRAP_ENABLED;
  boxsCfg->cfg.globalCfgData.trapFanStatusEnabled       = FD_BOXS_DEFAULT_FANSTATUS_TRAP_ENABLED;
  boxsCfg->cfg.globalCfgData.trapPowSupplyStatusEnabled = FD_BOXS_DEFAULT_POWSUPPLYSTATUS_TRAP_ENABLED;
  boxsCfg->cfg.globalCfgData.trapSfpStatusEnabled       = FD_BOXS_DEFAULT_SFP_TRAP_ENABLED;
  boxsCfg->cfg.globalCfgData.trapXfpStatusEnabled       = FD_BOXS_DEFAULT_XFP_TRAP_ENABLED;

  osapiStrncpy((L7_char8 *)boxsCfg->hdr.filename, BOXS_CFG_FILENAME,
      sizeof(boxsCfg->hdr.filename));
}

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void boxsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  return;
}


