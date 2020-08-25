/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpOperationGroup.c
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
#include "_xe_routingvrrpOperationGroup_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpOperationGroup_NodeVersion
*
* @purpose Get 'NodeVersion'
*
* @description [NodeVersion]: This value identifies the particular version
*              of the VRRP supported by this node. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationGroup_NodeVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNodeVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbVrrpNodeVersionGet (L7_UNIT_CURRENT, &objNodeVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNodeVersionValue, sizeof (objNodeVersionValue));

  /* return the object value: NodeVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNodeVersionValue,
                           sizeof (objNodeVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationGroup_NotificationCntl
*
* @purpose Get 'NotificationCntl'
*
* @description [NotificationCntl]: Indicates whether the VRRP-enabled router
*              will generate SNMP traps for events defined in this MIB.
*              'Enabled' results in SNMP traps; 'disabled', no traps are
*              sent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationGroup_NotificationCntl (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t objNotificationCntlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpNotificationCntlGet (L7_UNIT_CURRENT, &objNotificationCntlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotificationCntlValue,
                     sizeof (objNotificationCntlValue));

  /* return the object value: NotificationCntl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNotificationCntlValue,
                           sizeof (objNotificationCntlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationGroup_NotificationCntl
*
* @purpose Set 'NotificationCntl'
*
* @description [NotificationCntl]: Indicates whether the VRRP-enabled router
*              will generate SNMP traps for events defined in this MIB.
*              'Enabled' results in SNMP traps; 'disabled', no traps are
*              sent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationGroup_NotificationCntl (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotificationCntlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotificationCntl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNotificationCntlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotificationCntlValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbVrrpNotificationCntlSet (L7_UNIT_CURRENT, objNotificationCntlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
