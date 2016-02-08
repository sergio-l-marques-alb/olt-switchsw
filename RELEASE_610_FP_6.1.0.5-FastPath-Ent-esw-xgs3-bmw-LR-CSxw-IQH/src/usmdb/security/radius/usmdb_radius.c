/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename usmdb_radius.c
*
* @purpose RADIUS Client include file
*
* @component radius
*
* @comments
*
* @create 03/27/2003
*
* @author pmurthy
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "usmdb_radius_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "radius_api.h"
#include "defaultconfig.h"
#include "sysapi.h"

/*********************************************************************
*
* @purpose Get the maximum number of retransmissions parameter
*
* @param pHostAddr      @b((input)) server (ALL_RADIUS_SERVERS=>global)
* @param pMaxRetransmit @b((output)) ptr to the maxRetransmits value
* @param paramStatus    @b((output)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerRetransGet(L7_char8                             *pHostAddr,
                                    L7_uint32                            *maxRetransmit,
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if ((maxRetransmit == L7_NULLPTR) || (paramStatus == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusMaxNoOfRetransGet(&dnsHostAddr, maxRetransmit, paramStatus);
}

/*********************************************************************
*
* @purpose Set the maximum number of retransmissions parameter
*
* @param pHostAddr   @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param maxRetrans  @b((input)) the new value of the MaxNoOfRetrans parameter
* @param paramStatus @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerRetransSet(L7_char8                            *pHostAddr,
                                    L7_uint32                            maxRetransmit,
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusMaxRetransSet(&dnsHostAddr, maxRetransmit, paramStatus);
}

/*********************************************************************
*
* @purpose Get the value of the time-out duration parameter
*
* @param pHostAddr      @b((input)) server (ALL_RADIUS_SERVERS=>global)
* @param pTimeout       @b((output)) ptr to the timeout value
* @param paramStatus    @b((output)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerTimeOutGet(L7_char8                             *pHostAddr,
                                    L7_uint32                            *timeOut,
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if ((timeOut == L7_NULLPTR) || (paramStatus == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusTimeOutDurationGet(&dnsHostAddr, timeOut, paramStatus);
}

/*********************************************************************
*
* @purpose Set the value of the time-out duration parameter
*
* @param pHostAddr   @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param timeOutVal  @b((input)) the new value of the timeOutDuration
* @param paramStatus @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerTimeOutSet(L7_char8                            *pHostAddr,
                                    L7_uint32                            timeOut,
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusTimeOutDurationSet(&dnsHostAddr, timeOut, paramStatus);
}

/*********************************************************************
*
* @purpose Get the value of the deadtime parameter
*
* @param pHostAddr   @b((input)) server (ALL_RADIUS_SERVERS=>global)
* @param deadtime    @b((output)) the value of deadtime
* @param paramStatus @b((output)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerDeadtimeGet(L7_char8                             *pHostAddr,
                                     L7_uint32                            *deadtime,
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if ((deadtime == L7_NULLPTR) || (paramStatus == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusServerDeadtimeGet(&dnsHostAddr, deadtime, paramStatus);
}

/*********************************************************************
*
* @purpose Set the value of the deadtime parameter
*
* @param pHostAddr   @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param deadtime    @b((input)) the new value of deadtime
* @param paramStatus @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerDeadtimeSet(L7_char8                            *pHostAddr,
                                     L7_uint32                            deadtime,
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusServerDeadtimeSet(&dnsHostAddr, deadtime, paramStatus);
}

/*********************************************************************
*
* @purpose Get the value of the deadtime parameter
*
* @param pHostAddr    @b((input))  server (ALL_RADIUS_SERVERS=>global)
* @param sourceIPAddr @b((output)) the value of source IP Addr
* @param paramStatus  @b((output)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerSourceIPGet(L7_char8                             *pHostAddr,
                                     L7_uint32                            *sourceIPAddr,
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if ((sourceIPAddr == L7_NULLPTR) || (paramStatus == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusServerSourceIPGet(&dnsHostAddr, sourceIPAddr, paramStatus);
}

/*********************************************************************
*
* @purpose Set the value of the source IP address parameter
*
* @param pHostAddr    @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param sourceIPAddr @b((input)) the new value of source IP address
* @param paramStatus  @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerSourceIPSet(L7_char8                            *pHostAddr,
                                     L7_uint32                            sourceIPAddr,
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusServerSourceIPSet(&dnsHostAddr, sourceIPAddr, paramStatus);
}

/*********************************************************************
*
* @purpose Get the value of the server priority parameter
*
* @param pHostAddr      @b((input)) server
* @param serverPriority @b((output)) the value of server priority
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerPriorityGet(L7_char8  *pHostAddr,
                                     L7_uint32 *serverPriority)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (serverPriority == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerPriorityGet(pHostAddr, dnsHostAddr.hostAddrType, serverPriority);
}

/*********************************************************************
*
* @purpose Set the value of the server priority parameter
*
* @param pHostAddr      @b((input)) server to modify
* @param serverPriority @b((input)) the new value of server priority
*                                   (lower values mean higher priority)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerPrioritySet(L7_char8 *pHostAddr,
                                     L7_uint32 serverPriority)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerPrioritySet(pHostAddr, dnsHostAddr.hostAddrType, serverPriority);
}

/*********************************************************************
*
* @purpose Get the server usage type (Login, Dot1x, All) parameter
*
* @param pHostAddr @b((input))  server
* @param usageType @b((output)) the value of server usage type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerUsageTypeGet(L7_char8                      *pHostAddr,
                                      L7_RADIUS_SERVER_USAGE_TYPE_t *usageType)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usageType == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusServerUsageTypeGet(&dnsHostAddr, usageType);
}

/*********************************************************************
*
* @purpose Set the server usage type (Login, Dot1x, All) parameter
*
* @param pHostAddr @b((input)) server to modify
* @param usageType @b((input)) the new value of server usage type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerUsageTypeSet(L7_char8                     *pHostAddr,
                                      L7_RADIUS_SERVER_USAGE_TYPE_t usageType)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusServerUsageTypeSet(&dnsHostAddr, usageType);
}

/*********************************************************************
*
* @purpose Get the value of the radius-accounting mode parameter
*
* @param unitIndex @b((input)) the unit for this operation
* @param pAccmode @b((output)) ptr to the accounting mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments The accounting mode will be set to:
*             L7_ENABLE - if accounting has been enabled
*             L7_DISABLE - if accounting has been disabled
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingModeGet(L7_uint32 unitIndex,
                                     L7_uint32 *pAccMode)
{
  if (pAccMode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pAccMode = radiusAcctModeGet();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the value of the radius-accounting mode parameter
*
* @param unitIndex @b((input)) the unit for this operation
* @param accntMode @b((input)) the new value of the radiusAccountingMode
*                              (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingModeSet(L7_uint32 unitIndex,
                                     L7_uint32 accntMode)
{
  return radiusAcctModeSet(accntMode);
}

/*********************************************************************
*
* @purpose Verify a RADIUS Accounting server is a configured server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) HostName or IP Address of the server to verify
* @param type @b((input)) Address type either ipv4 or dns
* @param verified @b(output)) L7_TRUE/L7_FALSE if configured or not
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerIPHostNameVerify(L7_uint32 unitIndex,
                                              L7_uchar8 *serverAddr,
                                              L7_IP_ADDRESS_TYPE_t type,
                                              L7_BOOL *verified)
{
  if (verified == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *verified = radiusAcctServerIPHostNameVerify(serverAddr, type);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Get the IP address of the radius accounting server corresponding
* to the index input
*
* @param unitIndex @b((input)) the unit for this operation
* @param    index  @b((input))  Index of the Accounting Server
* @param    ipAddr @b((output)) IP address of the Accounting Server
*
* @returns
*           L7_SUCCESS - if there is a server corresponding to the specified index
*           L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerIPByIndexGet(L7_uint32 unitIndex,
                                                L7_uint32 index,
                                                L7_uint32 *pIpAddr)
{
  if (pIpAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusAcctServerIPByIndexGet(index, pIpAddr);
}

/*********************************************************************
*
* @purpose  Get the HostName or IP address of the radius accounting server
*           corresponding  to the index input
*
* @param unitIndex @b((input)) the unit for this operation
* @param    index  @b((input))  Index of the Accounting Server
* @param    pIpAddr @b((output)) HostName or IP address of the Accounting Server
* @param    pType  @b((input)) Address type either ipv4 or dns
*
* @returns
*           L7_SUCCESS - if there is a server corresponding to the specified index
*           L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerIPHostNameByIndexGet(L7_uint32 unitIndex,
                                                L7_uint32 index,
                                                L7_uchar8 *pIpAddr,
                                                L7_IP_ADDRESS_TYPE_t *pType)
{
  if (pIpAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusAcctServerIPHostNameByIndexGet(index, pIpAddr, pType);
}

/*********************************************************************
*
* @purpose Get the HostName or IP address of the accounting server being used
*
* @param unitIndex @b((input)) the unit for this operation
* @param pServAddr @b((output)) ptr to the accounting server HostName or IP address
* @param pType     @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerIPHostNameGet(L7_uint32 unitIndex,
                                             L7_uchar8 *pServAddr,
                                             L7_IP_ADDRESS_TYPE_t *pType)
{
  L7_uchar8 *hostAddr = L7_NULL;
  if (pServAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if ((hostAddr = radiusAcctServerIPHostNameGet(pType)) == L7_NULL)
  {
    return L7_FAILURE;
  }
  strcpy(pServAddr, hostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the HostName or IP address of the Accounting server being used
*
* @param L7_uint32 unitIndex @b((input)) the unit for this operation
* @param L7_uchar8 *serverAddr @b((input)) pointer to the server address.
* @param L7_IP_ADDRESS_TYPE_t type @b((input)) Address type DNS or IP address
* @param L7_char8 *serverName @b((input)) pointer to server Name.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerIPHostNameSet(L7_uint32 unitIndex,
                     L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type,
                     L7_char8 *serverName)
{
  return radiusAcctServerIPHostNameAdd(serverAddr, type, serverName);
}

/*********************************************************************
*
* @purpose Add an accounting server
*
* @param unitIndex @b((input)) the unit for this operation
* @param type @b((input))       Address type DNS or IP address
* @param serverAddr @b((input)) the Hostname or IP address to be set
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerAdd(L7_uint32 unitIndex,
                          L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  return usmDbRadiusAccountingServerIPHostNameSet(unitIndex, serverAddr, type,serverName);
}

/*********************************************************************
*
* @purpose Remove an accounting server
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the hostName or IP address to be removed
* @param type @b((input))       Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerRemove(L7_uint32 unitIndex,
                        L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type)
{
  return radiusAcctHostNameServerRemove(serverAddr, type);
}

/*********************************************************************
*
* @purpose Get the port number of the accounting server being used
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the HostName or ipAddress of the accounting server
* @param type @b((input))    Address type DNS or IP address
* @param pPort @b((output)) ptr to the port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerPortNumGet(L7_uint32 unitIndex,
                                              L7_uchar8 *serverAddr,
                                              L7_IP_ADDRESS_TYPE_t type,
                                              L7_uint32 *pPort)
{
  if (pPort == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusAcctServerPortNumGet( serverAddr, type, pPort);

/*
  if ((*pPort = radiusAcctServerPortNumGet()) == L7_NULL)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;*/
}

/*********************************************************************
*
* @purpose Set the port number of the accounting server being used.
*
* @param unitIndex @b((input)) the unit for this operation
* @param servAddr @b((input)) The HostName or ipAddress of the accounting server
* @param type @b((input))     Address type DNS or IP address
* @param portNum @b((input)) the port number to be set
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerPortNumSet(L7_uint32 unitIndex,
                                             L7_uchar8 *serverAddr,
                                             L7_IP_ADDRESS_TYPE_t type,
                                             L7_uint32 portNum)
{
  return radiusAcctHostNameServerPortNumSet(serverAddr, type, portNum);
}

/*********************************************************************
*
* @purpose Set the shared secret being used between the radius client and
*          the accounting server
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the HostName or ipAddress of the accounting server
* @param type @b((input))       Address type DNS or IP address
* @param sharedSecret @b((input)) the new shared secret
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerSharedSecretSet(L7_uint32 unitIndex,
                                                   L7_uchar8 *serverAddr,
                                                   L7_IP_ADDRESS_TYPE_t type,
                                                   L7_char8 *sharedSecret)
{
  return radiusAcctHostNameServerSharedSecretSet(serverAddr,type,sharedSecret);
}

/*********************************************************************
*
* @purpose Indicate if the shared secret is set for the accounting server
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the HostName or IP address of the accounting server
* @param type @b((input))       Address type DNS or IP address
* @param pFlag @b((output)) ptr to a boolean indicating if the secret is set
*
* @returns L7_FAILURE if the accounting server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerSharedSecretConfigured(L7_uint32 unitIndex,
                                           L7_uchar8 *serverAddr,
                                           L7_IP_ADDRESS_TYPE_t type,
                                           L7_BOOL *pVal )
{
  if (pVal == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusAcctHostNameServerSharedSecretConfigured(serverAddr, type, pVal);
}

/*********************************************************************
*
* @purpose Return the shared secret for the accounting server if set
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the HostName or IP address of the accounting server
* @param type @b((input))       Address type DNS or IP address
* @param secret @b((output)) ptr to a string to contain the secret
*
* @returns L7_FAILURE if the accounting server secret is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingHostNameServerSharedSecretGet(L7_uint32 unitIndex,
                                           L7_uchar8 *serverAddr,
                                           L7_IP_ADDRESS_TYPE_t type,
                                           L7_uchar8 *secret )
{
  if (secret == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusAcctHostNameServerSharedSecretGet(serverAddr, type, secret);
}

/*********************************************************************
*
* @purpose Verify a RADIUS Authentication server is a configured server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) Host Name or IP Address of the server to verify
* @param type @b((input))     Address type either ipv4 or dns
* @param verified @b(output)) L7_TRUE/L7_FALSE if configured or not
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerAddrVerify(L7_uint32 unitIndex,
                                    L7_uchar8 *serverAddr,
                                    L7_IP_ADDRESS_TYPE_t type,
                                    L7_BOOL *verified)
{
  if (verified == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *verified = radiusAuthServerIPHostNameVerify(serverAddr, type);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the IP address of the radius server corresponding to the
*          index input
*
* @param unitIndex @b((input)) the unit for this operation
* @param index     @b((input))  Index of the Auth Server
* @param ipAddr    @b((output)) HostName or IP address of the Auth Server
* @param pType     @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerIPHostNameByIndexGet(L7_uint32 unitIndex,
                                              L7_uint32 index,
                                              L7_uchar8 *pIpAddr,
                                              L7_IP_ADDRESS_TYPE_t *pType)
{
  if (pIpAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusServerIPHostNameByIndexGet(index, pIpAddr, pType);
}

/*********************************************************************
*
* @purpose Get the IP address of the radius server corresponding to the
*          index input
*
* @param unitIndex @b((input)) the unit for this operation
* @param index  @b((input))  Index of the Auth Server
* @param ipAddr @b((output)) IP address of the Auth Server
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerIPByIndexGet(L7_uint32 unitIndex,
                                      L7_uint32 index,
                                      L7_uint32 *pIpAddr)
{
  if (pIpAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusServerIPByIndexGet(index, pIpAddr);
}

/*********************************************************************
*
* @purpose Get the first configured server
*
* @param unitIndex   @b((input)) the unit for this operation
* @param pServerAddr @b((output)) ptr to the first configured server in the set
* @param type        @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerFirstIPHostNameGet(L7_uint32 unitIndex,
                                            L7_uchar8 *pServerAddr,
                                            L7_IP_ADDRESS_TYPE_t *pType)
{
  if ((pServerAddr == L7_NULLPTR) || (pType == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  return radiusServerFirstIPHostNameGet(pServerAddr, pType);
}

/*********************************************************************
*
* @purpose Get the configured server next in the list after the specified server
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the "current" server
* @param pServerAddr @b((output)) ptr to the next configured server in the set
* @param pType     @b((output))Address type DNS or IP address
*
* @returns L7_FAILURE if there is no next server configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerNextIPHostNameGet(L7_uint32 unitIndex,
                                           L7_uchar8 *serverAddr,
                                           L7_uchar8 *pServerAddr,
                                           L7_IP_ADDRESS_TYPE_t *pType)
{
  if (pServerAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusServerNextIPHostNameGet(serverAddr, pServerAddr, pType);
}

/*********************************************************************
*
* @purpose Get the configured Accounting server next in the list
*          after the specified server
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the  server
* @param pServerAddr @b((output)) ptr to the next configured server in the set
* @param pType     @b((output))Address type DNS or IP address
*
* @returns L7_FAILURE if there is no next server configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerNextIPHostNameGet(L7_uint32 unitIndex,
                                           L7_uchar8 *serverAddr,
                                           L7_uchar8 *pServerAddr,
                                           L7_IP_ADDRESS_TYPE_t *pType)
{
  if (pServerAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusAcctServerNextIPHostNameGet(serverAddr, pServerAddr, pType);
}

/*********************************************************************
*
* @purpose Add a radius server with a specific Host Name or IP address
*
* @param L7_uint32 unitIndex @b((input)) the unit for this operation
* @param L7_uchar8 *serverAddr @b((input)) Pointer to Host Name or
*        IP Address of the Auth server
* @param L7_IP_ADDRESS_TYPE_t pType @b((input)) Address type DNS or IP address
* @param L7_char8 *serverName @b((input)) pointer to server Name.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusIPHostNameServerAdd(L7_uint32 unitIndex,
                     L7_uchar8 *servAddr,  L7_IP_ADDRESS_TYPE_t type,
                     L7_char8 *serverName)
{
  return radiusServerIPHostNameAdd(servAddr, type,serverName);
}

/*********************************************************************
*
* @purpose Remove a RADIUS server
*
* @param unitIndex @b((input)) the unit for this operation
* @param servAddr @b((input)) the HostName or IP address to be removed
* @param type @b((input))       Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAuthHostNameServerRemove(L7_uint32 unitIndex,
                                L7_uchar8 *servAddr, L7_IP_ADDRESS_TYPE_t type)
{
  return radiusAuthHostNameServerRemove(servAddr, type);
}

/*********************************************************************
*
* @purpose  Get the number of configured RADIUS servers
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pVal @b((output)) ptr to the count of configured servrs
* @param    @b((output))
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerCountGet(L7_uint32 unitIndex,
                                  L7_uint32 *pVal)
{
  L7_uchar8 serverAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uchar8 nextServerAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 count = 0;

  if (pVal == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (usmDbRadiusServerFirstIPHostNameGet(unitIndex, serverAddr,
                                          &addrType) == L7_FAILURE)
  {
    /* No servers configured. Set pVal to 0 and return success */
    *pVal = count;
    return L7_SUCCESS;
  }
  else
  {
    count++;
  }

  while (usmDbRadiusServerNextIPHostNameGet(unitIndex,serverAddr,
                          nextServerAddr, &addrType) != L7_FAILURE)
  {
    count++;
    strcpy(serverAddr, nextServerAddr);
  }

  *pVal = count;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the port number of the radius server
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    serverAddr @b((input)) HostName or IP Address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param    pPort @b((output)) ptr to the server Port number
*
* @returns L7_FAILURE if the HostName or IP address does not match a configured server
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerPortNumGet(L7_uint32 unitIndex,
                                    L7_uchar8 *serverAddr,
                                    L7_IP_ADDRESS_TYPE_t type,
                                    L7_uint32 *pPort)
{
  return radiusHostNameServerPortNumGet(serverAddr, type, pPort);
}


/*********************************************************************
*
* @purpose  Set the radius server port number
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    serverIPAddr @b((input)) the HostName or IP address of the radius server
* @param    type @b((input))       Address type DNS or IP address
* @param    portNum @b((output))  the new value for the port number
*           corresponding to this radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerPortNumSet(L7_uint32 unitIndex,
                                    L7_uchar8 *serverAddr,
                                    L7_IP_ADDRESS_TYPE_t type,
                                    L7_uint32 portNum)
{
  return radiusHostNameServerPortNumSet(serverAddr, type, portNum);
}

/*********************************************************************
*
* @purpose Set the shared secret being used between the radius client and
*          the radius server
*
* @param serverAddr   @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param sharedSecret @b((input)) the new value for the shared secret
* @param paramStatus  @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerSharedSecretSet(L7_uchar8                           *serverAddr,
                                                 L7_char8                            *sharedSecret,
                                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, serverAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerSharedSecretSet(&dnsHostAddr, sharedSecret, paramStatus);
}

/*********************************************************************
*
* @purpose  Indicate if the shared secret is set
*
* @param    serverAddr @b((input)) the HostName or IP address of the radius server
* @param    pFlag      @b((output)) ptr to a boolean indicating if the secret is set
*
* @returns L7_FAILURE
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerSharedSecretConfigured(L7_uchar8 *serverAddr,
                                                        L7_BOOL   *pFlag )
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(0, serverAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerSharedSecretConfigured(&dnsHostAddr, pFlag);
}

/*********************************************************************
*
* @purpose  Retrieve the shared secret if set
*
* @param    serverAddr   @b((input)) the HostName or IP address of the radius server
* @param    secret       @b((output)) ptr to string to contain the secret
* @param    paramStatus  @b((output)) the value is global or local
*
* @returns L7_FAILURE
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerSharedSecretGet(L7_uchar8                            *serverAddr,
                                                 L7_uchar8                            *secret,
                                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if ((secret == L7_NULLPTR) || (paramStatus == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (usmDbRadiusConvertToDnsHostAddr(0, serverAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerSharedSecretGet(&dnsHostAddr, secret, paramStatus);
}

/*********************************************************************
*
* @purpose Get the primary server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pIpAddr @b((output)) ptr to the IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE - if there is no primary server set
*
* @comments If there is no primary server set, the IP address will
*           be 0.0.0.0.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusPrimaryServerGet(L7_uint32 unitIndex,
                                    L7_uint32 *pIpAddr)
{
  if (pIpAddr != L7_NULLPTR)
  {
    /* TBD: Callers h'v to take care to send correct index*/
    if (radiusPrimaryServerGet(0,pIpAddr) != L7_FAILURE)
    {
      /* primary server was found */
      return L7_SUCCESS;
    }

    /* No primary server found, set IP to null */
    *pIpAddr = L7_NULL;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the current active auth server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pservAddr @b((output)) ptr to the HostName or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE - if there is no auth server set
*
* @comments If there is no current server set, the IP address will
*           be 0.0.0.0.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusCurrentAuthHostNameServerGet(L7_uint32 unitIndex,
                                        L7_uchar8 *pservAddr)
{
  if (pservAddr != L7_NULLPTR)
  {
    /*Temperorily 0 is passed to go ahead with internal APs,
       Later it can be taken up to get the
      index parameter from UI callers n pass to this   fun. */
    if (radiusCurrentAuthHostNameServerGet(0, L7_RADIUS_SERVER_USAGE_ALL, pservAddr) != L7_FAILURE)
    {
      /* current server was found */
      return L7_SUCCESS;
    }

    /* No current server found, set IP to null */
    *pservAddr = L7_NULL;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the type of server (primary or secondary)
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) HostName or IP Address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param pType @b((output)) ptr to the server type
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerEntryTypeGet(L7_uint32 unitIndex,
                                      L7_uchar8 *serverAddr,
                                      L7_IP_ADDRESS_TYPE_t type,
                                      L7_uint32 *pType)
{
  if (pType == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerEntryTypeGet(serverAddr, type, pType);
}

/*********************************************************************
*
* @purpose  Set the type of server (primary or secondary)
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    serverAddr @b((input)) HostName or IP Address of the radius server
* @param    type @b((input))       Address type DNS or IP address
* @param    radiusType @b((input)) Primary or secondary
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerEntryTypeSet(L7_uint32 unitIndex,
                                      L7_uchar8 *serverAddr,
                                      L7_IP_ADDRESS_TYPE_t type,
                                      L7_uint32 radiusType)
{
  return radiusHostNameServerEntryTypeSet(serverAddr, type, radiusType);
}

/*********************************************************************
*
* @purpose Enable/Disable the inclusion of a Message-Authenticator attribute
*          in each Access-Request packet to a specified RADIUS server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) HostName or IP Address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param mode @b((input)) L7_ENABLE/L7_DISABLE to include the attribute or not
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerIncMsgAuthModeSet(L7_uint32 unitIndex,
                                           L7_uchar8 *serverAddr,
                                           L7_IP_ADDRESS_TYPE_t type,
                                           L7_uint32 mode)
{
  return radiusHostNameServerIncMsgAuthModeSet(serverAddr, type, mode);
}

/*********************************************************************
*
* @purpose Get the value of Include Message-Authenticator attribute mode
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) HostName or IP Address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param mode @b((output)) L7_ENABLE/L7_DISABLE if attribute is included or not
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusHostNameServerIncMsgAuthModeGet(L7_uint32 unitIndex,
                                           L7_uchar8 *serverAddr,
                                           L7_IP_ADDRESS_TYPE_t type,
                                           L7_uint32 *mode)
{
  if (mode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return radiusHostNameServerIncMsgAuthModeGet(serverAddr, type, mode);
}

/*********************************************************************
*
* @purpose  Get the round trip time
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr  @b((input)) Host Address of the radius server
* @param    pRoundTripTime @b((output)) ptr to round trip time
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatRTTGet(L7_uint32 unitIndex,
                                    L7_char8  *pHostAddr,
                                    L7_uint32 *pRoundTripTime)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (pRoundTripTime == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *pRoundTripTime = radiusServerStatRTTGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets sent
*          to this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr  @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatAccessReqGet(L7_uint32 unitIndex,
                                          L7_uchar8  *pHostAddr,
                                          L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatAccessReqGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets
*          retransmitted to this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatAccessRetransGet(L7_uint32 unitIndex,
                                              L7_uchar8  *pHostAddr,
                                              L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatAccessRetransGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Accept packets
*          received from this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatAccessAcceptGet(L7_uint32 unitIndex,
                                             L7_uchar8  *pHostAddr,
                                             L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatAccessAcceptGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Reject packets
*          received from this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatAccessRejectGet(L7_uint32 unitIndex,
                                             L7_uchar8  *pHostAddr,
                                             L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatAccessRejectGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Challenge packets
*          received from this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatAccessChallengeGet(L7_uint32 unitIndex,
                                                L7_uchar8 *pHostAddr,
                                                L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatAccessChallengeGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of malformed RADIUS Access-Response packets
*          received from this server. Malformed packets include packets
*          with an invalid length. Bad authenticators or signature attributes
*          or unknown types are not included
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatMalformedAccessResponseGet(L7_uint32 unitIndex,
                                                        L7_uchar8 *pHostAddr,
                                                        L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatMalformedAccessResponseGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Response packets
*          containing invalid authenticators or signature attributes received
*          from this server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatBadAuthGet(L7_uint32 unitIndex,
                                        L7_uchar8 *pHostAddr,
                                        L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatBadAuthGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets
*          destined for this server that have not yet timed out or received
*          a response from this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Host Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatPendingReqGet(L7_uint32 unitIndex,
                                           L7_uchar8  *pHostAddr,
                                           L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatPendingReqGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of authentication time-outs to this server
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) HostName or IP Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatTimeoutsGet(L7_uint32 unitIndex,
                                         L7_uchar8 *pHostAddr,
                                         L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatTimeoutsGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets of unknown type which
*          were received from this server on the authentication port
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) HostName or IP Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatUnknownTypeGet(L7_uint32 unitIndex,
                                            L7_uchar8 *pHostAddr,
                                            L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatUnknownTypeGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets received from this server on
*          the authentication port that were dropped for some other reason.
*
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) HostName or IP Address of the radius server
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerStatPktsDroppedGet(L7_uint32 unitIndex,
                                            L7_uchar8 *pHostAddr,
                                            L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatPktsDroppedGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Response packets
*          received from unknown addresses.
*
* @param unitIndex @b((input)) the unit for this operation
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusStatInvalidAddressesGet(L7_uint32 unitIndex, L7_uint32 *pPacketCount)
{
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusServerStatInvalidAddressesGet();

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get the round trip time
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatRTTGet(L7_uint32 unitIndex,
                                        L7_uchar8 *pHostAddr,
                                        L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatRTTGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Request packets sent
* @         to this server (without including retransmissions)
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatReqGet(L7_uint32 unitIndex,
                                              L7_uchar8 *pHostAddr,
                                              L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatReqGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Request packets
* @         retransmitted to this server
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatRetransGet(L7_uint32 unitIndex,
                                                  L7_uchar8 *pHostAddr,
                                                  L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatRetransGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Response packets
* @         received from this server
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatResponseGet(L7_uint32 unitIndex,
                                             L7_uchar8 *pHostAddr,
                                             L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatResponseGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of malformed RADIUS Accounting-Response packets
* @         received from this server. Malformed packets include packets
* @         with an invalid length. Bad authenticators or signature attributes
* @         or unknown types are not included
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatMalformedResponseGet(L7_uint32 unitIndex,
                                            L7_uchar8 *pHostAddr,
                                            L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatMalformedResponseGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Response packets
* @         containing invalid authenticators or signature attributes received
* @         from this server.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Host Name or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatBadAuthGet(L7_uint32 unitIndex,
                                            L7_uchar8 *pHostAddr,
                                            L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatBadAuthGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Request packets
* @         destined for this server that have not yet timed out or received
* @         a response from this server
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatPendingReqGet(L7_uint32 unitIndex,
                                               L7_uchar8 *pHostAddr,
                                               L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatPendingReqGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of authentication time-outs to this server
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Host Name or IP Address of the radius accounting
* @         server
* @param    pTimeouts @b((output)) ptr to number of timeouts
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatTimeoutsGet(L7_uint32 unitIndex,
                                             L7_uchar8 *pHostAddr,
                                             L7_uint32 *pTimeouts)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pTimeouts == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pTimeouts = radiusAcctServerStatTimeoutsGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS packets of unknown type which
* @         were received from this server on the accounting port
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatUnknownTypeGet(L7_uint32 unitIndex,
                                                L7_uchar8 *pHostAddr,
                                                L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatUnknownTypeGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS packets received from this server on
* @         the accounting port that were dropped for some other reason.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) HostName or IP Address of the radius accounting
* @         server
* @param    pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_FAILURE if the server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatPktsDroppedGet(L7_uint32 unitIndex,
                                                L7_uchar8 *pHostAddr,
                                                L7_uint32 *pPacketCount)
{
  dnsHost_t dnsHostAddr;

  memset(&dnsHostAddr, L7_NULL, sizeof(dnsHostAddr));

  if (usmDbRadiusConvertToDnsHostAddr(unitIndex, pHostAddr,
                                      &dnsHostAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatPktsDroppedGet(&dnsHostAddr);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting Response packets
*          received from unknown addresses.
*
* @param unitIndex @b((input)) the unit for this operation
* @param pPacketCount @b((output)) ptr to number of packets
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerStatInvalidAddressesGet(L7_uint32 unitIndex,
                                                     L7_uint32 *pPacketCount)
{
  if (pPacketCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *pPacketCount = radiusAcctServerStatInvalidAddressesGet();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Clear server (accounting/authorization) stats
*
* @param    unitIndex @b((input)) the unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t usmDbRadiusStatsClear(L7_uint32 unitIndex)
{
    return radiusStatsClear();
}

/*********************************************************************
*
* @purpose Get the NAS-Identifier attribute as is used by
*          the RADIUS client code.
*
* @param    unitIndex @b((input)) the unit for this operation
* @params pNasId @b((input/output)) ptr to location to write the NAS-Identifier
* @params pNasIdSize @b((input/output)) ptr to location to write the
*         NAS-Identifier size
*
* @returns void
*
* @comments nasId must accomadate a string equal or greater than
*           L7_RADIUS_NAS_IDENTIFIER_SIZE + 1. nasIdSize will NOT include
*           the null termination character.
*
* @end
*
*********************************************************************/
void usmDbRadiusNASIdentifierGet(L7_uint32 unitIndex, L7_uchar8 *pNasId,
                            L7_uint32 *pNasIdSize)
{
    radiusNASIdentifierGet( pNasId, pNasIdSize );
    return;
}

/*********************************************************************
*
* @purpose Set the the value of the RADIUS NAS-IP Attributes
*
* @param  unitIndex @b((input)) the unit for this operation
* @param  mode      @b{(input)} Radius NAS-IP Mode.
* @param  ipAddr    @b{(input)} Radius NAS-IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAttribute4Set(L7_uint32 unitIndex,
                                 L7_BOOL   mode,
                                 L7_uint32 ipAddr)
{
  return radiusAttribute4Set(mode,ipAddr);
}


/*********************************************************************
*
* @purpose Get the the value of the RADIUS NAS-IP Attributes
*
* @param  unitIndex @b((input)) the unit for this operation
* @param  mode      @b{(input)} Radius NAS-IP Mode.
* @param  ipAddr    @b{(input)} Radius NAS-IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAttribute4Get(L7_uint32 unitIndex,
                                 L7_BOOL   *mode,
                                 L7_uint32 *ipAddr)
{
  return radiusAttribute4Get(mode,ipAddr);
}
/*********************************************************************
*
* @purpose  Covert to DNS Host Address.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr  @b((input)) Host Address of the radius server
* @param    pDnsHost  @b((output)) ptr to DNS Host
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusConvertToDnsHostAddr(L7_uint32 unitIndex,
                                        L7_char8  *pHostAddr,
                                        dnsHost_t *pDnsHost)
{
  L7_uint32 ipAddr = L7_NULL;

  if (usmDbInetAton(pHostAddr, &ipAddr) != L7_SUCCESS)
  {
    pDnsHost->hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
    osapiStrncpySafe(pDnsHost->host.hostName, pHostAddr,
                     sizeof(pDnsHost->host.hostName));
  }
  else
  {
    pDnsHost->hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
    pDnsHost->host.ipAddr = ipAddr;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the first Accounting configured server
*
* @param unitIndex   @b((input)) the unit for this operation
* @param pServerAddr @b((output)) ptr to the first configured server in the set
* @param type        @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAcctServerFirstIPHostNameGet(L7_uint32 unitIndex,
                                            L7_uchar8 *pServerAddr,
                                            L7_IP_ADDRESS_TYPE_t *pType)
{
  if ((pServerAddr == L7_NULLPTR) || (pType == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  return radiusAcctServerFirstIPHostNameGet(pServerAddr, pType);
}

/*********************************************************************
*
* @purpose Set the serverName attribute to the acct. server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the HostName or IP address to be set
* @param L7_IP_ADDRESS_TYPE_t type @b((input))     Address type either ipv4 or dns
* @param L7_char8 *serverName  @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerNameSet(L7_uint32 unitIndex,
                              L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  return radiusServerAcctNameSet(serverAddr, type, serverName);
}

/*********************************************************************
*
* @purpose Set the serverName attribute to the Auth. server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverAddr @b((input)) the HostName or IP address to be set
* @param L7_IP_ADDRESS_TYPE_t type @b((input))     Address type either ipv4 or dns
* @param L7_char8 *serverName  @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerNameSet(L7_uint32 unitIndex,
                              L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  return radiusServerAuthNameSet(serverAddr, type, serverName);
}

/*********************************************************************
*
* @purpose Get the server Name of specified radius server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverName  @b((input)) Name of the server.
* @param L7_char8 *pServerAddr @b((output)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((output)) Address Type.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusNamedServerAddrGet(L7_uint32 unitIndex, L7_char8 *serverName, L7_uchar8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType)
{
  return radiusNamedServerAddrGet(serverName, L7_RADIUS_SERVER_USAGE_ANY, pServerAddr, pType);
}

/*********************************************************************
*
* @purpose Get the server Name of specified acct server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverName  @b((input)) Name of the server.
* @param L7_char8 *pServerAddr @b((output)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((output)) Address Type.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusNamedAcctServerAddrGet(L7_uint32 unitIndex, L7_char8 *serverName, L7_char8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType)
{
  return radiusNamedAcctServerAddrGet(serverName, pServerAddr, pType);
}

/*********************************************************************
*
* @purpose Get the server Name of specified radius server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverAddr  @b((input)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((input)) Address Type.

* @param L7_char8 *serverName  @b((output)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerHostNameGet(L7_uint32 unitIndex, L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  return radiusServerHostNameGet(serverAddr, type, serverName);

}

/*********************************************************************
*
* @purpose Get the server Name of specified accounting server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverAddr  @b((input)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((input)) Address Type.

* @param L7_char8 *serverName  @b((output)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerAcctHostNameGet(L7_uint32 unitIndex, L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  return radiusServerAcctHostNameGet(serverAddr, type, serverName);
}

/*********************************************************************
*
* @purpose Get the Server Name and Address of the next named auth server
*          from the servers' list.  The address could be DNS Host Name or IP
*          address.
*
* @param L7_char8 *name      @b((input)) Name of the configured Auth Server.
* @param L7_char8 *name      @b((input)) Name of the next configured Server.
* @param L7_char8 *nextAddr  @b((output)) Host name or IP address of the next
*                                   configured server
*
* @returns L7_SUCCESS - if there is a named server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusNamedServerOrderlyGetNext(L7_uint32 unitIndex,
                     L7_char8 *name, L7_char8 *nextName,
                     L7_uchar8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType)
{
  return radiusNamedServerOrderlyGetNext(name, nextName,
                     nextAddr ,pType);

}

/*********************************************************************
*
* @purpose Get the Server Name and Address of the next named auth server
*          from the servers' list.  The address could be DNS Host Name or IP
*          address.
*
* @param L7_char8 *name      @b((input)) Name of the configured Auth Server.
* @param L7_char8 *name      @b((input)) Name of the next configured Server.
* @param L7_char8 *nextAddr  @b((output)) Host name or IP address of the next
*                                   configured server
*
* @returns L7_SUCCESS - if there is a named server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusNamedAcctServerOrderlyGetNext(L7_uint32 unitIndex,
                     L7_char8 *name, L7_char8 *nextName,
                     L7_char8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType)
{
  return radiusNamedAcctServerOrderlyGetNext(name, nextName,
                     nextAddr ,pType);
}

/*********************************************************************
*
* @purpose Get the Number of configured servers of specified
*                    type (Auth or Acct).
*
* @param radiusServerType_t type @b((input)) Auth/Acct.
* @param L7_uint32 *count @b((input)) Pointer to the count.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusConfiguredServersCountGet( L7_uint32 unitIndex,
                     radiusServerType_t type,
                     L7_uint32 *count)
{
  return radiusConfiguredServersCountGet(type, count);
}

/*********************************************************************
*
* @purpose Get the Number of configured Named server groups of specified
*                    type (Auth or Acct).
*
* @param radiusServerType_t type @b((input)) Auth/Acct.
* @param L7_uint32 *count @b((input)) Pointer to the count.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusConfiguredNamedServersGroupCountGet(L7_uint32 unitIndex,
                     radiusServerType_t type,
                     L7_uint32 *count)
{
  return radiusConfiguredNamedServersGroupCountGet(type, count);
}

/*********************************************************************
*
* @purpose Get the IP address of the specified DNS server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param dnsAddr @b((input)) Pointer to the dns address string of
*                                   configured server
* @param radiusServerType_t type @b((input)) Auth/Acct.
* @param ipAddr @b((output)) Pointer to the IP address string.
*
* @returns L7_SUCCESS - if there is a server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusServerHostIPAddrGet(L7_uint32 unitIndex,
                     L7_uchar8 *dnsAddr,radiusServerType_t type, L7_uint32 *ipAddr)
{
  return radiusServerHostIPAddrGet(dnsAddr,type, ipAddr);

}

/*********************************************************************
*
* @purpose Get the DNS-Host-Name or Host-IP-Address value of the Current RADIUS server
*          in the group of Named RADIUS servers that bear the same given name.
*
* @param unitIndex @b((input)) the unit for this operation
* @param serverName @b((input)) the common Name of a group of servers
* @param serverAddr @b((output)) the DNS HostName or IP address of the Current
*                                radius server in the group of Named Radius
*                                Servers which the same given name.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE - if there is no auth server with given name
*
* @comments This index value of the Auth. & Acct. servers points
*           to respective serverName arrays. And is different from
*           indexing given for each server entry in server entries List.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAuthServerNameCorrespondingCurrentServerGet(L7_uint32 unitIndex,
                                 L7_uchar8 *serverName, L7_char8 *currServerHostName)
{
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

  /* Name based walk through the server Table */
  if( radiusNamedServerOrderlyGetNext(name, name, host, &addrType) != L7_SUCCESS )
  {
    /* Failed to get first entry. */
    return L7_FAILURE;
  }

  do
  {
    if(osapiStrncmp(serverName,name,sizeof(name)) == 0)
    {
      /* Found the corresponding Current Server. */
      osapiStrncpySafe(currServerHostName, host, sizeof(host));
      return L7_SUCCESS;
    }
    memset(host,0,L7_DNS_HOST_NAME_LEN_MAX);
  }while(radiusNamedServerOrderlyGetNext(name, name, host, &addrType) == L7_SUCCESS);

  return L7_FAILURE;
}
