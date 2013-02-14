/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingRip2Global.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Rip2Config-object.xml
*
* @create  22 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingRip2Global_obj.h"
#include "usmdb_mib_ripv2_api.h"

/*******************************************************************************
* @function fpObjGet_routingRip2Global_RouteChanges
*
* @purpose Get 'RouteChanges'
*
* @description [RouteChanges]: The number of route changes made to the IP
*              Route Database by RIP. This does not include the refresh of
*              a route's age. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Global_RouteChanges (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRouteChangesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2GlobalRouteChangesGet (L7_UNIT_CURRENT, &objRouteChangesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRouteChangesValue,
                     sizeof (objRouteChangesValue));

  /* return the object value: RouteChanges */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRouteChangesValue,
                           sizeof (objRouteChangesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2Global_Queries
*
* @purpose Get 'Queries'
*
* @description [Queries]: The number of responses sent to RIP queries from
*              other systems. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Global_Queries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQueriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRip2GlobalQueriesGet (L7_UNIT_CURRENT, &objQueriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQueriesValue, sizeof (objQueriesValue));

  /* return the object value: Queries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQueriesValue,
                           sizeof (objQueriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
