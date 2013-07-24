/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  bootp_cnfgr.c
*
* @purpose   bootp configurator API
*
* @component BOOTP client
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

#include "bootp_cnfgr.h"
#include "l7_bootp.h"

/* BOOTP Configuraration protection semaphore.
*/
extern void * bootpConfigSem;

/* BOOTP Notification protection semaphore.
*/
extern void * bootpNotificationSem;

/* BOOTP notification callback functions list */
extern bootpNotifyList_t *bootpNotifyList;

/**************************************************************************
* @purpose  BOOTP Init Phase1 function.
*
* @comments None.
*
* @end
*************************************************************************/
#ifndef L7_PRODUCT_SMARTPATH
L7_RC_t bootpCnfgrInitPhase1Process()
{
  bootpNotifyList = (bootpNotifyList_t *)osapiMalloc(L7_BOOTP_COMPONENT_ID, sizeof(bootpNotifyList_t) * L7_LAST_COMPONENT_ID);  
  if( bootpNotifyList == L7_NULLPTR )
    return L7_ERROR;

  memset((void*)bootpNotifyList, 0, sizeof(bootpNotifyList_t) * L7_LAST_COMPONENT_ID);

  bootpConfigSemCreate();
  bootpNotificationSemCreate();
  bootpTaskStart();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes bootpCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void bootpCnfgrFiniPhase1Process()
{
  if (bootpNotifyList != L7_NULLPTR)
  {
    osapiFree(L7_BOOTP_COMPONENT_ID, bootpNotifyList);
    bootpNotifyList = L7_NULLPTR;
  }
   
  if (bootpNotificationSem != L7_NULLPTR)
  {
    osapiSemaDelete(bootpNotificationSem);
    bootpNotificationSem = L7_NULLPTR;
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

L7_RC_t bootpCnfgrUconfigPhase2()
{
  memset((void*)bootpNotifyList, 0, sizeof(bootpNotifyList_t) * L7_LAST_COMPONENT_ID);

  return L7_SUCCESS;
}
#endif
