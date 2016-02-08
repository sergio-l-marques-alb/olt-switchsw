/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 **********************************************************************
 *
 * @filename xlib_api.h
 *
 * @purpose  general xlib apis
 *
 * @component XLIB
 *
 * @comments
 *
 * @create 06/30/2008
 *
 * @author Madap Vamshikrishna
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include "xlib_api.h"
#include "xlib.h"



xLibAccessLevelCheck_t levelCheck;

/*********************************************************************
*
* @purpose Registration function for access level check.
*
* @param XLIB_LEVEL_FUNC_t function pointer of type XLIB_LEVEL_FUNC_t
*
* @returns XLIB_TRUE in case of success and XLIB_FALSE in case of failure
*
* @notes
*
* @end
*
*********************************************************************/

xLibBool_t xLibAccessLevelFuncRegister(XLIB_LEVEL_FUNC_t func)
{
 levelCheck.accessLevelCheck = func;
 return XLIB_TRUE;
}

/*********************************************************************
*
* @purpose To check the node  access level.
*
* @param xLibU16_t user_level:User level of the logged in user
* @param xLibU16_t level: level of the node
* @param  xLibS8_t condition: condition to be checked for(equal,lesser or greater)
*
* @returns XLIB_TRUE in case of success and XLIB_FALSE in case of failure
*
* @notes
*
* @end
*
*********************************************************************/

xLibBool_t xLibAccessLevelCheck(xLibU16_t user_level,xLibU16_t level,xLibS8_t	 condition)
{
  if(levelCheck.accessLevelCheck == NULL)
  {
    /* we there is no registration function means, no access restrictions are needed */
    return XLIB_TRUE; 
  }
  return (*levelCheck.accessLevelCheck)(user_level,level,condition);
}


