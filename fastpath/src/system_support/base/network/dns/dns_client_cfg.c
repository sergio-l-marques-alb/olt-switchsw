/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_cfg.c
*
* @purpose DNS client configuration functions
*
* @component DNS client
*
* @comments none
*
* @create 02/28/2005
*
* @author dfowler
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "nvstoreapi.h"
#include "l7_product.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "dns_client_api.h"
#include "dns_client.h"
#include "dns_client_util.h"
#include "dns_client_cache.h"

void                  *dnsQueue         = L7_NULLPTR;
void                  *dnsSemaphore     = L7_NULLPTR;
dnsCfgData_t          *dnsCfgData       = L7_NULLPTR;
dnsOprData_t          *dnsOprData       = L7_NULLPTR;
dnsCacheData_t        *dnsCacheData     = L7_NULLPTR;
dnsNotifyEntry_t      *dnsNotifyTbl     = L7_NULLPTR;
L7_uint32              dnsCachePoolId   = 0;
L7_BOOL                dnsAdminModeReady  = L7_FALSE;
L7_BOOL                dnsInitAdminMode   = FD_DNS_CLIENT_ADMIN_MODE_ENABLED;


/*********************************************************************
* @purpose  Saves DNS client file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dnsSave(void)
{
  if (dnsCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    dnsCfgData->cfgHdr.dataChanged = L7_FALSE;

    dnsCfgData->checkSum = nvStoreCrc32((L7_char8 *)dnsCfgData,
                                        (sizeof (dnsCfgData_t) - sizeof (dnsCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_DNS_CLIENT_COMPONENT_ID, DNS_CLIENT_CFG_FILENAME,
                           (L7_char8 *)dnsCfgData, sizeof(dnsCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
              "dnsSave: Error on call to sysapiCfgFileWrite file %s\n", DNS_CLIENT_CFG_FILENAME);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if DNS client user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dnsHasDataChanged(void)
{
  return dnsCfgData->cfgHdr.dataChanged;
}
void dnsResetDataChanged(void)
{
  dnsCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply DNS client Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dnsApplyConfigData(void)
{
  return dnsClientAdminModeApply(dnsCfgData->enabled);
}

/*********************************************************************
* @purpose  Build default DNS client configuration data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    default is disabled with zero name servers.
*
* @end
*********************************************************************/
void dnsBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 srvIndex;

  memset((void *)dnsCfgData, 0, sizeof(dnsCfgData_t));

  strcpy(dnsCfgData->cfgHdr.filename, DNS_CLIENT_CFG_FILENAME);

  dnsCfgData->cfgHdr.version = ver;
  dnsCfgData->cfgHdr.componentID = L7_DNS_CLIENT_COMPONENT_ID;
  dnsCfgData->cfgHdr.type = L7_CFG_DATA;
  dnsCfgData->cfgHdr.length = sizeof(dnsCfgData_t);
  dnsCfgData->cfgHdr.dataChanged = L7_FALSE;
  if (cnfgrIsFeaturePresent(L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
    /* Initialize admin mode only after text based config
       apply is complete */
    dnsAdminModeReady   = L7_FALSE;
    dnsInitAdminMode    = FD_DNS_CLIENT_ADMIN_MODE_ENABLED;
    dnsCfgData->enabled = L7_FALSE;
  }
  else
  {
    dnsAdminModeReady = L7_TRUE;
    dnsCfgData->enabled = FD_DNS_CLIENT_ADMIN_MODE_ENABLED;
  }
  dnsCfgData->requestTimeout = FD_DNS_CLIENT_REQUEST_TIMEOUT;
  dnsCfgData->queryTimeout = FD_DNS_CLIENT_QUERY_TIMEOUT;
  dnsCfgData->retries    = FD_DNS_NO_OF_RETRIES;

  for (srvIndex = 0; srvIndex < L7_DNS_NAME_SERVER_ENTRIES; srvIndex++)
  {
    /* If nothing has been set, the default family would be L7_AF_INET.*/
    inetAddressZeroSet(L7_AF_INET, &dnsCfgData->servers[srvIndex]);
  }
  (void)dnsClientNameLookupResponseRegister(L7_DNS_CLIENT_COMPONENT_ID, dnsNameLookupResponseMessageSend);

  /* there is no default domain, or domain list, or default hosts, or servers */

  return;
}
#ifdef L7_CLI_PACKAGE
/*********************************************************************
* @purpose  DNS callback to process configuration apply completion event
*
* @param    L7_uint32 event @b{(input)} txtCfgApplyEvent_t type event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dnsApplyConfigCompleteCallback(L7_uint32 event)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsAdminModeReady == L7_FALSE)
  {
    dnsAdminModeReady = L7_TRUE;
    dnsCfgData->enabled = dnsInitAdminMode;
    if (dnsClientAdminModeApply(dnsCfgData->enabled) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DNS_CLIENT_COMPONENT_ID,
             "dnsApplyConfigCompleteCallback: Failed to %s DNS Admin Mode ", 
             dnsCfgData->enabled ? "Enable" : "Disable");
    }
  }
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}
#endif
