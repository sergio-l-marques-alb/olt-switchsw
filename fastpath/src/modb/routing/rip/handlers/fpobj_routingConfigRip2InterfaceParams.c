/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingConfigRip2InterfaceParams.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to RipInterfaceConfig-object.xml
*
* @create  22 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingConfigRip2InterfaceParams_obj.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_routingConfigRip2InterfaceParams_Address
*
* @purpose Get 'Address'
*
* @description [Address]: The IP Address of this system on the indicated subnet.
*              For unnumbered interfaces, the value 0.0.0.N, where
*              the least significant 24 bits (N) is the ifIndex for the IP
*              Interface in network byte order. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_Address (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressValue;
  xLibU32_t nextObjAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAddressValue = 0;
    owa.l7rc = usmDbRip2IfConfEntryNext(L7_UNIT_CURRENT, &nextObjAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressValue, owa.len);
    nextObjAddressValue = objAddressValue;
    owa.l7rc = usmDbRip2IfConfEntryNext (L7_UNIT_CURRENT, &nextObjAddressValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAddressValue, owa.len);

  /* return the object value: Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAddressValue,
                           sizeof (objAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingConfigRip2InterfaceParams_RoutingInterface
*
* @purpose Get 'RoutingInterface'
*
* @description [RoutingInterface]: Get the Valid Routing interface 
*               where RIP can be enabled.
* 
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_RoutingInterface (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_RoutingInterface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&objInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue); 
    if (owa.l7rc == L7_SUCCESS)
    {
      objInterfaceValue = nextObjInterfaceValue;
    }
  }

  while (owa.l7rc == L7_SUCCESS)
  {
    if ((usmDbRip2IsValidIntf(L7_UNIT_CURRENT, objInterfaceValue) != L7_TRUE)  ||
        (usmDbIpIntfExists(L7_UNIT_CURRENT, objInterfaceValue) != L7_TRUE))
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
      owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue); 
      if (owa.l7rc == L7_SUCCESS)
      {
        objInterfaceValue = nextObjInterfaceValue;
      }
    }
    else
    {
      break;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInterfaceValue,
                           sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingConfigRip2InterfaceParams_AuthType
*
* @purpose Get 'AuthType'
*
* @description [AuthType]: The type of Authentication used on this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_AuthType (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

  #if 0
  if(usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif 
  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfAuthTypeGet (L7_UNIT_CURRENT, keyAddressValue,
                                         &objAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthTypeValue,
                           sizeof (objAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2InterfaceParams_AuthType
*
* @purpose Set 'AuthType'
*
* @description [AuthType]: The type of Authentication used on this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_AuthType (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthTypeValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

 #if 0
  if(usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif
  if(usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyAddressValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfAuthTypeSet (L7_UNIT_CURRENT, keyAddressValue,
                                         objAuthTypeValue);
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
* @function fpObjGet_routingConfigRip2InterfaceParams_AuthKey
*
* @purpose Get 'AuthKey'
*
* @description [AuthKey]: The value to be used as the Authentication Key whenever
*              the corresponding instance of rip2IfConfAuthType has
*              a value other than noAuthentication. A modification of the
*              corresponding instance of rip2IfConfAuthType does not modify
*              the rip2IfConfAuthKey value. If a string shorter than 16
*              octets is supplied, it will be left-justified and padded
*              to 16 octets, on the right, with nulls (0x00). Reading this
*              object always results in an OCTET STRING of length zero; authentication
*              may not be bypassed by reading the MIB object.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_AuthKey (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

 #if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 #endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfAuthKeyActualGet(L7_UNIT_CURRENT, keyAddressValue,
                                        objAuthKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthKeyValue,
                           strlen (objAuthKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2InterfaceParams_AuthKey
*
* @purpose Set 'AuthKey'
*
* @description [AuthKey]: The value to be used as the Authentication Key whenever
*              the corresponding instance of rip2IfConfAuthType has
*              a value other than noAuthentication. A modification of the
*              corresponding instance of rip2IfConfAuthType does not modify
*              the rip2IfConfAuthKey value. If a string shorter than 16
*              octets is supplied, it will be left-justified and padded
*              to 16 octets, on the right, with nulls (0x00). Reading this
*              object always results in an OCTET STRING of length zero; authentication
*              may not be bypassed by reading the MIB object.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_AuthKey (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAuthKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthKeyValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

 #if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 #endif 

  if(usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyAddressValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfAuthKeySet (L7_UNIT_CURRENT, keyAddressValue,
                                        objAuthKeyValue);
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
* @function fpObjGet_routingConfigRip2InterfaceParams_Send
*
* @purpose Get 'Send'
*
* @description [Send]: What the router sends on this interface.ripVersion1
*              implies sending RIP updates compliant with RFC 1058.rip1Compatible
*              implies broadcasting RIP-2 updates using RFC 1058
*              route subsumption rules. ripVersion2 implies multicasting RIP-2
*              updates. ripV1Demand indicates the use of Demand RIP on
*              a WAN interface under RIP Version 1 rules. ripV2Demand indicates
*              the use of Demand RIP on a WAN interface under Version
*              2 rules. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_Send (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSendValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfSendGet (L7_UNIT_CURRENT, keyAddressValue,
                                     &objSendValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Send */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSendValue,
                           sizeof (objSendValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2InterfaceParams_Send
*
* @purpose Set 'Send'
*
* @description [Send]: What the router sends on this interface.ripVersion1
*              implies sending RIP updates compliant with RFC 1058.rip1Compatible
*              implies broadcasting RIP-2 updates using RFC 1058
*              route subsumption rules. ripVersion2 implies multicasting RIP-2
*              updates. ripV1Demand indicates the use of Demand RIP on
*              a WAN interface under RIP Version 1 rules. ripV2Demand indicates
*              the use of Demand RIP on a WAN interface under Version
*              2 rules. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_Send (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSendValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Send */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSendValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSendValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif

  if(usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyAddressValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfSendSet (L7_UNIT_CURRENT, keyAddressValue,
                                     objSendValue);
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
* @function fpObjGet_routingConfigRip2InterfaceParams_Receive
*
* @purpose Get 'Receive'
*
* @description [Receive]: This indicates which version of RIP updates are
*              to be accepted. Note that rip2 and rip1OrRip2 implies reception
*              of multicast packets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_Receive (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objReceiveValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);
  
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfReceiveGet (L7_UNIT_CURRENT, keyAddressValue,
                                        &objReceiveValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Receive */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objReceiveValue,
                           sizeof (objReceiveValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2InterfaceParams_Receive
*
* @purpose Set 'Receive'
*
* @description [Receive]: This indicates which version of RIP updates are
*              to be accepted. Note that rip2 and rip1OrRip2 implies reception
*              of multicast packets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_Receive (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objReceiveValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Receive */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objReceiveValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objReceiveValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif

  if(usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyAddressValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfReceiveSet (L7_UNIT_CURRENT, keyAddressValue,
                                        objReceiveValue);
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
* @function fpObjGet_routingConfigRip2InterfaceParams_DefaultMetric
*
* @purpose Get 'DefaultMetric'
*
* @description [DefaultMetric]: This variable indicates the metric that is
*              to be used for the default route entry in RIP updates originated
*              on this interface. A value of zero indicates that no
*              default route should be originated; in this case, a default
*              route via another router may be propagated. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_DefaultMetric (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif

  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfDefaultMetricGet (L7_UNIT_CURRENT, keyAddressValue,
                                              &objDefaultMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DefaultMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDefaultMetricValue,
                           sizeof (objDefaultMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2InterfaceParams_DefaultMetric
*
* @purpose Set 'DefaultMetric'
*
* @description [DefaultMetric]: This variable indicates the metric that is
*              to be used for the default route entry in RIP updates originated
*              on this interface. A value of zero indicates that no
*              default route should be originated; in this case, a default
*              route via another router may be propagated. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_DefaultMetric (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultMetricValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultMetric */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDefaultMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultMetricValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif

  if(usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyAddressValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfDefaultMetricSet (L7_UNIT_CURRENT, keyAddressValue,
                                              objDefaultMetricValue);
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
* @function fpObjGet_routingConfigRip2InterfaceParams_Status
*
* @purpose Get 'Status'
*
* @description [Status]: Writing invalid has the effect of deleting this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_Status (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfStatusGet (L7_UNIT_CURRENT, keyAddressValue,
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
* @function fpObjSet_routingConfigRip2InterfaceParams_Status
*
* @purpose Set 'Status'
*
* @description [Status]: Writing invalid has the effect of deleting this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_Status (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfStatusSet (L7_UNIT_CURRENT, keyAddressValue,
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
* @function fpObjGet_routingConfigRip2InterfaceParams_SrcAddress
*
* @purpose Get 'SrcAddress'
*
* @description [SrcAddress]: The IP Address this system will use as a source
*              address on this interface. If it is a numbered interface,
*              this MUST be the same value as rip2IfConfAddress. On unnumbered
*              interfaces, it must be the value of rip2IfConfAddress
*              for some interface on the system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_SrcAddress (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);

#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif

  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfSrcIpAddrGet (L7_UNIT_CURRENT, keyAddressValue, &objSrcAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSrcAddressValue,
                           sizeof (objSrcAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2InterfaceParams_SrcAddress
*
* @purpose Set 'SrcAddress'
*
* @description [SrcAddress]: The IP Address this system will use as a source
*              address on this interface. If it is a numbered interface,
*              this MUST be the same value as rip2IfConfAddress. On unnumbered
*              interfaces, it must be the value of rip2IfConfAddress
*              for some interface on the system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2InterfaceParams_SrcAddress (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SrcAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSrcAddressValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfSrcIpAddrSet (L7_UNIT_CURRENT, keyAddressValue,
                                          objSrcAddressValue);
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
* @function fpObjGet_routingConfigRip2InterfaceParams_IntfState
*
* @purpose Get 'IntfState'
 *@description  [IntfState] This is RIP2 routing interface.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2InterfaceParams_IntfState (void *wap, void *bufp)
{

  fpObjWa_t kwaAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwaAddress.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2InterfaceParams_Address,
                                 (xLibU8_t *) & keyAddressValue, &kwaAddress.len);
  if (kwaAddress.rc != XLIBRC_SUCCESS)
  {
    kwaAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddress);
    return kwaAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwaAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbRip2IntfStateGet(L7_UNIT_CURRENT, keyAddressValue, &objIntfStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfStateValue, sizeof (objIntfStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

