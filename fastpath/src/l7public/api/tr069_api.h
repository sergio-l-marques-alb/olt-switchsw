/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   tr069_api.h
*
* @purpose    Contains prototypes to APIs used by USMDB and tr069 
*             component
*
* @component  tr069
*
* @comments   none
*
* @create     10-Apr-2008
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#ifndef TR069_API_H
#define TR069_API_H

#define TR069_MAX_RPC_METHODS 20               
/*********************************************************************
* @purpose  Get the TR-069 ACS URL
*
* @param    urlString    @b{(output)}  ACS HTTP/HTTPS URL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSUrlGet(L7_uchar8 *urlString);
/*********************************************************************
* @purpose  Set the TR-069 ACS URL
*
* @param    urlString    @b{(input)}  ACS HTTP/HTTPS URL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSUrlSet(L7_uchar8 *urlString);
/*********************************************************************
* @purpose  Get the TR-069 ACS user name used by CPE to authenticate
*           with the ACS
*
* @param    uname    @b{(output)}  Username
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSUsernameGet(L7_uchar8 *uname);
/*********************************************************************
* @purpose  Set the TR-069 ACS user name used by CPE to authenticate
*           with the ACS
*
* @param    uname    @b{(input)}  Username
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSUsernameSet(L7_uchar8 *uname);
/*********************************************************************
* @purpose  Get the TR-069 ACS user password used by CPE to authenticate
*           with the ACS
*
* @param    passwd    @b{(output)}  Authentication password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments As per Always return a null string of length 
*           L7_TR069_ACS_PASSWD_LEN
*
* @end
*********************************************************************/
L7_RC_t tr069ACSPasswordGet(L7_uchar8 *passwd);
/*********************************************************************
* @purpose  Set the TR-069 ACS user password used by CPE to authenticate
*           with the ACS
*
* @param    passwd    @b{(input)}  Authentication password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments As per Always return a null string of length
*           L7_TR069_ACS_PASSWD_LEN
*
* @end
*********************************************************************/
L7_RC_t tr069ACSPasswordSet(L7_uchar8 *passwd);
/*********************************************************************
* @purpose  Get the configured periodic inform mode
*
* @param    mode    @b{(output)}  Inform Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t tr069PeriodicInformModeGet(L7_uint32 *mode);
/*********************************************************************
* @purpose  Set the periodic inform mode
*
* @param    mode    @b{(input)}  Inform Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069PeriodicInformModeSet(L7_uint32 mode);
/*********************************************************************
* @purpose  Get the configured periodic inform interval
*
* @param    interval    @b{(output)}  Inform interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069PeriodicInformIntervalGet(L7_uint32 *interval);
/*********************************************************************
* @purpose  Set the periodic inform interval
*
* @param    interval    @b{(input)}  Inform interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069PeriodicInformIntervalSet(L7_uint32 interval);
/*********************************************************************
* @purpose  Get the configured periodic inform time
*
* @param    interval    @b{(output)}  Inform time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The format is 0000-00-00T00:00:00
*
* @end
*********************************************************************/
L7_RC_t tr069PeriodicInformTimeGet(L7_uchar8 *utcTimeString);
/*********************************************************************
* @purpose  Set the configured periodic inform time
*
* @param    interval    @b{(input)}  Inform time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The format is 0000-00-00T00:00:00
*
* @end
*********************************************************************/
L7_RC_t tr069PeriodicInformTimeSet (L7_uchar8 *utcTimeString);
/*********************************************************************
* @purpose  Get the TR-069 CPE user name used by ACS to authenticate
*           with the CPE
*
* @param    uname    @b{(output)}  Username
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ConnRequestUsernameGet(L7_uchar8 *uname);
/*********************************************************************
* @purpose  Set the TR-069 CPE user name used by ACS to authenticate
*           with the CPE
*
* @param    uname    @b{(input)}  Username
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ConnRequestUsernameSet (L7_uchar8 *uname);
/*********************************************************************
* @purpose  Get the TR-069 CPE user password used by ACS to authenticate
*           with the CPE
*
* @param    passwd    @b{(output)}  Authentication password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments As per Always return a null string of length
*           L7_TR069_ACS_PASSWD_LEN
*
* @end
*********************************************************************/
L7_RC_t tr069ConnRequestPasswordGet(L7_uchar8 *passwd);
/*********************************************************************
* @purpose  Set the TR-069 CPE user password used by ACS to authenticate
*           with the CPE
*
* @param    passwd    @b{(input)}  Authentication password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t tr069ConnRequestPasswordSet (L7_uchar8 *passwd);
/*********************************************************************
* @purpose  Get the configured periodic inform mode
*
* @param    upgradesFlag    @b{(input)}  upgrades mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSUpgradesManagedGet(L7_BOOL *upgradesFlag);
/*********************************************************************
* @purpose  Set the upgrades managed mode
*
* @param    upgradesFlag    @b{(input)}  upgrades mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSUpgradesManagedSet (L7_BOOL upgradesFlag);
/*********************************************************************
* @purpose  Get the parameter key value associated with the latest
*           RPC request
*
* @param    paramKeyString    @b{(output)}  Parameter Key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ParameterKeyGet(L7_uchar8 *paramKeyString);
/*********************************************************************
* @purpose  Get the Connection request HTTP url for the CPE
*
* @param    urlString    @b{(output)}  HTTP URL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ConnRequestURLGet(L7_uchar8 *urlString);
/*********************************************************************
* @purpose  Get TR-069 statistics
*
* @param    statsCode    @b{(input)}  Stats code defined by the 
*                                     tr069_stats_t
* @param    statsVal     @b{(output)} Requested value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   Invalid stat id 
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069StatsGet(L7_uint32 statsCode, L7_uint32 *statsVal);
/*********************************************************************
* @purpose  Resets TR-069 statistics
*
* @param    statsCode    @b{(input)}  Stats code defined by the 
*                                     L7_TR069C_STATS_TYPE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   Invalid stat id 
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069StatsReset(L7_uint32 statsCode);
/*********************************************************************
* @purpose  Send diagnostics complete call back to vendor task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069PingDiagCompleteCallback ();
/*********************************************************************
* @purpose  Executes the RPCMethodsGet RPC
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069ACSRPCMethodsGet ();
/*********************************************************************
* @purpose  Posts message to vendor code to specify the event of
*           change in wanIP address
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069WANAddrChanged (L7_COMPONENT_IDS_t registrar_ID, L7_uint32 addrType,
                             L7_uchar8 addrFamily);
/*********************************************************************
* @purpose  Posts message to vendor code to load the SSL certificates
*           to SSL context
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069SSLCertsLoad ();
/*********************************************************************
* @purpose  Obtain SSL Certs status of TR-069 client
*
* @param    serverCACert  @b{(output)} 1 if server CA cert is loaded
* @param    clientCert    @b{(output)} 1 if Client cert is loaded
* @param    clientPrivKey @b{(output)} 1 if Client Private Key is loaded
*
* @returns  L7_SUCCESS  Success
* @returns  L7_FAILURE  Failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t tr069SSLCertsStatusGet(L7_uint32 *serverCACert, L7_uint32 *clientCert,
                               L7_uint32 *clientPrivKey);
/*********************************************************************
* @purpose  Verify if it is a valid URL string
*
* @param    urlString    @b{(output)}  ACS HTTP/HTTPS URL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t tr069UrlIsValid(L7_uchar8 *urlString);
/*********************************************************************
* @purpose  Get the default password for the device
*
* @param    passString    @b{(output)} Password string. String should
*                         be atleast 256 Bytes in size
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The default password is the base MAC address used by the DUT
*           as source MAC address to communicate with the ACS
*
* @end
*********************************************************************/
L7_RC_t tr069DefaultPasswordGet(L7_uchar8 *passString);
#endif /* TR069_API_H */
