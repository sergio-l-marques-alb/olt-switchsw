/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingipmapGlobalConfig.c
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
#include "_xe_routingipmapGlobalConfig_obj.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_IpSpoofing
*
* @purpose Get 'IpSpoofing'
*
* @description [IpSpoofing]: Sets/Gets Ip Spoofing mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_IpSpoofing (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpSpoofingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpSpoofingGet (L7_UNIT_CURRENT, &objIpSpoofingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpSpoofingValue, sizeof (objIpSpoofingValue));

  /* return the object value: IpSpoofing */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpSpoofingValue,
                           sizeof (objIpSpoofingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_IpSpoofing
*
* @purpose Set 'IpSpoofing'
*
* @description [IpSpoofing]: Sets/Gets Ip Spoofing mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_IpSpoofing (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpSpoofingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpSpoofing */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIpSpoofingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpSpoofingValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpSpoofingSet (L7_UNIT_CURRENT, objIpSpoofingValue);
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
* @function fpObjGet_routingipmapGlobalConfig_ipSourceAddrCheck
*
* @purpose Get 'ipSourceAddrCheck'
*
* @description [ipSourceAddrCheck]: Sets/Gets the Ip Source Checking mode
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipSourceAddrCheck (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipSourceAddrCheckValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpSourceCheckingGet (L7_UNIT_CURRENT, &objipSourceAddrCheckValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipSourceAddrCheckValue,
                     sizeof (objipSourceAddrCheckValue));

  /* return the object value: ipSourceAddrCheck */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipSourceAddrCheckValue,
                           sizeof (objipSourceAddrCheckValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipSourceAddrCheck
*
* @purpose Set 'ipSourceAddrCheck'
*
* @description [ipSourceAddrCheck]: Sets/Gets the Ip Source Checking mode
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipSourceAddrCheck (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipSourceAddrCheckValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipSourceAddrCheck */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipSourceAddrCheckValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipSourceAddrCheckValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpSourceCheckingSet (L7_UNIT_CURRENT, objipSourceAddrCheckValue);
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
* @function fpObjGet_routingipmapGlobalConfig_protocolType
*
* @purpose Get 'protocolType'
*
* @description [protocolType]: Router protocol type for Get/Set the router
*              preference 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_protocolType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objprotocolTypeValue;
  xLibU32_t nextObjprotocolTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: protocolType */
  owa.rc = xLibFilterGet (wap, XOBJ_routingipmapGlobalConfig_protocolType,
                          (xLibU8_t *) & objprotocolTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    /* We dont have getfirst and get next functions.So we are assigning the values directly */
    nextObjprotocolTypeValue = ROUTE_PREF_LOCAL;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolTypeValue, owa.len);
    nextObjprotocolTypeValue = objprotocolTypeValue + 1;
    /* We dont have getfirst and get next functions.So we are assigning the values directly */
    if(nextObjprotocolTypeValue < ROUTE_PREF_LAST)
    {
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
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjprotocolTypeValue, owa.len);

  /* return the object value: protocolType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjprotocolTypeValue,
                           sizeof (objprotocolTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreference
*
* @purpose Get 'ipRoutePreference'
*
* @description [ipRoutePreference]: All routes from the dynamic routing protocols
*              use the default preference for that protocol. The user
*              may configure the preference of individual static and default
*              routes. A change to the default preference for static
*              or default routes will not change the preference of existing
*              static or default routes. Static and default routes configured
*              after the change will use the new default preference.
*              The preference value shall be the same for all the OSPF
*              external route types like type1/type2/nssa1/nssa2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreference (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprotocolTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: protocolType */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapGlobalConfig_protocolType,
                          (xLibU8_t *) & keyprotocolTypeValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprotocolTypeValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet (L7_UNIT_CURRENT, keyprotocolTypeValue,
                                         &objipRoutePreferenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRoutePreference */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceValue,
                           sizeof (objipRoutePreferenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreference
*
* @purpose Set 'ipRoutePreference'
*
* @description [ipRoutePreference]: All routes from the dynamic routing protocols
*              use the default preference for that protocol. The user
*              may configure the preference of individual static and default
*              routes. A change to the default preference for static
*              or default routes will not change the preference of existing
*              static or default routes. Static and default routes configured
*              after the change will use the new default preference.
*              The preference value shall be the same for all the OSPF
*              external route types like type1/type2/nssa1/nssa2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreference (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprotocolTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreference */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRoutePreferenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceValue, owa.len);

  /* retrieve key: protocolType */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingipmapGlobalConfig_protocolType,
                          (xLibU8_t *) & keyprotocolTypeValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprotocolTypeValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet (L7_UNIT_CURRENT, keyprotocolTypeValue,
                                         objipRoutePreferenceValue);
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
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceLocal
*
* @purpose Get 'ipRoutePreferenceLocal'
 *@description  [ipRoutePreferenceLocal] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default
* routes. A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceLocal (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceLocalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_LOCAL, &objipRoutePreferenceLocalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceLocalValue,
                     sizeof (objipRoutePreferenceLocalValue));

  /* return the object value: ipRoutePreferenceLocal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceLocalValue,
                           sizeof (objipRoutePreferenceLocalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceLocal
*
* @purpose Set 'ipRoutePreferenceLocal'
 *@description  [ipRoutePreferenceLocal] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default
* routes. A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceLocal (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceLocalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceLocal */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceLocalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceLocalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_LOCAL, objipRoutePreferenceLocalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceStatic
*
* @purpose Get 'ipRoutePreferenceStatic'
 *@description  [ipRoutePreferenceStatic] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user
* may configure the preference of individual static and default
* routes. A change to the default preference for static or default
* routes will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceStatic (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceStaticValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_STATIC, &objipRoutePreferenceStaticValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceStaticValue,
                     sizeof (objipRoutePreferenceStaticValue));

  /* return the object value: ipRoutePreferenceStatic */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceStaticValue,
                           sizeof (objipRoutePreferenceStaticValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceStatic
*
* @purpose Set 'ipRoutePreferenceStatic'
 *@description  [ipRoutePreferenceStatic] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user
* may configure the preference of individual static and default
* routes. A change to the default preference for static or default
* routes will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceStatic (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceStaticValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceStatic */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceStaticValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceStaticValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_STATIC, objipRoutePreferenceStaticValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceMPLS
*
* @purpose Get 'ipRoutePreferenceMPLS'
 *@description  [ipRoutePreferenceMPLS] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceMPLS (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceMPLSValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_MPLS, &objipRoutePreferenceMPLSValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceMPLSValue, sizeof (objipRoutePreferenceMPLSValue));

  /* return the object value: ipRoutePreferenceMPLS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceMPLSValue,
                           sizeof (objipRoutePreferenceMPLSValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceMPLS
*
* @purpose Set 'ipRoutePreferenceMPLS'
 *@description  [ipRoutePreferenceMPLS] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceMPLS (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceMPLSValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceMPLS */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceMPLSValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceMPLSValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_MPLS, objipRoutePreferenceMPLSValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceOspfIntraArea
*
* @purpose Get 'ipRoutePreferenceOspfIntraArea'
 *@description  [ipRoutePreferenceOspfIntraArea] All routes from the dynamic
* routing protocols use the default preference for that protocol. The
* user may configure the preference of individual static and
* default routes. A change to the default preference for static or
* default routes will not change the preference of existing static or
* default routes. Static and default routes configured after the
* change will use the new default preference. The preference value shall
* be the same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceOspfIntraArea (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceOspfIntraAreaValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_OSPF_INTRA_AREA, &objipRoutePreferenceOspfIntraAreaValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceOspfIntraAreaValue,
                     sizeof (objipRoutePreferenceOspfIntraAreaValue));

  /* return the object value: ipRoutePreferenceOspfIntraArea */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceOspfIntraAreaValue,
                           sizeof (objipRoutePreferenceOspfIntraAreaValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceOspfIntraArea
*
* @purpose Set 'ipRoutePreferenceOspfIntraArea'
 *@description  [ipRoutePreferenceOspfIntraArea] All routes from the dynamic
* routing protocols use the default preference for that protocol. The
* user may configure the preference of individual static and
* default routes. A change to the default preference for static or
* default routes will not change the preference of existing static or
* default routes. Static and default routes configured after the
* change will use the new default preference. The preference value shall
* be the same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceOspfIntraArea (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceOspfIntraAreaValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceOspfIntraArea */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceOspfIntraAreaValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceOspfIntraAreaValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_OSPF_INTRA_AREA, objipRoutePreferenceOspfIntraAreaValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceOspfInterArea
*
* @purpose Get 'ipRoutePreferenceOspfInterArea'
 *@description  [ipRoutePreferenceOspfInterArea] All routes from the dynamic
* routing protocols use the default preference for that protocol. The
* user may configure the preference of individual static and
* default routes. A change to the default preference for static or
* default routes will not change the preference of existing static or
* default routes. Static and default routes configured after the
* change will use the new default preference. The preference value shall
* be the same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceOspfInterArea (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceOspfInterAreaValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_OSPF_INTER_AREA, &objipRoutePreferenceOspfInterAreaValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceOspfInterAreaValue,
                     sizeof (objipRoutePreferenceOspfInterAreaValue));

  /* return the object value: ipRoutePreferenceOspfInterArea */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceOspfInterAreaValue,
                           sizeof (objipRoutePreferenceOspfInterAreaValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceOspfInterArea
*
* @purpose Set 'ipRoutePreferenceOspfInterArea'
 *@description  [ipRoutePreferenceOspfInterArea] All routes from the dynamic
* routing protocols use the default preference for that protocol. The
* user may configure the preference of individual static and
* default routes. A change to the default preference for static or
* default routes will not change the preference of existing static or
* default routes. Static and default routes configured after the
* change will use the new default preference. The preference value shall
* be the same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceOspfInterArea (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceOspfInterAreaValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceOspfInterArea */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceOspfInterAreaValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceOspfInterAreaValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_OSPF_INTER_AREA, objipRoutePreferenceOspfInterAreaValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceOspfExternal
*
* @purpose Get 'ipRoutePreferenceOspfExternal'
 *@description  [ipRoutePreferenceOspfExternal] All routes from the dynamic
* routing protocols use the default preference for that protocol. The
* user may configure the preference of individual static and default
* routes. A change to the default preference for static or default
* routes will not change the preference of existing static or
* default routes. Static and default routes configured after the change
* will use the new default preference. The preference value shall
* be the same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceOspfExternal (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceOspfExternalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_OSPF_EXTERNAL, &objipRoutePreferenceOspfExternalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceOspfExternalValue,
                     sizeof (objipRoutePreferenceOspfExternalValue));

  /* return the object value: ipRoutePreferenceOspfExternal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceOspfExternalValue,
                           sizeof (objipRoutePreferenceOspfExternalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceOspfExternal
*
* @purpose Set 'ipRoutePreferenceOspfExternal'
 *@description  [ipRoutePreferenceOspfExternal] All routes from the dynamic
* routing protocols use the default preference for that protocol. The
* user may configure the preference of individual static and default
* routes. A change to the default preference for static or default
* routes will not change the preference of existing static or
* default routes. Static and default routes configured after the change
* will use the new default preference. The preference value shall
* be the same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceOspfExternal (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceOspfExternalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceOspfExternal */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceOspfExternalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceOspfExternalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_OSPF_EXTERNAL, objipRoutePreferenceOspfExternalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceRIP
*
* @purpose Get 'ipRoutePreferenceRIP'
 *@description  [ipRoutePreferenceRIP] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will use
* the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceRIP (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceRIPValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_RIP, &objipRoutePreferenceRIPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceRIPValue, sizeof (objipRoutePreferenceRIPValue));

  /* return the object value: ipRoutePreferenceRIP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceRIPValue,
                           sizeof (objipRoutePreferenceRIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceRIP
*
* @purpose Set 'ipRoutePreferenceRIP'
 *@description  [ipRoutePreferenceRIP] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will use
* the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceRIP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceRIPValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceRIP */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceRIPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceRIPValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_RIP, objipRoutePreferenceRIPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceIBGP
*
* @purpose Get 'ipRoutePreferenceIBGP'
 *@description  [ipRoutePreferenceIBGP] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceIBGP (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceIBGPValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_IBGP, &objipRoutePreferenceIBGPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceIBGPValue, sizeof (objipRoutePreferenceIBGPValue));

  /* return the object value: ipRoutePreferenceIBGP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceIBGPValue,
                           sizeof (objipRoutePreferenceIBGPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceIBGP
*
* @purpose Set 'ipRoutePreferenceIBGP'
 *@description  [ipRoutePreferenceIBGP] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceIBGP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceIBGPValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceIBGP */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceIBGPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceIBGPValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_IBGP, objipRoutePreferenceIBGPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceEBGP
*
* @purpose Get 'ipRoutePreferenceEBGP'
 *@description  [ipRoutePreferenceEBGP] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_ipRoutePreferenceEBGP (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceEBGPValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_EBGP, &objipRoutePreferenceEBGPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceEBGPValue, sizeof (objipRoutePreferenceEBGPValue));

  /* return the object value: ipRoutePreferenceEBGP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutePreferenceEBGPValue,
                           sizeof (objipRoutePreferenceEBGPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceEBGP
*
* @purpose Set 'ipRoutePreferenceEBGP'
 *@description  [ipRoutePreferenceEBGP] All routes from the dynamic routing
* protocols use the default preference for that protocol. The user may
* configure the preference of individual static and default routes.
* A change to the default preference for static or default routes
* will not change the preference of existing static or default
* routes. Static and default routes configured after the change will
* use the new default preference. The preference value shall be the
* same for all the OSPF external route types like
* type1/type2/nssa1/nssa2.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipmapGlobalConfig_ipRoutePreferenceEBGP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutePreferenceEBGPValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRoutePreferenceEBGP */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRoutePreferenceEBGPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutePreferenceEBGPValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpRouterPreferenceSet(L7_UNIT_CURRENT, ROUTE_PREF_EBGP, objipRoutePreferenceEBGPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_GoodRouteCount
*
* @purpose Get 'GoodRouteCount'
 *@description  [GoodRouteCount] Gets the good route count
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_GoodRouteCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGoodRouteCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;

  objGoodRouteCountValue = usmDbRouteCount(L7_UNIT_CURRENT, L7_TRUE);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGoodRouteCountValue, sizeof (objGoodRouteCountValue));

  /* return the object value: GoodRouteCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGoodRouteCountValue,
                           sizeof (objGoodRouteCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_routingipmapGlobalConfig_RouteCount
*
* @purpose Get 'RouteCount'
 *@description  [RouteCount] Gets the route count
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipmapGlobalConfig_RouteCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRouteCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;

  objRouteCountValue = usmDbRouteCount(L7_UNIT_CURRENT, L7_FALSE);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRouteCountValue, sizeof (objRouteCountValue));

  /* return the object value: RouteCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRouteCountValue, sizeof (objRouteCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


