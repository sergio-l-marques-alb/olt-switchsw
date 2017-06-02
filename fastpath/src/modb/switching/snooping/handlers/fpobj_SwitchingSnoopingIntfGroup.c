
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingSnoopingIntfGroup.c
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
#include "_xe_SwitchingSnoopingIntfGroup_obj.h"
#include "usmdb_snooping_api.h"
#include "dot1q_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dot3ad_api.h"

L7_RC_t
fpObjSnoopNextIntfValidate(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum ,
                          L7_uint32 family, L7_uint32 *nextFamily)
{

  L7_uint32 intIfNum;
  L7_uchar8 tmpFamily;

  tmpFamily = (L7_uchar8)family;

  if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)

  {
    /* go to next valid family */
    if (usmDbSnoopProtocolNextGet(tmpFamily, &tmpFamily) == L7_TRUE)
    {
      *nextExtIfNum = extIfNum;
      *nextFamily = tmpFamily;
       return L7_SUCCESS;
    }
  }

 /* get the first family and then get the next valid interface */
 if (usmDbSnoopProtocolNextGet(0, &tmpFamily) == L7_TRUE)
 {
   *nextFamily = tmpFamily;
   /* check to see if this is a visible external interface number */
   if (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
    {
      return usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum);
    }
 }
  *nextFamily = 0;
  return L7_FAILURE;
}


/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Specifies all configurable interfaces for DHCP
*              Filtering. 
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_Interface (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &objInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0,objInterfaceValue, &objInterfaceValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingProtocol
*
* @purpose Get 'SnoopingProtocol'
 *@description  [SnoopingProtocol] The protocol type of network protocol in use   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingProtocol (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t objSnoopingProtocolValue;
  xLibU32_t nextObjSnoopingProtocolValue;
  xLibU8_t tempObjSnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                          (xLibU8_t *) & objSnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objSnoopingProtocolValue, 0, sizeof (objSnoopingProtocolValue));
    if( usmDbSnoopProtocolNextGet(0, &tempObjSnoopingProtocolValue) != L7_TRUE)
    {
       owa.l7rc = L7_FAILURE;
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSnoopingProtocolValue, owa.len);
    tempObjSnoopingProtocolValue = objSnoopingProtocolValue;
    if(usmDbSnoopProtocolNextGet(tempObjSnoopingProtocolValue, &tempObjSnoopingProtocolValue)!= L7_TRUE)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
       owa.l7rc = L7_SUCCESS;
    }
  
  }

  if (owa.l7rc != L7_SUCCESS)
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
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfAdminMode
*
* @purpose Get 'SnoopingIntfAdminMode'
 *@description  [SnoopingIntfAdminMode] This enables or disables IGMP Snooping
* on a selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfAdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                              &objSnoopingIntfAdminModeValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingIntfAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingIntfAdminModeValue,
                           sizeof (objSnoopingIntfAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfAdminMode
*
* @purpose Set 'SnoopingIntfAdminMode'
 *@description  [SnoopingIntfAdminMode] This enables or disables IGMP Snooping
* on a selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfAdminModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIntfAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIntfAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIntfAdminModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                              objSnoopingIntfAdminModeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfGroupMembershipInterval
*
* @purpose Get 'SnoopingIntfGroupMembershipInterval'
 *@description  [SnoopingIntfGroupMembershipInterval] The amount of time in
* seconds that a switch will wait for a report from a particular group
* on the selected interface before deleting the interface from the
* entry. This value must be greater than
* SnoopingIntfMaxResponseTime.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfGroupMembershipInterval (void *wap,
                                                                                  void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfGroupMembershipIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfGroupMembershipIntervalGet (L7_UNIT_CURRENT, keyInterfaceValue,
                              &objSnoopingIntfGroupMembershipIntervalValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingIntfGroupMembershipInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingIntfGroupMembershipIntervalValue,
                    sizeof (objSnoopingIntfGroupMembershipIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfGroupMembershipInterval
*
* @purpose Set 'SnoopingIntfGroupMembershipInterval'
 *@description  [SnoopingIntfGroupMembershipInterval] The amount of time in
* seconds that a switch will wait for a report from a particular group
* on the selected interface before deleting the interface from the
* entry. This value must be greater than
* SnoopingIntfMaxResponseTime.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfGroupMembershipInterval (void *wap,
                                                                                  void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfGroupMembershipIntervalValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owaMaxRespTime = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMaxResponseTimeValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIntfGroupMembershipInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSnoopingIntfGroupMembershipIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIntfGroupMembershipIntervalValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
    if( (objSnoopingIntfGroupMembershipIntervalValue < L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL)
       || (objSnoopingIntfGroupMembershipIntervalValue > L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL))
    {
      return  XLIBRC_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL_INVALID;

    }

  }
  else if(keySnoopingProtocolValue == L7_AF_INET6)
  {
    if( (objSnoopingIntfGroupMembershipIntervalValue < L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL)
       || (objSnoopingIntfGroupMembershipIntervalValue > L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL))
    {
      return  XLIBRC_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL_INVALID;
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }

  /* retrieve object: SnoopingIntfMaxResponseTime */
  owaMaxRespTime.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingIntfMaxResponseTime,(xLibU8_t *) & objSnoopingIntfMaxResponseTimeValue, &owaMaxRespTime.len);
  if (owaMaxRespTime.rc == XLIBRC_SUCCESS)
  {
    if (objSnoopingIntfMaxResponseTimeValue >= objSnoopingIntfGroupMembershipIntervalValue)
    {
      owaMaxRespTime.rc = XLIBRC_SNOOPING_HOST_TIMEOUT_SET_ERR;
      FPOBJ_TRACE_EXIT (bufp, owaMaxRespTime);
      return owaMaxRespTime.rc;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfMaxResponseTime
*
* @purpose Get 'SnoopingIntfMaxResponseTime'
 *@description  [SnoopingIntfMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected interface
* because it did not receive a report for a particular group in that
* interface. This value must be less than
* SnoopingIntfGroupMembershipInterval.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfMaxResponseTime (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMaxResponseTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfResponseTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                               &objSnoopingIntfMaxResponseTimeValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingIntfMaxResponseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingIntfMaxResponseTimeValue,
                    sizeof (objSnoopingIntfMaxResponseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfMaxResponseTime
*
* @purpose Set 'SnoopingIntfMaxResponseTime'
 *@description  [SnoopingIntfMaxResponseTime] The amount of time in seconds a
* switch will wait after sending a query on the selected interface
* because it did not receive a report for a particular group in that
* interface. This value must be less than
* SnoopingIntfGroupMembershipInterval.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfMaxResponseTime (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMaxResponseTimeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owaGrpMemIntvl = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfGroupMembershipIntervalValue = 0;
  xLibU32_t objAppnSnoopingIntfMaxResponseTimeValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIntfMaxResponseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIntfMaxResponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIntfMaxResponseTimeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
    if( (objSnoopingIntfMaxResponseTimeValue < L7_IGMP_SNOOPING_MIN_RESPONSE_TIME )
       || (objSnoopingIntfMaxResponseTimeValue > L7_IGMP_SNOOPING_MAX_RESPONSE_TIME))
    {
      return  XLIBRC_IGMP_SNOOPING_MAX_RESP_TIME_INVALID;

    }

  }
  else if(keySnoopingProtocolValue == L7_AF_INET6)
  {
    if( (objSnoopingIntfMaxResponseTimeValue < L7_MLD_SNOOPING_MIN_RESPONSE_TIME )
       || (objSnoopingIntfMaxResponseTimeValue > L7_MLD_SNOOPING_MAX_RESPONSE_TIME))
    {
      return  XLIBRC_MLD_SNOOPING_MAX_RESP_TIME_INVALID;
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }

  /* retrieve object: SnoopingIntfMaxResponseTime */
  owaGrpMemIntvl.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingIntfGroupMembershipInterval,(xLibU8_t *) & objSnoopingIntfGroupMembershipIntervalValue, &owaGrpMemIntvl.len);
  if (owaGrpMemIntvl.rc == XLIBRC_SUCCESS)
  {
    /* get the values from application */
    if (usmDbSnoopIntfResponseTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                  &objAppnSnoopingIntfMaxResponseTimeValue, keySnoopingProtocolValue)
               == L7_SUCCESS)
    {
      if (objAppnSnoopingIntfMaxResponseTimeValue >= objSnoopingIntfGroupMembershipIntervalValue)
      {
        /* set the value in application */
        if ((owa.l7rc = usmDbSnoopIntfResponseTimeSet(L7_UNIT_CURRENT, keyInterfaceValue,
                                  objSnoopingIntfMaxResponseTimeValue, keySnoopingProtocolValue))
                              ==  L7_SUCCESS)
        {
          owa.l7rc = usmDbSnoopIntfGroupMembershipIntervalSet(L7_UNIT_CURRENT, keyInterfaceValue,
                                   objSnoopingIntfGroupMembershipIntervalValue, keySnoopingProtocolValue);
        }
      }
      else
      {
        /* set the value in application */
        if ((owa.l7rc = usmDbSnoopIntfGroupMembershipIntervalSet(L7_UNIT_CURRENT, keyInterfaceValue,
                                  objSnoopingIntfGroupMembershipIntervalValue, keySnoopingProtocolValue))
                              ==  L7_SUCCESS)
        {
          owa.l7rc = usmDbSnoopIntfResponseTimeSet(L7_UNIT_CURRENT, keyInterfaceValue,
                                   objSnoopingIntfMaxResponseTimeValue, keySnoopingProtocolValue);
        }
      }
    }
  }

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
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfMRPExpirationTime
*
* @purpose Get 'SnoopingIntfMRPExpirationTime'
 *@description  [SnoopingIntfMRPExpirationTime] The amount of time in seconds
* that a switch will wait for a query to be received on the selected
* interface before the interface is removed from the list of
* interfaces with multicast routers attached.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfMRPExpirationTime (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMRPExpirationTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfMcastRtrExpiryTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                               &objSnoopingIntfMRPExpirationTimeValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingIntfMRPExpirationTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingIntfMRPExpirationTimeValue,
                    sizeof (objSnoopingIntfMRPExpirationTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfMRPExpirationTime
*
* @purpose Set 'SnoopingIntfMRPExpirationTime'
 *@description  [SnoopingIntfMRPExpirationTime] The amount of time in seconds
* that a switch will wait for a query to be received on the selected
* interface before the interface is removed from the list of
* interfaces with multicast routers attached.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfMRPExpirationTime (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMRPExpirationTimeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIntfMRPExpirationTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIntfMRPExpirationTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIntfMRPExpirationTimeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
    if( (objSnoopingIntfMRPExpirationTimeValue < L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME )
       || (objSnoopingIntfMRPExpirationTimeValue > L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME))
    {
      return  XLIBRC_IGMP_SNOOPING_MAX_MCAST_MAX_TIME_INVALID;

    }
 
  }
  else if(keySnoopingProtocolValue == L7_AF_INET6)
  {
    if( (objSnoopingIntfMRPExpirationTimeValue < L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME )
       || (objSnoopingIntfMRPExpirationTimeValue > L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME))
    {
      return  XLIBRC_MLD_SNOOPING_MAX_MCAST_MAX_TIME_INVALID;
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  } 
  /* set the value in application */
  owa.l7rc = usmDbSnoopIntfMcastRtrExpiryTimeSet(L7_UNIT_CURRENT, keyInterfaceValue,
                               objSnoopingIntfMRPExpirationTimeValue,keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfFastLeaveAdminMode
*
* @purpose Get 'SnoopingIntfFastLeaveAdminMode'
 *@description  [SnoopingIntfFastLeaveAdminMode] This enables or disables IGMP
* Snooping on the selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfFastLeaveAdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfFastLeaveAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfFastLeaveAdminModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                              &objSnoopingIntfFastLeaveAdminModeValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingIntfFastLeaveAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingIntfFastLeaveAdminModeValue,
                    sizeof (objSnoopingIntfFastLeaveAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfFastLeaveAdminMode
*
* @purpose Set 'SnoopingIntfFastLeaveAdminMode'
 *@description  [SnoopingIntfFastLeaveAdminMode] This enables or disables IGMP
* Snooping on the selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfFastLeaveAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfFastLeaveAdminModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIntfFastLeaveAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIntfFastLeaveAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIntfFastLeaveAdminModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfFastLeaveAdminModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                               objSnoopingIntfFastLeaveAdminModeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfMulticastRouterMode
*
* @purpose Get 'SnoopingIntfMulticastRouterMode'
 *@description  [SnoopingIntfMulticastRouterMode] his enables or disables
* Multicast Routing on the selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfMulticastRouterMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t operIntfMode = L7_DISABLE;
  xLibU32_t vlanId = 0;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMulticastRouterModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfMrouterStatusGet (L7_UNIT_CURRENT, keyInterfaceValue,
                              &objSnoopingIntfMulticastRouterModeValue, keySnoopingProtocolValue);

  if( objSnoopingIntfMulticastRouterModeValue != L7_ENABLE )
  {

    objSnoopingIntfMulticastRouterModeValue = L7_DISABLE;
    for (vlanId = 1 ; vlanId <= L7_DOT1Q_MAX_VLAN_ID ; vlanId++)
    {
      operIntfMode = L7_DISABLE;
      (void)usmDbSnoopIntfMrouterGet(L7_UNIT_CURRENT, keyInterfaceValue, vlanId, &operIntfMode, keySnoopingProtocolValue);
      if (operIntfMode == L7_ENABLE)
      {
        objSnoopingIntfMulticastRouterModeValue = L7_ENABLE;
        break;
      }
    }
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingIntfMulticastRouterMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingIntfMulticastRouterModeValue,
                    sizeof (objSnoopingIntfMulticastRouterModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfMulticastRouterMode
*
* @purpose Set 'SnoopingIntfMulticastRouterMode'
 *@description  [SnoopingIntfMulticastRouterMode] his enables or disables
* Multicast Routing on the selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingIntfMulticastRouterMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfMulticastRouterModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingIntfMulticastRouterMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingIntfMulticastRouterModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingIntfMulticastRouterModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopIntfMrouterSet (L7_UNIT_CURRENT, keyInterfaceValue,
                              objSnoopingIntfMulticastRouterModeValue, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfVlanIDs
*
* @purpose Get 'SnoopingIntfVlanIDs'
 *@description  [SnoopingIntfVlanIDs] This field lists all the VlanIDs which
*              include the selected interface 
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopingIntfVlanID (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingIntfVlanIDsValue;
  xLibU32_t nextObjSnoopingIntfVlanIDsValue;
  
  xLibU32_t vlanId;
  xLibU8_t  cfgVlanMode;
  xLibU32_t operVlanMode;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

                           
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingIntfVlanID,
                                          (xLibU8_t *) & objSnoopingIntfVlanIDsValue,
                                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objSnoopingIntfVlanIDsValue = 0;
  }

  for( vlanId = 1+objSnoopingIntfVlanIDsValue; vlanId < L7_DOT1Q_MAX_VLAN_ID; vlanId++)
  {
     cfgVlanMode = L7_DISABLE;
     operVlanMode = L7_DISABLE; 
    (void)usmDbsnoopIntfApiVlanStaticMcastRtrGet(L7_UNIT_CURRENT,
                                               keyInterfaceValue,
                                               vlanId,
                                                (L7_uchar8*)&cfgVlanMode,
                                               (L7_uchar8) keySnoopingProtocolValue);
          
    (void) usmDbSnoopIntfMrouterGet(L7_UNIT_CURRENT,
                                               keyInterfaceValue,
                                               vlanId,
                                               &operVlanMode,
                                               (L7_uchar8) keySnoopingProtocolValue);
 
   if((cfgVlanMode == L7_ENABLE) || (operVlanMode == L7_ENABLE))
   {
     nextObjSnoopingIntfVlanIDsValue = vlanId;
     break;
   }

  }
  if ((owa.l7rc != L7_SUCCESS) ||(vlanId >= L7_DOT1Q_MAX_VLAN_ID))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
        
  /* return the object value: SnoopingIntfVlanIDs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjSnoopingIntfVlanIDsValue,
                           sizeof(objSnoopingIntfVlanIDsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_intfMulticastRouterVlanMode(void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingIntfVlanIDsValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastRouterVlanMode;
    

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

                           
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingIntfVlanID,
                                          (xLibU8_t *) & keySnoopingIntfVlanIDsValue,
                                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_intfMulticastRouterVlanMode,
                                          (xLibU8_t *) & objMulticastRouterVlanMode,
                                          &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbsnoopIntfApiVlanStaticMcastRtrSet(L7_UNIT_CURRENT,
                                               keyInterfaceValue,
                                               keySnoopingIntfVlanIDsValue,
                                               objMulticastRouterVlanMode,
                                               keySnoopingProtocolValue);

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
        
  /* return the object value: SnoopingIntfVlanIDs */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_intfMulticastRouterVlanMode(void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingIntfVlanIDsValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastRouterVlanMode = 0;
  xLibU8_t  tempMulticastRouterVlanMode=0;
   

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);

                           
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingIntfVlanID,
                                          (xLibU8_t *) & keySnoopingIntfVlanIDsValue,
                                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  owa.l7rc = usmDbsnoopIntfApiVlanStaticMcastRtrGet(L7_UNIT_CURRENT,
                                               keyInterfaceValue,
                                               keySnoopingIntfVlanIDsValue,
                                               (L7_uchar8*) &tempMulticastRouterVlanMode,
                                               (L7_uchar8) keySnoopingProtocolValue);
    objMulticastRouterVlanMode= tempMulticastRouterVlanMode;

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
        
  /* return the object value: SnoopingIntfVlanIDs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objMulticastRouterVlanMode,
                           sizeof(objMulticastRouterVlanMode));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingSnoopingIntfGroup_ClearSnoopEntries
*
* @purpose Set 'ClearSnoopEntries'
 *@description  [ClearSnoopEntries] Flush all Snooping entries and multicast
*              router entries 
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_ClearSnoopEntries (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClearSnoopEntriesValue;

  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClearSnoopEntries */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClearSnoopEntriesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClearSnoopEntriesValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopEntriesFlush(L7_UNIT_CURRENT, keySnoopingProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_SnoopDataFramesForwarded
*
* @purpose Get 'SnoopDataFramesForwarded'
 *@description  [SnoopDataFramesForwarded] The number of multicast data frames
*              forwarded by the CPU 
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_SnoopDataFramesForwarded (void *wap, void *bufp)
{

  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopDataFramesForwardedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
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
  owa.l7rc = usmDbSnoopDataFramesForwarded(L7_UNIT_CURRENT,
                              &objSnoopDataFramesForwardedValue, keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopDataFramesForwarded */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopDataFramesForwardedValue,
                    sizeof (objSnoopDataFramesForwardedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingSnoopingIntfGroup_VLANIDsEnabled
*
* @purpose Get 'VLANIDsEnabled'
 *@description  [VLANIDsEnabled] VLAN Ids Enabled for IGMP Snooping   
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSnoopingIntfGroup_VLANIDsEnabled (void *wap, void *bufp)
{

  fpObjWa_t kwaSnoopingProtocol = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;

  xLibU32_t vlanId;
  xLibU32_t snoopVlanMode;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  L7_VLAN_MASK_t objVLANIDsEnabledValue;
  L7_VLAN_MASK_t tempInVlanMask;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objVLANIDsEnabledValue, 0x00, sizeof(objVLANIDsEnabledValue));
  memset(&tempInVlanMask,0x00,sizeof(tempInVlanMask));

  /* retrieve key: SnoopingProtocol */
  kwaSnoopingProtocol.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                                          (xLibU8_t *) & keySnoopingProtocolValue,
                                          &kwaSnoopingProtocol.len);
  if (kwaSnoopingProtocol.rc != XLIBRC_SUCCESS)
  {
    kwaSnoopingProtocol.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSnoopingProtocol);
    return kwaSnoopingProtocol.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwaSnoopingProtocol.len);
  
  for(vlanId = L7_DOT1Q_MIN_VLAN_ID; vlanId <= L7_DOT1Q_MAX_VLAN_ID ; vlanId++)
  {
    if((usmDbSnoopVlanModeGet(L7_UNIT_CURRENT, vlanId,&snoopVlanMode, keySnoopingProtocolValue)== L7_SUCCESS)&&
       (snoopVlanMode == L7_ENABLE))
    {
      L7_VLAN_SETMASKBIT(tempInVlanMask, vlanId);
    } 
    
  }

  memcpy(&objVLANIDsEnabledValue, &tempInVlanMask, sizeof(objVLANIDsEnabledValue)); 
  /* return the object value: VLANIDsEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objVLANIDsEnabledValue,
                           sizeof (objVLANIDsEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
 * * @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalAdminMode
 * *
 * * @purpose Set 'SnoopingGlobalAdminMode'
 *  *@description  [SnoopingGlobalAdminMode] This enables or disables IGMP Snooping
 *  * on all interfaces   
 *  * @notes       
 *  *
 *  * @return
 *  *******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalAdminMode (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSnoopingGlobalAdminModeValue;
 
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t intIfNum, nextIfNum;

  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve object: SnoopingGlobalAdminMode */
  owa.len = sizeof (objSnoopingGlobalAdminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingGlobalAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingGlobalAdminModeValue, owa.len);
 
  /* retrieve key: SnoopingProtocol */
  owa.len = sizeof (keySnoopingProtocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                          (xLibU8_t *) & keySnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, owa.len);
 
  intIfNum = 0;
  while(usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, 
           USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &nextIfNum) == L7_SUCCESS)
  {
    intIfNum = nextIfNum;
    /* don't allow if the ifIndex is a PORTCHANNEL member */
    if ((usmDbDot3adValidIntfCheck(L7_UNIT_CURRENT, intIfNum) == L7_TRUE)
        && (usmDbDot3adIsConfigured(L7_UNIT_CURRENT, intIfNum) != L7_TRUE))
    {
      continue;
    }
 
    owa.l7rc = usmDbSnoopIntfModeSet(L7_UNIT_CURRENT, intIfNum, 
                     objSnoopingGlobalAdminModeValue, keySnoopingProtocolValue);
    
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}


/*******************************************************************************
 * * @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalGroupMembershipInterval
 * *
 * * @purpose Set 'SnoopingGlobalGroupMembershipInterval'
 *  *@description  [SnoopingGlobalGroupMembershipInterval] The amount of time in
 *  * seconds that a switch will wait for a report from a particular
 *  * group on the selected interface before deleting the interface from
 *  * the entry. This value must be greater than
 *  * SnoopingIntfMaxResponseTime.   
 *  * @notes       
 *  *
 *  * @return
 *  *******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalGroupMembershipInterval (void *wap,
                                                                                    void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSnoopingGlobalGroupMembershipIntervalValue;
   
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t intIfNum, nextIfNum;
  xLibU32_t responseTime;
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve object: SnoopingGlobalGroupMembershipInterval */
  owa.len = sizeof (objSnoopingGlobalGroupMembershipIntervalValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSnoopingGlobalGroupMembershipIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingGlobalGroupMembershipIntervalValue, owa.len);
 
  /* retrieve key: SnoopingProtocol */
  owa.len = sizeof (keySnoopingProtocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                          (xLibU8_t *) & keySnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, owa.len);
 
  intIfNum = 0;
  while(usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,
           USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &nextIfNum) == L7_SUCCESS)
  {
    intIfNum = nextIfNum;
    /* don't allow if the ifIndex is a PORTCHANNEL member */
    if ((usmDbDot3adValidIntfCheck(L7_UNIT_CURRENT, intIfNum) == L7_TRUE)
        && (usmDbDot3adIsConfigured(L7_UNIT_CURRENT, intIfNum) != L7_TRUE))
    {
      continue;
    }

    if (usmDbSnoopIntfResponseTimeGet(L7_UNIT_CURRENT, intIfNum, 
             &responseTime, keySnoopingProtocolValue) == L7_SUCCESS)
    {
      if (objSnoopingGlobalGroupMembershipIntervalValue <= responseTime)
      {
        owa.rc = XLIBRC_FAILURE;
        break; 
      }
    }
 
    owa.l7rc = usmDbSnoopIntfGroupMembershipIntervalSet(L7_UNIT_CURRENT, intIfNum,
                     objSnoopingGlobalGroupMembershipIntervalValue, keySnoopingProtocolValue);
 
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

/*******************************************************************************
 * * @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalMaxResponseTime
 * *
 * * @purpose Set 'SnoopingGlobalMaxResponseTime'
 *  *@description  [SnoopingGlobalMaxResponseTime] The amount of time in seconds a
 *  * switch will wait after sending a query on the selected interface
 *  * because it did not receive a report for a particular group in
 *  * that interface. This value must be less than
 *  * SnoopingIntfGroupMembershipInterval.   
 *  * @notes       
 *  *
 *  * @return
 *  *******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalMaxResponseTime (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSnoopingGlobalMaxResponseTimeValue;
   
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t intIfNum, nextIfNum;
  xLibU32_t groupMembershipInterval;

  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve object: SnoopingGlobalMaxResponseTime */
  owa.len = sizeof (objSnoopingGlobalMaxResponseTimeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingGlobalMaxResponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingGlobalMaxResponseTimeValue, owa.len);
 
  /* retrieve key: SnoopingProtocol */
  owa.len = sizeof (keySnoopingProtocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                          (xLibU8_t *) & keySnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, owa.len);
 
  intIfNum = 0;
  while(usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,
           USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &nextIfNum) == L7_SUCCESS)
  {
    intIfNum = nextIfNum;
    /* don't allow if the ifIndex is a PORTCHANNEL member */
    if ((usmDbDot3adValidIntfCheck(L7_UNIT_CURRENT, intIfNum) == L7_TRUE)
        && (usmDbDot3adIsConfigured(L7_UNIT_CURRENT, intIfNum) != L7_TRUE))
    {
      continue;
    }

    if (usmDbSnoopIntfGroupMembershipIntervalGet(L7_UNIT_CURRENT, intIfNum, 
             &groupMembershipInterval, keySnoopingProtocolValue) == L7_SUCCESS)
    {
      if (objSnoopingGlobalMaxResponseTimeValue >= groupMembershipInterval)
      {
        owa.rc = XLIBRC_FAILURE;
        break; 
      }
    }
 
    owa.l7rc = usmDbSnoopIntfResponseTimeSet(L7_UNIT_CURRENT, intIfNum,
                     objSnoopingGlobalMaxResponseTimeValue, keySnoopingProtocolValue);
 
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

/*******************************************************************************
 * * @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalMRPExpirationTime
 * *
 * * @purpose Set 'SnoopingGlobalMRPExpirationTime'
 *  *@description  [SnoopingGlobalMRPExpirationTime] The amount of time in seconds
 *  * that a switch will wait for a query to be received on the
 *  * selected interface before the interface is removed from the list of
 *  * interfaces with multicast routers attached.   
 *  * @notes       
 *  *
 *  * @return
 *  *******************************************************************************/

xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalMRPExpirationTime (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSnoopingGlobalMRPExpirationTimeValue;
 
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t intIfNum, nextIfNum;  

  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve object: SnoopingGlobalMRPExpirationTime */
  owa.len = sizeof (objSnoopingGlobalMRPExpirationTimeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingGlobalMRPExpirationTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingGlobalMRPExpirationTimeValue, owa.len);
 
  /* retrieve key: SnoopingProtocol */
  owa.len = sizeof (keySnoopingProtocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                          (xLibU8_t *) & keySnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, owa.len);
 
  intIfNum = 0;
  while(usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,
           USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &nextIfNum) == L7_SUCCESS)
  {
    intIfNum = nextIfNum;
    /* don't allow if the ifIndex is a PORTCHANNEL member */
    if ((usmDbDot3adValidIntfCheck(L7_UNIT_CURRENT, intIfNum) == L7_TRUE)
        && (usmDbDot3adIsConfigured(L7_UNIT_CURRENT, intIfNum) != L7_TRUE))
    {
      continue;
    }
 
    owa.l7rc = usmDbSnoopIntfMcastRtrExpiryTimeSet(L7_UNIT_CURRENT, intIfNum,
                     objSnoopingGlobalMRPExpirationTimeValue, keySnoopingProtocolValue);
 
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

 
/*******************************************************************************
 * * @function fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalFastLeaveAdminMode
 * *
 * * @purpose Set 'SnoopingGlobalFastLeaveAdminMode'
 *  *@description  [SnoopingGlobalFastLeaveAdminMode] This enables or disables IGMP
 *  * Snooping on all interface   
 *  * @notes       
 *  *
 *  * @return
 *  *******************************************************************************/
xLibRC_t fpObjSet_SwitchingSnoopingIntfGroup_SnoopingGlobalFastLeaveAdminMode (void *wap,
                                                                               void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSnoopingGlobalFastLeaveAdminModeValue;
 
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t intIfNum, nextIfNum;
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve object: SnoopingGlobalFastLeaveAdminMode */
  owa.len = sizeof (objSnoopingGlobalFastLeaveAdminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSnoopingGlobalFastLeaveAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingGlobalFastLeaveAdminModeValue, owa.len);
 
  /* retrieve key: SnoopingProtocol */
  owa.len = sizeof (keySnoopingProtocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingSnoopingIntfGroup_SnoopingProtocol,
                          (xLibU8_t *) & keySnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, owa.len);
 
  intIfNum = 0;
  while(usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,
           USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &nextIfNum) == L7_SUCCESS)
  {
    intIfNum = nextIfNum;
    /* don't allow if the ifIndex is a PORTCHANNEL member */
    if ((usmDbDot3adValidIntfCheck(L7_UNIT_CURRENT, intIfNum) == L7_TRUE)
        && (usmDbDot3adIsConfigured(L7_UNIT_CURRENT, intIfNum) != L7_TRUE))
    {
      continue;
    }
 
    owa.l7rc = usmDbSnoopIntfFastLeaveAdminModeSet(L7_UNIT_CURRENT, intIfNum,
                     objSnoopingGlobalFastLeaveAdminModeValue, keySnoopingProtocolValue);
 
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}
