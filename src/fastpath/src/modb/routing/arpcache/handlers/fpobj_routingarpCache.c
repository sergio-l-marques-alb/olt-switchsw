/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingarpCache.c
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
#include "_xe_routingarpCache_obj.h"

/*******************************************************************************
* @function fpObjSet_routingarpCache_ArpCacheClear
*
* @purpose Set 'ArpCacheClear'
*
* @description [ArpCacheClear]: Clears the ARP cache of all dynamic/gateway
*              entries 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpCache_ArpCacheClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objArpCacheClearValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ArpCacheClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objArpCacheClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objArpCacheClearValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpArpCacheClear (L7_UNIT_CURRENT, objArpCacheClearValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

