/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename     usmdb_cpdm_web_api.h
*
* @purpose      Captive Portal Data Manager (WDM) USMDB API functions
*
* @component    CPDM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       7/26/2007
*
* @author       rjindal
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_USMDB_CPDM_WEB_API_H
#define INCLUDE_USMDB_CPDM_WEB_API_H

#include "l7_common.h"
#include "usmdb_cpdm_api.h"
#include "captive_portal_commdefs.h"

/*********************************************************************
*
* @purpose  Get CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_short16 *webId @b{(input)} web id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigWebIdGet(cpId_t cpId, webId_t webId);

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
L7_RC_t usmDbCpdmCPConfigWebIdNextGet(cpId_t cpId, webId_t webId, L7_short16 *nextWebId);

L7_RC_t usmDbCpdmCPConfigWebIdAdd(cpId_t cpId, webId_t webId);

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
L7_RC_t usmDbCpdmCPConfigWebAddDefaults(cpId_t cpId, webId_t webId, L7_char8 *code, CP_PAGE_TYPE_t page);

L7_RC_t usmDbCpdmCPConfigWebIdDelete(cpId_t cpId, webId_t webId);
L7_uint32 usmDbCpdmCPConfigWebIdAllDelete(cpId_t cpId);

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
L7_BOOL usmDbCpdmCPConfigWebIsDefault(L7_char8 *code, L7_char8 *param, L7_char8 *def);

/*************************************************************
* @purpose  Set value of field "accountImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Set value of field "brandingImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrandingImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Set value of field "backgroundImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Set value of field "browserTitleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *browserTitleText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *browserTitleText);

/*************************************************************
* @purpose  Set value of field "langCode"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *langCode @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLangCodeSet(cpId_t cpId, webId_t webId, L7_char8 *langCode);

/*************************************************************
* @purpose  Set value of field "localeLink"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *localeLink @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLocaleLinkSet(cpId_t cpId, webId_t webId, L7_char8 *localeLink);

/*************************************************************
* @purpose  Set value of field "titleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *titleText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

/*************************************************************
* @purpose  Set value of field "accountLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *accountLabel @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountLabelSet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel);

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
L7_RC_t usmDbCpdmCPConfigWebFontListSet(cpId_t cpId, webId_t webId, L7_char8 *fontList);

/*************************************************************
* @purpose  Set value of field "userLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *userLabel @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebUserLabelSet(cpId_t cpId, webId_t webId, L7_char8 *userLabel);

/*************************************************************
* @purpose  Set value of field "passwordLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *passwordLabel @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebPasswordLabelSet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel);

/*************************************************************
* @purpose  Set value of field "buttonLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *buttonLabel @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t usmDbCpdmCPConfigWebInstructionalTextAppend(cpId_t cpId, webId_t webId, L7_char8 *instructionalText);

/*************************************************************
* @purpose  Set value of field "instructionalText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *instructionalText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebInstructionalTextSet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText);

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
L7_RC_t usmDbCpdmCPConfigWebAUPTextAppend(cpId_t cpId, webId_t webId, L7_char8 *aupText);

/*************************************************************
* @purpose  Set value of field "aupText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *aupText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAUPTextSet(cpId_t cpId, webId_t webId, L7_char8 *aupText);

/*************************************************************
* @purpose  Set value of field "acceptText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *acceptText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAcceptTextSet(cpId_t cpId, webId_t webId, L7_char8 *acceptText);

/*************************************************************
* @purpose  Set value of field "noAcceptMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *noAcceptMsg @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebNoAcceptMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptMsg);

/*************************************************************
* @purpose  Set value of field "wipMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *wipMsg @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWipMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *wipMsg);

/*************************************************************
* @purpose  Set value of field "deniedMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *deniedMsg @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebDeniedMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *deniedMsg);

/*************************************************************
* @purpose  Set value of field "resourceMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *resourceMsg @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebResourceMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *resourceMsg);

/*************************************************************
* @purpose  Set value of field "timeoutMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *timeoutMsg @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTimeoutMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *timeoutMsg);

/*************************************************************
* @purpose  Set value of field "welcomeTitleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *welcomeTitleText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText);

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
L7_RC_t usmDbCpdmCPConfigWebWelcomeTextAppend(cpId_t cpId, webId_t webId, L7_char8 *welcomeText);

/*************************************************************
* @purpose  Set value of field "welcomeText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *welcomeText @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText);

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
L7_RC_t usmDbCpdmCPConfigWebScriptTextSet(cpId_t cpId, webId_t webId, L7_char8 *scriptText);

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
L7_RC_t usmDbCpdmCPConfigWebPopupTextSet(cpId_t cpId, webId_t webId, L7_char8 *popupText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutContentText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutConfirmTextSet(cpId_t cpId, webId_t webId, L7_char8 *confirmText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutSuccessContentText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Get value of field "accountImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Get value of field "brandingImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrandingImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Get value of field "backgroundImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Get value of field "browserTitleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *browserTitleText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *browserTitleText);


/*************************************************************
* @purpose  Get value of field "langCode"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *langCode @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLangCodeGet(cpId_t cpId, webId_t webId, L7_char8 *langCode);


/*************************************************************
* @purpose  Get value of field "localeLink"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *localeLink @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebLocaleLinkGet(cpId_t cpId, webId_t webId, L7_char8 *localeLink);


/*************************************************************
* @purpose  Get value of field "titleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *titleText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);


/*************************************************************
* @purpose  Get value of field "accountLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *accountLabel @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAccountLabelGet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel);

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
L7_RC_t usmDbCpdmCPConfigWebFontListGet(cpId_t cpId, webId_t webId, L7_char8 *fontList);

/*************************************************************
* @purpose  Get value of field "userLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *userLabel @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebUserLabelGet(cpId_t cpId, webId_t webId, L7_char8 *userLabel);


/*************************************************************
* @purpose  Get value of field "passwordLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *passwordLabel @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebPasswordLabelGet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel);


/*************************************************************
* @purpose  Get value of field "buttonLabel"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *buttonLabel @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);


/*************************************************************
* @purpose  Get value of field "instructionalText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *instructionalText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebInstructionalTextGet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText);


/*************************************************************
* @purpose  Get value of field "aupText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *aupText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAUPTextGet(cpId_t cpId, webId_t webId, L7_char8 *aupText);


/*************************************************************
* @purpose  Get value of field "acceptText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *acceptText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebAcceptTextGet(cpId_t cpId, webId_t webId, L7_char8 *acceptText);


/*************************************************************
* @purpose  Get value of field "noAcceptMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *noAcceptMsg @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebNoAcceptMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptMsg);


/*************************************************************
* @purpose  Get value of field "wipMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *wipMsg @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWipMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *wipText);

/*************************************************************
* @purpose  Get value of field "deniedMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *deniedMsg @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebDeniedMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *deniedMsg);


/*************************************************************
* @purpose  Get value of field "resourceMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *resourceMsg @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebResourceMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *resourceMsg);


/*************************************************************
* @purpose  Get value of field "timeoutMsg"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *timeoutMsg @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebTimeoutMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *timeoutMsg);


/*************************************************************
* @purpose  Get value of field "welcomeTitleText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *welcomeTitleText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText);


/*************************************************************
* @purpose  Get value of field "welcomeText"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *welcomeText @b{(output)} - area for copy
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigWebWelcomeTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText);

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
L7_RC_t usmDbCpdmCPConfigWebScriptTextGet(cpId_t cpId, webId_t webId, L7_char8 *scriptText);

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
L7_RC_t usmDbCpdmCPConfigWebPopupTextGet(cpId_t cpId, webId_t webId, L7_char8 *popupText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutConfirmTextGet(cpId_t cpId, webId_t webId, L7_char8 *confirmText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

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
L7_RC_t usmDbCpdmCPConfigWebImageResetAll(L7_char8 *imageName);

#endif /* INCLUDE_USMDB_CPDM_WEB_API_H */

