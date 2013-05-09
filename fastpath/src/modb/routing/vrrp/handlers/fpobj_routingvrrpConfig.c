/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to vrrp-object.xml
*
* @create  13 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingvrrpConfig_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpConfig_adminState
*
* @purpose Get 'adminState'
*
* @description [adminState]: Sets the administrative mode of VRRP in the router.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpConfig_adminState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadminStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbVrrpOperAdminStateGet(L7_UNIT_CURRENT, &objadminStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objadminStateValue, sizeof (objadminStateValue));

  /* return the object value: adminState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objadminStateValue,
                           sizeof (objadminStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpConfig_adminState
*
* @purpose Set 'adminState'
*
* @description [adminState]: Sets the administrative mode of VRRP in the router.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpConfig_adminState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objadminStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: adminState */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objadminStateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objadminStateValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbVrrpOperAdminStateSet(L7_UNIT_CURRENT, objadminStateValue);
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
* @function fpObjGet_routingvrrpConfig_newMasterTrapFlag
*
* @purpose Get 'newMasterTrapFlag'
*
* @description [newMasterTrapFlag]: This flag enables the sending of VRRP
*              new master notification traps. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpConfig_newMasterTrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnewMasterTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapVrrpSwGet(L7_UNIT_CURRENT, &objnewMasterTrapFlagValue, L7_VRRP_TRAP_NEW_MASTER);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnewMasterTrapFlagValue,
                     sizeof (objnewMasterTrapFlagValue));

  /* return the object value: newMasterTrapFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objnewMasterTrapFlagValue,
                           sizeof (objnewMasterTrapFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpConfig_newMasterTrapFlag
*
* @purpose Set 'newMasterTrapFlag'
*
* @description [newMasterTrapFlag]: This flag enables the sending of VRRP
*              new master notification traps. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpConfig_newMasterTrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnewMasterTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: newMasterTrapFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objnewMasterTrapFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnewMasterTrapFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTrapVrrpSwSet(L7_UNIT_CURRENT, objnewMasterTrapFlagValue, L7_VRRP_TRAP_NEW_MASTER);
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
* @function fpObjGet_routingvrrpConfig_authFailureTrapFlag
*
* @purpose Get 'authFailureTrapFlag'
*
* @description [authFailureTrapFlag]: This flag enables the sending of VRRP
*              authentication failure notification traps. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpConfig_authFailureTrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objauthFailureTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapVrrpSwGet(L7_UNIT_CURRENT, &objauthFailureTrapFlagValue, L7_VRRP_TRAP_AUTH_FAILURE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objauthFailureTrapFlagValue,
                     sizeof (objauthFailureTrapFlagValue));

  /* return the object value: authFailureTrapFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objauthFailureTrapFlagValue,
                           sizeof (objauthFailureTrapFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpConfig_authFailureTrapFlag
*
* @purpose Set 'authFailureTrapFlag'
*
* @description [authFailureTrapFlag]: This flag enables the sending of VRRP
*              authentication failure notification traps. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpConfig_authFailureTrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objauthFailureTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: authFailureTrapFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objauthFailureTrapFlagValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objauthFailureTrapFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTrapVrrpSwSet(L7_UNIT_CURRENT, objauthFailureTrapFlagValue, L7_VRRP_TRAP_AUTH_FAILURE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
