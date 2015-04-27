/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_dhcp4ClientLeaseparametersdhcp4ClientLeaseparameters.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dhcpv4client-object.xml
*
* @create  
*
* @author  
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_dhcp4ClientLeaseParameters_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_rtrdisc_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dhcp_client.h"

static L7_RC_t
usmDbIpInterfaceNextGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum)
{
  L7_int32 temp_val;
  L7_uint32 method = 0;
  L7_IP_ADDR_t ipAddr = 0;

  /* loop through available external interface numbers */
  while (usmDbValidIntIfNumNext(*intIfNum, intIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if ((usmDbValidateRtrIntf(UnitIndex, *intIfNum) == L7_SUCCESS) &&
        (usmDbVisibleInterfaceCheck(UnitIndex, *intIfNum, &temp_val) == L7_SUCCESS))
    {
      if ((usmDbIpIntfExists(L7_UNIT_CURRENT, *intIfNum) == L7_TRUE) &&
         (usmDbIpRtrIntfIpAddressMethodGet(*intIfNum, &method) == L7_SUCCESS) &&
         (usmDbIpDhcpClientIPAddressGet(*intIfNum, &ipAddr) == L7_SUCCESS) &&
         (ipAddr != 0))
      {
        if ((L7_INTF_IP_ADDR_METHOD_t) method == L7_INTF_IP_ADDR_METHOD_DHCP)
        {
          return L7_SUCCESS;
        }
      }
    }
  }
  return L7_FAILURE;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: The IfIndex associated with this instance. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex (void *wap, 
                                                                        void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &objIfIndexValue, &owa.len);
  
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjIfIndexValue = 0;
    owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    nextObjIfIndexValue = objIfIndexValue;
    owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjIfIndexValue); 
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
                           sizeof (nextObjIfIndexValue));
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
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientIpAddress (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) & keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpDhcpClientIPAddressGet (keyIfIndexValue, &objipAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objipAddressValue,
                           sizeof (objipAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientSubnetMask
*
* @purpose Get 'netMask'
*
* @description [netMask]: The NetMask assigned to this interface. When setting
*              this value, the value of agentSwitchIpInterfaceIpAddress
*              must be set at the same time. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientSubnetMask (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnetMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpDhcpClientNetworkMaskGet (keyIfIndexValue, &objnetMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: netMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objnetMaskValue,
                           sizeof (objnetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientDhcpServerAddress
*
* @purpose Get 'DhcpServerAddress'
*
* @description [DhcpServerAddress]: Gets the DHCP server IpAddress.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientDhcpServerAddress (void *wap,
               															                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddrValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value in application */
  owa.l7rc = usmDbIpDhcpClientDhcpServerIPAddressGet (keyIfIndexValue, &objIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PRIMARY_ADDRESS_REMOVAL_FAILED; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objIpAddrValue,
                           sizeof (objIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientState
*
* @purpose Get 'dhcp4ClientState'
*
* @description [dhcp4ClientState]: The State of the DHCP Client on this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientState (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdhcp4ClientState;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbIpDhcpClientStateGet (keyIfIndexValue, &objdhcp4ClientState);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dhcp4ClientState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objdhcp4ClientState ,
                           sizeof (objdhcp4ClientState ));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientTransactionID
*
* @purpose Get 'TransactionID'
*
* @description [TransactionID]: The transaction ID of the DHCP Client.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientTransactionID (void *wap,
                                                                       void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTransactionID;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  owa.l7rc = usmDbIpDhcpClientTransactionIdGet (keyIfIndexValue, &objTransactionID);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TransactionID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTransactionID ,
                           sizeof (objTransactionID ));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientLeaseTime
*
* @purpose Get 'LeaseTime'
*
* @description [LeaseTime]: The time (in seconds) that the IP address was leased 
*                            by the DHCP Server.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientLeaseTime (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaseTime, objRenewalTime, objRebindTime;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpDhcpClientLeaseTimeInfoGet (keyIfIndexValue, &objLeaseTime, 
                                                &objRenewalTime, &objRebindTime);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the LeaseTime value in time ticks */
  objLeaseTime = (objLeaseTime * 100);

  /* return the object value: LeaseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objLeaseTime,
                           sizeof (objLeaseTime));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientRenewTime
*
* @purpose Set 'RenewTime'
*
* @description [RenewTime]: The time (in seconds) when the next DHCP Renew 
*                           Request is sent by DHCP Client to renew the 
*                           leased IP address.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientRenewTime (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaseTime, objRenewalTime, objRebindTime;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);
 
  /* set the value in application */
  owa.l7rc = usmDbIpDhcpClientLeaseTimeInfoGet (keyIfIndexValue, &objLeaseTime, 
                                                &objRenewalTime, &objRebindTime);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the RenewTime value in time ticks */
  objRenewalTime = (objRenewalTime * 100);

  /* return the object value: RenewTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRenewalTime,
                           sizeof (objRenewalTime));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientRebindTime
*
* @purpose Get 'RebindTime'
*
* @description [RebindTime]: The time (in seconds) when the DHCP Rebind process 
*                            starts.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientRebindTime(void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaseTime, objRenewalTime, objRebindTime;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpDhcpClientLeaseTimeInfoGet (keyIfIndexValue, &objLeaseTime, 
                                                &objRenewalTime, &objRebindTime);
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the RebindTime value in time ticks */
  objRebindTime = (objRebindTime * 100);

  /* return the object value: RebindTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRebindTime,
                           sizeof (objRebindTime));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientRetryCount 
*
* @purpose Get 'RetryCount'
*
* @description [RetryCount]: The number of times the DHCP Client sends a DHCP  
*                            REQUEST message  before the server responds.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientRetryCount (void *wap,
                                                           			      void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRetryCount;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_dhcp4ClientLeaseParameters_dhcp4ClientInterfaceIndex,
                          (xLibU8_t *) &keyIfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpDhcpClientRetryCountGet (keyIfIndexValue, &objRetryCount);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RetryCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)&objRetryCount,
                           sizeof (objRetryCount));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientConfigured
*
* @purpose Get 'dhcp4ClientConfigured'
* @description  [dhcp4ClientConfigured] Returns True if client is configured.
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_dhcp4ClientLeaseParameters_dhcp4ClientConfigured (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdhcp4ClientInterfaceIndexValue = 0;
  xLibU32_t objdhcp4ClientConfiguredValue = L7_TRUE1FALSE2_FALSE;
  FPOBJ_TRACE_ENTER (bufp);
 
  owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &objdhcp4ClientInterfaceIndexValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    objdhcp4ClientConfiguredValue = L7_TRUE1FALSE2_TRUE;
  }
  /* return the object value: dhcp4ClientInterfaceIndexValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objdhcp4ClientConfiguredValue,
                           sizeof (objdhcp4ClientConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
