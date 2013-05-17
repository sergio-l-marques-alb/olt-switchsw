
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastipMcastStaticMRouteTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  18 September 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastipMcastStaticMRouteTable_obj.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_util_api.h"
#include "_xe_baseInterfaceInfo_obj.h"
#include "l7utils_inet_addr_api.h"
#include "osapi_support.h"

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface
*
* @purpose Get 'ipMcastStaticMRouteInterface'
 *@description  [ipMcastStaticMRouteInterface] <HTML>unit/slot/port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRouteInterfaceValue;
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  L7_inet_addr_t objipMcastStaticMRouteSrcIpAddrValue;  
  xLibV4V6Mask_t objipMcastStaticMRouteSrcIpMaskValue;  
  L7_inet_addr_t objipMcastStaticMRouteSrcMaskValue;
  xLibU8_t addressType;  
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof(objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                        (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);  
  
  if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  
  /* retrieve key: ipMcastStaticMRouteSrcIpAddr */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  /* retrieve key: ipMcastStaticmRouteSrcMask */
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  owa.l7rc = inetMaskLenToMask(addressType,(L7_uchar8)objipMcastStaticMRouteSrcIpMaskValue.addr,
                               &objipMcastStaticMRouteSrcMaskValue);
  
  if(usmDbMcastStaticMRouteEntryGet(L7_UNIT_CURRENT, addressType,
                                  &objipMcastStaticMRouteSrcIpAddrValue,
                                  &objipMcastStaticMRouteSrcMaskValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MROUTE_ROUTE_ENTRY_GET_FAILED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteSrcIpAddrValue, owa.len);  

  /* get the value from application */
  owa.l7rc = usmDbMcastStaticMRouteInterfaceGet (L7_UNIT_CURRENT,addressType,
                                       &objipMcastStaticMRouteSrcIpAddrValue,
                                       &objipMcastStaticMRouteSrcMaskValue,
                                       &objipMcastStaticMRouteInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRouteInterfaceValue,
                     sizeof (objipMcastStaticMRouteInterfaceValue));

  /* return the object value: ipMcastStaticMRouteInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastStaticMRouteInterfaceValue,
                           sizeof (objipMcastStaticMRouteInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface
*
* @purpose Set 'ipMcastStaticMRouteInterface'
 *@description  [ipMcastStaticMRouteInterface] <HTML>unit/slot/port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface (void *wap,
                                                                                void *bufp)
{
  return XLIBRC_SUCCESS;

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRouteInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastStaticMRouteInterface */
  owa.len = sizeof (objipMcastStaticMRouteInterfaceValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastStaticMRouteInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRouteInterfaceValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objipMcastStaticMRouteInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType
*
* @purpose Get 'ipMcastStaticMRouteAddressType'
 *@description  [ipMcastStaticMRouteAddressType] <HTML>Ip Address Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType (void *wap,
                                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  xLibU32_t nextObjipMcastStaticMRouteAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof (objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                          (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipMcastStaticMRouteAddressTypeValue = L7_INET_ADDR_TYPE_IPV4; 
    owa.l7rc = L7_SUCCESS ;
  }
  else
  {
  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);
    if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjipMcastStaticMRouteAddressTypeValue = objipMcastStaticMRouteAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
    
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastStaticMRouteAddressTypeValue, owa.len);

  /* return the object value: ipMcastStaticMRouteAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastStaticMRouteAddressTypeValue,
                           sizeof (nextObjipMcastStaticMRouteAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRoutePreference
*
* @purpose Get 'ipMcastStaticMRoutePreference'
 *@description  [ipMcastStaticMRoutePreference] <HTML>metric   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRoutePreference (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRoutePreferenceValue;
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  L7_inet_addr_t objipMcastStaticMRouteSrcIpAddrValue;  
  xLibV4V6Mask_t objipMcastStaticMRouteSrcIpMaskValue;  
  L7_inet_addr_t objipMcastStaticMRouteSrcMaskValue;
  xLibU8_t addressType;  
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof(objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                        (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);  
  
  if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  
  /* retrieve key: ipMcastStaticMRouteSrcIpAddr */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  /* retrieve key: ipMcastStaticmRouteSrcMask */
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  owa.l7rc = inetMaskLenToMask(addressType,(L7_uchar8)objipMcastStaticMRouteSrcIpMaskValue.addr,
                               &objipMcastStaticMRouteSrcMaskValue);
  
  if(usmDbMcastStaticMRouteEntryGet(L7_UNIT_CURRENT, addressType,
                                  &objipMcastStaticMRouteSrcIpAddrValue,
                                  &objipMcastStaticMRouteSrcMaskValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MROUTE_ROUTE_ENTRY_GET_FAILED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteSrcIpAddrValue, owa.len);  

  /* get the value from application */  
  owa.l7rc = usmDbMcastStaticMRoutePreferenceGet (L7_UNIT_CURRENT,addressType,
                                        &objipMcastStaticMRouteSrcIpAddrValue, 
                                        &objipMcastStaticMRouteSrcMaskValue, 
                                        &objipMcastStaticMRoutePreferenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRoutePreferenceValue,
                     sizeof (objipMcastStaticMRoutePreferenceValue));

  /* return the object value: ipMcastStaticMRoutePreference */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastStaticMRoutePreferenceValue,
                           sizeof (objipMcastStaticMRoutePreferenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface
*
* @purpose List 'ipMcastStaticMRouteInterface'
 *@description  [ipMcastStaticMRouteInterface] <HTML>Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof(objifIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF,
                                 0, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF,
                                               0, objifIndexValue, &nextObjifIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);
  
  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue,
                           sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRoutePreference
*
* @purpose Set 'ipMcastStaticMRoutePreference'
 *@description  [ipMcastStaticMRoutePreference] <HTML>metric   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRoutePreference (void *wap,
                                                                                 void *bufp)
{
  return XLIBRC_SUCCESS;

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRoutePreferenceValue;


  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: ipMcastStaticMRoutePreference */
  owa.len = sizeof (objipMcastStaticMRoutePreferenceValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastStaticMRoutePreferenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRoutePreferenceValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objipMcastStaticMRoutePreferenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRPFNextHop
*
* @purpose Get 'ipMcastStaticMRouteRPFNextHop'
 *@description  [ipMcastStaticMRouteRPFNextHop] <HTML>RPF Next Hop   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRPFNextHop (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_inet_addr_t objipMcastStaticMRouteRPFNextHopValue;
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  L7_inet_addr_t objipMcastStaticMRouteSrcIpAddrValue;  
  xLibV4V6Mask_t objipMcastStaticMRouteSrcIpMaskValue;
  L7_inet_addr_t objipMcastStaticMRouteSrcMaskValue;
  xLibU8_t addressType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof(objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                        (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);  
  
  if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  
  /* retrieve key: ipMcastStaticMRouteSrcIpAddr */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  /* retrieve key: ipMcastStaticmRouteSrcMask */
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  owa.l7rc = inetMaskLenToMask(addressType,(L7_uchar8)objipMcastStaticMRouteSrcIpMaskValue.addr,
                               &objipMcastStaticMRouteSrcMaskValue);
  


  if(usmDbMcastStaticMRouteEntryGet(L7_UNIT_CURRENT,addressType,
                                  &objipMcastStaticMRouteSrcIpAddrValue,
                                  &objipMcastStaticMRouteSrcMaskValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MROUTE_ROUTE_ENTRY_GET_FAILED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteSrcIpAddrValue, owa.len);  

  /* get the value from application */
  owa.l7rc = usmDbMcastStaticMRouteRpfAddressGet(L7_UNIT_CURRENT,addressType,
                                                 &objipMcastStaticMRouteSrcIpAddrValue,
                                                 &objipMcastStaticMRouteSrcMaskValue,
                                                 &objipMcastStaticMRouteRPFNextHopValue); 
  objipMcastStaticMRouteRPFNextHopValue.family = addressType;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objipMcastStaticMRouteRPFNextHopValue,
                     strlen (objipMcastStaticMRouteRPFNextHopValue));

  /* return the object value: ipMcastStaticMRouteRPFNextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastStaticMRouteRPFNextHopValue,
                           sizeof (objipMcastStaticMRouteRPFNextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRPFNextHop
*
* @purpose Set 'ipMcastStaticMRouteRPFNextHop'
 *@description  [ipMcastStaticMRouteRPFNextHop] <HTML>RPF Next Hop   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRPFNextHop (void *wap,
                                                                                 void *bufp)
{
  return XLIBRC_SUCCESS;

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objipMcastStaticMRouteRPFNextHopValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastStaticMRouteRPFNextHop */
  owa.len = sizeof (objipMcastStaticMRouteRPFNextHopValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objipMcastStaticMRouteRPFNextHopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objipMcastStaticMRouteRPFNextHopValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objipMcastStaticMRouteRPFNextHopValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRowStatus
*
* @purpose Get 'ipMcastStaticMRouteRowStatus'
 *@description  [ipMcastStaticMRouteRowStatus] <HTML>Row Status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRowStatus (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRouteRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objipMcastStaticMRouteRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRouteRowStatusValue,
                     sizeof (objipMcastStaticMRouteRowStatusValue));

  /* return the object value: ipMcastStaticMRouteRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastStaticMRouteRowStatusValue,
                           sizeof (objipMcastStaticMRouteRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRowStatus
*
* @purpose Set 'ipMcastStaticMRouteRowStatus'
 *@description  [ipMcastStaticMRouteRowStatus] <HTML>Row Status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRowStatus (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticMRouteRowStatusValue;
  xLibU32_t objipMcastStaticMRoutePreferenceValue;  
  xLibU32_t objipMcastStaticMRouteInterfaceValue = L7_NULL;
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  L7_inet_addr_t objipMcastStaticMRouteSrcIpAddrValue;  
  L7_inet_addr_t objipMcastStaticmRouteSrcMaskValue;  
  xLibV4V6Mask_t objipMcastStaticmRoutev4v6MaskValue;

  L7_inet_addr_t objipMcastStaticMRouteRPFNextHopValue;
  xLibU8_t addressType;  
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastStaticMRouteRowStatus */
  owa.len = sizeof (objipMcastStaticMRouteRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastStaticMRouteRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRouteRowStatusValue, owa.len);

  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof(objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                        (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);  
  
  if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  
  /* retrieve key: ipMcastStaticMRouteSrcIpAddr */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr,
                        (xLibU8_t *) &objipMcastStaticMRouteSrcIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteSrcIpAddrValue, owa.len);  


  /* retrieve : ipMcastStaticMRouteSourceMask */
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask,
                        (xLibU8_t *) &objipMcastStaticmRoutev4v6MaskValue, &owa.len); 


  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = inetMaskLenToMask(addressType,(L7_uchar8)objipMcastStaticmRoutev4v6MaskValue.addr,
                               &objipMcastStaticmRouteSrcMaskValue);

  if (owa.l7rc != L7_SUCCESS)
  {
   /* owa.l7rc = L7_FAILURE; */
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticmRoutev4v6MaskValue,
                     sizeof (objipMcastStaticmRoutev4v6MaskValue));

  if (inetAddrIsHostBitSet (&objipMcastStaticMRouteSrcIpAddrValue,
                             &objipMcastStaticmRouteSrcMaskValue) == L7_TRUE)
  {
    owa.rc = XLIBRC_MCAST_STATIC_ROUTE_HOST_BITS_SET;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objipMcastStaticMRouteRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* retrieve : ipMcastStaticMRouteRpfAddress */  
    owa.len = sizeof(objipMcastStaticMRouteRPFNextHopValue);
    owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteRPFNextHop,
                          (xLibU8_t *) &objipMcastStaticMRouteRPFNextHopValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    { 
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    
    FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRouteRPFNextHopValue,
                       sizeof (objipMcastStaticMRouteRPFNextHopValue)); 
  
  
    /* retrieve : ipMcastStaticMRoutePreference */  
    owa.len = sizeof(objipMcastStaticMRoutePreferenceValue);
    owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRoutePreference,
                          (xLibU8_t *) &objipMcastStaticMRoutePreferenceValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    { 
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    
    FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRoutePreferenceValue,
                       sizeof (objipMcastStaticMRoutePreferenceValue));
  
  
    /* retrieve : ipMcastStaticMRouteInterface */  
    if(addressType == L7_AF_INET6)
    {
      owa.len = sizeof(objipMcastStaticMRouteInterfaceValue);      
      owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteInterface,
                            (xLibU8_t *) &objipMcastStaticMRouteInterfaceValue, &owa.len);
      if (owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
     
      FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticMRouteInterfaceValue,
                         sizeof (objipMcastStaticMRouteInterfaceValue));

      if (L7_IP6_IS_ADDR_LINK_LOCAL (&(objipMcastStaticMRouteRPFNextHopValue.addr.ipv6)) == 0)
      {
        objipMcastStaticMRouteInterfaceValue = 0;
      }
    }

    /* set the value in application */
    owa.l7rc = usmDbMcastStaticMRouteAdd(L7_UNIT_CURRENT,addressType,
                                   &objipMcastStaticMRouteSrcIpAddrValue,
                                   &objipMcastStaticmRouteSrcMaskValue, 
                                   &objipMcastStaticMRouteRPFNextHopValue,
                                   objipMcastStaticMRouteInterfaceValue,
                                   objipMcastStaticMRoutePreferenceValue);
    if (owa.l7rc == L7_NOT_SUPPORTED)
    {
      owa.rc = XLIBRC_IP_ADDR_NOT_SUPPORTED;    /* TODO: Change if required */
    }
   else if (owa.l7rc == L7_SUCCESS) 
    {
     /* do nothing */
    }
    else if (owa.l7rc == L7_REQUEST_DENIED)
    {
      owa.rc = XLIBRC_RPF_ADDRESS_INTERFACE_INVALID;
    }
    else 
    {
      owa.rc = XLIBRC_ROW_STATUS_ADD;    /* TODO: Change if required */
    }
  }
  else if (objipMcastStaticMRouteRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Deletet the value in application */
    owa.l7rc = usmDbMcastStaticMRouteDelete(L7_UNIT_CURRENT, addressType,
                                         &objipMcastStaticMRouteSrcIpAddrValue,
                                         &objipMcastStaticmRouteSrcMaskValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ROW_STATUS_DELETE;   
    }
  }
  else
  {
     owa.rc = XLIBRC_SUCCESS;    
  } /* end of if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO | L7_ROW_STATUS_DESTROY ) */
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr
*
* @purpose Get 'ipMcastStaticMRouteSrcIpAddr'
 *@description  [ipMcastStaticMRouteSrcIpAddr] <HTML>Source Ip Address   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_inet_addr_t objipMcastStaticMRouteSrcIpAddrValue;      
  L7_inet_addr_t nextObjipMcastStaticMRouteSrcIpAddrValue;  
  L7_inet_addr_t nextObjipMcastStaticMRouteSrcIpMaskValue;  
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  xLibU8_t addressType;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof(objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                          (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

    
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);
  
  if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }  
  /* retrieve key: ipMcastStaticMRouteSrcIpAddr */  
  inetAddressZeroSet(addressType, &nextObjipMcastStaticMRouteSrcIpAddrValue);
  inetAddressZeroSet(addressType, &nextObjipMcastStaticMRouteSrcIpMaskValue);
  owa.len = sizeof (objipMcastStaticMRouteSrcIpAddrValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr,
                          (xLibU8_t *) & objipMcastStaticMRouteSrcIpAddrValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbMcastStaticMRouteEntryNextGet(L7_UNIT_CURRENT,addressType,
                                                  &nextObjipMcastStaticMRouteSrcIpAddrValue,
                                                  &nextObjipMcastStaticMRouteSrcIpMaskValue);
  }
  else
  {
    
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteSrcIpAddrValue, owa.len);
    inetCopy(&nextObjipMcastStaticMRouteSrcIpAddrValue, &objipMcastStaticMRouteSrcIpAddrValue);

    do
    {
      owa.l7rc = usmDbMcastStaticMRouteEntryNextGet (L7_UNIT_CURRENT,addressType,
                                  &nextObjipMcastStaticMRouteSrcIpAddrValue,
                                  &nextObjipMcastStaticMRouteSrcIpMaskValue);
    }while((owa.l7rc == L7_SUCCESS) && 
           (L7_INET_ADDR_COMPARE(&nextObjipMcastStaticMRouteSrcIpAddrValue, &objipMcastStaticMRouteSrcIpAddrValue) == 0));

  }


  if (owa.l7rc != L7_SUCCESS ||
      (L7_INET_ADDR_COMPARE(&nextObjipMcastStaticMRouteSrcIpAddrValue, &objipMcastStaticMRouteSrcIpAddrValue) == 0))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastStaticMRouteSrcIpAddrValue, owa.len);

  /* return the object value: ipMcastStaticMRouteSrcIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastStaticMRouteSrcIpAddrValue,
                           sizeof (nextObjipMcastStaticMRouteSrcIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask
*
* @purpose Get 'ipMcastStaticmRouteSrcMask'
 *@description  [ipMcastStaticmRouteSrcMask] <HTML>Source mask   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibV4V6Mask_t objipMcastStaticmRoutev4v6MaskValue;
  xLibV4V6Mask_t nextObjipMcastStaticmRoutev4v6MaskValue;
  xLibU32_t objipMcastStaticMRouteAddressTypeValue;
  L7_inet_addr_t objipMcastStaticMRouteSrcIpAddrValue;
  L7_inet_addr_t objipMcastStaticmRouteSrcMaskValue;
  L7_inet_addr_t nextObjipMcastStaticMRouteSrcIpAddrValue;
  L7_inet_addr_t nextObjipMcastStaticMRouteSrcIpMaskValue;
  xLibU8_t addressType, maskLen = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastStaticMRouteAddressType */
  owa.len = sizeof(objipMcastStaticMRouteAddressTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteAddressType,
                          (xLibU8_t *) & objipMcastStaticMRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteAddressTypeValue, owa.len);  
  
  if(objipMcastStaticMRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &nextObjipMcastStaticMRouteSrcIpAddrValue);
  inetAddressZeroSet(addressType, &nextObjipMcastStaticMRouteSrcIpMaskValue);

  /* retrieve key: ipMcastStaticMRouteSrcIpAddr */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticMRouteSrcIpAddr,
                          (xLibU8_t *) &objipMcastStaticMRouteSrcIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  inetCopy(&nextObjipMcastStaticMRouteSrcIpAddrValue, &objipMcastStaticMRouteSrcIpAddrValue);

  /* retrieve key: ipMcastStaticMRouteSrcIpMask */  
  owa.len = sizeof (objipMcastStaticmRoutev4v6MaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask,
                          (xLibU8_t *) & objipMcastStaticmRoutev4v6MaskValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(addressType, &nextObjipMcastStaticMRouteSrcIpMaskValue);
    owa.l7rc = usmDbMcastStaticMRouteEntryNextGet(L7_UNIT_CURRENT,addressType,
                                                  &nextObjipMcastStaticMRouteSrcIpAddrValue,
                                                  &nextObjipMcastStaticMRouteSrcIpMaskValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastStaticMRouteSrcIpAddrValue, owa.len);
    inetAddressZeroSet(addressType, &objipMcastStaticmRouteSrcMaskValue);
    owa.l7rc = inetMaskLenToMask(addressType,(L7_uchar8)objipMcastStaticmRoutev4v6MaskValue.addr,
                                 &objipMcastStaticmRouteSrcMaskValue);
    if (owa.l7rc != L7_SUCCESS)
    {
     /* owa.l7rc = L7_FAILURE; */
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    inetCopy(&nextObjipMcastStaticMRouteSrcIpMaskValue, &objipMcastStaticmRouteSrcMaskValue);

    do
    {
      owa.l7rc = usmDbMcastStaticMRouteEntryNextGet (L7_UNIT_CURRENT,addressType,
                                  &nextObjipMcastStaticMRouteSrcIpAddrValue,
                                  &nextObjipMcastStaticMRouteSrcIpMaskValue);
    }while((owa.l7rc == L7_SUCCESS) && 
           (L7_INET_ADDR_COMPARE(&nextObjipMcastStaticMRouteSrcIpAddrValue, &objipMcastStaticMRouteSrcIpAddrValue) == 0)&&
           (L7_INET_ADDR_COMPARE(&nextObjipMcastStaticMRouteSrcIpMaskValue, &objipMcastStaticmRouteSrcMaskValue) == 0));

  }

  if (owa.l7rc != L7_SUCCESS ||
      (L7_INET_ADDR_COMPARE(&nextObjipMcastStaticMRouteSrcIpAddrValue, &objipMcastStaticMRouteSrcIpAddrValue) != 0) ||
      (L7_INET_ADDR_COMPARE(&nextObjipMcastStaticMRouteSrcIpMaskValue, &objipMcastStaticmRouteSrcMaskValue) == 0))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  inetMaskToMaskLen(&nextObjipMcastStaticMRouteSrcIpMaskValue, &maskLen);
  nextObjipMcastStaticmRoutev4v6MaskValue.addr = maskLen;
  nextObjipMcastStaticmRoutev4v6MaskValue.family = addressType;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastStaticMRouteSrcIpMaskValue, owa.len);

  /* return the object value: ipMcastStaticMRouteSrcIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastStaticmRoutev4v6MaskValue,
                           sizeof (nextObjipMcastStaticmRoutev4v6MaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask
*
* @purpose Set 'ipMcastStaticmRouteSrcMask'
 *@description  [ipMcastStaticmRouteSrcMask] <HTML>Source mask   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastStaticMRouteTable_ipMcastStaticmRouteSrcMask (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objipMcastStaticmRouteSrcMaskValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastStaticmRouteSrcMask */
  owa.len = sizeof (objipMcastStaticmRouteSrcMaskValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastStaticmRouteSrcMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastStaticmRouteSrcMaskValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objipMcastStaticmRouteSrcMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
}
