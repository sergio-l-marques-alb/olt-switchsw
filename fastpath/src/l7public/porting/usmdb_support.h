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

#include "compdefs.h"
#include "support_api.h"

/*********************************************************************
* @purpose  Determine if any debug routine is registered for 
*           the category and/or component
*
* Index values of the current entry:
* @param    category         @b; support routine category
* @param    componentId      @b; component ID
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_BOOL usmDbSupportDebugCategoryIsPresent(SUPPORT_CATEGORIES_t category,
                                      L7_COMPONENT_IDS_t componentId);

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
L7_BOOL usmDbSupportDebugCategoryRegistrationExists(void);

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
L7_RC_t usmDbSupportDebugCategoryDump(SUPPORT_CATEGORIES_t category);

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
L7_RC_t usmDbSupportDebugSave(void);

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
void usmDbSupportDebugClear(void);

#ifdef LVL7_DEBUG_BREAKIN
/***********************************************************************
*@purpose  Enable/Disable  console break-in
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
**************************************************************************/
L7_RC_t usmDbConsoleBreakinModeSet(L7_BOOL enable_flag, L7_int32 session);

/***************************************************************************
* @purpose  Gets the console break-in Mode setting for selected session.
*
* @param    session     - CLI login session
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @returns  L7_TRUE -  Console Debugging Mode is Enabled.
*           L7_FALSE - Console Debugging Mode is Disabled.
*
* @notes
*
*
* @end
*********************************************************************/
L7_BOOL usmDbConsoleBreakinModeGet (L7_int32 session);

/************************************************************************
* @purpose  Gets the breakin string
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS If successful in getting the password.
* @returns  L7_FAILURE If the Argument Passed to this routine is a Null Pointer.
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t  usmDbConsoleBreakinstringGet(L7_uint32 index, L7_char8 *password);

/************************************************************************
* @purpose  Sets  console breakin string.
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS If successful in getting the Password.
*           L7_FAILURE If Argument Passsed to this function is a Null Pointer.
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t  usmDbConsoleBreakinstringSet(L7_uint32 index, L7_char8 *password);
#endif
