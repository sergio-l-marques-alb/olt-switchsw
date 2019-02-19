
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimNetMgmtNotification.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimNetMgmtNotification_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterAddressType
*
* @purpose Get 'pimInvalidRegisterAddressType'
*
* @description [pimInvalidRegisterAddressType] The address type stored in pimInvalidRegisterOrigin, pimInvalidRegisterGroup, and pimInvalidRegisterRp. If no invalid Register messages have been received, then this object is set to unknown(0).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterAddressType (void *wap,
                                                                               void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidRegisterAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInvalidRegisterAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidRegisterAddressTypeValue,
                     sizeof (objpimInvalidRegisterAddressTypeValue));

  /* return the object value: pimInvalidRegisterAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInvalidRegisterAddressTypeValue,
                           sizeof (objpimInvalidRegisterAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterOrigin
*
* @purpose Get 'pimInvalidRegisterOrigin'
*
* @description [pimInvalidRegisterOrigin] The source address of the last invalid Register message received by this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterOrigin (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimInvalidRegisterOriginValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimInvalidRegisterOriginValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimInvalidRegisterOriginValue,
                     sizeof (objpimInvalidRegisterOriginValue));

  /* return the object value: pimInvalidRegisterOrigin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimInvalidRegisterOriginValue,
                           sizeof (objpimInvalidRegisterOriginValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterGroup
*
* @purpose Get 'pimInvalidRegisterGroup'
*
* @description [pimInvalidRegisterGroup] The IP multicast group address to which the last invalid Register message received by this device was addressed.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterGroup (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimInvalidRegisterGroupValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimInvalidRegisterGroupValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimInvalidRegisterGroupValue,
                     sizeof (objpimInvalidRegisterGroupValue));

  /* return the object value: pimInvalidRegisterGroup */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimInvalidRegisterGroupValue,
                           sizeof (objpimInvalidRegisterGroupValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterRp
*
* @purpose Get 'pimInvalidRegisterRp'
*
* @description [pimInvalidRegisterRp] The RP address to which the last invalid Register message received by this device was delivered.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidRegisterRp (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimInvalidRegisterRpValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimInvalidRegisterRpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimInvalidRegisterRpValue, sizeof (objpimInvalidRegisterRpValue));

  /* return the object value: pimInvalidRegisterRp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimInvalidRegisterRpValue,
                           sizeof (objpimInvalidRegisterRpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneNotificationPeriod
*
* @purpose Get 'pimInvalidJoinPruneNotificationPeriod'
*
* @description [pimInvalidJoinPruneNotificationPeriod] The minimum time that must elapse between pimInvalidJoinPrune notifications originated by this router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneNotificationPeriod (void *wap,
                                                                                       void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidJoinPruneNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInvalidJoinPruneNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidJoinPruneNotificationPeriodValue,
                     sizeof (objpimInvalidJoinPruneNotificationPeriodValue));

  /* return the object value: pimInvalidJoinPruneNotificationPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInvalidJoinPruneNotificationPeriodValue,
                           sizeof (objpimInvalidJoinPruneNotificationPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneNotificationPeriod
*
* @purpose Set 'pimInvalidJoinPruneNotificationPeriod'
*
* @description [pimInvalidJoinPruneNotificationPeriod] The minimum time that must elapse between pimInvalidJoinPrune notifications originated by this router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneNotificationPeriod (void *wap,
                                                                                       void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidJoinPruneNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInvalidJoinPruneNotificationPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objpimInvalidJoinPruneNotificationPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidJoinPruneNotificationPeriodValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimInvalidJoinPruneNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneMsgsRcvd
*
* @purpose Get 'pimInvalidJoinPruneMsgsRcvd'
*
* @description [pimInvalidJoinPruneMsgsRcvd] The number of invalid PIM Join/Prune messages that have been received by this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneMsgsRcvd (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidJoinPruneMsgsRcvdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInvalidJoinPruneMsgsRcvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidJoinPruneMsgsRcvdValue,
                     sizeof (objpimInvalidJoinPruneMsgsRcvdValue));

  /* return the object value: pimInvalidJoinPruneMsgsRcvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInvalidJoinPruneMsgsRcvdValue,
                           sizeof (objpimInvalidJoinPruneMsgsRcvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneAddressType
*
* @purpose Get 'pimInvalidJoinPruneAddressType'
*
* @description [pimInvalidJoinPruneAddressType] The address type stored in pimInvalidJoinPruneOrigin, pimInvalidJoinPruneGroup, and pimInvalidJoinPruneRp. If no invalid Join/Prune messages have been received, this object is set to unknown(0).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneAddressType (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidJoinPruneAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInvalidJoinPruneAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidJoinPruneAddressTypeValue,
                     sizeof (objpimInvalidJoinPruneAddressTypeValue));

  /* return the object value: pimInvalidJoinPruneAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInvalidJoinPruneAddressTypeValue,
                           sizeof (objpimInvalidJoinPruneAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneOrigin
*
* @purpose Get 'pimInvalidJoinPruneOrigin'
*
* @description [pimInvalidJoinPruneOrigin] The source address of the last invalid Join/Prune message received by this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneOrigin (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimInvalidJoinPruneOriginValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimInvalidJoinPruneOriginValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimInvalidJoinPruneOriginValue,
                     sizeof (objpimInvalidJoinPruneOriginValue));

  /* return the object value: pimInvalidJoinPruneOrigin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimInvalidJoinPruneOriginValue,
                           sizeof (objpimInvalidJoinPruneOriginValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneGroup
*
* @purpose Get 'pimInvalidJoinPruneGroup'
*
* @description [pimInvalidJoinPruneGroup] The IP multicast group address carried in the last invalid Join/Prune message received by this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneGroup (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimInvalidJoinPruneGroupValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimInvalidJoinPruneGroupValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimInvalidJoinPruneGroupValue,
                     sizeof (objpimInvalidJoinPruneGroupValue));

  /* return the object value: pimInvalidJoinPruneGroup */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimInvalidJoinPruneGroupValue,
                           sizeof (objpimInvalidJoinPruneGroupValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneRp
*
* @purpose Get 'pimInvalidJoinPruneRp'
*
* @description [pimInvalidJoinPruneRp] The RP address carried in the last invalid Join/Prune message received by this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInvalidJoinPruneRp (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimInvalidJoinPruneRpValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimInvalidJoinPruneRpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimInvalidJoinPruneRpValue, sizeof (objpimInvalidJoinPruneRpValue));

  /* return the object value: pimInvalidJoinPruneRp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimInvalidJoinPruneRpValue,
                           sizeof (objpimInvalidJoinPruneRpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimRPMappingNotificationPeriod
*
* @purpose Get 'pimRPMappingNotificationPeriod'
*
* @description [pimRPMappingNotificationPeriod] The minimum time that must elapse between pimRPMappingChange notifications originated by this router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimRPMappingNotificationPeriod (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRPMappingNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimRPMappingNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimRPMappingNotificationPeriodValue,
                     sizeof (objpimRPMappingNotificationPeriodValue));

  /* return the object value: pimRPMappingNotificationPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimRPMappingNotificationPeriodValue,
                           sizeof (objpimRPMappingNotificationPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimNetMgmtNotification_pimRPMappingNotificationPeriod
*
* @purpose Set 'pimRPMappingNotificationPeriod'
*
* @description [pimRPMappingNotificationPeriod] The minimum time that must elapse between pimRPMappingChange notifications originated by this router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimNetMgmtNotification_pimRPMappingNotificationPeriod (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRPMappingNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimRPMappingNotificationPeriod */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimRPMappingNotificationPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimRPMappingNotificationPeriodValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimRPMappingNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimRPMappingChangeCount
*
* @purpose Get 'pimRPMappingChangeCount'
*
* @description [pimRPMappingChangeCount] The number of changes to active RP mappings on this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimRPMappingChangeCount (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRPMappingChangeCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimRPMappingChangeCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimRPMappingChangeCountValue,
                     sizeof (objpimRPMappingChangeCountValue));

  /* return the object value: pimRPMappingChangeCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimRPMappingChangeCountValue,
                           sizeof (objpimRPMappingChangeCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInterfaceElectionNotificationPeriod
*
* @purpose Get 'pimInterfaceElectionNotificationPeriod'
*
* @description [pimInterfaceElectionNotificationPeriod] The minimum time that must elapse between pimInterfaceElection notifications originated by this router. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInterfaceElectionNotificationPeriod (void *wap,
                                                                                        void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceElectionNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInterfaceElectionNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceElectionNotificationPeriodValue,
                     sizeof (objpimInterfaceElectionNotificationPeriodValue));

  /* return the object value: pimInterfaceElectionNotificationPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceElectionNotificationPeriodValue,
                           sizeof (objpimInterfaceElectionNotificationPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimNetMgmtNotification_pimInterfaceElectionNotificationPeriod
*
* @purpose Set 'pimInterfaceElectionNotificationPeriod'
*
* @description [pimInterfaceElectionNotificationPeriod] The minimum time that must elapse between pimInterfaceElection notifications originated by this router. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimNetMgmtNotification_pimInterfaceElectionNotificationPeriod (void *wap,
                                                                                        void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceElectionNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceElectionNotificationPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objpimInterfaceElectionNotificationPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceElectionNotificationPeriodValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimInterfaceElectionNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimInterfaceElectionWinCount
*
* @purpose Get 'pimInterfaceElectionWinCount'
*
* @description [pimInterfaceElectionWinCount] The number of times this device has been elected DR or DF on any interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimInterfaceElectionWinCount (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceElectionWinCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInterfaceElectionWinCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceElectionWinCountValue,
                     sizeof (objpimInterfaceElectionWinCountValue));

  /* return the object value: pimInterfaceElectionWinCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceElectionWinCountValue,
                           sizeof (objpimInterfaceElectionWinCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNetMgmtNotification_pimRefreshInterval
*
* @purpose Get 'pimRefreshInterval'
*
* @description [pimRefreshInterval] The interval between successive State Refresh messages sent by an Originator. This timer period is called the RefreshInterval in the PIM-DM specification. This object is used only by PIM-DM.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNetMgmtNotification_pimRefreshInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRefreshIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimRefreshIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimRefreshIntervalValue, sizeof (objpimRefreshIntervalValue));

  /* return the object value: pimRefreshInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimRefreshIntervalValue,
                           sizeof (objpimRefreshIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimNetMgmtNotification_pimRefreshInterval
*
* @purpose Set 'pimRefreshInterval'
*
* @description [pimRefreshInterval] The interval between successive State Refresh messages sent by an Originator. This timer period is called the RefreshInterval in the PIM-DM specification. This object is used only by PIM-DM.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimNetMgmtNotification_pimRefreshInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRefreshIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimRefreshInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimRefreshIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimRefreshIntervalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimRefreshIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
