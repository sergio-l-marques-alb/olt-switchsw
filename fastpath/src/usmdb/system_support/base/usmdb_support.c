/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename  usmdb_support.c
*
* @purpose   USMDB routines for System Support (Debug) Manager
*
* @component system
*
* @comments  none
*
* @create    09/12/2006
*
* @author    W. Jacobs
*
* @end
*
**********************************************************************/


#include "support_api.h"
#include "osapi.h"
#include "cli_web_mgr_api.h"
#include "usmdb_support.h"

/*********************************************************************
* @purpose  Determine if any debug routine is registered for 
*           the category and/or component
*
* Index values of the current entry:
* @param    category         @b; support routine category
* @param    componentId      @b; component ID
*
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_BOOL usmDbSupportDebugCategoryIsPresent(SUPPORT_CATEGORIES_t category,
                                      L7_COMPONENT_IDS_t componentId)
{
  return supportDebugCategoryIsPresent(category,componentId);
}

/*********************************************************************
* @purpose  Determine if any registration exists for any snapshot. 
*
* @param    void
*
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*                                 
* @end
*********************************************************************/
L7_BOOL usmDbSupportDebugCategoryRegistrationExists(void)
{
  return supportDebugCategoryRegistrationExists();
}

/*********************************************************************
* @purpose  Invoke the debug dump routines registered in this category
*
* Index values of the current entry:
* @param    category         @b{(input/output)} support routine category
* @param    componentId      @b{(input/output)} component ID
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_RC_t usmDbSupportDebugCategoryDump(SUPPORT_CATEGORIES_t category)
{
  return supportDebugCategoryDump(category);
}

/*********************************************************************
* @purpose  Call all registered non-volitale debug save routines
*
* @param    void      
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_RC_t usmDbSupportDebugSave(void)
{
  return supportDebugSave();
}

/*********************************************************************
* @purpose  Call all registered debug clear routines
*
* @param    void      
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void usmDbSupportDebugClear(void)
{
  return supportDebugClear();
}
#ifdef LVL7_DEBUG_BREAKIN
/***********************************************************************
*@purpose  Enable/Disable console break-in
*
* @param    enable_flag - L7_TRUE.
*                         L7_FALSE.
* @param    session     - CLI login session

* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t usmDbConsoleBreakinModeSet(L7_BOOL enable_flag, L7_int32 session)
{
  return cliWebConsoleBreakinModeSet(enable_flag);
}

/*********************************************************************
* @purpose  Gets the console break-in  Mode setting for selected session.
*
* @param    session     - CLI login session
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @returns  L7_TRUE -  display of trace output is enabled on the login session.
*           L7_FALSE - display of trace output is disabled on the login session.
*
* @notes
*
*
* @end
*********************************************************************/
L7_BOOL usmDbConsoleBreakinModeGet (L7_int32 session)
{
  return  cliWebConsoleBreakinModeGet();
}

/*********************************************************************
* @purpose  Gets the console breakin string
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  usmDbConsoleBreakinstringGet(L7_uint32 index, L7_char8 *password)
{
  return cliWebConsoleBreakinstringGet(password);
}

/*********************************************************************
* @purpose  Sets  console breakin string.
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS
*           L_FAILURE if password is too long or index is too large
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbConsoleBreakinstringSet(L7_uint32 index, L7_char8 *password)
{
  return cliWebConsoleBreakinstringSet(password);
}
#endif
