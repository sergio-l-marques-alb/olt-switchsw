/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingVlanBaseConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to VLan-object.xml
*
* @create  13 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingVlanBaseConfig_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_garp.h"

/*******************************************************************************
* @function fpObjGet_SwitchingVlanBaseConfig_NumVlans
*
* @purpose Get 'NumVlans'
*
* @description [NumVlans]: The current number of IEEE 802.1Q VLANs that are
*              configured in this device. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanBaseConfig_NumVlans (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumVlansValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qNumVlansGet (L7_UNIT_CURRENT, &objNumVlansValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNumVlansValue, sizeof (objNumVlansValue));

  /* return the object value: NumVlans */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumVlansValue,
                           sizeof (objNumVlansValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanBaseConfig_MaxSupportedVlans
*
* @purpose Get 'MaxSupportedVlans'
*
* @description [MaxSupportedVlans]: The maximum number of IEEE 802.1Q VLANs
*              that this device supports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanBaseConfig_MaxSupportedVlans (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxSupportedVlansValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qMaxSupportedVlansGet (L7_UNIT_CURRENT,
                                    &objMaxSupportedVlansValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxSupportedVlansValue,
                     sizeof (objMaxSupportedVlansValue));

  /* return the object value: MaxSupportedVlans */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxSupportedVlansValue,
                           sizeof (objMaxSupportedVlansValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanBaseConfig_MaxVlanId
*
* @purpose Get 'MaxVlanId'
*
* @description [MaxVlanId]: The maximum IEEE 802.1Q VLAN ID that this device
*              supports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanBaseConfig_MaxVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qMaxVlanIdGet (L7_UNIT_CURRENT, &objMaxVlanIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxVlanIdValue, sizeof (objMaxVlanIdValue));

  /* return the object value: MaxVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxVlanIdValue,
                           sizeof (objMaxVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#ifdef L7_FEAT_GMRP
/*******************************************************************************
* @function fpObjGet_SwitchingVlanBaseConfig_GvrpStatus
*
* @purpose Get 'GvrpStatus'
*
* @description [GvrpStatus]: The administrative status requested by management
*              for GVRP. The value enabled(1) indicates that GVRP should
*              be enabled on this device, on all ports for which it has
*              not been specifically disabled. When disabled(2), GVRP is
*              disabled on all ports and all 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanBaseConfig_GvrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGvrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qGvrpStatusGet (L7_UNIT_CURRENT, &objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGvrpStatusValue, sizeof (objGvrpStatusValue));

  /* return the object value: GvrpStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpStatusValue,
                           sizeof (objGvrpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanBaseConfig_GvrpStatus
*
* @purpose Set 'GvrpStatus'
*
* @description [GvrpStatus]: The administrative status requested by management
*              for GVRP. The value enabled(1) indicates that GVRP should
*              be enabled on this device, on all ports for which it has
*              not been specifically disabled. When disabled(2), GVRP is
*              disabled on all ports and all 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanBaseConfig_GvrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGvrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GvrpStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGvrpStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1qGvrpStatusSet (L7_UNIT_CURRENT, objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif
/*******************************************************************************
* @function fpObjGet_SwitchingVlanBaseConfig_VersionNumber
*
* @purpose Get 'VersionNumber'
*
* @description [VersionNumber]: The version number of IEEE 802.1Q that this
*              device supports 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanBaseConfig_VersionNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVersionNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qVlanVersionNumberGet (L7_UNIT_CURRENT, &objVersionNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVersionNumberValue,
                     sizeof (objVersionNumberValue));

  /* return the object value: VersionNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVersionNumberValue,
                           sizeof (objVersionNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanBaseConfig_NumDeletes
*
* @purpose Get 'NumDeletes'
*
* @description [NumDeletes]: The number of times a VLAN entry has been deleted
*              from the dot1qVlanCurrentTable (for any reason). If an
*              entry is deleted, then inserted, and then deleted, this counter
*              will be incremented by 2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanBaseConfig_NumDeletes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumDeletesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qVlanNumDeletesGet (L7_UNIT_CURRENT, &objNumDeletesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNumDeletesValue, sizeof (objNumDeletesValue));

  /* return the object value: NumDeletes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumDeletesValue,
                           sizeof (objNumDeletesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
