/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_api.h
*
* @purpose TACACS+ Client API Header file.
*
* @component tacacs+
*
* @comments none
*
* @create 04/08/2005
*
* @author gkiran
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_TACACS_API_H
#define INCLUDE_TACACS_API_H

#include "l7_common.h"
#include "user_manager_exports.h"
#include "user_manager_exports.h"

typedef enum
{
  TACACS_STATUS_AUTH_SUCCESS = 1,
  TACACS_STATUS_AUTH_FAILURE = 2,
  TACACS_STATUS_SERVER_FAILURE = 3,
  TACACS_STATUS_SESSION_TIMEOUT = 4,
  TACACS_STATUS_AUTH_CHALLENGE,      /* This is used to request data (username/password) from the user */
  TACACS_STATUS_OTHER_FAILURE
} tacacsAuthStatus_t;

#define  L7_TACACS_MULTICAST_ADDR        0xe0000000

/*************************************************************************
*
* @purpose Authenticate a user with a TACACS+ server
*
* @param L7_char8  *username     @b{(input)} name of user to be authenticated.
* @param L7_char8  *password     @b{(input)} password for the user.
* @param L7_uint32  correlator   @b{(input)} correlator.
* @param L7_uint32  componentId  @b{(input)} component Id.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t tacacsUserAuthenticate(userMgrAuthRequest_t *request,
                               L7_uint32  correlator);

/*************************************************************************
*
* @purpose Command Authorization with a TACACS+ server
*
* @param L7_char8  *username     @b{(input)} name of user to be authenticated.
* @param L7_uint32  correlator   @b{(input)} correlator.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t tacacsUserCmdAuthorize( userMgrAuthRequest_t *request,
                                L7_uint32  correlator);

/*********************************************************************
*
* @purpose Register a routine to be called when a TACACS+ response is
*          received from a server for a previously submitted request.
*
* @param L7_uint32 componentId @b((input)) routine registrar id (See L7_COMPONENT_ID_t)
* @param *notify @b((input)) pointer to a routine to be invoked upon a respones.
*                            Each routine has the following parameters:
*                            (status, correlator, *attributes, attributesLen)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t tacacsResponseRegister(L7_COMPONENT_IDS_t registrar_ID,
                               L7_uint32 (*notify)(L7_uint32 status,
                              L7_uint32 correlator, L7_uint32 accessLevel));
/*********************************************************************
*
* @purpose Deregister all routines to be called when a tacacs response is
*          received from a server for a previously submitted request.
*
* @param   L7_uint32 componentId  @b{(input)}  one of L7_COMPONENT_IDS_t
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t tacacsResponseDeregister( L7_COMPONENT_IDS_t componentId);
/*********************************************************************
*
* @purpose Set the TACACS+ server port number.
*
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} Address type DNS or IP address
* @param L7_uchar8   serverAddress @b{(input)} the Hostname or
*                                              IP address of the server.
* @param L7_ushort16 port          @b{(input)} the port number value to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPortNumberSet(L7_IP_ADDRESS_TYPE_t type,
                                          L7_uchar8  *serverAddress,
                                          L7_ushort16 port);

/*********************************************************************
*
* @purpose Get the TACACS+ server port number.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_short16 *port         @b{(output)}pointer to port number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPortNumberGet(L7_IP_ADDRESS_TYPE_t type,
                                          L7_uchar8  *serverAddress,
                                          L7_short16 *port);

/*********************************************************************
*
* @purpose Set the TACACS+ server time out value.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uchar8 timeout        @b{(input)} connection timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes timeout == 0 indicates to use global timeout configuration,
*        by default each server uses global value unless set.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerTimeoutSet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8  *serverAddress,
                                       L7_uchar8   timeout);

/*********************************************************************
*
* @purpose Get the TACACS+ server time out.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uchar8 *timeout       @b{(output)} pointer to timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerTimeoutGet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8  *serverAddress,
                                       L7_uchar8 *timeout);

/*********************************************************************
*
* @purpose Get the TACACS+ server time out global flag.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_BOOL   *useGlobal     @b{(output)} L7_TRUE, using global timeout
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerUseGlobalTimeoutGet(L7_IP_ADDRESS_TYPE_t type,
                                                L7_uchar8  *serverAddress,
                                                L7_BOOL    *useGlobal);

/*********************************************************************
*
* @purpose Set the TACACS+ server secret key.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_char8  *key           @b{(input)} encryption key.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes key == L7_NULLPTR or "" indicates to use global key configuration,
*        by default each server uses global value unless set.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerSecretKeySet(L7_IP_ADDRESS_TYPE_t type,
                                         L7_uchar8  *serverAddress,
                                         L7_char8   *key);
/*********************************************************************
*
* @purpose Get the TACACS+ server key configuration.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uchar8 *key           @b{(output)} key string
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerSecretKeyGet(L7_IP_ADDRESS_TYPE_t type,
                                         L7_uchar8  *serverAddress,
                                         L7_uchar8  *key);

/*********************************************************************
*
* @purpose Get the TACACS+ server key global flag.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_BOOL   *useGlobal     @b{(output)} L7_TRUE, using global key
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerUseGlobalKeyGet(L7_IP_ADDRESS_TYPE_t type,
                                            L7_uchar8  *serverAddress,
                                            L7_BOOL    *useGlobal);

/*********************************************************************
*
* @purpose To set the TACACS+ server priority.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_ushort16  priority    @b{(input)} the server priority.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPrioritySet(L7_IP_ADDRESS_TYPE_t type,
                                        L7_uchar8  *serverAddress,
                                        L7_ushort16 priority);
/*********************************************************************
*
* @purpose Get the TACACS+ server priority configuration.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_ushort16  *priority   @b{(output)} pointer to server priority
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPriorityGet(L7_IP_ADDRESS_TYPE_t type,
                                        L7_uchar8  *serverAddress,
                                        L7_ushort16 *priority);

/*********************************************************************
*
* @purpose To Set the TACACS+ server Source Ip Address.
*
* @param L7_uint32 ipAddress @b{(input)} the IP address of the server.
* @param L7_uint32 sourceIP  @b{(input)} the Source Ip Address to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes need some ipAddress value to indicate global configuration
*         TBD - 255.255.255.255?
*        NOT IMPLEMENTED, place holder for L3 multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerSrcIpSet(L7_uint32 ipAddress,
                             L7_uint32 sourceIP);
/*********************************************************************
*
* @purpose Get the TACACS+ server source IP configuration
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uint32 *sourceIP      @b{(output)} pointer to source IP
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerSrcIpGet(L7_IP_ADDRESS_TYPE_t type,
                                     L7_uchar8  *serverAddress,
                                     L7_uint32 *sourceIP);

/*********************************************************************
*
* @purpose Get the first configured TACACS+ server.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(output)} Address type DNS or IP address
* @param L7_uchar8 pServerAddress @b{(output)} the Hostname or
*                                              IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure, no servers configured.
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerFirstGet(L7_IP_ADDRESS_TYPE_t *type,
                                     L7_uchar8  *pServerAddress);

/*********************************************************************
*
* @purpose Get the next configured TACACS+ server.
*
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_IP_ADDRESS_TYPE_t type@b{(inout)} Address type DNS or IP address
* @param L7_uchar8  pNextServerAddress @b{(output)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes ipAddress of 0 returns first configured server.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerNextGet(L7_uchar8  *serverAddress,
                                    L7_IP_ADDRESS_TYPE_t *type,
                                    L7_uchar8  *pNextServerAddress);

/*********************************************************************
*
* @purpose Add a new TACACS+ server configuration.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerAdd(L7_IP_ADDRESS_TYPE_t type,
                                L7_uchar8  *serverAddress);

/*********************************************************************
*
* @purpose To delete TACACS+ server configuration
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerRemove(L7_IP_ADDRESS_TYPE_t type,
                                   L7_uchar8  *serverAddress);

/*********************************************************************
*
* @purpose To set the TACACS+ global source IP configuration
*
* @param L7_uint32 ipAddress  @b{(input)} source IP address.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes TBD - placeholder for L3 multiple interfaces, not implemented.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSrcIpSet(L7_uint32 ipAddress);
/*********************************************************************
*
* @purpose To get the TACACS+ global source IP configuration
*
* @param L7_uint32 *ipAddress  @b{(output)} pointer to source IP.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSrcIpGet(L7_uint32 *ipAddress);
/*********************************************************************
*
* @purpose To set the TACACS+ global timeout configuration.
*
* @param L7_uchar8 timeout  @b{(input)} connection timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblTimeoutSet(L7_uchar8 timeout);
/*********************************************************************
*
* @purpose To get the TACACS+ global timeout configuration
*
* @param L7_uchar8 *timeout  @b{(output)} pointer to connection timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblTimeoutGet(L7_uchar8 *timeout);
/*********************************************************************
*
* @purpose To set the TACACS+ global key configuration
*
* @param L7_uchar8 *key  @b{(input)} global key string.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSecretKeySet(L7_uchar8 *key);
/*********************************************************************
*
* @purpose To get the TACACS+ Gloabal Secret Key.
*
* @param L7_uchar8 *key  @b{(output)} the Global Secret Key to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSecretKeyGet(L7_uchar8 *key);

/*********************************************************************
*
* @purpose To get the TACACS+ server connection status.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_BOOL   *status        @b{(output)} L7_TRUE(connected) or
*                                              L7_FALSE(disconnected)
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes only a single-connection server will ever be connected,
*        and single-connection is not implemented, keep configuration
*        and operational structures as placeholder.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerConnectionStatusGet(L7_IP_ADDRESS_TYPE_t type,
                                                L7_uchar8  *serverAddress,
                                                L7_BOOL    *status);

#endif /* INCLUDE_TACACS_API_H */
