/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_sysnet.c
*
* @purpose   Interface to sysnet to register and deregister packet intercepts.
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/29/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "log.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "defaultconfig.h"
#include "wio_util.h"

extern wioInfo_t *wioInfo;


/*********************************************************************
* @purpose  Register sysnet intercepts.
*
* @param    void
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t wioSysnetRegister(void)
{
  sysnetPduIntercept_t sysnetPduInterceptIn;
  sysnetPduIntercept_t sysnetPduInterceptOut;

  /* Intercept incoming IPv4 packets at DTL layer. */
  sysnetPduInterceptIn.addressFamily = L7_AF_INET;
  sysnetPduInterceptIn.hookId = SYSNET_INET_RECV_IN;
  sysnetPduInterceptIn.hookPrecedence = FD_SYSNET_HOOK_CP_WIO_IN_PRECEDENCE;
  sysnetPduInterceptIn.interceptFunc = wioPacketInterceptIn;
  strcpy(sysnetPduInterceptIn.interceptFuncName, "wioPacketInterceptIn");

  if (sysNetPduInterceptRegister(&sysnetPduInterceptIn) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
           "CP WIO incoming sysnet registration failed.");
    return L7_FAILURE;
  }

  /* Intercept incoming ARP packets */
  sysnetPduInterceptIn.addressFamily = L7_AF_INET;
  sysnetPduInterceptIn.hookId = SYSNET_INET_RECV_ARP_IN;
  sysnetPduInterceptIn.hookPrecedence = FD_SYSNET_HOOK_CP_WIO_ARP_IN_PRECEDENCE;
  sysnetPduInterceptIn.interceptFunc = wioArpIntercept;
  strcpy(sysnetPduInterceptIn.interceptFuncName, "wioArpIntercept");

  if (sysNetPduInterceptRegister(&sysnetPduInterceptIn) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
           "CP WIO sysnet registration for ARP packets failed.");
    return L7_FAILURE;
  }

  /* Intercept outgoing packets prior to L2 encapsulation */
  sysnetPduInterceptOut.addressFamily = L7_AF_INET;
  sysnetPduInterceptOut.hookId = SYSNET_INET_POSTCAP_OUT;
  sysnetPduInterceptOut.hookPrecedence = FD_SYSNET_HOOK_CP_WIO_OUT_PRECEDENCE;
  sysnetPduInterceptOut.interceptFunc = wioPacketInterceptOut;
  strcpy(sysnetPduInterceptOut.interceptFuncName, "wioPacketInterceptOut");

  if (sysNetPduInterceptRegister(&sysnetPduInterceptOut) != L7_SUCCESS)
  {
    sysNetPduInterceptDeregister(&sysnetPduInterceptIn);
    L7_LOG(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
           "CP WIO outgoing sysnet registration failed.");
    return L7_FAILURE;
  }

  wioInfo->isRegWithSysnet = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deregister sysnet intercepts.
*
* @param    void
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t wioSysnetDeregister(void)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  /* Intercept incoming IPv4 packets at DTL layer. */
  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_RECV_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_CP_WIO_IN_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = wioPacketInterceptIn;
  strcpy(sysnetPduIntercept.interceptFuncName, "wioPacketInterceptIn");
  if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
           "CP WIO incoming sysnet deregistration failed.");
    /* blunder on */
  }

  /* Intercept incoming ARP packets */
  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_RECV_ARP_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_CP_WIO_ARP_IN_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = wioArpIntercept;
  strcpy(sysnetPduIntercept.interceptFuncName, "wioArpIntercept");
  if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
           "CP WIO sysnet deregistration for ARP packets failed.");
    /* blunder on */
  }

  /* Intercept outgoing packets prior to L2 encapsulation */
  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_POSTCAP_OUT;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_CP_WIO_OUT_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = wioPacketInterceptOut;
  strcpy(sysnetPduIntercept.interceptFuncName, "wioPacketInterceptOut");
  if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_CRITICAL, L7_CP_WIO_COMPONENT_ID,
           "CP WIO outgoing sysnet deregistration failed.");
    /* blunder on */
  }

  wioInfo->isRegWithSysnet = L7_FALSE;

  return L7_SUCCESS;
}

