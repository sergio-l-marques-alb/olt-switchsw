/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpdm_web_api.c
*
* @purpose      Captive Portal Data Manager (CPDM) API functions
*
* @component    CPDM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       07/26/2007
*
* @author       rjindal,dcaugherty
*
* @end
*
**********************************************************************/

#include <string.h>
#include "osapi.h"
#include "l7_ip_api.h"
#include "pw_scramble_api.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"
#include "cpdm_api.h"
#include "cpdm_web_api.h"
#include "cpdm.h"
#include "usmdb_util_api.h"

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);  


static L7_uint32 localeCount = 0;


static cpdmCustomLocaleSkeleton_t skels[CP_DEFAULT_LOCALE_MAX] =
{
  {
    WEB_DEF1_ACCOUNT_IMAGE_NAME,
    WEB_DEF1_BRANDING_IMAGE_NAME,
    WEB_DEF1_BACKGROUND_IMAGE_NAME,
    WEB_DEF1_BROWSER_TITLE_TEXT,
    WEB_DEF1_LANG_CODE,
    WEB_DEF1_LOCALE_LINK,
    WEB_DEF1_TITLE_TEXT,
    WEB_DEF1_ACCOUNT_LABEL,
    WEB_DEF1_FONT_LIST,
    WEB_DEF1_USER_LABEL,
    WEB_DEF1_PASSWORD_LABEL,
    WEB_DEF1_BUTTON_LABEL,
    WEB_DEF1_INSTRUCTIONAL_TEXT,
    WEB_DEF1_AUP_TEXT,
    WEB_DEF1_ACCEPT_TEXT,
    WEB_DEF1_NO_ACCEPT_MSG,
    WEB_DEF1_WIP_MSG,
    WEB_DEF1_DENIED_MSG,
    WEB_DEF1_RESOURCE_MSG,
    WEB_DEF1_TIMEOUT_MSG,
    WEB_DEF1_WELCOME_TITLE_TEXT,
    WEB_DEF1_WELCOME_TEXT,
    WEB_DEF1_SCRIPT_TEXT,
    WEB_DEF1_POPUP_TEXT,
    WEB_DEF1_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF1_LOGOUT_TITLE_TEXT,
    WEB_DEF1_LOGOUT_CONTENT_TEXT,
    WEB_DEF1_LOGOUT_BUTTON_LABEL,
    WEB_DEF1_LOGOUT_CONFIRM_TEXT,
    WEB_DEF1_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF1_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF1_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF1_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  },
  {
    WEB_DEF2_ACCOUNT_IMAGE_NAME,
    WEB_DEF2_BRANDING_IMAGE_NAME,
    WEB_DEF2_BACKGROUND_IMAGE_NAME,
    WEB_DEF2_BROWSER_TITLE_TEXT,
    WEB_DEF2_LANG_CODE,
    WEB_DEF2_LOCALE_LINK,
    WEB_DEF2_TITLE_TEXT,
    WEB_DEF2_ACCOUNT_LABEL,
    WEB_DEF2_FONT_LIST,
    WEB_DEF2_USER_LABEL,
    WEB_DEF2_PASSWORD_LABEL,
    WEB_DEF2_BUTTON_LABEL,
    WEB_DEF2_INSTRUCTIONAL_TEXT,
    WEB_DEF2_AUP_TEXT,
    WEB_DEF2_ACCEPT_TEXT,
    WEB_DEF2_NO_ACCEPT_MSG,
    WEB_DEF2_WIP_MSG,
    WEB_DEF2_DENIED_MSG,
    WEB_DEF2_RESOURCE_MSG,
    WEB_DEF2_TIMEOUT_MSG,
    WEB_DEF2_WELCOME_TITLE_TEXT,
    WEB_DEF2_WELCOME_TEXT,
    WEB_DEF2_SCRIPT_TEXT,
    WEB_DEF2_POPUP_TEXT,
    WEB_DEF2_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF2_LOGOUT_TITLE_TEXT,
    WEB_DEF2_LOGOUT_CONTENT_TEXT,
    WEB_DEF2_LOGOUT_BUTTON_LABEL,
    WEB_DEF2_LOGOUT_CONFIRM_TEXT,
    WEB_DEF2_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF2_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF2_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF2_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  },
  {
    WEB_DEF3_ACCOUNT_IMAGE_NAME,
    WEB_DEF3_BRANDING_IMAGE_NAME,
    WEB_DEF3_BACKGROUND_IMAGE_NAME,
    WEB_DEF3_BROWSER_TITLE_TEXT,
    WEB_DEF3_LANG_CODE,
    WEB_DEF3_LOCALE_LINK,
    WEB_DEF3_TITLE_TEXT,
    WEB_DEF3_ACCOUNT_LABEL,
    WEB_DEF3_FONT_LIST,
    WEB_DEF3_USER_LABEL,
    WEB_DEF3_PASSWORD_LABEL,
    WEB_DEF3_BUTTON_LABEL,
    WEB_DEF3_INSTRUCTIONAL_TEXT,
    WEB_DEF3_AUP_TEXT,
    WEB_DEF3_ACCEPT_TEXT,
    WEB_DEF3_NO_ACCEPT_MSG,
    WEB_DEF3_WIP_MSG,
    WEB_DEF3_DENIED_MSG,
    WEB_DEF3_RESOURCE_MSG,
    WEB_DEF3_TIMEOUT_MSG,
    WEB_DEF3_WELCOME_TITLE_TEXT,
    WEB_DEF3_WELCOME_TEXT,
    WEB_DEF3_SCRIPT_TEXT,
    WEB_DEF3_POPUP_TEXT,
    WEB_DEF3_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF3_LOGOUT_TITLE_TEXT,
    WEB_DEF3_LOGOUT_CONTENT_TEXT,
    WEB_DEF3_LOGOUT_BUTTON_LABEL,
    WEB_DEF3_LOGOUT_CONFIRM_TEXT,
    WEB_DEF3_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF3_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF3_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF3_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  },
  {
    WEB_DEF4_ACCOUNT_IMAGE_NAME,
    WEB_DEF4_BRANDING_IMAGE_NAME,
    WEB_DEF4_BACKGROUND_IMAGE_NAME,
    WEB_DEF4_BROWSER_TITLE_TEXT,
    WEB_DEF4_LANG_CODE,
    WEB_DEF4_LOCALE_LINK,
    WEB_DEF4_TITLE_TEXT,
    WEB_DEF4_ACCOUNT_LABEL,
    WEB_DEF4_FONT_LIST,
    WEB_DEF4_USER_LABEL,
    WEB_DEF4_PASSWORD_LABEL,
    WEB_DEF4_BUTTON_LABEL,
    WEB_DEF4_INSTRUCTIONAL_TEXT,
    WEB_DEF4_AUP_TEXT,
    WEB_DEF4_ACCEPT_TEXT,
    WEB_DEF4_NO_ACCEPT_MSG,
    WEB_DEF4_WIP_MSG,
    WEB_DEF4_DENIED_MSG,
    WEB_DEF4_RESOURCE_MSG,
    WEB_DEF4_TIMEOUT_MSG,
    WEB_DEF4_WELCOME_TITLE_TEXT,
    WEB_DEF4_WELCOME_TEXT,
    WEB_DEF4_SCRIPT_TEXT,
    WEB_DEF4_POPUP_TEXT,
    WEB_DEF4_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF4_LOGOUT_TITLE_TEXT,
    WEB_DEF4_LOGOUT_CONTENT_TEXT,
    WEB_DEF4_LOGOUT_BUTTON_LABEL,
    WEB_DEF4_LOGOUT_CONFIRM_TEXT,
    WEB_DEF4_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF4_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF4_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF4_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  },
  {
    WEB_DEF5_ACCOUNT_IMAGE_NAME,
    WEB_DEF5_BRANDING_IMAGE_NAME,
    WEB_DEF5_BACKGROUND_IMAGE_NAME,
    WEB_DEF5_BROWSER_TITLE_TEXT,
    WEB_DEF5_LANG_CODE,
    WEB_DEF5_LOCALE_LINK,
    WEB_DEF5_TITLE_TEXT,
    WEB_DEF5_ACCOUNT_LABEL,
    WEB_DEF5_FONT_LIST,
    WEB_DEF5_USER_LABEL,
    WEB_DEF5_PASSWORD_LABEL,
    WEB_DEF5_BUTTON_LABEL,
    WEB_DEF5_INSTRUCTIONAL_TEXT,
    WEB_DEF5_AUP_TEXT,
    WEB_DEF5_ACCEPT_TEXT,
    WEB_DEF5_NO_ACCEPT_MSG,
    WEB_DEF5_WIP_MSG,
    WEB_DEF5_DENIED_MSG,
    WEB_DEF5_RESOURCE_MSG,
    WEB_DEF5_TIMEOUT_MSG,
    WEB_DEF5_WELCOME_TITLE_TEXT,
    WEB_DEF5_WELCOME_TEXT,
    WEB_DEF5_SCRIPT_TEXT,
    WEB_DEF5_POPUP_TEXT,
    WEB_DEF5_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF5_LOGOUT_TITLE_TEXT,
    WEB_DEF5_LOGOUT_CONTENT_TEXT,
    WEB_DEF5_LOGOUT_BUTTON_LABEL,
    WEB_DEF5_LOGOUT_CONFIRM_TEXT,
    WEB_DEF5_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF5_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF5_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF5_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  },
  {
    WEB_DEF6_ACCOUNT_IMAGE_NAME,
    WEB_DEF6_BRANDING_IMAGE_NAME,
    WEB_DEF6_BACKGROUND_IMAGE_NAME,
    WEB_DEF6_BROWSER_TITLE_TEXT,
    WEB_DEF6_LANG_CODE,
    WEB_DEF6_LOCALE_LINK,
    WEB_DEF6_TITLE_TEXT,
    WEB_DEF6_ACCOUNT_LABEL,
    WEB_DEF6_FONT_LIST,
    WEB_DEF6_USER_LABEL,
    WEB_DEF6_PASSWORD_LABEL,
    WEB_DEF6_BUTTON_LABEL,
    WEB_DEF6_INSTRUCTIONAL_TEXT,
    WEB_DEF6_AUP_TEXT,
    WEB_DEF6_ACCEPT_TEXT,
    WEB_DEF6_NO_ACCEPT_MSG,
    WEB_DEF6_WIP_MSG,
    WEB_DEF6_DENIED_MSG,
    WEB_DEF6_RESOURCE_MSG,
    WEB_DEF6_TIMEOUT_MSG,
    WEB_DEF6_WELCOME_TITLE_TEXT,
    WEB_DEF6_WELCOME_TEXT,
    WEB_DEF6_SCRIPT_TEXT,
    WEB_DEF6_POPUP_TEXT,
    WEB_DEF6_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF6_LOGOUT_TITLE_TEXT,
    WEB_DEF6_LOGOUT_CONTENT_TEXT,
    WEB_DEF6_LOGOUT_BUTTON_LABEL,
    WEB_DEF6_LOGOUT_CONFIRM_TEXT,
    WEB_DEF6_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF6_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF6_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF6_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  },
  {
    WEB_DEF7_ACCOUNT_IMAGE_NAME,
    WEB_DEF7_BRANDING_IMAGE_NAME,
    WEB_DEF7_BACKGROUND_IMAGE_NAME,
    WEB_DEF7_BROWSER_TITLE_TEXT,
    WEB_DEF7_LANG_CODE,
    WEB_DEF7_LOCALE_LINK,
    WEB_DEF7_TITLE_TEXT,
    WEB_DEF7_ACCOUNT_LABEL,
    WEB_DEF7_FONT_LIST,
    WEB_DEF7_USER_LABEL,
    WEB_DEF7_PASSWORD_LABEL,
    WEB_DEF7_BUTTON_LABEL,
    WEB_DEF7_INSTRUCTIONAL_TEXT,
    WEB_DEF7_AUP_TEXT,
    WEB_DEF7_ACCEPT_TEXT,
    WEB_DEF7_NO_ACCEPT_MSG,
    WEB_DEF7_WIP_MSG,
    WEB_DEF7_DENIED_MSG,
    WEB_DEF7_RESOURCE_MSG,
    WEB_DEF7_TIMEOUT_MSG,
    WEB_DEF7_WELCOME_TITLE_TEXT,
    WEB_DEF7_WELCOME_TEXT,
    WEB_DEF7_SCRIPT_TEXT,
    WEB_DEF7_POPUP_TEXT,
    WEB_DEF7_LOGOUT_BROWSER_TITLE_TEXT,
    WEB_DEF7_LOGOUT_TITLE_TEXT,
    WEB_DEF7_LOGOUT_CONTENT_TEXT,
    WEB_DEF7_LOGOUT_BUTTON_LABEL,
    WEB_DEF7_LOGOUT_CONFIRM_TEXT,
    WEB_DEF7_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT,
    WEB_DEF7_LOGOUT_SUCCESS_TITLE_TEXT,
    WEB_DEF7_LOGOUT_SUCCESS_CONTENT_TEXT,
    WEB_DEF7_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME
  }
};

/*********************************************************************
*
* @purpose  Find a CP ID in our config database
*
* @param    cpId_t cpId @b{(input)} captive portal ID
*
* @returns  cpdmConfigData_t *, L7_NULLPTR if config can't be found
*
* @comments This is a helper routine that should NOT be called
*           outside of its module.  Its whole purpose is to avoid
*           repetitive redundancy. 
*           It should also have semaphore protection as well.  
* @end 
*
*********************************************************************/
extern cpdmConfigData_t * cpdmFindConfig(cpId_t cpId);


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
void cpdmWebIdCountUpdate(void)
{
  L7_uint32 count = 0;

  SEMA_TAKE(WRITE);
  if (L7_NULLPTR != cpdmCfgData)
  {
    cpdmCustomLocale_t * pBase = &(cpdmCfgData->cpLocaleData[0]);
    for(count = 0; 
      	count < FD_CP_CONFIG_MAX * FD_CP_CUSTOM_LOCALE_MAX; count++)
    {
      if (0 == pBase[count].key.cpId)
      {
        break;
      }
    }
  }
  localeCount = count;
  SEMA_GIVE(WRITE);
}

/*********************************************************************
*
* @purpose  Return count of webId's entries for given cpId
*
* @param    none
*
* @returns  L7_uint32
*
* @comments none
*
* @end
*
*********************************************************************/
L7_uint32 cpdmWebIdCountGet(cpId_t cpId)
{
  L7_uint32 count = 0;
  L7_uint32 idx;
  cpdmCustomLocale_t * pBase = &(cpdmCfgData->cpLocaleData[0]);

  for (idx = 0; idx < localeCount; idx++)
  {
    if (cpId == pBase[idx].key.cpId)
    {
      count++;
    }
  }  
  return count;
}


/*********************************************************************
*
* @purpose  Find a custom locale in our config database
*
* @param    cpId_t cpId  @b{(input)} captive portal ID
* @param    cpId_t webId @b{(input)} web ID
*
* @returns  L7_int32 index in our array, or value < 0 if 
*           item cannot be found.
*
* @comments This is a helper routine that should NOT be called
*           outside of its module.  Its whole purpose is to avoid
*           repetitive redundancy. 
*           It should also have semaphore protection as well.  
* @end 
*
*********************************************************************/

static L7_int32 cpdmFindLocaleIndex(cpId_t cpId, webId_t webId)
{
  L7_int32 idx = -1;
  cpdmCustomLocale_t * pBase = &(cpdmCfgData->cpLocaleData[0]);

  if (0 == localeCount)
  {
    return -1;
  }

  for(idx = 0; idx < localeCount; idx++)
  {
    if ((cpId == pBase[idx].key.cpId) && (webId == pBase[idx].key.webId))
    {
      return idx;
    }
    else if (cpId < pBase[idx].key.cpId)
    { 
      /* if entry was here, we'd have found it already. Leave. */
      break;
    }
  }  
  return -2;
}

/*********************************************************************
*
* @purpose  Find a custom locale in our config database
*
* @param    cpId_t cpId  @b{(input)} captive portal ID
* @param    cpId_t webId @b{(input)} web ID
*
* @returns  cpdmCustomLocale_t *, L7_NULLPTR if locale can't be found
*
* @comments This is a helper routine that should NOT be called
*           outside of its module.  Its whole purpose is to avoid
*           repetitive redundancy. 
*           It should also have semaphore protection as well.  
* @end 
*
*********************************************************************/

static cpdmCustomLocale_t * cpdmFindLocale(cpId_t cpId, 
                                           webId_t webId)
{
  cpdmCustomLocale_t * pL = L7_NULLPTR;
  L7_int32 idx = cpdmFindLocaleIndex(cpId, webId);
  if (0 <= idx)
    pL = &(cpdmCfgData->cpLocaleData[idx]);
  return pL;
}

/*********************************************************************
*
* @purpose  Wipe out locales for a given CP ID
*
* @param    cpId_t cpId  @b{(input)} captive portal ID
*
* @returns  L7_uint32 number of locales purged
*
* @comments This routine does NOT check if the cpId == CP_ID_MIN.
*           We really need to disallow that, but right now that's
*           not possible.  This is because this routine is in an
*           API file, and no "util" file exists yet to hide its
*           implementation.  
*           
* @end 
*
*********************************************************************/
L7_uint32 cpdmCPConfigWebIdAllDelete(cpId_t cpId)
{
  L7_uint32 retval = 0;
  L7_int32  start = 0, end = 0;
  L7_BOOL   quitEarly = L7_FALSE;
  cpdmCustomLocale_t * pBase = L7_NULLPTR;

  /* First, find out what contiguous chunk of the array
   * holds our victims - where it starts, and one index past
   * the last victim.
   */

  SEMA_TAKE(WRITE);
  pBase = &(cpdmCfgData->cpLocaleData[0]);
  for(start = 0; start < localeCount; start++)
  {
    if (cpId == pBase[start].key.cpId)
    {
      break; /* Done. */
    }
    else if (cpId < pBase[start].key.cpId)
    {
      /* We would have found something by now. 
       * No point in continuing. 
       */
      quitEarly = L7_TRUE;
      break;
    }
  }  

  if (start == localeCount) /* Nothing to do! */
  {
    quitEarly = L7_TRUE;
  }

  if (L7_FALSE == quitEarly)
  {
    for(end = start + 1; end < localeCount; end++)
    {
      if (cpId != pBase[end].key.cpId)
      {
         break; /* Done. */
      }
    }  

    /* If we get here, we know something will be purged.
     * How many items?
     */
    retval = end - start;
    {
      L7_uint32 remainder = localeCount - end;

      if (0 < remainder)
      {
      /* If there are valid elements past the end of 
       * the group to purge, overwrite the victims with
       * them.
       */
        memmove(&pBase[start], &pBase[end], 
  	        remainder * sizeof(cpdmCustomLocale_t));
      }
      /* Now zero out the array from just past our new 
       * ending index to the former ending index.
       */
      memset(&pBase[start + remainder], 0, 
	     (end - start) * sizeof(cpdmCustomLocale_t));
    }
    localeCount -= retval;
  }

  if (retval > 0)
  {
    cpdmConfigData_t   *pCData = cpdmFindConfig(cpId);    
    if (L7_NULLPTR != pCData)
    {
      if (retval > pCData->localeCount)
      {
	/* Something bad happened. Try to recover. */
        LOG_MSG("CP id %d: Axed %d locales, but only had %d!?",
		cpId, retval, pCData->localeCount);
        pCData->localeCount = 0;
      }
      else
      {
        pCData->localeCount -= retval;
      }
    }
    else
    {
      LOG_MSG("Removed %d locales for CP id %d, which doesn't exist!?",
	      retval, cpId);
    }
  }
  SEMA_GIVE(WRITE);
  return retval;
}


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
L7_RC_t cpdmCPConfigWebIdAdd(cpId_t cpId, webId_t webId)
{ 
  cpdmConfigData_t   *pCData = L7_NULLPTR;
  cpdmCustomLocale_t *pData = L7_NULLPTR;
  cpdmCustomLocale_t *pBase = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  if ((webId < CP_WEB_ID_MIN) || (webId > FD_CP_CUSTOM_LOCALE_MAX))
  {
    LOG_MSG("%s: Invalid webId: %d", __FUNCTION__, webId);
    return rc;
  }

  SEMA_TAKE(WRITE);
  do 
  {
    L7_int32 idx;
    pCData = cpdmFindConfig(cpId);

    if (L7_NULLPTR == pCData)
    {
      LOG_MSG("%s: Invalid cpId:%d, does not exist", __FUNCTION__, cpId);
      break;
    }

    if (pCData->localeCount > FD_CP_CUSTOM_LOCALE_MAX) 
    {
      LOG_MSG("%s: Invalid webId:%d, reached maximum.", __FUNCTION__, webId);
      break;
    }

    pBase = &(cpdmCfgData->cpLocaleData[0]);
    for(idx = 0; idx < localeCount; idx++)
    {
      pData = &(pBase[idx]);
      if ((cpId == pBase[idx].key.cpId) && (webId == pBase[idx].key.webId))
      {
        rc = L7_ALREADY_CONFIGURED;
        break;
      }
      if (cpId < pBase[idx].key.cpId)
      { 
        break;   /* Found larger CP ID, so insert here. */
      }
      if ((cpId == pBase[idx].key.cpId) && 
          (webId < pBase[idx].key.webId))
      {
        break;   /* CP ID is equal, but web ID is greater. Insert here. */
      }
    }  

    if (L7_ALREADY_CONFIGURED == rc)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
         "%s: cpId:%d webId:%d already exists\n", __FUNCTION__, cpId, webId);
      break;
    }

    if (localeCount != idx)
    {
      /* push up later entries */
      memmove(&(pBase[idx + 1]), &(pBase[idx]), 
        (localeCount - idx) * sizeof(cpdmCustomLocale_t));
    }
    else
    {
      pData = &(pBase[idx]);
    }

    memset(pData, 0, sizeof(cpdmCustomLocale_t));
    pData->key.cpId = cpId;
    pData->key.webId = webId;
    localeCount++;
    pCData->localeCount++;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
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
*
* @returns  L7_SUCCESS if code is found and webId is initialized
*
* @end 
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebAddDefaults(cpId_t cpId, webId_t webId, L7_char8 *code, CP_PAGE_TYPE_t page)
{
  cpdmCustomLocaleSkeleton_t *pSkel = L7_NULLPTR;
  L7_BOOL fFound = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 idx;
  L7_uint32 len;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  if (L7_NULLPTR == code)
  {
    return rc;
  }

  len = strlen(code);
  if ((0 == len) || (len > CP_LANG_CODE_MAX))
  {
    return rc;
  }

  for (idx=0; idx<CP_DEFAULT_LOCALE_MAX; idx++)
  {
    pSkel = &(skels[idx]); 
    if (osapiStrncmp(pSkel->langCode,code,CP_LANG_CODE_MAX) == 0)
    {
      fFound = L7_TRUE;
      break;
    }
  }

  /* Use default if not adding a preferred default */
  if (L7_TRUE != fFound)
  {
    pSkel = &(skels[0]); 
  }

  SEMA_TAKE(WRITE);
  do 
  {
    cpdmCustomLocale_t *pConf = L7_NULLPTR;
    pConf = cpdmFindLocale(cpId, webId);
    osapiStrncpySafe(pConf->langCode, code, CP_LANG_CODE_MAX+1);
    osapiStrncpySafe(pConf->localeLink, pSkel->localeLink, CP_LOCALE_LINK_MAX+1);

    if (L7_NULLPTR == pConf)
    {
      break;
    }
    /* apply default values */
    pConf->key.cpId = cpId;
    pConf->key.webId = webId;

    if ((page==CP_GLOBAL_PAGE) || (page==CP_ALL))
    {
      osapiStrncpySafe(pConf->backgroundImageName, pSkel->backgroundImageName, L7_MAX_FILENAME+1);
      osapiStrncpySafe(pConf->brandingImageName, pSkel->brandingImageName, L7_MAX_FILENAME+1);
      osapiStrncpySafe(pConf->fontList, pSkel->fontList, CP_FONT_LIST_MAX+1);
      osapiStrncpySafe(pConf->scriptText, pSkel->scriptText, CP_SCRIPT_TEXT_MAX+1);
      osapiStrncpySafe(pConf->popupText, pSkel->popupText, CP_POPUP_TEXT_MAX+1);
    }
    if ((page==CP_AUTHENTICATION_PAGE) || (page==CP_ALL))
    {
      osapiStrncpySafe(pConf->browserTitleText, pSkel->browserTitleText, CP_BROWSER_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->titleText, pSkel->titleText, CP_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->accountImageName, pSkel->accountImageName, L7_MAX_FILENAME+1);
      osapiStrncpySafe(pConf->accountLabel, pSkel->accountLabel, CP_ACCOUNT_LABEL_MAX+1);
      osapiStrncpySafe(pConf->userLabel, pSkel->userLabel, CP_USER_LABEL_MAX+1);
      osapiStrncpySafe(pConf->passwordLabel,  pSkel->passwordLabel, CP_PASSWORD_LABEL_MAX+1);
      osapiStrncpySafe(pConf->buttonLabel, pSkel->buttonLabel, CP_BUTTON_LABEL_MAX+1);
      osapiStrncpySafe(pConf->instructionalText, pSkel->instructionalText, CP_INSTRUCTIONAL_TEXT_MAX+1);
      osapiStrncpySafe(pConf->aupText, pSkel->aupText, CP_AUP_TEXT_MAX+1);
      osapiStrncpySafe(pConf->acceptText, pSkel->acceptText, CP_ACCEPT_TEXT_MAX+1);
      osapiStrncpySafe(pConf->noAcceptMsg, pSkel->noAcceptMsg,  CP_MSG_TEXT_MAX+1);
      osapiStrncpySafe(pConf->deniedMsg, pSkel->deniedMsg, CP_MSG_TEXT_MAX+1);
      osapiStrncpySafe(pConf->wipMsg, pSkel->wipMsg, CP_MSG_TEXT_MAX+1);
      osapiStrncpySafe(pConf->resourceMsg, pSkel->resourceMsg, CP_MSG_TEXT_MAX+1); 
      osapiStrncpySafe(pConf->timeoutMsg, pSkel->timeoutMsg, CP_MSG_TEXT_MAX+1);
    }
    if ((page==CP_WELCOME_PAGE) || (page==CP_ALL))
    {
      osapiStrncpySafe(pConf->welcomeTitleText, pSkel->welcomeTitleText, CP_WELCOME_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->welcomeText, pSkel->welcomeText, CP_WELCOME_TEXT_MAX+1);
    }
    if ((page==CP_LOGOUT_PAGE) || (page==CP_ALL))
    {
      osapiStrncpySafe(pConf->logoutBrowserTitleText, pSkel->logoutBrowserTitleText, CP_LOGOUT_BROWSER_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->logoutTitleText, pSkel->logoutTitleText, CP_LOGOUT_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->logoutContentText, pSkel->logoutContentText, CP_LOGOUT_CONTENT_TEXT_MAX+1);
      osapiStrncpySafe(pConf->logoutButtonLabel, pSkel->logoutButtonLabel, CP_LOGOUT_BUTTON_LABEL_MAX+1);
      osapiStrncpySafe(pConf->logoutConfirmText, pSkel->logoutConfirmText, CP_LOGOUT_CONFIRM_TEXT_MAX+1);
    }
    if ((page==CP_LOGOUT_SUCCESS_PAGE) || (page==CP_ALL))
    {
      osapiStrncpySafe(pConf->logoutSuccessBrowserTitleText, pSkel->logoutSuccessBrowserTitleText, CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->logoutSuccessTitleText, pSkel->logoutSuccessTitleText, CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX+1);
      osapiStrncpySafe(pConf->logoutSuccessContentText, pSkel->logoutSuccessContentText, CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX+1);
      osapiStrncpySafe(pConf->logoutSuccessBackgroundImageName, pSkel->logoutSuccessBackgroundImageName, L7_MAX_FILENAME+1);
    }
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
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
* @returns  L7_BOOL if *def is equal to default value. Code is not
*           match worthy since its the index for the defaults.
*
* @end 
*
*********************************************************************/
L7_BOOL cpdmCPConfigWebIsDefault(L7_char8 *code, L7_char8 *param, L7_char8 *def)
{
  cpdmCustomLocaleSkeleton_t *pSkel = L7_NULLPTR;
  L7_BOOL fFound = L7_FALSE;
  L7_RC_t rc = L7_FALSE;
  L7_uint32 idx, len;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  len = strlen(code);
  if ((0 == len) || (len > CP_LANG_CODE_MAX))
  {
    return rc;
  }

  if ((L7_NULLPTR == param) || (L7_NULLPTR == def))
  {
    return rc;
  }

  for (idx=0; idx<CP_DEFAULT_LOCALE_MAX; idx++)
  {
    pSkel = &(skels[idx]); 
    if (osapiStrncmp(pSkel->langCode,code,CP_LANG_CODE_MAX) == 0)
    {
      fFound = L7_TRUE;
      break;
    }
  }

  /* if we didn't match a default code, then its definately not a default param */
  if (L7_TRUE != fFound)
  {
    return rc;
  }

  SEMA_TAKE(READ);

  if (osapiStrncmp(param,CP_PARAM_NAME_CODE,sizeof(CP_PARAM_NAME_CODE))==0)
  {
    rc = fFound;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_ACCOUNT_IMAGE,sizeof(CP_PARAM_NAME_ACCOUNT_IMAGE))==0)
  {
    rc = (osapiStrncmp(pSkel->accountImageName,def,CP_FILE_NAME_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_ACCOUNT_LABEL,sizeof(CP_PARAM_NAME_ACCOUNT_LABEL))==0)
  {
    rc = (osapiStrncmp(pSkel->accountLabel,def,CP_ACCOUNT_LABEL_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_ACCEPT_MSG,sizeof(CP_PARAM_NAME_ACCEPT_MSG))==0)
  {
    rc = (osapiStrncmp(pSkel->noAcceptMsg,def,CP_MSG_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_ACCEPT_TEXT,sizeof(CP_PARAM_NAME_ACCEPT_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->acceptText,def,CP_ACCEPT_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_AUP_TEXT,sizeof(CP_PARAM_NAME_AUP_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->aupText,def,CP_AUP_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_BACKGROUND_IMAGE,sizeof(CP_PARAM_NAME_BACKGROUND_IMAGE))==0)
  {
    rc = (osapiStrncmp(pSkel->backgroundImageName,def,CP_FILE_NAME_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_BUTTON_LABEL,sizeof(CP_PARAM_NAME_BUTTON_LABEL))==0)
  {
    rc = (osapiStrncmp(pSkel->buttonLabel,def,CP_BUTTON_LABEL_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_BRANDING_IMAGE,sizeof(CP_PARAM_NAME_BRANDING_IMAGE))==0)
  {
    rc = (osapiStrncmp(pSkel->brandingImageName,def,CP_FILE_NAME_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_BROWSER_TITLE,sizeof(CP_PARAM_NAME_BROWSER_TITLE))==0)
  {
    rc = (osapiStrncmp(pSkel->browserTitleText,def,CP_BROWSER_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_DENIED_MSG,sizeof(CP_PARAM_NAME_DENIED_MSG))==0)
  {
    rc = (osapiStrncmp(pSkel->deniedMsg,def,CP_MSG_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_FONT_LIST,sizeof(CP_PARAM_NAME_FONT_LIST))==0)
  {
    rc = (osapiStrncmp(pSkel->fontList,def,CP_FONT_LIST_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_INSTRUCTIONAL_TEXT,sizeof(CP_PARAM_NAME_INSTRUCTIONAL_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->instructionalText,def,CP_INSTRUCTIONAL_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LINK,sizeof(CP_PARAM_NAME_LINK))==0)
  {
    rc = (osapiStrncmp(pSkel->localeLink,def,CP_LOCALE_LINK_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_PASSWORD_LABEL,sizeof(CP_PARAM_NAME_PASSWORD_LABEL))==0)
  {
    rc = (osapiStrncmp(pSkel->passwordLabel,def,CP_PASSWORD_LABEL_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_RESOURCE_MSG,sizeof(CP_PARAM_NAME_RESOURCE_MSG))==0)
  {
    rc = (osapiStrncmp(pSkel->resourceMsg,def,CP_MSG_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_TITLE_TEXT,sizeof(CP_PARAM_NAME_TITLE_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->titleText,def,CP_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_TIMEOUT_MSG,sizeof(CP_PARAM_NAME_TIMEOUT_MSG))==0)
  {
    rc = (osapiStrncmp(pSkel->timeoutMsg,def,CP_MSG_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_USER_LABEL,sizeof(CP_PARAM_NAME_USER_LABEL))==0)
  {
    rc = (osapiStrncmp(pSkel->userLabel,def,CP_USER_LABEL_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_WELCOME_TITLE,sizeof(CP_PARAM_NAME_WELCOME_TITLE))==0)
  {
    rc = (osapiStrncmp(pSkel->welcomeTitleText,def,CP_WELCOME_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_WELCOME_TEXT,sizeof(CP_PARAM_NAME_WELCOME_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->welcomeText,def,CP_WELCOME_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_WIP_MSG,sizeof(CP_PARAM_NAME_WIP_MSG))==0)
  {
    rc = (osapiStrncmp(pSkel->wipMsg,def,CP_MSG_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_SCRIPT_TEXT,sizeof(CP_PARAM_NAME_SCRIPT_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->scriptText,def,CP_SCRIPT_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_POPUP_TEXT,sizeof(CP_PARAM_NAME_POPUP_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->popupText,def,CP_POPUP_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_BROWSER_TITLE,sizeof(CP_PARAM_NAME_LOGOUT_BROWSER_TITLE))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutBrowserTitleText,def,CP_LOGOUT_BROWSER_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_TITLE,sizeof(CP_PARAM_NAME_LOGOUT_TITLE))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutTitleText,def,CP_LOGOUT_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_CONTENT,sizeof(CP_PARAM_NAME_LOGOUT_CONTENT))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutContentText,def,CP_LOGOUT_CONTENT_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_BUTTON_LABEL,sizeof(CP_PARAM_NAME_LOGOUT_BUTTON_LABEL))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutButtonLabel,def,CP_LOGOUT_BUTTON_LABEL_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_CONFIRM_TEXT,sizeof(CP_PARAM_NAME_LOGOUT_CONFIRM_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutConfirmText,def,CP_LOGOUT_CONFIRM_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_SUCCESS_BROWSER_TITLE,sizeof(CP_PARAM_NAME_LOGOUT_SUCCESS_BROWSER_TITLE))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutSuccessBrowserTitleText,def,CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_SUCCESS_TITLE_TEXT,sizeof(CP_PARAM_NAME_LOGOUT_SUCCESS_TITLE_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutSuccessTitleText,def,CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_SUCCESS_CONTENT_TEXT,sizeof(CP_PARAM_NAME_LOGOUT_SUCCESS_CONTENT_TEXT))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutSuccessContentText,def,CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX)==0)?L7_TRUE:L7_FALSE;
  }
  else if (osapiStrncmp(param,CP_PARAM_NAME_LOGOUT_SUCCESS_BACKGROUND_IMAGE,sizeof(CP_PARAM_NAME_LOGOUT_SUCCESS_BACKGROUND_IMAGE))==0)
  {
    rc = (osapiStrncmp(pSkel->logoutSuccessBackgroundImageName,def,CP_FILE_NAME_MAX)==0)?L7_TRUE:L7_FALSE;
  }

  SEMA_GIVE(READ);
  return rc;
}

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
L7_RC_t cpdmCPConfigWebIdDelete(cpId_t cpId, webId_t webId)
{
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  if ((webId < CP_WEB_ID_MIN) || (webId > FD_CP_CUSTOM_LOCALE_MAX))
  {
    LOG_MSG("%s: Invalid webId: %d", __FUNCTION__, webId);
    return rc;
  }

  SEMA_TAKE(WRITE);
  do
  {
    L7_int32 idx = cpdmFindLocaleIndex(cpId, webId);
    cpdmCustomLocale_t *pBase = &(cpdmCfgData->cpLocaleData[0]);
    cpdmConfigData_t * pCData = L7_NULLPTR; 

    if (0 > idx)
    {
      break;
    }

    if (1 == cpdmWebIdCountGet(cpId))
    {
      LOG_MSG("Refusing to remove the last WebId:%d",webId);
      SEMA_GIVE(WRITE);
      return L7_REQUEST_DENIED;
    }

    localeCount--;
    memmove(&(pBase[idx]), &(pBase[idx + 1]),
	    (localeCount - idx) * sizeof(cpdmCustomLocale_t));
    memset(&(pBase[localeCount]), 0, sizeof(cpdmCustomLocale_t));

    pCData = cpdmFindConfig(cpId);
    if ((L7_NULLPTR == pCData) || (0 == pCData->localeCount))
    {
      break;
    }
    pCData->localeCount--;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(WRITE);

  return rc;
}

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
L7_RC_t cpdmCPConfigWebIdGet(cpId_t cpId, webId_t webId)
{
  L7_int32 idx = -1;

  SEMA_TAKE(READ);
  idx = cpdmFindLocaleIndex(cpId, webId);
  SEMA_GIVE(READ);
  return ((idx < 0) ? L7_FAILURE : L7_SUCCESS);
}

/*********************************************************************
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
L7_RC_t cpdmCPConfigWebIdNextGet(cpId_t cpId, webId_t webId, webId_t *nextWebId)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == nextWebId)
  {
    return rc;
  }

  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pBase = &(cpdmCfgData->cpLocaleData[0]);
    L7_int32 idx;

    if (0 == localeCount)
    {
      break;
    }
    if (0 == cpId)
    {
      break;
    }

    /* if web ID is 0, we'll find the first webId for this CP ID, 
     * or the next one (if it exists) otherwise
     */

    for(idx = 0; idx < localeCount; idx++) 
    {
      if ((cpId == pBase[idx].key.cpId) &&
	  (webId < pBase[idx].key.webId))
      {
        *nextWebId = pBase[idx].key.webId;
        rc = L7_SUCCESS;
        break;
      }
      else if (cpId < pBase[idx].key.cpId)
      {
	    break; /* not found */
      }
    }
  } while(0);
  SEMA_GIVE(READ);    

  return rc;
}

/********************************************************************* 
* @purpose  Get CP config Account imageName
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *imageName @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebAccountImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == imageName) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(imageName, pLoc->accountImageName, L7_MAX_FILENAME+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config Branding imageName
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *imageName @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebBrandingImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == imageName) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(imageName, pLoc->brandingImageName, L7_MAX_FILENAME+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config Background imageName
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *imageName @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == imageName) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(imageName, pLoc->backgroundImageName, L7_MAX_FILENAME+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config browserTitleText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *browserTitleText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *browserTitleText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == browserTitleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(browserTitleText, pLoc->browserTitleText, CP_BROWSER_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config langCode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *langCode @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebLangCodeGet(cpId_t cpId, webId_t webId, L7_char8 *langCode) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == langCode) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(langCode, pLoc->langCode, CP_LANG_CODE_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config localeLink
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *localeLink @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebLocaleLinkGet(cpId_t cpId, webId_t webId, L7_char8 *localeLink) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == localeLink) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(localeLink, pLoc->localeLink, CP_LOCALE_LINK_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config titleText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *titleText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == titleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(titleText, pLoc->titleText, CP_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config accountLabel
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *accountLabel @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebAccountLabelGet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == accountLabel) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(accountLabel, pLoc->accountLabel, CP_ACCOUNT_LABEL_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebFontListGet(cpId_t cpId, webId_t webId, L7_char8 *fontList) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == fontList) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(fontList, pLoc->fontList, CP_FONT_LIST_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config userLabel
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *userLabel @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebUserLabelGet(cpId_t cpId, webId_t webId, L7_char8 *userLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == userLabel) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(userLabel, pLoc->userLabel, CP_USER_LABEL_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config passwordLabel
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *passwordLabel @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebPasswordLabelGet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == passwordLabel) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(passwordLabel, pLoc->passwordLabel, CP_PASSWORD_LABEL_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config buttonLabel
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *buttonLabel @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == buttonLabel) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
    {
       break; 
    }
    osapiStrncpySafe(buttonLabel, pLoc->buttonLabel, CP_BUTTON_LABEL_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config instructionalText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *instructionalText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebInstructionalTextGet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == instructionalText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(instructionalText, pLoc->instructionalText, CP_INSTRUCTIONAL_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config aupText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *aupText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebAUPTextGet(cpId_t cpId, webId_t webId, L7_char8 *aupText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == aupText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(aupText, pLoc->aupText, CP_AUP_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config acceptText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *acceptText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebAcceptTextGet(cpId_t cpId, webId_t webId, L7_char8 *acceptText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == acceptText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(acceptText, pLoc->acceptText, CP_ACCEPT_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config noAcceptMsg
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *noAcceptMsg @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebNoAcceptMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == noAcceptMsg) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(noAcceptMsg, pLoc->noAcceptMsg, CP_MSG_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config wipMsg
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *wipMsg @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebWipMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *wipMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == wipMsg) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(wipMsg, pLoc->wipMsg, CP_MSG_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config deniedMsg
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *deniedMsg @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebDeniedMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *deniedMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == deniedMsg) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(deniedMsg, pLoc->deniedMsg, CP_MSG_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config resourceMsg
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *resourceMsg @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebResourceMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *resourceMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == resourceMsg) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(resourceMsg, pLoc->resourceMsg, CP_MSG_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config timeoutMsg
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *timeoutMsg @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebTimeoutMsgTextGet(cpId_t cpId, webId_t webId, L7_char8 *timeoutMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == timeoutMsg) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(timeoutMsg, pLoc->timeoutMsg, CP_MSG_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config welcomeTitleText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *welcomeTitleText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebWelcomeTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == welcomeTitleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(welcomeTitleText, pLoc->welcomeTitleText, CP_WELCOME_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Get CP config welcomeText
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    webId_t  webId @b{(input)} web ID
* @param    L7_char8 *welcomeText @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigWebWelcomeTextGet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == welcomeText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(welcomeText, pLoc->welcomeText, CP_WELCOME_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebScriptTextGet(cpId_t cpId, webId_t webId, L7_char8 *scriptText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == scriptText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(scriptText, pLoc->scriptText, CP_SCRIPT_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebPopupTextGet(cpId_t cpId, webId_t webId, L7_char8 *popupText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == popupText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(popupText, pLoc->popupText, CP_POPUP_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == titleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(titleText, pLoc->logoutBrowserTitleText, CP_LOGOUT_BROWSER_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == titleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(titleText, pLoc->logoutTitleText, CP_LOGOUT_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == contentText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(contentText, pLoc->logoutContentText, CP_LOGOUT_CONTENT_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutButtonLabelGet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == buttonLabel) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(buttonLabel, pLoc->logoutButtonLabel, CP_LOGOUT_BUTTON_LABEL_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutConfirmTextGet(cpId_t cpId, webId_t webId, L7_char8 *confirmText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == confirmText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(confirmText, pLoc->logoutConfirmText, CP_LOGOUT_CONFIRM_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == titleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(titleText, pLoc->logoutSuccessBrowserTitleText, CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessTitleTextGet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == titleText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(titleText, pLoc->logoutSuccessTitleText, CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessContentTextGet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == contentText) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(contentText, pLoc->logoutSuccessContentText, CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == imageName) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmCustomLocale_t *pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR == pLoc)
       break; 
    osapiStrncpySafe(imageName, pLoc->logoutSuccessBackgroundImageName, L7_MAX_FILENAME+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "accountImageName"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebAccountImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == imageName)
    {
      break;
    }
    if (strlen(imageName) > L7_MAX_FILENAME)
    {
      break;
    }

    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->accountImageName,imageName,L7_MAX_FILENAME) != 0)
        {
          osapiStrncpySafe(pLoc->accountImageName, imageName, L7_MAX_FILENAME+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "brandingImageName"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebBrandingImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == imageName) 
    {
      break;
    }
    if (strlen(imageName) > L7_MAX_FILENAME)
    {
      break;
    }

    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->brandingImageName,imageName,L7_MAX_FILENAME) != 0)
        {
          osapiStrncpySafe(pLoc->brandingImageName, imageName, L7_MAX_FILENAME+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "backgroundImageName"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == imageName) 
    {
      break;
    }
    if (strlen(imageName) > L7_MAX_FILENAME)
    {
      break;
    }

    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->backgroundImageName,imageName,L7_MAX_FILENAME) != 0)
        {
          osapiStrncpySafe(pLoc->backgroundImageName, imageName, L7_MAX_FILENAME+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}


/********************************************************************* 
* @purpose  Set value of locale field "browserTitleText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *browserTitleText @b{(input)} - new
*           "browserTitleText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments zero length is acceptable
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *browserTitleText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }
  /* Validate incoming hex string length */
  res = div(strlen(browserTitleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(browserTitleText))
  {
    return rc;
  }
  if (L7_NULLPTR == browserTitleText) 
  {
    return rc;
  }
  if (strlen(browserTitleText) > CP_BROWSER_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->browserTitleText, browserTitleText, CP_BROWSER_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->browserTitleText, browserTitleText, CP_BROWSER_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "langCode"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebLangCodeSet(cpId_t cpId, webId_t webId, L7_char8 *langCode) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }
  if (L7_NULLPTR == langCode) 
  {
    return rc;
  }
  len = strlen(langCode);
  if ((0 == len) || (len > CP_LANG_CODE_MAX))
  {
   return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->langCode, langCode, CP_LANG_CODE_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->langCode, langCode, CP_LANG_CODE_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "localeLink"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebLocaleLinkSet(cpId_t cpId, webId_t webId, L7_char8 *localeLink) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(localeLink),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(localeLink))
  {
    return rc;
  }
  if (L7_NULLPTR == localeLink) 
  {
    return rc;
  }
  len = strlen(localeLink);
  if ((0 == len) || (len > CP_LOCALE_LINK_MAX))
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->localeLink, localeLink, CP_LOCALE_LINK_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->localeLink, localeLink, CP_LOCALE_LINK_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "titleText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *titleText @b{(input)} - new
*           "titleText" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments zero length is acceptable
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(titleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(titleText))
  {
    return rc;
  }
  if (L7_NULLPTR == titleText) 
  {
    return rc;
  }
  if (strlen(titleText) > CP_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->titleText, titleText, CP_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->titleText, titleText, CP_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "accountLabel"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
* @param    L7_char8 *accountLabel @b{(input)} - new
*           "accountLabel" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments zero length is acceptable
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigWebAccountLabelSet(cpId_t cpId, webId_t webId, L7_char8 *accountLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(accountLabel),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(accountLabel))
  {
    return rc;
  }
  if (L7_NULLPTR == accountLabel) 
  {
    return rc;
  }
  if (strlen(accountLabel) > CP_ACCOUNT_LABEL_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->accountLabel, accountLabel, CP_ACCOUNT_LABEL_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->accountLabel, accountLabel, CP_ACCOUNT_LABEL_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebFontListSet(cpId_t cpId, webId_t webId, L7_char8 *fontList) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  if (L7_NULLPTR == fontList)
  {
    return rc;
  }
  if (strlen(fontList) > CP_FONT_LIST_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->fontList, fontList, CP_FONT_LIST_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->fontList, fontList, CP_FONT_LIST_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "userLabel"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebUserLabelSet(cpId_t cpId, webId_t webId, L7_char8 *userLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(userLabel),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(userLabel))
  {
    return rc;
  }
  if (L7_NULLPTR == userLabel) 
  {
    return rc;
  }
  if (strlen(userLabel) > CP_USER_LABEL_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->userLabel, userLabel, CP_USER_LABEL_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->userLabel, userLabel, CP_USER_LABEL_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "passwordLabel"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebPasswordLabelSet(cpId_t cpId, webId_t webId, L7_char8 *passwordLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(passwordLabel),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(passwordLabel))
  {
    return rc;
  }
  if (L7_NULLPTR == passwordLabel) 
  {
    return rc;
  }
  if (strlen(passwordLabel) > CP_PASSWORD_LABEL_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->passwordLabel, passwordLabel, CP_PASSWORD_LABEL_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->passwordLabel, passwordLabel, CP_PASSWORD_LABEL_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "buttonLabel"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(buttonLabel),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(buttonLabel))
  {
    return rc;
  }
  if (L7_NULLPTR == buttonLabel) 
  {
    return rc;
  }
  len = strlen(buttonLabel);
  if ((0 == len) || (len > CP_BUTTON_LABEL_MAX))
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->buttonLabel, buttonLabel, CP_BUTTON_LABEL_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->buttonLabel, buttonLabel, CP_BUTTON_LABEL_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebInstructionalTextAppend(cpId_t cpId, webId_t webId, L7_char8 *instructionalText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(instructionalText))
  {
    return rc;
  }
  if (L7_NULLPTR == instructionalText) 
  {
    return rc;
  }
  if (strlen(instructionalText) > CP_INSTRUCTIONAL_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (CP_INSTRUCTIONAL_TEXT_MAX >= (strlen(pLoc->instructionalText)+(strlen(instructionalText))))
      {
      osapiStrncat(pLoc->instructionalText, instructionalText, sizeof(pLoc->instructionalText)-strlen(pLoc->instructionalText));
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      }
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "instructionalText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebInstructionalTextSet(cpId_t cpId, webId_t webId, L7_char8 *instructionalText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(instructionalText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(instructionalText))
  {
    return rc;
  }
  if (L7_NULLPTR == instructionalText)
  {
    return rc;
  }
  if (strlen(instructionalText) > CP_INSTRUCTIONAL_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->instructionalText, instructionalText, CP_INSTRUCTIONAL_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->instructionalText, instructionalText, CP_INSTRUCTIONAL_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebAUPTextAppend(cpId_t cpId, webId_t webId, L7_char8 *aupText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(aupText))
  {
    return rc;
  }
  if (L7_NULLPTR == aupText) 
  {
    return rc;
  }
  if (strlen(aupText) > CP_AUP_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (CP_AUP_TEXT_MAX >= (strlen(pLoc->aupText)+(strlen(aupText))))
      {
      osapiStrncat(pLoc->aupText, aupText, sizeof(pLoc->aupText)-strlen(pLoc->aupText));
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      }
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "aupText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebAUPTextSet(cpId_t cpId, webId_t webId, L7_char8 *aupText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(aupText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(aupText))
  {
    return rc;
  }
  if (L7_NULLPTR == aupText) 
  {
    return rc;
  }
  if (strlen(aupText) > CP_AUP_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->aupText, aupText, CP_AUP_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->aupText, aupText, CP_AUP_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "acceptText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebAcceptTextSet(cpId_t cpId, webId_t webId, L7_char8 *acceptText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(acceptText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(acceptText))
  {
    return rc;
  }
  if (L7_NULLPTR == acceptText) 
  {
    return rc;
  }
  if (strlen(acceptText) > CP_ACCEPT_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->acceptText, acceptText, CP_ACCEPT_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->acceptText, acceptText, CP_ACCEPT_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "noAcceptMsg"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebNoAcceptMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *noAcceptMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(noAcceptMsg),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(noAcceptMsg))
  {
    return rc;
  }
  if (L7_NULLPTR == noAcceptMsg) 
  {
    return rc;
  }
  if (strlen(noAcceptMsg) > CP_MSG_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->noAcceptMsg, noAcceptMsg, CP_MSG_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->noAcceptMsg, noAcceptMsg, CP_MSG_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "wipMsg"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebWipMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *wipMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

 /* Validate incoming hex string length */
  res = div(strlen(wipMsg),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(wipMsg))
  {
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == wipMsg) 
      break;
    len = strlen(wipMsg);
    if ((0 == len) || (len > CP_MSG_TEXT_MAX))
      break;
    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->wipMsg, wipMsg, CP_MSG_TEXT_MAX) != 0)
        {
          osapiStrncpySafe(pLoc->wipMsg, wipMsg, CP_MSG_TEXT_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "deniedMsg"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebDeniedMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *deniedMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

 /* Validate incoming hex string length */
  res = div(strlen(deniedMsg),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(deniedMsg))
  {
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == deniedMsg) 
      break;
    len = strlen(deniedMsg);
    if ((0 == len) || (len > CP_MSG_TEXT_MAX))
      break;
    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->deniedMsg, deniedMsg, CP_MSG_TEXT_MAX) != 0)
        {
          osapiStrncpySafe(pLoc->deniedMsg, deniedMsg, CP_MSG_TEXT_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "resourceMsg"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebResourceMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *resourceMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(resourceMsg),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(resourceMsg))
  {
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == resourceMsg) 
      break;
    len = strlen(resourceMsg);
    if ((0 == len) || (len > CP_MSG_TEXT_MAX))
      break;
    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->resourceMsg, resourceMsg, CP_MSG_TEXT_MAX) != 0)
        {
          osapiStrncpySafe(pLoc->resourceMsg, resourceMsg, CP_MSG_TEXT_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "timeoutMsg"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebTimeoutMsgTextSet(cpId_t cpId, webId_t webId, L7_char8 *timeoutMsg) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(timeoutMsg),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(timeoutMsg))
  {
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == timeoutMsg) 
      break;
    len = strlen(timeoutMsg);
    if ((0 == len) || (len > CP_MSG_TEXT_MAX))
      break;
    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->timeoutMsg, timeoutMsg, CP_MSG_TEXT_MAX) != 0)
        {
          osapiStrncpySafe(pLoc->timeoutMsg, timeoutMsg, CP_MSG_TEXT_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}

/********************************************************************* 
* @purpose  Set value of locale field "welcomeTitleText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebWelcomeTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeTitleText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(welcomeTitleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(welcomeTitleText))
  {
    return rc;
  }
  if (L7_NULLPTR == welcomeTitleText) 
  {
    return rc;
  }
  if (strlen(welcomeTitleText) > CP_WELCOME_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->welcomeTitleText, welcomeTitleText, CP_WELCOME_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->welcomeTitleText, welcomeTitleText, CP_WELCOME_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebWelcomeTextAppend(cpId_t cpId, webId_t webId, L7_char8 *welcomeText) 
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(welcomeText))
  {
    return rc;
  }
  if (L7_NULLPTR == welcomeText) 
  {
    return rc;
  }
  if (strlen(welcomeText) > CP_WELCOME_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (CP_WELCOME_TEXT_MAX >= (strlen(pLoc->welcomeText)+(strlen(welcomeText))))
      {
      osapiStrncat(pLoc->welcomeText, welcomeText, sizeof(pLoc->welcomeText)-strlen(pLoc->welcomeText));
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      }
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
}


/********************************************************************* 
* @purpose  Set value of locale field "welcomeText"
*
* @param    cpId_t      cpId @b{(input)} CP config id
* @param    L7_short16  webId @b{(input)} web Id
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
L7_RC_t cpdmCPConfigWebWelcomeTextSet(cpId_t cpId, webId_t webId, L7_char8 *welcomeText) 
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(welcomeText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(welcomeText))
  {
    return rc;
  }
  if (L7_NULLPTR == welcomeText)
  {
    return rc;
  }
  if (strlen(welcomeText) > CP_WELCOME_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->welcomeText, welcomeText, CP_WELCOME_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->welcomeText, welcomeText, CP_WELCOME_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebScriptTextSet(cpId_t cpId, webId_t webId, L7_char8 *scriptText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(scriptText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(scriptText))
  {
    return rc;
  }
  if (L7_NULLPTR == scriptText)
  {
    return rc;
  }
  if (strlen(scriptText) > CP_SCRIPT_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->scriptText, scriptText, CP_SCRIPT_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->scriptText, scriptText, CP_SCRIPT_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebPopupTextSet(cpId_t cpId, webId_t webId, L7_char8 *popupText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(popupText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(popupText))
  {
    return rc;
  }
  if (L7_NULLPTR == popupText)
  {
    return rc;
  }
  if (strlen(popupText) > CP_POPUP_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->popupText, popupText, CP_POPUP_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->popupText, popupText, CP_POPUP_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(titleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(titleText))
  {
    return rc;
  }
  if (L7_NULLPTR == titleText)
  {
    return rc;
  }
  if (strlen(titleText) > CP_LOGOUT_BROWSER_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutBrowserTitleText, titleText, CP_LOGOUT_BROWSER_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutBrowserTitleText, titleText, CP_LOGOUT_BROWSER_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(titleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(titleText))
  {
    return rc;
  }
  if (L7_NULLPTR == titleText)
  {
    return rc;
  }
  if (strlen(titleText) > CP_LOGOUT_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutTitleText, titleText, CP_LOGOUT_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutTitleText, titleText, CP_LOGOUT_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(contentText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(contentText))
  {
    return rc;
  }
  if (L7_NULLPTR == contentText)
  {
    return rc;
  }
  if (strlen(contentText) > CP_LOGOUT_CONTENT_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutContentText, contentText, CP_LOGOUT_CONTENT_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutContentText, contentText, CP_LOGOUT_CONTENT_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutContentText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(logoutContentText))
  {
    return rc;
  }
  if (L7_NULLPTR == logoutContentText) 
  {
    return rc;
  }
  if (strlen(logoutContentText) > CP_LOGOUT_CONTENT_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (CP_LOGOUT_CONTENT_TEXT_MAX >= (strlen(pLoc->logoutContentText)+(strlen(logoutContentText))))
      {
      osapiStrncat(pLoc->logoutContentText, logoutContentText, sizeof(pLoc->logoutContentText)-strlen(pLoc->logoutContentText));
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      }
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutButtonLabelSet(cpId_t cpId, webId_t webId, L7_char8 *buttonLabel)
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(buttonLabel),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(buttonLabel))
  {
    return rc;
  }
  if (L7_NULLPTR == buttonLabel) 
  {
    return rc;
  }
  len = strlen(buttonLabel);
  if ((0 == len) || (len > CP_LOGOUT_BUTTON_LABEL_MAX))
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutButtonLabel, buttonLabel, CP_LOGOUT_BUTTON_LABEL_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutButtonLabel, buttonLabel, CP_LOGOUT_BUTTON_LABEL_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutConfirmTextSet(cpId_t cpId, webId_t webId, L7_char8 *confirmText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(confirmText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(confirmText))
  {
    return rc;
  }
  if (L7_NULLPTR == confirmText)
  {
    return rc;
  }
  if (strlen(confirmText) > CP_LOGOUT_CONFIRM_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutConfirmText, confirmText, CP_LOGOUT_CONFIRM_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutConfirmText, confirmText, CP_LOGOUT_CONFIRM_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessBrowserTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(titleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(titleText))
  {
    return rc;
  }
  if (L7_NULLPTR == titleText)
  {
    return rc;
  }
  if (strlen(titleText) > CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutSuccessBrowserTitleText, titleText, CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutSuccessBrowserTitleText, titleText, CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessTitleTextSet(cpId_t cpId, webId_t webId, L7_char8 *titleText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(titleText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(titleText))
  {
    return rc;
  }
  if (L7_NULLPTR == titleText)
  {
    return rc;
  }
  if (strlen(titleText) > CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutSuccessTitleText, titleText, CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutSuccessTitleText, titleText, CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessContentTextSet(cpId_t cpId, webId_t webId, L7_char8 *contentText)
{ 
  L7_RC_t rc = L7_FAILURE; 
  div_t res;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  res = div(strlen(contentText),4);
  if (0 != res.rem)
  {
    return rc;
  }
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(contentText))
  {
    return rc;
  }
  if (L7_NULLPTR == contentText)
  {
    return rc;
  }
  if (strlen(contentText) > CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (osapiStrncmp(pLoc->logoutSuccessContentText, contentText, CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX) != 0)
      {
        osapiStrncpySafe(pLoc->logoutSuccessContentText, contentText, CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessContentTextAppend(cpId_t cpId, webId_t webId, L7_char8 *logoutSuccessContentText)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  /* Validate incoming hex string length */
  if (L7_SUCCESS != usmDbStringHexadecimalCheck(logoutSuccessContentText))
  {
    return rc;
  }
  if (L7_NULLPTR == logoutSuccessContentText) 
  {
    return rc;
  }
  if (strlen(logoutSuccessContentText) > CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX)
  {
    return rc;
  }

  do 
  {  
    cpdmCustomLocale_t *pLoc = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pLoc = cpdmFindLocale(cpId, webId); 
    if (L7_NULLPTR != pLoc)
    {
      if (CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX >= (strlen(pLoc->logoutSuccessContentText)+(strlen(logoutSuccessContentText))))
      {
      osapiStrncat(pLoc->logoutSuccessContentText, logoutSuccessContentText, sizeof(pLoc->logoutSuccessContentText)-strlen(pLoc->logoutSuccessContentText));
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      }
    }
    SEMA_GIVE(WRITE); 
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId_t cpId, webId_t webId, L7_char8 *imageName)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    LOG_MSG("%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  do 
  {  
    if (L7_NULLPTR == imageName)
    {
      break;
    }
    if (strlen(imageName) > L7_MAX_FILENAME)
    {
      break;
    }

    {
      cpdmCustomLocale_t *pLoc = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pLoc = cpdmFindLocale(cpId, webId); 
      if (L7_NULLPTR != pLoc)
      {
        if (osapiStrncmp(pLoc->logoutSuccessBackgroundImageName,imageName,L7_MAX_FILENAME) != 0)
        {
          osapiStrncpySafe(pLoc->logoutSuccessBackgroundImageName, imageName, L7_MAX_FILENAME+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
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
L7_RC_t cpdmCPConfigWebImageResetAll(L7_char8 *imageName)
{
  L7_char8 tmpName[CP_FILE_NAME_MAX+1];
  cpId_t cpId;
  webId_t webId;

  cpId = 0;
  while (L7_SUCCESS == cpdmCPConfigNextGet(cpId, &cpId))
  {
    webId = 0;
    while (L7_SUCCESS == cpdmCPConfigWebIdNextGet(cpId,webId,&webId))
    {
      memset(tmpName,0,sizeof(tmpName));
      cpdmCPConfigWebAccountImageNameGet(cpId,webId,tmpName);
      if (osapiStrncmp(imageName,tmpName,CP_FILE_NAME_MAX) == 0)
      {
        cpdmCPConfigWebAccountImageNameSet(cpId,webId,WEB_DEF_ACCOUNT_IMAGE_NAME);
      }
      memset(tmpName,0,sizeof(tmpName));
      cpdmCPConfigWebBackgroundImageNameGet(cpId,webId,tmpName);
      if (osapiStrncmp(imageName,tmpName,CP_FILE_NAME_MAX) == 0)
      {
        cpdmCPConfigWebBackgroundImageNameSet(cpId,webId,WEB_DEF_BACKGROUND_IMAGE_NAME);
      }
      memset(tmpName,0,sizeof(tmpName));
      cpdmCPConfigWebBrandingImageNameGet(cpId,webId,tmpName);
      if (osapiStrncmp(imageName,tmpName,CP_FILE_NAME_MAX) == 0)
      {
        cpdmCPConfigWebBrandingImageNameSet(cpId,webId,WEB_DEF_BRANDING_IMAGE_NAME);
      }
      memset(tmpName,0,sizeof(tmpName));
      cpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId,webId,tmpName);
      if (osapiStrncmp(imageName,tmpName,CP_FILE_NAME_MAX) == 0)
      {
        cpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId,webId,WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME);
      }
    }
  }
  return L7_SUCCESS;
}

