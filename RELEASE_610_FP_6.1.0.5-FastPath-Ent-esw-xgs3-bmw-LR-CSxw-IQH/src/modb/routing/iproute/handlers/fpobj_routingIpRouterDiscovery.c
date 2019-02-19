/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingIpRouterDiscovery.c
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
#include "_xe_routingIpRouterDiscovery_obj.h"
#include "usmdb_rtrdisc_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_util_api.h"
#include "usmdb_1213_api.h"

static L7_RC_t
usmDbIpInterfaceNextGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum)
{
  L7_int32 temp_val;
  /* loop through available external interface numbers */
  while (usmDbValidIntIfNumNext(*intIfNum, intIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if ( (usmDbValidateRtrIntf(UnitIndex, *intIfNum) == L7_SUCCESS) && 
         (usmDbVisibleInterfaceCheck(UnitIndex, *intIfNum, &temp_val) == L7_SUCCESS) )
    return L7_SUCCESS;

  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_routingIpRouterDiscovery_IfIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: Interface Number to configure Router Discovery on.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_IfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjIfIndexValue = 0;
    /* we dont have direct usmdb function which gives the ip router discovery interface
    to compensate this i have writedown following function*/
    owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjIfIndexValue);
   
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
 
    nextObjIfIndexValue = objIfIndexValue;
    owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue,
                           sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingIpRouterDiscovery_advertiseMode
*
* @purpose Get 'advertiseMode'
*
* @description [advertiseMode]: Enable or disable router discovery on the
*              interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_advertiseMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadvertiseModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_TRUE)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbRtrDiscAdvertiseGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                       &objadvertiseModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: advertiseMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objadvertiseModeValue,
                           sizeof (objadvertiseModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRouterDiscovery_advertiseMode
*
* @purpose Set 'advertiseMode'
*
* @description [advertiseMode]: Enable or disable router discovery on the
*              interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRouterDiscovery_advertiseMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadvertiseModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: advertiseMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objadvertiseModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objadvertiseModeValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);


  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbRtrDiscAdvertiseSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                       objadvertiseModeValue);
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
* @function fpObjGet_routingIpRouterDiscovery_maxAdvertisementInterval
*
* @purpose Get 'maxAdvertisementInterval'
*
* @description [maxAdvertisementInterval]: Maximum time allowed between sending
*              router advertisements from the interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_maxAdvertisementInterval (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmaxAdvertisementIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_TRUE)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbRtrDiscMaxAdvIntervalGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                            &objmaxAdvertisementIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: maxAdvertisementInterval */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objmaxAdvertisementIntervalValue,
                    sizeof (objmaxAdvertisementIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRouterDiscovery_maxAdvertisementInterval
*
* @purpose Set 'maxAdvertisementInterval'
*
* @description [maxAdvertisementInterval]: Maximum time allowed between sending
*              router advertisements from the interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRouterDiscovery_maxAdvertisementInterval (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmaxAdvertisementIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: maxAdvertisementInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objmaxAdvertisementIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmaxAdvertisementIntervalValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;                                          
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbRtrDiscMaxAdvIntervalSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                            objmaxAdvertisementIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {  
    owa.rc = XLIBRC_INVALID_ROUTE_DISC_INTERVAL;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingIpRouterDiscovery_minAdvertisementInterval
*
* @purpose Get 'minAdvertisementInterval'
*
* @description [minAdvertisementInterval]: Minimum time allowed between sending
*              router advertisements from the interface. This value
*              must be less than or equal to agentSwitchIpRouterDiscoveryMaxAdvertisementInterval.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_minAdvertisementInterval (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objminAdvertisementIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_TRUE)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbRtrDiscMinAdvIntervalGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                            &objminAdvertisementIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: minAdvertisementInterval */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objminAdvertisementIntervalValue,
                    sizeof (objminAdvertisementIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRouterDiscovery_minAdvertisementInterval
*
* @purpose Set 'minAdvertisementInterval'
*
* @description [minAdvertisementInterval]: Minimum time allowed between sending
*              router advertisements from the interface. This value
*              must be less than or equal to agentSwitchIpRouterDiscoveryMaxAdvertisementInterval.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRouterDiscovery_minAdvertisementInterval (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objminAdvertisementIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: minAdvertisementInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objminAdvertisementIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objminAdvertisementIntervalValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc = usmDbRtrDiscMinAdvIntervalSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                            objminAdvertisementIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_ROUTE_DISC_INTERVAL; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingIpRouterDiscovery_advertisementLifetime
*
* @purpose Get 'advertisementLifetime'
*
* @description [advertisementLifetime]: Value of lifetime field of router
*              advertsisement sent from the interface. This value must be
*              greater than or equal to agentSwitchIpRouterDiscoveryMaxAdvertisementInterval.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_advertisementLifetime (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadvertisementLifetimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_TRUE)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbRtrDiscAdvLifetimeGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                         &objadvertisementLifetimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: advertisementLifetime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objadvertisementLifetimeValue,
                           sizeof (objadvertisementLifetimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRouterDiscovery_advertisementLifetime
*
* @purpose Set 'advertisementLifetime'
*
* @description [advertisementLifetime]: Value of lifetime field of router
*              advertsisement sent from the interface. This value must be
*              greater than or equal to agentSwitchIpRouterDiscoveryMaxAdvertisementInterval.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRouterDiscovery_advertisementLifetime (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadvertisementLifetimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: advertisementLifetime */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objadvertisementLifetimeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objadvertisementLifetimeValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* set the value in application */
  owa.l7rc = usmDbRtrDiscAdvLifetimeSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                       objadvertisementLifetimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_ROUTE_DISC_INTERVAL;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingIpRouterDiscovery_preferenceLevel
*
* @purpose Get 'preferenceLevel'
*
* @description [preferenceLevel]: Preferability of the address as a default
*              router address, related to other addresses on the same subnet.
*              This is defined as the larger the number, the higher
*              the preference. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_preferenceLevel (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objpreferenceLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_TRUE)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbRtrDiscPreferenceLevelGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                             &objpreferenceLevelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: preferenceLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpreferenceLevelValue,
                           sizeof (objpreferenceLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRouterDiscovery_preferenceLevel
*
* @purpose Set 'preferenceLevel'
*
* @description [preferenceLevel]: Preferability of the address as a default
*              router address, related to other addresses on the same subnet.
*              This is defined as the larger the number, the higher
*              the preference. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRouterDiscovery_preferenceLevel (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objpreferenceLevelValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: preferenceLevel */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objpreferenceLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpreferenceLevelValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ( ( objpreferenceLevelValue >= (L7_int32)L7_RTR_DISC_PREFERENCE_MIN ) && ( objpreferenceLevelValue <= (L7_int32)L7_RTR_DISC_PREFERENCE_MAX ) )
  {
     owa.l7rc = usmDbRtrDiscPreferenceLevelSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                             objpreferenceLevelValue);
     if (owa.l7rc != L7_SUCCESS)
     {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }
  else
  {
     /* SET_ERR_MSG
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangePrefValMustBeInRangeOfTo, L7_RTR_DISC_PREFERENCE_MIN, L7_RTR_DISC_PREFERENCE_MAX); */
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
  }
  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingIpRouterDiscovery_advertisementAddress
*
* @purpose Get 'advertisementAddress'
*
* @description [advertisementAddress]: Address used when sending router advertisements
*              from the interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpRouterDiscovery_advertisementAddress (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadvertisementAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_TRUE)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbRtrDiscAdvAddrGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                     &objadvertisementAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: advertisementAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objadvertisementAddressValue,
                           sizeof (objadvertisementAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingIpRouterDiscovery_advertisementAddress
*
* @purpose Set 'advertisementAddress'
*
* @description [advertisementAddress]: Address used when sending router advertisements
*              from the interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpRouterDiscovery_advertisementAddress (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadvertisementAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: advertisementAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objadvertisementAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objadvertisementAddressValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingIpRouterDiscovery_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbRtrDiscAdvAddrSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                     objadvertisementAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_ROUTE_DISC_ADVERT_ADDRESS;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
