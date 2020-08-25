/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingipmapConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ipmapconfig-object.xml
*
* @create  24 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingipmapConfig_obj.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingipmapConfig_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface]: Get the first valid interface for participation
*              in the component 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIpMapValidIntfFirstGet(&nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbIpMapValidIntfNextGet (objInterfaceValue,
                                           &nextObjInterfaceValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingipmapConfig_intfgratArpAdminStatus
*
* @purpose Get 'intfgratArpAdminStatus'
*
* @description [intfgratArpAdminStatus]:  Get/Set the gratuitous ARP administrative
*              status for an interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_intfgratArpAdminStatus (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintfgratArpAdminStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbGratArpGet (L7_UNIT_CURRENT, keyInterfaceValue,
                              &objintfgratArpAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: intfgratArpAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objintfgratArpAdminStatusValue,
                           sizeof (objintfgratArpAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingipmapConfig_intfgratArpAdminStatus
*
* @purpose Set 'intfgratArpAdminStatus'
*
* @description [intfgratArpAdminStatus]:  Get/Set the gratuitous ARP administrative
*              status for an interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapConfig_intfgratArpAdminStatus (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintfgratArpAdminStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: intfgratArpAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objintfgratArpAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objintfgratArpAdminStatusValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbGratArpSet (L7_UNIT_CURRENT, keyInterfaceValue,
                              objintfgratArpAdminStatusValue);
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
* @function fpObjGet_routingipmapConfig_IntfBandwidth
*
* @purpose Get 'IntfBandwidth'
*
* @description [IntfBandwidth]: Get interface bandwidth without modifying
*              the SET value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IntfBandwidth (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfBandwidthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfBWGet (keyInterfaceValue,
                           &objIntfBandwidthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfBandwidth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfBandwidthValue,
                           sizeof (objIntfBandwidthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingipmapConfig_IntfEffectiveIpMtu
*
* @purpose Get 'IntfEffectiveIpMtu'
*
* @description [IntfEffectiveIpMtu]: Gets the IP MTU value being enforced
*              on a given interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IntfEffectiveIpMtu (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfEffectiveIpMtuValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIntfEffectiveIpMtuGet (keyInterfaceValue,
                                         &objIntfEffectiveIpMtuValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfEffectiveIpMtu */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfEffectiveIpMtuValue,
                           sizeof (objIntfEffectiveIpMtuValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingipmapConfig_IntfMaxIpMtu
*
* @purpose Get 'IntfMaxIpMtu'
*
* @description [IntfMaxIpMtu]: Get the maximum IP MTU that may be set on an
*              interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IntfMaxIpMtu (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfMaxIpMtuValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIntfMaxIpMtuGet (keyInterfaceValue,
                                   &objIntfMaxIpMtuValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfMaxIpMtu */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfMaxIpMtuValue,
                           sizeof (objIntfMaxIpMtuValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingipmapConfig_IntfICMPUnreachableMode
*
* @purpose Get 'IntfICMPUnreachableMode'
*
* @description [IntfICMPUnreachableMode]: Get/Set the administrative mode
*              of sending ICMP Unreachables 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IntfICMPUnreachableMode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfICMPUnreachableModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIpMapICMPUnreachablesModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       &objIntfICMPUnreachableModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfICMPUnreachableMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfICMPUnreachableModeValue,
                           sizeof (objIntfICMPUnreachableModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingipmapConfig_IntfICMPUnreachableMode
*
* @purpose Set 'IntfICMPUnreachableMode'
*
* @description [IntfICMPUnreachableMode]: Get/Set the administrative mode
*              of sending ICMP Unreachables 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapConfig_IntfICMPUnreachableMode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfICMPUnreachableModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfICMPUnreachableMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objIntfICMPUnreachableModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfICMPUnreachableModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpMapICMPUnreachablesModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       objIntfICMPUnreachableModeValue);
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
* @function fpObjGet_routingipmapConfig_IntfMcastFwdMode
*
* @purpose Get 'IntfMcastFwdMode'
*
* @description [IntfMcastFwdMode]: Determine whether an interface is configured
*              to forward multicast packets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IntfMcastFwdMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfMcastFwdModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpMcastsFwdModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                      &objIntfMcastFwdModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfMcastFwdMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfMcastFwdModeValue,
                           sizeof (objIntfMcastFwdModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingipmapConfig_clearAllIpAddresses
*
* @purpose Set 'clearAllIpAddresses'
*
* @description [clearAllIpAddresses]: Removes all IP addresses configured
*              on an interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapConfig_clearAllIpAddresses (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objclearAllIpAddressesValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: clearAllIpAddresses */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objclearAllIpAddressesValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objclearAllIpAddressesValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpRtrIntfIpAddressesRemove (L7_UNIT_CURRENT, keyInterfaceValue);
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
* @function fpObjGet_routingipmapConfig_IntfOperMode
*
* @purpose Get 'IntfOperMode'
*
* @description [IntfOperMode]:  Determine whether a given IP interface is
*              up for IPv4 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IntfOperMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfOperModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfOperModeGet (keyInterfaceValue,
                                        &objIntfOperModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfOperMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfOperModeValue,
                           sizeof (objIntfOperModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingipmapConfig_IsUnnumbered
*
* @purpose Get 'IsUnnumbered'
*
* @description [IsUnnumbered]: Returns L7_TRUE if a given interface is configured
*              to be unnumbered. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapConfig_IsUnnumbered (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsUnnumberedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objIsUnnumberedValue =  usmDbIntfIsUnnumbered (keyInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsUnnumbered */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsUnnumberedValue,
                           sizeof (objIsUnnumberedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
