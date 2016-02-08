
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseTrapReceiver.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 April 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseTrapReceiver_obj.h"
#include "usmdb_trapmgr_api.h"
#include "snmp_exports.h"
#include "snmp_api.h"
/*******************************************************************************
* @function fpObjGet_baseTrapReceiver_TrapReceiverIndex
*
* @purpose Get 'TrapReceiverIndex'
*
* @description [TrapReceiverIndex] TrapReceiver Index
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapReceiver_TrapReceiverIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapReceiverIndexValue;
  xLibU32_t nextObjTrapReceiverIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
  L7_uint32 status;
  L7_uint32 index;
  L7_BOOL entryFound;

  /* retrieve key: TrapReceiverIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                          (xLibU8_t *) & objTrapReceiverIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    entryFound = L7_FALSE;
    for (index =0; index < L7_MAX_SNMP_COMM; index += 1)
   {
       if (SnmpTrapMgrStatusGet(index, &status) == L7_SUCCESS && status != L7_SNMP_TRAP_MGR_STATUS_DELETE)
       {
          entryFound = L7_TRUE;
	   nextObjTrapReceiverIndexValue = index;
	   break;
       }
    }
     if(entryFound == L7_TRUE)
     {
        owa.l7rc  = L7_SUCCESS;
     }
     else
     {
       owa.l7rc  = L7_FAILURE;
     }

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTrapReceiverIndexValue, owa.len);
    entryFound = L7_FALSE;
    for (index = objTrapReceiverIndexValue+1; index < L7_MAX_SNMP_COMM; index += 1)
   {
       if (SnmpTrapMgrStatusGet(index, &status) == L7_SUCCESS && status != L7_SNMP_TRAP_MGR_STATUS_DELETE)
       {
          entryFound = L7_TRUE;
	   nextObjTrapReceiverIndexValue = index;
	   break;
       }
    }
     if(entryFound == L7_TRUE)
     {
        owa.l7rc  = L7_SUCCESS;
     }
     else
     {
       owa.l7rc  = L7_FAILURE;
     }
  }  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTrapReceiverIndexValue, owa.len);

  /* return the object value: TrapReceiverIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTrapReceiverIndexValue,
                           sizeof (objTrapReceiverIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapReceiver_TrapReceiverCommunityName
*
* @purpose Get 'TrapReceiverCommunityName'
*
* @description [TrapReceiverCommunityName] TrapReceiver Community Name
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapReceiver_TrapReceiverCommunityName (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTrapReceiverCommunityNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbTrapManagerCommIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              objTrapReceiverCommunityNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TrapReceiverCommunityName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTrapReceiverCommunityNameValue,
                           strlen (objTrapReceiverCommunityNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapReceiver_TrapReceiverCommunityName
*
* @purpose Set 'TrapReceiverCommunityName'
*
* @description [TrapReceiverCommunityName] TrapReceiver Community Name
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapReceiver_TrapReceiverCommunityName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTrapReceiverCommunityNameValue;

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  xLibU32_t currStatus = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapReceiverCommunityName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objTrapReceiverCommunityNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTrapReceiverCommunityNameValue, owa.len);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* set the value in application */

  if(usmDbTrapManagerStatusIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
      owa.l7rc = usmDbTrapManagerCommIpSet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                objTrapReceiverCommunityNameValue);

      if((currStatus == L7_SNMP_TRAP_MGR_STATUS_CONFIG) && (owa.l7rc != L7_SUCCESS))
      {
         usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, L7_SNMP_TRAP_MGR_STATUS_DELETE);
      }

  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapReceiver_TrapReceiverIPV4Address
*
* @purpose Get 'TrapReceiverIPV4Address'
*
* @description [TrapReceiverIPV4Address] ipv4 address
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapReceiver_TrapReceiverIPV4Address (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objTrapReceiverIPV4AddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbTrapManagerIpAddrGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              &objTrapReceiverIPV4AddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TrapReceiverIPV4Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTrapReceiverIPV4AddressValue,
                           sizeof (objTrapReceiverIPV4AddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapReceiver_TrapReceiverIPV4Address
*
* @purpose Set 'TrapReceiverIPV4Address'
*
* @description [TrapReceiverIPV4Address] ipv4 address
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapReceiver_TrapReceiverIPV4Address (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objTrapReceiverIPV4AddressValue;

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  xLibU32_t  currStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapReceiverIPV4Address */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objTrapReceiverIPV4AddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTrapReceiverIPV4AddressValue, owa.len);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(usmDbTrapManagerStatusIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
      if(usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, L7_SNMP_TRAP_MGR_STATUS_CONFIG) == L7_SUCCESS)
      {
        if((owa.l7rc = usmDbTrapManagerIpAddrRawSet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                objTrapReceiverIPV4AddressValue)) == L7_SUCCESS)
          usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, currStatus);
      }

  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapReceiver_TrapReceiverStatus
*
* @purpose Get 'TrapReceiverStatus'
*
* @description [TrapReceiverStatus] status
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapReceiver_TrapReceiverStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapReceiverStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbTrapManagerStatusIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              &objTrapReceiverStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TrapReceiverStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapReceiverStatusValue,
                           sizeof (objTrapReceiverStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapReceiver_TrapReceiverStatus
*
* @purpose Set 'TrapReceiverStatus'
*
* @description [TrapReceiverStatus] status
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapReceiver_TrapReceiverStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapReceiverStatusValue;

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapReceiverStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapReceiverStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapReceiverStatusValue, owa.len);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbTrapManagerStatusIpSet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              objTrapReceiverStatusValue);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapReceiver_TrapReceiverVersion
*
* @purpose Get 'TrapReceiverVersion'
*
* @description [TrapReceiverVersion] trap version
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapReceiver_TrapReceiverVersion (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapReceiverVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbTrapManagerVersionGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              &objTrapReceiverVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TrapReceiverVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapReceiverVersionValue,
                           sizeof (objTrapReceiverVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapReceiver_TrapReceiverVersion
*
* @purpose Set 'TrapReceiverVersion'
*
* @description [TrapReceiverVersion] trap version
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapReceiver_TrapReceiverVersion (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapReceiverVersionValue;

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;
  xLibU32_t currStatus;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapReceiverVersion */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapReceiverVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapReceiverVersionValue, owa.len);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(usmDbTrapManagerStatusIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
      if(usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, L7_SNMP_TRAP_MGR_STATUS_CONFIG) == L7_SUCCESS)
      {
        if((owa.l7rc = usmDbTrapManagerVersionSet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              objTrapReceiverVersionValue)) == L7_SUCCESS)
           usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, currStatus);
      }

  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapReceiver_TrapReceiverIpv6Address
*
* @purpose Get 'TrapReceiverIpv6Address'
*
* @description [TrapReceiverIpv6Address] Ipv6 address
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapReceiver_TrapReceiverIpv6Address (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV6_t));
  xLibIpV6_t objTrapReceiverIpv6AddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbTrapManagerIPv6AddrGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              (L7_in6_addr_t *)&objTrapReceiverIpv6AddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TrapReceiverIpv6Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTrapReceiverIpv6AddressValue,
                           sizeof (objTrapReceiverIpv6AddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapReceiver_TrapReceiverIpv6Address
*
* @purpose Set 'TrapReceiverIpv6Address'
*
* @description [TrapReceiverIpv6Address] Ipv6 address
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapReceiver_TrapReceiverIpv6Address (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV6_t));
  xLibIpV6_t objTrapReceiverIpv6AddressValue;

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;

  xLibU32_t currStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapReceiverIpv6Address */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objTrapReceiverIpv6AddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTrapReceiverIpv6AddressValue, owa.len);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(usmDbTrapManagerStatusIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
      if(usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, L7_SNMP_TRAP_MGR_STATUS_CONFIG) == L7_SUCCESS)
      {
        if((owa.l7rc = usmDbTrapManagerIPv6AddrSet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                              (L7_in6_addr_t *)&objTrapReceiverIpv6AddressValue)) == L7_SUCCESS)
          usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, keyTrapReceiverIndexValue, currStatus);
      }

  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



xLibRC_t fpObjGet_baseTrapReceiver_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;
  xLibU32_t currStatus;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if (kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  if((owa.l7rc = usmDbTrapManagerStatusIpGet (L7_UNIT_CURRENT, keyTrapReceiverIndexValue,
                                          &currStatus) == L7_SUCCESS))
  {
    if( currStatus != L7_SNMP_TRAP_MGR_STATUS_DELETE )
    {
       objRowStatusValue = L7_ROW_STATUS_ACTIVE;	     
    }
    else
    {  
       objRowStatusValue = L7_ROW_STATUS_INVALID;
    }      
          	    
  }	  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
  
}

xLibRC_t fpObjSet_baseTrapReceiver_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  fpObjWa_t kwaTrapReceiverIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapReceiverIndexValue;
  xLibU32_t status = 0, i=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapReceiverIpv6Address */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: TrapReceiverIndex */
  kwaTrapReceiverIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapReceiver_TrapReceiverIndex,
                                           (xLibU8_t *) & keyTrapReceiverIndexValue,
                                           &kwaTrapReceiverIndex.len);
  if ((kwaTrapReceiverIndex.rc != XLIBRC_SUCCESS)&&(objRowStatusValue !=L7_ROW_STATUS_CREATE_AND_GO))
  {
    kwaTrapReceiverIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapReceiverIndex);
    return kwaTrapReceiverIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapReceiverIndexValue, kwaTrapReceiverIndex.len);

  /* set the value in application */
  if( objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)  
  {
    i=0;
    while (i < L7_MAX_SNMP_COMM && status!=L7_SNMP_TRAP_MGR_STATUS_DELETE)
    {
      owa.l7rc = usmDbTrapManagerStatusIpGet(L7_UNIT_CURRENT, i++, &status);
    }

    if(status == L7_SNMP_TRAP_MGR_STATUS_DELETE)
    { 
      i--;
      /* status must be set to config before other fields can be set */
      if(usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, i, L7_SNMP_TRAP_MGR_STATUS_CONFIG) == L7_SUCCESS)
      {
          if(xLibFilterSet(wap,XOBJ_baseTrapReceiver_TrapReceiverIndex,0,
                              (xLibU8_t *) &i,
                              sizeof(i)) == XLIBRC_SUCCESS)
              owa.l7rc = L7_SUCCESS;
      } 
      else
        owa.l7rc = L7_FAILURE;
    }
    else
      owa.l7rc = L7_FAILURE;
     
  }
  else
  {
     i = keyTrapReceiverIndexValue;
     owa.l7rc = usmDbTrapManagerStatusIpSet(L7_UNIT_CURRENT, i, L7_SNMP_TRAP_MGR_STATUS_DELETE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
