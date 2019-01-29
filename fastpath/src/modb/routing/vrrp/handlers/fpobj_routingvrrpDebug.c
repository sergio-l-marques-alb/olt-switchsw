/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpDebug.c
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
#include "_xe_routingvrrpDebug_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpDebug_PacketTraceFlag
*
* @purpose Get 'PacketTraceFlag'
*
* @description [PacketTraceFlag]: Turns on/off the displaying of vrrp packet
*              debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpDebug_PacketTraceFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPacketTraceFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
 
  if(usmDbIpVrrpPacketDebugTraceFlagGet () == L7_TRUE)
  {  
     objPacketTraceFlagValue = L7_TRUE;
  }
  else
  {
     objPacketTraceFlagValue = L7_FALSE;
  }
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPacketTraceFlagValue,
                     sizeof (objPacketTraceFlagValue));

  /* return the object value: PacketTraceFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPacketTraceFlagValue,
                           sizeof (objPacketTraceFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpDebug_PacketTraceFlag
*
* @purpose Set 'PacketTraceFlag'
*
* @description [PacketTraceFlag]: Turns on/off the displaying of vrrp packet
*              debug info 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpDebug_PacketTraceFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPacketTraceFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PacketTraceFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPacketTraceFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPacketTraceFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpVrrpPacketDebugTraceFlagSet (objPacketTraceFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
