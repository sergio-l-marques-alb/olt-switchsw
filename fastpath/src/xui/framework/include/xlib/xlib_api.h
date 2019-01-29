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

#include "xlib.h"
#include "_xeobj_xtype.h"
#include "_xe_obj.h"
#include "xlib_private.h"

typedef struct 
{
 xLibBool_t (*accessLevelCheck)(xLibU16_t user_level,xLibU16_t level,xLibS8_t condition);
}xLibAccessLevelCheck_t;

typedef xLibBool_t (*XLIB_LEVEL_FUNC_t)(xLibU16_t user_level,xLibU16_t level,xLibS8_t condition);

extern void *xLibWebLiveObjGet (void *wap, xLibId_t oid, xLibU16_t type);
extern void *xLibWeb2Init (void *webSrvCntx);
extern void xLibWeb2FilterSet (void *cntx, xLibId_t oid, xLibU16_t type, char *value);

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

xLibBool_t xLibAccessLevelFuncRegister(XLIB_LEVEL_FUNC_t func);

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

xLibBool_t xLibAccessLevelCheck(xLibU16_t user_level,xLibU16_t level,xLibS8_t condition);
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

char *xLibWebContextRedirect(void *cntx, xLibU32_t count, char *url, xLibU32_t oid, ...);
