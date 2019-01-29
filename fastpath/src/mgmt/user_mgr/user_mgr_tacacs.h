
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_tacacs.h
*
* @purpose   function prototypes and structure definitions for user_mgr_tacacs.c
*
* @component userMgr component
*
* @comments  none
*
* @create    6/03/05
*
* @author    dfowler
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef INCLUDE_USER_MGR_TACACS_H
#define INCLUDE_USER_MGR_TACACS_H

#include "l7_common.h"
#include "user_manager_exports.h"
#include "user_mgr_api.h"

/* Message structure to hold responses TACACS */
typedef struct userMgrTacacsAuthMessage_s
{
  L7_uint32 status;
  L7_uint32 accessLevel;
} userMgrTacacsAuthMessage_t;

/*********************************************************************
* @purpose  Determine if a user is authenticated by TACACS
*
* @param   L7_char8  *pUserName    @((input)) ptr to user name to authenticate
* @param   L7_char8  *pPwd         @((input)) ptr to user provided password or challenge response
* @param   L7_uint32 *pAccessLevel @((input)) access of authenticated user
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user,
*                      bad password, or system error
* @returns L7_ERROR    If attempt timed out
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t userMgrAuthenticateUserTacacs(userMgrAuthRequest_t *request);

/*********************************************************************
* @purpose  Determine if a user cmd is authorized by TACACS
*
* @param   userMgrAuthRequest_t     *request @((input/output))
*
* @returns L7_SUCCESS  If cmd is authorized
* @returns L7_FAILURE  If cmd is not authorized
* @returns L7_ERROR    If attempt timed out/system error
*
* @notes
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t userMgrCmdAuthorTacacs(userMgrAuthRequest_t *request);

/*********************************************************************
* @purpose Initialize the userMgr for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrTacacsPhaseTwoInit();

/*********************************************************************
* @purpose Clean up phase 2 resources
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrTacacsPhaseTwoFini();

#endif /* INCLUDE_USER_MGR_TACACS_H */
