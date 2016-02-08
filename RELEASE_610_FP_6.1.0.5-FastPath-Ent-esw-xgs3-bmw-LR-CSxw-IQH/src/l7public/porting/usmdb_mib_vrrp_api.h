/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/l7public/porting/usmdb_mib_vrrp_api.h
*
* @purpose usmdb support for LVL7 extensions to rfc2787/2338 VRRP support
*                  
* @component usmdb
*
* @comments tba
*
* @create 10/26/2001
*
* @author mfiorito
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_MIB_VRRP_API_H
#define USMDB_MIB_VRRP_API_H

#include <l3_comm_structs.h>
#include "l7_common.h"
#include "l7_common_l3.h"
#include "l7_ip_api.h"
#include <l7_vrrp_api.h>
#include <string.h>
#include <osapi.h>

/*********************************************************************
* @purpose  To get the Virtual Mac addres of the virtual Router 
*
* @param    unitIndex    Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    vmac   pointer to vmac address.
*
* @returns  L7_SUCCESS - if router configuraion is present 
* @returns  L7_FAILURE - if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperVirtualMacAddrGet(L7_uint32 UnitIndex, 
                                              L7_uchar8 vrId, 
                                              L7_uint32 intIfNum, 
                                              L7_uchar8* vmac);

/*********************************************************************
* @purpose  To get the Number of configured IP address.
*
* @param    unitIndex    Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    count   Total number of ip configured for a virtual router
*
* @returns  L7_SUCCESS - if router configuraion is present 
* @returns  L7_FAILURE - if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperIpAddrCountGet(L7_uint32 unitIndex, 
                                           L7_uchar8 vrId, 
                                           L7_uint32 intIfNum, 
                                           L7_uchar8* count);

/*********************************************************************
* @purpose  To get Authentication  type
*
* @param    unitIndex    Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    authType  - returned value of the authentication type
*
* @returns  L7_SUCCESS - if router configuraion is present 
* @returns  L7_FAILURE - if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperAuthTypeGet(L7_uint32 unitIndex, 
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum, 
                                        L7_AUTH_TYPES_t * authType);

/*********************************************************************
* @purpose  To set Authentication  type
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    auttype      Auth type to be set
*
* @returns  L7_SUCCESS   if router configuraion is present 
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperAuthTypeSet(L7_uint32 unitIndex, 
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum, 
                                        L7_AUTH_TYPES_t authType);


/*********************************************************************
* @purpose  To get Authentication  key
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    authKey      returned value of Auth key
*
* @returns  L7_SUCCESS   if router configuraion is present 
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperAuthKeyGet(L7_uint32 unitIndex, 
                                       L7_uchar8 vrId, 
                                       L7_uint32 intIfNum, 
                                       L7_uchar8* authKey);
 
/*********************************************************************
* @purpose  To set Authentication  key
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum  Interface Number
* @param    authKey    Auth key to be set
*
* @returns  L7_SUCCESS   if router configuraion is present 
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperAuthKeySet(L7_uint32 unitIndex, 
                                       L7_uchar8 vrId, 
                                       L7_uint32 intIfNum, 
                                       L7_uchar8* authKey);

     
/*********************************************************************
* @purpose  To set the priority for the virtual router.
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    priority - priority to be set
*
* @returns  L7_SUCCESS  if router configuraion is present 
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperPrioritySet(L7_uint32 unitIndex,
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum, 
                                        L7_uchar8 priority );


/*********************************************************************
* @purpose  To get the priority of the virtual router.
*
* @param    unitIndex   Unit for this operation
* @param    vrId    Router Id. 
* @param    intIfNum   Interface Number
* @param    priority    returned value of priority 
*
* @returns  L7_SUCCESS  if router configuraion is present 
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperPriorityGet(L7_uint32 unitIndex, 
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum,
                                        L7_uchar8* priority);

/*********************************************************************
* @purpose  To get the priority of the virtual router.
*
* @param    unitIndex   Unit for this operation
* @param    vrId    Router Id.
* @param    intIfNum   Interface Number
* @param    priority    returned value of priority
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbVrrpConfigPriorityGet(L7_uint32 unitIndex,
                                          L7_uchar8 vrId,
                                          L7_uint32 intIfNum,                                                                                                           L7_uchar8* priority);

/*********************************************************************
* @purpose  To get the Advertisement Interval.
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    advIntvl     Advertisement interval
*
* @returns  L7_SUCCESS  if router configuraion is present 
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperAdvertisementIntervalGet(L7_uint32 unitIndex, 
                                                     L7_uchar8 vrId, 
                                                     L7_uint32 intIfNum,
                                                     L7_uchar8* advIntvl);

/*********************************************************************
* @purpose  To Set the Advertisement Interval.
*
* @param    unitIndex   Unit for this operation
* @param    vrId    Router Id. 
* @param    intIfNum   Interface Number
* @param    advIntvl    Advertisement interval to be set
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperAdvertisementIntervalSet(L7_uint32 unitIndex, 
                                                     L7_uchar8 vrId, 
                                                     L7_uint32 intIfNum,
                                                     L7_uchar8 advIntvl);

/*********************************************************************
* @purpose  To Get the Preempt mode
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum  Interface Number
* @param    prmtFlg    returned value of the preempt flag
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperPreemptModeGet(L7_uint32 unitIndex, 
                                           L7_uchar8 vrId, 
                                           L7_uint32 intIfNum,
                                           L7_BOOL* prmtFlg);

/*********************************************************************
* @purpose  To Set the Preempt mode on/off
*
* @param    unitIndex    Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    prmtFlg - Preempt flag
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVrrpOperPreemptModeSet(L7_uint32 unitIndex, 
                                           L7_uchar8 vrId, 
                                           L7_uint32 intIfNum,
                                           L7_BOOL prmtFlg);

/*********************************************************************
* @purpose  To set the Virtual Router Id and Interface Num in the configuration packet.
*
* @param    unitIndex    Unit for this operation
* @param    vrId    Router Id to be set. 
* @param    intIfNum   Interface Number to be set.
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpRouterIdCreate(L7_uint32 unitIndex, 
                                       L7_uchar8 vrId, 
                                       L7_uint32 intIfNum );           
                                  
/*********************************************************************
* @purpose  To delete the configuration
*
* @param    unitIndex    Unit for this operation
* @param    vrId    Router Id 
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpRouterIdDelete (L7_uint32 unitIndex, 
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum );
                                                                                                                               


/*********************************************************************
* @purpose  To Associate an ip address to the router
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    ipAddress    Ip address of the virtual router
*
* @returns  L7_SUCCESS   if router configuraion is present   
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpAssocIpAddress(L7_uint32 unitIndex, 
                                       L7_uchar8 vrId,
                                       L7_uint32 intIfNum, 
                                       L7_uint32 ipAddress);
                                                                                                                     

/*********************************************************************
* @purpose  To set the Virtual router mode
*
* @param    unitIndex    Unit for this operation
* @param    vrId        Router Id. 
* @param    intIfNum    Interface Number
* @param    state       Mode of the virtual router Up/Down
*
* @returns  L7_SUCCESS   if router configuraion is present   
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
 
extern L7_RC_t usmDbVrrpOperVrrpModeSet(L7_uint32 unitIndex, 
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum, 
                                        L7_vrrpStartState_t state);

/*********************************************************************
* @purpose  To get the Virtual router mode
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    state        returned value of the Mode 
*
* @returns  L7_SUCCESS   if router configuraion is present   
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpOperVrrpModeGet(L7_uint32 unitIndex, 
                                 L7_uchar8 vrId, 
                                 L7_uint32 intIfNum, 
                                 L7_vrrpStartState_t* state);



/*********************************************************************
* @purpose  To get the vrrp version supported by this node.
*
* @param    unitIndex  Unit for this operation
* @param    Version    Version of the vrrp supported
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @notes    
*
 @end
*********************************************************************/
extern L7_RC_t usmDbVrrpNodeVersionGet(L7_uint32 unitIndex,
                                       L7_uint32* version); 


/*********************************************************************
* @purpose  To get the status wheather VRRP-enabled router will generate 
            SNMP traps for events defined in this MIB
*
* @param    unitIndex  Unit for this operation
* @param    Mode -     SNMP TRAP Flag
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVrrpNotificationCntlGet(L7_uint32 unitIndex, 
                                            L7_uchar8* Mode); 
                                                                     

/*********************************************************************
* @purpose  To set the status wheather VRRP-enabled router will generate 
            SNMP traps for events defined in this MIB
*
* @param    unitIndex  Unit for this operation
* @param    Mode -     SNMP TRAP Flag
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVrrpNotificationCntlSet(L7_uint32 unitIndex, 
                                            L7_uchar8 Mode); 


             
/*********************************************************************
* @purpose  To Get the state of router(Master/backup or initialised)
*
* @param    unitIndex   Unit for this operation
* @param    vrId    Router Id. 
* @param    intIfNum   Interface Number
* @param    state-      State of the router(Out param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperStateGet(L7_uint32 unitIndex, 
                              L7_uchar8 vrId, 
                              L7_uint32 intIfNum, 
                              L7_vrrpState_t* state); 
                                                                       
/*********************************************************************
* @purpose  To Get the Ip address of virtual router
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    ipAddress  virtual IpAddress(Out param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperIpAddressGet(L7_uint32 unitIndex, 
                                  L7_uchar8 vrId, 
                                  L7_uint32 intIfNum,
                                  L7_uint32* ipAddress ); 

/*********************************************************************
* @purpose  To Get the real IP address of the master router
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    ipAddress  Master router's IP Address(Out param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperMasterIpAddressGet(L7_uint32 unitIndex, 
                                        L7_uchar8 vrId, 
                                        L7_uint32 intIfNum,
                                        L7_uint32* ipAddress );

/*********************************************************************
* @purpose  To Get the up time of the virtual Router
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    uptime - up time of the router (Out param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperUpTimeGet(L7_uint32 unitIndex, 
                               L7_uchar8 vrId, 
                               L7_uint32 intIfNum, 
                               L7_uint32* uptime);


/*********************************************************************
* @purpose  To Get the The particular protocol being controlled by the
         VRRP Router
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    protocol - protocol type
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperProtocolGet(L7_uint32 unitIndex, 
                                 L7_uchar8 vrId, 
                                 L7_uint32 intIfNum, 
                                 L7_vrrpProtocolType_t* protocol); 

/*********************************************************************
* @purpose  To Set the The particular protocol being controlled by the
         VRRP Router
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    protocol - protocol type
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperProtocolSet(L7_uint32 unitIndex, 
                                 L7_uchar8 vrId, 
                                 L7_uint32 intIfNum, 
                                 L7_vrrpProtocolType_t protocol); 

/*********************************************************************
* @purpose  To Get the the number of packet received with invalid checksum
*
* @param    unitIndex  Unit for this operation
* @param    checkSumError - Checksum error(Out Param)
*
* @returns  L7_SUCCESS     
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpRouterChecksumErrorGet(L7_uint32 unitIndex, 
                                      L7_uint32* checkSumError); 
 

/*********************************************************************
* @purpose  To Get the the number of VRRP packet received with wrong 
*           vrrp version
*
* @param    unitIndex  Unit for this operation
* @param    versionError - version error(Out Param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpRouterVerErrorGet(L7_uint32 unitIndex, 
                                       L7_uint32* versionError); 

/*********************************************************************
* @purpose  To Get the the number of packet received with wrong vrrpid
*
* @param    unitIndex  Unit for this operation
* @param    vrIdError - version Id Errors (Out Param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpRouterVridErrorGet(L7_uint32 unitIndex, 
                                  L7_uint32* vrIdError); 


/*********************************************************************
* @purpose  To Get the the number of time state change to master
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    masterState  master state transfer (Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpStatsBecomeMaster(L7_uint32 unitIndex, 
                                   L7_uchar8 vrId, 
                                   L7_uint32 intIfNum, 
                                   L7_uint32* masterState); 
 

/*********************************************************************
* @purpose  To Get the the number of vrrp advertisement received 
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    addReceived  Number of add received (Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpStatsAdvertiseRcvd(L7_uint32 unitIndex, 
                                    L7_uchar8 vrId, 
                                    L7_uint32 intIfNum, 
                                    L7_uint32* addReceived); 




/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with wrong Advert
*           interval
*
* @param    unitIndex    Unit for this operation
* @param    vrId     Router Id. 
* @param    intIfNum    Interface Number
* @param    addIntErrors  Number of wrong interval pkt(Out Param)
*
* @returns  L7_SUCCESS   if router configuraion is present   
* @returns  L7_FAILURE   if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsAdvertiseIntervalErrorsGet(L7_uint32 unitIndex, 
                                                 L7_uchar8 vrId, 
                                                 L7_uint32 intIfNum, 
                                                 L7_uint32* addIntErrors);  


/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received which fails the 
*           authentication criteria
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    authFailedPkt - Number of auth failed pkt(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsAuthFailuresGet(L7_uint32 unitIndex, 
                                      L7_uchar8 vrId, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32* authFailedPkt);  


/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with IP TTL not 
*           equal to 255
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    ttlFailedPkt - Number of wrong ttl pkt received pkt(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsIpTTLFailedGet(L7_uint32 unitIndex, 
                                     L7_uchar8 vrId, 
                                     L7_uint32 intIfNum, 
                                     L7_uint32* ttlFailedPkt);



/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with 0 priority
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    zeroPriorRcvPkt - number of zero priority pkt received(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsPriorityZeroPktsRcvdGet(L7_uint32 unitIndex, 
                                              L7_uchar8 vrId, 
                                              L7_uint32 intIfNum, 
                                              L7_uint32* zeroPriorRcvPkt);  
/*********************************************************************
* @purpose  To Get the the number of vrrp pkt sent with 0 priority
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    zeroPriorSentPkt - number of zero priority pkt sent(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsPriorityZeroPktsSentGet(L7_uint32 unitIndex, 
                                              L7_uchar8 vrId, 
                                              L7_uint32 intIfNum, 
                                              L7_uint32* zeroPriorSentPkt);  

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an 
*           invalid value in the type field
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    invalidTypePkt - number of invalid type pkt received(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsInvalidTypePktsRcvdGet(L7_uint32 unitIndex, 
                                             L7_uchar8 vrId, 
                                             L7_uint32 intIfNum, 
                                             L7_uint32* invalidTypePkt);  

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an 
*           invalid Ip address.
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    addListerrPkt - number of invalid pkt received 
*           with invalid ip address(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsAddressListErrorsGet(L7_uint32 unitIndex, 
                                           L7_uchar8 vrId, 
                                           L7_uint32 intIfNum, 
                                           L7_uint32* addListerrPkt);  
 
/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an 
*           invalid Authentication type.
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    invAuthTypePkt - number of invalid pkt received 
*           with an invalid Authentication type(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsInvalidAuthTypeGet(L7_uint32 unitIndex, 
                                         L7_uchar8 vrId, 
                                         L7_uint32 intIfNum, 
                                         L7_uint32* invAuthTypePkt);  

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with mismatched
*           Authentication data.
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    authMismatchPkt - number of invalid pkt received 
*           with mismatched Authentication data(Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsAuthTypeMismatchGet(L7_uint32 unitIndex, 
                                          L7_uchar8 vrId, 
                                          L7_uint32 intIfNum, 
                                          L7_uint32* authMismatchPkt);  


/*********************************************************************
* @purpose  To Get the the number of invalid vrrp pkt received 
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    pktError - number of invalid pkt received (Out Param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpStatsPktLengthGet(L7_uint32 unitIndex, 
                                   L7_uchar8 vrId, 
                                   L7_uint32 intIfNum, 
                                   L7_uint32* pktError);  


/*********************************************************************
* @purpose  To get the admin mode enable/disable
*
* @param    unitIndex  Unit for this operation
* @param    mode - Admin mode to be set
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t usmDbVrrpOperAdminStateGet(L7_uint32 unitIndex,L7_BOOL* mode);


/*********************************************************************
* @purpose  To Set the admin mode enable/disable
*
* @param    unitIndex  Unit for this operation
* @param    mode - Admin mode to be set
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperAdminStateSet(L7_uint32 unitIndex, L7_BOOL mode);

/*********************************************************************
* @purpose  To get the router id and intif num of the first vrrp 
*           configuration from the list.
*
* @param    unitIndex  Unit for this operation
* @param    vrId  Virtual router id (out param)
* @param    intIfNum int interface number (out param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperFirstGet(L7_uint32 unitIndex,L7_uchar8* vrId, 
                              L7_uint32* intIfNum);

                                 
/*********************************************************************
* @purpose  To get the router id and intif num of the next vrrp 
*           configuration from the list.
*
* @param    unitIndex  Unit for this operation
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVrrpOperNextGet(L7_uint32 unitIndex,L7_uchar8 vrId, 
                                    L7_uint32 intIfNum,
                                    L7_uchar8* nextVrId, 
                                    L7_uint32* nextInfNum);


/*********************************************************************
* @purpose  To get the total number of virtual router configuration
*
* @param    unitIndex  Unit for this operation
* @param    count   returned value of the total configured
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/

extern L7_RC_t usmDbVrrpOperConfigCountGet(L7_uint32 unitIndex, 
                                           L7_uint32* count);
                                           


/*********************************************************************
* @purpose  To Check if configuration is present corresponding to 
*           a given vrid and intnum
*
* @param    unitIndex  Unit for this operation
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/

extern L7_RC_t usmDbVrrpOperEntryGet(L7_uint32 unitIndex,
                                     L7_uchar8 vrId, L7_uint32 intIfNum);
                                     

/*********************************************************************
* @purpose  To get the logical next entry from the configuration list
*
* @param    unitIndex  Unit for this operation
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVrrpOperEntryNextGet(L7_uint32 unitIndex,
                                         L7_uchar8 vrId, 
                                         L7_uint32 intIfNum,
                                         L7_uchar8* nextVrId, 
                                         L7_uint32* nextInfNum);

/*********************************************************************
* @purpose  To Check if configuration is present corresponding to 
*           a given vrid, intnum and ipaddress.
*
* @param    unitIndex  Unit for this operation
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @param    ipAddress ip address of the configuration.
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpAssoIpAddrEntryGet(L7_uint32 UnitIndex, 
                                    L7_uchar8 vrId, 
                                    L7_uint32 intIfNum, 
                                    L7_uint32 ipAddress);


/*********************************************************************
* @purpose  To get the next logical entry corresponding to a given vrid,
*           infnum and ipaddress.
*
* @param    unitIndex  Unit for this operation
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @param    ipAddress ip address of the configuration.
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpAssoIpAddrEntryNextGet(L7_uint32 UnitIndex, 
                                        L7_uchar8 vrId,L7_uint32 intIfNum, 
                                        L7_uint32 ipAddress,L7_uchar8 *vrIdNext, 
                                        L7_uint32 *intIfNumNext, 
                                        L7_uint32 *ipAddressNext);




/*********************************************************************
* @purpose  To get the Vmac address associated with an IP
*
* @param    unitIndex  Unit for this operation
* @param    ipAddress  Ip address 
* @param    vMac   Vmac address ( Out param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes   The function returns the vmac if router is in master state 
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpVMacGet(L7_uint32 UnitIndex, L7_uint32 ipAddress, 
                         L7_uchar8* vMac);

/*********************************************************************
* @purpose  To get the ip address associated with the VMAC.
*
* @param    unitIndex  Unit for this operation
* @param    vMac  Virtual Mac address 
* @param    ipAddress  returned value of ip address
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpIpGet(L7_uint32 UnitIndex, L7_uchar8* vMac,
                       L7_uint32 *ipAddress);


 
/*********************************************************************
* @purpose  To get the 0 priority advert rcvd flag
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    zeroFlg   zerp priority advert rcvd flag
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpZeroPriorityFlgGet(L7_uint32 unitIndex, 
                                    L7_uchar8 vrId,
                                    L7_uint32 intIfNum,
                                    L7_BOOL *zeroFlg);

/*********************************************************************
* @purpose  To set the 0 priority advert rcvd flag
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    zeroFlg   zerp priority advert rcvd flag
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpZeroPriorityFlgSet(L7_uint32 unitIndex, 
                                    L7_uchar8 vrId,
                                    L7_uint32 intIfNum,
                                    L7_BOOL zeroFlg);


/*********************************************************************
* @purpose  To get the row status
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    rowStatus  row status 
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperRowStatusGet(L7_uint32  unitIndex, 
                                  L7_uchar8  vrId,
                                  L7_uint32  intIfNum,
                                  L7_uint32* rowStatus);

/*********************************************************************
* @purpose  To get the row status of Associate Ip address.
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    assocIpRowStat  returned value of ip row status
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpAssocIpRowStatusGet(L7_uint32 unitIndex, 
                                    L7_uchar8 vrId,
                                    L7_uint32 intIfNum,
                                    L7_uint32* assocIpRowStat);

/*********************************************************************
* @purpose  To Set the row status of Associate Ip address.
*
* @param    unitIndex  Unit for this operation
* @param    vrId   Router Id. 
* @param    intIfNum   Interface Number
* @param    assocIpRowStat   ip row status value to be set
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpAssocIpRowStatusSet(L7_uint32 unitIndex, 
                                    L7_uchar8 vrId,
                                    L7_uint32 intIfNum,
                                    L7_uint32 assocIpRowStat);

/*********************************************************************
* @purpose  To convert the ip address from network format to int.
*
* @param    ip   Ip address in byte[4] format 
* @param    ipIntForm   returned value of ip address in uint32 format.
*
* @returns  L7_SUCCESS  if router configuraion is present   
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDBVrrpNtohl(L7_uint32 unitIndex, L7_uchar8 *ip, L7_uint32* ipIntForm);     

/*********************************************************************
*
* @purpose Get the VRRP traps configuration mode.
*          
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 *mode      returned value (L7_ENABLE or L7_DISABLE)
* @param L7_uint32 trapType   VRRP trap type specified
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid VRRP Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapVrrpSwGet(L7_uint32 UnitIndex, L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose Set the VRRP related traps configuration mode.
*          
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 mode       trap status (L7_ENABLE or L7_DISABLE)
* @param L7_uint32 trapType   VRRP trap type specified
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapVrrpSwSet(L7_uint32 UnitIndex, L7_uint32 mode, L7_uint32 trapType);


/*********************************************************************
* @purpose  To add a secondary IP address to the virtual router
*
* @param    UnitIndex - the unit for this operation
* @param    routerID  - router Id
* @param    intIfNum  - Interface Number
* @param    ipAddress - Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpAssocSecondaryIpAddress(L7_uint32 UnitIndex,
                              L7_uchar8 routerID,
                              L7_uint32 intIfNum,
                              L7_uint32 ipAddress);

/*********************************************************************
* @purpose  To remove a secondary IP address from the virtual router
*
* @param    UnitIndex - the unit for this operation
* @param    routerID  - router Id
* @param    intIfNum  - Interface Number
* @param    ipAddress - Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpDissocSecondaryIpAddress(L7_uint32 UnitIndex,
                                          L7_uchar8 routerID,
                                          L7_uint32 intIfNum,
                                          L7_uint32 ipAddress);

/*********************************************************************
* @purpose  To get the next configured Virtual Router IP address
*
* @param    UnitIndex - the unit for this operation
* @param    routerID  - router Id
* @param    intIfNum  - Interface Number
* @param    ipAddress  -Ip address
* @param    pIpAddress - Next Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Function returns primary address (First)- if 0 passed as
*           a
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpIpAddressNextGet(L7_uint32 UnitIndex, L7_uchar8 routerID, 
                                  L7_uint32 intIfNum, L7_uint32 ipAddress, 
                                  L7_uint32 *pIpAddress);

/*********************************************************************
* @purpose  Check if an interface is valid for VRRP.
*
* @param    UnitIndex - the unit for this operation
* @param    intIfNum  - Interface Number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbVrrpIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Add IP interface into the VRRP Tracking Interfaces List
*
* @param    unitIndex @b{input}  Unit for this operation
* @param    vrId  @b{input} Router Id.
* @param    intIfNum  @b{input} Interface Number
* @param    trackIntIfNum  @b{input} Interface Number to be tracked
* @param    prio_dec @b{input} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackIntfAdd(L7_uint32 unitIndex, L7_uchar8 vrId,
                                  L7_uint32 intIfNum,  L7_uint32 trackIntIfNum,
                                  L7_uchar8 prio_dec);

/*********************************************************************
* @purpose  Get the Tracking Interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input}  Interface Number to be tracked
* @param    prio_dec @b{output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackIntfPrioGet(L7_uint32 unitIndex, L7_uchar8 vrId,
                                      L7_uint32 intIfNum,  L7_uint32 trackIntIfNum,
                                      L7_uchar8 *prio_dec );

/*********************************************************************
* @purpose  Get the tracking Interface state.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number.
* @param    trackIntIfNum @b{input}  Interface Number to be tracked.
* @param    state @b{output} IP state of the Tracked interface.
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackIntfStateGet(L7_uint32 unitIndex, L7_uchar8 vrId,
                                       L7_uint32 intIfNum, L7_uint32 trackIntIfNum,
                                       L7_BOOL *state );

/*********************************************************************
* @purpose  Get Next Tracking Interface.
*
* @param    vrId @b{input/output} Router Id.
* @param    intIfNum @b{inpu/outputt} Interface Number
* @param    trackIntIfNum @b{input/output}  Interface Number to be tracked
* @param    prio_dec @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackIntfNextGet(L7_uint32 unitIndex,L7_uchar8 *vrId,
                                      L7_uint32 *intIfNum,L7_uint32 * trackIntIfNum);

/*********************************************************************
* @purpose  Get Next Tracking Interface for the given vrid and interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input/output}  Interface Number to be tracked
* @param    prio_dec @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpVrIdIntfTrackIntfNextGet(L7_uint32 unitIndex, L7_uchar8 vrId, L7_uint32 intIfNum,
                                          L7_uint32 * trackIntIfNum, L7_uchar8 * prio_dec );

/*********************************************************************
* @purpose  Get the Tracking Interface for the given vrid and interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input}  Interface Number to be tracked
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpVrIdIntfTrackIntfGet(L7_uint32 unitIndex, L7_uchar8 vrId, L7_uint32 intIfNum,
                                              L7_uint32  trackIntIfNum);
/*********************************************************************
* @purpose  Get the Tracking route.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    ipAddr @b{input}  IP address of tracked route.
* @param    netMask @b{input} Subnet mask
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpVrIdIntfTrackRouteGet(L7_uint32 unitIndex, L7_uchar8 vrId,
                                       L7_uint32 intIfNum,L7_uint32 ipAddr,
                                       L7_uint32 netMask );

/*********************************************************************
* @purpose  Get Next Tracking Route.
*
* @param    vrId @b{input/output} Router Id.
* @param    intIfNum @b{inpu/outputt} Interface Number
* @param    ipAddr @b{input/output}  ipAddress of the tracked route 
* @param    mask @b{input/output} mask 
* @param    prio_dec @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackRouteNextGet(L7_uint32 unitIndex, L7_uchar8 *vrId, 
                                       L7_uint32 *intIfNum, L7_uint32 * ipAddr, 
                                       L7_uint32 * mask);

/*********************************************************************
* @purpose  Get Next Tracking Route for given Vrid and interface
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{inpu} Interface Number
* @param    ipAddr @b{input/output}  ipAddress of the tracked route
* @param    mask @b{input/output} mask
* @param    prio_dec @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpVridIntfTrackRouteNextGet(L7_uint32 unitIndex, L7_uchar8 vrId,
                                           L7_uint32 intIfNum, L7_uint32 * ipAddr,
                                           L7_uint32 * mask, L7_uchar8 * prio_dec );

/*********************************************************************
* @purpose  Delete IP interface from the VRRP Tracking interfaces list
*
* @param    unitIndex @b{input} Unit for this operation
* @param    vrId  @b{input} Router Id.
* @param    intIfNum  @b{input} Interface Number
* @param    trackIntIfNum @b{input}  Interface Number to be tracked
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackIntfDelete(L7_uint32 unitIndex, L7_uchar8 vrId,
                                     L7_uint32 intIfNum,  L7_uint32 trackIntIfNum);

/*********************************************************************
* @purpose  Add IP route into the VRRP Tracking route list
*
* @param    unitIndex @b{input} Unit for this operation
* @param    vrId     @b{input}  Router Id.
* @param    intIfNum  @b{input} Interface Number
* @param    netAddr  @b{input} Prefix of the route to be tracked
* @param    mask    @b{input} Mask of the route to be tracked
* @param    prio_dec  @b{input} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackRouteAdd(L7_uint32 unitIndex, L7_uchar8 vrId,
                                   L7_uint32 intIfNum, L7_uint32 netAddr,
                                   L7_uint32 mask, L7_uchar8 prio_dec);

/*********************************************************************
* @purpose  Delete IP route from the VRRP Tracking routes list
*
* @param    unitIndex @b{input} Unit for this operation
* @param    vrId  @b{input} Router Id.
* @param    intIfNum @b{input}  Interface Number
* @param    netAddr  @b{input} Prefix of the tracking route 
* @param    mask    @b{input} Mask of the tracking route 
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackRouteDelete(L7_uint32 unitIndex, L7_uchar8 vrId,
                                      L7_uint32 intIfNum, L7_uint32 netAddr, 
                                      L7_uint32 mask);

/*********************************************************************
* @purpose  Get the Tracking Route priority.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    ipAddr @b{input}  ip address of tracked route.
* @param    netMask @b{input} subnet mask
* @param    prio_dec @b{output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackRoutePrioGet(L7_uint32 unitIndex,
                                       L7_uchar8 vrId, L7_uint32 intIfNum,
                                       L7_uint32 ipAddr,L7_uint32 netMask,
                                       L7_uchar8 *prio_dec );

/*********************************************************************
* @purpose  Get the Tracking Route reachability
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    ipAddr @b{input}  IP address of tracked route.
* @param    netMask @b{input} Subnet mask
* @param    reachable @b{output} Reachability of the tracking route.
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbVrrpOperTrackRouteReachabilityGet(L7_uint32 unitIndex,L7_uchar8 vrId,
                                               L7_uint32 intIfNum,
                                               L7_uint32 ipAddr,L7_uint32 netMask,
                                               L7_BOOL *reachable );

/*********************************************************************
* @purpose  Turns on/off the displaying of vrrp packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpVrrpPacketDebugTraceFlagSet(L7_BOOL flag);

/*********************************************************************
* @purpose  Get the current status of displaying vrrp packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbIpVrrpPacketDebugTraceFlagGet();

#endif /* USMDB_MIB_VRRP_API_H */
