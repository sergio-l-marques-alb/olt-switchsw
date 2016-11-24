
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename vrrp_util.h
*
* @purpose VRRP header file
*
* @component VRRP
*
* @comments 
*
* @create  08/11/2001
*
* @author  Kavleen Kaur et. al.
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef _VRRP_MAP_UTIL_H_
#define _VRRP_MAP_UTIL_H_

           
#include "l7_common.h" 
#include "vrrp_config.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"                          
#include "async_event_api.h"
#include "l3_default_cnfgr.h"
#include "rto_api.h"
/****************************************
*
*  VRRP Basic Configuration data                
*
*****************************************/

typedef struct vrrpRouterBasicConfiguration_s
{  
  L7_uchar8   notif_flag;
  L7_uchar8   vrrp_version;
  L7_uint32   checksum_err;
  L7_uint32   version_err;
  L7_uint32   vrid_error;
} vrrpRouterBasicConfiguration_t;

/****************************************
*
*  VRRP non-Configuration Data         
*
*****************************************/

typedef struct vrrpVirtRouterOperData_s
{
  L7_uint32          intIfNum;    /* internal interface number */
  L7_vrrpState_t     vr_state;   /* Indicates the State of the virtual router  Init/Master/Backup*/
  L7_short16         master_down_int;      /* Master down timer Interval*/
  L7_uchar8          vmac[L7_MAC_ADDR_LEN];  /* Virtual MAC address */
  L7_vrrpProtocolType_t protocol_type;       /* protocol type supported by VRRP */
  L7_vrrpOperRowStatus_t oper_rowstatus;   /* row status for vrrpOperTable */
  L7_uchar8          ipAddressCount;       /* number of IP addresses associated with this virtual router */
  L7_uint32          num_time_become_master;  /* number of times the virtual router's state has transitined to MASTER */
  L7_uint32          advert_received;         /* number of advertisements received */
  L7_uint32          pkt_advert_interval_error; /* number of packets in which advertisement received is diffrent than the configured */
  L7_uint32          pkt_auth_failure;         /* number of packets failing the authentication check */
  L7_uint32          pkt_ttl_incorrect;        /* number of packets where TTL is not equal to 255 */
  L7_uint32          pkt_rcvd_zero_priority;   /* number of packets received with priority equal to 0 */
  L7_uint32          pkt_sent_zero_priority;   /* number of packets sent with priority equal to 0 */
  L7_uint32          pkt_rcvd_invalid_type;    /* number of packets with invalid value in 'type' field */
  L7_uint32          pkt_rcvd_invalid_ip_address_list; /* number of packets with IP address list error */
  L7_uint32          pkt_rcvd_invalid_auth_type;    /* number of packets with invalid auth. type */
  L7_uint32          pkt_rcvd_mismatched_auth_type; /* number of packets with Auth. type mismatch */
  L7_uint32          pkt_rcvd_invalid_length;       /* number of packets received with packet
                                                 length less than the length of the VRRP header */ 
  L7_uint32          pkt_rcvd_wrong_subnet;    /* source IP addr not in same subnet as local primary */
  L7_uint32          pkt_addr_count;           /* Number of addresses in rx pkt doesn't match local */

  L7_vrrpOperRowStatus_t assoc_ip_rowstatus;
  L7_BOOL            zerp_priority_pkt_rcvd_flag; /*Flag indicate that wheather subsystem received a 0 priority advert*/
  L7_clocktime       last_packet_recvtime;       /* time at which the last VRRP packet was received */
  L7_clocktime       vrrp_uptime;  /* time of day when the virtual router trasitioned out of 'initialized'*/
  L7_clocktime       lastAdvt_Sendtime;               /* value of 'sysUpTime' when the virtual router
                                                 has trasitioned to 'initialized'*/
  L7_uint32          lastGratArpTime;           /* time since grat ARP last sent. */
  L7_IP_ADDR_t       masterIpAddr;              /* the master router's real IP address */
  L7_BOOL            rtrIntfUp;     /* L7_TRUE if router interface is up */
  L7_rtrIntfIpAddr_t localAddr;     /* primary IP addr and mask on local interface */
  L7_uint32          vrrpSysUpTime; /* value of sysUpTime when the virtual router trasitioned out of 'initialized' */
  L7_uchar8          oper_priority; /* Operational priority of the VRRP router */
} vrrpVirtRouterOperData_t;

/****************************************
*
*  VRRP Router Info data                
*
*****************************************/

typedef struct vrrpRouterInfo_s 
{
  vrrpVirtRouterCfg_t        vrrpCfgInfo;              /*vrrp configuration data*/ 
  vrrpVirtRouterOperData_t   vrrpVirtRouterOperInfo;   /*vrrp non configuration data*/
} vrrpRouterInfo_t;

/****************************************
*
*  VRRP List         
*
*****************************************/

typedef struct vrrpList_s 
{
    struct vrrpList_s    *next;         /* pointer to next list item */
    struct vrrpList_s    *previous;     /* pointer to previous list item */
    vrrpRouterInfo_t     *object;       /* data */      
} vrrpList_t;


/****************************************
*
*  VRRP Row Information data                 
*
*****************************************/

typedef struct vrrpIntfInformation_s 
{
  L7_short16 intIfNum;
  L7_short16 totalConfigured;
} vrrpIntfInformation_t;

typedef struct vrrpMapInfo_s
{
  /* Timer that tells VRRP to request best route changes from RTO. */
  osapiTimerDescr_t *bestRouteTimer;

} vrrpMapInfo_t;

/*-------------------------------------------------*/
/*              FUNCTION PROTOTYPES                */
/*-------------------------------------------------*/

/* vrrp_map.c */

L7_RC_t vrrpStartTasks(void);
L7_RC_t vrrpSave(void);
L7_RC_t vrrpRestoreProcess(void);
L7_BOOL vrrpHasDataChanged(void);
void vrrpResetDataChanged(void);
void vrrpBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t vrrpApplyConfigData(void);
L7_RC_t vrrpSysnetRegister(L7_BOOL mode);
L7_RC_t vrrpDebugSave(void);
L7_RC_t vrrpDebugRestore(void);
L7_BOOL vrrpDebugHasDataChanged(void);
void vrrpDebugBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t vrrpApplyDebugConfigData(void);


/* vrrp_util.c */

L7_short16 vrrpIntfInfoTblIndexGet(L7_uint32 intIfNum);
L7_RC_t L7_vrrpCheckPrecondition(vrrpRouterInfo_t *operData);

void  L7_vrrpRouterStart(vrrpRouterInfo_t*  virtualRouter);

void  L7_vrrpRouterStop (vrrpRouterInfo_t*  virtualRouter);

L7_RC_t  vrrplistAdd( L7_uchar8 vrid, L7_uint32 intIfNum, L7_BOOL rtrIntfUp,
                      L7_uint32 ipAddr, L7_uint32 ipMask);
L7_RC_t  vrrplistDelete( L7_uchar8 vrid, L7_uint32 intIfNum);
void vrrpListMapping();
void L7_vrrpStopAll();
void L7_vrrpStartAll();
L7_RC_t vrrpRoutingIntfEnable(L7_uint32 intIfNum, L7_uint32 ipAddr, 
                              L7_uint32 netmask);
L7_RC_t vrrpRoutingIntfDisable(L7_uint32 intIfNum);
L7_RC_t L7_vrrpAllInfRouterStart(L7_uint32 intIfNum);
L7_RC_t L7_vrrpAllInfRouterStop(L7_uint32 intIfNum);
L7_RC_t L7_vrrpRouterIntfConfigClear(L7_uint32 intIfNum);
L7_RC_t L7_vrrpShutdownProcess(L7_uchar8 vrid, L7_uint32 intifnum );

L7_RC_t vrrpMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
L7_RC_t vrrpMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *response);
L7_RC_t vrrpMapIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
L7_RC_t vrrpMapRoutingEventChangeProcess(L7_uint32 intIfNum,
                                         L7_uint32 event,
                                         void *pData, 
                                         ASYNC_EVENT_NOTIFY_INFO_t *response);


L7_RC_t L7_vrrpChecksumErrorIncr();
L7_RC_t L7_vrrpRouterVerErrorIncr();
L7_RC_t L7_vrrpVridErrorIncr();
vrrpRouterInfo_t*  L7_vrrpParamsFetch (L7_uchar8 vrid, 
                                       L7_uint32 intIfNum);

vrrpRouterInfo_t*  L7_vrrpVirtualRouterFetch (L7_uchar8 vrid, 
                                            L7_uint32 intIfNum);
L7_BOOL vrrpInterfaceAssociate(L7_uint32 intIfNum);
L7_RC_t vrrpGetVMac(L7_uchar8 *ipAddress, L7_uchar8 *tVMAC);
SYSNET_PDU_RC_t vrrpIpFrameFilter(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc);
SYSNET_PDU_RC_t vrrpPacketIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc);

L7_RC_t vrrpMapIntfChangeNimNotify(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_NOTIFY_CB_FUNC_t callback,
                                 NIM_HANDLE_t *handle);

L7_RC_t L7_vrrpMgmtVmacAddrAdd(L7_uint32 intIfNum, L7_uchar8 *vmac);

L7_RC_t L7_vrrpMgmtVmacAddrDel(L7_uint32 intIfNum, L7_uchar8 *vmac);

vrrpRouterInfo_t *L7_vrrpParamsFetchLocked (L7_uchar8 vrid, L7_uint32 intIfNum, L7_BOOL next);

/* vrrp tracking related apis */
void vrrpMapRouteCallback();
L7_RC_t vrrpMapBestRoutesGet(void);
L7_RC_t vrrpMapRtoRouteChangeProcess(L7_routeEntry_t *routeEntry, RTO_ROUTE_EVENT_t route_status);
void vrrpMapBestRouteTimerCb(void);
L7_RC_t vrrpMapTrackRouteEntryEnable(L7_uint32 ipAddr, L7_uint32 subnetMask);
L7_RC_t vrrpMapTrackRouteEntryDisable(L7_uint32 ipAddr, L7_uint32 subnetMask);
L7_RC_t vrrpTrackingInterfaceEnable(L7_uint32 intIfNum);
L7_RC_t vrrpTrackingInterfaceDisable(L7_uint32 intIfNum);
void vrrpOperationalPriorityCalc (vrrpRouterInfo_t *virtualRouter);

/* vrrp_migrate.c */
void vrrpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);


/*                                                                     
***********************************************************************
*                      VRRP TRAPS FUNCTIONS
***********************************************************************
*/

L7_RC_t vrrpMapTrapNewMasterTrap(L7_uint32 vrrpOperMasterIpAddr);

L7_RC_t vrrpMapTrapAuthFailureTrap(L7_uint32 vrrpTrapPacketSrc, L7_int32 vrrpTrapAuthErrorType);

L7_BOOL vrrpIntfIsConfigurable(L7_uint32 intIfNum, vrrpIntfInformation_t **pIntfInfo);

L7_BOOL vrrpIntfInfoTblEntryGet(L7_uint32 intIfNum, vrrpIntfInformation_t **pIntfInfo);

L7_RC_t vrrpIntfCreate(L7_uint32 intIfNum) ;

L7_RC_t vrrpIntfDelete(L7_uint32 intIfNum) ;

L7_RC_t vrrpIntfCreateOperInfo(L7_uint32 intIfNum);

L7_RC_t vrrpIntfDeleteOperInfo(L7_uint32 intIfNum);

void vrrpIntfBuildDefaultConfigData(L7_uint32 intIfNum, vrrpIntfInformation_t *pIntfInfo);

void vrrpVRIdToIndexMapTblInit();
L7_uchar8 vrrpVRIdToIndexGet(L7_uchar8 vrId, L7_uint32 intIfNum);
L7_RC_t vrrpVRIdToIndexMap(L7_uchar8 vrId, L7_uint32 intIfNum);
L7_RC_t vrrpVRIdToIndexUnmap(L7_uchar8 vrId, L7_uint32 intIfNum);



#endif /* _VRRP_MAP_UTIL_H_ */
