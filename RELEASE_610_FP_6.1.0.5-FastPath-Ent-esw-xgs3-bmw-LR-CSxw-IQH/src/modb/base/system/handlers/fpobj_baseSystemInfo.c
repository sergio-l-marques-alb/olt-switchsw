/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSystemInfo.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseSystemInfo_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_sntp_api.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_mib_vlan_api.h"
L7_RC_t
fpObjUtil_baseSystemInfo_SortServicePortIpv6AddressIndices(L7_in6_prefix_t in[], L7_uint32 out[], L7_uint32 numAddrs)
{
  L7_uint32 i, j, temp;

  for(i = 0; i<numAddrs; i++)
    out[i]=i;
  for(i = 0; i<numAddrs; i++)
  {
    for(j=0; j<numAddrs-(i+1); j++)
    {
      if(memcmp(&in[out[j]], &in[out[j+1]], sizeof(L7_in6_addr_t)) > 0)
      {
        /*swap ths indexes*/
        temp = out[j+1];
        out[j+1] = out[j];
        out[j] = temp;
      }
    }
  }
  return L7_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysName
*
* @purpose Get 'sysName'
*
* @description  An administratively-assigned name for this managed node. By convention, 
*              this is the node's fully-qualified domain name. If 
*              the name is unknown, the value is the zero-length string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysNameGet (L7_UNIT_CURRENT, objsysNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysNameValue, strlen (objsysNameValue));

  /* return the object value: sysName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysNameValue,
                           strlen (objsysNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysName
*
* @purpose Set 'sysName'
*
* @description  An administratively-assigned name for this managed node. By convention, 
*              this is the node's fully-qualified domain name. If 
*              the name is unknown, the value is the zero-length string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objsysNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysNameValue, owa.len);
  if(strlen(objsysNameValue) >= L7_SYS_SIZE )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDb1213SysNameSet (L7_UNIT_CURRENT, objsysNameValue);
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
* @function fpObjGet_baseSystemInfo_sysServices
*
* @purpose Get 'sysServices'
*
* @description  A value which indicates the set of services that this entity 
*              may potentially offer. The value is a sum. This sum initially 
*              takes the value zero. Then, for each layer, L, in the range 
*              1 through 7, that this node 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysServices (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysServicesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysServicesGet (L7_UNIT_CURRENT, &objsysServicesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysServicesValue, sizeof (objsysServicesValue));

  /* return the object value: sysServices */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysServicesValue,
                           sizeof (objsysServicesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysLocation
*
* @purpose Get 'sysLocation'
*
* @description  The physical location of this node (e.g., 'telephone closet, 
*              3rd floor'). If the location is unknown, the value is the zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysLocation (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysLocationValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysLocationGet (L7_UNIT_CURRENT, objsysLocationValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysLocationValue, strlen (objsysLocationValue));

  /* return the object value: sysLocation */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysLocationValue,
                           strlen (objsysLocationValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysLocation
*
* @purpose Set 'sysLocation'
*
* @description  The physical location of this node (e.g., 'telephone closet, 
*              3rd floor'). If the location is unknown, the value is the zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysLocation (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysLocationValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysLocation */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objsysLocationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysLocationValue, owa.len);

  if(strlen(objsysLocationValue) >= L7_SYS_SIZE)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDb1213SysLocationSet (L7_UNIT_CURRENT, objsysLocationValue);
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
* @function fpObjGet_baseSystemInfo_sysObjectID
*
* @purpose Get 'sysObjectID'
*
* @description  The vendor's authoritative identification of the network management 
*              subsystem contained in the entity. This value is allocated 
*              within the SMI enterprises subtree (1.3.6.1.4.1) and provides 
*              an easy and unamb 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysObjectID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysObjectIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysObjectIDGet (L7_UNIT_CURRENT, objsysObjectIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysObjectIDValue, strlen (objsysObjectIDValue));

  /* return the object value: sysObjectID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysObjectIDValue,
                           strlen (objsysObjectIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysDescr
*
* @purpose Get 'sysDescr'
*
* @description  A textual description of the entity. This value should include 
*              the full name and version identification of the system's hardware 
*              type, software operating-system, and networking software. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysDescr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysDescrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysDescrGet (L7_UNIT_CURRENT, objsysDescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysDescrValue, strlen (objsysDescrValue));

  /* return the object value: sysDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysDescrValue,
                           strlen (objsysDescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysUpTime
*
* @purpose Get 'sysUpTime'
*
* @description  The time (in hundredths of a second) since the network management 
*              portion of the system was last re-initialized. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysUpTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysUpTimeRawGet (L7_UNIT_CURRENT, &objsysUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objsysUpTimeValue, sizeof (objsysUpTimeValue));
	

  /* return the object value: sysUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysUpTimeValue,
                           sizeof (objsysUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysContact
*
* @purpose Get 'sysContact'
*
* @description  The textual identification of the contact person for this managed 
*              node, together with information on how to contact this person. 
*              If no contact information is known, the value is the zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysContact (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysContactValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213SysContactGet (L7_UNIT_CURRENT, objsysContactValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysContactValue, strlen (objsysContactValue));

  /* return the object value: sysContact */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysContactValue,
                           strlen (objsysContactValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysContact
*
* @purpose Set 'sysContact'
*
* @description  The textual identification of the contact person for this managed 
*              node, together with information on how to contact this person. 
*              If no contact information is known, the value is the zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysContact (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysContactValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysContact */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objsysContactValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysContactValue, owa.len);

  if(strlen(objsysContactValue) >= L7_SYS_SIZE)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDb1213SysContactSet (L7_UNIT_CURRENT, objsysContactValue);
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
* @function fpObjGet_baseSystemInfo_sysIpAddr
*
* @purpose Get 'sysIpAddr'
*
* @description [sysIpAddr]: The IP Address assigned to the network interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysIpAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAgentIpIfAddressGet  (L7_UNIT_CURRENT, &objsysIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIpAddrValue, sizeof (objsysIpAddrValue));

  /* return the object value: sysIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysIpAddrValue,
                           sizeof (objsysIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysIpAddr
*
* @purpose Set 'sysIpAddr'
*
* @description [sysIpAddr]: The IP Address assigned to the network interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysIpAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIpAddrValue,objsysIpNetMaskValue,objsysAgentIpIfDefaultRouterValue;
  xLibU32_t oldIpAddr,errorNum, val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysIpAddr */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIpAddrValue, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysIpNetMask,
                          (xLibU8_t *) &objsysIpNetMaskValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysAgentIpIfDefaultRouter,
                          (xLibU8_t *) &objsysAgentIpIfDefaultRouterValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

 /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_UNIT_CURRENT,&val);
  if (val != L7_SYSCONFIG_MODE_NONE)
  {
    owa.rc = XLIBRC_SYSINFO_IPCFG_ERROR;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbAgentIpIfAddressGet (0, &oldIpAddr);


  if (objsysIpAddrValue == oldIpAddr) 
  {
    return XLIBRC_SUCCESS;
  }

  if (objsysIpNetMaskValue != 0)
  {

    if (usmDbIpAddressValidate(objsysIpAddrValue, objsysIpNetMaskValue) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_SERVPORT_IPMASKCFG_ERROR;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if (objsysAgentIpIfDefaultRouterValue != 0)
    { 
      if (usmDbIpInfoValidate (objsysIpAddrValue, objsysIpNetMaskValue, objsysAgentIpIfDefaultRouterValue) != L7_SUCCESS)
      {
        owa.rc =  XLIBRC_SERVPORT_IPGATEWAYCFG_ERROR;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
  }

  if (objsysIpNetMaskValue != 0)
  {
    /* set up the netmask, ip addr with user specified values */
    if (usmDbSystemIPAndNetMaskSet(L7_UNIT_CURRENT, objsysIpAddrValue,
                       objsysIpNetMaskValue, &errorNum) != L7_SUCCESS)  /* SET IP/NETMASK */
    {
      /* If we could not set the IP and NetMask, reset the gateway, and return with the given error message */
      usmDbAgentIpIfDefaultRouterSet(L7_UNIT_CURRENT, objsysAgentIpIfDefaultRouterValue);   /* RESET GATEWAY */

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
      usmDbAgentIpIfAddressSet (L7_UNIT_CURRENT, objsysIpAddrValue);
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
* @function fpObjGet_baseSystemInfo_sysIpNetMask
*
* @purpose Get 'sysIpNetMask'
*
* @description [sysIpNetMask]: The Unit's System NetMask 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysIpNetMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIpNetMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAgentIpIfNetMaskGet(L7_UNIT_CURRENT, &objsysIpNetMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIpNetMaskValue,
                     sizeof (objsysIpNetMaskValue));

  /* return the object value: sysIpNetMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysIpNetMaskValue,
                           sizeof (objsysIpNetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysIpNetMask
*
* @purpose Set 'sysIpNetMask'
*
* @description [sysIpNetMask]: The Unit's System NetMask 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysIpNetMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIpNetMaskValue,objsysIpAddrValue,objsysAgentIpIfDefaultRouterValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t  oldNetMask,errorNum, val;

  /* retrieve object: sysIpNetMask */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysIpNetMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIpNetMaskValue, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysIpAddr,
                          (xLibU8_t *) &objsysIpAddrValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysAgentIpIfDefaultRouter,
                          (xLibU8_t *) &objsysAgentIpIfDefaultRouterValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_UNIT_CURRENT,&val);
  if (val != L7_SYSCONFIG_MODE_NONE)
  {
    owa.rc = XLIBRC_SYSINFO_IPCFG_ERROR;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbAgentIpIfNetMaskGet (0, &oldNetMask);


  if (objsysIpNetMaskValue == oldNetMask) 
  {
    return XLIBRC_SUCCESS;
  }

  if (objsysIpAddrValue != 0)
  {

    if (usmDbIpAddressValidate(objsysIpAddrValue, objsysIpNetMaskValue) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_SERVPORT_IPMASKCFG_ERROR;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if (objsysAgentIpIfDefaultRouterValue != 0)
    { 
      if (usmDbIpInfoValidate (objsysIpAddrValue, objsysIpNetMaskValue, objsysAgentIpIfDefaultRouterValue) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
  }

  if (objsysIpAddrValue != 0)
  {
    /* set up the netmask, ip addr with user specified values */
    if (usmDbSystemIPAndNetMaskSet(L7_UNIT_CURRENT,objsysIpAddrValue, 
                   objsysIpNetMaskValue, &errorNum) != L7_SUCCESS)  /* SET IP/NETMASK */
    {
      /* If we could not set the IP and NetMask, reset the gateway, and return with the given error message */
      usmDbAgentIpIfDefaultRouterSet(L7_UNIT_CURRENT, objsysAgentIpIfDefaultRouterValue);   /* RESET GATEWAY */

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
      usmDbAgentIpIfNetMaskSet (L7_UNIT_CURRENT, objsysIpNetMaskValue);
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
* @function fpObjGet_baseSystemInfo_sysAgentIpIfDefaultRouter
*
* @purpose Get 'sysAgentIpIfDefaultRouter'
*
* @description [sysAgentIpIfDefaultRouter]: Unit's System Gateway 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysAgentIpIfDefaultRouter (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentIpIfDefaultRouterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAgentIpIfDefaultRouterGet (L7_UNIT_CURRENT, &objsysAgentIpIfDefaultRouterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentIpIfDefaultRouterValue,
                     sizeof (objsysAgentIpIfDefaultRouterValue));

  /* return the object value: sysAgentIpIfDefaultRouter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysAgentIpIfDefaultRouterValue,
                    sizeof (objsysAgentIpIfDefaultRouterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysAgentIpIfDefaultRouter
*
* @purpose Set 'sysAgentIpIfDefaultRouter'
*
* @description [sysAgentIpIfDefaultRouter]: Unit's System Gateway 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysAgentIpIfDefaultRouter (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentIpIfDefaultRouterValue,objsysIpNetMaskValue,objsysIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t  oldGateway, val;

  /* retrieve object: sysAgentIpIfDefaultRouter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysAgentIpIfDefaultRouterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentIpIfDefaultRouterValue, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysIpAddr,
                          (xLibU8_t *) &objsysIpAddrValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysIpNetMask,
                          (xLibU8_t *) &objsysIpNetMaskValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &, owa.len);

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_UNIT_CURRENT,&val);
  if (val != L7_SYSCONFIG_MODE_NONE)
  {
    owa.rc = XLIBRC_SYSINFO_IPCFG_ERROR;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbAgentIpIfDefaultRouterGet (0, &oldGateway);

  if (objsysAgentIpIfDefaultRouterValue == oldGateway) 
  {
    owa.rc = XLIBRC_SUCCESS;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objsysIpAddrValue != 0 && objsysIpNetMaskValue != 0)
  {
    /* Verify the validity of the ip address, subnet mask & gateway together */
    if (usmDbIpInfoValidate(objsysIpAddrValue, objsysIpNetMaskValue, objsysAgentIpIfDefaultRouterValue) != L7_SUCCESS)
    {
      /* If the triad is invalid return without doing anything */
      owa.rc = XLIBRC_SERVPORT_IPGATEWAYCFG_ERROR;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* if gateway is not 0, and if setting the gateway fails */
  if ( objsysAgentIpIfDefaultRouterValue != 0 && 
           usmDbAgentIpIfDefaultRouterSet(L7_UNIT_CURRENT,objsysAgentIpIfDefaultRouterValue) != L7_SUCCESS) 
  {
    /* Revert back changes and display reason for failure, does not check validity */
    usmDbAgentIpIfDefaultRouterSet(L7_UNIT_CURRENT, oldGateway);     /* RESET GATEWAY */
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysPortsPerBoxGet
*
* @purpose Get 'sysPortsPerBoxGet'
*
* @description the total number of ports per box 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysPortsPerBoxGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysPortsPerBoxGetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbPortsPerBoxGet (L7_UNIT_CURRENT, &objsysPortsPerBoxGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysPortsPerBoxGetValue,
                     sizeof (objsysPortsPerBoxGetValue));

  /* return the object value: sysPortsPerBoxGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysPortsPerBoxGetValue,
                           sizeof (objsysPortsPerBoxGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysMgmtVlanId
*
* @purpose Get 'sysMgmtVlanId'
*
* @description the management vlan id. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysMgmtVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysMgmtVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMgmtVlanIdGet (L7_UNIT_CURRENT, &objsysMgmtVlanIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysMgmtVlanIdValue,
                     sizeof (objsysMgmtVlanIdValue));

  /* return the object value: sysMgmtVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysMgmtVlanIdValue,
                           sizeof (objsysMgmtVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysMgmtVlanId
*
* @purpose Set 'sysMgmtVlanId'
*
* @description the management vlan id. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysMgmtVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysMgmtVlanIdValue;
#ifdef L7_ROUTING_PACKAGE
  xLibU32_t intIfNum;
#endif
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysMgmtVlanId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysMgmtVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysMgmtVlanIdValue, owa.len);

  /* set the value in application */
#ifdef L7_ROUTING_PACKAGE
    /* if this VLAN is enabled for routing, return a failure */
    if (usmDbIpVlanRtrVlanIdToIntIfNum(0, objsysMgmtVlanIdValue, &intIfNum) == L7_SUCCESS)
    {
      owa.rc = XLIBRC_MGMTVLAN_VLAN_ROUTING_MODE_CANT_CONF;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
#endif

  owa.l7rc = usmDbMgmtVlanIdSet (L7_UNIT_CURRENT, objsysMgmtVlanIdValue);
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
* @function fpObjGet_baseSystemInfo_sysMgmtPort
*
* @purpose Get 'sysMgmtPort'
*
* @description the managementport 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysMgmtPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysMgmtPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMgmtPortGet (L7_UNIT_CURRENT, &objsysMgmtPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysMgmtPortValue, sizeof (objsysMgmtPortValue));

  /* return the object value: sysMgmtPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysMgmtPortValue,
                           sizeof (objsysMgmtPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysMgmtPort
*
* @purpose Set 'sysMgmtPort'
*
* @description the managementport 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysMgmtPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysMgmtPortValue,mgmtVlan;
  FPOBJ_TRACE_ENTER (bufp);
 
  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysMgmtVlanId,
                          (xLibU8_t *) &mgmtVlan, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  /* Check if VLAN exists */
  owa.l7rc = usmDbVlanIDGet ( L7_UNIT_CURRENT, mgmtVlan);

  if ((owa.l7rc ==L7_NOT_EXISTS) || ( owa.l7rc == L7_FAILURE))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* retrieve object: sysMgmtPort */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysMgmtPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysMgmtPortValue, owa.len);

  /* Set PVID of the preffered management port */
  owa.l7rc = usmDbQportsPVIDSet ( L7_UNIT_CURRENT, objsysMgmtPortValue, mgmtVlan);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 
  /* Set participation mode for the interface in the managment VLAN */
  owa.l7rc = usmDbVlanMemberSet ( L7_UNIT_CURRENT , mgmtVlan, objsysMgmtPortValue , L7_DOT1Q_FIXED,DOT1Q_SWPORT_MODE_GENERAL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbMgmtPortSet (L7_UNIT_CURRENT, objsysMgmtPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjList_baseSystemInfo_sysMgmtPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMgmtPortValue, nextPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSystemInfo_sysMgmtPort,
                          (xLibU8_t *) & objMgmtPortValue, &owa.len);

 if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet (&nextPortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMgmtPortValue, owa.len);
    owa.l7rc = usmDbNextIntIfNumberByTypeGet (L7_PHYSICAL_INTF, objMgmtPortValue , &nextPortValue);
  }


   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextPortValue, owa.len);

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextPortValue,
                           sizeof (nextPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysIPFirstLocalAddress
*
* @purpose Get 'sysIPFirstLocalAddress'
*
* @description Finds first valid IP address on this system.returns 0.0.0.0 if 
*              no valid IP addresses are found 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysIPFirstLocalAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIPFirstLocalAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  usmDbIPFirstLocalAddress (&objsysIPFirstLocalAddressValue);
  owa.l7rc = L7_SUCCESS;

  FPOBJ_TRACE_VALUE (bufp, &objsysIPFirstLocalAddressValue,
                     sizeof (objsysIPFirstLocalAddressValue));

  /* return the object value: sysIPFirstLocalAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysIPFirstLocalAddressValue,
                           sizeof (objsysIPFirstLocalAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysAgentBasicConfigNetworkConfigProtocol
*
* @purpose Get 'sysAgentBasicConfigNetworkConfigProtocol'
*
* @description The units basic network protocol being set 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysAgentBasicConfigNetworkConfigProtocol (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentBasicConfigNetworkConfigProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet (L7_UNIT_CURRENT,
                                                          &objsysAgentBasicConfigNetworkConfigProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentBasicConfigNetworkConfigProtocolValue,
                     sizeof (objsysAgentBasicConfigNetworkConfigProtocolValue));

  /* return the object value: sysAgentBasicConfigNetworkConfigProtocol */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objsysAgentBasicConfigNetworkConfigProtocolValue,
                    sizeof (objsysAgentBasicConfigNetworkConfigProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysAgentBasicConfigNetworkConfigProtocol
*
* @purpose Set 'sysAgentBasicConfigNetworkConfigProtocol'
*
* @description The units basic network protocol being set 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysAgentBasicConfigNetworkConfigProtocol (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentBasicConfigNetworkConfigProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  xLibU32_t oldVal,servPortVal;
  

    /* retrieve object: sysAgentBasicConfigNetworkConfigProtocol */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objsysAgentBasicConfigNetworkConfigProtocolValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentBasicConfigNetworkConfigProtocolValue,
                     owa.len);

  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet (L7_UNIT_CURRENT, &oldVal);

  if (oldVal == objsysAgentBasicConfigNetworkConfigProtocolValue)
  {
    return XLIBRC_SUCCESS; 
  } 
 
  if (objsysAgentBasicConfigNetworkConfigProtocolValue == L7_SYSCONFIG_MODE_NONE)
  {
    if (oldVal == L7_SYSCONFIG_MODE_DHCP)
    {
      usmDbDhcp_release (L7_UNIT_CURRENT);
    }
  }
  else if (objsysAgentBasicConfigNetworkConfigProtocolValue == L7_SYSCONFIG_MODE_DHCP)
  {
    usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_UNIT_CURRENT,&servPortVal);
    if (servPortVal == L7_SYSCONFIG_MODE_NONE)
    {
      if (oldVal == L7_SYSCONFIG_MODE_DHCP)
      {
        usmDbDhcp_release(L7_UNIT_CURRENT);
      }
    }
    else
    {
       owa.rc = XLIBRC_SYSINFO_PROTOCFG_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
  }
  else if (objsysAgentBasicConfigNetworkConfigProtocolValue == L7_SYSCONFIG_MODE_BOOTP)
  {
    usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_UNIT_CURRENT,&servPortVal);
    if (servPortVal == L7_SYSCONFIG_MODE_NONE)
    {
      if (oldVal == L7_SYSCONFIG_MODE_DHCP)
      {
        usmDbDhcp_release(L7_UNIT_CURRENT);
      }
    }
    else
    {
       owa.rc = XLIBRC_SYSINFO_PROTOCFG_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
  }
    
  usmDbAgentIpIfDefaultRouterSet (L7_UNIT_CURRENT, 0);
  usmDbAgentIpIfAddressSet (L7_UNIT_CURRENT, 0);
  usmDbAgentIpIfNetMaskSet (L7_UNIT_CURRENT, 0);
  usmDbAgentBasicConfigProtocolDesiredSet (L7_UNIT_CURRENT, objsysAgentBasicConfigNetworkConfigProtocolValue);

  if (oldVal == L7_SYSCONFIG_MODE_BOOTP)
  {
    usmDbBootpTaskReInit(L7_UNIT_CURRENT);
  }

  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysCommandPrompt
*
* @purpose Get 'sysCommandPrompt'
*
* @description CLI Command prompt string 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysCommandPrompt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysCommandPromptValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCommandPromptGet (L7_UNIT_CURRENT, objsysCommandPromptValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysCommandPromptValue,
                     strlen (objsysCommandPromptValue));

  /* return the object value: sysCommandPrompt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysCommandPromptValue,
                           strlen (objsysCommandPromptValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_sysCommandPrompt
*
* @purpose Set 'sysCommandPrompt'
*
* @description CLI Command prompt string 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_sysCommandPrompt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysCommandPromptValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysCommandPrompt */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objsysCommandPromptValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysCommandPromptValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCommandPromptSet (L7_UNIT_CURRENT, objsysCommandPromptValue);
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
* @function fpObjGet_baseSystemInfo_TerminalLineLength
*
* @purpose Get 'TerminalLineLength'
*
* @description the Terminal Lines for show running-config 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_TerminalLineLength (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTerminalLineLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTerminalLineGet (L7_UNIT_CURRENT, &objTerminalLineLengthValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTerminalLineLengthValue,
                     sizeof (objTerminalLineLengthValue));

  /* return the object value: TerminalLineLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTerminalLineLengthValue,
                           sizeof (objTerminalLineLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSystemInfo_TerminalLineLength
*
* @purpose Set 'TerminalLineLength'
*
* @description the Terminal Lines for show running-config 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSystemInfo_TerminalLineLength (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTerminalLineLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TerminalLineLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTerminalLineLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTerminalLineLengthValue, owa.len);

  /* set the value in application */
/*  owa.l7rc = usmDbTerminalLineSet (L7_UNIT_CURRENT, objTerminalLineLengthValue);*/
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
* @function fpObjGet_baseSystemInfo_sysSNTPSyncTime
*
* @purpose Get 'sysSNTPSyncTime'
*
* @description  currently synchronized SNTP time in UTC. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysSNTPSyncTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysSNTPSyncTime;
  L7_uint32 val;
  memset(objsysSNTPSyncTime, 0x00, sizeof(objsysSNTPSyncTime));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  owa.l7rc = usmDbSntpLastUpdateTimeGet (L7_UNIT_CURRENT, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
   if (  val == 0)
   {
      sprintf(objsysSNTPSyncTime, "Not Synchronized");
   }
   else
   { 
      val = osapiUTCTimeNow();
      osapiStrncpySafe(objsysSNTPSyncTime, (const char *)usmDbConvertTimeToDateString(val), 21);
      osapiStrncat(objsysSNTPSyncTime," UTC", sizeof(objsysSNTPSyncTime));
   }
   FPOBJ_TRACE_VALUE (bufp, objsysSNTPSyncTime, strlen (objsysSNTPSyncTime));

  /* return the object value: sysDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysSNTPSyncTime,
                           strlen (objsysSNTPSyncTime));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysCurrentDate
*
* @purpose Get 'sysDescr'
*
* @description  The current system date. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysCurrentDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysCurrentDateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  L7_uint32 currentTime;
  struct tm *today;

  currentTime = osapiUTCTimeNow();
  today = localtime((time_t *)&currentTime);
  memset(objsysCurrentDateValue, 0x00, sizeof(objsysCurrentDateValue));
  osapiSnprintf(objsysCurrentDateValue, sizeof(objsysCurrentDateValue), "%.2d/%.2d/%d", today->tm_mon + 1, today->tm_mday, today->tm_year + 1900);
						
  owa.l7rc = L7_SUCCESS;
  FPOBJ_TRACE_VALUE (bufp, objsysCurrentDateValue, strlen (objsysCurrentDateValue));

  /* return the object value: sysDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysCurrentDateValue,
                           strlen (objsysCurrentDateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSystemInfo_sysCurrentTime
*
* @purpose Get 'sysCurrentTime'
*
* @description  The current system Time to display in HH:MM:SS format. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSystemInfo_sysCurrentTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysCurrentTimeValue;

  struct tm *LocalTime;
  xLibU32_t currentTime;

  FPOBJ_TRACE_ENTER (bufp);

  /* Display current time information */
  currentTime = usmDbAdjustedTimeGet();
  LocalTime = localtime((time_t *)&currentTime);

  memset(objsysCurrentTimeValue, 0x00, sizeof(objsysCurrentTimeValue));
  osapiSnprintf(objsysCurrentTimeValue, sizeof(objsysCurrentTimeValue), "%.2d:%.2d:%.2d", LocalTime->tm_hour, LocalTime->tm_min, LocalTime->tm_sec);
						
  owa.l7rc = L7_SUCCESS;
  FPOBJ_TRACE_VALUE (bufp, objsysCurrentTimeValue, strlen (objsysCurrentTimeValue));

  /* return the object value: sysDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysCurrentTimeValue,
                           strlen (objsysCurrentTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


