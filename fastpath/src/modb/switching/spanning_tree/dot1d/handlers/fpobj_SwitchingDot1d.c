/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1d.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1d-object.xml
*
* @create  11 March 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1d_obj.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_common.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dot1d_api.h"

L7_RC_t
fpObjUtilDot1dBasePortEntryGet ( L7_uint32 UnitIndex, L7_uint32 dot1dBasePort)
{
  L7_RC_t rc = L7_FAILURE;
  L7_INTF_TYPES_t sysIntfType;

  /* interfaces must be valid and visible */
  rc = usmDbDot1dBasePortEntryGet(UnitIndex, dot1dBasePort);

  if ((rc == L7_SUCCESS) && (usmDbVisibleIntIfNumberCheck(UnitIndex, dot1dBasePort) == L7_SUCCESS) &&
      (usmDbIntfTypeGet(dot1dBasePort, &sysIntfType) == L7_SUCCESS) && (sysIntfType != L7_CPU_INTF))
  {
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
fpObjUtilDot1dBasePortEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *dot1dBasePort)
{
  /* cycle through valid internal interfaces */
  while (usmDbGetNextVisibleIntIfNumber(*dot1dBasePort, dot1dBasePort) == L7_SUCCESS)
  {
    /* only return success if it's a valid interface for this table */
    if (usmDbDot1dIsValidIntf(UnitIndex, *dot1dBasePort) == L7_TRUE &&
        fpObjUtilDot1dBasePortEntryGet(UnitIndex, *dot1dBasePort) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }

  /* if no more valid interfaces exist, return failure */
  return L7_FAILURE;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dBasePort
*
* @purpose Get 'Dot1dBasePort'
*
* @description [Dot1dBasePort]: list of information for each port of bridge.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dBasePort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dBasePortValue;
  xLibU32_t nextObjDot1dBasePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & objDot1dBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjDot1dBasePortValue = 0;
    owa.l7rc = fpObjUtilDot1dBasePortEntryNextGet(L7_UNIT_CURRENT,
                                     &nextObjDot1dBasePortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1dBasePortValue, owa.len);
    nextObjDot1dBasePortValue = objDot1dBasePortValue;
    owa.l7rc = fpObjUtilDot1dBasePortEntryNextGet(L7_UNIT_CURRENT, 
                                    &nextObjDot1dBasePortValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDot1dBasePortValue, owa.len);

  /* return the object value: Dot1dBasePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDot1dBasePortValue,
                           sizeof (objDot1dBasePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dBasePortIfIndex
*
* @purpose Get 'Dot1dBasePortIfIndex'
*
* @description [Dot1dBasePortIfIndex]: ifIndex associated with the internal
*              interface number. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dBasePortIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dBasePortIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dBasePortIfIndexGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                  &objDot1dBasePortIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dBasePortIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dBasePortIfIndexValue,
                           sizeof (objDot1dBasePortIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dBasePortCircuit
*
* @purpose Get 'Dot1dBasePortCircuit'
*
* @description [Dot1dBasePortCircuit]: name of an object instance unique to
*              this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dBasePortCircuit (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1dBasePortCircuitValue;
  L7_uint32 buf_len;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dBasePortCircuitGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                  (xLibU32_t *)objDot1dBasePortCircuitValue, &buf_len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dBasePortCircuit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1dBasePortCircuitValue,
                           strlen (objDot1dBasePortCircuitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dBasePortDelayExceededDiscards
*
* @purpose Get 'Dot1dBasePortDelayExceededDiscards'
*
* @description [Dot1dBasePortDelayExceededDiscards]: number of frames discarded
*              by this port due to delay through the bridge. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dBasePortDelayExceededDiscards (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dBasePortDelayExceededDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dBasePortDelayExceededDiscardsGet (L7_UNIT_CURRENT,
                                                keyDot1dBasePortValue,
                                                &objDot1dBasePortDelayExceededDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dBasePortDelayExceededDiscards */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1dBasePortDelayExceededDiscardsValue,
                    sizeof (objDot1dBasePortDelayExceededDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dBasePortMtuExceededDiscards
*
* @purpose Get 'Dot1dBasePortMtuExceededDiscards'
*
* @description [Dot1dBasePortMtuExceededDiscards]: number of frames discarded
*              by this port due to excessive size 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dBasePortMtuExceededDiscards (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dBasePortMtuExceededDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dBasePortMtuExceededDiscardsGet (L7_UNIT_CURRENT,
                                              keyDot1dBasePortValue,
                                              &objDot1dBasePortMtuExceededDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dBasePortMtuExceededDiscards */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1dBasePortMtuExceededDiscardsValue,
                    sizeof (objDot1dBasePortMtuExceededDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortCapabilities
*
* @purpose Get 'Dot1dPortCapabilities'
*
* @description [Dot1dPortCapabilities]: the optional parts of IEEE 802.1D
*              that are implemented and are manageable 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortCapabilities (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1dPortCapabilitiesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortCapabilitiesGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                   objDot1dPortCapabilitiesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1dPortCapabilitiesValue,
                           strlen (objDot1dPortCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortDefaultUserPriority
*
* @purpose Get 'Dot1dPortDefaultUserPriority'
*
* @description [Dot1dPortDefaultUserPriority]: the default ingress User Priority
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortDefaultUserPriority (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortDefaultUserPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortDefaultUserPriorityGet (L7_UNIT_CURRENT,
                                          keyDot1dBasePortValue,
                                          &objDot1dPortDefaultUserPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortDefaultUserPriority */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dPortDefaultUserPriorityValue,
                    sizeof (objDot1dPortDefaultUserPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1d_Dot1dPortDefaultUserPriority
*
* @purpose Set 'Dot1dPortDefaultUserPriority'
*
* @description [Dot1dPortDefaultUserPriority]: the default ingress User Priority
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1d_Dot1dPortDefaultUserPriority (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortDefaultUserPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dPortDefaultUserPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1dPortDefaultUserPriorityValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dPortDefaultUserPriorityValue, owa.len);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1dPortDefaultUserPrioritySet (L7_UNIT_CURRENT,
                                          keyDot1dBasePortValue,
                                          objDot1dPortDefaultUserPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#ifdef L7_FEAT_GMRP
/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortGarpJoinTime
*
* @purpose Get 'Dot1dPortGarpJoinTime'
*
* @description [Dot1dPortGarpJoinTime]: GARP Join time, in centiseconds 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortGarpJoinTime (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortGarpJoinTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortGarpJoinTimeGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                   &objDot1dPortGarpJoinTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortGarpJoinTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dPortGarpJoinTimeValue,
                           sizeof (objDot1dPortGarpJoinTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortGarpLeaveAllTime
*
* @purpose Get 'Dot1dPortGarpLeaveAllTime'
*
* @description [Dot1dPortGarpLeaveAllTime]: GARP LeaveAll time 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortGarpLeaveAllTime (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortGarpLeaveAllTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortGarpLeaveAllTimeGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                       &objDot1dPortGarpLeaveAllTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortGarpLeaveAllTime */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dPortGarpLeaveAllTimeValue,
                    sizeof (objDot1dPortGarpLeaveAllTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortGarpLeaveTime
*
* @purpose Get 'Dot1dPortGarpLeaveTime'
*
* @description [Dot1dPortGarpLeaveTime]: GARP Leave time 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortGarpLeaveTime (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortGarpLeaveTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortGarpLeaveTimeGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                    &objDot1dPortGarpLeaveTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortGarpLeaveTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dPortGarpLeaveTimeValue,
                           sizeof (objDot1dPortGarpLeaveTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortGmrpStatus
*
* @purpose Get 'Dot1dPortGmrpStatus'
*
* @description [Dot1dPortGmrpStatus]: Gmrp status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortGmrpStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortGmrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortGmrpStatusGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                 &objDot1dPortGmrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortGmrpStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dPortGmrpStatusValue,
                           sizeof (objDot1dPortGmrpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dPortNumTrafficClasses
*
* @purpose Get 'Dot1dPortNumTrafficClasses'
*
* @description [Dot1dPortNumTrafficClasses]: number of traffic classes 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dPortNumTrafficClasses (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dPortNumTrafficClassesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dPortNumTrafficClassesGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                        &objDot1dPortNumTrafficClassesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dPortNumTrafficClasses */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dPortNumTrafficClassesValue,
                    sizeof (objDot1dPortNumTrafficClassesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dTpPortInFrames
*
* @purpose Get 'Dot1dTpPortInFrames'
*
* @description [Dot1dTpPortInFrames]: Port In frames 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dTpPortInFrames (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpPortInFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dTpPort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dTpPortInFramesGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                 &objDot1dTpPortInFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dTpPortInFrames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTpPortInFramesValue,
                           sizeof (objDot1dTpPortInFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_timeSinceCountersLastCleared
*
* @purpose Get 'timeSinceCountersLastCleared'
*
* @description [timeSinceCountersLastCleared] timeSinceCountersLastCleared
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_timeSinceCountersLastCleared (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1dBasePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtimeSinceCountersLastClearedValue;
  FPOBJ_TRACE_ENTER (bufp);

    usmDbTimeSpec_t ts;


  /* retrieve key: Dot1dBasePort */
  kwaDot1dBasePort.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                                       (xLibU8_t *) & keyDot1dBasePortValue, &kwaDot1dBasePort.len);
  if (kwaDot1dBasePort.rc != XLIBRC_SUCCESS)
  {
    kwaDot1dBasePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1dBasePort);
    return kwaDot1dBasePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwaDot1dBasePort.len);

  /* set the value in application */
  owa.l7rc = usmDbTimeSinceLastStatsResetGet (L7_UNIT_CURRENT, keyDot1dBasePortValue, &ts);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  sprintf(objtimeSinceCountersLastClearedValue,"%d day %d hr %d min %d sec", ts.days, ts.hours, ts.minutes, ts.seconds);
  

  /* return the object value: timeSinceCountersLastCleared */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtimeSinceCountersLastClearedValue,
                           strlen (objtimeSinceCountersLastClearedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1d_IntfStatsClear
*
* @purpose Set 'IntfStatsClear'
*
* @description [IntfStatsClear] clear stats on given interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1d_IntfStatsClear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfStatsClearValue;

  fpObjWa_t kwaDot1dBasePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfStatsClear */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIntfStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfStatsClearValue, owa.len);

  /* retrieve key: Dot1dBasePort */
  kwaDot1dBasePort.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dBasePort,
                                       (xLibU8_t *) & keyDot1dBasePortValue, &kwaDot1dBasePort.len);
  if (kwaDot1dBasePort.rc != XLIBRC_SUCCESS)
  {
    kwaDot1dBasePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1dBasePort);
    return kwaDot1dBasePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwaDot1dBasePort.len);

  /* set the value in application */
  owa.l7rc = usmDbIntfStatReset (L7_UNIT_CURRENT, keyDot1dBasePortValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dTpPortMaxInfo
*
* @purpose Get 'Dot1dTpPortMaxInfo'
*
* @description [Dot1dTpPortMaxInfo]: Max info of port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dTpPortMaxInfo (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpPortMaxInfoValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dTpPort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dTpPortMaxInfoGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                         &objDot1dTpPortMaxInfoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dTpPortMaxInfo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTpPortMaxInfoValue,
                           sizeof (objDot1dTpPortMaxInfoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1d_Dot1dTpPortOutFrames
*
* @purpose Get 'Dot1dTpPortOutFrames'
*
* @description [Dot1dTpPortOutFrames]: Port out frames 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dTpPortOutFrames (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpPortOutFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dTpPort,
                          (xLibU8_t *) & keyDot1dBasePortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1dTpPortOutFramesGet (L7_UNIT_CURRENT, keyDot1dBasePortValue,
                                  &objDot1dTpPortOutFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dTpPortOutFrames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTpPortOutFramesValue,
                           sizeof (objDot1dTpPortOutFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*****************************************************/
/**
* @brief
*
* @param wap
* @param bufp
*
* @returns XLIBRC_SUCCESS if succeeded to return value.
           XLIBRC_FAILURE if failed to return value.
*/

/* This is KEY Object */
xLibRC_t fpObjGet_SwitchingDot1d_Dot1dTpPort (void *wap, void *bufp)
{
  L7_int32 dot1dTpPort;
  xLibU16_t portLen=sizeof(dot1dTpPort);

   if (XLIBRC_NO_FILTER==xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dTpPort,(xLibU8_t *)&dot1dTpPort, &portLen))
   {
     dot1dTpPort=0;
     portLen=sizeof(dot1dTpPort);
   }
   while (usmDbGetNextVisibleIntIfNumber(dot1dTpPort, &dot1dTpPort) == L7_SUCCESS)
   {
       /* only return success if it's a valid interface for this table */
       if ((usmDbDot1dIsValidIntf(L7_USMDB_UNIT_ZERO, dot1dTpPort) == L7_TRUE)&&
           (usmDbDot1dBasePortEntryGet(L7_USMDB_UNIT_ZERO, dot1dTpPort)==L7_SUCCESS)&&
           (usmDbVisibleIntIfNumberCheck(L7_USMDB_UNIT_ZERO, dot1dTpPort)==L7_SUCCESS))
        {
            xLibBufDataSet (bufp, (xLibU8_t *) &dot1dTpPort, sizeof(dot1dTpPort));
            return XLIBRC_SUCCESS;
        }
   }

    return XLIBRC_ENDOF_TABLE;

}

/**
* @brief
*
* @param wap
* @param bufp
*
* @returns XLIBRC_SUCCESS if succeeded to return value.
           XLIBRC_FAILURE if failed to return value.
*/

xLibRC_t fpObjGet_SwitchingDot1d_Dot1dTpPortInDiscards (void *wap, void *bufp)
{
  L7_int32 dot1dTpPort;
  xLibU16_t portLen=sizeof(dot1dTpPort);
  L7_uint32 dot1dTpPortInDiscards;

   if (XLIBRC_SUCCESS!=xLibFilterGet (wap, XOBJ_SwitchingDot1d_Dot1dTpPort, (xLibU8_t *)&dot1dTpPort, &portLen))
   {
     XLIB_HWARN ("Filter Missing");
     return XLIBRC_FILTER_MISSING;
   }
   if (usmDbDot1dTpPortInDiscardsGet(L7_USMDB_UNIT_ZERO,dot1dTpPort,&dot1dTpPortInDiscards) == L7_SUCCESS)
   {
      return xLibBufDataSet (bufp, (xLibU8_t *)&dot1dTpPortInDiscards,sizeof(dot1dTpPortInDiscards));

   }
 return XLIBRC_FAILURE;
}


