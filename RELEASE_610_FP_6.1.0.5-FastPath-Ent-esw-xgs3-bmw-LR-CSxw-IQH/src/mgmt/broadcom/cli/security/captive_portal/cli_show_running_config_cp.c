/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/security/captive_portal/cli_show_running_config_cp.c
*
* @purpose running config for Captive portal
*
* @component user interface
*
* @comments none
*
* @create  08/13/2007
*
* @author  rjain, rjindal
*
* @end
*
**********************************************************************/

#include "strlib_security_cli.h"
#include "commdefs.h"
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "l7_productconfig.h"
#include "cliapi.h"
#include "cli_show_running_config.h"
#include "config_script_api.h"
#include "osapi_support.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_user_api.h"
#include "usmdb_cpdm_web_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"

static L7_BOOL cp_entered_cmd = L7_FALSE;
static L7_BOOL config_entered_cmd = L7_FALSE;
static L7_BOOL locale_entered_cmd = L7_FALSE;

void cp_enter_cmd_init() { cp_entered_cmd = L7_FALSE; }

static void cp_enter_cmd(EwsContext ewsContext)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (cp_entered_cmd != L7_TRUE)
  {
    /* enter captive portal mode */
    cp_entered_cmd = L7_TRUE;
    osapiSnprintf(buf, sizeof(buf), "\r\ncaptive-portal");
    EWSWRITEBUFFER(ewsContext, buf);
  }
}

static void cp_exit_cmd(EwsContext ewsContext)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (cp_entered_cmd == L7_TRUE)
  {
    /* exit captive portal mode */
    cp_entered_cmd = L7_FALSE;
    osapiSnprintf(buf, sizeof(buf), "\r\nexit \r\n");
    EWSWRITEBUFFER(ewsContext, buf);
  }
}

void config_enter_cmd_init() { config_entered_cmd = L7_FALSE; }

static void config_enter_cmd(EwsContext ewsContext, cpId_t cpId)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (config_entered_cmd != L7_TRUE)
  {
    config_entered_cmd = L7_TRUE;
    osapiSnprintf(buf, sizeof(buf), "\r\nconfiguration %d", cpId);
    EWSWRITEBUFFER(ewsContext, buf);
  }
}

static void config_exit_cmd(EwsContext ewsContext)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (config_entered_cmd == L7_TRUE)
  {
    config_entered_cmd = L7_FALSE;
    osapiSnprintf(buf, sizeof(buf), "\r\nexit \r\n");
    EWSWRITEBUFFER(ewsContext, buf);
  }
}

void locale_enter_cmd_init() { locale_entered_cmd = L7_FALSE; }

static void locale_enter_cmd(EwsContext ewsContext, webId_t webId)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (locale_entered_cmd != L7_TRUE)
  {
    locale_entered_cmd = L7_TRUE;
    osapiSnprintf(buf, sizeof(buf), "\r\nlocale %d", webId);
    EWSWRITEBUFFER(ewsContext, buf);
  }
}

static void locale_exit_cmd(EwsContext ewsContext)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (locale_entered_cmd == L7_TRUE)
  {
    locale_entered_cmd = L7_FALSE;
    osapiSnprintf(buf, sizeof(buf), "\r\nexit \r\n");
    EWSWRITEBUFFER(ewsContext, buf);
  }
}

/*********************************************************************
* @purpose  Print Captive Portal Global running configuration info
*           for locales.
*
* @param    EwsContext  ewsContext  @b{(input)} EWS Context Handle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a helper function with local scope
*
* @end
*********************************************************************/
static L7_RC_t cliRunningConfigCPInfoConfigurationLocales(EwsContext ewsContext, cpId_t cpId)
{
  L7_char8 buf[(L7_CLI_MAX_LARGE_STRING_LENGTH*2)-1]; /* currently the max */
  L7_char8 tmp[CP_AUP_TEXT_MAX+1]; /* max UTF-16 format */
  L7_char8 code[CP_LANG_CODE_MAX+1];
  L7_uint32 idx,jdx;
  webId_t webId;

  locale_enter_cmd_init();

  webId = 0;
  while (L7_SUCCESS == usmDbCpdmCPConfigWebIdNextGet(cpId,webId,&webId))
  {
    usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,code);

    /* code - special because its the index for defaults */
    if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
        ((CP_WEB_ID_MIN==webId) && (osapiStrncmp(WEB_DEF1_LANG_CODE,code,CP_LANG_CODE_MAX)!=0)) ||
        (CP_WEB_ID_MIN!=webId))
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      locale_enter_cmd(ewsContext, webId);
      memset(buf, 0x00, sizeof(buf));
      osapiSnprintf(buf, sizeof(buf), "\r\n%s \"%s\"", CP_PARAM_NAME_CODE, code);
      EWSWRITEBUFFER(ewsContext, buf);
    }

    /* account-image */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebAccountImageNameGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_ACCOUNT_IMAGE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_ACCOUNT_IMAGE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s \"%s\"", CP_PARAM_NAME_ACCOUNT_IMAGE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* account-label */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebAccountLabelGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_ACCOUNT_LABEL,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_ACCOUNT_LABEL);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_ACCOUNT_LABEL, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* accept-msg */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebNoAcceptMsgTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_ACCEPT_MSG,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_ACCEPT_MSG);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_ACCEPT_MSG, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* accept-text */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebAcceptTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_ACCEPT_TEXT,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_ACCEPT_TEXT);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_ACCEPT_TEXT, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* aup-text (chunk it) */
    {
      L7_char8 CMD[] = "\r\naup-text ";
      L7_char8 CMD_LEN = 11;

      memset(tmp, 0x00, sizeof(tmp));
      memset(buf, 0x00, sizeof(buf));
      if (usmDbCpdmCPConfigWebAUPTextGet(cpId,webId,tmp) == L7_SUCCESS)
      {
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
            (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_AUP_TEXT,tmp) != L7_TRUE))
        {
          cp_enter_cmd(ewsContext);
          config_enter_cmd(ewsContext, cpId);
          locale_enter_cmd(ewsContext, webId);
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_AUP_TEXT);
          EWSWRITEBUFFER(ewsContext, buf);

          memset(buf, 0x00, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), "%s", CMD);
          jdx = CMD_LEN;
          for (idx=0; (('\0'!=tmp[idx]) && (jdx<sizeof(buf))); idx++,jdx++ )
          {
            if (jdx==sizeof(buf)-1)
            {
              EWSWRITEBUFFER(ewsContext, buf);
              memset(buf, 0x00, sizeof(buf));
              osapiSnprintf(buf, sizeof(buf), "%s", CMD);
              jdx = CMD_LEN;
            }
            buf[jdx] = tmp[idx];
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }

    /* button-label */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebButtonLabelGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_BUTTON_LABEL,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_BUTTON_LABEL, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* background-image */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebBackgroundImageNameGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_BACKGROUND_IMAGE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_BACKGROUND_IMAGE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s \"%s\"", CP_PARAM_NAME_BACKGROUND_IMAGE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* branding-image */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebBrandingImageNameGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_BRANDING_IMAGE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_BRANDING_IMAGE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s \"%s\"", CP_PARAM_NAME_BRANDING_IMAGE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* browser-title */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebBrowserTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_BROWSER_TITLE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_BROWSER_TITLE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_BROWSER_TITLE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* denied-msg */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebDeniedMsgTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_DENIED_MSG,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_DENIED_MSG, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* font-list */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebFontListGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_FONT_LIST,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_FONT_LIST);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s \"%s\"", CP_PARAM_NAME_FONT_LIST, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* instructional-text (chunk it) */
    {
      L7_char8 CMD[] = "\r\ninstructional-text ";
      L7_char8 CMD_LEN = 21;

      memset(tmp, 0x00, sizeof(tmp));
      memset(buf, 0x00, sizeof(buf));
      if (usmDbCpdmCPConfigWebInstructionalTextGet(cpId,webId,tmp) == L7_SUCCESS)
      {
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
            (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_INSTRUCTIONAL_TEXT,tmp) != L7_TRUE))
        {
          cp_enter_cmd(ewsContext);
          config_enter_cmd(ewsContext, cpId);
          locale_enter_cmd(ewsContext, webId);
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_INSTRUCTIONAL_TEXT);
          EWSWRITEBUFFER(ewsContext, buf);

          memset(buf, 0x00, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), "%s", CMD);
          jdx = CMD_LEN;
          for (idx=0; (('\0'!=tmp[idx]) && (jdx<sizeof(buf))); idx++,jdx++ )
          {
            if (jdx==sizeof(buf)-1)
            {
              EWSWRITEBUFFER(ewsContext, buf);
              memset(buf, 0x00, sizeof(buf));
              osapiSnprintf(buf, sizeof(buf), "%s", CMD);
              jdx = CMD_LEN;
            }
            buf[jdx] = tmp[idx];
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }

    /* link */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLocaleLinkGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LINK,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LINK, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* password-label */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebPasswordLabelGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_PASSWORD_LABEL,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_PASSWORD_LABEL);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_PASSWORD_LABEL, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* resource-msg */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebResourceMsgTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_RESOURCE_MSG,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_RESOURCE_MSG, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* title-text */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_TITLE_TEXT,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_TITLE_TEXT);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_TITLE_TEXT, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* timeout-msg */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebTimeoutMsgTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_TIMEOUT_MSG,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_TIMEOUT_MSG, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* user-label */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebUserLabelGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_USER_LABEL,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_USER_LABEL);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_USER_LABEL, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* welcome-title */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebWelcomeTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_WELCOME_TITLE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_WELCOME_TITLE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_WELCOME_TITLE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* welcome-text (chunk it) */
    {
      L7_char8 CMD[] = "\r\nwelcome-text ";
      L7_char8 CMD_LEN = 15;

      memset(tmp, 0x00, sizeof(tmp));
      memset(buf, 0x00, sizeof(buf));
      if (usmDbCpdmCPConfigWebWelcomeTextGet(cpId,webId,tmp) == L7_SUCCESS)
      {
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
            (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_WELCOME_TEXT,tmp) != L7_TRUE))
        {
          cp_enter_cmd(ewsContext);
          config_enter_cmd(ewsContext, cpId);
          locale_enter_cmd(ewsContext, webId);
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_WELCOME_TEXT);
          EWSWRITEBUFFER(ewsContext, buf);

          memset(buf, 0x00, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), "%s", CMD);
          jdx = CMD_LEN;
          for (idx=0; (('\0'!=tmp[idx]) && (jdx<sizeof(buf))); idx++,jdx++ )
          {
            if (jdx==sizeof(buf)-1)
            {
              EWSWRITEBUFFER(ewsContext, buf);
              memset(buf, 0x00, sizeof(buf));
              osapiSnprintf(buf, sizeof(buf), "%s", CMD);
              jdx = CMD_LEN;
            }
            buf[jdx] = tmp[idx];
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }

    /* wip-msg */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebWipMsgTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_WIP_MSG,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_WIP_MSG, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* script-text */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebScriptTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_SCRIPT_TEXT,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_SCRIPT_TEXT);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_SCRIPT_TEXT, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* popup-text */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebPopupTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_POPUP_TEXT,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_POPUP_TEXT);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_POPUP_TEXT, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-browser-title */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutBrowserTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_BROWSER_TITLE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_BROWSER_TITLE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LOGOUT_BROWSER_TITLE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-title */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_TITLE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_TITLE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LOGOUT_TITLE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-text (chunk it) */
    {
      L7_char8 CMD[] = "\r\nlogout-text ";
      L7_char8 CMD_LEN = 14;

      memset(tmp, 0x00, sizeof(tmp));
      memset(buf, 0x00, sizeof(buf));
      if (usmDbCpdmCPConfigWebLogoutContentTextGet(cpId,webId,tmp) == L7_SUCCESS)
      {
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
            (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_CONTENT,tmp) != L7_TRUE))
        {
          cp_enter_cmd(ewsContext);
          config_enter_cmd(ewsContext, cpId);
          locale_enter_cmd(ewsContext, webId);
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_CONTENT);
          EWSWRITEBUFFER(ewsContext, buf);

          memset(buf, 0x00, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), "%s", CMD);
          jdx = CMD_LEN;
          for (idx=0; (('\0'!=tmp[idx]) && (jdx<sizeof(buf))); idx++,jdx++ )
          {
            if (jdx==sizeof(buf)-1)
            {
              EWSWRITEBUFFER(ewsContext, buf);
              memset(buf, 0x00, sizeof(buf));
              osapiSnprintf(buf, sizeof(buf), "%s", CMD);
              jdx = CMD_LEN;
            }
            buf[jdx] = tmp[idx];
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }

    /* logout-button-label */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutButtonLabelGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_BUTTON_LABEL,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LOGOUT_BUTTON_LABEL, tmp);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-confirmation-text */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutConfirmTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_CONFIRM_TEXT,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_CONFIRM_TEXT);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LOGOUT_CONFIRM_TEXT, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-success-browser-title */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_SUCCESS_BROWSER_TITLE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_SUCCESS_BROWSER_TITLE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LOGOUT_SUCCESS_BROWSER_TITLE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-success-title */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutSuccessTitleTextGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_SUCCESS_TITLE_TEXT,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_SUCCESS_TITLE_TEXT);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s", CP_PARAM_NAME_LOGOUT_SUCCESS_TITLE_TEXT, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* logout-success-text (chunk it) */
    {
      L7_char8 CMD[] = "\r\nlogout-success-text ";
      L7_char8 CMD_LEN = 22;

      memset(tmp, 0x00, sizeof(tmp));
      memset(buf, 0x00, sizeof(buf));
      if (usmDbCpdmCPConfigWebLogoutSuccessContentTextGet(cpId,webId,tmp) == L7_SUCCESS)
      {
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
            (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_SUCCESS_CONTENT_TEXT,tmp) != L7_TRUE))
        {
          cp_enter_cmd(ewsContext);
          config_enter_cmd(ewsContext, cpId);
          locale_enter_cmd(ewsContext, webId);

          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_SUCCESS_CONTENT_TEXT);
          EWSWRITEBUFFER(ewsContext, buf);

          memset(buf, 0x00, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), "%s", CMD);
          jdx = CMD_LEN;
          for (idx=0; (('\0'!=tmp[idx]) && (jdx<sizeof(buf))); idx++,jdx++ )
          {
            if (jdx==sizeof(buf)-1)
            {
              EWSWRITEBUFFER(ewsContext, buf);
              memset(buf, 0x00, sizeof(buf));
              osapiSnprintf(buf, sizeof(buf), "%s", CMD);
              jdx = CMD_LEN;
            }
            buf[jdx] = tmp[idx];
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }

    /* logout-success-background-image */
    memset(tmp, 0x00, sizeof(tmp));
    memset(buf, 0x00, sizeof(buf));
    if (usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId,webId,tmp) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_LOGOUT_SUCCESS_BACKGROUND_IMAGE,tmp) != L7_TRUE))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        locale_enter_cmd(ewsContext, webId);
        if (0==strlen(tmp))
        {
          osapiSnprintf(buf, sizeof(buf), "\r\nno %s", CP_PARAM_NAME_LOGOUT_SUCCESS_BACKGROUND_IMAGE);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s \"%s\"", CP_PARAM_NAME_LOGOUT_SUCCESS_BACKGROUND_IMAGE, tmp);
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    locale_exit_cmd(ewsContext);

  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print Captive Portal Global running configuration info
*
* @param    EwsContext  ewsContext  @b{(input)} EWS Context Handle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t cliRunningConfigCPInfoGlobal(EwsContext ewsContext)
{
  L7_uint32 val = 0;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbCpdmGlobalStatusSessionTimeoutGet(&val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_AUTH_SESSION_TIMEOUT)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nauthentication timeout %u", val);
      EWSWRITEBUFFER(ewsContext, buf);    
    }
  }

  if (usmDbCpdmGlobalHTTPPortGet(&val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_HTTP_PORT)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nhttp port %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (usmDbCpdmGlobalHTTPSecurePort2Get(&val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_HTTP_AUX_SECURE_PORT2)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nhttps port %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

#endif

  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    if (usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet(&val) == L7_SUCCESS)
    {
      if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_PS_STATS_REPORT_INTERVAL)
      {
        cp_enter_cmd(ewsContext);
        osapiSnprintf(buf, sizeof(buf), "\r\nstatistics interval %u", val);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }

  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_AUTH_FAILURE, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_DEFAULT_TRAP_AUTH_FAILURE)
    {
      cp_enter_cmd(ewsContext);
      if (val == L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\ntrapflags client-auth-failure");
      }
      else 
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno trapflags client-auth-failure");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_CONNECTED, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_DEFAULT_TRAP_CLIENT_CONNECTED)
    {
      cp_enter_cmd(ewsContext);
      if (val == L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\ntrapflags client-connect");
      }
      else 
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno trapflags client-connect");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CONNECTION_DB_FULL, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_DEFAULT_TRAP_CONNECTION_DB_FULL)
    {
      cp_enter_cmd(ewsContext);
      if (val == L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\ntrapflags client-db-full");
      }
      else 
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno trapflags client-db-full");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_DISCONNECTED, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_DEFAULT_TRAP_CLIENT_DISCONNECTED)
    {
      cp_enter_cmd(ewsContext);
      if (val == L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\ntrapflags client-disconnect");
      }
      else 
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno trapflags client-disconnect");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print Captive Portal configuration running config info
*
* @param    EwsContext  ewsContext  @b{(input)} EWS Context Handle
* @param    cpId_t      cpId        @b{(input)} configuration ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t cliRunningConfigCPInfoConfiguration(EwsContext ewsContext, cpId_t cpId)
{
  L7_uint32 val, intf, u, s, p;
  cpId_t cpId_next;
  gpId_t gId;
  L7_CP_MODE_STATUS_t mode;
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  L7_uchar8 redirectMode, userLogoutMode;
  L7_char8 name[CP_NAME_MAX+1];
  L7_char8 urlBuf[CP_WELCOME_URL_MAX+1];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  config_enter_cmd_init();

  /* cp configuration */
  if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || (cpId != CP_ID_MIN))
  {
    cp_enter_cmd(ewsContext);
    config_enter_cmd(ewsContext, cpId);
  }

  /* cp name */
  memset(name, 0x00, sizeof(name));
  if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || (strcmp(name, CP_DEF_NAME) != 0))
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nname \"%s\"", name);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* cp mode */
  if (usmDbCpdmCPConfigModeGet(cpId, &mode) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || mode != CP_DEF_MODE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      if (mode == L7_CP_MODE_ENABLED)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nenable");
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno enable");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* protocol */
  if (usmDbCpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || protocol != CP_DEF_PROTOCAL_MODE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      if (protocol == L7_LOGIN_TYPE_HTTP)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nprotocol http");
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nprotocol https");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* verification */
  if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || verifyMode != CP_DEF_VERIFY_MODE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      if (verifyMode == CP_VERIFY_MODE_GUEST)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nverification guest");
      }
      else if (verifyMode == CP_VERIFY_MODE_LOCAL)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nverification local");
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nverification radius");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* radius authentication server */
  if (verifyMode == CP_VERIFY_MODE_RADIUS)
  {
    memset(name, 0x00, sizeof(name));
    if (usmDbCpdmCPConfigRadiusAuthServerGet(cpId, name) == L7_SUCCESS)
    {
      if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || (strcmp(name, CP_DEF_RADIUS_AUTH_SERVER) != 0))
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        osapiSnprintf(buf, sizeof(buf), "\r\nradius-auth-server \"%s\"", name);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }

  /* group */
  if (verifyMode != CP_VERIFY_MODE_GUEST)
  {
    if (usmDbCpdmCPConfigGpIdGet(cpId, &gId) == L7_SUCCESS)
    {
      if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || gId != GP_ID_MIN)
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        osapiSnprintf(buf, sizeof(buf), "\r\ngroup %d", gId);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }

  /* user logout mode */
  if (usmDbCpdmCPConfigUserLogoutModeGet(cpId, &userLogoutMode) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || userLogoutMode != CP_DEF_USER_LOGOUT_MODE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      if (userLogoutMode == L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nuser-logout");
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno user-logout");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* redirect mode */
  if (usmDbCpdmCPConfigRedirectModeGet(cpId, &redirectMode) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || redirectMode != CP_DEF_REDIRECT_MODE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      if (redirectMode == L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nredirect");
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nno redirect");
      }
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* redirect url */
  if (redirectMode == L7_ENABLE)
  {
    memset(urlBuf, 0x00, sizeof(urlBuf));
    if (usmDbCpdmCPConfigRedirectURLGet(cpId, urlBuf) == L7_SUCCESS)
    {
      if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || strcmp(name, CP_DEF_URL) != 0)
      {
        cp_enter_cmd(ewsContext);
        config_enter_cmd(ewsContext, cpId);
        osapiSnprintf(buf, sizeof(buf), "\r\nredirect-url %s", urlBuf);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }

  /* session timeout */
  if (usmDbCpdmCPConfigSessionTimeoutGet(cpId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_SESSION_TIMEOUT)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nsession-timeout %d", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* idle timeout */
  if (usmDbCpdmCPConfigIdleTimeoutGet(cpId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_IDLE_TIMEOUT)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nidle-timeout %d", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max bandwidth up */
  if (usmDbCpdmCPConfigUserUpRateGet(cpId, &val) == L7_SUCCESS)
  {
    val = val/8; /* Convert from bits to bytes */
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_UP_RATE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nmax-bandwidth-up %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max bandwidth down */
  if (usmDbCpdmCPConfigUserDownRateGet(cpId, &val) == L7_SUCCESS)
  {
    val = val/8; /* Convert from bits to bytes */
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_DOWN_RATE)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nmax-bandwidth-down %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max input octets */
  if (usmDbCpdmCPConfigMaxInputOctetsGet(cpId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_INPUT_OCTETS)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nmax-input-octets %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max output octets */
  if (usmDbCpdmCPConfigMaxOutputOctetsGet(cpId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_OUTPUT_OCTETS)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nmax-output-octets %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max total octets */
  if (usmDbCpdmCPConfigMaxTotalOctetsGet(cpId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_TOTAL_OCTETS)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nmax-total-octets %u", val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* block - not persistent */

  /* interface */
  intf = 0;
  while ((usmDbCpdmCPConfigIntIfNumNextGet(cpId, intf, &cpId_next, &intf) == L7_SUCCESS) && (cpId == cpId_next))
  {
    if (usmDbUnitSlotPortGet(intf, &u, &s, &p) == L7_SUCCESS)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\ninterface %s", cliDisplayInterfaceHelp(u, s, p));
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* separator color */
  memset(name, 0x00, sizeof(name));
  if (usmDbCpdmCPConfigSeparatorColorGet(cpId, name) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || strcmp(name, CP_DEF_SEPARATOR_COLOR) != 0)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nseparator-color \"%s\"", name);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* background color */
  memset(name, 0x00, sizeof(name));
  if (usmDbCpdmCPConfigBackgroundColorGet(cpId, name) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || strcmp(name, CP_DEF_BACKGROUND_COLOR) != 0)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nbackground-color \"%s\"", name);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* foreground color */
  memset(name, 0x00, sizeof(name));
  if (usmDbCpdmCPConfigForegroundColorGet(cpId, name) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || strcmp(name, CP_DEF_FOREGROUND_COLOR) != 0)
    {
      cp_enter_cmd(ewsContext);
      config_enter_cmd(ewsContext, cpId);
      osapiSnprintf(buf, sizeof(buf), "\r\nforeground-color \"%s\"", name);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* locales */
  cliRunningConfigCPInfoConfigurationLocales(ewsContext, cpId);

  config_exit_cmd(ewsContext);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print Captive Portal user group running configuration info
*
* @param    EwsContext  ewsContext  @b{(input)} EWS Context Handle
* @param    gpId_t      gId         @b{(input)} group ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t cliRunningConfigCPInfoUserGroup(EwsContext ewsContext, gpId_t gId)
{
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* group name */
  memset(name, 0x00, sizeof(name));
  if (usmDbCpdmUserGroupEntryNameGet(gId, name) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || 
        ((gId == GP_ID_MIN) && (strcmp(name, GP_DEFAULT_NAME) != 0)) || 
        (gId != GP_ID_MIN))
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nuser group %d name \"%s\"", gId, name);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print Captive Portal user running configuration info
*
* @param    EwsContext  ewsContext  @b{(input)} EWS Context Handle
* @param    uId_t       uId         @b{(input)} user ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t cliRunningConfigCPInfoUser(EwsContext ewsContext, uId_t uId)
{
  L7_uint32 val;
  uId_t uId_next;
  gpId_t gId;
  L7_char8 encryptedPwd[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* user name */
  memset(name, 0x00, sizeof(name));
  if (usmDbCpdmUserEntryLoginNameGet(uId, name) == L7_SUCCESS)
  {
    cp_enter_cmd(ewsContext);
    osapiSnprintf(buf, sizeof(buf), "\r\nuser %d name \"%s\"", uId, name);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  /* password */
  if (usmDbCpdmUserEntryEncryptedPasswordGet(uId, encryptedPwd) == L7_SUCCESS)
  {
    cp_enter_cmd(ewsContext);
    osapiSnprintf(buf, sizeof(buf), "\r\nuser %d password encrypted %s", uId, encryptedPwd);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  /* session-timeout */
  if (usmDbCpdmUserEntrySessionTimeoutGet(uId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_LOCAL_USER_SESSION_TIMEOUT)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d session-timeout %d", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* idle-timeout */
  if (usmDbCpdmUserEntryIdleTimeoutGet(uId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != FD_CP_LOCAL_USER_IDLE_TIMEOUT)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d idle-timeout %d", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max bandwidth up */
  if (usmDbCpdmUserEntryMaxBandwidthUpGet(uId, &val) == L7_SUCCESS)
  {
    val = val/8; /* Convert from bits to bytes */
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_UP_RATE)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d max-bandwidth-up %u", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max bandwidth down */
  if (usmDbCpdmUserEntryMaxBandwidthDownGet(uId, &val) == L7_SUCCESS)
  {
    val = val/8; /* Convert from bits to bytes */
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_DOWN_RATE)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d max-bandwidth-down %u", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max input octets */
  if (usmDbCpdmUserEntryMaxInputOctetsGet(uId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_INPUT_OCTETS)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d max-input-octets %u", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max output octets */
  if (usmDbCpdmUserEntryMaxOutputOctetsGet(uId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_USER_OUTPUT_OCTETS)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d max-output-octets %u", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* max total octets */
  if (usmDbCpdmUserEntryMaxTotalOctetsGet(uId, &val) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT || val != CP_DEF_TOTAL_OCTETS)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser %d max-total-octets %u", uId, val);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* group */
  gId = 0;
  while ((usmDbCpdmUserGroupAssocEntryNextGet(uId, gId, &uId_next, &gId) == L7_SUCCESS) && (uId == uId_next))
  {
    cp_enter_cmd(ewsContext);
    osapiSnprintf(buf, sizeof(buf), "\r\nuser %d group %d", uId, gId);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print Captive Portal Branding Images running config info
*
* @param    EwsContext  ewsContext  @b{(input)} EWS Context Handle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Unfortunately, the CLI maximum buffer size is quite limited
*           so we need to split up the stream and send in base64 chunks
*
* @end
*********************************************************************/
static L7_RC_t cliRunningConfigCPEncodedImage(EwsContext ewsContext)
{
  L7_char8 buf[(L7_CLI_MAX_LARGE_STRING_LENGTH*2)-1]; /* currently the max */
  L7_char8 CMD[] = "\r\nencoded-image-text \"";
  L7_char8 CMD_LEN = 22;
  L7_uint32 idx = 0;
  L7_uint32 jdx = 0;
  L7_uint32 kdx = 0;
  L7_uint32 eoi = 0; /* end of image data */
  L7_uchar8 *images = L7_NULLPTR;
  L7_uint8 byte1, byte2, byte3;    /* input, 3 bytes */
  L7_char8 chars[4];
  L7_int32 len = 0;

  if (usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
  {
    /* Don't bother for non-text based configurations */
    return L7_SUCCESS;
  }

  images = usmDbCpdmImageDataGet(&eoi);

  if ((L7_NULLPTR != images) && (0!=eoi))
  {
    /* Enter encoded-image mode */
    memset(buf,0,sizeof(buf));
    strcpy(buf, "\r\nencoded-image");
    EWSWRITEBUFFER(ewsContext, buf);

    /* Clear existing image data */
    memset(buf,0,sizeof(buf));
    strcpy(buf, "\r\nno encoded-image-text");
    EWSWRITEBUFFER(ewsContext, buf);

    /* Save actual binary data size (used when processing the image chunks */
    memset(buf,0,sizeof(buf));
    osapiSnprintf(buf, sizeof(buf), "\r\ndecoded-image-size %d",eoi);
    EWSWRITEBUFFER(ewsContext, buf);

    memset(buf,0,sizeof(buf));
    strcpy(buf,CMD);
    kdx = CMD_LEN;
    for (idx = 0, len = eoi; len > 0; len -= 3) /* for now, the entire stream */
    {
      /* Get a 3 byte group */
      byte1 = images[idx++];
      byte2 = (len >= 2) ? images[idx++] : 0;
      byte3 = (len >= 3) ? images[idx++] : 0;

      /* Compose the 6-bit values. */
      chars[0] = (byte1 >> 2) & 0x3F;
      chars[1] = ((byte1 << 4) | (byte2 >> 4)) & 0x3F;
      chars[2] = ((byte2 << 2) | (byte3 >> 6)) & 0x3F;
      chars[3] = byte3 & 0x3F;

      /* Translate the 6-bit values to characters. */
      for (jdx = 0; jdx < 4; jdx++)
      {
        chars[jdx] =
            (chars[jdx] <= 25) ? chars[jdx] + 'A' :
            (chars[jdx] <= 51) ? chars[jdx] + 'a' - 26 :
            (chars[jdx] <= 61) ? chars[jdx] + '0' - 52 :
            (chars[jdx] == 62) ? '+' :
            '/';
      }

      /* Turn the last characters to '=' if this is the end of the string. */
      switch (len)
      {
        case 1:
          chars[2] = '=';
          /* fall through */
        case 2:
          chars[3] = '=';
          break;
        default:
          ; /* no action */
      }

      if ((kdx+6)>(sizeof(buf)))
      {
        buf[kdx] = '"';
        EWSWRITEBUFFER(ewsContext, buf);
        memset(buf,0,sizeof(buf));
        strcpy(buf,CMD);
        kdx = CMD_LEN;
      }

      buf[kdx++] = chars[0];
      buf[kdx++] = chars[1];
      buf[kdx++] = chars[2];
      buf[kdx++] = chars[3];
    }
    buf[kdx] = '"';
    EWSWRITEBUFFER(ewsContext, buf);

    /* Exit encoded-image mode */
    EWSWRITEBUFFER(ewsContext,"\r\nexit \r\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print Captive Portal running configuration information
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliRunningConfigCPInfo(EwsContext ewsContext)
{
  L7_uint32 val = 0;
  cpId_t cpId;
  gpId_t gId;
  uId_t uId;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  cp_enter_cmd_init();

  if (usmDbCpdmGlobalModeGet(&val) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((val != FD_CP_DEFAULT_CP_MODE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
  {
    cp_enter_cmd(ewsContext);
    if (val == L7_DISABLE)
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nno enable");
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nenable");
    }
    EWSWRITEBUFFER(ewsContext, buf);
  }

  cliRunningConfigCPInfoGlobal(ewsContext);
  EWSWRITEBUFFER(ewsContext,"\r\n");

  gId = 0;
  while (usmDbCpdmUserGroupEntryNextGet(gId, &gId) == L7_SUCCESS)
  {
    if ((gId != GP_ID_MIN) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      cp_enter_cmd(ewsContext);
      osapiSnprintf(buf, sizeof(buf), "\r\nuser group %d", gId);
      EWSWRITEBUFFER(ewsContext, buf);
    }
      cliRunningConfigCPInfoUserGroup(ewsContext, gId);
      EWSWRITEBUFFER(ewsContext,"\r\n");
  }

  cpId = 0;
  while (usmDbCpdmCPConfigNextGet(cpId, &cpId) == L7_SUCCESS)
  {
    cliRunningConfigCPInfoConfiguration(ewsContext, cpId);
  }

  uId = 0;
  while (usmDbCpdmUserEntryNextGet(uId, &uId) == L7_SUCCESS)
  {
    cliRunningConfigCPInfoUser(ewsContext, uId);
    EWSWRITEBUFFER(ewsContext,"\r\n");
  }

  /* Display branding images */
  cliRunningConfigCPEncodedImage(ewsContext);

  cp_exit_cmd(ewsContext);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check Captive Portal running configuration command type
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL cliRunningConfigCPIsCmdUserPwd(L7_char8 *buf)
{
  L7_uint32 buf_len;
  L7_char8 tmp_buf[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
  L7_char8 *u, *i, *p;

  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FALSE;
  }

  memset(tmp_buf, 0, sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  u = strtok(tmp_buf, pStrInfo_common_Space);
  i = strtok(L7_NULL, pStrInfo_common_Space);
  p = strtok(L7_NULL, pStrInfo_common_Space);

  if ((u == L7_NULLPTR) || (i == L7_NULLPTR) || (p == L7_NULLPTR))
  {
    return L7_FALSE;
  }

  if ((strcmp(u, "user") == 0) && (strcmp(p, "password") == 0))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

