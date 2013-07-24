/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1dGmrpPortConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to GMRP-object.xml
*
* @create  6 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot1dGmrpPortConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_garp.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1dGmrpPortConfig_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface]: Interface to be configured for GARP Parameters
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGmrpPortConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGmrpPortConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext( objInterfaceValue,
                                       &nextObjInterfaceValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1dGmrpPortConfig_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The administrative state of GMRP operation on this
*              port. The value enabled(1) indicates that GMRP is enabled
*              on this port in all VLANs as long as dot1dGmrpStatus is also
*              enabled(1). A value of disabled(2) indicates that GMRP is
*              disabled on this port in al 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGmrpPortConfig_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGmrpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpGmrpPortEnableGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1dGmrpPortConfig_Status
*
* @purpose Set 'Status'
*
* @description [Status]: The administrative state of GMRP operation on this
*              port. The value enabled(1) indicates that GMRP is enabled
*              on this port in all VLANs as long as dot1dGmrpStatus is also
*              enabled(1). A value of disabled(2) indicates that GMRP is
*              disabled on this port in al 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dGmrpPortConfig_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGmrpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbGarpGmrpPortEnableSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         objStatusValue);
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
* @function fpObjGet_Switchingdot1dGmrpPortConfig_FailedRegistrations
*
* @purpose Get 'FailedRegistrations'
*
* @description [FailedRegistrations]: The total number of failed GMRP registrations,
*              for any reason, in all VLANs, on this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGmrpPortConfig_FailedRegistrations (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFailedRegistrationsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGmrpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortGmrpFailedRegistrationsGet (L7_UNIT_CURRENT,
                                              keyInterfaceValue,
                                              &objFailedRegistrationsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FailedRegistrations */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFailedRegistrationsValue,
                           sizeof (objFailedRegistrationsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1dGmrpPortConfig_LastPduOrigin
*
* @purpose Get 'LastPduOrigin'
*
* @description [LastPduOrigin]: The Source MAC Address of the last GMRP message
*              received on this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGmrpPortConfig_LastPduOrigin (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLastPduOriginValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGmrpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortGmrpLastPduOriginGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                        objLastPduOriginValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LastPduOrigin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLastPduOriginValue,
                           strlen (objLastPduOriginValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
