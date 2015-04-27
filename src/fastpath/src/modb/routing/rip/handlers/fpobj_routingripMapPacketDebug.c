/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingripMapPacketDebug.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ripDebug-object.xml
*
* @create  29 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingripMapPacketDebug_obj.h"
#include "usmdb_mib_ripv2_api.h"

/*******************************************************************************
* @function fpObjGet_routingripMapPacketDebug_DebugTraceFlag
*
* @purpose Get 'DebugTraceFlag'
*
* @description [DebugTraceFlag]: Get/Set the current status of displaying
*              rip packet debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingripMapPacketDebug_DebugTraceFlag (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDebugTraceFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objDebugTraceFlagValue = usmDbRipMapPacketDebugTraceFlagGet ();

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDebugTraceFlagValue,
                     sizeof (objDebugTraceFlagValue));

  /* return the object value: DebugTraceFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDebugTraceFlagValue,
                           sizeof (objDebugTraceFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingripMapPacketDebug_DebugTraceFlag
*
* @purpose Set 'DebugTraceFlag'
*
* @description [DebugTraceFlag]: Get/Set the current status of displaying
*              rip packet debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingripMapPacketDebug_DebugTraceFlag (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDebugTraceFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DebugTraceFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDebugTraceFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDebugTraceFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbRipMapPacketDebugTraceFlagSet (objDebugTraceFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
