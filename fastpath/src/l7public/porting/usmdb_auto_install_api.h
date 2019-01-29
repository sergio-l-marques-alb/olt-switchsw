/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
*
* @filename  usmdb_auto_install_api.h
*
* @purpose   Provide interface to API for usmdb autoinstall
*
* @component auto_install
*
* @comments
*
* @create    29/01/2008
*
* @author    lkhedyk
*
* @end
*
**********************************************************************/

#ifndef USMDB_AUTO_INSTALL_API_H
#define USMDB_AUTO_INSTALL_API_H

#include "l7_common.h"

#ifdef L7_AUTO_INSTALL_PACKAGE
/*********************************************************************
*
* @purpose  Start/stop autoinstall procedure
*
* @param    L7_BOOL  start  @b((input)) start/stop flag to set. 
*                              ( L7_TRUE - start, L7_FALSE - stop )
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallStartStopSet(L7_BOOL start);

/*********************************************************************
*
* @purpose   Get current auto-istal state (started/stopped)
*
* @param     L7_BOOL  *start  @b((input)) auto-instal state
*                              ( L7_TRUE - started, L7_FALSE - stoped )
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallStartStopGet(L7_BOOL *start);

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
L7_RC_t usmdbAutoInstallAutoSaveSet(L7_BOOL save);
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
L7_RC_t usmdbAutoInstallAutoSaveGet(L7_BOOL *save);

/*********************************************************************
*
* @purpose  Enable or disable auto image download.
*
* @param    L7_BOOL  autoImage  @b((input)) Auto image download status to set. 
*                    L7_TRUE - downloaded image using TFTP options,
*                    L7_FALSE - disable downloading image.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallAutoImageSet(L7_BOOL autoImage);
/*********************************************************************
*
* @purpose  Get the auto image download status.
*
* @param    L7_BOOL  autoImage  @b((output)) Auto image download status. 
*                    L7_TRUE - downloaded image using TFTP options,
*                    L7_FALSE - disable downloading image.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallAutoImageGet(L7_BOOL *autoImage);

/*********************************************************************
*
* @purpose  Enable or disable auto reboot.
*
* @param    L7_BOOL  autoReboot  @b((input)) Auto Reboot status. 
*                    L7_TRUE - Reboot after successful image download,
*                    L7_FALSE - Administrative control to reboot when needed.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallAutoRebootSet(L7_BOOL autoReboot);
/*********************************************************************
*
* @purpose  Get the auto reboot status.
*
* @param    L7_BOOL  autoReboot  @b((input)) Auto Reboot status. 
*                    L7_TRUE - Reboot after successful image download,
*                    L7_FALSE - Administrative control to reboot when needed.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallAutoRebootGet(L7_BOOL *autoReboot);
/*********************************************************************
*
* @purpose  Set the retrycount for the DHCP specified configuration file.
*
* @param    L7_uint32  retryCntInfinite  @b((input)) Number of times the unicast 
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
L7_RC_t usmdbAutoInstallFileDownLoadRetryCountSet(L7_uint32 retryCntInfinite);
/*********************************************************************
*
* @purpose  Get the retrycount for the DHCP specified configuration file.
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
L7_RC_t usmdbAutoInstallFileDownLoadRetryCountGet(L7_uint32 *retryCnt);
/*********************************************************************
*
* @purpose   Get current auto-istal state (started/stopped)
*
* @param     L7_BOOL  *start  @b((input)) auto-instal state
*                              ( L7_TRUE - started, L7_FALSE - stoped )
* @purpose  Get the status of auto-install
*
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbAutoInstallCurrentStatusGet(L7_uchar8 *status);

#endif /* L7_AUTO_INSTALL_PACKAGE*/
#endif /* USMDB_AUTO_INSTALL_API_H */

