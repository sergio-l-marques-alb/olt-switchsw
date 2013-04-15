
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingSnoopingMldGlobalConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  19 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingSnoopingMldGlobalConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_snooping_api.h"

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldAdminMode
*
* @purpose Set 'SnoopingMldAdminMode'
 *@description  [SnoopingMldAdminMode] This enables or disables Mld Snooping on
* a selected Vlan interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingMldAdminModeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingMldAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingMldAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingMldAdminModeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfModeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                           objSnoopingMldAdminModeValue,
                           L7_AF_INET6);

    objInterfaceValue = nextObjInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                       &nextObjInterfaceValue);

  }
#if 0
  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldGroupMembershipInterval
*
* @purpose Set 'SnoopingMldGroupMembershipInterval'
 *@description  [SnoopingMldGroupMembershipInterval] The amount of time in
* seconds that a switch will wait for a report from a particular group
* on the selected vlan before deleting the interface participating
* in the vlan from the entry. This value must be greater than
* SnoopingIntfMaxResponseTime   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldGroupMembershipInterval (void *wap,
                                                                                       void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingMldGroupMembershipIntervalValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingMldGroupMembershipInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSnoopingMldGroupMembershipIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingMldGroupMembershipIntervalValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {

    usmDbSnoopIntfGroupMembershipIntervalSet (L7_UNIT_CURRENT,
                                              nextObjInterfaceValue,
                                              objSnoopingMldGroupMembershipIntervalValue,
                                              L7_AF_INET6);

 
    objInterfaceValue = nextObjInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                       &nextObjInterfaceValue);

  }
#if 0
  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldMaxResponseTime
*
* @purpose Set 'SnoopingMldMaxResponseTime'
 *@description  [SnoopingMldMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected vlan because
* it did not receive a report for a particular group in the
* interface participating in the vlan. This value must be less than
* SnoopingIntfGroupMembershipInte   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldMaxResponseTime (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingMldMaxResponseTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingMldMaxResponseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingMldMaxResponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingMldMaxResponseTimeValue, owa.len);

  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfResponseTimeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                            objSnoopingMldMaxResponseTimeValue,
                                            L7_AF_INET6);

    objInterfaceValue = nextObjInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                       &nextObjInterfaceValue);

  }
#if 0
  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objSnoopingIgmpAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldFastLeaveAdminMode
*
* @purpose Set 'SnoopingMldFastLeaveAdminMode'
 *@description  [SnoopingMldFastLeaveAdminMode] This enables or disables IGMP
* Snooping on the selected vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldFastLeaveAdminMode (void *wap,
                                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingMldFastLeaveAdminModeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingMldFastLeaveAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingMldFastLeaveAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingMldFastLeaveAdminModeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfFastLeaveAdminModeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                         objSnoopingMldFastLeaveAdminModeValue,
                                         L7_AF_INET6);

    objInterfaceValue = nextObjInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                       &nextObjInterfaceValue);

  }
#if 0
  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objSnoopingIgmpAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldMRPExpirationTime
*
* @purpose Set 'SnoopingMldMRPExpirationTime'
 *@description  [SnoopingMldMRPExpirationTime] The amount of time in seconds
* that a switch will wait for a query to be received on the selected
* interface participating in the vlan before the interface is
* removed from the list of interfaces with multicast routers attached.
* This parameter is configurabl   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingMldGlobalConfig_SnoopingMldMRPExpirationTime (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingMldMRPExpirationTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingMldMRPExpirationTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingMldMRPExpirationTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingMldMRPExpirationTimeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfMcastRtrExpiryTimeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                         objSnoopingMldMRPExpirationTimeValue,
                                         L7_AF_INET6);

    objInterfaceValue = nextObjInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                       &nextObjInterfaceValue);

  }
#if 0
  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objSnoopingIgmpAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
