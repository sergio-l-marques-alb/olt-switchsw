/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_vrrp_api.h
*
* @purpose VRRP header file
*
* @component VRRP
*
* @comments
*
* @create  
*
* @author  
* @end
*
**********************************************************************/
#ifndef _L7_VRRP_API_H_
#define _L7_VRRP_API_H_

#include "l3_comm_structs.h"
#include "comm_structs.h"

#define VRRP_AUTH_DATA_LEN     8    

/* Below are the define statements for various Authentication type failures, which will be used 
   in logging the Authentication Failure trap */

#define VRRP_TRAP_INVALID_AUTH_TYPE   1

#define VRRP_TRAP_AUTH_TYPE_MISMATCH  2

#define VRRP_TRAP_AUTH_FAILURE        3

typedef  enum 
{
  L7_VRRP_STATE_INIT =1,                     /*Initialise state*/   
  L7_VRRP_STATE_MASTER,                      /*Master state*/
  L7_VRRP_STATE_BACKUP                       /*Backup state*/
} L7_vrrpState_t;  

typedef  enum 
{
  L7_VRRP_UP =1,                     /*Admin state is UP */   
  L7_VRRP_DOWN                       /*Admin state is DOWN */
} L7_vrrpStartState_t;

typedef  enum 
{
  L7_VRRP_IP =1,                     /*Protocol used is IP */   
  L7_VRRP_BRIDGE,
  L7_VRRP_DECNET,
  L7_VRRP_OTHER
} L7_vrrpProtocolType_t;

typedef  enum                /* Row Status for conceptual rows of vrrpOperTable */
{
  L7_VRRP_ACTIVE =1,                   
  L7_VRRP_NOTINSERVICE,
  L7_VRRP_NOTREADY,
  L7_VRRP_CREATEANDGO,
  L7_VRRP_CREATEANDWAIT,
  L7_VRRP_DESTROY
} L7_vrrpOperRowStatus_t;

/************************************************************************
* VRRP funtion table that contains the APIs that are available for the
* external components when OSPF component exists.
************************************************************************/
typedef struct vrrpFuncTable_s
{
  L7_BOOL (*L7_vrrpIsActive)(L7_uint32 intIfNum, L7_uchar8 *vrrpId);
  L7_RC_t (*vrrpMapGetVMac)(L7_uchar8 *ipAddress, L7_uchar8 *vmac, L7_uchar8 *vrId);
  L7_RC_t (*L7_vrrpAdminModeGet)(L7_BOOL *adminMode);
  L7_RC_t (*L7_vrrpOperVirtualMacAddrGet)(L7_uchar8 routerID, 
                                          L7_uint32 intIfNum, 
                                          L7_uchar8* vmac);
  L7_RC_t (*L7_vrrpIpGet)(L7_uchar8* vMac,L7_uint32 *ipAddress);
  void    (*L7_vrrpMasterGratARPSend)(void);
  L7_RC_t (*L7_vrrpStateGet)(L7_uchar8 routerID, L7_uint32 intIfNum, L7_vrrpState_t* state);
} vrrpFuncTable_t;

extern vrrpFuncTable_t vrrpFuncTable;

/*---------------------------------------------------------------------
 *                        API FUNCTIONS  -  INIT
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Function to add the virtual router structure in the list.
*
* @param    vrid - Virtual router Id
* @param    intIfNum - Interface Number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpVirtualRouterConfigAdd(L7_uchar8 vrid, L7_uint32 intifnum);


/*********************************************************************
* @purpose  Function to remove the virtual router structure from the list.
*			This remove the structure by matching with vrID
*
* @param    vrid - Virtual router Id
* @param    intIfNum - Interface Number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpVirtualRouterConfigDelete(L7_uchar8 vrid, L7_uint32 intifnum );

/*********************************************************************
* @purpose  Function to get the VRRP priority
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    priority - pointer to priority (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperPriorityGet(L7_uchar8 routerID, 
                               L7_uint32 intIfNum, 
                               L7_uchar8* priority);

/*********************************************************************
* @purpose  Function to get the VRRP priority
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    priority - pointer to priority (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpConfigPriorityGet(L7_uchar8 routerID,
                                 L7_uint32 intIfNum,
                                 L7_uchar8* priority);

/*********************************************************************
* @purpose  Configure the priority for a specific virtual router on a given interface.
*
* @param    vrid     - virtual router ID
* @param    intIfNum - internal interface number where virtual router operates
* @param    priority - priority 
*
* @returns  L7_SUCCESS 
*           L7_REQUEST_DENIED if priority is out of range
*           L7_FAILURE if an error occurs
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpConfigPrioritySet(L7_uchar8 vrid,
                              L7_uint32 intIfNum,
                              L7_uchar8 priority);

/*********************************************************************
* @purpose  To get the Virtual Mac addres of the virtual Router 
*
* @param    routerID   Router Id. 
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
L7_RC_t L7_vrrpOperVirtualMacAddrGet(L7_uchar8 routerID, 
                                     L7_uint32 intIfNum, 
                                     L7_uchar8* vmac);

/*********************************************************************
* @purpose  Function to get the Ip address count
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    count - pointer to count (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperIpAddrCountGet(L7_uchar8 routerID, 
                                  L7_uint32 intIfNum, 
                                  L7_uchar8* count);

/*********************************************************************
* @purpose  Function to get the Auth type
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authType - returned value of authType 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthTypeGet(L7_uchar8 routerID, 
                               L7_uint32 intIfNum, 
                               L7_AUTH_TYPES_t * authType);

/*********************************************************************
* @purpose  Function to set the Auth type
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authType - pointer to authType
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthTypeSet(L7_uchar8 routerID, 
                               L7_uint32 intIfNum, 
                               L7_AUTH_TYPES_t  authType);

/*********************************************************************
* @purpose  Function to get the Auth Key
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authKey - authKey pointer (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthKeyGet(L7_uchar8 routerID, 
                              L7_uint32 intIfNum, 
                              L7_uchar8* authKey);

/*********************************************************************
* @purpose  Function to Set the Auth Key
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authKey - authKey pointer 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthKeySet(L7_uchar8 routerID, 
                              L7_uint32 intIfNum, 
                              L7_uchar8* authKey);

/*********************************************************************
* @purpose  Function to Get the Advertisemnet Interval
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    advIntvl - advInterval  pointer (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAdvertisementIntervalGet(L7_uchar8 routerID, 
                                            L7_uint32 intIfNum,
                                            L7_uchar8* advIntvl);

/*********************************************************************
* @purpose  Function to Set the Advertisemnet Interval
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    advIntvl - advInterval  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAdvertisementIntervalSet(L7_uchar8 routerID, 
                                            L7_uint32 intIfNum,
                                            L7_uchar8 advIntvl);
/*********************************************************************
* @purpose  Function to Get the Preempt Mode
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    prmtFlg - pointer to preempt mode
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperPreemptModeGet(L7_uchar8 routerID, 
                                  L7_uint32 intIfNum,
                                  L7_BOOL* prmtFlg);

/*********************************************************************
* @purpose  Function to Set the Preempt Mode
*
* @param    routerID -  router Id
* @param    intIfNum - Interface Number
* @param    prmtFlg -   preempt mode
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperPreemptModeSet(L7_uchar8 routerID, 
                                  L7_uint32 intIfNum,
                                  L7_BOOL prmtFlg);

/*********************************************************************
* @purpose  Function to assigned the IP Address to the virtual router
*
* @param    routerID -  router Id
* @param    intIfNum - Interface Number
* @param    ipAddress - Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssocIpAddress(L7_uchar8 routerID, 
                              L7_uint32 intIfNum,
                              L7_uint32 ipAddress);

/*********************************************************************
* @purpose  To Set the VRRP mode
*
* @param    routerID   Router Id to be set.
* @param    intIfNum  Internal interface number. 
* @param    state  Virtual router state. 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrrpModeSet(L7_uchar8 routerID, 
                           L7_uint32 intIfNum,
                           L7_vrrpStartState_t state);

/*********************************************************************
* @purpose  To Get the VRRP mode
*
* @param    routerID   Router Id to be set.
* @param    intIfNum  Internal interface number. 
* @param    state  Virtual router state. 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrrpModeGet(L7_uchar8 routerID, 
                           L7_uint32 intIfNum,
                           L7_vrrpStartState_t* state);

/*********************************************************************
* @purpose  To get the vrrp version supported by this node.
*
* @param    Version    Version of the vrrp supported
*
* @returns  L7_SUCCESS     
* @returns  L7_FAILURE  
*
* @notes    
*
 @end
*********************************************************************/
L7_RC_t L7_vrrpNodeVersionGet(L7_uint32* version) ;

/*********************************************************************
* @purpose  To get the status wheather VRRP-enabled router will generate 
            SNMP traps for events defined in this MIB
*
* @param    Mode -     SNMP TRAP Flag
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
 @end
*********************************************************************/
L7_RC_t L7_vrrpNotificationCntlGet(L7_uchar8* mode);


/*********************************************************************
* @purpose  To Set the SNMP traps Flag.
*
* @param    Mode -     SNMP TRAP Flag
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpNotificationCntlSet(L7_uchar8 mode);

/*********************************************************************
* @purpose  To Get the state of router(Master/backup or initialised)
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    state -     State of the router(Out param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpStateGet(L7_uchar8 routerID,L7_uint32 intIfNum, 
                        L7_vrrpState_t* state);

/*********************************************************************
* @purpose  To Get the  Ip address of virtial router
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    ipAddress - virtual IpAddress(Out param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpIpAddressGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                            L7_uint32* ipAddress);

/*********************************************************************
* @purpose  To Get the  Ip address of the master router
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    ipAddress  Master router's IP Address(Out param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpMasterIpAddressGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                                  L7_uint32* ipAddress);


L7_RC_t L7_vrrpIpUpTimeGet(L7_uchar8 routerID, L7_uint32 intIfNum, 
                           L7_ulong32* uptime); 

/*********************************************************************
* @purpose  To Get the The particular protocol being controlled by this 
*           Virtual Router
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    protocol - protocol type
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpProtocolGet(L7_uchar8 routerID, L7_uint32 intIfNum, 
                           L7_vrrpProtocolType_t* protocol);
/*********************************************************************
* @purpose  To Set the The particular protocol being controlled by this 
*           Virtual Router
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    protocol - protocol type
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpProtocolSet(L7_uchar8 routerID, L7_uint32 intIfNum, 
                           L7_vrrpProtocolType_t protocol);


/*********************************************************************
* @purpose  To Get the the number of packet received with invalid checksum
*
* @param    checkSumError - Checksum error(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpChecksumErrorGet(L7_uint32* checkSumError);

/*********************************************************************
* @purpose  To Get the the number of packet received with wrong vrrp version
*
* @param    versionError - version error(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpRouterVerErrorGet(L7_uint32* versionError);

/*********************************************************************
* @purpose  To Get the the number of packet received with wrong vrrpid
*
* @param    vrIdError - version Id Errors (Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVridErrorGet(L7_uint32* vrIdError);

/*********************************************************************
* @purpose  To Get the the number of time state change to master
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    masterState - master state transfer (Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpStateMasterGet(L7_uchar8 routerID, L7_uint32 intIfNum, 
                              L7_uint32* masterState);
/*********************************************************************
* @purpose  To Get the the number of vrrp advertisement received master
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    addReceived - Number of add received (Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAdverReceived(L7_uchar8 routerID,L7_uint32 intIfNum, 
                             L7_uint32* addReceived);
/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with wrong Addinterval
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    addIntErrors - Number of wrong interval pkt(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAddIntErrorGet(L7_uchar8 routerID,L7_uint32 intIfNum, 
                              L7_uint32* addIntErrors);


/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received which fails 
*           the authentication criteria
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    authFailedPkt - Number of auth failed pkt(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAuthFailedGet(L7_uchar8 routerID, 
                             L7_uint32 intIfNum,
                             L7_uint32* authFailedPkt);

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with IP TTL 
*           not equal to 255
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    ttlFailedPkt - Number of wrong ttl pkt received pkt(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpIpTTLFailedGet(L7_uchar8 routerID,
                              L7_uint32 intIfNum,
                              L7_uint32* ttlFailedPkt);

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with 0 priority
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    zeroPriorRcvPkt - number of zero priority pkt received(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpZeroPriorityRcvGet(L7_uchar8 routerID,L7_uint32 intIfNum, 
                                  L7_uint32* zeroPriorRcvPkt);

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt sent with 0 priority
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    zeroPriorSentPkt - number of zero priority pkt sent(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpZeroPrioritySentGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                                   L7_uint32* zeroPriorSentPkt);

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an 
*           invalid value in the type field
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    invalidTypePkt - number of invalid type pkt received(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpInvldTypePktGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                               L7_uint32* invalidTypePkt);


/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an 
*           invalid Ip address.
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    addListerrPkt - number of pkt received 
*           with invalid ip address(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAddListErrGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                             L7_uint32* addListerrPkt);

 
/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an 
*           invalid Authentication type.
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    invAuthTypePkt - number of invalid pkt received 
*           with an invalid Authentication type(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpInvldAuthTypeGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                                L7_uint32* invAuthTypePkt);

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with mismatched
*           Authentication type.
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    authMismatchPkt - number of invalid pkt received 
*           with mismatched Authentication data(Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAuthTypeMismatchGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                                   L7_uint32* authMismatchPkt);

/*********************************************************************
* @purpose  To Get the the number of invalid vrrp pkt received 
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    pktError - number of invalid pkt received (Out Param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpInvldPktGet(L7_uchar8 routerID,L7_uint32 intIfNum,
                           L7_uint32* pktError);

/*********************************************************************
* @purpose  To Get the up time of the virtual Router
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    uptime - up time of the router (Out param)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpUpTimeGet(L7_uchar8 routerID,
                         L7_uint32 intIfNum,
                         L7_uint32* uptime);

 /*********************************************************************
* @purpose  To Get the Admin Mode
*
* @param    adminMode   Admin mode
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAdminModeGet(L7_BOOL *adminMode);

/*********************************************************************
* @purpose  To Set the Admin Mode
*
* @param    adminMode   Admin mode
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAdminModeSet(L7_BOOL adminMode);


/*********************************************************************
* @purpose  To get the router id and intif num of the first vrrp 
*           configuration from the list.
*
* @param    vrId  Virtual router id (out param)
* @param    intIfNum int interface number (out param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpFirstGet(L7_uchar8* vrId, L7_uint32* intIfNum);

/*********************************************************************
* @purpose  To get the router id and intif num of the next vrrp 
*           configuration from the list.
*
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                       L7_uchar8* nextVrId, L7_uint32* nextInfNum);


/*********************************************************************
* @purpose  To get the Vmac address associated with an IP
*
* @param    ipAddress  Ip address 
* @param    vMac   Vmac address ( Out param)
* @param    vrId   VR Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes   The function returns the vmac if router is in master state 
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVMacGet(L7_uint32 ipAddress, L7_uchar8* vMac, L7_uchar8 *vrId);

/*********************************************************************
* @purpose  To get the ip address associated with the VMAC.
*
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
L7_RC_t L7_vrrpIpGet(L7_uchar8* vMac,L7_uint32 *ipAddress);

/*********************************************************************
* @purpose  To get the total number of virtual router configuration
*
* @param    count 
* @param    intIfNum int interface number 
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpConfigCountGet(L7_uint32* count);


/*********************************************************************
* @purpose  To Check if configuration is present corresponding to a vrid 
*           and intnum
*
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperEntryGet(L7_uchar8 vrId, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  To get the logical next entry from the configuration list
*
* @param    vrId  Virtual router id 
* @param    intIfNum int interface number 
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperEntryNextGet(L7_uchar8 vrId, 
                                L7_uint32 intIfNum,
                                L7_uchar8* nextVrId, 
                                L7_uint32* nextInfNum);





/*********************************************************************
* @purpose  To Set the state of router(Master/backup or initialised)
*
* @param    routerID   Router Id. 
* @param    intIfNum   Interface Number
* @param    state -     State of the router
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpStateSet(L7_uchar8 routerID,L7_uint32 intIfNum, 
                        L7_vrrpState_t state);

/*********************************************************************
* @purpose  To get the 0 priority advert rcvd flag
*
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
L7_RC_t L7_vrrpZeroPriorityFlgGet(L7_uchar8 vrId,
                                  L7_uint32 intIfNum,
                                  L7_BOOL *zeroFlg);

/*********************************************************************
* @purpose  To set the 0 priority advert rcvd flag
*
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
L7_RC_t L7_vrrpZeroPriorityFlgSet(L7_uchar8 vrId,
                                  L7_uint32 intIfNum,
                                  L7_BOOL zeroFlg);


/*********************************************************************
* @purpose  To get the row status
*
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
L7_RC_t L7_vrrpOperRowStatusGet(L7_uchar8  vrId,
                                L7_uint32  intIfNum,
                                L7_uint32* rowStatus);

/*********************************************************************
* @purpose  To get the row status of Associate Ip address.
*
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
L7_RC_t L7_vrrpAssocIpRowStatusGet(L7_uchar8 vrId,
                                   L7_uint32 intIfNum,
                                   L7_uint32* assocIpRowStat);

/*********************************************************************
* @purpose  To Set the row status of Associate Ip address.
*
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
L7_RC_t L7_vrrpAssocIpRowStatusSet(L7_uchar8 vrId,
                                   L7_uint32 intIfNum,
                                   L7_uint32 assocIpRowStat);
/*********************************************************************
* @purpose  To add a secondary IP address to the virtual router
*
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
L7_RC_t L7_vrrpAssocSecondaryIpAddress(L7_uchar8 routerID,
                              L7_uint32 intIfNum,
                              L7_uint32 ipAddress);

/*********************************************************************
* @purpose  To remove a secondary IP address from the virtual router
*
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
L7_RC_t L7_vrrpDissocSecondaryIpAddress(L7_uchar8 routerID, 
                                        L7_uint32 intIfNum,
                                        L7_uint32 ipAddress);

/*********************************************************************
* @purpose  To get the next configured Virtual Router IP address
*
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
L7_RC_t L7_vrrpIpAddressNextGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                                L7_uint32 ipAddress, L7_uint32 *pIpAddress);

/*********************************************************************
* @purpose  Determine whether this router is the master or backup on a given 
*           interface for any virtual router.
*
* @param    intIfNum @b{(input)} - Internal Interface Number
* @param    vrrpId @b{(output)} - VRRP vrid of virtual router for which
*              this router is master on interface intIfNum
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL L7_vrrpIsActive(L7_uint32 intIfNum, L7_uchar8 *vrrpId);

/*********************************************************************
* @purpose  Checks to see if the interface is valid
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL L7_vrrpIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  To get the next  valid VR Id,interface and IP address
*
* @param    vrId  @b{(input)} - router Id
* @param    intIfNum  @b{(input)} - Interface Number
* @param    ipAddress  @b{(input)} -Ip address
* @param    vrIdNext @b{(output)} - Next Vr Id
* @parm     intIfNumNext @b{(output)} - Next Internal Interface Number
* @parm     ipAddressNext @b{(output)} - Next Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssoIpAddrEntryNextGet (L7_uchar8 vrId, L7_uint32 intIfNum,
                                       L7_uint32 ipAddress, L7_uchar8 *vrIdNext,
                                       L7_uint32 *intIfNumNext, 
                                       L7_uint32 *ipAddressNext);

/*********************************************************************
* @purpose  Uses utility function in VRRP to search VRRP data 
*           structures to determine if the provided IP address is
*           associated with a VRRP virtual router for which we are
*           master.
*
* @param    ipAddress  ip address
* @param    vmac       VRRP mac address
* @param    vrId       VRRP Id
*
* @returns  L7_SUCCESS  success
* @returns  L7_FALIURE  failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t vrrpMapGetVMac(L7_uchar8 *ipAddress, L7_uchar8 *vmac, L7_uchar8 *vrId);

/*********************************************************************
* @purpose  add ip interface into the vrrp list of tracking interfaces.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    trackIntIfNum   Interface Number to be tracked
* @param    priorityDecrement - priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfAdd(L7_uchar8 vrId, L7_uint32 intIfNum,
                                L7_uint32 trackIntIfNum,
                                L7_uchar8 prio_dec );

/*********************************************************************
* @purpose  delete ip interface from vrrp list of tracking interfaces.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    trackIntIfNum   Interface Number to be tracked
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfDelete( L7_uchar8 vrId, L7_uint32 intIfNum,
                                    L7_uint32 trackIntIfNum);

/*********************************************************************
* @purpose  add ip route into the vrrp list of  tracking routes.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    trackRoute   Route to be tracked
* @param    priorityDecrement - priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteAdd( L7_uchar8 vrId, L7_uint32 intIfNum,
                                  L7_uint32 netAddr, L7_uint32 mask,
                                  L7_uchar8 prio_dec);

/*********************************************************************
* @purpose  delete ip route from the vrrp list of tracking routes.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    trackRoute Route to be tracked
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteDelete( L7_uchar8 vrId, L7_uint32 intIfNum,
                                     L7_uint32 netAddr, L7_uint32 mask);

/*********************************************************************
* @purpose  get the tracked interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input}  Interface Number to be tracked
* @param    priorityDecrement @b{output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfPrioGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                    L7_uint32 trackIntIfNum, L7_uchar8 *prio_dec );

/*********************************************************************
* @purpose  get the tracked interface state.
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
L7_RC_t L7_vrrpOperTrackIntfStateGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                     L7_uint32 trackIntIfNum, L7_BOOL *state );

/*********************************************************************
* @purpose  Get Next tracked interface.
*
* @param    vrId @b{input/output} Router Id.
* @param    intIfNum @b{inpu/outputt} Interface Number
* @param    trackIntIfNum @b{input/output}  Interface Number to be tracked
* @param    priorityDecrement @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfNextGet(L7_uchar8 *vrId, L7_uint32 *intIfNum,
                                    L7_uint32 * trackIntIfNum);

/*********************************************************************
* @purpose  Get Next tracked interface for the given vrid and interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input/output}  Interface Number to be tracked
* @param    priorityDecrement @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrIdIntfTrackIntfNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                        L7_uint32 * trackIntIfNum, L7_uchar8 * prio_dec );

/*********************************************************************
* @purpose  Get the tracked interface for the given vrid and interface.
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
L7_RC_t L7_vrrpVrIdIntfTrackIntfGet(L7_uchar8 vrId, L7_uint32 intIfNum,
        L7_uint32  trackIntIfNum);

/*********************************************************************
* @purpose  get the tracked route .
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
L7_RC_t L7_vrrpVrIdIntfTrackRouteGet(L7_uchar8 vrId, L7_uint32 intIfNum,
        L7_uint32 ipAddr,L7_uint32 netMask );

/*********************************************************************
* @purpose  Get the first (lowest numeric prefix) tracked route for a given
*           VR on a given interface.
*
* @param    vrId     @b{input}   VRRP router ID
* @param    intIfNum @b{input}   interface where virtual router is configured
* @param    ipAddr   @b{output}  Destination prefix of first tracked route
* @param    netmask  @b{output}  Destination network mask of first tracked route
*
* @returns  L7_SUCCESS  if a tracked route is found
* @returns  L7_FAILURE  if no tracked routes on this VR
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t vrrpVrTrackedRouteGetFirst(L7_uchar8 vrId, L7_uint32 intIfNum, 
                                   L7_uint32 *ipAddr, L7_uint32 *netmask);

/*********************************************************************
* @purpose  Get Next tracked Route.
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
L7_RC_t L7_vrrpOperTrackRouteNextGet(L7_uchar8 *vrId, L7_uint32 *intIfNum,
                                     L7_uint32 * ipAddr, L7_uint32 * mask);

/*********************************************************************
* @purpose  Get Next tracked Route for given Vrid and interface
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
L7_RC_t L7_vrrpVridIntfTrackRouteNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                         L7_uint32 * ipAddr, L7_uint32 * mask,
                                         L7_uchar8 * prio_dec );

/*********************************************************************
* @purpose  get the tracked route priority.
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
L7_RC_t L7_vrrpOperTrackRoutePrioGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                     L7_uint32 ipAddr,L7_uint32 netMask, 
                                     L7_uchar8 *prio_dec );

/*********************************************************************
* @purpose  get the tracked route reachability
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    ipAddr @b{input}  ip address of tracked route.
* @param    netMask @b{input} subnet mask
* @param    reachable @b{output} reachability of the tracked route.
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteReachabilityGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                             L7_uint32 ipAddr,L7_uint32 netMask, 
                                             L7_BOOL *reachable );

/*********************************************************************
* @purpose  To Set the VRRP accept mode
*
* @param    routerID    @b{input} Router Id.
* @param    intIfNum    @b{input} Internal interface number. 
* @param    accept_mode @b{input} When enabled VRRP master will accept 
*                                 packets destined to VRIP and on 
*                                 disabling VRRP master will discard 
*                                 packets destined to VRIP.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAcceptModeSet(L7_uchar8 routerID,
                                 L7_uint32 intIfNum,
                                 L7_BOOL   accept_mode);

/*********************************************************************
* @purpose  To Get the VRRP accept mode
*
* @param    routerID    @b{input}  Router Id.
* @param    intIfNum    @b{input}  Internal interface number. 
* @param    accept_mode @b{output} When enabled VRRP master will accept 
*                                  packets destined to VRIP and on 
*                                  disabling VRRP master will discard 
*                                  packets destined to VRIP.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAcceptModeGet(L7_uchar8 routerID,
                                 L7_uint32 intIfNum,
                                 L7_BOOL   *accept_mode);


/*********************************************************************
* @purpose  To send the gratuitous ARP packets on those interfaces and
*           in those VR groups where the VRRP is in Master mode.
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void L7_vrrpMasterGratARPSend(void);

/*********************************************************************
* @purpose  To Set the description of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    desc   Description string pointer
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperDescriptionSet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uchar8* description);

/*********************************************************************
* @purpose  To get the description of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    desc   Description string pointer
*
* @returns  L7_SUCCESS or L7_FAILURE.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperDescriptionGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uchar8* description);

/*********************************************************************
* @purpose  To Set the timer leaning status of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    mode       to enable/disable the timer leaning on the router
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperTimerLearnSet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_BOOL mode);

/*********************************************************************
* @purpose  Function to Get the timer learn state
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    mode     - pointer to timer learning status
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperTimerLearnGet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_BOOL* mode);

/*********************************************************************
* @purpose  To Set a delay for the preemption of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    delay      to set the delay interval for preemption
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperPreemptDelaySet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_ushort16 delay);

/*********************************************************************
* @purpose  To get the configured delay for the preemption.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    preemptdelay  to get the delay interval for preemption
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperPreemptDelayGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_ushort16* preemptdelay);

#endif /* _L7_VRRP_API_H_ */
