/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     cpdm_web_api.h
 *
 * @purpose      Captive Portal Data Manager (WDM) API header
 *
 * @component    CPDM
 *
 * @comments     APIs for usmDb layer, these APIs can be called via
 *               usmDb and captive portal components.
 *
 * @create       07/26/2007
 *
 * @author       rjindal
 *
 * @end
 *
 **********************************************************************/

#ifndef INCLUDE_CPDM_WEB_API_H
#define INCLUDE_CPDM_WEB_API_H

#include "datatypes.h"
#include "captive_portal_commdefs.h"



/*********************************************************************
*
* @purpose  Updates count of locale entries after save/restore
*
* @param    none
*
* @returns  nothing
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmWebIdCountUpdate(void);

/*********************************************************************
* @purpose  Add a CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE - exists in table, or we've reached the max
*                        table size, or we could not find enough
*                        free space to store the new entry.
*
* @comments Data is stored in a flat array (no AVL tree here).
*           We enforce the invariant that all unused slots in the
*           array are at the end (no holes!).
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebIdAdd(cpId_t cpId, webId_t webId);

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
*
* @returns  L7_SUCCESS if code is found and webId is initialized
*
* @end 
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebAddDefaults(cpId_t cpId, webId_t webId, L7_char8 *code, CP_PAGE_TYPE_t page);

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
L7_BOOL cpdmCPConfigWebIsDefault(L7_char8 *code, L7_char8 *param, L7_char8 *def);

/*********************************************************************
* @purpose  Delete a CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id of victim
* @param    webId_t webId @b{(input)} web id of victim
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE - non-existent item
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebIdDelete(cpId_t cpId, webId_t webId);


/*********************************************************************
*
* @purpose  Wipe out locales for a given CP ID
*
* @param    cpId_t cpId  @b{(input)} captive portal ID
*
* @returns  L7_uint32 number of locales purged
*
* @comments 
*           
* @end 
*
*********************************************************************/
L7_uint32 cpdmCPConfigWebIdAllDelete(cpId_t cpId);

/*********************************************************************
* @purpose  Get CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This Get routine is just a check, nothing else.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebIdGet(cpId_t cpId, webId_t webId);

/*********************************************************************
* @purpose  Get the next sequential CP config web id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    webId_t webId @b{(input)} web id
* @param    webId_t *nextWebId @b{(output)} next web id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebIdNextGet(cpId_t cpId, webId_t webId, webId_t *nextWebId);

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
L7_RC_t cpdmCPConfigWebAcceptTextGet(cpId_t cpId, webId_t webId, L7_char8 *acceptText);

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
L7_RC_t cpdmCPConfigWebAccountLabelGet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel);

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
L7_RC_t cpdmCPConfigWebFontListGet(cpId_t cpId, webId_t webId, L7_char8 *fontList);

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
L7_RC_t cpdmCPConfigWebAUPTextGet(cpId_t cpId, webId_t webId, L7_char8 *aupText);

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
L7_RC_t cpdmCPConfigWebButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t cpdmCPConfigWebBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebInstructionalTextGet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText);

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
L7_RC_t cpdmCPConfigWebLangCodeGet(cpId_t cpId, webId_t webId, L7_char8 *langCode);

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
L7_RC_t cpdmCPConfigWebLocaleLinkGet(cpId_t cpId, webId_t webId, L7_char8 *localeLink);

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
L7_RC_t cpdmCPConfigWebPasswordLabelGet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel);

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
L7_RC_t cpdmCPConfigWebTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebUserLabelGet(cpId_t cpId, webId_t webId, L7_char8 *userLabel);

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
L7_RC_t cpdmCPConfigWebNoAcceptMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptText);

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
L7_RC_t cpdmCPConfigWebWipMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *wipText);

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
L7_RC_t cpdmCPConfigWebDeniedMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *deniedText);

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
L7_RC_t cpdmCPConfigWebResourceMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *resourceText);

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
L7_RC_t cpdmCPConfigWebTimeoutMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *timeoutText);

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
L7_RC_t cpdmCPConfigWebWelcomeTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText);

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
L7_RC_t cpdmCPConfigWebWelcomeTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText);

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
L7_RC_t cpdmCPConfigWebScriptTextGet(cpId_t cpId, webId_t webId, L7_char8 *scriptText);

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
L7_RC_t cpdmCPConfigWebPopupTextGet(cpId_t cpId, webId_t webId, L7_char8 *popupText);

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
L7_RC_t cpdmCPConfigWebLogoutBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t cpdmCPConfigWebLogoutButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t cpdmCPConfigWebLogoutConfirmTextGet(cpId_t cpId, webId_t webId, L7_char8 *confirmText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/********************************************************************* 
 * @purpose  Set value of locale field "accountImageName"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *imageName @b{(input)} - new
 *           "imageName" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebAccountImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/********************************************************************* 
 * @purpose  Set value of locale field "brandingImageName"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *imageName @b{(input)} - new
 *           "imageName" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebBrandingImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*************************************************************
* @purpose  Set value of field "backgroundImageName"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param   webId_t webId @b{(input)} web Id
*
*  @param L7_char8 *imageName @b{(input)} - new value
*
*************************************************************/
L7_RC_t cpdmCPConfigWebBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/********************************************************************* 
 * @purpose  Set value of locale field "browserTitleText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *browserTitleText @b{(input)} - new
 *           "browserTitleText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *browserTitleText);

/********************************************************************* 
 * @purpose  Set value of locale field "langCode"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *langCode @b{(input)} - new
 *           "langCode" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebLangCodeSet(cpId_t cpId, webId_t webId, L7_char8 *langCode);

/********************************************************************* 
 * @purpose  Set value of locale field "localeLink"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *localeLink @b{(input)} - new
 *           "localeLink" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebLocaleLinkSet(cpId_t cpId, webId_t webId, L7_char8 *localeLink);

/********************************************************************* 
 * @purpose  Set value of locale field "titleText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *titleText @b{(input)} - new
 *           "titleText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

/********************************************************************* 
 * @purpose  Set value of locale field "accountLabel"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *accountLabel @b{(input)} - new
 *           "accountLabel" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebAccountLabelSet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel);

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
L7_RC_t cpdmCPConfigWebFontListSet(cpId_t cpId, webId_t webId, L7_char8 *fontList);

/********************************************************************* 
 * @purpose  Set value of locale field "userLabel"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *userLabel @b{(input)} - new
 *           "userLabel" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebUserLabelSet(cpId_t cpId, webId_t webId, L7_char8 *userLabel);

/********************************************************************* 
 * @purpose  Set value of locale field "passwordLabel"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *passwordLabel @b{(input)} - new
 *           "passwordLabel" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebPasswordLabelSet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel);

/********************************************************************* 
 * @purpose  Set value of locale field "buttonLabel"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *buttonLabel @b{(input)} - new
 *           "buttonLabel" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t cpdmCPConfigWebInstructionalTextAppend(cpId_t cpId, webId_t webId, L7_char8 *instructionalText);

/********************************************************************* 
 * @purpose  Set value of locale field "instructionalText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *instructionalText @b{(input)} - new
 *           "instructionalText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebInstructionalTextSet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText);

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
L7_RC_t cpdmCPConfigWebAUPTextAppend(cpId_t cpId, webId_t webId, L7_char8 *aupText);

/********************************************************************* 
 * @purpose  Set value of locale field "aupText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *aupText @b{(input)} - new
 *           "aupText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebAUPTextSet(cpId_t cpId, webId_t webId, L7_char8 *aupText);

/********************************************************************* 
 * @purpose  Set value of locale field "acceptText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *acceptText @b{(input)} - new
 *           "acceptText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebAcceptTextSet(cpId_t cpId, webId_t webId, L7_char8 *acceptText);

/********************************************************************* 
 * @purpose  Set value of locale field "noAcceptMsg"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *noAcceptMsg @b{(input)} - new
 *           "noAcceptMsg" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebNoAcceptMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptMsg);

/********************************************************************* 
 * @purpose  Set value of locale field "wipMsg"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *wipMsg @b{(input)} - new
 *           "wipMsg" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebWipMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *wipMsg);

/********************************************************************* 
 * @purpose  Set value of locale field "deniedMsg"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *deniedMsg @b{(input)} - new
 *           "deniedMsg" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebDeniedMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *deniedMsg);

/********************************************************************* 
 * @purpose  Set value of locale field "resourceMsg"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *resourceMsg @b{(input)} - new
 *           "resourceMsg" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebResourceMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *resourceMsg);

/********************************************************************* 
 * @purpose  Set value of locale field "timeoutMsg"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *timeoutMsg @b{(input)} - new
 *           "timeoutMsg" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebTimeoutMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *timeoutMsg);

/********************************************************************* 
 * @purpose  Set value of locale field "welcomeTitleText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *welcomeTitleText @b{(input)} - new
 *           "welcomeTitleText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebWelcomeTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText);

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
L7_RC_t cpdmCPConfigWebWelcomeTextAppend(cpId_t cpId, webId_t webId, L7_char8 *welcomeText);

/********************************************************************* 
 * @purpose  Set value of locale field "welcomeText"
 *
 * @param    cpId_t      cpId @b{(input)} CP config id
 * @param    webId_t  webId @b{(input)} web Id
 * @param    L7_char8 *welcomeText @b{(input)} - new
 *           "welcomeText" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigWebWelcomeTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText);

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
L7_RC_t cpdmCPConfigWebScriptTextSet(cpId_t cpId, webId_t webId, L7_char8 *scriptText);

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
L7_RC_t cpdmCPConfigWebPopupTextSet(cpId_t cpId, webId_t webId, L7_char8 *popupText);

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
L7_RC_t cpdmCPConfigWebLogoutBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t cpdmCPConfigWebLogoutContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutContentText);

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
L7_RC_t cpdmCPConfigWebLogoutButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel);

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
L7_RC_t cpdmCPConfigWebLogoutConfirmTextSet(cpId_t cpId, webId_t webId, L7_char8 *confirmText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutSuccessContentText);

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
L7_RC_t cpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

L7_RC_t cpdmCPConfigWebImageFileAdd();
L7_RC_t cpdmCPConfigWebImageFileDelete();

/*********************************************************************
 *
 * @purpose  Get the CP Web account image name for this configuration.
 *           This does not include any directory specification.
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
L7_RC_t cpdmCPConfigWebAccountImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*********************************************************************
 *
 * @purpose  Get the CP Web branding image name for this configuration.
 *           This does not include any directory specification.
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
L7_RC_t cpdmCPConfigWebBrandingImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

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
L7_RC_t cpdmCPConfigWebBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

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
L7_RC_t cpdmCPConfigWebImageResetAll(L7_char8 *imageName);


#endif /* INCLUDE_CPDM_WEB_API_H */

