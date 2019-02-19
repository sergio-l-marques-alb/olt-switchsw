/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingloopbackSummary.c
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
#include "_xe_routingloopbackSummary_obj.h"
#include "usmdb_rlim_api.h"
#include "usmdb_ip6_api.h"
#include "osapi_support.h"
#include "osapi.h"

/*******************************************************************************
* @function fpObjGet_routingloopbackSummary_loopbackIdString
*
* @purpose Get ''
*
* @description [ID]: The Loopback ID is associated with Internal Interface
*              number which will be generated when we create a loopback.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingloopbackSummary_LoopbackIdString (void *wap, void *bufp)
{
  xLibU32_t objIDValue=0;
  xLibU32_t nextObjIDValue,intfNum;
  FPOBJ_TRACE_ENTER (bufp);
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLoopbackIdString={0};
  xLibStr256_t nextObjLoopbackIdString={0};

  /* retrieve key: ID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackSummary_LoopbackIdString,
                          (xLibU8_t *) objLoopbackIdString, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbRlimLoopbackIdFirstGet(&nextObjIDValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objLoopbackIdString, owa.len);
    sscanf(objLoopbackIdString, "loopback%d", &objIDValue);
    owa.l7rc = usmDbRlimLoopbackIdNextGet(objIDValue, &nextObjIDValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc = usmDbRlimLoopbackIntIfNumGet (nextObjIDValue, &intfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  osapiSnprintf(nextObjLoopbackIdString, sizeof(nextObjLoopbackIdString), "loopback%d", nextObjIDValue);

  FPOBJ_TRACE_CURRENT_KEY (bufp, nextObjLoopbackIdString, owa.len);

   /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextObjLoopbackIdString, strlen(nextObjLoopbackIdString));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_routingloopbackSummary_LoopbackAddrIndex (void *wap, void *bufp)
{
  xLibU32_t objIDValue=0,intIfNum;
  FPOBJ_TRACE_ENTER (bufp);
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t kwaObjLoopbackIdString={0};
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLoopbackAddrIndex=0;
  xLibU32_t nextObjLoopbackAddrIndex=0;

  xLibU32_t           addrCount;
  xLibU32_t           numIP4Addr;
  L7_rtrIntfIpAddr_t  ip4AddrList[L7_L3_NUM_IP_ADDRS];
  xLibU32_t           numIP6Addr,ct;
  #ifdef L7_IPV6_PACKAGE
  L7_ipv6IntfAddr_t   ip6AddrList[L7_RTR6_MAX_INTF_ADDRS+1];
  #endif
  

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackSummary_LoopbackIdString,
                          (xLibU8_t *) kwaObjLoopbackIdString, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  sscanf(kwaObjLoopbackIdString, "loopback%d", &objIDValue);
  owa.l7rc = usmDbRlimLoopbackIntIfNumGet (objIDValue, &intIfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingloopbackSummary_LoopbackAddrIndex,
                          (xLibU8_t *)&objLoopbackAddrIndex, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLoopbackAddrIndex, owa.len);
  }
 
  numIP4Addr = 0;
  if (usmDbIpRtrIntfCfgIpAddrListGet(L7_UNIT_CURRENT, intIfNum, ip4AddrList) == L7_SUCCESS)
  {
    if (ip4AddrList[0].ipAddr != 0)
    {
      for (ct=0; ct < L7_L3_NUM_IP_ADDRS; ct++)
      {
        if (ip4AddrList[ct].ipAddr != 0)
        { 
          numIP4Addr++;
        }
      }
    }
  }

#ifdef L7_IPV6_PACKAGE
  numIP6Addr = L7_RTR6_MAX_INTF_ADDRS+1;
  if (usmDbIp6RtrIntfAddressesGet(intIfNum, &numIP6Addr, ip6AddrList) != L7_SUCCESS)
#endif
  numIP6Addr = 0;
  addrCount = (numIP4Addr + numIP6Addr);

  if (objLoopbackAddrIndex <= addrCount)
  {
    if(owa.rc != XLIBRC_SUCCESS)
    {
      nextObjLoopbackAddrIndex = 0;
    }
    else
    {
      nextObjLoopbackAddrIndex = objLoopbackAddrIndex+1;
      if(nextObjLoopbackAddrIndex >= addrCount)
      {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    } 
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjLoopbackAddrIndex, owa.len);

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)&nextObjLoopbackAddrIndex, sizeof(nextObjLoopbackAddrIndex));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



xLibRC_t fpObjGet_routingloopbackSummary_LoopbackIpAddrString (void *wap, void *bufp)
{
  xLibU32_t objIDValue=0,intIfNum;
  FPOBJ_TRACE_ENTER (bufp);
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyObjLoopbackIdString={0};
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t loopbackAddrIndex=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLoopbackAddress={0};

  xLibU32_t           addrCount;
  xLibU32_t           addrIndex;
  xLibU32_t           numIP4Addr;
  L7_rtrIntfIpAddr_t  ip4AddrList[L7_L3_NUM_IP_ADDRS];
  xLibS8_t            ip4AddrStr[16]={0};
  xLibS8_t            ip4MaskStr[16]={0};
  xLibU32_t           numIP6Addr,ct;
  #ifdef L7_IPV6_PACKAGE
  L7_ipv6IntfAddr_t   ip6AddrList[L7_RTR6_MAX_INTF_ADDRS+1];
  xLibS8_t            ip6AddrStr[64]={0};
  #endif
  

  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingloopbackSummary_LoopbackIdString,
                          (xLibU8_t *) keyObjLoopbackIdString, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  /* retrieve key: Address */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingloopbackSummary_LoopbackAddrIndex,
                          (xLibU8_t *)&loopbackAddrIndex, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  sscanf(keyObjLoopbackIdString, "loopback%d", &objIDValue);

  owa.l7rc = usmDbRlimLoopbackIntIfNumGet (objIDValue, &intIfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  numIP4Addr = 0;
  if (usmDbIpRtrIntfCfgIpAddrListGet(L7_UNIT_CURRENT, intIfNum, ip4AddrList) == L7_SUCCESS)
  {
    if (ip4AddrList[0].ipAddr != 0)
    {
      for (ct=0; ct < L7_L3_NUM_IP_ADDRS; ct++)
      {
        if (ip4AddrList[ct].ipAddr != 0)
        {
          numIP4Addr++;
        }  
      }
    }
  }

#ifdef L7_IPV6_PACKAGE
  numIP6Addr = L7_RTR6_MAX_INTF_ADDRS+1;
  if (usmDbIp6RtrIntfAddressesGet(intIfNum, &numIP6Addr, ip6AddrList) != L7_SUCCESS)
#endif
  numIP6Addr = 0;
  addrCount = (numIP4Addr + numIP6Addr);

  if((addrCount == 0) && (loopbackAddrIndex == 0))
  {
    osapiSnprintf(objLoopbackAddress, sizeof(objLoopbackAddress), "%s", "No Addresses Configured");
  }
  else if (loopbackAddrIndex <= addrCount)
  {
    if (addrCount == 0)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else if (loopbackAddrIndex < numIP4Addr)
    {
      addrIndex = loopbackAddrIndex;
      osapiInetNtop(L7_AF_INET,
                    (char *)&ip4AddrList[addrIndex].ipAddr,
                    ip4AddrStr, sizeof(ip4AddrStr));
      osapiInetNtop(L7_AF_INET,
                    (char *)&ip4AddrList[addrIndex].ipMask,
                    ip4MaskStr, sizeof(ip4MaskStr));
      
      osapiSnprintf(objLoopbackAddress, sizeof(objLoopbackAddress), "%s / %s", ip4AddrStr, ip4MaskStr);
    }
    #ifdef L7_IPV6_PACKAGE
    else /* IPv6 Addresses */
    {
      addrIndex = loopbackAddrIndex-numIP4Addr;
      osapiInetNtop(L7_AF_INET6,
                    (char *)&ip6AddrList[addrIndex].ip6Addr,
                    ip6AddrStr, sizeof(ip6AddrStr));

      if (ip6AddrList[addrIndex].ip6AddrState != L7_IPV6_INTFADDR_ACTIVE)
      {
        osapiSnprintf(objLoopbackAddress, sizeof(objLoopbackAddress), "%s/%u [TENT]", ip6AddrStr, ip6AddrList[addrIndex].ip6PrefixLen);
      }
      else
      {
        osapiSnprintf(objLoopbackAddress, sizeof(objLoopbackAddress), "%s/%u", ip6AddrStr, ip6AddrList[addrIndex].ip6PrefixLen);
      }
    }
    #endif
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)&objLoopbackAddress, strlen(objLoopbackAddress));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
