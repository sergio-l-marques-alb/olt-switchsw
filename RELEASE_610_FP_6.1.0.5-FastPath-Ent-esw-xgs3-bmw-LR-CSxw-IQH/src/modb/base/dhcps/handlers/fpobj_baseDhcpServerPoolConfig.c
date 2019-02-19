/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerPoolConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to snmp-object.xml
*
* @create  9 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDhcpServerPoolConfig_obj.h"
#include "usmdb_dhcps_api.h"
#include "dhcps_exports.h"
#include "usmdb_util_api.h"
/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_PoolIndex
*
* @purpose Get 'PoolIndex'
*
* @description The Pool Index, which 'll be used as index for the PoolConfig 
*              Table. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_PoolIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoolIndexValue;
  xLibU32_t nextObjPoolIndexValue;
  xLibStr256_t poolNameTemp;

  FPOBJ_TRACE_ENTER (bufp);
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));

  /* retrieve key: PoolIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolIndex,
                          (xLibU8_t *) & objPoolIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
   /* nextObjPoolIndexValue =0;
    owa.l7rc = usmDbDhcpsPoolEntryFirst(L7_UNIT_CURRENT, poolNameTemp, &nextObjPoolIndexValue);*/
    objPoolIndexValue = 0;
    owa.l7rc = usmDbDhcpsPoolGetNext (L7_UNIT_CURRENT, objPoolIndexValue,
                                      &nextObjPoolIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPoolIndexValue, owa.len);
    owa.l7rc = usmDbDhcpsPoolGetNext (L7_UNIT_CURRENT, objPoolIndexValue,
                                      &nextObjPoolIndexValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPoolIndexValue, owa.len);

  /* return the object value: PoolIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPoolIndexValue,
                           sizeof (objPoolIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_PoolName
*
* @purpose Get 'PoolName'
*
* @description The name of the DHCP Address pool. This value cannot be modified 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_PoolName (void *wap, void *bufp)
{
 fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objPoolIndexValue;
  xLibStr256_t poolNameTemp;

  FPOBJ_TRACE_ENTER (bufp);
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));

  /* retrieve key: PoolIndex */
  owa.rc = xLibFilterGet (wap,XOBJ_baseDhcpServerPoolConfig_PoolIndex,
                          (xLibU8_t *) & objPoolIndexValue, &owa.len);

 if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPoolIndexValue = 0;
     if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT,objPoolIndexValue,poolNameTemp) != L7_SUCCESS)
     {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }

  }
  else
  {

   if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT,objPoolIndexValue, poolNameTemp) != L7_SUCCESS)
     {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }




  FPOBJ_TRACE_NEW_KEY (bufp, poolNameTemp, owa.len);

  /* return the object value: PoolIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) poolNameTemp,
                           strlen (poolNameTemp));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_PoolName
*
* @purpose Set 'PoolName'
*
* @description The name of the DHCP Address pool. This value cannot be modified. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_PoolName (void *wap, void *bufp)
{

  return XLIBRC_SUCCESS;

}
 

/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_DefRouter
*
* @purpose Get 'DefRouter'
*
* @description This specifies the default-router list(each default-router is 
*              separated by comma and the list is terminated by semi-colon) 
*              for a DHCP client, an example would be 10.10.1.1,192.168.36.1,157.227.44.1;(no 
*              spaces in b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DefRouter (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDefRouterValue;
  xLibStr256_t poolNameTemp;
  xLibU32_t  routersList[L7_DHCPS_DEFAULT_ROUTER_MAX];
  xLibU32_t count = 0;
  xLibU32_t tempAddr;

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));	
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsRoutersGet (L7_UNIT_CURRENT, poolNameTemp,
                                   routersList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  for(count = 0; count < L7_DHCPS_DEFAULT_ROUTER_MAX ; count++)
  {
	  tempAddr = routersList[count];
      memcpy(objDefRouterValue+(count*4), &tempAddr, sizeof(L7_uint32) );
  }

  /* return the object value: DefRouter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefRouterValue,
                          ( L7_DHCPS_DEFAULT_ROUTER_MAX * sizeof(L7_uint32) ));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DefRouter
*
* @purpose Set 'DefRouter'
*
* @description This specifies the default-router list(each default-router is 
*              separated by comma and the list is terminated by semi-colon) 
*              for a DHCP client, an example would be 10.10.1.1,192.168.36.1,157.227.44.1;(no 
*              spaces in b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DefRouter (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDefRouterValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;

  xLibStr256_t strRouter;
  xLibU32_t intIPaddr[L7_DHCPS_DEFAULT_ROUTER_MAX];
  xLibU32_t count, tempAddr;
  xLibBool_t routerFound;
	

  memset(strRouter, 0x00, sizeof(strRouter));
  memset(intIPaddr, 0x00, sizeof(intIPaddr));
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: DefRouter */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefRouterValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefRouterValue, owa.len);
  routerFound = XLIB_FALSE;
  for(count = 0; count < L7_DHCPS_DEFAULT_ROUTER_MAX ; count++)
  {
      memcpy(&tempAddr, objDefRouterValue+(count*4), sizeof(L7_uint32) );
	   intIPaddr[count] = tempAddr;
		if(tempAddr != 0)
			routerFound = XLIB_TRUE;
  }

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if(routerFound == XLIB_FALSE)
  {
	    owa.l7rc = usmDbDhcpsDefaultRoutersIpReset(L7_UNIT_CURRENT,poolNameTemp);
  }
  else
  {
	  owa.l7rc = usmDbDhcpsRoutersSet (L7_UNIT_CURRENT, poolNameTemp,
	                                   intIPaddr);
  }
	
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
* @function fpObjGet_baseDhcpServerPoolConfig_DNSServer
*
* @purpose Get 'DNSServer'
*
* @description This specifies the DNS IP servers(each DNS IP server is separated 
*              by comma and the list is terminated by semi-colon) for a 
*              DHCP client, an example would be 10.10.1.1,192.168.36.1,157.227.44.1;(no 
*              spaces in between 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DNSServer (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDNSServerValue;
  xLibStr256_t poolNameTemp;
  xLibU32_t  serverList[L7_DHCPS_DNS_SERVER_MAX];
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  xLibU32_t count = 0;
  xLibU32_t tempAddr;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsDnsServersGet (L7_UNIT_CURRENT, poolNameTemp,
                                      serverList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  for(count = 0; count < L7_DHCPS_DNS_SERVER_MAX ; count++)
  {
	  tempAddr = serverList[count];
      memcpy(objDNSServerValue+(count*4), &tempAddr, sizeof(L7_uint32) );
  }

  /* return the object value: DefRouter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDNSServerValue,
                          ( L7_DHCPS_DNS_SERVER_MAX * sizeof(L7_uint32) ));
 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DNSServer
*
* @purpose Set 'DNSServer'
*
* @description This specifies the DNS IP servers(each DNS IP server is separated 
*              by comma and the list is terminated by semi-colon) for a 
*              DHCP client, an example would be 10.10.1.1,192.168.36.1,157.227.44.1;(no 
*              spaces in between 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DNSServer (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDNSServerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  xLibStr256_t strRouter;
  xLibU32_t intIPaddr[L7_DHCPS_DNS_SERVER_MAX];

   xLibU32_t count, tempAddr;
   xLibBool_t routerFound;

  memset(strRouter, 0x00, sizeof(strRouter));
  memset(intIPaddr, 0x00, sizeof(intIPaddr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DNSServer */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDNSServerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDNSServerValue, owa.len);

  routerFound = XLIB_FALSE;
  for(count = 0; count < L7_DHCPS_DNS_SERVER_MAX ; count++)
  {
      memcpy(&tempAddr, objDNSServerValue+(count*4), sizeof(L7_uint32) );
	   intIPaddr[count] = tempAddr;
		if(tempAddr != 0)
			routerFound = XLIB_TRUE;
  }
   /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if(routerFound == XLIB_FALSE)
  {
	    owa.l7rc = usmDbDhcpsDnsServersReset(L7_UNIT_CURRENT,poolNameTemp);
  }
  else
  {
	  owa.l7rc = usmDbDhcpsDnsServersSet (L7_UNIT_CURRENT, poolNameTemp,
	                                   intIPaddr);
  }

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
* @function fpObjGet_baseDhcpServerPoolConfig_LeaseTime
*
* @purpose Get 'LeaseTime'
*
* @description This specifies the duration of the lease(in minutes) for an IP 
*              address that is assigened from a DHCP server to a DHCP client. 
*              For infinite leases, lease-time value will be set to 86400. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_LeaseTime (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaseTimeValue;
  xLibStr256_t poolNameTemp;
 
 memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsLeaseTimeGet (L7_UNIT_CURRENT, &objLeaseTimeValue,
                                                                     poolNameTemp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LeaseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLeaseTimeValue,
                           sizeof (objLeaseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_LeaseTime
*
* @purpose Set 'LeaseTime'
*
* @description This specifies the duration of the lease(in minutes) for an IP 
*              address that is assigened from a DHCP server to a DHCP client. 
*              For infinite leases, lease-time value will be set to 86400. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_LeaseTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaseTimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
  
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));

  /* retrieve object: LeaseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLeaseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLeaseTimeValue, owa.len);


  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsLeaseTimeSet (L7_UNIT_CURRENT, objLeaseTimeValue,
                                     poolNameTemp);
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
* @function fpObjGet_baseDhcpServerPoolConfig_Type
*
* @purpose Get 'Type'
*
* @description This specifies the type of the binding that is associated with 
*              this pool. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsPoolTypeGet (L7_UNIT_CURRENT, poolNameTemp,
                                    &objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeValue,
                           sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_Type
*
* @purpose Set 'Type'
*
* @description 
*              
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve object: LeaseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  owa.l7rc = L7_SUCCESS;
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
* @function fpObjGet_baseDhcpServerPoolConfig_NetbiosNameServer
*
* @purpose Get 'NetbiosNameServer'
*
* @description This specifies the IP address-list of Net BIOS name-servers that 
*              are available to DHCP clients (each IP address in this list 
*              is separated by comma and the list is terminated by semi-colon) 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_NetbiosNameServer (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNetbiosNameServerValue;
  xLibStr256_t poolNameTemp;

 memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  xLibU32_t  serverList[L7_DHCPS_DNS_SERVER_MAX];

  xLibU32_t count = 0;
  xLibU32_t tempAddr;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc =
    usmDbDhcpsNetbiosNameServerAddressGet (L7_UNIT_CURRENT, poolNameTemp,
                                           serverList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  for(count = 0; count < L7_DHCPS_DNS_SERVER_MAX ; count++)
  {
	  tempAddr = serverList[count];
      memcpy(objNetbiosNameServerValue+(count*4), &tempAddr, sizeof(L7_uint32) );
  }

  /* return the object value: DefRouter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objNetbiosNameServerValue,
                          ( L7_DHCPS_DNS_SERVER_MAX * sizeof(L7_uint32) ));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_NetbiosNameServer
*
* @purpose Set 'NetbiosNameServer'
*
* @description This specifies the IP address-list of Net BIOS name-servers that 
*              are available to DHCP clients (each IP address in this list 
*              is separated by comma and the list is terminated by semi-colon) 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_NetbiosNameServer (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNetbiosNameServerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;

   xLibStr256_t strRouter;
  xLibU32_t intIPaddr[L7_DHCPS_NETBIOS_NAME_SERVER_MAX];

  xLibU32_t count, tempAddr;
   xLibBool_t routerFound;

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  memset(strRouter, 0x00, sizeof(strRouter));
  memset(intIPaddr, 0x00, sizeof(intIPaddr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NetbiosNameServer */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objNetbiosNameServerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objNetbiosNameServerValue, owa.len);

  routerFound = XLIB_FALSE;
  for(count = 0; count < L7_DHCPS_NETBIOS_NAME_SERVER_MAX ; count++)
  {
      memcpy(&tempAddr, objNetbiosNameServerValue+(count*4), sizeof(L7_uint32) );
	   intIPaddr[count] = tempAddr;
		if(tempAddr != 0)
			routerFound = XLIB_TRUE;
  }


  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if(routerFound == XLIB_FALSE)
  {
	    owa.l7rc = usmDbDhcpsNetbiosNameServerAddressDelete(L7_UNIT_CURRENT,poolNameTemp);
  }
  else
  {
	  owa.l7rc = usmDbDhcpsNetbiosNameServerAddressSet (L7_UNIT_CURRENT, poolNameTemp,
	                                   intIPaddr);
  }
	
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
* @function fpObjGet_baseDhcpServerPoolConfig_NetbiosNodeType
*
* @purpose Get 'NetbiosNodeType'
*
* @description This specifies the NetBIOS node-type for DHCP clients. 'none' 
*              is shown if node-type is not set to any of the four values. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_NetbiosNodeType (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNetbiosNodeTypeValue;
  xLibStr256_t poolNameTemp;
  xLibU8_t tempValue;


 memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  /* get the value from application */
  owa.l7rc = usmDbDhcpsNetbiosNodeTypeGet (L7_UNIT_CURRENT, poolNameTemp,
                                           &tempValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objNetbiosNodeTypeValue = (xLibU32_t)tempValue;

  /* return the object value: NetbiosNodeType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNetbiosNodeTypeValue,
                           sizeof (objNetbiosNodeTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_NetbiosNodeType
*
* @purpose Set 'NetbiosNodeType'
*
* @description This specifies the NetBIOS node-type for DHCP clients. 'none' 
*              is shown if node-type is not set to any of the four values. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_NetbiosNodeType (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNetbiosNodeTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;

  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve object: NetbiosNodeType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNetbiosNodeTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNetbiosNodeTypeValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsNetbiosNodeTypeSet (L7_UNIT_CURRENT, poolNameTemp,
                                           objNetbiosNodeTypeValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_NextServer
*
* @purpose Get 'NextServer'
*
* @description This configures the next server in the boot-process of a DHCP 
*              clients.User should enter zero(0.0.0.0) to remove the current 
*              Next-server address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_NextServer (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNextServerValue;
  xLibStr256_t poolNameTemp;
 
 memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  owa.l7rc = usmDbDhcpsNextServerGet (L7_UNIT_CURRENT, poolNameTemp,
                                      &objNextServerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NextServer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNextServerValue,
                           sizeof (objNextServerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_NextServer
*
* @purpose Set 'NextServer'
*
* @description This configures the next server in the boot-process of a DHCP 
*              clients.User should enter zero(0.0.0.0) to remove the current 
*              Next-server address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_NextServer (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNextServerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NextServer */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNextServerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNextServerValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  owa.l7rc = usmDbDhcpsNextServerSet (L7_UNIT_CURRENT, poolNameTemp,
                                      objNextServerValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_DomainName
*
* @purpose Get 'DomainName'
*
* @description This specifies the domain-name for a DHCP client. User should 
*              enter a 'null' or 'NULL' string to remove specified domain-name. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DomainName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDomainNameValue;
  xLibStr256_t poolNameTemp;
  
  FPOBJ_TRACE_ENTER (bufp);

 memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
	
  owa.l7rc = usmDbDhcpsDomainNameGet (L7_UNIT_CURRENT, poolNameTemp,
                                      objDomainNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DomainName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDomainNameValue,
                           strlen (objDomainNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DomainName
*
* @purpose Set 'DomainName'
*
* @description This specifies the domain-name for a DHCP client. User should 
*              enter a 'null' or 'NULL' string to remove specified domain-name. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DomainName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDomainNameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
   xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

 memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve object: DomainName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDomainNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDomainNameValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsDomainNameSet (L7_UNIT_CURRENT, poolNameTemp,
                                      objDomainNameValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_Bootfile
*
* @purpose Get 'Bootfile'
*
* @description This specifies the name of the default boot image for a DHCP client.User 
*              should enter a 'null' or 'NULL' string to remove specified 
*              bootfile name. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_Bootfile (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objBootfileValue;
  xLibStr256_t poolNameTemp;

 memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  owa.l7rc = usmDbDhcpsBootfileGet (L7_UNIT_CURRENT, poolNameTemp,
                                    objBootfileValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Bootfile */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objBootfileValue,
                           strlen (objBootfileValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_Bootfile
*
* @purpose Set 'Bootfile'
*
* @description This specifies the name of the default boot image for a DHCP client.User 
*              should enter a 'null' or 'NULL' string to remove specified 
*              bootfile name. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_Bootfile (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objBootfileValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;

 memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
 FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Bootfile */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objBootfileValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objBootfileValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if( strlen(objBootfileValue) == 0 )
 {
    owa.l7rc = usmDbDhcpsBootfileDelete(L7_UNIT_CURRENT,poolNameTemp);
 }
 else
 {
  	owa.l7rc = usmDbDhcpsBootfileSet (L7_UNIT_CURRENT, poolNameTemp,
                                    objBootfileValue);
 }
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
* @function fpObjSet_baseDhcpServerPoolConfig_BootfileDelete
*
* @purpose Set 'BootfileDelete'
 *@description  [BootfileDelete] Delete the bootfile for the given pool.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_BootfileDelete (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBootfileDeleteValue;

  fpObjWa_t kwaPoolIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  

  xLibStr256_t poolNameTemp;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BootfileDelete */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBootfileDeleteValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBootfileDeleteValue, owa.len);

  /* retrieve key: PoolIndex*/
  kwaPoolIndex.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolIndex,
                                   (xLibU8_t *) & poolNameTemp, &kwaPoolIndex.len);
  if (kwaPoolIndex.rc != XLIBRC_SUCCESS)
  {
    kwaPoolIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPoolIndex);
    return kwaPoolIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwaPoolIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbDhcpsBootfileDelete (L7_UNIT_CURRENT, poolNameTemp);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_AllocationName
*
* @purpose Get 'AllocationName'
*
* @description This specifies name of the DHCP Address pool having dynamic binding. 
*              This value is same as DhcpServerPoolName of the DhcpServerPoolConfigTable. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_AllocationName (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAllocationNameValue,poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);


  /* return the object value: AllocationName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAllocationNameValue,
                           strlen (objAllocationNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_DynamicPoolIpAddress
*
* @purpose Get 'DynamicPoolIpAddress'
*
* @description The IP address of the DHCP Address pool. The IP address must be 
*              set before setting the IP Mask or IP Prefix Length. After setting 
*              IP address, network -mask, or IP Prefix length must be 
*              set in order to change the 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DynamicPoolIpAddress (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicPoolIpAddressValue;
  xLibU32_t ipMaskTemp;
  xLibStr256_t poolNameTemp;

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  owa.l7rc = usmDbDhcpsNetworkGet (L7_UNIT_CURRENT, poolNameTemp,
                                   &objDynamicPoolIpAddressValue, &ipMaskTemp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DynamicPoolIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicPoolIpAddressValue,
                           sizeof (objDynamicPoolIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DynamicPoolIpAddress
*
* @purpose Set 'DynamicPoolIpAddress'
*
* @description The IP address of the DHCP Address pool. The IP address must be 
*              set before setting the IP Mask or IP Prefix Length. After setting 
*              IP address, network -mask, or IP Prefix length must be 
*              set in order to change the 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DynamicPoolIpAddress (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicPoolIpAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
 
 memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DynamicPoolIpAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDynamicPoolIpAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDynamicPoolIpAddressValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_DynamicPoolIpMask
*
* @purpose Get 'DynamicPoolIpMask'
*
* @description The bit-combination that renders which portion of the address 
*              of the DHCP address pool refers to the network or subnet and 
*              which part refers to the host.The IP address must have been set 
*              before. This value shows 0 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DynamicPoolIpMask (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicPoolIpMaskValue;
    xLibU32_t ipAddrTemp;
  xLibStr256_t poolNameTemp;
 
 memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

	
  owa.l7rc = usmDbDhcpsNetworkGet (L7_UNIT_CURRENT, poolNameTemp,
                                   &ipAddrTemp, &objDynamicPoolIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DynamicPoolIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicPoolIpMaskValue,
                           sizeof (objDynamicPoolIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DynamicPoolIpMask
*
* @purpose Set 'DynamicPoolIpMask'
*
* @description The bit-combination that renders which portion of the address 
*              of the DHCP address pool refers to the network or subnet and 
*              which part refers to the host.The IP address must have been set 
*              before. This value shows 0 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DynamicPoolIpMask (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicPoolIpMaskValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibU32_t objDynamicPoolIpAddress,prevIpAddr=0,prevIpMask=0;
  xLibStr256_t poolNameTemp;
 memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DynamicPoolIpMask */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDynamicPoolIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDynamicPoolIpMaskValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  /* retrieve key: DynamicPoolIPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_DynamicPoolIpAddress,
                          (xLibU8_t *) & objDynamicPoolIpAddress, &owa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicPoolIpAddress, kwa.len);

  owa.l7rc = usmDbDhcpsNetworkGet (L7_UNIT_CURRENT, poolNameTemp, &prevIpAddr, &prevIpMask);
  if(owa.l7rc == L7_SUCCESS)
  {
    if(prevIpAddr != 0x0)
    {
      if((prevIpAddr != objDynamicPoolIpAddress) || (prevIpMask != objDynamicPoolIpMaskValue))
      {
        owa.l7rc = usmDbDhcpsNetworkDelete(L7_UNIT_CURRENT, poolNameTemp);
        if(owa.l7rc != L7_SUCCESS)
        {
          owa.rc = XLIBRC_FAILURE;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      }
      else if((prevIpAddr == objDynamicPoolIpAddress)&& (prevIpMask == objDynamicPoolIpMaskValue))
      {
          owa.rc = XLIBRC_SUCCESS;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
      }   

    }
  }

  owa.l7rc = usmDbDhcpsNetworkSet (L7_UNIT_CURRENT, poolNameTemp, objDynamicPoolIpAddress, objDynamicPoolIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if(prevIpAddr != 0)
    {
      usmDbDhcpsNetworkSet (L7_UNIT_CURRENT, poolNameTemp, prevIpAddr, prevIpMask);
    } 
    owa.rc = XLIBRC_DHCPS_POOL_NETWORK_SET_FAILED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_Network
*
* @purpose Set 'Network'
*
* @description [Network]: set the network . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_Network (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNetworkValue;
  fpObjWa_t kwaPoolIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t poolNameTemp;  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Network */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNetworkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNetworkValue, owa.len);

  /* retrieve key: PoolIndex*/
  kwaPoolIndex.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolIndex,
                                   (xLibU8_t *) & poolNameTemp,
                                   &kwaPoolIndex.len);
  if (kwaPoolIndex.rc != XLIBRC_SUCCESS)
  {
    kwaPoolIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPoolIndex);
    return kwaPoolIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwaPoolIndex.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_NOT_SUPPORTED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_DynamicPoolIpPrefixLength
*
* @purpose Get 'DynamicPoolIpPrefixLength'
*
* @description This specifies the no. of bits that comprise the address-prefix. 
*              The prefix is an alternative way of specifying the network-mask 
*              of the client.The IP address must have been set before. 
*              This value shows 0, if the b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DynamicPoolIpPrefixLength (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicPoolIpPrefixLengthValue;
  xLibStr256_t poolNameTemp;
  xLibU32_t tempPoolType;
  xLibU32_t ipAddrTemp;
  xLibU32_t ipMaskTemp;
  xLibU32_t length=0;
  xLibU32_t bitValue;

  bitValue = 0X80000000;

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));		
  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {

    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_DYNAMIC_POOL)
    {
      if(usmDbDhcpsNetworkGet(L7_UNIT_CURRENT, poolNameTemp, &ipAddrTemp, &ipMaskTemp) != L7_SUCCESS)
      {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
      }

      while(ipMaskTemp & bitValue)
      {
        length ++;
        bitValue = bitValue >> 1;
      }

      objDynamicPoolIpPrefixLengthValue = length;
    }
    else
    {
      objDynamicPoolIpPrefixLengthValue = L7_NULL;
    }

   }
   else
   {
   	    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
   }

  /* return the object value: DynamicPoolIpPrefixLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicPoolIpPrefixLengthValue,
                    sizeof (objDynamicPoolIpPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DynamicPoolIpPrefixLength
*
* @purpose Set 'DynamicPoolIpPrefixLength'
*
* @description This specifies the no. of bits that comprise the address-prefix. 
*              The prefix is an alternative way of specifying the network-mask 
*              of the client.The IP address must have been set before. 
*              This value shows 0, if the b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DynamicPoolIpPrefixLength (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicPoolIpPrefixLengthValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
  xLibU32_t ipAddr,ipMask;
  xLibU32_t prevIpAddr=0,prevIpMask=0;
  
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DynamicPoolIpPrefixLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDynamicPoolIpPrefixLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDynamicPoolIpPrefixLengthValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  /* retrieve key: DynamicPoolIPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_DynamicPoolIpAddress,
                          (xLibU8_t *) & ipAddr, &owa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &ipAddr, kwa.len);

  
  /* set the value in application */
  ipMask = 0xFFFFFFFF;
  ipMask = ipMask << (32 - objDynamicPoolIpPrefixLengthValue);

  owa.l7rc = usmDbDhcpsNetworkGet (L7_UNIT_CURRENT, poolNameTemp, &prevIpAddr, &prevIpMask);
  if(owa.l7rc == L7_SUCCESS)
  {
    if(prevIpAddr != 0x0)
    {
      if((prevIpAddr != ipAddr) || (prevIpMask != ipMask))
      {
        owa.l7rc = usmDbDhcpsNetworkDelete(L7_UNIT_CURRENT, poolNameTemp);
        if(owa.l7rc != L7_SUCCESS)
        {
          owa.rc = XLIBRC_FAILURE;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      }
    }
  }


  owa.l7rc = usmDbDhcpsNetworkSet (L7_UNIT_CURRENT, poolNameTemp, ipAddr, ipMask);
  if (owa.l7rc != L7_SUCCESS)
  {
    if(prevIpAddr != 0)
    {
      usmDbDhcpsNetworkSet (L7_UNIT_CURRENT, poolNameTemp, prevIpAddr, prevIpMask);
    } 
    owa.rc = XLIBRC_DHCPS_POOL_NETWORK_SET_FAILED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_PoolAllocationType
*
* @purpose Get 'PoolAllocationType'
*
* @description This specifies the type of the binding that is associated with 
*              this pool. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_PoolAllocationType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoolAllocationTypeValue;
  xLibStr256_t poolNameTemp;

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  owa.l7rc = usmDbDhcpsPoolTypeGet (L7_UNIT_CURRENT, poolNameTemp,
                                    &objPoolAllocationTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoolAllocationType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoolAllocationTypeValue,
                           sizeof (objPoolAllocationTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_ClientIdentifier
*
* @purpose Get 'ClientIdentifier'
*
* @description It specifies the unique identifier (in colon separated hexadecimal 
*              format) of a DHCP client. It is valid for manual bindings 
*              only. It displays 'unconfigured',if the binding type is 'un-allocated',or,'dynamic'. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_ClientIdentifier (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objClientIdentifierValue;
  xLibStr256_t poolNameTemp; 

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  xLibU32_t keyClientIDLength;
  xLibU32_t tempPoolType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

   if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
      owa.l7rc = usmDbDhcpsClientIdGet (L7_UNIT_CURRENT, poolNameTemp,
                                    objClientIdentifierValue,&keyClientIDLength);
      if (owa.l7rc != L7_SUCCESS)
     {
		owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
		FPOBJ_TRACE_EXIT (bufp, owa);
		return owa.rc;
      }
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* return the object value: ClientIdentifier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objClientIdentifierValue,
                           keyClientIDLength);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_ClientIdentifier
*
* @purpose Set 'ClientIdentifier'
*
* @description It specifies the unique identifier (in colon separated hexadecimal 
*              format) of a DHCP client. It is valid for manual bindings 
*              only. It displays 'unconfigured',if the binding type is 'un-allocated',or,'dynamic'. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_ClientIdentifier (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objClientIdentifierValue;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
	
  xLibStr256_t poolNameTemp;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClientIdentifier */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objClientIdentifierValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objClientIdentifierValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  

  
  owa.l7rc = usmDbDhcpsClientIdSet (L7_UNIT_CURRENT, poolNameTemp,
		                                   objClientIdentifierValue, owa.len);
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
* @function fpObjGet_baseDhcpServerPoolConfig_ManualPoolClientName
*
* @purpose Get 'ManualPoolClientName'
*
* @description It specifies the name of a DHCP client. Client name should't include 
*              domain-name. It displays 'unconfigured',if the binding 
*              type is 'un-allocated',or,'dynamic'. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_ManualPoolClientName (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManualPoolClientNameValue;
  xLibStr256_t poolNameTemp;
  xLibU32_t tempPoolType;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
      /* get the value from application */
      owa.l7rc = usmDbDhcpsClientNameGet (L7_UNIT_CURRENT, poolNameTemp,
                                      objManualPoolClientNameValue);
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManualPoolClientName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManualPoolClientNameValue,
                           strlen (objManualPoolClientNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_ManualPoolClientName
*
* @purpose Set 'ManualPoolClientName'
*
* @description It specifies the name of a DHCP client. Client name should't include 
*              domain-name. It displays 'unconfigured',if the binding 
*              type is 'un-allocated',or,'dynamic'. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_ManualPoolClientName (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManualPoolClientNameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ManualPoolClientName */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objManualPoolClientNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objManualPoolClientNameValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if(strlen(objManualPoolClientNameValue) > L7_DHCPS_CLIENT_NAME_MAXLEN )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDhcpsClientNameSet (L7_UNIT_CURRENT, poolNameTemp,
                                      objManualPoolClientNameValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_ManualPoolClientHWAddr
*
* @purpose Get 'ManualPoolClientHWAddr'
*
* @description It specifies the hardware-address of a DHCP client. It is valid 
*              for manual bindings only. It displays 'unconfigured',if the 
*              binding type is 'un-allocated',or,'dynamic'. After hardware-address, 
*              hardware-type will b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_ManualPoolClientHWAddr (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objManualPoolClientHWAddrValue;

  xLibU32_t hardwareType = L7_NULL;
  xLibU32_t tempPoolType;
  xLibStr256_t poolNameTemp;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
 	/* get the value from application */
	owa.l7rc = usmDbDhcpsHwAddressGet (L7_UNIT_CURRENT, poolNameTemp, objManualPoolClientHWAddrValue,&hardwareType);
	if (owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}
	
  }
  else
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
  }

  /* return the object value: ManualPoolClientHWAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManualPoolClientHWAddrValue,
                           sizeof (objManualPoolClientHWAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_ManualPoolClientHWAddr
*
* @purpose Set 'ManualPoolClientHWAddr'
*
* @description It specifies the hardware-address of a DHCP client. It is valid 
*              for manual bindings only. It displays 'unconfigured',if the 
*              binding type is 'un-allocated',or,'dynamic'. After hardware-address, 
*              hardware-type will b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_ManualPoolClientHWAddr (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objManualPoolClientHWAddrValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ManualPoolClientHWAddr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objManualPoolClientHWAddrValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objManualPoolClientHWAddrValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  /* set the value in application */
  owa.l7rc = usmDbDhcpsHwAddressSet (L7_UNIT_CURRENT, poolNameTemp,
                                     objManualPoolClientHWAddrValue,L7_DHCPS_ETHERNET);
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
* @function fpObjGet_baseDhcpServerPoolConfig_ManualPoolClientHWType
*
* @purpose Get 'ManualPoolClientHWType'
*
* @description It specifies the hardware-type of a DHCP client. It is valid for 
*              manual bindings only. It displays 'unconfigured',if the binding 
*              type is 'un-allocated',or,'dynamic'. Before this, hardware-address 
*              must be configure 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_ManualPoolClientHWType (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManualPoolClientHWTypeValue;
  xLibStr256_t poolNameTemp;
  xLibU32_t tempPoolType;
  xLibStr256_t tempHWType;
 
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
    /* get the value from application */
    owa.l7rc = usmDbDhcpsHwAddressGet (L7_UNIT_CURRENT, poolNameTemp,
                                     tempHWType,&objManualPoolClientHWTypeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* return the object value: ManualPoolClientHWType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManualPoolClientHWTypeValue,
                           sizeof (objManualPoolClientHWTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_ManualPoolClientHWType
*
* @purpose Set 'ManualPoolClientHWType'
*
* @description It specifies the hardware-type of a DHCP client. It is valid for 
*              manual bindings only. It displays 'unconfigured',if the binding 
*              type is 'un-allocated',or,'dynamic'. Before this, hardware-address 
*              must be configure 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_ManualPoolClientHWType (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManualPoolClientHWTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve object: ManualPoolClientHWType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objManualPoolClientHWTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objManualPoolClientHWTypeValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  /* set the value in application */
  owa.l7rc = usmDbDhcpsSNMPHwTypeSet (L7_UNIT_CURRENT, poolNameTemp,
                                      objManualPoolClientHWTypeValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_ManualPoolIpAddress
*
* @purpose Get 'ManualPoolIpAddress'
*
* @description The IP address of the DHCP Address pool for manual binding. The 
*              IP address must be set before setting the IP Mask or IP Prefix 
*              Length.After setting IP address, network -mask, or IP Prefix 
*              length must be set in order to change the pool 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_ManualPoolIpAddress (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManualPoolIpAddressValue;
  xLibU32_t tempPoolType;
  xLibU32_t tempIPMask;
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
	/* get the value from application */
	owa.l7rc = usmDbDhcpsHostGet (L7_UNIT_CURRENT, poolNameTemp,
	                            &objManualPoolIpAddressValue,&tempIPMask);
	if (owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManualPoolIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManualPoolIpAddressValue,
                           sizeof (objManualPoolIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_ManualPoolIpAddress
*
* @purpose Set 'ManualPoolIpAddress'
*
* @description The IP address of the DHCP Address pool for manual binding. The 
*              IP address must be set before setting the IP Mask or IP Prefix 
*              Length.After setting IP address, network -mask, or IP Prefix 
*              length must be set in order to change the pool 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_ManualPoolIpAddress (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManualPoolIpAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve object: ManualPoolIpAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objManualPoolIpAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objManualPoolIpAddressValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  /* set the value in application */
  owa.l7rc = usmDbDhcpsSNMPHostIpAddrSet (L7_UNIT_CURRENT, poolNameTemp,
                                          objManualPoolIpAddressValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_ManualPoolIpMask
*
* @purpose Get 'ManualPoolIpMask'
*
* @description The bit-combination that renders which portion of the address 
*              of the DHCP address pool for a manual binding refers to the network 
*              or subnet and which part refers to the host. The IP address 
*              must have been set befo 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_ManualPoolIpMask (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManualPoolIpMaskValue;
   xLibU32_t tempPoolType;
  xLibU32_t tempIPAddr;
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
	/* get the value from application */
	owa.l7rc = usmDbDhcpsHostGet (L7_UNIT_CURRENT, poolNameTemp,&tempIPAddr,
	                            &objManualPoolIpMaskValue);
	if (owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManualPoolIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManualPoolIpMaskValue,
                           sizeof (objManualPoolIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_ManualPoolIpMask
*
* @purpose Set 'ManualPoolIpMask'
*
* @description The bit-combination that renders which portion of the address 
*              of the DHCP address pool for a manual binding refers to the network 
*              or subnet and which part refers to the host. The IP address 
*              must have been set befo 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_ManualPoolIpMask (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManualPoolIpMaskValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibStr256_t poolNameTemp;
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp)); 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ManualPoolIpMask */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objManualPoolIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objManualPoolIpMaskValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  /* set the value in application */
  owa.l7rc = usmDbDhcpsSNMPHostMaskSet (L7_UNIT_CURRENT, poolNameTemp,
                                        objManualPoolIpMaskValue);
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
* @function fpObjGet_baseDhcpServerPoolConfig_PoolIpPrefixLength
*
* @purpose Get 'PoolIpPrefixLength'
*
* @description This specifies the no. of bits that comprise the address-prefix. 
*              The prefix is an alternative way of specifying the network-mask 
*              of the client.The IP address must have been set before. 
*              This value shows 0, if the b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_PoolIpPrefixLength (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoolIpPrefixLengthValue;
  xLibU32_t tempIPAddr;
  xLibU32_t tempIPMask;
  xLibU32_t length=0;
  xLibU32_t bitValue;
  xLibStr256_t poolNameTemp;
  xLibU32_t tempPoolType;
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  bitValue = 0X80000000;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  if (usmDbDhcpsPoolTypeGet(L7_UNIT_CURRENT, poolNameTemp, &tempPoolType) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
	/* get the value from application */
	owa.l7rc = usmDbDhcpsHostGet (L7_UNIT_CURRENT, poolNameTemp,
	                            &tempIPAddr,&tempIPMask);
	if (owa.l7rc != L7_SUCCESS)
	{
	owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
	}
	
	while(tempIPMask & bitValue)
      {
        length ++;
       bitValue = bitValue >> 1;
      }
      objPoolIpPrefixLengthValue = length;
  }
  else
  {
    objPoolIpPrefixLengthValue = 0;
    owa.rc = L7_SUCCESS;
  }

  /* return the object value: PoolIpPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoolIpPrefixLengthValue,
                           sizeof (objPoolIpPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_PoolIpPrefixLength
*
* @purpose Set 'PoolIpPrefixLength'
*
* @description This specifies the no. of bits that comprise the address-prefix. 
*              The prefix is an alternative way of specifying the network-mask 
*              of the client.The IP address must have been set before. 
*              This value shows 0, if the b 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_PoolIpPrefixLength (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoolIpPrefixLengthValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  xLibU32_t poolPrefixLength = 0xffffffff;
  xLibStr256_t poolNameTemp;
  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));   
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoolIpPrefixLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoolIpPrefixLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoolIpPrefixLengthValue, owa.len);

  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);
  
  poolPrefixLength = poolPrefixLength << (32- objPoolIpPrefixLengthValue);

  /* set the value in application */  /* Already set in host mask field */
  owa.l7rc = usmDbDhcpsSNMPHostMaskSet (L7_UNIT_CURRENT, poolNameTemp,
                                        poolPrefixLength);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required*/ 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The row status variable is used according to installation
*              and removal conventions for conceptual rows. When
*              a pool is created by applying write operation on 'DhcpServerPoolCreateName',row-status
*              becomes 'active'. If row-status
*              for 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  xLibStr256_t poolNameTemp;
  FPOBJ_TRACE_ENTER (bufp);

  memset(poolNameTemp, 0x00, sizeof(poolNameTemp));
  /* retrieve key: PoolName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolName,
                          (xLibU8_t *) & poolNameTemp, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &poolNameTemp, kwa.len);

  /* get the value from application */
  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_Status
*
* @purpose Set 'Status'
*
* @description [Status]: The row status variable is used according to installation
*              and removal conventions for conceptual rows. When
*              a pool is created by applying write operation on 'DhcpServerPoolCreateName',row-status
*              becomes 'active'. If row-status
*              for 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue,keyPoolIndexValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  L7_uchar8 poolNameTemp [255];
  memset(poolNameTemp, 0x00,255);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PoolIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolConfig_PoolIndex,
                          (xLibU8_t *) & keyPoolIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPoolIndexValue, kwa.len);

  /* get the value from application */

  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyPoolIndexValue, poolNameTemp) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_ACTIVE)
  {
      owa.rc = XLIBRC_SUCCESS;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
    {
    /* Delete the existing row */

    owa.l7rc = usmDbDhcpsPoolDelete(L7_UNIT_CURRENT, poolNameTemp);
    
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolConfig_DhcpServerPoolNameCreate
*
* @purpose Get 'DhcpServerPoolNameCreate'
*
* @description A pool is created by applying write operation on this MIB object 
*              and providing a new pool-name. Get operation on this MIB object 
*              returns a null-value, as for the purpose of viewing the 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolConfig_DhcpServerPoolNameCreate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDhcpServerPoolNameCreateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(objDhcpServerPoolNameCreateValue, 0x00, sizeof(objDhcpServerPoolNameCreateValue));

  owa.rc = XLIBRC_SUCCESS; /*this function is just a place holder and always return SUCCESS */
  FPOBJ_TRACE_VALUE (bufp, objDhcpServerPoolNameCreateValue,
                     strlen (objDhcpServerPoolNameCreateValue));

  /* return the object value: DhcpServerPoolNameCreate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDhcpServerPoolNameCreateValue,
                           strlen (objDhcpServerPoolNameCreateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolConfig_DhcpServerPoolNameCreate
*
* @purpose Set 'DhcpServerPoolNameCreate'
*
* @description A pool is created by applying write operation on this MIB object 
*              and providing a new pool-name. Get operation on this MIB object 
*              returns a null-value, as for the purpose of viewing the 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolConfig_DhcpServerPoolNameCreate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDhcpServerPoolNameCreateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerPoolNameCreate */
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *) objDhcpServerPoolNameCreateValue,&owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDhcpServerPoolNameCreateValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsPoolCreate (L7_UNIT_CURRENT,objDhcpServerPoolNameCreateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

 xLibFilterSet(wap,XOBJ_baseDhcpServerPoolConfig_PoolName, 0, (xLibU8_t *)&objDhcpServerPoolNameCreateValue,strlen(objDhcpServerPoolNameCreateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


