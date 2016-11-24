/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  dhcp_cnfgr.c
*
* @purpose   dhcp configurator API
*
* @component DHCP client
*
* @comments
*
* @create    1/02/2008
*
* @author    ipopov
*
* @end
*
**********************************************************************/

#include "defaultconfig.h"
#include "dhcp_cnfgr.h"
#include "osapi.h"
#include "l7_dhcp.h"
#include "string.h"

/* DHCP Configuraration protection semaphore.
*/
extern void * dhcpConfigSem;

/* DHCP Notification protection semaphore.
*/
extern void * dhcpNotificationSem;

/* DHCP notification callback functions list */
extern dhcpNotifyList_t *dhcpNotifyList;

/* DHCP configurable Options.*/
extern dhcpOptions_t dhcpOptions;

/**************************************************************************
* @purpose  DHCP Init Phase1 function.
*
* @comments None.
*
* @end
*************************************************************************/
L7_RC_t dhcpCnfgrInitPhase1Process()
{
  dhcpNotifyList = (dhcpNotifyList_t *)osapiMalloc(L7_DHCP_COMPONENT_ID, sizeof(dhcpNotifyList_t) * L7_LAST_COMPONENT_ID);  
  if( dhcpNotifyList == L7_NULLPTR )
    return L7_ERROR;

  memset((void*)dhcpNotifyList, 0, sizeof(dhcpNotifyList_t) * L7_LAST_COMPONENT_ID);

  dhcpConfigSemCreate();
  dhcpNotificationSemCreate();
  dhcpTaskStart();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes dhcpCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpCnfgrFiniPhase1Process()
{
  if (dhcpNotifyList != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_COMPONENT_ID, dhcpNotifyList);
    dhcpNotifyList = L7_NULLPTR;
  }
   
  if (dhcpNotificationSem != L7_NULLPTR)
  {
    osapiSemaDelete(dhcpNotificationSem);
    dhcpNotificationSem = L7_NULLPTR;
  }
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpCnfgrUconfigPhase2()
{
/*  memset((void*)dhcpNotifyList, 0, sizeof(dhcpNotifyList_t) * L7_LAST_COMPONENT_ID);*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function Builds the default configuration for the DHCP Client
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpBuildDefaultConfig()
{
  /* Initialise to default configurtion.*/
  dhcpOptions.vendorClassOption.optionMode = FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE;
  osapiStrncpy(dhcpOptions.vendorClassOption.vendorClassString,
               FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING,
               DHCP_VENDOR_CLASS_STRING_MAX);
  return L7_SUCCESS;
}

