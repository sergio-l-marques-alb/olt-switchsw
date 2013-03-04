/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename usmdb_tacacs_include.h
*
* @purpose TACACS+ Client include file
*
* @component tacacs+
*
* @comments none
*
* @create 04/10/2005
*
* @author gkiran
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_USMDB_TACACS_H
#define INCLUDE_USMDB_TACACS_H
/*********************************************************************
*
* @purpose To set the TACACS+ Server port number.
*
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} Address type DNS or IP address
* @param L7_uchar8   serverAddress @b{(input)} the Hostname or
*                                              IP address of the server.
* @param portNum                   @b{(input)} the port number value to be set.
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
                                         L7_ushort16 portNum);

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
                                         L7_short16 *pPortNum);

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
                                      L7_uchar8 timeOutVal);

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
                                      L7_uchar8 *pTimeOutVal);

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
                                                     L7_BOOL   *useGlobal);
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
                                  L7_uchar8 *pKey);

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
                                  L7_uchar8 *pSecretKey);
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
                                                 L7_BOOL    *useGlobal);

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
                                       L7_ushort16 priority);

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
                                       L7_ushort16 *pPriority);

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
L7_RC_t usmDbTacacsSourceIpAddrSet(L7_uint32 ipAddress,L7_uint32 sourceIp);

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
                                   L7_uint32 *pSourceIp);

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
                                          L7_uchar8  *pServerAddress);

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
                                         L7_uchar8  *pNextServerAddress);
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
                             L7_uchar8  *serverAddress);
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
                                        L7_uchar8  *serverAddress);

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
L7_RC_t usmDbTacacsGblSrcIpAddrSet(L7_uint32 ipAddress);
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
L7_RC_t usmDbTacacsGblSrcIpAddrGet(L7_uint32 *pSourceIp);
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
L7_RC_t usmDbTacacsGblTimeOutSet(L7_uchar8 timeOut);
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
L7_RC_t usmDbTacacsGblTimeOutGet(L7_uchar8 *pTimeOut);
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
L7_RC_t usmDbTacacsGblKeySet(L7_uchar8 *pSecretKey);
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
L7_RC_t usmDbTacacsGblKeyGet(L7_uchar8 *pSecretKey);

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
L7_RC_t usmDbTacacsHostNameServerConnectionStatusGet(L7_IP_ADDRESS_TYPE_t type, L7_uchar8 *serverAddress, L7_BOOL *status);

#endif /* INCLUDE_USMDB_TACACS_H */

