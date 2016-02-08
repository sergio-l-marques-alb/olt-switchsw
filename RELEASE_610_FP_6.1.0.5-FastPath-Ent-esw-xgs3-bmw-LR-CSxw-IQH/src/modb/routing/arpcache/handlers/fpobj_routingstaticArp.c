/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingstaticArp.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to arpcache-object.xml
*
* @create  27 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingstaticArp_obj.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingstaticArp_StaticArpCount
*
* @purpose Get 'StaticArpCount'
*
* @description [StaticArpCount]: Count the number of static ARP entries currently
*              configured. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingstaticArp_StaticArpCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticArpCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;

  objStaticArpCountValue =  usmDbIpMapStaticArpCount (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticArpCountValue,
                     sizeof (objStaticArpCountValue));

  /* return the object value: StaticArpCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticArpCountValue,
                           sizeof (objStaticArpCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
