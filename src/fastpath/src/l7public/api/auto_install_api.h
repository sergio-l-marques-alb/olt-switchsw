/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  auto_install_api.h
*
* @purpose   auto-istall APIs
*
* @component auto-install
*
* @comments
*
* @create    30/01/2008
*
* @author    lkhedyk
*
* @end
**********************************************************************/
#ifndef _AUTO_INSTALL_API_H_
#define _AUTO_INSTALL_API_H_


#include "l7_common.h"

/* Events specifying when the component registering with AutoInstall would need 
   to be notified.*/
typedef enum
{
  AUTO_INSTALL_SPECIFIED_FILE_FAILED = 1,
  AUTO_INSTALL_PREDEFINED_FILE_FAILED = 2
}autoInstallNotifyEvent_t;

/* The function prototype of the callbacks registering with the AutoInstall. */
typedef struct eventNotifyList_s
{
  L7_COMPONENT_IDS_t  compId;      /* Component */
  L7_uint32           notifyEvent; /* this is mask from cases mentioned in autoInstallNotifyEvent_t*/
  L7_RC_t (*notifyChange)(void);
}eventNotifyList_t;

#ifdef L7_AUTO_INSTALL_PACKAGE

/*********************************************************************
*
* @purpose  Start/stop autoinstall procedure
*
* @param    L7_BOOL  start  @b((input)) start/stop flag to set. 
 *                                      L7_TRUE - start, L7_FALSE - stop 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallStartStopSet(L7_BOOL start);

/*********************************************************************
*
* @purpose  Get current auto-istal state (started/stopped)
*
* @param     L7_BOOL  *start   @b((input)) start/stop flag to set. 
*                                          L7_TRUE - start, L7_FALSE - stop
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallStartStopGet(L7_BOOL *start);
/*********************************************************************
*
* @purpose  Enable or disable auto saving the installed configuration.
*
* @param    L7_BOOL  save  @b((input)) Auto save flag to set. 
*                                      L7_TRUE - save downloaded config by default,
*                                      L7_FALSE - prompt user for explicit save 
*                                                 when config is dowmloaded.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallAutoSaveSet(L7_BOOL save);

/*********************************************************************
*
* @purpose  Get auto saving status.
*
* @param     L7_BOOL  *start   @b((output)) Auto save flag to set. 
*                              L7_TRUE - save downloaded config by default,
*                              L7_FALSE - prompt user for explicit save 
*                                         when config is dowmloaded.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallAutoSaveGet(L7_BOOL *save);

/*********************************************************************
*
* @purpose  Set the retrycount for the DHCP specified configuration file.
*
* @param    L7_uint32  retryCnt  @b((input)) Number of times the unicast 
*                                        TFTP tries should be made for the DHCP 
*                                        specified file before falling back for 
*                                        broadcast TFTP tries.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallFileDownLoadRetryCountSet(L7_uint32 retryCnt);

/*********************************************************************
*
* @purpose  Set the retrycount for the DHCP specified configuration file.
*
* @param    L7_uint32  cnt  @b((output)) Number of times the unicast TFTP tries 
*                                        should be made for the DHCP specified 
*                                        file before falling back for broadcast 
*                                        TFTP tries.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallFileDownLoadRetryCountGet(L7_uint32 *retryCnt);
/*********************************************************************
*
* @purpose  Get the status of auto-install
*
* @param    L7_uchar8 *status @b((output)) Current autoInstall status
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallCurrentStatusGet(L7_uchar8 *status);
/*********************************************************************
*
* @purpose  Register with AutoInstall for notification of config file 
*           attempt failure.
*
* @param     L7_COMPONENT_IDS_t       compId   @b((input)) Component being 
*                                                          registered. 
* @param     autoInstallNotifyEvent_t event    @b((input)) event signifying
*                                                           when to notify. 
* @param     L7_RC_t (*notifyFn)()    notifyFn @b((input)) Callback function 
*                                                           registered. 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallNotifyRegister(L7_COMPONENT_IDS_t compId, 
                                  L7_uint32  notifyEventMask,
                                  L7_RC_t (*notifyFn)());
/*********************************************************************
*
* @purpose  Register with AutoInstall for notification of config file 
*           attempt failure.
*
* @param     L7_COMPONENT_IDS_t       compId   @b((input)) Component being 
*                                                          registered. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallNotifyDeregister(L7_COMPONENT_IDS_t compId);

#endif /* L7_AUTO_INSTALL_PACKAGE*/
#endif /* _AUTO_INSTALL_API_H_ */
 
