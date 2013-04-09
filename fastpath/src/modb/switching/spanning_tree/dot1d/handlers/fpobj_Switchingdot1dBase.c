/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1dBase.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Miscellaneous-object.xml
*
* @create  6 February 2008
*
* @author  Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot1dBase_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_garp.h"
#include "usmdb_mib_bridge_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_TrafficClassesEnabled
*
* @purpose Get 'TrafficClassesEnabled'
*
* @description [TrafficClassesEnabled]: The value true(1) indicates that Traffic
*              Classes are enabled on this bridge. When false(2), the
*              bridge operates with a single priority level for all traffic
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_TrafficClassesEnabled (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrafficClassesEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dTrafficClassesEnabledGet (L7_UNIT_CURRENT,
                                        &objTrafficClassesEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrafficClassesEnabledValue,
                     sizeof (objTrafficClassesEnabledValue));

  /* return the object value: TrafficClassesEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrafficClassesEnabledValue,
                           sizeof (objTrafficClassesEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1dBase_TrafficClassesEnabled
*
* @purpose Set 'TrafficClassesEnabled'
*
* @description [TrafficClassesEnabled]: The value true(1) indicates that Traffic
*              Classes are enabled on this bridge. When false(2), the
*              bridge operates with a single priority level for all traffic
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dBase_TrafficClassesEnabled (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrafficClassesEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrafficClassesEnabled */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTrafficClassesEnabledValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrafficClassesEnabledValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1dTrafficClassesEnabledSet (L7_UNIT_CURRENT,
                                        objTrafficClassesEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



#ifdef L7_FEAT_GMRP
/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_GmrpStatus
*
* @purpose Get 'GmrpStatus'
*
* @description [GmrpStatus]: The administrative status requested by management
*              for GMRP. The value enabled(1) indicates that GMRP should
*              be enabled on this device, in all VLANs, on all ports for
*              which it has not been specifically disabled. When disabled(2),
*              GMRP is disabled, in al 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_GmrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGmrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGarpGmrpEnableGet (L7_UNIT_CURRENT, &objGmrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGmrpStatusValue, sizeof (objGmrpStatusValue));

  /* return the object value: GmrpStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGmrpStatusValue,
                           sizeof (objGmrpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1dBase_GmrpStatus
*
* @purpose Set 'GmrpStatus'
*
* @description [GmrpStatus]: The administrative status requested by management
*              for GMRP. The value enabled(1) indicates that GMRP should
*              be enabled on this device, in all VLANs, on all ports for
*              which it has not been specifically disabled. When disabled(2),
*              GMRP is disabled, in al 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dBase_GmrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGmrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GmrpStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGmrpStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGmrpStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbGarpGmrpEnableSet (L7_UNIT_CURRENT, objGmrpStatusValue);
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
* @function fpObjGet_Switchingdot1dBase_GvrpStatus
*
* @purpose Get 'GvrpStatus'
*
* @description [GvrpStatus] To do
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_GvrpStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGvrpStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGarpGvrpEnableGet(L7_UNIT_CURRENT, &objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGvrpStatusValue, sizeof (objGvrpStatusValue));

  /* return the object value: GvrpStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpStatusValue, sizeof (objGvrpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_Switchingdot1dBase_GvrpStatus
*
* @purpose Set 'GvrpStatus'
*
* @description [GvrpStatus] To do
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dBase_GvrpStatus (void *wap, void *bufp)
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
  owa.l7rc = usmDbGarpGvrpEnableSet(L7_UNIT_CURRENT, objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif
/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_NumPorts
*
* @purpose Get 'NumPorts'
*
* @description [NumPorts]: The number of ports controlled by this bridging
*              entity. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_NumPorts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1dBaseNumPortsGet (L7_UNIT_CURRENT, &objNumPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNumPortsValue, sizeof (objNumPortsValue));

  /* return the object value: NumPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumPortsValue,
                           sizeof (objNumPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_dot1dBaseType
*
* @purpose Get 'dot1dBaseType'
*
* @description [dot1dBaseType]: Indicates what type of bridging this bridge
*              can perform. If a bridge is actually performing a certain
*              type of bridging this will be indicated by entries in the
*              port table for the given type 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_dot1dBaseType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1dBaseTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1dBaseTypeGet (L7_UNIT_CURRENT, &objdot1dBaseTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1dBaseTypeValue,
                     sizeof (objdot1dBaseTypeValue));

  /* return the object value: dot1dBaseType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1dBaseTypeValue,
                           sizeof (objdot1dBaseTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_BridgeAddress
*
* @purpose Get 'BridgeAddress'
*
* @description [BridgeAddress]: The MAC address used by this bridge when it
*              must referred to in a unique fashion. It is recommended that
*              this be the numerically smallest MAC address of all ports
*              that belong to this bridge. However it is only required
*              to be unique. When concatenated w 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_BridgeAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objBridgeAddressValue;
  xLibU32_t length;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dBaseBridgeAddressGet (L7_UNIT_CURRENT, objBridgeAddressValue, &length);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objBridgeAddressValue,
                     sizeof (objBridgeAddressValue));

  /* return the object value: BridgeAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objBridgeAddressValue,
                           sizeof (objBridgeAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_Dot1dTpAgingTime
*
* @purpose Get 'Dot1dTpAgingTime'
 *@description  [Dot1dTpAgingTime] Aging time
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_Dot1dTpAgingTime (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpAgingTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1dTpAgingTimeGet(L7_UNIT_CURRENT, &objDot1dTpAgingTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1dTpAgingTimeValue, sizeof (objDot1dTpAgingTimeValue));

  /* return the object value: Dot1dTpAgingTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTpAgingTimeValue,
                           sizeof (objDot1dTpAgingTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_Switchingdot1dBase_Dot1dTpAgingTime
*
* @purpose Set 'Dot1dTpAgingTime'
 *@description  [Dot1dTpAgingTime] Aging time
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dBase_Dot1dTpAgingTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpAgingTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dTpAgingTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1dTpAgingTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dTpAgingTimeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDot1dTpAgingTimeSet(L7_UNIT_CURRENT, objDot1dTpAgingTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dBase_Dot1dDeviceCapabilities
*
* @purpose Get 'Dot1dDeviceCapabilities'
 *@description  [Dot1dDeviceCapabilities] Get device capabilities
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dBase_Dot1dDeviceCapabilities (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1dDeviceCapabilitiesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1dDeviceCapabilitiesGet(L7_UNIT_CURRENT, objDot1dDeviceCapabilitiesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDot1dDeviceCapabilitiesValue,
                     strlen (objDot1dDeviceCapabilitiesValue));

  /* return the object value: Dot1dDeviceCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1dDeviceCapabilitiesValue,
                           strlen (objDot1dDeviceCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_Switchingdot1dBase_Dot1dDeviceCapabilities
*
* @purpose Set 'Dot1dDeviceCapabilities'
 *@description  [Dot1dDeviceCapabilities] Get device capabilities
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dBase_Dot1dDeviceCapabilities (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1dDeviceCapabilitiesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dDeviceCapabilities */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDot1dDeviceCapabilitiesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDot1dDeviceCapabilitiesValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDot1dDeviceCapabilitiesSet(L7_UNIT_CURRENT, objDot1dDeviceCapabilitiesValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


