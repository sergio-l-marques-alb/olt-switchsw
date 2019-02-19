/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_radius_include.h
*
* @purpose RADIUS Client include file
*
* @component radius
*
* @comments none
*
* @create 03/27/2003
*
* @author pmurthy
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_USMDB_RADIUS_H
#define INCLUDE_USMDB_RADIUS_H
#include "commdefs.h"
#include "radius_exports.h"

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
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                    L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                     L7_uint32 *serverPriority);

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
                                     L7_uint32 serverPriority);

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
                                      L7_RADIUS_SERVER_USAGE_TYPE_t *usageType);

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
                                      L7_RADIUS_SERVER_USAGE_TYPE_t usageType);

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
                                     L7_uint32 *pAccMode);

/*********************************************************************
*
* @purpose Set the value of the radius-accounting mode parameter
*          
* @param unitIndex @b((input)) the unit for this operation
* @param accntMode @b((input)) the new value of the radiusAccountingMode
*  			                   (L7_ENABLE or L7_DISABLE)
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
                                     L7_uint32 accntMode);

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
                                              L7_BOOL *verified);

/*********************************************************************
*
* @purpose  Get the IP address of the radius accounting server corresponding 
* to the index input
*          
* @param unitIndex @b((input)) the unit for this operation
* @param    index  @b((input))  Index of the Accounting Server
* @param    ipAddr @b((output)) IP address of the Accounting Server
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerIPByIndexGet(L7_uint32 unitIndex, 
                                                L7_uint32 index, 
                                                L7_uint32 *pIpAddr);

/*********************************************************************
*
* @purpose  Get the HostName or IP address of the radius accounting server
*           corresponding  to the index input
*
* @param unitIndex @b((input)) the unit for this operation
* @param    index  @b((input))  Index of the Accounting Server
* @param    ipAddr @b((output)) HostName or IP address of the Accounting Server
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
                                                L7_IP_ADDRESS_TYPE_t *pType);

/*********************************************************************
*
* @purpose Get the HostName or IP address of the accounting server being used
*
* @param unitIndex @b((input)) the unit for this operation
* @param pServAddr @b((output)) ptr to the accounting server HostName or 
*                               IP address
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
                                                 L7_IP_ADDRESS_TYPE_t *pType);

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
                          L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type,L7_char8 *serverName);
/*********************************************************************
*
* @purpose Remove an accounting server 
*          
* @param unitIndex @b((input)) the unit for this operation
* @param ipAddr @b((input)) the IP address to be set
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbRadiusAccountingServerRemove(L7_uint32 unitIndex,
                                          L7_uint32 ipAddr);

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
                        L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type);

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
                                              L7_uint32 *pPort);

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
                                             L7_uint32 portNum);

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
                                                   L7_char8 *sharedSecret);

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
                                           L7_BOOL *pVal );

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
                                           L7_uchar8 *secret );

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
                                    L7_BOOL *verified);

/*********************************************************************
*
* @purpose Get the IP address of the radius server corresponding to the
*          index input
*
* @param unitIndex @b((input)) the unit for this operation
* @param index  @b((input))  Index of the Auth Server
* @param ipAddr @b((output)) HostName or IP address of the Auth Server
* @param pType          @b((output))Address type DNS or IP address
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
                                              L7_IP_ADDRESS_TYPE_t *pType);

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
                                      L7_uint32 *pIpAddr);

/*********************************************************************
*
* @purpose Get the first configured server
*
* @param unitIndex @b((input)) the unit for this operation
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
                                            L7_IP_ADDRESS_TYPE_t *pType);

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
                                           L7_IP_ADDRESS_TYPE_t *pType);

/*********************************************************************
*
* @purpose Get the configured server next in the list after the specified server
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
                                           L7_IP_ADDRESS_TYPE_t *pType);

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
                     L7_char8 *serverName);

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
                     L7_uchar8 *servAddr, L7_IP_ADDRESS_TYPE_t type);

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
                                  L7_uint32 *pVal);

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
                                    L7_uint32 *pPort);

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
                                    L7_uint32 portNum);

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
                                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                                        L7_BOOL   *pFlag );

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
                                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                    L7_uint32 *pIpAddr);

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
                                        L7_uchar8 *pservAddr);

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
                                      L7_uint32 *pType);

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
                                      L7_uint32 radiusType);

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
                                           L7_uint32 mode);

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
                                           L7_uint32 *mode);

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
                                    L7_uint32 *pRoundTripTime);

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
                                          L7_uint32 *pPacketCount);

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
                                              L7_uint32 *pPacketCount);
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
                                             L7_uint32 *pPacketCount);

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
                                             L7_uint32 *pPacketCount);

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
                                                L7_uint32 *pPacketCount);

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
                                                        L7_uint32 *pPacketCount);

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
                                        L7_uint32 *pPacketCount);

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
                                           L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose Retrieve the number of authentication time-outs to this server
*          
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                         L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets of unknown type which 
*          were received from this server on the authentication port
*          
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                            L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets received from this server on
*          the authentication port that were dropped for some other reason.
*          
* @param unitIndex @b((input)) the unit for this operation
* @param pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                            L7_uint32 *pPacketCount);


/* Radius-client status parameters for the accounting server */
/*********************************************************************
*
* @purpose  Get the round trip time 
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                        L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Request packets sent 
* @         to this server (without including retransmissions)
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                        L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Request packets 
* @         retransmitted to this server
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                            L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Response packets 
* @         received from this server
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                             L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of malformed RADIUS Accounting-Response packets 
* @         received from this server. Malformed packets include packets
* @         with an invalid length. Bad authenticators or signature attributes
* @         or unknown types are not included
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                                      L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Response packets 
* @         containing invalid authenticators or signature attributes received 
* @         from this server. 
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                            L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting-Request packets 
* @         destined for this server that have not yet timed out or received
* @         a response from this server
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                               L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of authentication time-outs to this server
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                             L7_uint32 *pTimeouts);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS packets of unknown type which 
* @         were received from this server on the accounting port
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                                L7_uint32 *pPacketCount);

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS packets received from this server on
* @         the accounting port that were dropped for some other reason.
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    pHostAddr @b((input)) Hostname or IP Address of the radius server 
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
                                                L7_uint32 *pPacketCount);

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
L7_RC_t usmDbRadiusStatInvalidAddressesGet(L7_uint32 unitIndex,
                                           L7_uint32 *pPacketCount);

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
                                                     L7_uint32 *pPacketCount);

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
L7_RC_t usmDbRadiusStatsClear(L7_uint32 unitIndex);

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
                            L7_uint32 *pNasIdSize);

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
                                 L7_uint32 ipAddr);

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
                                 L7_uint32 *ipAddr);

/*********************************************************************
*
* @purpose  Covert to DNS Host Address.
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    hostAddr  @b((input)) Host Address of the radius server 
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
                                        L7_char8  *hostAddr,
                                        dnsHost_t *pDnsHost);

/*********************************************************************
*
* @purpose Get the first configured Accountng server
*
* @param unitIndex @b((input)) the unit for this operation
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
                                            L7_IP_ADDRESS_TYPE_t *pType);

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
                              L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
L7_RC_t usmDbRadiusNamedServerAddrGet(L7_uint32 unitIndex, L7_char8 *serverName, L7_uchar8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType);

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
L7_RC_t usmDbRadiusNamedAcctServerAddrGet(L7_uint32 unitIndex, L7_char8 *serverName, L7_char8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType);

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
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
                              L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
                     L7_uchar8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType);

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
                     L7_char8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType);

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
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                     L7_uint32 *count);

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
                     L7_uint32 *count);

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
L7_RC_t usmDbRadiusServerHostIPAddrGet(L7_uint32 unitIndex,L7_uchar8 *dnsAddr,
                     radiusServerType_t type, L7_uint32 *ipAddr);

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
                                 L7_uchar8 *serverName, L7_char8 *currServerHostName);                     
                     
#endif /* INCLUDE_USMDB_RADIUS_H */

