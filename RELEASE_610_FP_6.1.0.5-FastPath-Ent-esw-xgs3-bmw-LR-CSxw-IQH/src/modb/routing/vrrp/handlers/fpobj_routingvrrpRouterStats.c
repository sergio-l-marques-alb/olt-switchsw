/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpRouterStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to vrrpOper-object.xml
*
* @create  19 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingvrrpRouterStats_obj.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_1213_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] A unique value, greater than zero, for each interface.
* It is recommended that values are assigned contiguously starting
* from 1. The value for each interface sub-layer must remain
* constant at least from one re-initialization of the entity's network
* management system to the next re-initialization.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_ifIndex (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t objVrIdValue;
  xLibU8_t nextObjVrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = 0;
    owa.l7rc =
      usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                           (xLibU8_t*)&nextObjVrIdValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
    nextObjVrIdValue = 0;
    nextObjifIndexValue = 0;
    do
    {
      objVrIdValue = nextObjVrIdValue;
      owa.l7rc =
        usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                             (xLibU8_t*)&nextObjVrIdValue, &nextObjifIndexValue);
    }
    while ((objifIndexValue == nextObjifIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingvrrpRouterStats_VrId
*
* @purpose Get 'VrId'
 *@description  [VrId] This object contains the Virtual Router Identifier (VRID).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_VrId (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t vrId = 0;
  xLibU8_t objVrIdValue;
  xLibU8_t nextObjVrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                          (xLibU8_t *) & vrId, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrIdValue, owa.len);
    objVrIdValue = (xLibU8_t)vrId;
  }
  nextObjVrIdValue = 0;
  nextObjifIndexValue = 0;
  owa.l7rc =
    usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                        (xLibU8_t*)&nextObjVrIdValue, &nextObjifIndexValue);

  if ((objifIndexValue != nextObjifIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrIdValue, owa.len);

  vrId = nextObjVrIdValue;
  /* return the object value: VrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & vrId, sizeof (vrId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_BecomeMaster
*
* @purpose Get 'BecomeMaster'
*
* @description [BecomeMaster]: The total number of times that this virtual
*              router's state has transitioned to MASTER 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_BecomeMaster (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBecomeMasterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsBecomeMaster (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                         keyifIndexValue,
                                         &objBecomeMasterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BecomeMaster */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBecomeMasterValue,
                           sizeof (objBecomeMasterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_AdvertiseRcvd
*
* @purpose Get 'AdvertiseRcvd'
*
* @description [AdvertiseRcvd]: The total number of VRRP advertisements received
*              by this virtual router 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_AdvertiseRcvd (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdvertiseRcvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsAdvertiseRcvd (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                          keyifIndexValue,
                                          &objAdvertiseRcvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdvertiseRcvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdvertiseRcvdValue,
                           sizeof (objAdvertiseRcvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_AdvertiseIntervalErrors
*
* @purpose Get 'AdvertiseIntervalErrors'
*
* @description [AdvertiseIntervalErrors]: The total number of VRRP advertisement
*              packets received for which the advertisement interval
*              is different than the one configured for the local virtual
*              router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_AdvertiseIntervalErrors (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdvertiseIntervalErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpStatsAdvertiseIntervalErrorsGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                              keyifIndexValue,
                                              &objAdvertiseIntervalErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdvertiseIntervalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdvertiseIntervalErrorsValue,
                           sizeof (objAdvertiseIntervalErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_AuthFailures
*
* @purpose Get 'AuthFailures'
*
* @description [AuthFailures]: The total number of VRRP packets received that
*              do not pass the authentication check. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_AuthFailures (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthFailuresValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsAuthFailuresGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                            keyifIndexValue,
                                            &objAuthFailuresValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthFailuresValue,
                           sizeof (objAuthFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_IpTtlErrors
*
* @purpose Get 'IpTtlErrors'
*
* @description [IpTtlErrors]: The total number of VRRP packets received by
*              the virtual router with IP TTL (Time-To-Live) not equal to
*              255. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_IpTtlErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpTtlErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsIpTTLFailedGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                           keyifIndexValue,
                                           &objIpTtlErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpTtlErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpTtlErrorsValue,
                           sizeof (objIpTtlErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_PriorityZeroPktsRcvd
*
* @purpose Get 'PriorityZeroPktsRcvd'
*
* @description [PriorityZeroPktsRcvd]: The total number of VRRP packets received
*              by the virtual router with a priority of '0'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_PriorityZeroPktsRcvd (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPriorityZeroPktsRcvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpStatsPriorityZeroPktsRcvdGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                           keyifIndexValue,
                                           &objPriorityZeroPktsRcvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PriorityZeroPktsRcvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPriorityZeroPktsRcvdValue,
                           sizeof (objPriorityZeroPktsRcvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_PriorityZeroPktsSent
*
* @purpose Get 'PriorityZeroPktsSent'
*
* @description [PriorityZeroPktsSent]: The total number of VRRP packets sent
*              by the virtual router with a priority of '0'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_PriorityZeroPktsSent (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPriorityZeroPktsSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpStatsPriorityZeroPktsSentGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                           keyifIndexValue,
                                           &objPriorityZeroPktsSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PriorityZeroPktsSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPriorityZeroPktsSentValue,
                           sizeof (objPriorityZeroPktsSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_InvalidTypePktsRcvd
*
* @purpose Get 'InvalidTypePktsRcvd'
*
* @description [InvalidTypePktsRcvd]: The number of VRRP packets received
*              by the virtual router with an invalid value in the 'type' field.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_InvalidTypePktsRcvd (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInvalidTypePktsRcvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpStatsInvalidTypePktsRcvdGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                          keyifIndexValue,
                                          &objInvalidTypePktsRcvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: InvalidTypePktsRcvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInvalidTypePktsRcvdValue,
                           sizeof (objInvalidTypePktsRcvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_AddressListErrors
*
* @purpose Get 'AddressListErrors'
*
* @description [AddressListErrors]: The total number of packets received for
*              which the address list does not match the locally configured
*              list for the virtual router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_AddressListErrors (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressListErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsAddressListErrorsGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                                 keyifIndexValue,
                                                 &objAddressListErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AddressListErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAddressListErrorsValue,
                           sizeof (objAddressListErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_InvalidAuthType
*
* @purpose Get 'InvalidAuthType'
*
* @description [InvalidAuthType]: The total number of packets received with
*              an unknown authentication type. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_InvalidAuthType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInvalidAuthTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsInvalidAuthTypeGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                               keyifIndexValue,
                                               &objInvalidAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: InvalidAuthType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInvalidAuthTypeValue,
                           sizeof (objInvalidAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_AuthTypeMismatch
*
* @purpose Get 'AuthTypeMismatch'
*
* @description [AuthTypeMismatch]: The total number of packets received with
*              'Auth Type' not equal to the locally configured authentication
*              method (`vrrpOperAuthType'). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_AuthTypeMismatch (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeMismatchValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsAuthTypeMismatchGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                                keyifIndexValue,
                                                &objAuthTypeMismatchValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthTypeMismatch */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthTypeMismatchValue,
                           sizeof (objAuthTypeMismatchValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_PacketLengthErrors
*
* @purpose Get 'PacketLengthErrors'
*
* @description [PacketLengthErrors]: The total number of packets received
*              with a packet length less than the length of the VRRP header.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_PacketLengthErrors (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPacketLengthErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    /* SET_ERR_MSG
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext); */

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = usmDbVrrpStatsPktLengthGet (L7_UNIT_CURRENT, (xLibU8_t)keyVrIdValue,
                                         keyifIndexValue,
                                         &objPacketLengthErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PacketLengthErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPacketLengthErrorsValue,
                           sizeof (objPacketLengthErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingvrrpRouterStats_UpTime
*
* @purpose Get 'UpTime'
*
* @description [UpTime]: This is the value of the `sysUpTime'
*              object when this virtual router (i.e., the `vrrpOperState')
*              transitioned out of `initialized'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterStats_UpTime (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVirtualRouterUpTimeValue;

  L7_vrrpState_t state;
  L7_uint32 vrEnableTime;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_VrId,
                           (xLibU8_t *) & keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpRouterStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* get the value from application */


  if ((usmDbVrrpOperStateGet(L7_UNIT_CURRENT,
		                                 (xLibU8_t)keyVrIdValue, 
		                                 keyifIndexValue, 
		                                 &state) == L7_SUCCESS) &&
        (state != L7_VRRP_STATE_INIT) &&
        (usmDbVrrpOperUpTimeGet(L7_UNIT_CURRENT, keyVrIdValue,
                                     keyifIndexValue,
                                     &vrEnableTime) == L7_SUCCESS))
    {
      /* Figure out how long the virtual router has been up. */
      objVirtualRouterUpTimeValue = osapiUpTimeRaw() - vrEnableTime;
	   owa.l7rc = L7_SUCCESS;
    }
    else
    {
      objVirtualRouterUpTimeValue = 0;
	   owa.l7rc = L7_SUCCESS;
    }
		
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VirtualRouterUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVirtualRouterUpTimeValue,
                           sizeof (objVirtualRouterUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

