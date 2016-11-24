/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/websrc/telnet/cli_lvl7.c
 *
 * @purpose transfer commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  08/09/2000
 *
 * @author  Forrest Samuels
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include "strlib_base_common.h"
#include "l7_common.h"
#include "osapi.h"                      /*lvl7*/
#include "defaultconfig.h"              /*lvl7*/

#include "ews.h"
#include "ews_telnet.h"
#include "ewnet.h"
#include "ews_api.h"

#include "cli.h"
#include "cliutil.h"
#include "clicommands.h"
#include "cliapi.h"
#include "usmdb_user_mgmt_api.h"
#include <string.h>

/*
 * ewaNetTelnetData
 * Application-provided function to process data from telnet session.
 * Here, we implement a simple CLI for illustration purposes.
 *
 * We use APP_BUFFER to buffer up a line, where APP_DATA is the current
 * index to APP_BUFFER.
 *
 *   handle - application network handle
 *   datap  - pointer to telnet data
 *   bytesp - (input/output) pointer to number of bytes in buffer, updated
 *            by number of bytes consumed.
 */
L7_char8 * ewaNetTelnetData ( EwaNetHandle handle, L7_char8 * datap,uintf * bytesp)
{
  L7_char8 loginbuffer[2]={0};                                                /*make #define for size*/
  EwsContext ewsContext = handle->context;
  L7_char8 * aSplat = "*";
  L7_uint32 maxInputLength = 0;
  static L7_BOOL has_CR = L7_FALSE;
  static L7_char8 strChar[4];

  /*
   * must set the handle before anything else, otherwise
   * you will be using some other connections' data
   */
  cliCurrentHandleSet(handle->handleNum);
  cliCurrentSocketSet(handle->socket);

  if ( (cliResetSerialConnectionGet() == L7_TRUE)
      && (cliCurrentHandleGet() == CLI_SERIAL_HANDLE_NUM) )
  {
    handle->logged_in = FALSE;

    /* clear runtime cli variables */
    cliInitConnection(cliCurrentHandleGet());

    /* Reset parser state */
    ewsCliStart(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliSystemPromptGet());

    cliResetSerialConnectionSet(L7_FALSE);
  }

  /* do not update activity time for a serial connection that
     is not logged in because a loginSession has not been created */
  if (!((!handle->logged_in) && (cliTelnetConnectionGet() == L7_FALSE)))
  {
    usmDbLoginSessionUpdateActivityTime(U_IDX, cliLoginSessionIndexGet());
  }

  /*
   * If not yet logged in
   */
  if (!handle->logged_in)
  {
    /* We only process one byte at a time. */
    *bytesp = *bytesp - 1;

    if (*datap == 0)
    {
      return NULL;
    }                                                                     /* skip nuls in CRNUL */

    /* Read line in APP_BUFFER until '\r' or buffer full */
    if (L7_PASSWORD_SIZE >= L7_LOGIN_SIZE)
    {
      maxInputLength = L7_PASSWORD_SIZE;
    }
    else
    {
      maxInputLength = L7_LOGIN_SIZE;
    }

    if ((*datap != '\r' && *datap != '\n')
        && (APP_DATA <= maxInputLength) && (APP_DATA < APP_BUFFER_SIZE -2))
    {

      if (*datap == EWS_CLI_CHAR_DEL_BACK0 || *datap == EWS_CLI_CHAR_DEL_BACK1)
      {
        if (APP_DATA != 0)
        {
          APP_DATA--;

          ewsTelnetWrite(ewsContext, "\b");
          ewsTelnetWrite(ewsContext, " ");
          ewsTelnetWrite(ewsContext, "\b");
          ewsFlushAll(ewsContext);
        }
        return NULL;
      }
      else
      {
        if (*datap >= ' ')
        {
          APP_BUFFER[APP_DATA++] = *datap;

          if (cliHaveUserGet() == L7_FALSE)
          {
            strncpy(loginbuffer,datap,1);
            ewsTelnetWrite(ewsContext,loginbuffer);
            ewsFlushAll(ewsContext);
          }
          else
          {
            return aSplat;
          }
        }
        return NULL;
      }
    }

    /* Terminate string and reset for next time */
    APP_BUFFER[APP_DATA] = 0;
    APP_DATA = 0;

    if (cliCommon[cliUtil.handleNum].loginStatus == L7_FALSE)
    {
      if ( cliLoginCheck(APP_BUFFER, ewsContext) == L7_TRUE )
      {
        cliCommon[cliUtil.handleNum].passwdExpStatus = L7_TRUE;
        cliCommon[cliUtil.handleNum].loginStatus = L7_TRUE;
        if (cliPasswdExpireCheck (APP_BUFFER, ewsContext) == L7_TRUE)
        {
          /*
           * Successful login, change prompt and state.
           */
          cliCommon[cliUtil.handleNum].loginStatus = L7_FALSE;
          ewaLogHook(ewsContext, EWS_LOG_STATUS_OK);
          handle->logged_in = TRUE;
          cliCommandsInit();
          return cliPrompt(ewsContext);;
        }
        else
        {
          /* password is expired; force user to enter new password */
          return NULL;
        }
      }
      else
      {
        return NULL;
      }
    }
    /* take user's new password, check its validity, store it, and log the user in */
    else if (cliCommon[cliUtil.handleNum].passwdExpStatus == L7_TRUE)
    {
      if (configExpiredPasswd (ewsContext, APP_BUFFER, &cliCommon[cliUtil.handleNum].callCount) == L7_TRUE)
      {
        cliCommon[cliUtil.handleNum].passwdExpStatus = L7_FALSE;
        cliCommon[cliUtil.handleNum].loginStatus = L7_FALSE;
        cliCommon[cliUtil.handleNum].callCount = 0;
        /*
         * Successful login, change prompt and state.
         */
        ewaLogHook(ewsContext, EWS_LOG_STATUS_OK);
        handle->logged_in = TRUE;
        cliCommandsInit();
        return cliPrompt(ewsContext);

      }
      else
      {
        return NULL;
      }
    }
  }

  /*
   * Pass to CLI
   */

  /* Ignore NULL characters */
  if (*datap == '\0')
  {
    *bytesp = *bytesp - 1;
    return NULL;
  }

  /* Ignore LF's following CR's */
  if (*datap == '\r')
  {
    has_CR = L7_TRUE;
  }
  else
  {
    if (*datap == '\n' && has_CR == L7_TRUE)
    {
      has_CR = L7_FALSE;

      /* Ignore this character */
      *bytesp = *bytesp - 1;
      return NULL;
    }
    has_CR = L7_FALSE;
  }

  if (cliCheckScrollInput() == L7_TRUE)
  {
    cliProcessScrollInput(handle->context, datap, bytesp);
    return NULL;
  }

  cliProcessCharInput(datap);

  if (cliProcessStringInput(datap) == L7_TRUE)
  {
    *bytesp = *bytesp - 1;

    if (cliGetStringPassword() == L7_TRUE)
    {
      if ((datap[0] != '\b') && (datap[0] != '\177'))
      {
        if (*datap == 0)
        {
          return NULL;
        }
        else
        {
          return aSplat;
        }
      }
      else
      {
        strcpy(strChar, "\b \b\0");
        return strChar;
      }
    }
    strChar[0] = datap[0];
    strChar[1] = 0;
    return strChar;
  }

  return (L7_char8 *) ewsCliData(handle->context ,datap ,bytesp ,mainMenu[cliCurrentHandleGet()]);
}
