/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dot1x_auth_serv_debug.c
*
* @purpose Dot1x Auth Server debug utilities
*
* @component Dot1x Authentication Server
*
* @comments none
*
* @create 11/17/2009
*
* @author pradeepk
*
* @end
*             
**********************************************************************/
#include "dot1x_auth_serv_api.h"
#include "commdefs.h"
#include "cli_web_exports.h"
#include "dot1x_auth_serv_exports.h"
#include "dot1x_auth_serv.h"

/*********************************************************************
 * @purpose  Print the current configured users to
 *           serial port
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBShow(void)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8  strPassword[L7_PASSWORD_SIZE + 1];
  L7_char8  strUserName[L7_LOGIN_SIZE];
  L7_uint32 userCount;
  L7_uint32 numOfUsers = 0;

  /*------------------------------------------------------*/
  /* Dump logins structure
   */
  /*------------------------------------------------------*/
  sysapiPrintf("Dot1x Users: ");

  for (userCount = 0; userCount < L7_MAX_IAS_USERS; userCount++ )
  {
    memset(strUserName, 0, sizeof(strUserName));
    if (dot1xAuthServUserDBUserNameGet(userCount, strUserName, sizeof(strUserName)) == L7_SUCCESS)
    {
      /* skip user if empty.  The name list will contain blanks if a user is deleted.  */
      if (strcmp(strUserName, "") == 0)
      {
        continue;
      }
      memset(strPassword, 0, sizeof(strPassword));
      if (dot1xAuthServUserDBUserPasswordClearTextGet(userCount, strPassword, sizeof(strPassword)) == L7_SUCCESS)
      {
        numOfUsers++;
        sysapiPrintf("\n");
        sysapiPrintf("\nUser Name: %s", strUserName);
        sysapiPrintf("\nPassword: %s ", strPassword);
      }
    }
  }
  sysapiPrintf("\n\nTotal Number of Configured Dot1x Users:%d\n",numOfUsers);
  return rc;
}


