/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1qForwardUnregistered.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1q-object.xml
*
* @create  9 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1qForwardUnregistered_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qForwardUnregistered_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex]: The VLAN-ID or other identifier refering to this
*              VLAN. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qForwardUnregistered_VlanIndex (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanIndexValue = 0;
    owa.l7rc = usmDbDot1qForwardUnregisteredEntryNextGet(L7_UNIT_CURRENT, &nextObjVlanIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    owa.l7rc =
      usmDbDot1qForwardUnregisteredEntryNextGet (L7_UNIT_CURRENT,
                                                 &objVlanIndexValue
                                                 );
    nextObjVlanIndexValue =  objVlanIndexValue;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue,
                           sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1qForwardUnregistered_Ports
*
* @purpose Get 'Ports'
*
* @description [Ports]: The complete set of ports in this VLAN to which multicast
*              group-addressed frames for which there is no more specific
*              forwarding information will be forwarded. This includes
*              ports for which this need has been determined dynamically
*              by GMRP, or configured statically by management. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qForwardUnregistered_Ports (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qForwardUnregisteredPortsGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                           objPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Ports */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortsValue,
                           strlen (objPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qForwardUnregistered_Ports
*
* @purpose Set 'Ports'
*
* @description [Ports]: The complete set of ports in this VLAN to which multicast
*              group-addressed frames for which there is no more specific
*              forwarding information will be forwarded. This includes
*              ports for which this need has been determined dynamically
*              by GMRP, or configured statically by management. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qForwardUnregistered_Ports (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Ports */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objPortsValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qForwardUnregisteredPortsSet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                           objPortsValue);
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
* @function fpObjGet_SwitchingDot1qForwardUnregistered_StaticPorts
*
* @purpose Get 'StaticPorts'
*
* @description [StaticPorts]: The set of ports configured by management, in
*              this VLAN, to which multicast group-addressed frames for
*              which there is no more specific forwarding information are
*              to be forwarded. Ports entered in this list will also appear
*              in the complete set shown by dot1qForwardUnregisteredPorts.
*              This value will be restored after the device is reset. A
*              port may not be added in this set if it is already a member
*              of the set of ports in dot1qForwardUnregisteredForbiddenPorts.
*              The default value is a string of zeros of appropriate
*              length, although this has no effect with the default value
*              of dot1qForwardAllStaticPorts. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qForwardUnregistered_StaticPorts (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStaticPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qForwardUnregisteredStaticPortsGet (L7_UNIT_CURRENT,
                                                 keyVlanIndexValue,
                                                 objStaticPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StaticPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStaticPortsValue,
                           strlen (objStaticPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qForwardUnregistered_StaticPorts
*
* @purpose Set 'StaticPorts'
*
* @description [StaticPorts]: The set of ports configured by management, in
*              this VLAN, to which multicast group-addressed frames for
*              which there is no more specific forwarding information are
*              to be forwarded. Ports entered in this list will also appear
*              in the complete set shown by dot1qForwardUnregisteredPorts.
*              This value will be restored after the device is reset. A
*              port may not be added in this set if it is already a member
*              of the set of ports in dot1qForwardUnregisteredForbiddenPorts.
*              The default value is a string of zeros of appropriate
*              length, although this has no effect with the default value
*              of dot1qForwardAllStaticPorts. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qForwardUnregistered_StaticPorts (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStaticPortsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objStaticPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objStaticPortsValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qForwardUnregisteredStaticPortsSet (L7_UNIT_CURRENT,
                                                 keyVlanIndexValue,
                                                 objStaticPortsValue);
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
* @function fpObjGet_SwitchingDot1qForwardUnregistered_ForbiddenPorts
*
* @purpose Get 'ForbiddenPorts'
*
* @description [ForbiddenPorts]: The set of ports configured by management
*              in this VLAN for which the Service Requirement attribute Forward
*              Unregistered Multicast Groups may not be dynamically
*              registered by GMRP. This value will be restored after the
*              device is reset. A port may not be added in this set if it
*              is already a member of the set of ports in dot1qForwardUnregisteredStaticPorts.
*              The default value is a string of zeros
*              of appropriate length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qForwardUnregistered_ForbiddenPorts (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objForbiddenPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDdot1qForwardUnregisteredForbiddenPortsGet (L7_UNIT_CURRENT,
                                                     keyVlanIndexValue,
                                                     objForbiddenPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ForbiddenPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objForbiddenPortsValue,
                           strlen (objForbiddenPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qForwardUnregistered_ForbiddenPorts
*
* @purpose Set 'ForbiddenPorts'
*
* @description [ForbiddenPorts]: The set of ports configured by management
*              in this VLAN for which the Service Requirement attribute Forward
*              Unregistered Multicast Groups may not be dynamically
*              registered by GMRP. This value will be restored after the
*              device is reset. A port may not be added in this set if it
*              is already a member of the set of ports in dot1qForwardUnregisteredStaticPorts.
*              The default value is a string of zeros
*              of appropriate length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qForwardUnregistered_ForbiddenPorts (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objForbiddenPortsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ForbiddenPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objForbiddenPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objForbiddenPortsValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qForwardUnregistered_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDdot1qForwardUnregisteredForbiddenPortsSet (L7_UNIT_CURRENT,
                                                     keyVlanIndexValue,
                                                     objForbiddenPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
