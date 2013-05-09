
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingSnoopingIgmpGlobalConfig.c
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
#include "_xe_SwitchingSnoopingIgmpGlobalConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_snooping_api.h"

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpAdminMode
*
* @purpose Set 'SnoopingIgmpAdminMode'
 *@description  [SnoopingIgmpAdminMode] This enables or disables IGMP Snooping
* on a selected Vlan interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIgmpAdminModeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIgmpAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIgmpAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIgmpAdminModeValue, owa.len);

  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbSnoopIntfModeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                           objSnoopingIgmpAdminModeValue,
                           L7_AF_INET);
    if(owa.l7rc != L7_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return XLIBRC_FAILURE;
    } 
    objInterfaceValue = nextObjInterfaceValue;          
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                       &nextObjInterfaceValue);

  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpGroupMembershipInterval
*
* @purpose Set 'SnoopingIgmpGroupMembershipInterval'
 *@description  [SnoopingIgmpGroupMembershipInterval] The amount of time in
* seconds that a switch will wait for a report from a particular group
* on the selected vlan before deleting the interface participating
* in the vlan from the entry. This value must be greater than
* SnoopingIntfMaxResponseTime   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpGroupMembershipInterval (void *wap,
                                                                                         void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIgmpGroupMembershipIntervalValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIgmpGroupMembershipInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSnoopingIgmpGroupMembershipIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIgmpGroupMembershipIntervalValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfGroupMembershipIntervalSet (L7_UNIT_CURRENT,
                                              nextObjInterfaceValue,
                                              objSnoopingIgmpGroupMembershipIntervalValue,
                                              L7_AF_INET);

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
* @function fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpMaxResponseTime
*
* @purpose Set 'SnoopingIgmpMaxResponseTime'
 *@description  [SnoopingIgmpMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected vlan because
* it did not receive a report for a particular group in the
* interface participating in the vlan. This value must be less than
* SnoopingIntfGroupMembershipInte   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpMaxResponseTime (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIgmpMaxResponseTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIgmpMaxResponseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIgmpMaxResponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIgmpMaxResponseTimeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfResponseTimeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                            objSnoopingIgmpMaxResponseTimeValue,
                                            L7_AF_INET);

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
* @function fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpFastLeaveAdminMode
*
* @purpose Set 'SnoopingIgmpFastLeaveAdminMode'
 *@description  [SnoopingIgmpFastLeaveAdminMode] This enables or disables IGMP
* Snooping on the selected vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpFastLeaveAdminMode (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIgmpFastLeaveAdminModeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIgmpFastLeaveAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIgmpFastLeaveAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIgmpFastLeaveAdminModeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfFastLeaveAdminModeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                         objSnoopingIgmpFastLeaveAdminModeValue,
                                         L7_AF_INET);

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
* @function fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpMRPExpirationTime
*
* @purpose Set 'SnoopingIgmpMRPExpirationTime'
 *@description  [SnoopingIgmpMRPExpirationTime] The amount of time in seconds
* that a switch will wait for a query to be received on the selected
* interface participating in the vlan before the interface is
* removed from the list of interfaces with multicast routers attached.
* This parameter is configurabl   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIgmpGlobalConfig_SnoopingIgmpMRPExpirationTime (void *wap,
                                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIgmpMRPExpirationTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIgmpMRPExpirationTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIgmpMRPExpirationTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIgmpMRPExpirationTimeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    usmDbSnoopIntfMcastRtrExpiryTimeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                         objSnoopingIgmpMRPExpirationTimeValue,
                                         L7_AF_INET);

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
