
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_radius.h
*
* @purpose   function prototypes and structure definitions for user_mgr_radius.c
*
* @component userMgr component
*
* @comments  none
*
* @create    05/14/03
*
* @author    jflanagan
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef INCLUDE_USER_MGR_RADIUS_H
#define INCLUDE_USER_MGR_RADIUS_H

#include "l7_common.h"
#include "user_mgr_tacacs.h"

/* Message structure to hold responses RADIUS */
typedef struct userMgrAuthMessage_s
{
  L7_uint32 status;      /* status of response */
  L7_uint32 respLen;     /* length of data (response) being passed */
  L7_uchar8 *pResponse;  /* pointer to response from auth server */
} userMgrAuthMessage_t;

typedef struct userMgrMsg_s
{
  L7_uint32 event;
  L7_uint32 correlator;
  union
  {
    userMgrAuthMessage_t        userMgrAuthMsg;
    userMgrTacacsAuthMessage_t  userMgrTacacsAuthMsg;
  } data;
} userMgrMsg_t;

typedef enum userMgrQueueEvents_s
{
  USER_MGR_REMOTE_AUTH_RESPONSE = 100,
  USER_MGR_REMOTE_TACACS_AUTH_RESPONSE
} userMgrQueueEvents_t;


/*********************************************************************
* @purpose  Setup required to handle RADIUS
*
* @param   pUserName (input) ptr to user name to authenticate
* @param   pPwdOrChallengeResponse (input) ptr to user provided password or challenge response
* @param   pState (input/output) output value of previous challenge or new query state
* @param   pAccessLevel (output) access of authenticated user
* @param   pChallengeFlag (output) indicates if failure caused by challenge (L7_TRUE = challenged)
* @param   pChallengePhrase (output) challenge phrase to display to the user.
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, 
*                      challenge (indicated by challenge flag), or system error
* @returns L7_ERROR  If attempt timed out
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t 
userMgrRadiusInit(void);


/*********************************************************************
* @purpose  Determine if a user is authenticated by RADIUS
*
* @param   pUserName (input) ptr to user name to authenticate
* @param   pPwdOrChallengeResponse (input) ptr to user provided password or challenge response
* @param   pState (input/output) output value of previous challenge or new query state
* @param   pAccessLevel (output) access of authenticated user
* @param   pChallengeFlag (output) indicates if failure caused by challenge (L7_TRUE = challenged)
* @param   pChallengePhrase (output) challenge phrase to display to the user.
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, 
*                      challenge (indicated by challenge flag), or system error
* @returns L7_ERROR  If attempt timed out
*
* @notes   
*
* @notes TO DO: On an initial authentication request, state should be set to ????
*       
* @end
*********************************************************************/
L7_RC_t 
userMgrAuthenticateUserRadius(L7_char8 *pUserName, L7_char8 *pPwdOrChallengeResponse,
                        L7_uchar8 *pState, L7_uint32 *pAccessLevel,
                        L7_BOOL *pChallengeFlag, L7_char8 *pChallengePhrase );

/*********************************************************************
* @purpose Initialize the userMgr for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrRadiusPhaseOneInit();

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
L7_RC_t userMgrRadiusPhaseTwoInit();

/*********************************************************************
* @purpose Initialize the userMgr for Phase 1
*
* @param   void
*
* @returns void
*
* @end
*********************************************************************/
void userMgrRadiusPhaseOneFini();

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
L7_RC_t userMgrRadiusPhaseTwoFini();


#endif /* INCLUDE_USER_MGR_RADIUS_H */
