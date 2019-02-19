/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingIpRoutingGlobal.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ipconfig-object.xml
*
* @create  6 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingIpRoutingGlobal_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingIpRoutingGlobal_routingMode
*
* @purpose Get 'routingMode'
*
* @description [routingMode]: Administratively enables/disables routing on
*              the switch. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRoutingGlobal_routingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objroutingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrAdminModeGet (L7_UNIT_CURRENT, &objroutingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objroutingModeValue, sizeof (objroutingModeValue));

  /* return the object value: routingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objroutingModeValue,
                           sizeof (objroutingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRoutingGlobal_routingMode
*
* @purpose Set 'routingMode'
*
* @description [routingMode]: Administratively enables/disables routing on
*              the switch. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRoutingGlobal_routingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objroutingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: routingMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objroutingModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objroutingModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRtrAdminModeSet (L7_UNIT_CURRENT, objroutingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingIpRoutingGlobal_MaxEqualCostEntries
*
* @purpose Get 'MaxEqualCostEntries'
 *@description  [MaxEqualCostEntries] Get the routing max equal cost entries.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRoutingGlobal_MaxEqualCostEntries (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxEqualCostEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRtrRouteMaxEqualCostEntriesGet(L7_UNIT_CURRENT, &objMaxEqualCostEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMaxEqualCostEntriesValue, sizeof (objMaxEqualCostEntriesValue));

  /* return the object value: MaxEqualCostEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxEqualCostEntriesValue,
                           sizeof (objMaxEqualCostEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpRoutingGlobal_MaxRouteEntries
*
* @purpose Get 'MaxRouteEntries'
 *@description  [MaxRouteEntries] Get the routing max equal cost entries.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRoutingGlobal_MaxRouteEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxRouteEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRtrRouteMaxRouteEntriesGet(L7_UNIT_CURRENT, &objMaxRouteEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMaxRouteEntriesValue, sizeof (objMaxRouteEntriesValue));

  /* return the object value: MaxRouteEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxRouteEntriesValue,
                           sizeof (objMaxRouteEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


