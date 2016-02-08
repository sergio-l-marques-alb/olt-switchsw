/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingloopbackConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to loopback-object.xml
*
* @create  13 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingloopbackConfig_obj.h"
#include "usmdb_rlim_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_ID
*
* @purpose Get 'ID'
*
* @description [ID]: The Loopback ID is associated with Internal Interface
*              number which will be generated when we create a loopback.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_ID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIDValue=0;
  xLibU32_t nextObjIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & objIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbRlimLoopbackIdFirstGet(&nextObjIDValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIDValue, owa.len);
    owa.l7rc = usmDbRlimLoopbackIdNextGet(objIDValue, &nextObjIDValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIDValue, owa.len);

  /* return the object value: ID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIDValue,
                           sizeof (objIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_routingloopbackConfig_ID
*
* @purpose List 'ID'
 *@description  [ID] The Loopback ID is associated with Internal Interface
* number which will be generated when we create a loopback.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_routingloopbackConfig_ID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIDValue;
  xLibU32_t nextObjIDValue;
  xLibS32_t loopbackId;
  L7_uint32 ct = 0, flag = L7_FALSE;
  L7_uint32 intIfNum = 0; 

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID, (xLibU8_t *) & objIDValue, &owa.len);
  
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    loopbackId = -1; /* since loopback ids started from 0 onwards */
  }
  else
  { 
    loopbackId = objIDValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjIDValue, owa.len);
  }

  for(ct = (loopbackId + 1); ct<L7_MAX_NUM_LOOPBACK_INTF; ct++)
  {
     if(usmDbRlimLoopbackIntIfNumGet(ct, &intIfNum) == L7_SUCCESS)
     {
       continue;
     }
     else
     {
       nextObjIDValue = ct;
       flag = L7_TRUE;
       break;
     }
  }

  if(flag == L7_TRUE)
  {
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIDValue, owa.len);

  /* return the object value: ID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIDValue, sizeof (nextObjIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_Protocol
*
* @purpose Get 'Protocol'
*
* @description [ID]: The Loopback ID is associated with Internal Interface
*              number which will be generated when we create a loopback.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_Protocol (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolValue=0;
  xLibU32_t nextObjProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_Protocol,
                          (xLibU8_t *) & objProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjProtocolValue = L7_IPv4;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objProtocolValue, owa.len);
    if(objProtocolValue == L7_IPv6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjProtocolValue = L7_IPv6;
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjProtocolValue, owa.len);

  /* return the object value: ID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjProtocolValue,
                           sizeof (objProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_routingloopbackConfig_Protocol (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  owa.rc=XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_IsIpv4Exist
*
* @purpose Get 'Protocol'
*
* @description [ID]: The Loopback ID is associated with Internal Interface
*              number which will be generated when we create a loopback.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_IsIpv4Exist (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t value,intfNum;
  xLibU32_t ipaddress,subval;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRlimLoopbackIntIfNumGet (keyIDValue, &intfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT, intfNum, &ipaddress, &subval);
  if ((owa.l7rc != L7_SUCCESS) || (ipaddress == 0))
  {
    value = L7_FALSE;
  }
  else
  {
    value = L7_TRUE;
  }

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & value,
                           sizeof (value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_ifIndex
*
* @purpose Get 'ifIndex'
*
* @description [ifIndex]: This is external interface associated with inetrnal
*              interface of loopback. The Loopback ID is associated with
*              Internal Interface number which will be generated when we
*              create a loopback. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_ifIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRlimLoopbackIntIfNumGet (keyIDValue,
                                           &objifIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifIndexValue,
                           sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_SecondaryIfIndex
*
* @purpose Get 'ifIndex'
*
* @description [ifIndex]: This is external interface associated with inetrnal
*              interface of loopback. The Loopback ID is associated with
*              Internal Interface number which will be generated when we
*              create a loopback. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_SecondaryIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_SecondaryIfIndex,
                          (xLibU8_t *) & objifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbRlimLoopbackIntIfNumGet (keyIDValue,
                                           &objifIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifIndexValue,
                           sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingloopbackConfig_ifIndex
*
* @purpose set 'ifIndex'
*
* @description [ifIndex]: This is external interface associated with inetrnal
*              interface of loopback. The Loopback ID is associated with
*              Internal Interface number which will be generated when we
*              create a loopback.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingloopbackConfig_ifIndex (void *wap, void *bufp)
{
  FPOBJ_TRACE_ENTER (bufp);
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_IPAddress
*
* @purpose Get 'IPAddress'
*
* @description [IPAddress]: The IP Address configured for the respective loopback
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_IPAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  xLibU32_t intfNum;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPAddressValue,subval;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  if(usmDbRlimLoopbackIntIfNumGet(keyIDValue, &intfNum) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT, intfNum, &objIPAddressValue, &subval);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPAddressValue,
                           sizeof (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingloopbackConfig_IPAddress
*
* @purpose Set 'IPAddress'
*
* @description [IPAddress]: The IP Address configured for the respective loopback
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingloopbackConfig_IPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  xLibU32_t ipsubvalue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IPAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIPAddressValue, owa.len);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);


  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_IPSubnet,
                          (xLibU8_t *) & ipsubvalue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &ipsubvalue, kwa.len);

  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_IPSubnet
*
* @purpose Get 'IPSubnet'
*
* @description [IPSubnet]: The Subnet mask configured for the respective loopback
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_IPSubnet (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPSubnetValue;
  xLibU32_t intfNum;
  xLibU32_t objIPAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  if(usmDbRlimLoopbackIntIfNumGet(keyIDValue, &intfNum) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT, intfNum, &objIPAddressValue, &objIPSubnetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IPSubnet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPSubnetValue,
                           sizeof (objIPSubnetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingloopbackConfig_IPSubnet
*
* @purpose Set 'IPSubnet'
*
* @description [IPSubnet]: The Subnet mask configured for the respective loopback
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingloopbackConfig_IPSubnet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPSubnetValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  xLibU32_t intfNum;
  xLibU32_t ipaddress;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IPSubnet */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIPSubnetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIPSubnetValue, owa.len);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_IPAddress,
                          (xLibU8_t *) & ipaddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &ipaddress, kwa.len);

  if(usmDbRlimLoopbackIntIfNumGet(keyIDValue, &intfNum) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (usmDbNetmaskValidate32(objIPSubnetValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_SUBNET_MASK32;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbIpRtrIntfIPAddressSet (L7_UNIT_CURRENT, intfNum, ipaddress, objIPSubnetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    switch (owa.l7rc)
    {
      case L7_ERROR:
        owa.rc = XLIBRC_IP_ADDR_CONFLICT;
        break;
      case L7_NOT_SUPPORTED:
        owa.rc = XLIBRC_SUPPORTON_LOGICALINTF;
        break;
      case L7_TABLE_IS_FULL:
        owa.rc = XLIBRC_IPV4_INTF_ADDRESS_TABLE_FULL;
        break;
      case L7_ALREADY_CONFIGURED:
        owa.rc = XLIBRC_ADDRESS_ALREADY_CONFIGURED;
        break;
      case L7_REQUEST_DENIED:
        owa.rc = XLIBRC_NEXTHOPOF_STATIC_ROUTE_OR_ARP_ENTRY_EXIST;
        break;
      case L7_NOT_EXIST:
        owa.rc = XLIBRC_CANT_SET_UNNUMBERED_INTF;
        break;
      default:
        owa.rc = XLIBRC_FAILURE;
        break;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingloopbackConfig_IpAddressDel
*
* @purpose Set 'IPAddress'
*
* @description [IPAddress]: The IP Address configured for the respective loopback
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingloopbackConfig_IPAddressDel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  xLibU32_t ipaddress;
  xLibU32_t ipsubvalue,intIfNum;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: ID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, owa.len);

  if(usmDbRlimLoopbackIntIfNumGet(keyIDValue, &intIfNum) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve object: IPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_IPAddress,
                          (xLibU8_t *) & ipaddress, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &ipaddress, owa.len);

  /* retrieve object: IPMaskAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_IPSubnet,
                          (xLibU8_t *) & ipsubvalue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ipsubvalue, owa.len);


  /* Remove Primary IP address */
  owa.l7rc = usmDbIpRtrIntfIPAddressRemove(L7_UNIT_CURRENT, intIfNum, ipaddress, ipsubvalue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if (owa.l7rc == L7_ERROR)
    {
      owa.rc = XLIBRC_LOOPBACK_PRIMARYIP_DEL_FAIL2;
    }
    else
    {
      owa.rc = XLIBRC_LOOPBACK_PRIMARYIP_DEL_FAIL1;
    }

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingloopbackConfig_status
*
* @purpose Get 'status'
*
* @description [status]: Status of this instance. The rows can be added/deleted
*              in the table by setting createAndGo/destroy respectively
*              active(1) - this loopback instance is active ;createAndGo(4)
*              - set to this value to create an instance ; destroy(6)
*              - set to this value to delete an instance 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackConfig_status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstatusValue;
  xLibU32_t intfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  if(usmDbRlimLoopbackIntIfNumGet(keyIDValue, &intfNum) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objstatusValue = L7_ROW_STATUS_ACTIVE;
  /* return the object value: status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objstatusValue,
                           sizeof (objstatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingloopbackConfig_status
*
* @purpose Set 'status'
*
* @description [status]: Status of this instance. The rows can be added/deleted
*              in the table by setting createAndGo/destroy respectively
*              active(1) - this loopback instance is active ;createAndGo(4)
*              - set to this value to create an instance ; destroy(6)
*              - set to this value to delete an instance 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingloopbackConfig_status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;
  xLibU32_t intfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objstatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objstatusValue, owa.len);

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackConfig_ID,
                          (xLibU8_t *) & keyIDValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objstatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbRlimLoopbackCreate(keyIDValue);

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objstatusValue == L7_ROW_STATUS_DESTROY)
  {
    if(usmDbRlimLoopbackIntIfNumGet(keyIDValue, &intfNum) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_LOOPBACK_INVALID_ID;    
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;

    }  
    /* Delete the existing row */
    owa.l7rc =
      usmDbRlimLoopbackDelete (L7_UNIT_CURRENT, intfNum);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}
