/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingdebugFlags.c
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
#include "_xe_routingdebugFlags_obj.h"
#include "usmdb_iputil_api.h"
#include "acl_exports.h"

/*******************************************************************************
* @function fpObjGet_routingdebugFlags_arpDebugFlag
*
* @purpose Get 'arpDebugFlag'
*
* @description [arpDebugFlag]: Get/Set the current status of displaying arp
*              packet debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingdebugFlags_arpDebugFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objarpDebugFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  
  objarpDebugFlagValue =  usmDbArpPacketDebugTraceFlagGet ();

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objarpDebugFlagValue,
                     sizeof (objarpDebugFlagValue));

  /* return the object value: arpDebugFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objarpDebugFlagValue,
                           sizeof (objarpDebugFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingdebugFlags_arpDebugFlag
*
* @purpose Set 'arpDebugFlag'
*
* @description [arpDebugFlag]: Get/Set the current status of displaying arp
*              packet debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingdebugFlags_arpDebugFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objarpDebugFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: arpDebugFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objarpDebugFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objarpDebugFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbArpMapPacketDebugTraceFlagSet (objarpDebugFlagValue);
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
* @function fpObjGet_routingdebugFlags_debugFlagAclNum
*
* @purpose Get 'debugFlagAclNum'
*
* @description [debugFlagAclNum]: debug flag acl index 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingdebugFlags_debugFlagAclNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdebugFlagAclNumValue;
  xLibU32_t nextObjdebugFlagAclNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: debugFlagAclNum */
  owa.rc = xLibFilterGet (wap, XOBJ_routingdebugFlags_debugFlagAclNum,
                          (xLibU8_t *) & objdebugFlagAclNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = L7_SUCCESS; 

    /* We dont have get and get nex functions. So we are assigning directly */
    nextObjdebugFlagAclNumValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdebugFlagAclNumValue, owa.len);
    /* We dont have get and get nex functions. So we are assigning directly */
    if(objdebugFlagAclNumValue < L7_MAX_ACL_ID)
    {
       nextObjdebugFlagAclNumValue = objdebugFlagAclNumValue + 1;
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
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdebugFlagAclNumValue, owa.len);

  /* return the object value: debugFlagAclNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdebugFlagAclNumValue,
                           sizeof (objdebugFlagAclNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#ifdef L7_QOS_FLEX_PACKAGE_ACL
/*******************************************************************************
* @function fpObjGet_routingdebugFlags_ipMapDebugFlag
*
* @purpose Get 'ipMapDebugFlag'
*
* @description [ipMapDebugFlag]: Turns on/off the displaying of ip packet
*              debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingdebugFlags_ipMapDebugFlag (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydebugFlagAclNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMapDebugFlagValue;
  L7_BOOL flag;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: debugFlagAclNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingdebugFlags_debugFlagAclNum,
                          (xLibU8_t *) & keydebugFlagAclNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydebugFlagAclNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIpPacketDebugTraceFlagGet (keydebugFlagAclNumValue, &flag);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objipMapDebugFlagValue = flag;

  /* return the object value: ipMapDebugFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMapDebugFlagValue,
                           sizeof (objipMapDebugFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingdebugFlags_ipMapDebugFlag
*
* @purpose Set 'ipMapDebugFlag'
*
* @description [ipMapDebugFlag]: Turns on/off the displaying of ip packet
*              debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingdebugFlags_ipMapDebugFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMapDebugFlagValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydebugFlagAclNumValue;
  L7_BOOL flag;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMapDebugFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipMapDebugFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMapDebugFlagValue, owa.len);

  /* retrieve key: debugFlagAclNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingdebugFlags_debugFlagAclNum,
                          (xLibU8_t *) & keydebugFlagAclNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydebugFlagAclNumValue, kwa.len);

  /* set the value in application */
  flag =(L7_BOOL)objipMapDebugFlagValue;
  owa.l7rc =
    usmDbIpMapPacketDebugTraceFlagSet (keydebugFlagAclNumValue,
                                       objipMapDebugFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/

