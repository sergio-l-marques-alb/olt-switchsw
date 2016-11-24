
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename vrrpTask.h
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
#ifndef _VRRP_MAP_TASK_H_
#define _VRRP_MAP_TASK_H_

#define VRRP_PKT_ADV				1	/* Advertisement packet type */

#define VRRP_PROTO_VERSION			2

#define VRRP_MSG_SIZE				sizeof(vrrpMsg_t)
 
#define VRRP_ARP_ADDR_LEN			4

#define ADVT_SEND_TIMER_INTERVAL	100
#define MASTER_CHECK_TIMER_INTERVAL	100

/* Master sends a grat ARP every 30 seconds to aid station learning. */
#define VRRP_GRAT_ARP_INTERVAL 30

#define SECONDS_FACTOR				1000
#define NANOSECONDS_FACTOR			1000000

#define VRRP_QUEUE					"vrrp_Queue"


typedef enum
{
    vrrpMsgAdvtTimerTick = 1,
    vrrpMsgMasterCheckTimerTick,
    vrrpMsgPacketRecv,
    vrrpMsgCnfgr,
    vrrpMsgIntfEvent,
    vrrpMsgRtrEvent,
    vrrpMsgRestoreEvent,
    vrrpMsgBestRouteEvent  
} vrrpMessages_t;


typedef struct
{
	L7_uchar8       version_type;	/* Type and Version */
	L7_uchar8       vrid; 			/* Virtual router Id */	
	L7_uchar8       priority;		/* Router Priority */
	L7_uchar8       naddr;			/* No of IP Addresses */
	L7_uchar8       auth_type;		/* Authentication Type */
	L7_uchar8       adver_int;		/* Advertisement Interval */
	L7_ushort16     chksum;			/* checksum */
	L7_uint32		ipaddress[L7_L3_NUM_IP_ADDRS];	
	L7_uchar8		auth_data[L7_VRRP_MAX_AUTH_DATA];                                
} vrrpPkt_t;
#define VRRP_PACKET_LENGTH  ( L7_IP_HDR_LEN+sizeof(vrrpPkt_t) )



typedef struct vrrpIntfEventMsg_s
{
    L7_uint32           intIfNum;       
    L7_uint32           event;          /* one of L7_PORT_EVENTS_t from NIM */
    NIM_CORRELATOR_t    correlator;
} vrrpIntfEventMsg_t;
      


typedef struct vrrpRtrEventMsg_s
{
    L7_uint32                   intIfNum;       
    L7_uint32                   event;          /* one of L7_RTR_EVENT_CHANGE_t */
    void                        *pData;         /* event-specific data */
    L7_BOOL                     asyncResponseNeeded;
    ASYNC_EVENT_NOTIFY_INFO_t   response;     /* Asynchronous response info */
} vrrpRtrEventMsg_t;


typedef struct vrrpMsg_s
{
    L7_uint32          msg_id;

    union
    {
        L7_uint32                   intfNumPktRcvd; /* Used by the protocol when msg_id==vrrpMsgPacketRecv */
        vrrpIntfEventMsg_t          intfEvent;
        vrrpRtrEventMsg_t           rtrEvent;
        L7_CNFGR_CMD_DATA_t         cnfgrCmdData;
    }type;

    L7_uchar8 *vrrpMessage;
    L7_uint32 messageLen;
} vrrpMsg_t;

extern void  *vrrp_Queue; 

extern void VRRPDaemonTask();
extern void VRRPAdvertTimerCheck();
extern void VRRPMasterCheck();
extern L7_RC_t vrrpPacketProcess(L7_netBufHandle netBufHandle,L7_uint32 intIfNum);

L7_RC_t VRRPAdvertisementSend();
L7_RC_t VRRPMasterTimeCheck();
L7_RC_t vrrpIPBuild(vrrpRouterInfo_t* routerInfo,L7_uchar8* vrrp_buff);
L7_RC_t vrrpDataBuild(vrrpRouterInfo_t* routerInfo,L7_uchar8* vrrp_buff,int zeroPriorityFlag);
L7_RC_t vrrpPktBuild(vrrpRouterInfo_t* routerInfo,L7_uchar8* vrrp_buff,int zeroPriorityFlag);

L7_RC_t vrrpPacketValidate(L7_ipHeader_t *ip_hdr,vrrpPkt_t *vrrp_pkt,L7_uint32 intIfNum,L7_uint32 recv_pktlen);
L7_ushort16 vrrpCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum);
L7_RC_t vrrpAdvtPacketProcess(vrrpMsg_t *msg);
L7_ushort16 vrrpCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum);
extern L7_RC_t VRRPTransitToMaster(vrrpRouterInfo_t* routerInfo, L7_uchar8 vmac[L7_MAC_ADDR_LEN],L7_uint32 intIfNum);
extern L7_RC_t VRRPTransitToBackup(vrrpRouterInfo_t* routerInfo, L7_uint32 intIfNum);
extern L7_RC_t VRRPShutdownVirtualRouter(vrrpRouterInfo_t* routerInfo,L7_uint32 intIfNum);

/*extern L7_RC_t VRRPZeroPriorityAdvtSend(vrrpRouterInfo_t* routerInfo);*/

L7_RC_t vrrpGratuitousARPSend(L7_uchar8 vmac[L7_MAC_ADDR_LEN],
                                     L7_uint32 sourceIP,L7_uint32 infNumber);
L7_RC_t vrrpAdvtPacketSend(vrrpRouterInfo_t* routerInfo,int zeroPriorityFlag);
    
#endif



