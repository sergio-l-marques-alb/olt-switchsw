/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1qPort.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1qConfig-object.xml
*
* @create  9 March 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1qPort_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_garp.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qPort_BasePort
*
* @purpose Get 'BasePort'
*
* @description [BasePort]: The port number of the port for which this entry
*              contains bridge management information. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qPort_BasePort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBasePortValue;
  xLibU32_t nextObjBasePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: BasePort */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qPort_BasePort,
                          (xLibU8_t *) & objBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjBasePortValue = 0;
    owa.l7rc = usmDbGetNextVisibleIntIfNumber(nextObjBasePortValue, &nextObjBasePortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objBasePortValue, owa.len);
    owa.l7rc = usmDbGetNextVisibleIntIfNumber(objBasePortValue, &nextObjBasePortValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjBasePortValue, owa.len);

  /* return the object value: BasePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjBasePortValue,
                           sizeof (objBasePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1qPort_AcceptableFrameType
*
* @purpose Get 'AcceptableFrameType'
*
* @description [AcceptableFrameType]: Get the acceptable frame type for a
*              specific port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qPort_AcceptableFrameType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAcceptableFrameTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: BasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qPort_BasePort,
                          (xLibU8_t *) & keyBasePortValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qPortAcceptableFrameTypes (L7_UNIT_CURRENT, keyBasePortValue,
                                        &objAcceptableFrameTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AcceptableFrameType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAcceptableFrameTypeValue,
                           sizeof (objAcceptableFrameTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1qPort_GvrpStatus
*
* @purpose Get 'GvrpStatus'
*
* @description [GvrpStatus]: The state of GVRP operation on this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qPort_GvrpStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGvrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: BasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qPort_BasePort,
                          (xLibU8_t *) & keyBasePortValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qPortGvrpStatus (L7_UNIT_CURRENT, keyBasePortValue,
                                       &objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GvrpStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpStatusValue,
                           sizeof (objGvrpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1qPort_IngressFiltering
*
* @purpose Get 'IngressFiltering'
*
* @description [IngressFiltering]: Get VLAN ingress filtering status per port
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qPort_IngressFiltering (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIngressFilteringValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: BasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qPort_BasePort,
                          (xLibU8_t *) & keyBasePortValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qPortIngressFiltering (L7_UNIT_CURRENT, keyBasePortValue,
                                             &objIngressFilteringValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IngressFiltering */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIngressFilteringValue,
                           sizeof (objIngressFilteringValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1qPort_Pvid
*
* @purpose Get 'Pvid'
*
* @description [Pvid]: get the PVID, the VLAN ID assigned to untagged frames
*              or Priority-Tagged frames received on this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qPort_Pvid (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPvidValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: BasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qPort_BasePort,
                          (xLibU8_t *) & keyBasePortValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qPvid (L7_UNIT_CURRENT, keyBasePortValue, &objPvidValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Pvid */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPvidValue,
                           sizeof (objPvidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
