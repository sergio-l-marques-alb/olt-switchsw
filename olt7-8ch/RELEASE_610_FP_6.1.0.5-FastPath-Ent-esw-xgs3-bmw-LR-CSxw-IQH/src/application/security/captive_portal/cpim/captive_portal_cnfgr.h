
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     captive_portal_cnfgr.h
*
* @purpose      Captive portal configuration header
*
* @component    captive portal
*
* @comments     none
*
* @create       06/25/2007
* 
* @author       dcaugherty
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_CAPTIVE_PORTAL_CNFGR_H
#define INCLUDE_CAPTIVE_PORTAL_CNFGR_H

#include "l7_cnfgr_api.h"
#include "captive_portal_common.h"


typedef enum
{
  CAPTIVE_PORTAL_PHASE_INIT_0 = 0,
  CAPTIVE_PORTAL_PHASE_INIT_1,
  CAPTIVE_PORTAL_PHASE_INIT_2,
  CAPTIVE_PORTAL_PHASE_WMU,
  CAPTIVE_PORTAL_PHASE_INIT_3,
  CAPTIVE_PORTAL_PHASE_EXECUTE,
  CAPTIVE_PORTAL_PHASE_UNCONFIG_1,
  CAPTIVE_PORTAL_PHASE_UNCONFIG_2,
} captivePortalCnfgrState_t;


/*
Internal variable prototypes
*/


void    captivePortalApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*
Internal function prototypes
*/

L7_RC_t         
captivePortalTaskStart(void);


/*********************************************************************
* @purpose  Checks if Captive Portal config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Call should probably be protected by semaphore. 
*
* @end
*********************************************************************/
L7_BOOL cpConfigHasDataChanged(void);
void cpResetDataChanged(void);

/*********************************************************************
* @purpose  Builds factory default configuration data
*
* @param    L7_uint32 version - configuration format version to use
*
* @returns  void
*
* @end
*********************************************************************/
void cpBuildDefaultConfigData(L7_uint32 version);

/*********************************************************************
*
* @purpose  Restore CP images NVRAM format to default.
*
* @param    L7_uchar8 *images @b{(input)} pointer to NVRAM cfg
*
* @returns  void
*
* @notes  This function is called to build default data, this would
*           be done on a "clear config". As such, the images
*           structure is cleared included the RAM disk.
*
* @end
*********************************************************************/
void cpImageCfgDefault(L7_uchar8 *images);

/*********************************************************************
* @purpose  cpConfigSave
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Call should probably be protected by semaphore. 
*
* @end
*********************************************************************/
L7_RC_t cpConfigSave(void);


/*********************************************************************
* @purpose  Read existing configuration from NV store.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Call should probably be protected by semaphore. 
*
* @end
*********************************************************************/
L7_RC_t cpConfigRead(void);

void cpConfigApply(void);


#endif /* INCLUDE_CAPTIVE_PORTAL_CFG_H */

