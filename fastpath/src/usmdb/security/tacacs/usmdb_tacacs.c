/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename usmdb_tacacs.c
*
* @purpose TACACS+  Client include file
*
* @component tacacs
*
* @comments
*
* @create 04/10/2003
*
* @author gkiran
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "usmdb_tacacs_api.h"
#include "tacacs_api.h"

/*********************************************************************
*
* @purpose To set the TACACS+ Server port number.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param portNum       @b{(input)} the port number value to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNamePortNumberSet(L7_IP_ADDRESS_TYPE_t type,
                                         L7_uchar8  *serverAddress,
                                         L7_ushort16 portNum)
{
  return(tacacsHostNameServerPortNumberSet(type, serverAddress, portNum));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Server port number.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param pPortNum      @b{(output)} the port number value to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNamePortNumberGet(L7_IP_ADDRESS_TYPE_t type,
                                         L7_uchar8  *serverAddress,
                                         L7_short16 *pPortNum)
{
  return(tacacsHostNameServerPortNumberGet(type, serverAddress, pPortNum));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Server timeout value.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param timeOutVal    @b{(input)} the port number value to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameTimeOutSet(L7_IP_ADDRESS_TYPE_t type,
                                      L7_uchar8  *serverAddress,
                                      L7_uchar8 timeOutVal)
{
  return(tacacsHostNameServerTimeoutSet(type, serverAddress, timeOutVal));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Server timeout value.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param pTimeOutVal   @b{(output)} the time out value to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameTimeOutGet(L7_IP_ADDRESS_TYPE_t type, 
                                      L7_uchar8  *serverAddress, 
                                      L7_uchar8 *pTimeOutVal)
{
  return(tacacsHostNameServerTimeoutGet(type, serverAddress, pTimeOutVal));
}
/*********************************************************************
*
* @purpose Get the TACACS+ server time out global flag.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param *useGlobal    @b{(output)} L7_TRUE, using global timeout
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerUseGlobalTimeoutGet(L7_IP_ADDRESS_TYPE_t type,
                                                     L7_uchar8  *serverAddress,
                                                     L7_BOOL    *useGlobal)
{
  return(tacacsHostNameServerUseGlobalTimeoutGet(type, serverAddress, 
                                                 useGlobal));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Server Key.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param pKey          @b{(input)} the Key string to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameKeySet(L7_IP_ADDRESS_TYPE_t type,
                                  L7_uchar8  *serverAddress,
                                  L7_uchar8 *pKey)
{
  return(tacacsHostNameServerSecretKeySet(type, serverAddress, pKey));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Server Secret Key.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param pSecretKey    @b{(output)} the Secret Key Value.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameKeyGet(L7_IP_ADDRESS_TYPE_t type, 
                                  L7_uchar8  *serverAddress, 
                                  L7_uchar8 *pSecretKey)
{
  return(tacacsHostNameServerSecretKeyGet(type, serverAddress, pSecretKey));
}
/*********************************************************************
*
* @purpose Get the TACACS+ server key global flag.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param L7_BOOL   *useGlobal @b{(output)} L7_TRUE, using global key
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerUseGlobalKeyGet(L7_IP_ADDRESS_TYPE_t type,
                                                 L7_uchar8  *serverAddress,
                                                 L7_BOOL    *useGlobal)
{
  return(tacacsHostNameServerUseGlobalKeyGet(type, serverAddress, useGlobal));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Server Priority.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param priority      @b{(input)} the priority to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNamePrioritySet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8  *serverAddress,
                                       L7_ushort16 priority)
{
  return(tacacsHostNameServerPrioritySet(type, serverAddress, priority));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Server Priority.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param pPriority     @b{(output)} the priority to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNamePriorityGet(L7_IP_ADDRESS_TYPE_t type, 
                                       L7_uchar8  *serverAddress, 
                                       L7_ushort16 *pPriority)
{
  return(tacacsHostNameServerPriorityGet(type, serverAddress, pPriority));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Server Source Ip Address.
*
* @param ipAddress    @b{(input)} the IP address of the server.
* @param sourceIp     @b{(input)} the Source Ip Address to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsSourceIpAddrSet(L7_uint32 ipAddress,L7_uint32 sourceIp)
{
  return(tacacsServerSrcIpSet(ipAddress,sourceIp));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Server Source Ip Address.
*
* @param type          @b{(input)} Address type DNS or IP address
* @param serverAddress @b{(input)} the Hostname or IP address of the server.
* @param pSourceIp     @b{(output)} the Source Ip Address to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsSourceIpAddrGet(L7_IP_ADDRESS_TYPE_t type, 
                                   L7_uchar8  *serverAddress, 
                                   L7_uint32 *pSourceIp)
{
  return(tacacsHostNameServerSrcIpGet(type, serverAddress, pSourceIp));
}
/*********************************************************************
*
* @purpose To get the TACACS+ First Server Entry Ip Adress.
*
* @param type           @b{(inout)} Address type DNS or IP address
* @param pServerAddress @b{(output)} the Hostname or IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerFirstGet(L7_IP_ADDRESS_TYPE_t *type, 
                                          L7_uchar8  *pServerAddress)
{
  return(tacacsHostNameServerFirstGet(type, pServerAddress));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Next Server Entry Ip Adress.
*
* @param serverAddress  @b{(input)} the Hostname or IP address of the server.
* @param type           @b{(inout)} Address type DNS or IP address
* @param pNextServerAddress @b{(output)} the Hostname or IP address 
*                                        of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerNextGet(L7_uchar8  *serverAddress, 
                                         L7_IP_ADDRESS_TYPE_t *type, 
                                         L7_uchar8  *pNextServerAddress)
{
  return(tacacsHostNameServerNextGet(serverAddress, type, pNextServerAddress));
}

/*********************************************************************
*
* @purpose To Add TACACS+ Server Entry.
*
* @param type           @b{(input)} Address type DNS or IP address
* @param serverAddress  @b{(input)} the Hostname or IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerAdd(L7_IP_ADDRESS_TYPE_t type, 
                                     L7_uchar8  *serverAddress)
{
  return (tacacsHostNameServerAdd(type, serverAddress));
}

/*********************************************************************
*
* @purpose To Remove TACACS+ Server Entry.
*
* @param type           @b{(input)} Address type DNS or IP address
* @param serverAddress  @b{(input)} the Hostname or IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerRemove(L7_IP_ADDRESS_TYPE_t type, 
                                        L7_uchar8  *serverAddress)
{
  return(tacacsHostNameServerRemove(type, serverAddress));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Global Source Ip Address.
*
* @param ipAddress    @b{(input)} the Global Source Ip Address to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsGblSrcIpAddrSet(L7_uint32 ipAddress)
{
  return(tacacsGblSrcIpSet(ipAddress));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Gloabal Source Ip Address.
*
* @param pSourceIp    @b{(output)} the Global Source Ip Address to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsGblSrcIpAddrGet(L7_uint32 *pSourceIp)
{
  return(tacacsGblSrcIpGet(pSourceIp));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Global Time Out Value.
*
* @param timeOut    @b{(input)} the Global timeout Value to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsGblTimeOutSet(L7_uchar8 timeOut)
{
  return(tacacsGblTimeoutSet(timeOut));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Global Time Out Value.
*
* @param *pTimeOut    @b{(output)} the Global timeout Value to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsGblTimeOutGet(L7_uchar8 *pTimeOut)
{
  return(tacacsGblTimeoutGet(pTimeOut));
}
/*********************************************************************
*
* @purpose To set the TACACS+ Global Secret Key.
*
* @param pSecretKey    @b{(input)} the Global Secret Key to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsGblKeySet(L7_uchar8 *pSecretKey)
{
  return(tacacsGblSecretKeySet(pSecretKey));
}
/*********************************************************************
*
* @purpose To get the TACACS+ Global Secret Key.
*
* @param pSecretKey    @b{(output)} the Global Secret Key to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsGblKeyGet(L7_uchar8 *pSecretKey)
{
  return(tacacsGblSecretKeyGet(pSecretKey));
}

/*********************************************************************
*
* @purpose To get the TACACS+ Connection status.
*
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTacacsHostNameServerConnectionStatusGet(L7_IP_ADDRESS_TYPE_t type, L7_uchar8 *serverAddress, L7_BOOL *status)
{
	return tacacsHostNameServerConnectionStatusGet(type, serverAddress, status );
}
