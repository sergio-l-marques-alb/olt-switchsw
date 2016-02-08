/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  usmdb_cpdm_web.c
*
* @purpose   Captive Portal Data Manager (CPDM) USMDB API functions
*
* @component CPDM
*
* @comments  none
*
* @create    07/26/2007
*
* @author    rjindal
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "osapi.h"
#include "captive_portal_commdefs.h"
#include "cpdm_web_api.h"


/*********************************************************************
*
* @purpose  Get CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    cpId_t webId @b{(input)} web id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebIdGet(cpId_t cpId, webId_t webId)
{
  return cpdmCPConfigWebIdGet(cpId, webId);
}

/*********************************************************************
*
* @purpose  Get the next sequential CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_short16 *nextWebId @b{(output)} next web id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebIdNextGet(cpId_t cpId, webId_t webId, L7_short16 *nextWebId)
{
  return cpdmCPConfigWebIdNextGet(cpId, webId, nextWebId);
}

/*********************************************************************
* CP Web Page Customization Configuration 
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebIdAdd(cpId_t cpId, webId_t webId)
{
  return cpdmCPConfigWebIdAdd(cpId, webId);
}

/*********************************************************************
*
* @purpose  Initializes an existing webId with specified locale data.
*           This is provided by searching for an existing language code
*           within the defaults array. If its found, the associated
*           fields are set.
*
* @param    cpId_t cpId  @b{(input)} existing captive portal ID
* @param    webId_t webId  @b{(input)} existing captive portal Web id
* @param    L7_char8 *code  @b{(input)} language code to search
* @param    L7_char8 *link  @b{(input)} language locale string
*
* @returns  L7_SUCCESS if code is found and webId is initialized
*
* @end 
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAddDefaults(cpId_t cpId, webId_t webId, L7_char8 *code, CP_PAGE_TYPE_t page)
{
  return cpdmCPConfigWebAddDefaults(cpId, webId, code, page);
}

L7_RC_t usmDbCpdmCPConfigWebIdDelete(cpId_t cpId, webId_t webId)
{
  return cpdmCPConfigWebIdDelete(cpId, webId);
}

/*********************************************************************
*
* @purpose  A read only helper function used to retrieve the associated
*           default web customization data matching the appropriate
*           parameter.
*
* @param    cpId_t cpId  @b{(input)} existing captive portal ID
* @param    webId_t webId  @b{(input)} existing captive portal Web id
* @param    L7_char8 *param  @b{(input)} field name to parse
* @param    L7_char8 *def  @b{(input)} default string to check
*
* @returns  L7_BOOL if *def is equal to default value
*
* @end 
*
*********************************************************************/
L7_BOOL usmDbCpdmCPConfigWebIsDefault(L7_char8 *code, L7_char8 *param, L7_char8 *def)
{
  return cpdmCPConfigWebIsDefault(code, param, def);
}

/*********************************************************************
*
* @purpose  Wipe out locales for a given CP ID
*
* @param    cpId_t cpId  @b{(input)} captive portal ID
*
* @returns  L7_uint32 number of locales purged
*
* @end 
*
*********************************************************************/
L7_uint32 usmDbCpdmCPConfigWebIdAllDelete(cpId_t cpId)
{
  return cpdmCPConfigWebIdAllDelete(cpId);
}

/*********************************************************************
*
* @purpose  Get the CP Web accept text
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *acceptText @b{(output)} accept text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAcceptTextGet(cpId_t cpId, webId_t webId, L7_char8 *acceptText)
{
  return cpdmCPConfigWebAcceptTextGet(cpId,webId,acceptText);
}

/*********************************************************************
*
* @purpose  Get the CP Web account label
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *accountLabel @b{(output)} account label (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountLabelGet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel)
{
  return cpdmCPConfigWebAccountLabelGet(cpId,webId,accountLabel);
}

/********************************************************************* 
* @purpose  Get CP font list
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *fontList @b{(output)} - corresponds to
*           "fontList" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This can be a comma-separated list
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebFontListGet(cpId_t cpId, webId_t webId, L7_char8 *fontList)
{
  return cpdmCPConfigWebFontListGet(cpId,webId,fontList);
}

/*********************************************************************
*
* @purpose  Get the CP Web acceptance use policy (aup)
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *aupText @b{(output)} aup text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAUPTextGet(cpId_t cpId, webId_t webId, L7_char8 *aupText)
{
  return cpdmCPConfigWebAUPTextGet(cpId,webId,aupText);
}

/*********************************************************************
*
* @purpose  Get the CP Web submit button label
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *buttonLabel @b{(output)} button label
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel)
{
  return cpdmCPConfigWebButtonLabelGet(cpId,webId,buttonLabel);
}

/*********************************************************************
*
* @purpose  Get the CP Web browser title text. This is used actual title
*           for the web page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} browser title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebBrowserTitleTextGet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web instructional text used to describe the intent
*           and operations of the verification page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *instructionalText @b{(output)} instructional text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebInstructionalTextGet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText)
{
  return cpdmCPConfigWebInstructionalTextGet(cpId,webId,instructionalText);
}

/*********************************************************************
*
* @purpose  Get the CP Web language code. This is used to identify the
*           choice for customized content. IANA Lang Subtag Code plus
*           the optional region code.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *langCode @b{(output)} lang code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLangCodeGet(cpId_t cpId, webId_t webId, L7_char8 *langCode)
{
  return cpdmCPConfigWebLangCodeGet(cpId,webId,langCode);
}

/*********************************************************************
*
* @purpose  Get the CP Web link language locale. This is the text to display
*           and identify the web locale. This shows in the administration 
*           tab and the validation page at the top which provides user selection.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *localeLink @b{(output)} locale link (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLocaleLinkGet(cpId_t cpId, webId_t webId, L7_char8 *localeLink)
{
  return cpdmCPConfigWebLocaleLinkGet(cpId,webId,localeLink);
}

/*********************************************************************
*
* @purpose  Get the CP Web password label.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *passwordLabel @b{(output)} password label (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebPasswordLabelGet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel)
{
  return cpdmCPConfigWebPasswordLabelGet(cpId,webId,passwordLabel);
}

/*********************************************************************
*
* @purpose  Get the CP Web title text used to greet the user.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebTitleTextGet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user (uid) label.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *userLabel @b{(output)} user label (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebUserLabelGet(cpId_t cpId, webId_t webId, L7_char8 *userLabel)
{
  return cpdmCPConfigWebUserLabelGet(cpId,webId,userLabel);
}

/*********************************************************************
*
* @purpose  Get the CP Web message text for no accept failure. This is
*           error text to display when the user attempts to connect but
*           did not check/acknowledge the AUP.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *noAcceptText @b{(output)} no cccept error text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebNoAcceptMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptText)
{
  return cpdmCPConfigWebNoAcceptMsgTextGet(cpId,webId,noAcceptText);
}

/*********************************************************************
*
* @purpose  Get the CP Web message text for WIP. This text is used to
*           indicate to the user work-in-progress (wip) status.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *wipText @b{(output)} work-in-progress message
*           text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWipMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *wipText)
{
  return cpdmCPConfigWebWipMsgTextGet(cpId,webId,wipText);
}

/*********************************************************************
*
* @purpose  Get the CP Web message text for a denied failure. This is
*           error text to display when the user attempts to connect but
*           the login credentials do not match.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *deniedText @b{(output)} denied error text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebDeniedMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *deniedText)
{
  return cpdmCPConfigWebDeniedMsgTextGet(cpId,webId,deniedText);
}

/*********************************************************************
*
* @purpose  Get the CP Web message text for a resource failure. This is
*           error text to display when the user attempts to connect but
*           the system has resource problems. This could be such that the
*           CP connection limit has been reached. But this message should be
*           a generic resource failure message.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *resourceText @b{(output)} resource error text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebResourceMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *resourceText)
{
  return cpdmCPConfigWebResourceMsgTextGet(cpId,webId,resourceText);
}

/*********************************************************************
*
* @purpose  Get the CP Web message text for a timeout failure. This is
*           error text to display when the user attempts to connect but
*           the connnection has timed out. This could be such that the
*           CP connection time has been reached. Maybe the RADIUS sever
*           is busy. But this message should be a generic timeout failure
*           message.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *timeoutText @b{(output)} timeout error text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTimeoutMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *timeoutText)
{
  return cpdmCPConfigWebTimeoutMsgTextGet(cpId,webId,timeoutText);
}

/*********************************************************************
*
* @purpose  Get the CP Web message title text for the welcome page. This is
*           the title text used in the welcome page once the user is connected.
*           The welcome page is optional.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *welcomeTitleText @b{(output)} welcome title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText)
{
  return cpdmCPConfigWebWelcomeTitleTextGet(cpId,webId,welcomeTitleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web message text for the welcome page. This is
*           the general text used to notify that the user has successfully
*           connected in the network.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *welcomeText @b{(output)} welcome text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText)
{
  return cpdmCPConfigWebWelcomeTextGet(cpId,webId,welcomeText);
}

/*********************************************************************
*
* @purpose  Get the CP Web script notification text when client java scripting is disabled.
*           This is used only when user logout requests are enabled because it depends on
*           javascript.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *scriptText @b{(output)} info text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebScriptTextGet(cpId_t cpId, webId_t webId, L7_char8 *scriptText)
{
  return cpdmCPConfigWebScriptTextGet(cpId,webId,scriptText);
}

/*********************************************************************
*
* @purpose  Get the CP Web script notification text when user logout requests are enabled.
*           This message is a reminder/request to allow popups because we need to create
*           a client browser popup.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *scriptText @b{(output)} info text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebPopupTextGet(cpId_t cpId, webId_t webId, L7_char8 *popupText)
{
  return cpdmCPConfigWebPopupTextGet(cpId,webId,popupText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout browser title text. This is the actual
*           browser title for the web page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} browser title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutBrowserTitleTextGet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout title text. This is the main title
*           used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutTitleTextGet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout content title text. This is the
*           content message used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} content text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{
  return cpdmCPConfigWebLogoutContentTextGet(cpId,webId,contentText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout request submit button label
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *buttonLabel @b{(output)} button label
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel)
{
  return cpdmCPConfigWebLogoutButtonLabelGet(cpId,webId,buttonLabel);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout request confirmation. Text is
*           used when user clicks logout button or closes the logout
*           window.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *confirmText @b{(output)} confirm text
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutConfirmTextGet(cpId_t cpId, webId_t webId, L7_char8 *confirmText)
{
    return cpdmCPConfigWebLogoutConfirmTextGet(cpId,webId,confirmText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout success browser title text.
*           This is the actual browser title for the web page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} browser title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout success title text.
*           This is the main title used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutSuccessTitleTextGet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Get the CP Web user logout success content title text. This is the
*           content message used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} content text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{
  return cpdmCPConfigWebLogoutSuccessContentTextGet(cpId,webId,contentText);
}

/***************************************************************
*
* @purpose  Get the CP Web logout success image name for this
*           configuration. This does not include any directory
*           specification.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *imageName @b{(output)} image name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId,webId,imageName);
}

/*************************************************************
* @purpose  Set value of field "accountImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebAccountImageNameSet(cpId, webId, imageName);
}

/*************************************************************
* @purpose  Set value of field "brandingImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrandingImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebBrandingImageNameSet(cpId, webId, imageName);
}

/*************************************************************
* @purpose  Set value of field "backgroundImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebBackgroundImageNameSet(cpId, webId, imageName);
}

/*************************************************************
* @purpose  Set value of field "browserTitleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *browserTitleText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *browserTitleText)
{
  return cpdmCPConfigWebBrowserTitleTextSet(cpId, webId, browserTitleText);
}

/*************************************************************
* @purpose  Set value of field "langCode"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *langCode @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLangCodeSet(cpId_t cpId, webId_t webId, L7_char8 *langCode)
{
  return cpdmCPConfigWebLangCodeSet(cpId, webId, langCode);
}

/*************************************************************
* @purpose  Set value of field "localeLink"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *localeLink @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLocaleLinkSet(cpId_t cpId, webId_t webId, L7_char8 *localeLink)
{
  return cpdmCPConfigWebLocaleLinkSet(cpId, webId, localeLink);
}

/*************************************************************
* @purpose  Set value of field "titleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *titleText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebTitleTextSet(cpId,  webId, titleText);
}

/*************************************************************
* @purpose  Set value of field "accountLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *accountLabel @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountLabelSet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel)
{
  return cpdmCPConfigWebAccountLabelSet(cpId,  webId, accountLabel);
}

/********************************************************************* 
* @purpose  Set value of locale field "fontList"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *fontList @b{(input)} - new
*           "fontList" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This can be a comma-separated list.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebFontListSet(cpId_t cpId, webId_t webId, L7_char8 *fontList)
{
  return cpdmCPConfigWebFontListSet(cpId,  webId, fontList);
}


/*************************************************************
* @purpose  Set value of field "userLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *userLabel @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebUserLabelSet(cpId_t cpId, webId_t webId, L7_char8 *userLabel)
{
  return cpdmCPConfigWebUserLabelSet(cpId, webId, userLabel);
}

/*************************************************************
* @purpose  Set value of field "passwordLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *passwordLabel @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebPasswordLabelSet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel)
{
  return cpdmCPConfigWebPasswordLabelSet(cpId, webId, passwordLabel);
}

/*************************************************************
* @purpose  Set value of field "buttonLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *buttonLabel @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel)
{
  return cpdmCPConfigWebButtonLabelSet(cpId, webId, buttonLabel);
}

/********************************************************************* 
* @purpose  Append value of locale field "instructionalText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *instructionalText @b{(input)} - new
*           "instructionalText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It is assumed that instructionalText (cpdmCustomLocale_t)
*           is initialized and ready for concatenation. In addition,
*           sense this is a non-user API, we don't validate the input
*           string length (because we can get the hex stream in random
*           chunks).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebInstructionalTextAppend(cpId_t cpId, webId_t webId, L7_char8 *instructionalText)
{
  return cpdmCPConfigWebInstructionalTextAppend(cpId, webId, instructionalText);
}

/*************************************************************
* @purpose  Set value of field "instructionalText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *instructionalText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebInstructionalTextSet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText)
{
  return cpdmCPConfigWebInstructionalTextSet(cpId, webId, instructionalText);
}

/********************************************************************* 
* @purpose  Append value of locale field "aupText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *aupText @b{(input)} - new
*           "aupText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It is assumed that aupText (cpdmCustomLocale_t) is
*           initialized and ready for concatenation. In addition,
*           sense this is a non-user API, we don't validate the input
*           string length (because we can get the hex stream in random
*           chunks).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAUPTextAppend(cpId_t cpId, webId_t webId, L7_char8 *aupText)
{
  return cpdmCPConfigWebAUPTextAppend(cpId, webId, aupText);
}

/*************************************************************
* @purpose  Set value of field "aupText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *aupText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAUPTextSet(cpId_t cpId, webId_t webId, L7_char8 *aupText)
{
  return cpdmCPConfigWebAUPTextSet(cpId, webId, aupText);
}

/*************************************************************
* @purpose  Set value of field "acceptText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *acceptText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAcceptTextSet(cpId_t cpId, webId_t webId, L7_char8 *acceptText)
{
  return cpdmCPConfigWebAcceptTextSet(cpId, webId, acceptText);
}

/*************************************************************
* @purpose  Set value of field "noAcceptMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *noAcceptMsg @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebNoAcceptMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptMsg)
{
  return cpdmCPConfigWebNoAcceptMsgTextSet(cpId,  webId,  noAcceptMsg);
}

/*************************************************************
* @purpose  Set value of field "wipMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *wipMsg @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWipMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *wipMsg)
{
  return cpdmCPConfigWebWipMsgTextSet(cpId, webId, wipMsg);
}

/*************************************************************
* @purpose  Set value of field "deniedMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *deniedMsg @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebDeniedMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *deniedMsg)
{
  return cpdmCPConfigWebDeniedMsgTextSet(cpId, webId, deniedMsg);
}

/*************************************************************
* @purpose  Set value of field "resourceMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *resourceMsg @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebResourceMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *resourceMsg)
{
  return cpdmCPConfigWebResourceMsgTextSet(cpId, webId, resourceMsg);
}

/*************************************************************
* @purpose  Set value of field "timeoutMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *timeoutMsg @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTimeoutMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *timeoutMsg)
{
  return cpdmCPConfigWebTimeoutMsgTextSet(cpId, webId, timeoutMsg);
}

/*************************************************************
* @purpose  Set value of field "welcomeTitleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *welcomeTitleText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText)
{
  return cpdmCPConfigWebWelcomeTitleTextSet(cpId, webId, welcomeTitleText);
}

/********************************************************************* 
* @purpose  Append value of locale field "welcomeText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *welcomeText @b{(input)} - new
*           "welcomeText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It is assumed that welcomeText (cpdmCustomLocale_t)
*           is initialized and ready for concatenation. In addition,
*           sense this is a non-user API, we don't validate the input
*           string length (because we can get the hex stream in random
*           chunks).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTextAppend(cpId_t cpId, webId_t webId, L7_char8 *welcomeText)
{
  return cpdmCPConfigWebWelcomeTextAppend(cpId, webId, welcomeText);
}

/*************************************************************
* @purpose  Set value of field "welcomeText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *welcomeText @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText)
{
  return cpdmCPConfigWebWelcomeTextSet(cpId, webId, welcomeText);
}

/*********************************************************************
*
* @purpose  Set the CP Web script notification text when client java scripting is disabled.
*           This is used only when user logout requests are enabled because it depends on
*           javascript.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *scriptText @b{(output)} info text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebScriptTextSet(cpId_t cpId, webId_t webId, L7_char8 *scriptText)
{
  return cpdmCPConfigWebScriptTextSet(cpId,webId,scriptText);
}

/*********************************************************************
*
* @purpose  Set the CP Web script notification text when user logout requests are enabled.
*           This message is a reminder/request to allow popups because we need to create
*           a client browser popup.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *scriptText @b{(output)} info text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebPopupTextSet(cpId_t cpId, webId_t webId, L7_char8 *popupText)
{
  return cpdmCPConfigWebPopupTextSet(cpId,webId,popupText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout browser title text. This is the actual
*           browser title for the web page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} browser title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutBrowserTitleTextSet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout title text. This is the main title
*           used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutTitleTextSet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout content title text. This is the
*           content message used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} content text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{
  return cpdmCPConfigWebLogoutContentTextSet(cpId,webId,contentText);
}

/********************************************************************* 
* @purpose  Append value of locale field "logoutContentText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *logoutContentText @b{(input)} - new
*           "logoutContentText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It is assumed that logoutContentText (cpdmCustomLocale_t)
*           is initialized and ready for concatenation. In addition,
*           sense this is a non-user API, we don't validate the input
*           string length (because we can get the hex stream in random
*           chunks).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutContentText)
{
  return cpdmCPConfigWebLogoutContentTextAppend(cpId,webId,logoutContentText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout request submit button label
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *buttonLabel @b{(output)} button label
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel)
{
  return cpdmCPConfigWebLogoutButtonLabelSet(cpId,webId,buttonLabel);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout request confirmation. Text is
*           used when user clicks logout button or closes the logout
*           window.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *confirmText @b{(output)} confirmation text
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutConfirmTextSet(cpId_t cpId, webId_t webId, L7_char8 *confirmText)
{
  return cpdmCPConfigWebLogoutConfirmTextSet(cpId,webId,confirmText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout success browser title text.
*           This is the actual browser title for the web page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} browser title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutSuccessBrowserTitleTextSet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout success title text.
*           This is the main title used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} title text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{
  return cpdmCPConfigWebLogoutSuccessTitleTextSet(cpId,webId,titleText);
}

/*********************************************************************
*
* @purpose  Set the CP Web user logout success content title text. This is the
*           content message used in the user logout request page.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *titleText @b{(output)} content text (unicode)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{
  return cpdmCPConfigWebLogoutSuccessContentTextSet(cpId,webId,contentText);
}

/********************************************************************* 
* @purpose  Append value of locale field "logoutSuccessContentText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *logoutSuccessContentText @b{(input)} - new
*           "logoutSuccessContentText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It is assumed that logoutSuccessContentText (cpdmCustomLocale_t)
*           is initialized and ready for concatenation. In addition,
*           sense this is a non-user API, we don't validate the input
*           string length (because we can get the hex stream in random
*           chunks).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutSuccessContentText)
{
  return cpdmCPConfigWebLogoutSuccessContentTextAppend(cpId,webId,logoutSuccessContentText);
}

/***************************************************************
*
* @purpose  Set the CP Web logout success image name for this
*           configuration. This does not include any directory
*           specification.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *imageName @b{(output)} image name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId,webId,imageName);
}

/***************************************************************
*
* @purpose  Get the CP Web account image name for this
*           configuration. This does not include any directory
*           specification.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *imageName @b{(output)} image name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebAccountImageNameGet(cpId,webId,imageName);
}

/***************************************************************
*
* @purpose  Get the CP Web branding image name for this
*           configuration. This does not include any directory
*           specification.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *imageName @b{(output)} image name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrandingImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebBrandingImageNameGet(cpId,webId,imageName);
}

/***************************************************************
*
* @purpose  Get the CP Web background image name for this
*           configuration. This does not include any directory
*           specification.
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    L7_char8 *imageName @b{(output)} image name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{
  return cpdmCPConfigWebBackgroundImageNameGet(cpId,webId,imageName);
}

/********************************************************************* 
* @purpose  Replace matching images with the default. This is used
*           when the administrator has deleted an image from CP RAM.
*
* @param    L7_char8 *imageName @b{(input)} - image to replace
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebImageResetAll(L7_char8 *imageName)
{
  return cpdmCPConfigWebImageResetAll(imageName);
}
