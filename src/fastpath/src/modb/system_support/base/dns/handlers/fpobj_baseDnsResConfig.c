/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDnsResConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dns-object.xml
*
* @create  4 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDnsResConfig_obj.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_common.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_registry_api.h"
#include "usmdb_util_api.h"

#define L7_DEF_CONFIG_RESET_TIME 0

/*******************************************************************************
* @function fpObjGet_baseDnsResConfig_Service
*
* @purpose Get 'Service'
*
* @description Kind of DNS resolution service provided: recursiveOnly(1) indicates 
*              a stub resolver. iterativeOnly(2) indicates a normal full 
*              service resolver. recursiveAndIterative(3) indicates a full-service 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResConfig_Service (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objServiceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientServiceGet (&objServiceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objServiceValue, sizeof (objServiceValue));

  /* return the object value: Service */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServiceValue,
                           sizeof (objServiceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResConfig_ImplementIdent
*
* @purpose Get 'ImplementIdent'
*
* @description The implementation identification string for the resolver software 
*              in use on the system, for example; `RES-2.1' 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResConfig_ImplementIdent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objImplementIdentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  usmDbCodeVersion_t ver;
  L7_uint32 unit;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    owa.l7rc =  usmDbUnitMgrMgrNumberGet(&unit);
    if ( (owa.l7rc == L7_SUCCESS) &&(owa.l7rc = usmDbUnitMgrDetectCodeVerRunningGet(unit, &ver)) == L7_SUCCESS)
    {
        sprintf(objImplementIdentValue, "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
    }
  }
  else
     owa.l7rc = usmDbSwVersionGet (L7_UNIT_CURRENT, objImplementIdentValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objImplementIdentValue,
                     strlen (objImplementIdentValue));

  /* return the object value: ImplementIdent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objImplementIdentValue,
                           strlen (objImplementIdentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResConfig_ResetTime
*
* @purpose Get 'ResetTime'
*
* @description If the resolver has a persistent state (e.g., a process) and supports 
*              a `reset' operation (e.g., can be told to re-read configuration 
*              files), this value will be the time elapsed since 
*              the last time the resolver wa 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResConfig_ResetTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objResetTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objResetTimeValue = L7_DEF_CONFIG_RESET_TIME;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objResetTimeValue, sizeof (objResetTimeValue));

  /* return the object value: ResetTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objResetTimeValue,
                           sizeof (objResetTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResConfig_UpTime
*
* @purpose Get 'UpTime'
*
* @description If the resolver has a persistent state (e.g., a process), this 
*              value will be the time elapsed since it started. For software 
*              without persistant state, this value will be 0. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResConfig_UpTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientUpTimeGet (&objUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUpTimeValue, sizeof (objUpTimeValue));

  /* return the object value: UpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUpTimeValue,
                           sizeof (objUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResConfig_MaxCnames
*
* @purpose Get 'MaxCnames'
*
* @description Limit on how many CNAMEs the resolver should allow before deciding 
*              that there's a CNAME loop. Zero means that resolver has 
*              no explicit CNAME limit. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResConfig_MaxCnames (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxCnamesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientMaxCnamesGet (&objMaxCnamesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxCnamesValue, sizeof (objMaxCnamesValue));

  /* return the object value: MaxCnames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxCnamesValue,
                           sizeof (objMaxCnamesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
