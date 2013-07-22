/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingswitchIPInterfaceConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ipconfig-object.xml
*
* @create  6 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingswitchIPInterfaceConfig_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_rtrdisc_api.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_ip6_api.h"
#include "usmdb_sim_api.h"

#ifdef L7_CLI_PACKAGE
extern L7_BOOL cliIntfIsNamedByUSP(L7_uint32 intfNum);
#endif

#ifndef L7_CLI_PACKAGE
L7_BOOL cliIntfIsNamedByUSP(L7_uint32 intfNum)
{
  L7_uint32 intfType;
  if ((usmDbIntfTypeGet(intfNum, &intfType) == L7_SUCCESS) &&
      (intfType == L7_LOOPBACK_INTF || intfType == L7_TUNNEL_INTF))
  {
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}
#endif

/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_IfIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: The IfIndex associated with this instance. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_IfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF |USM_LOGICAL_VLAN_INTF | USM_LOGICAL_LOOPBACK_INTF, 0,
                                         &nextObjIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    nextObjIfIndexValue = objIfIndexValue;
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF | USM_LOGICAL_LOOPBACK_INTF, 0,
                                        objIfIndexValue, &nextObjIfIndexValue);    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue,
                           sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_ipAddress
*
* @purpose Get 'ipAddress'
*
* @description [ipAddress]: The IpAddress assigned to this interface. When
*              setting this value, the value of agentSwitchIpInterfaceNetMask
*              must be set at the same time. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_ipAddress (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAddressValue;
  xLibU32_t objMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                         &objipAddressValue, &objMaskValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipAddressValue,
                           sizeof (objipAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_ipAddress
*
* @purpose Set 'ipAddress'
*
* @description [ipAddress]: The IpAddress assigned to this interface. When
*              setting this value, the value of agentSwitchIpInterfaceNetMask
*              must be set at the same time. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_ipAddress (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t ipMask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipAddressValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);


  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_netMask,
                          (xLibU8_t *) & ipMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_netMask
*
* @purpose Get 'netMask'
*
* @description [netMask]: The NetMask assigned to this interface. When setting
*              this value, the value of agentSwitchIpInterfaceIpAddress
*              must be set at the same time. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_netMask (void *wap, void *bufp)
{
                                                                             /* tHIS FUNCTION IS NOT REQUIRED SINCE WE DONT HAVE SET FUNCTION */ 
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnetMaskValue;
  xLibU32_t objIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet(L7_UNIT_CURRENT, keyIfIndexValue, &objIpAddrValue, &objnetMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: netMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objnetMaskValue,
                           sizeof (objnetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_netMask
*
* @purpose Set 'netMask'
*
* @description [netMask]: The NetMask assigned to this interface. When setting
*              this value, the value of agentSwitchIpInterfaceIpAddress
*              must be set at the same time. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_netMask (void *wap, void *bufp)
{
                                                                              /*tHIS FUNCTION IS NOT REQUIRED SINCE WE DONT HAVE SET FUNCTION*/  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnetMaskValue;
  xLibU32_t objIpAddrValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: netMask */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objnetMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnetMaskValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_ipAddress,
                          (xLibU8_t *) &objIpAddrValue , &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddrValue, kwa.len);
  if (usmDbNetmaskValidate(objnetMaskValue) != L7_SUCCESS)
  {
     owa.rc = XLIBRC_INVALID_SUBNET_MASK;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbIpRtrIntfIPAddressSet(L7_UNIT_CURRENT, keyIfIndexValue, objIpAddrValue, 
                                        objnetMaskValue, L7_INTF_IP_ADDR_METHOD_CONFIG);

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
* @function fpObjSet_routingswitchIPInterfaceConfig_clearIp
*
* @purpose Set 'clearIp'
*
* @description [clearIp]: Sets the interfaces IpAddress and NetMask back to
*              0.0.0.0 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_clearIp (void *wap, void *bufp)
{

       /*THIS FUNCTION EXPECTING TWO MORE VALUES IN CLI. BUT WE HAVE ONLY ONE ARGUEMENT. HOW TO GET THOSE OTHER TWO OBJECTS*/                            
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddrValue;
  xLibU32_t ipMask;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: clearIp 
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objclearIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objclearIpValue, owa.len);
*/
  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_ipAddress,
                          (xLibU8_t *) &objIpAddrValue , &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddrValue, kwa.len);


  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_netMask,
                          (xLibU8_t *) & ipMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &ipMask, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRtrIntfIPAddressRemove (L7_UNIT_CURRENT, keyIfIndexValue,objIpAddrValue,ipMask);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PRIMARY_ADDRESS_REMOVAL_FAILED; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_routingMode
*
* @purpose Get 'routingMode'
*
* @description [routingMode]: Enables or disables routing for this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_routingMode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objroutingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbIpRtrIntfModeGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                    &objroutingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: routingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objroutingModeValue,
                           sizeof (objroutingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_routingMode
*
* @purpose Set 'routingMode'
*
* @description [routingMode]: Enables or disables routing for this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_routingMode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objroutingModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: routingMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objroutingModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objroutingModeValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbIpRtrIntfModeSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                    objroutingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

#ifdef L7_IPV6_PACKAGE
  owa.l7rc = usmDbIp6RtrIntfModeSet(keyIfIndexValue,
                                    objroutingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_proxyARPMode
*
* @purpose Get 'proxyARPMode'
*
* @description [proxyARPMode]: Enables or disables Proxy ARP for this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_proxyARPMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objproxyARPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  if(cliIntfIsNamedByUSP(keyIfIndexValue) == L7_FALSE)
  {
     /* SET_ERR_MSG 
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1); */
     owa.rc = XLIBRC_FAILURE;  
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbProxyArpGet (L7_UNIT_CURRENT, keyIfIndexValue,
                               &objproxyARPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: proxyARPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objproxyARPModeValue,
                           sizeof (objproxyARPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_proxyARPMode
*
* @purpose Set 'proxyARPMode'
*
* @description [proxyARPMode]: Enables or disables Proxy ARP for this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_proxyARPMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objproxyARPModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: proxyARPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objproxyARPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objproxyARPModeValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);
 
  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbProxyArpSet (L7_UNIT_CURRENT, keyIfIndexValue,
                               objproxyARPModeValue);
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
* @function fpObjGet_routingswitchIPInterfaceConfig_mtuValue
*
* @purpose Get 'mtuValue'
*
* @description [mtuValue]: Configures the MTU value for this interface. If
*              the value returned is 0 this interface is not enabled for
*              routing. It is not valid to set this value to 0 if routing
*              is enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_mtuValue (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmtuValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIntfEffectiveIpMtuGet(keyIfIndexValue,
                                &objmtuValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mtuValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmtuValueValue,
                           sizeof (objmtuValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_mtuValue
*
* @purpose Set 'mtuValue'
*
* @description [mtuValue]: Configures the MTU value for this interface. If
*              the value returned is 0 this interface is not enabled for
*              routing. It is not valid to set this value to 0 if routing
*              is enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_mtuValue (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmtuValueValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mtuValue */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmtuValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmtuValueValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbIntfIpMtuSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                objmtuValueValue);
  if (owa.l7rc == L7_ERROR)
  {
    owa.rc = XLIBRC_IP_MTU_OUT_OF_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_bandwidth
*
* @purpose Get 'bandwidth'
*
* @description [bandwidth]: Configures the Bandwidth value for this interface.
*              If the value returned is 0 bandwidth for this interface
*              is not Configured. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_bandwidth (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objbandwidthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  if(cliIntfIsNamedByUSP(keyIfIndexValue) == L7_FALSE)
  {
     /* SET_ERR_MSG 
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1); */
     owa.rc = XLIBRC_FAILURE;    
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  } 

  /* get the value from application */
  owa.l7rc = usmDbIfBandwidthGet (keyIfIndexValue,
                                  &objbandwidthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: bandwidth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objbandwidthValue,
                           sizeof (objbandwidthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_bandwidth
*
* @purpose Set 'bandwidth'
*
* @description [bandwidth]: Configures the Bandwidth value for this interface.
*              If the value returned is 0 bandwidth for this interface
*              is not Configured. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_bandwidth (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objbandwidthValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: bandwidth */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objbandwidthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objbandwidthValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbIfBandwidthSet (keyIfIndexValue, objbandwidthValue);
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
* @function fpObjGet_routingswitchIPInterfaceConfig_unnumberedIfIndex
*
* @purpose Get 'unnumberedIfIndex'
*
* @description [unnumberedIfIndex]: If this object is non-zero, it indicates
*              that the interface is unnumbered, and specifies which interface
*              the address is borrowed from. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_unnumberedIfIndex (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objunnumberedIfIndexValue;
  xLibU32_t numberedIfc;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  if(cliIntfIsNamedByUSP(keyIfIndexValue) == L7_FALSE)
  {
    /* SET_ERR_MSG 
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);*/
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

  /* get the value from application */
  owa.l7rc = usmDbIpUnnumberedGet (keyIfIndexValue, &objunnumberedIfIndexValue, &numberedIfc);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: unnumberedIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objunnumberedIfIndexValue,
                           sizeof (objunnumberedIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_unnumberedIfIndex
*
* @purpose Set 'unnumberedIfIndex'
*
* @description [unnumberedIfIndex]: If this object is non-zero, it indicates
*              that the interface is unnumbered, and specifies which interface
*              the address is borrowed from. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_unnumberedIfIndex (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objunnumberedIfIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t numberedIfc=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: unnumberedIfIndex */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objunnumberedIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objunnumberedIfIndexValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpUnnumberedSet (keyIfIndexValue, objunnumberedIfIndexValue, numberedIfc);
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
* @function fpObjGet_routingswitchIPInterfaceConfig_IcmpUnreachables
*
* @purpose Get 'IcmpUnreachables'
 *@description  [IcmpUnreachables] If this object is enabled, it indicates that
* ICMP unreachables can be sent on this interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_IcmpUnreachables (void *wap, void *bufp)
{

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIcmpUnreachablesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbIpMapICMPUnreachablesModeGet (L7_UNIT_CURRENT, keyIfIndexValue, &objIcmpUnreachablesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IcmpUnreachables */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIcmpUnreachablesValue,
                           sizeof (objIcmpUnreachablesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_IcmpUnreachables
*
* @purpose Set 'IcmpUnreachables'
 *@description  [IcmpUnreachables] If this object is enabled, it indicates that
* ICMP unreachables can be sent on this interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_IcmpUnreachables (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIcmpUnreachablesValue;

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IcmpUnreachables */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIcmpUnreachablesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIcmpUnreachablesValue, owa.len);

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbIpMapICMPUnreachablesModeSet (L7_UNIT_CURRENT, keyIfIndexValue, objIcmpUnreachablesValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_IcmpRedirects
*
* @purpose Get 'IcmpRedirects'
 *@description  [IcmpRedirects] If this object is enabled and
* agentSwitchIpIcmpRedirectsMode is also enabled, ICMP Redirects can be sent on this
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_IcmpRedirects (void *wap, void *bufp)
{

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIcmpRedirectsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbIpMapIfICMPRedirectsModeGet (L7_UNIT_CURRENT, keyIfIndexValue, &objIcmpRedirectsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IcmpRedirects */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIcmpRedirectsValue,
                           sizeof (objIcmpRedirectsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_IcmpRedirects
*
* @purpose Set 'IcmpRedirects'
 *@description  [IcmpRedirects] If this object is enabled and
* agentSwitchIpIcmpRedirectsMode is also enabled, ICMP Redirects can be sent on this
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_IcmpRedirects (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIcmpRedirectsValue;

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IcmpRedirects */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIcmpRedirectsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIcmpRedirectsValue, owa.len);

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbIpMapIfICMPRedirectsModeSet (L7_UNIT_CURRENT, keyIfIndexValue, objIcmpRedirectsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_netDirectBroadcasts
*
* @purpose Get 'netDirectBroadcasts'
*
* @description [netDirectBroadcasts]: Configures Ip regarding forwarding net
*              directed broadcasts 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_netDirectBroadcasts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnetDirectBroadcastsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(cliIntfIsNamedByUSP(keyIfIndexValue) == L7_FALSE)
  {
    /* SET_ERR_MSG 
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1); */
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIpNetDirectBcastsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                        &objnetDirectBroadcastsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: netDirectBroadcasts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objnetDirectBroadcastsValue,
                           sizeof (objnetDirectBroadcastsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_netDirectBroadcasts
*
* @purpose Set 'netDirectBroadcasts'
*
* @description [netDirectBroadcasts]: Configures Ip regarding forwarding net
*              directed broadcasts 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_netDirectBroadcasts (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnetDirectBroadcastsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: netDirectBroadcasts */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objnetDirectBroadcastsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnetDirectBroadcastsValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbIpNetDirectBcastsSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                        objnetDirectBroadcastsValue);
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
* @function fpObjGet_routingswitchIPInterfaceConfig_irdpHoldTime
*
* @purpose Get 'irdpHoldTime'
*
* @description [irdpHoldTime]: Sets the value of lifetime field of router
*              advertisement sent from the interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_irdpHoldTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objirdpHoldTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  owa.l7rc = usmDbRtrDiscIsValidIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = usmDbRtrDiscAdvLifetimeGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                         &objirdpHoldTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: irdpHoldTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objirdpHoldTimeValue,
                           sizeof (objirdpHoldTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_irdpHoldTime
*
* @purpose Set 'irdpHoldTime'
*
* @description [irdpHoldTime]: Sets the value of lifetime field of router
*              advertisement sent from the interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_irdpHoldTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objirdpHoldTimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: irdpHoldTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objirdpHoldTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objirdpHoldTimeValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT,keyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbRtrDiscAdvLifetimeSet (L7_UNIT_CURRENT, keyIfIndexValue,
                                         objirdpHoldTimeValue);
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
* @function fpObjGet_routingswitchIPConfig_IsPrimaryConfigured
*
* @purpose Get 'IsPrimaryConfigured'
 *@description  [IsPrimaryConfigured] <HTML>Returns True if Primary Address is
* Configured
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_IsPrimaryConfigured (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_INTF_IP_ADDR_METHOD_t objIsPrimaryConfiguredValue;
  xLibU32_t keyIfIndexValue;
  xLibU32_t ipval, subval;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.len = sizeof (keyIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, owa.len);

  /* get the value from application */
  if (keyIfIndexValue != 0)
  {
    owa.l7rc = ((usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT, keyIfIndexValue, &ipval, &subval)) ||
                (usmDbIpRtrIntfIpAddressMethodGet(keyIfIndexValue,
                                                  &objIsPrimaryConfiguredValue)));
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (ipval == 0)
  {
    objIsPrimaryConfiguredValue = L7_INTF_IP_ADDR_METHOD_NONE;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objIsPrimaryConfiguredValue, sizeof (objIsPrimaryConfiguredValue));

  /* return the object value: IsPrimaryConfigured */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsPrimaryConfiguredValue,
                           sizeof (objIsPrimaryConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_routingswitchIPInterfaceConfig_IPAddressConfigMethod
*
* @purpose Get 'ConfigMethod'
*
* @description [ConfigMethod]: Get the method of configuring IpAddress for an interface.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPInterfaceConfig_IPAddressConfigMethod(void *wap,
                                                                       void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMethod;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressMethodGet(keyIfIndexValue, 
                                              (void *)&objMethod);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMethod,
                           sizeof (objMethod));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_IPAddressConfigMethod
*
* @purpose Get 'ConfigMethod'
*
* @description [ConfigMethod]: Set the method of configuring IpAddress for an 
*                              interface.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_IPAddressConfigMethod(void *wap,
                                                                       void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_INTF_IP_ADDR_METHOD_t  objMethod;
  L7_INTF_IP_ADDR_METHOD_t  oldObjMethod = L7_INTF_IP_ADDR_METHOD_NONE;
  L7_BOOL actImmediate = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: IPAddressConfigMethod */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMethod, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMethod, owa.len);
  
  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if(kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* Get the current address configuration from the application */
  owa.l7rc = usmDbIpRtrIntfIpAddressMethodGet (keyIfIndexValue, (void *)&oldObjMethod);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* NOTE: When the address method is set to None, IPMAP API posts an event
   * to the DHCP Client Task which inturn sends out a Release message and
   * then removes the Address configuration from IPMAP.  This might take about
   * 2-3 seconds as it is event based mechanism.
   * So, when the page is refreshed before this happens, there is a possibility
   * that it might still show the address there by misleading the user.
   * So in such scenarios' release the address immediately in the caller's
   * task context so that the user is not confused.
   */
  if ((oldObjMethod == L7_INTF_IP_ADDR_METHOD_DHCP) &&
      (objMethod == L7_INTF_IP_ADDR_METHOD_NONE))
  {
    actImmediate = L7_TRUE;
  }

  /* set the value in application */
  owa.l7rc = usmDbIntfIpAddressMethodSet (keyIfIndexValue, objMethod,
                                          L7_MGMT_IPPORT, actImmediate);

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_renewDHCP
*
* @purpose Set to 'renewDHCP'
*
* @description [renewDHCP]: To renew IPaddress configured through DHCP on a 
*                           particular interface.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_renewDHCP(void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: renewDHCP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objValue,
                           &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if(kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIntfIpAddressMethodSet (keyIfIndexValue, L7_INTF_IP_ADDR_RENEW,
                                          L7_MGMT_IPPORT, L7_FALSE);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingswitchIPInterfaceConfig_releaseDHCP
*
* @purpose Set 'releaseDHCP'
*
* @description [releaseDHCP]: To release IPaddress configured through DHCP on a 
*                            particular interface.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPInterfaceConfig_releaseDHCP(void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnetDirectBroadcastsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: releaseDHCP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objnetDirectBroadcastsValue,
                           &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnetDirectBroadcastsValue, owa.len);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if(kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIntfIpAddressMethodSet (keyIfIndexValue,
                                          L7_INTF_IP_ADDR_RELEASE,
                                          L7_MGMT_IPPORT, L7_FALSE);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


