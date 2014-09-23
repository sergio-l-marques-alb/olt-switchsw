/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpcm.h
*
* @purpose      Captive Portal Client Credential Manager (CPCM) header
*
* @component    CPCM
*
* @comments     none
*
* @create       08/08/2007
*
* @author       darsenault
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_CPCM_H
#define INCLUDE_CPCM_H

#include "datatypes.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"

#define CPCM_MAX_AUTH_ATTEMPTS_B4_YIELD  16


typedef struct cpcmAuthInProgressStatusData_s
{
  L7_IP_ADDR_t                      ipAddr;    /* AVL KEY, Client MAC waiting for authorization */
  cpcmAuthInProgressStatusDesc_t    desc;
  cpConnectionLimits_t              limits;
  void *                            avlPtr;
} cpcmAuthInProgressStatusData_t;

/*****************************************************************************
*
* @purpose  This function is called from the common CP task when it receives
*           an authentication request. This function cycles through the aip
*           table and loops until there are no more wip status entries. This
*           prevents any user request from getting lost while we're busy.
*           Upon finding a WIP entry, authentication will be performed and 
*           as a result, the associated tables are updated appropriately.
*           Specifically, if authentication is successful, then the user
*           is deleted from the AIP table and added to the connected client
*           table. If authentication failed for any reason, the AIP status
*           is updated. The AIP entries will eventually get removed by the
*           common CP task.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmTryAuth(void);


#endif /* INCLUDE_CPCM_H */

