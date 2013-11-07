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

/*********************************************************************
*
* @purpose To set the object value in context into filters for passed oid's
*  and redirect the page specified by paramater url
*
* @param cntx: emweb context
* @param xLibU16_t count: Number of objects whose filters have to be set in new context
* @param xLibU16_t url: url of page to be redirected
* @param  xLibS8_t oid; oid's of an object
* takes variable number of arguments
* information passed shall be in oid, type, value - pairs as many number of object values that will be pushed to context
*
* @Returns EWS_STATUS_OK on success, else error code


* @notes
*
* @end
*
*********************************************************************/

char *xLibWebContextRedirect(void *cntx, xLibU32_t count, char *url, xLibU32_t oid, ...)
{

 xLibWa_t *wap = (xLibWa_t *)xLibWeb2Init(cntx);
 L7_uint32 tempOid,i;
 xLibU16_t type;
 L7_uchar8 *value;

 va_list ap;

 va_start (ap, oid);

 tempOid = oid; /* for the first object in the list */
 for(i=0; i<count;tempOid = va_arg (ap, int),i++)
 {
    type = (short)va_arg(ap, int);
    value = (char *)va_arg(ap,char **);
    xLibWeb2FilterSet(wap, tempOid, type,value);
 }
 va_end(ap);
 return emwebContextSendReply (wap->ui.web.webCntxt, url);

}




