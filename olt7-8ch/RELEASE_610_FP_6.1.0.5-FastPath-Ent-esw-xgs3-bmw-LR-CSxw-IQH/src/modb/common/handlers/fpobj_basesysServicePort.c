/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basesysServicePort.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  6 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basesysServicePort_obj.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_base_api.h"

/*******************************************************************************
* @function fpObjGet_basesysServicePort_sysAgentBasicConfigServPortConfigProtocol
*
* @purpose Get 'sysAgentBasicConfigServPortConfigProtocol'
*
* @description The units basic service port config mode 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_basesysServicePort_sysAgentBasicConfigServPortConfigProtocol (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentBasicConfigServPortConfigProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAgentBasicConfigServPortConfigProtocolDesiredGet (L7_UNIT_CURRENT,
                                                           &objsysAgentBasicConfigServPortConfigProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentBasicConfigServPortConfigProtocolValue,
                     sizeof
                     (objsysAgentBasicConfigServPortConfigProtocolValue));

  /* return the object value: sysAgentBasicConfigServPortConfigProtocol */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objsysAgentBasicConfigServPortConfigProtocolValue,
                    sizeof (objsysAgentBasicConfigServPortConfigProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysServicePort_sysAgentBasicConfigServPortConfigProtocol
*
* @purpose Set 'sysAgentBasicConfigServPortConfigProtocol'
*
* @description The units basic service port config mode 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_basesysServicePort_sysAgentBasicConfigServPortConfigProtocol (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentBasicConfigServPortConfigProtocolValue;
  xLibU32_t oldVal,networkVal;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysAgentBasicConfigServPortConfigProtocol */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objsysAgentBasicConfigServPortConfigProtocolValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentBasicConfigServPortConfigProtocolValue,
                     owa.len);


  /* Get the old protocol configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_UNIT_CURRENT,&oldVal);

  if (oldVal == objsysAgentBasicConfigServPortConfigProtocolValue)
  {
    return XLIBRC_SUCCESS; 
  } 
 
  if (objsysAgentBasicConfigServPortConfigProtocolValue == L7_SYSCONFIG_MODE_NONE)
  {
    if (oldVal == L7_SYSCONFIG_MODE_DHCP)
    {
      usmDbDhcp_release(L7_UNIT_CURRENT);
    }
  }
  else if (objsysAgentBasicConfigServPortConfigProtocolValue == L7_SYSCONFIG_MODE_BOOTP)
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_UNIT_CURRENT,&networkVal);
    if (networkVal == L7_SYSCONFIG_MODE_NONE)
    {
      if (oldVal == L7_SYSCONFIG_MODE_DHCP)
      {
        usmDbDhcp_release(L7_UNIT_CURRENT);
      }
    }
    else
    {
       owa.rc = XLIBRC_SERVPORT_PROTOCFG_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
  }
  else if (objsysAgentBasicConfigServPortConfigProtocolValue == L7_SYSCONFIG_MODE_DHCP)
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_UNIT_CURRENT,&networkVal);
    if (networkVal == L7_SYSCONFIG_MODE_NONE)
    {
      if (oldVal == L7_SYSCONFIG_MODE_DHCP)
      {
        usmDbDhcp_release(L7_UNIT_CURRENT);
      } 
    }
    else
    {
       owa.rc = XLIBRC_SERVPORT_PROTOCFG_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
  }

  usmDbServicePortGatewaySet(L7_UNIT_CURRENT, 0);
  usmDbServicePortIPAddrSet(L7_UNIT_CURRENT, 0);
  usmDbServicePortNetMaskSet(L7_UNIT_CURRENT, 0);
 

  /* set the value in application */
  owa.l7rc =
    usmDbAgentBasicConfigServPortProtocolDesiredSet (L7_UNIT_CURRENT,
                                                     objsysAgentBasicConfigServPortConfigProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (oldVal == L7_SYSCONFIG_MODE_BOOTP)
  {
    usmDbBootpTaskReInit(L7_UNIT_CURRENT);
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysServicePort_sysServicePortGateway
*
* @purpose Get 'sysServicePortGateway'
*
* @description the service port Gateway 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysServicePort_sysServicePortGateway (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicePortGatewayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbServicePortGatewayGet (L7_UNIT_CURRENT,
                                &objsysServicePortGatewayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicePortGatewayValue,
                     sizeof (objsysServicePortGatewayValue));

  /* return the object value: sysServicePortGateway */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysServicePortGatewayValue,
                           sizeof (objsysServicePortGatewayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysServicePort_sysServicePortGateway
*
* @purpose Set 'sysServicePortGateway'
*
* @description the service port Gateway 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysServicePort_sysServicePortGateway (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicePortGatewayValue,objsysServicePortIPAddrValue,objsysServicePortNetMask;
  xLibU32_t oldGateway,protocolVal;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysServicePortGateway */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysServicePortGatewayValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicePortGatewayValue, owa.len);

 owa.rc = xLibFilterGet (wap, XOBJ_basesysServicePort_sysServicePortIPAddr,
                          (xLibU8_t *) &objsysServicePortIPAddrValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_basesysServicePort_sysServicePortNetMask,
                          (xLibU8_t *) &objsysServicePortNetMask, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_UNIT_CURRENT,&protocolVal);
  if (protocolVal != L7_SYSCONFIG_MODE_NONE)
  {
    owa.rc = XLIBRC_SERVPORT_IPCFG_ERROR;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbServicePortGatewayGet(L7_UNIT_CURRENT, &oldGateway);
  if(oldGateway == objsysServicePortGatewayValue)
  {
    owa.rc = XLIBRC_SUCCESS;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objsysServicePortIPAddrValue != 0 && objsysServicePortNetMask != 0)
  {
    /* Verify the validity of the ip address, subnet mask & gateway together */
    if (usmDbIpInfoValidate(objsysServicePortIPAddrValue, objsysServicePortNetMask, objsysServicePortGatewayValue) != L7_SUCCESS)
    {
      /* If the triad is invalid return without doing anything */
      owa.rc = XLIBRC_SERVPORT_IPGATEWAYCFG_ERROR;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* if gateway is not 0, and if setting the gateway fails */
  if ( objsysServicePortGatewayValue != 0 && usmDbServicePortGatewaySet(L7_UNIT_CURRENT, objsysServicePortGatewayValue) != L7_SUCCESS) 
  {
    /* Revert back changes and display reason for failure, does not check validity */
    usmDbServicePortGatewaySet(L7_UNIT_CURRENT, oldGateway);     /* RESET GATEWAY */
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysServicePort_sysServicePortIPAddr
*
* @purpose Get 'sysServicePortIPAddr'
*
* @description The service port IP Addr 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysServicePort_sysServicePortIPAddr (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicePortIPAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbServicePortIPAddrGet (L7_UNIT_CURRENT, &objsysServicePortIPAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicePortIPAddrValue,
                     sizeof (objsysServicePortIPAddrValue));

  /* return the object value: sysServicePortIPAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysServicePortIPAddrValue,
                           sizeof (objsysServicePortIPAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysServicePort_sysServicePortIPAddr
*
* @purpose Set 'sysServicePortIPAddr'
*
* @description The service port IP Addr 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysServicePort_sysServicePortIPAddr (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicePortIPAddrValue,objsysServicePortNetMask,objsysServicePortGateway;
  xLibU32_t oldIpAddr,protocolVal;
  xLibU32_t errorNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysServicePortIPAddr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysServicePortIPAddrValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicePortIPAddrValue, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_basesysServicePort_sysServicePortNetMask,
                          (xLibU8_t *) &objsysServicePortNetMask, &owa.len);
  
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_basesysServicePort_sysServicePortGateway,
                          (xLibU8_t *) &objsysServicePortGateway, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_UNIT_CURRENT,&protocolVal);
  if (protocolVal != L7_SYSCONFIG_MODE_NONE)
  {
    owa.rc = XLIBRC_SERVPORT_IPCFG_ERROR;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbServicePortIPAddrGet(L7_UNIT_CURRENT, &oldIpAddr);
  if(oldIpAddr == objsysServicePortIPAddrValue)
  {
    owa.rc = XLIBRC_SUCCESS;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objsysServicePortNetMask != 0)
  {
    if (usmDbIpAddressValidate(objsysServicePortIPAddrValue, objsysServicePortNetMask) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_SERVPORT_IPMASKCFG_ERROR;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if (objsysServicePortGateway  != 0)
    {
      /* Verify the validity of the ip address, subnet mask & gateway together */
      if (usmDbIpInfoValidate(objsysServicePortIPAddrValue, objsysServicePortNetMask, objsysServicePortGateway) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_SERVPORT_IPGATEWAYCFG_ERROR;  
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
  }


  if (objsysServicePortNetMask  != 0)
  {
    /* set up the netmask, ip addr with user specified values */
    if (usmDbServicePortIPAndNetMaskSet(L7_UNIT_CURRENT, objsysServicePortIPAddrValue,
                       objsysServicePortNetMask , &errorNum) != L7_SUCCESS)  /* SET IP/NETMASK */
    {
      /* If we could not set the IP and NetMask, reset the gateway, and return with the given error message */
      usmDbServicePortGatewaySet(L7_UNIT_CURRENT, objsysServicePortGateway);   /* RESET GATEWAY */

      switch(errorNum)
      {
        case 7101:
          owa.rc = XLIBRC_SERVPORT_IPMASKCFG_SWT_CONFLICT_ERROR;  
          break;
        case 7102:
          owa.rc = XLIBRC_SERVPORT_IPMASKCFG_SERVPORT_CONFLICT_ERROR;  
          break;
        case 7103:
          owa.rc = XLIBRC_SERVPORT_IPMASKCFG_RTRINTF_CONFLICT_ERROR;  
          break;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
    /* set the value in application */
    owa.l7rc =
      usmDbServicePortIPAddrSet (L7_UNIT_CURRENT, objsysServicePortIPAddrValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysServicePort_sysServicePortNetMask
*
* @purpose Get 'sysServicePortNetMask'
*
* @description The service port NetMask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysServicePort_sysServicePortNetMask (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicePortNetMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbServicePortNetMaskGet (L7_UNIT_CURRENT,
                                &objsysServicePortNetMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicePortNetMaskValue,
                     sizeof (objsysServicePortNetMaskValue));

  /* return the object value: sysServicePortNetMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysServicePortNetMaskValue,
                           sizeof (objsysServicePortNetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysServicePort_sysServicePortNetMask
*
* @purpose Set 'sysServicePortNetMask'
*
* @description The service port NetMask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysServicePort_sysServicePortNetMask (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicePortNetMaskValue,objsysServicePortIPAddrValue,objsysServicePortGateway;
  xLibU32_t oldNetMask,protocolVal;
  xLibU32_t errorNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysServicePortNetMask */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysServicePortNetMaskValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicePortNetMaskValue, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_basesysServicePort_sysServicePortIPAddr,
                          (xLibU8_t *) &objsysServicePortIPAddrValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_basesysServicePort_sysServicePortGateway,
                          (xLibU8_t *) &objsysServicePortGateway, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_UNIT_CURRENT,&protocolVal);
  if (protocolVal != L7_SYSCONFIG_MODE_NONE)
  {
    owa.rc = XLIBRC_SERVPORT_IPCFG_ERROR;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbServicePortNetMaskGet(L7_UNIT_CURRENT, &oldNetMask);
  if(oldNetMask == objsysServicePortNetMaskValue)
  {
    owa.rc = XLIBRC_SUCCESS;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objsysServicePortIPAddrValue  != 0)
  {
    if (usmDbIpAddressValidate(objsysServicePortIPAddrValue , objsysServicePortNetMaskValue) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_SERVPORT_IPMASKCFG_ERROR;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if (objsysServicePortGateway != 0)
    {
      /* Verify the validity of the ip address, subnet mask & gateway together */
      if (usmDbIpInfoValidate(objsysServicePortIPAddrValue, objsysServicePortNetMaskValue, objsysServicePortGateway) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_SERVPORT_IPGATEWAYCFG_ERROR;  
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
  }


  if (objsysServicePortIPAddrValue != 0)
  {
    /* set up the netmask, ip addr with user specified values */
    if (usmDbServicePortIPAndNetMaskSet(L7_UNIT_CURRENT, objsysServicePortIPAddrValue,
                       objsysServicePortNetMaskValue, &errorNum) != L7_SUCCESS)  /* SET IP/NETMASK */
    {
      /* If we could not set the IP and NetMask, reset the gateway, and return with the given error message */
      usmDbServicePortGatewaySet(L7_UNIT_CURRENT, objsysServicePortGateway);   /* RESET GATEWAY */

      switch(errorNum)
      {
        case 7101:
          owa.rc = XLIBRC_SERVPORT_IPMASKCFG_SWT_CONFLICT_ERROR;  
          break;
        case 7102:
          owa.rc = XLIBRC_SERVPORT_IPMASKCFG_SERVPORT_CONFLICT_ERROR;  
          break;
        case 7103:
          owa.rc = XLIBRC_SERVPORT_IPMASKCFG_RTRINTF_CONFLICT_ERROR;  
          break;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
    /* set the value in application */
    owa.l7rc =
      usmDbServicePortNetMaskSet (L7_UNIT_CURRENT, objsysServicePortNetMaskValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysServicePort_sysServicePortMacAddress
*
* @purpose Get 'sysServicePortMacAddress'
*
* @description The service port Mac Address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysServicePort_sysServicePortMacAddress (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr6_t objsysServicePortMacAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbServPortMacAddressGet (L7_UNIT_CURRENT,
                                objsysServicePortMacAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysServicePortMacAddressValue,
                     sizeof (objsysServicePortMacAddressValue));

  /* return the object value: sysServicePortMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysServicePortMacAddressValue,
                           sizeof (objsysServicePortMacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

