/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
* @filename  dot1x_auth_serv_cfg.c
*
* @purpose   Internal Dot1x Authentication Server Configuration file.
*
* @component Internal Dot1x Authentication Server
*
* @comments none
*
* @create 11/05/2010
*
* @author msaleem
*
* @end
*
**********************************************************************/

/* Common header file includes */
#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "comm_mask.h"
#include "nvstoreapi.h"

/* Component related header file includes */
#include "dot1x_auth_serv.h"
#include "dot1x_auth_serv_cfg.h"

extern dot1xAuthServCfgData_t dot1xAuthServCfgData;

/*********************************************************************
* @purpose  Build default IDAS config data
*
* @param    ver   @{{input}} Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void  dot1xAuthServBuildDefaultConfigData(L7_uint32 ver)
{
  memset((char*)&dot1xAuthServCfgData, 0, sizeof(dot1xAuthServCfgData_t));

  /* Build header */
  strcpy(dot1xAuthServCfgData.cfgHdr.filename,DOT1X_AUTH_SERV_CFG_FILENAME);
  dot1xAuthServCfgData.cfgHdr.version = ver;
  dot1xAuthServCfgData.cfgHdr.componentID = L7_DOT1X_AUTH_SERV_COMPONENT_ID;
  dot1xAuthServCfgData.cfgHdr.type = L7_CFG_DATA;
  dot1xAuthServCfgData.cfgHdr.length = sizeof(dot1xAuthServCfgData_t);
  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if IDAS user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1xAuthServHasDataChanged(void)
{
  return dot1xAuthServCfgData.cfgHdr.dataChanged;
}

/*********************************************************************
* @purpose  Restores  IDAS user config data change Flag
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL dot1xAuthServResetDataChanged(void)
{
  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_FALSE;
  return dot1xAuthServCfgData.cfgHdr.dataChanged;
}
/*********************************************************************
* @purpose  Saves IDAS user config file to NVStore
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
L7_RC_t dot1xAuthServSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (dot1xAuthServCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    dot1xAuthServCfgData.cfgHdr.dataChanged = L7_FALSE;
    dot1xAuthServCfgData.checkSum = nvStoreCrc32((L7_char8 *)&dot1xAuthServCfgData,
                                           sizeof(dot1xAuthServCfgData_t) - sizeof(dot1xAuthServCfgData.checkSum));

    if (sysapiCfgFileWrite(L7_DOT1X_AUTH_SERV_COMPONENT_ID,
                           DOT1X_AUTH_SERV_CFG_FILENAME,
                           (L7_char8 *)&dot1xAuthServCfgData,
                           sizeof(dot1xAuthServCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_DOT1X_AUTH_SERV_COMPONENT_ID,
      "\r\n Failed to write IDAS config to file %s \r \n",DOT1X_AUTH_SERV_CFG_FILENAME);
    }
  }
  return(rc);
}
/*********************************************************************
* @purpose  Restores IDAS user config file to factore defaults
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
L7_uint32 dot1xAuthServRestore(void)
{

  dot1xAuthServBuildDefaultConfigData(dot1xAuthServCfgData.cfgHdr.version);
  /* Implement a Apply Default Config Data if needed */

  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}


