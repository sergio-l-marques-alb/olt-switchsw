
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseGVRPErrorStatistics.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  19 October 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseGVRPErrorStatistics_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_baseGVRPErrorStatistics_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex]  A unique value, greater than zero, for each
* interface. It is recommended that values are assigned contiguously
* starting from 1. The value for each interface sub-layer must remain
* constant at least from one re-initial   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPErrorStatistics_ifIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof (objifIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseGVRPErrorStatistics_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT,  USM_PHYSICAL_INTF | USM_LAG_INTF, 
                                         0, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 
                                        0, objifIndexValue, &nextObjifIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseGVRPErrorStatistics_GVRPErrorType
*
* @purpose Get 'GVRPErrorType'
 *@description  [GVRPErrorType] This object used to identify a GVRP error type .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPErrorStatistics_GVRPErrorType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGVRPErrorTypeValue;
  xLibU32_t nextObjGVRPErrorTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GVRPErrorType */
  owa.len = sizeof (objGVRPErrorTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseGVRPErrorStatistics_GVRPErrorType,
                          (xLibU8_t *) & objGVRPErrorTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjGVRPErrorTypeValue = L7_XUI_GARP_INVALID_PROTOCOL_ID;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGVRPErrorTypeValue, owa.len);
    if ((objGVRPErrorTypeValue < L7_XUI_GARP_INVALID_ATTR_EVENT) && (objGVRPErrorTypeValue > 0))
    {
      nextObjGVRPErrorTypeValue = objGVRPErrorTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
    else 
    {
      owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjGVRPErrorTypeValue, owa.len);

  /* return the object value: GVRPErrorType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjGVRPErrorTypeValue,
                           sizeof (nextObjGVRPErrorTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseGVRPErrorStatistics_GVRPErrorPktsRx
*
* @purpose Get 'GVRPErrorPktsRx'
 *@description  [GVRPErrorPktsRx] This object indicates the number of packets
* received with the given error and on a given interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPErrorStatistics_GVRPErrorPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGVRPErrorPktsRxValue;

  xLibU32_t keyifIndexValue;
  xLibU32_t keyGVRPErrorTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof (keyifIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseGVRPErrorStatistics_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, owa.len);

  /* retrieve key: GVRPErrorType */
  owa.len = sizeof (keyGVRPErrorTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseGVRPErrorStatistics_GVRPErrorType,
                          (xLibU8_t *) & keyGVRPErrorTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGVRPErrorTypeValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpErrorStatisticGet(keyifIndexValue,
                              keyGVRPErrorTypeValue, &objGVRPErrorPktsRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGVRPErrorPktsRxValue, sizeof (objGVRPErrorPktsRxValue));

  /* return the object value: GVRPErrorPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGVRPErrorPktsRxValue,
                           sizeof (objGVRPErrorPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
