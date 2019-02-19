
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingSnoopingVlanGroup.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  24 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingSnoopingVlanGroup_obj.h"
#include "usmdb_snooping_api.h"
#include "dot1q_exports.h"

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_dot1qVlanIndex
*
* @purpose Get 'dot1qVlanIndex'
 *@description  [dot1qVlanIndex] The VLAN-ID or other identifier refering to
* this VLAN   
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_dot1qVlanIndex (void *wap, void *bufp)
{

  xLibU32_t objSnoopingVlanAdminModeValue = 0;
  xLibU32_t objdot1qVlanIndexValue;
  xLibU32_t nextObjdot1qVlanIndexValue;
  xLibU32_t objSnoopingProtocolValue;
  xLibU32_t vlanId;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                          (xLibU8_t *) & objSnoopingProtocolValue, &owa.len);

  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: dot1qVlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                          (xLibU8_t *) & objdot1qVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
     
    objdot1qVlanIndexValue = 0;

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1qVlanIndexValue, owa.len);
  }

  for( vlanId = 1+objdot1qVlanIndexValue; vlanId < L7_DOT1Q_MAX_VLAN_ID; vlanId++)
  {
      
     owa.l7rc = usmDbSnoopVlanModeGet(L7_UNIT_CURRENT, vlanId,
                              &objSnoopingVlanAdminModeValue,objSnoopingProtocolValue);
     if((objSnoopingVlanAdminModeValue == L7_ENABLE)&&(owa.l7rc==L7_SUCCESS))
     {
        nextObjdot1qVlanIndexValue = vlanId;
        break;
     } 
        
  }
  if(vlanId == vlanId)
  {
   vlanId = vlanId;
  }
  if ((owa.l7rc != L7_SUCCESS) ||(vlanId >= L7_DOT1Q_MAX_VLAN_ID))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1qVlanIndexValue, owa.len);

  /* return the object value: dot1qVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1qVlanIndexValue,
                           sizeof (objdot1qVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_SnoopingProtocol
*
* @purpose Get 'SnoopingProtocol'
 *@description  [SnoopingProtocol] The protocol type of network protocol in use   
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_SnoopingProtocol (void *wap, void *bufp)
{

  xLibU32_t objSnoopingProtocolValue;
  xLibU32_t nextObjSnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

#if 0
  /* retrieve key: dot1qVlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                          (xLibU8_t *) & objdot1qVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1qVlanIndexValue, owa.len);

#endif
  /* retrieve key: SnoopingProtocol */
  owa.l7rc = L7_SUCCESS;
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                          (xLibU8_t *) & objSnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjSnoopingProtocolValue = L7_AF_INET;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSnoopingProtocolValue, owa.len);
    if( objSnoopingProtocolValue == L7_AF_INET)
       nextObjSnoopingProtocolValue = L7_AF_INET6;
    else
       owa.l7rc = L7_FAILURE;  

  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSnoopingProtocolValue, owa.len);

  /* return the object value: SnoopingProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSnoopingProtocolValue,
                           sizeof (objSnoopingProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanAdminMode
*
* @purpose Get 'SnoopingVlanAdminMode'
 *@description  [SnoopingVlanAdminMode] This enables or disables IGMP Snooping
* on a selected Vlan interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanAdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopVlanModeGet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                              &objSnoopingVlanAdminModeValue,keySnoopingProtocolValue);
  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingVlanAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingVlanAdminModeValue,
                           sizeof (objSnoopingVlanAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanAdminMode
*
* @purpose Set 'SnoopingVlanAdminMode'
 *@description  [SnoopingVlanAdminMode] This enables or disables IGMP Snooping
* on a selected Vlan interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanAdminModeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingVlanAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingVlanAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingVlanAdminModeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopVlanModeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               objSnoopingVlanAdminModeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanCfgDelete
*
* @purpose Set 'SnoopingVlanCfgDelete'
 *@description  [SnoopingVlanCfgDelete] This disables snooping
* on a selected Vlan interface and defaults all the snooping configuration related
* to that VLAN
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanCfgDelete (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanAdminModeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingVlanAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingVlanAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingVlanAdminModeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopVlanModeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               objSnoopingVlanAdminModeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  /* Fast Leave Mode */
   owa.l7rc = usmDbSnoopVlanFastLeaveModeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                                            keySnoopingProtocolValue == L7_AF_INET ? FD_IGMP_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE:
                                            FD_MLD_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  /* Group membership interval */
  owa.l7rc = usmDbSnoopVlanGroupMembershipIntervalSet (L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                                            keySnoopingProtocolValue == L7_AF_INET ? FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL:
                                            FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  /* Max Response time */
    owa.l7rc = usmDbSnoopVlanMaximumResponseTimeSet (L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                                            keySnoopingProtocolValue == L7_AF_INET ? FD_IGMP_SNOOPING_MAX_RESPONSE_TIME:
                                            FD_MLD_SNOOPING_MAX_RESPONSE_TIME, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  /* Mcast router expiry time */
   owa.l7rc = usmDbSnoopVlanMcastRtrExpiryTimeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                                            keySnoopingProtocolValue == L7_AF_INET ? FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME:
                                            FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  else
  {
    (void)xLibFilterClear(wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex);
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanGroupMembershipInterval
*
* @purpose Get 'SnoopingVlanGroupMembershipInterval'
 *@description  [SnoopingVlanGroupMembershipInterval] The amount of time in
* seconds that a switch will wait for a report from a particular group
* on the selected vlan before deleting the interface participating
* in the vlan from the entry. This value must be greater than
* SnoopingIntfMaxResponseTime   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanGroupMembershipInterval (void *wap,
                                                                                  void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanGroupMembershipIntervalValue;
  L7_ushort16 objSnoopingVlanGroupMembershipIntervalValueShort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopVlanGroupMembershipIntervalGet (L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                              (xLibU16_t*) &objSnoopingVlanGroupMembershipIntervalValueShort, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objSnoopingVlanGroupMembershipIntervalValue = objSnoopingVlanGroupMembershipIntervalValueShort;

  /* return the object value: SnoopingVlanGroupMembershipInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingVlanGroupMembershipIntervalValue,
                    sizeof (objSnoopingVlanGroupMembershipIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanGroupMembershipInterval
*
* @purpose Set 'SnoopingVlanGroupMembershipInterval'
 *@description  [SnoopingVlanGroupMembershipInterval] The amount of time in
* seconds that a switch will wait for a report from a particular group
* on the selected vlan before deleting the interface participating
* in the vlan from the entry. This value must be greater than
* SnoopingIntfMaxResponseTime   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanGroupMembershipInterval (void *wap,
                                                                                  void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanGroupMembershipIntervalValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingVlanGroupMembershipInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSnoopingVlanGroupMembershipIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingVlanGroupMembershipIntervalValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);
  
   if(keySnoopingProtocolValue ==  L7_AF_INET)
  {
    if( (objSnoopingVlanGroupMembershipIntervalValue < L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL)
       || (objSnoopingVlanGroupMembershipIntervalValue > L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL))
    {
      return  XLIBRC_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL_INVALID;

    }

  }
  else if(keySnoopingProtocolValue == L7_AF_INET6)
  {
    if( (objSnoopingVlanGroupMembershipIntervalValue < L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL)
       || (objSnoopingVlanGroupMembershipIntervalValue > L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL))
    {
      return  XLIBRC_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL_INVALID;
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }

  /* set the value in application */
  owa.l7rc = usmDbSnoopVlanGroupMembershipIntervalSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                           objSnoopingVlanGroupMembershipIntervalValue,
                              keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    switch(owa.l7rc)
    {
      case L7_REQUEST_DENIED:
         owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
      default:
         owa.rc = XLIBRC_SNOOPING_HOST_TIMEOUT_SET_ERR;    /* TODO: Change if required */
      break;
    }
  }

 FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanMaxResponseTime
*
* @purpose Get 'SnoopingVlanMaxResponseTime'
 *@description  [SnoopingVlanMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected vlan because
* it did not receive a report for a particular group in the
* interface participating in the vlan. This value must be less than
* SnoopingIntfGroupMembershipInte   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanMaxResponseTime (void *wap, void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanMaxResponseTimeValue;
  L7_ushort16 objSnoopingVlanMaxResponseTimeValueShort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopVlanMaximumResponseTimeGet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               (xLibU16_t*) &objSnoopingVlanMaxResponseTimeValueShort, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  objSnoopingVlanMaxResponseTimeValue = objSnoopingVlanMaxResponseTimeValueShort;

  /* return the object value: SnoopingVlanMaxResponseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingVlanMaxResponseTimeValue,
                    sizeof (objSnoopingVlanMaxResponseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanMaxResponseTime
*
* @purpose Set 'SnoopingVlanMaxResponseTime'
 *@description  [SnoopingVlanMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected vlan because
* it did not receive a report for a particular group in the
* interface participating in the vlan. This value must be less than
* SnoopingIntfGroupMembershipInte   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanMaxResponseTime (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanMaxResponseTimeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingVlanMaxResponseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingVlanMaxResponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingVlanMaxResponseTimeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  if(keySnoopingProtocolValue ==  L7_AF_INET)
  {
    if( (objSnoopingVlanMaxResponseTimeValue < L7_IGMP_SNOOPING_MIN_RESPONSE_TIME )
       || (objSnoopingVlanMaxResponseTimeValue > L7_IGMP_SNOOPING_MAX_RESPONSE_TIME))
    {
      return  XLIBRC_IGMP_SNOOPING_MAX_RESP_TIME_INVALID;

    }

  }
  else if(keySnoopingProtocolValue == L7_AF_INET6)
  {
    if( (objSnoopingVlanMaxResponseTimeValue < L7_MLD_SNOOPING_MIN_RESPONSE_TIME )
       || (objSnoopingVlanMaxResponseTimeValue > L7_MLD_SNOOPING_MAX_RESPONSE_TIME))
    {
      return  XLIBRC_MLD_SNOOPING_MAX_RESP_TIME_INVALID;
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }
  
  /* set the value in application */
  owa.l7rc = usmDbSnoopVlanMaximumResponseTimeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               objSnoopingVlanMaxResponseTimeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    switch(owa.l7rc)
    {
      case L7_REQUEST_DENIED:
         owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
      default:
         owa.rc = XLIBRC_SNOOPING_LEAVE_TIMEOUT_SET_ERR;  /* TODO: Change if required */
      break;
    }
  }

 FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanFastLeaveAdminMode
*
* @purpose Get 'SnoopingVlanFastLeaveAdminMode'
 *@description  [SnoopingVlanFastLeaveAdminMode] This enables or disables IGMP
* Snooping on the selected vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanFastLeaveAdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanFastLeaveAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopVlanFastLeaveModeGet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               &objSnoopingVlanFastLeaveAdminModeValue,keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingVlanFastLeaveAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingVlanFastLeaveAdminModeValue,
                    sizeof (objSnoopingVlanFastLeaveAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanFastLeaveAdminMode
*
* @purpose Set 'SnoopingVlanFastLeaveAdminMode'
 *@description  [SnoopingVlanFastLeaveAdminMode] This enables or disables IGMP
* Snooping on the selected vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanFastLeaveAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanFastLeaveAdminModeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingVlanFastLeaveAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingVlanFastLeaveAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingVlanFastLeaveAdminModeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopVlanFastLeaveModeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                              objSnoopingVlanFastLeaveAdminModeValue,keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanMRPExpirationTime
*
* @purpose Get 'SnoopingVlanMRPExpirationTime'
 *@description  [SnoopingVlanMRPExpirationTime] The amount of time in seconds
* that a switch will wait for a query to be received on the selected
* interface participating in the vlan before the interface is
* removed from the list of interfaces with multicast routers attached.
* This parameter is configurabl   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingVlanGroup_SnoopingVlanMRPExpirationTime (void *wap, void *bufp)
{

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanMRPExpirationTimeValue;
  L7_ushort16 objSnoopingVlanMRPExpirationTimeValueShort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopVlanMcastRtrExpiryTimeGet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               (xLibU16_t*) &objSnoopingVlanMRPExpirationTimeValueShort, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objSnoopingVlanMRPExpirationTimeValue = objSnoopingVlanMRPExpirationTimeValueShort;

  /* return the object value: SnoopingVlanMRPExpirationTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingVlanMRPExpirationTimeValue,
                    sizeof (objSnoopingVlanMRPExpirationTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanMRPExpirationTime
*
* @purpose Set 'SnoopingVlanMRPExpirationTime'
 *@description  [SnoopingVlanMRPExpirationTime] The amount of time in seconds
* that a switch will wait for a query to be received on the selected
* interface participating in the vlan before the interface is
* removed from the list of interfaces with multicast routers attached.
* This parameter is configurabl   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingVlanGroup_SnoopingVlanMRPExpirationTime (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingVlanMRPExpirationTimeValue;

  fpObjWa_t kwadot1qVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1qVlanIndexValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingVlanMRPExpirationTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingVlanMRPExpirationTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingVlanMRPExpirationTimeValue, owa.len);

  /* retrieve key: dot1qVlanIndex */
  kwadot1qVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_dot1qVlanIndex,
                                        (xLibU8_t *) & keydot1qVlanIndexValue,
                                        &kwadot1qVlanIndex.len);
  if (kwadot1qVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwadot1qVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1qVlanIndex);
    return kwadot1qVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1qVlanIndexValue, kwadot1qVlanIndex.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingVlanGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

  if(keySnoopingProtocolValue ==  L7_AF_INET)
  {
    if( (objSnoopingVlanMRPExpirationTimeValue < L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME )
       || (objSnoopingVlanMRPExpirationTimeValue > L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME))
    {
      return  XLIBRC_IGMP_SNOOPING_MAX_MCAST_MAX_TIME_INVALID;

    }

  }
  else if(keySnoopingProtocolValue == L7_AF_INET6)
  {
    if( (objSnoopingVlanMRPExpirationTimeValue < L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME )
       || (objSnoopingVlanMRPExpirationTimeValue > L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME))
    {
      return  XLIBRC_MLD_SNOOPING_MAX_MCAST_MAX_TIME_INVALID;
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }

  /* set the value in application */
  owa.l7rc = usmDbSnoopVlanMcastRtrExpiryTimeSet(L7_UNIT_CURRENT, keydot1qVlanIndexValue,
                               objSnoopingVlanMRPExpirationTimeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
