/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dhcp.h
*
* @purpose dhcp h file
*
* @component dhcp
*
* @comments Ported from linux ecos dhcp_support.c (refer Next Header)
*
* @create 17/05/2001
*
* @author  ported by Paresh Jain
*
* Krishna Mohan CS - 11-Jan-2010:
* - Modifications to include Host Interface Support
*
*
* @end
*             
**********************************************************************/

#include "l7_socket.h"
#include "osapi.h"
#include "dhcp_client_api.h"

/*==========================================================================
  
        include/dhcp.h
  
        DHCP protocol support
  
  ==========================================================================
  ####COPYRIGHTBEGIN####
                                                                            
   -------------------------------------------                              
   The contents of this file are subject to the Red Hat eCos Public License 
   Version 1.1 (the "License"); you may not use this file except in         
   compliance with the License.  You may obtain a copy of the License at    
   http:  www.redhat.com/                                                   
                                                                            
   Software distributed under the License is distributed on an "AS IS"      
   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
   License for the specific language governing rights and limitations under 
   the License.                                                             
                                                                            
   The Original Code is eCos - Embedded Configurable Operating System,      
   released September 30, 1998.                                             
                                                                            
   The Initial Developer of the Original Code is Red Hat.                   
   Portions created by Red Hat are                                          
   Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
   All Rights Reserved.                                                     
   -------------------------------------------                              
                                                                            
  ####COPYRIGHTEND####
  ####BSDCOPYRIGHTBEGIN####
  
   -------------------------------------------
  
   Portions of this software may have been derived from OpenBSD or other sources,
   and are covered by the appropriate copyright disclaimers included herein.
  
   -------------------------------------------
  
  ####BSDCOPYRIGHTEND####
  ==========================================================================
  #####DESCRIPTIONBEGIN####
  
   Author(s):    hmt
   Contributors: gthomas
   Date:         2000-07-01
   Purpose:      Support DHCP initialization in eCos TCPIP stack
   Description:  
                
  
  ####DESCRIPTIONEND####
  
  ==========================================================================

   
   DHCP.  RFC2131, RFC1533, RFC1534
   See also bootp.h
*/ 
#ifndef INCLUDE_DHCP
#define INCLUDE_DHCP

/* DHCP messages; these are sent in the tag TAG_DHCP_MESS_TYPE already
   defined in bootp.h*/

#define DHCP_CLIENT_DBG_MSG_SIZE_MAX   256

#define DHCP_LEASE_T1 1
#define DHCP_LEASE_T2 2
#define DHCP_LEASE_EX 4

#define DHCP_CLIENT_NOTIFY_LIST_MAX   10

#define DHCP_CLIENT_SELECT_TIMEOUT_SEC  3
#define DHCP_CLIENT_SELECT_TIMEOUT_MSEC 0

/* Number of back-off exponential timeout trails before the DHCP Client
 * ceases to operate.
 */
#define DHCP_CLIENT_NUM_ADDRESS_TRAILS 3

/* Following definitions have been picked up from Red Hat/eCos bootp.h file*/
#define BP_CHADDR_LEN  16
#define BP_SNAME_LEN   64
#define BP_FILE_LEN   128

/* The standard requires only 312 bytes here*/
#define BP_VEND_LEN  312
#define BP_MINPKTSZ  576                                        
#define IP_BUFF_SIZE  60

typedef enum
{
  DHCP_CLIENT_CNFGR_MSG = 1,
  DHCP_CLIENT_ROUTING_EVENT_MSG,
  DHCP_CLIENT_UI_EVENT_MSG
} dhcpClientMessages_t;

typedef struct dhcpClientRoutingEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                       *pData;
  L7_BOOL                    asyncResponseRequired;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;

} dhcpClientRoutingEventParms_t;

typedef struct dhcpClientUIEventParms_s 
{
  L7_uint32           event;
  L7_uint32           intIfNum;
  L7_MGMT_PORT_TYPE_t mgmtPortType;
  L7_uint32           ipAddr;
  L7_uint32           netMask;
  L7_uint32           ipGateway;
  L7_BOOL             vendorClassOptionMode;
  L7_uchar8           vendorClassOptionString[DHCP_VENDOR_CLASS_STRING_MAX];

} dhcpClientUIEventParms_t;

typedef enum
{
  DHCP_CLIENT_ADDRESS_ACQUIRE = 1,
  DHCP_CLIENT_ADDRESS_RENEW,
  DHCP_CLIENT_ADDRESS_RELEASE,
  DHCP_CLIENT_ADDRESS_CONFIG,
  DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE,
  DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING,

} DHCP_CLIENT_UI_EVENT_TYPE_t;

typedef struct dhcpClientQueueMsg_s
{
  L7_uint32 msgId;

  union
  {
    L7_CNFGR_CMD_DATA_t cnfgrCmdData;
    dhcpClientRoutingEventParms_t dhcpClientRoutingEventParms;
    dhcpClientUIEventParms_t dhcpClientUIEventParms;
  } u;

} dhcpClientQueueMsg_t;

#define DHCP_CLIENT_QUEUE_NAME     "dhcpClientQueue"
#define DHCP_CLIENT_QUEUE_SIZE     64
#define DHCP_CLIENT_QUEUE_MSG_SIZE sizeof (dhcpClientQueueMsg_t)

typedef struct dhcpLeaseInfo_s
{
  L7_uint32 t1;
  L7_uint32 t2;
  L7_uint32 expiry;
  volatile L7_short16 next;
  volatile L7_short16 which;
  osapiTimerDescr_t *alarm;
  L7_short16 *dhcpState;
  L7_uint32 *configStatus;
  L7_uint32 net_t1;
  L7_uint32 net_t2;
  L7_uint32 net_expiry;
  L7_uint32 expiry_uptime;

} dhcpLeaseInfo_t;

/* This specifies the dhcp options to be added to dhcp */
typedef struct dhcpVendorClassOption_s
{
  L7_uint32 optionMode;   /* L7_ENABLE or L7_DISABLE*/
  L7_uchar8 vendorClassString[DHCP_VENDOR_CLASS_STRING_MAX+1];

}dhcpVendorClassOption_t;

typedef struct dhcpOptions_s
{
  dhcpVendorClassOption_t vendorClassOption;  /* This is updated from UI configuration.*/
  dhcpVendorSpecificOption_t vendorSpecificOption;  /* This is updated from the server replies.*/

} dhcpOptions_t;

/* Structure involves pointer to notify callback function and componentId assigned to it */
typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
  L7_uint32  requestedOptionsMask;    /* Mask holds offered options IDs. Set mask using OR operation
                                         with flags defined in L7_bootp_dhcp_OfferedOptions */
  L7_RC_t (*notifyFunction)(L7_bootp_dhcp_t *networkParamsPtr);

} dhcpNotifyList_t;

typedef struct dhcpClientQueue_s
{
  void        *queuePtr;
  L7_uchar8   queueName[32];
  L7_uint32   queueSize;
  L7_uint32   queueMsgSize;
  L7_uint32   queueSendFailedCnt;
  L7_uint32   queueRxSuccessCnt;
  void        *queueRecvBuffer;
  void        *queueCountersSem;

}dhcpClientQueue_t;

struct bootp
{
  unsigned char    bp_op;     /* packet opcode type */
  unsigned char    bp_htype;      /* hardware addr type */
  unsigned char    bp_hlen;     /* hardware addr length */
  unsigned char    bp_hops;     /* gateway hops */
  unsigned int     bp_xid;      /* transaction ID */
  unsigned short   bp_secs;     /* seconds since boot began */
  unsigned short   bp_flags;      /* RFC1532 broadcast, etc. */
  struct in_addr   bp_ciaddr;     /* client IP address */
  struct in_addr   bp_yiaddr;     /* 'your' IP address */
  struct in_addr   bp_siaddr;     /* server IP address */
  struct in_addr   bp_giaddr;     /* gateway IP address */
  unsigned char    bp_chaddr[BP_CHADDR_LEN];  /* client hardware address */
  char       bp_sname[BP_SNAME_LEN];  /* server host name */
  char       bp_file[BP_FILE_LEN];  /* boot file name */
  unsigned char    bp_vend[BP_VEND_LEN];  /* vendor-specific area */
  /* note that bp_vend can be longer, extending to end of packet. */
};

/* structure that contains the last received bootp packet
 * from the server along with the server's ip address */
struct bootp_serverinfo
{
  /* NOTE: bootp_struct has to be the first field in this structure
   * since it would be typecasted with bootp_serverinfo
   * and bootp structures in the client code */
  struct bootp   bootp_struct;
  struct in_addr server_ip;
};

/* structure that contains the last received bootp packet
 * from the server along with the server's ip address */
typedef struct dhcpClientRecvdInfo_s
{
  /* NOTE: bootp_struct has to be the first field in this structure
   * since it would be typecasted with bootp_serverinfo
   * and bootp structures in the client code */
  struct bootp rxBuf;
  struct in_addr offeredIpAddr; /* Offered Client address from OFFER message */
  struct in_addr server_ip;

} dhcpClientRecvdInfo_t;

typedef struct timeout_state_s
{
  L7_uint32 secs;
  L7_int32 countdown;
} timeout_state_t;

/* Dhcp Client Information pertaining to an Interface */
typedef struct dhcpClientInfo_s
{
  L7_BOOL inUse;                    /* Whether DHCP Client operational on this
                                     * interface */
  L7_uint32 sockFD;                 /* DHCP Client's Socket FD - used for packet
                                     * reception and transmission */
  L7_uint32 intIfNum;               /* Internal Interface Number */
  L7_uint32 rtrIfNum;               /* Router Interface Number associated with intIfNum */
  L7_MGMT_PORT_TYPE_t mgmtPortType; /* Management Port Type */
  L7_uchar8 intfName[IFNAMSIZ];     /* Interface Name */
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN]; /* Interface MAC Address */
  L7_uint32 transID;                  /* Transaction ID on this Interface */
  L7_BOOL actImmediate;               /* For Synchronous Release mechanism */
  dhcpClientRecvdInfo_t dhcpInfo;     /* Tx Packet */
  struct bootp txBuf;
  dhcpLeaseInfo_t leaseInfo;        /* DHCP Lease Timer information */
  L7_DHCP_CLIENT_STATE_t dhcpState; /* DHCP Client's current state */
  L7_uint32 retryCount;             /* DHCP Request message retransmission count */
  L7_sockaddr_in_t rxAddr;
  osapiTimerDescr_t *nextTimeout;
  L7_uint32 timeout_random;
  struct timeval tv;
  timeout_state_t timeout_scratch;
  L7_uint32 bytesRcvd;
  L7_uint32 dhcpSelectingStateCounter;
  L7_uint32 dhcpReqRecvStateCounter;
  L7_uint32 dhcpRenewRecvStateCounter;
  L7_uint32 dhcpRebindRecvStateCounter;

  /* This is defying the RFC.  In scenarios where an interface is
   * configured for DHCP, by the time the 8-unit stack's interfaces
   * come up the DHCP timeout mechanism for sending DISCOVERs is complete.
   * As a result, the DUT never gets an address even though the interface
   * is configured for DHCP.  So, run the timeout mechanism again to get
   * an address.  The number of trails is defined by
   * DHCP_CLIENT_NUM_ADDRESS_TRAILS.
   */
  L7_uint32 numTrails;

} dhcpClientInfo_t;

typedef struct dhcpClientCB_s
{
  /* DHCP Client Information pertained to an Interface */
  dhcpClientInfo_t *clientIntfInfo[DHCP_CLIENT_MAX_INTERFACES+1];

  /* Protection for clientIntfInfo */
  void *clientIntfInfoSem;

  /* Control Block members protection semaphore.
   * Protection for synchornous Release mechanism and 
   * dhcpOptions, for now.
   */
  void *cbMembersProtectionSem;

  /* Client's Component Notify List */
  dhcpNotifyList_t *dhcpNotifyList;

  /* Protection for notifyList */
  void *notifyListSem;

  /* DHCP Client's Task ID */
  L7_int32 dhcpClientTaskId;

  /* DHCP Client's Message Queue */
  dhcpClientQueue_t dhcpClientMsgQ;

  /* DHCP Vendor Options given by the DHCP Server.
   * User configuration of client options. Only persistent on products 
   * using text based config. */
  dhcpOptions_t dhcpOptions;

  /* Used by NSF */
  L7_BOOL dhcpcWarmRestart;

} dhcpClientCB_t;

extern dhcpClientCB_t dhcpClientCB;

/* DHCP Packet Types */
#define DHCPDISCOVER 1
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPDECLINE  4
#define DHCPACK      5
#define DHCPNAK      6
#define DHCPRELEASE  7
#define DHCPINFORM   8

/* DHCP interface state machine states; these are published so that app
   code can know what to do... (see page 35 of RFC2131)*/

/* The values for these states must be different from the values used above.
 * Even though the state machines themselves are independent, there is 
 * common code (like in L7_dhcp()) that looks at the DHCP state.  */
#define DHCP_INFORM_REQUEST_PHASE  21 /* Inform DHCP server we've got our statically configured
                                          IP address, give us the rest of options */
#define DHCP_INFORM_ACK_WAIT_PHASE 22 /* wait for and handle DHCP server ACK reply */       
#define DHCP_INFORM_FAILED_PHASE   23 /* failed during resolving DHCP options */
#define DHCP_INFORM_BOUND_PHASE    24 /* we've successfully got our options */         

#define CONFIG_NONE    0
#define CONFIG_SET    1
#define CONFIG_CHANGE   2

#define DHCP_SERVICE_PORT 0
#define DHCP_NETWORK_PORT 1
/* These we dont use
  #define DHCPSTATE_INITREBOOT
  #define DHCPSTATE_REBOOTING

   These are to let the app inspect the state of the interfaces when
   managing them itself, by analogy with eth0_up &c; eth0_bootp_data and so
   on will still be used with DHCP.*/
extern L7_short16   l7_eth0_dhcpstate;
extern L7_short16   l7_eth1_dhcpstate;  



/* Shutdown any interface which is not already shut down - whether
   initialized by DHCP or not.  Reason: because of the broadcast-while-not-
   -fully-initialized nature of the DHCP conversation, all other interfaces
   must be shut down during that.  So one down, all down, is required.*/
extern L7_int32 dhcp_halt( void );


/* ---------------------------------------------------------------------------
   These are rather more internal routines, internal to the protocol engine
   in dhcp_prot.c - those above are in dhcp_support.c */


/* IP packet TTL number used in requests*/
#define DHCP_IP_TTL_NUMBER 255

/* structure covers one DHCP option*/
struct bp_option
{
  L7_uchar8 tag;
  L7_uchar8 dataLen;
  L7_uchar8 data;
};

typedef enum
{
  DHCP_OPTION_OVERLOAD_BOOTFILE = 1,
  DHCP_OPTION_OVERLOAD_SNAME,
  DHCP_OPTION_OVERLOAD_SNAME_BOOTFILE
} dhcpOptionOverload;


/*
 * UDP port numbers, server and client.
 */
#define IPPORT_BOOTPS  67
#define IPPORT_BOOTPC  68

#define BOOTREPLY  2
#define BOOTREQUEST  1

/*
 * Hardware types from Assigned Numbers RFC.
 */
#define HTYPE_ETHERNET    1
#define HTYPE_EXP_ETHERNET   2
#define HTYPE_AX25    3
#define HTYPE_PRONET    4
#define HTYPE_CHAOS    5
#define HTYPE_IEEE802    6
#define HTYPE_ARCNET    7

/*
 * Vendor magic cookie (v_magic) for CMU
 */
#define VM_CMU  "CMU"

/*
 * Vendor magic cookie (v_magic) for RFC1048
 */
#define VM_RFC1048 { 99, 130, 83, 99 }



/*
 * Tag values used to specify what information is being supplied in
 * the vendor (options) data area of the packet.
 */
/* RFC 1048 */
/* End of cookie */
#define TAG_END   ((unsigned char) 255)

/* padding for alignment */
#define TAG_PAD   ((unsigned char)   0)

/* Subnet mask */
#define TAG_SUBNET_MASK  ((unsigned char)   1)

/* Time offset from UTC for this system */
#define TAG_TIME_OFFSET  ((unsigned char)   2)

/* List of routers on this subnet */
#define TAG_GATEWAY  ((unsigned char)   3)

/* List of rfc868 time servers available to client */
#define TAG_TIME_SERVER  ((unsigned char)   4)

/* List of IEN 116 name servers */
#define TAG_NAME_SERVER  ((unsigned char)   5)

/* List of DNS name servers */
#define TAG_DOMAIN_SERVER ((unsigned char)   6)

/* List of MIT-LCS UDL log servers */
#define TAG_LOG_SERVER  ((unsigned char)   7)

/* List of rfc865 cookie servers */
#define TAG_COOKIE_SERVER ((unsigned char)   8)

/* List of rfc1179 printer servers (in order to try) */
#define TAG_LPR_SERVER  ((unsigned char)   9)

/* List of Imagen Impress servers (in prefered order) */
#define TAG_IMPRESS_SERVER ((unsigned char)  10)

/* List of rfc887 Resourse Location servers */
#define TAG_RLP_SERVER  ((unsigned char)  11)

/* Hostname of client */
#define TAG_HOST_NAME  ((unsigned char)  12)

/* boot file size */
#define TAG_BOOT_SIZE  ((unsigned char)  13)

/* RFC 1395 */
/* path to dump to in case of crash */
#define TAG_DUMP_FILE  ((unsigned char)  14)

/* domain name for use with the DNS */
#define TAG_DOMAIN_NAME  ((unsigned char)  15)

/* IP address of the swap server for this machine */
#define TAG_SWAP_SERVER  ((unsigned char)  16)

/* The path name to the root filesystem for this machine */
#define TAG_ROOT_PATH  ((unsigned char)  17)

/* RFC 1497 */
/* filename to tftp with more options in it */
#define TAG_EXTEN_FILE  ((unsigned char)  18)

/* RFC 1533 */
/* The following are in rfc1533 and may be used by BOOTP/DHCP */
/* IP forwarding enable/disable */
#define TAG_IP_FORWARD          ((unsigned char)  19)

/* Non-Local source routing enable/disable */
#define TAG_IP_NLSR             ((unsigned char)  20)

/* List of pairs of addresses/masks to allow non-local source routing to */
#define TAG_IP_POLICY_FILTER    ((unsigned char)  21)

/* Maximum size of datagrams client should be prepared to reassemble */
#define TAG_IP_MAX_DRS          ((unsigned char)  22)

/* Default IP TTL */
#define TAG_IP_TTL              ((unsigned char)  23)

/* Timeout in seconds to age path MTU values found with rfc1191 */
#define TAG_IP_MTU_AGE          ((unsigned char)  24)

/* Table of MTU sizes to use when doing rfc1191 MTU discovery */
#define TAG_IP_MTU_PLAT         ((unsigned char)  25)

/* MTU to use on this interface */
#define TAG_IP_MTU              ((unsigned char)  26)

/* All subnets are local option */
#define TAG_IP_SNARL            ((unsigned char)  27)

/* broadcast address */
#define TAG_IP_BROADCAST        ((unsigned char)  28)

/* perform subnet mask discovery using ICMP */
#define TAG_IP_SMASKDISC        ((unsigned char)  29)

/* act as a subnet mask server using ICMP */
#define TAG_IP_SMASKSUPP        ((unsigned char)  30)

/* perform rfc1256 router discovery */
#define TAG_IP_ROUTERDISC       ((unsigned char)  31)

/* address to send router solicitation requests */
#define TAG_IP_ROUTER_SOL_ADDR  ((unsigned char)  32)

/* list of static routes to addresses (addr, router) pairs */
#define TAG_IP_STATIC_ROUTES    ((unsigned char)  33)

/* use trailers (rfc893) when using ARP */
#define TAG_IP_TRAILER_ENC      ((unsigned char)  34)

/* timeout in seconds for ARP cache entries */
#define TAG_ARP_TIMEOUT         ((unsigned char)  35)

/* use either Ethernet version 2 (rfc894) or IEEE 802.3 (rfc1042) */
#define TAG_ETHER_IEEE          ((unsigned char)  36)

/* default TCP TTL when sending TCP segments */
#define TAG_IP_TCP_TTL          ((unsigned char)  37)

/* time for client to wait before sending a keepalive on a TCP connection */
#define TAG_IP_TCP_KA_INT       ((unsigned char)  38)

/* don't send keepalive with an octet of garbage for compatability */
#define TAG_IP_TCP_KA_GARBAGE   ((unsigned char)  39)

/* NIS domainname */
#define TAG_NIS_DOMAIN  ((unsigned char)  40)

/* list of NIS servers */
#define TAG_NIS_SERVER  ((unsigned char)  41)

/* list of NTP servers */
#define TAG_NTP_SERVER  ((unsigned char)  42)

/* and stuff vendors may want to add */
#define TAG_VEND_SPECIFIC       ((unsigned char)  43)

/* NetBios over TCP/IP name server */
#define TAG_NBNS_SERVER         ((unsigned char)  44)

/* NetBios over TCP/IP NBDD servers (rfc1001/1002) */
#define TAG_NBDD_SERVER         ((unsigned char)  45)

/* NetBios over TCP/IP node type option for use with above */
#define TAG_NBOTCP_OTPION       ((unsigned char)  46)

/* NetBios over TCP/IP scopt option for use with above */
#define TAG_NB_SCOPE            ((unsigned char)  47)

/* list of X Window system font servers */
#define TAG_XFONT_SERVER        ((unsigned char)  48)

/* list of systems running X Display Manager (xdm) available to this client */
#define TAG_XDISPLAY_SERVER     ((unsigned char)  49)

/* While the following are only allowed for DHCP */
/* DHCP requested IP address */
#define TAG_DHCP_REQ_IP         ((unsigned char)  50)

/* DHCP time for lease of IP address */
#define TAG_DHCP_LEASE_TIME     ((unsigned char)  51)

/* DHCP options overload */
#define TAG_DHCP_OPTOVER        ((unsigned char)  52)

/* DHCP message type */
#define TAG_DHCP_MESS_TYPE      ((unsigned char)  53)

/* DHCP server identification */
#define TAG_DHCP_SERVER_ID      ((unsigned char)  54)

/* DHCP ordered list of requested parameters */
#define TAG_DHCP_PARM_REQ_LIST  ((unsigned char)  55)

/* DHCP reply message */
#define TAG_DHCP_TEXT_MESSAGE   ((unsigned char)  56)

/* DHCP maximum packet size willing to accept */
#define TAG_DHCP_MAX_MSGSZ      ((unsigned char)  57)

/* DHCP time 'til client needs to renew */
#define TAG_DHCP_RENEWAL_TIME   ((unsigned char)  58)

/* DHCP  time 'til client needs to rebind */
#define TAG_DHCP_REBIND_TIME    ((unsigned char)  59)

/* DHCP class identifier */
#define TAG_DHCP_CLASSID        ((unsigned char)  60)

/* DHCP client unique identifier */
#define TAG_DHCP_CLIENTID       ((unsigned char)  61)

/* TFTP server name */
#define TAG_TFTP_SERVER_NAME    ((unsigned char)  66)

/* configuration file name */
#define TAG_BOOT_FILENAME       ((unsigned char)  67)

/* TFTP server address */
#define TAG_TFTP_SERVER         ((unsigned char)  150)


/* Following definitions have been picked up from Red Hat/eCos network.h file*/

extern L7_short16
l7_get_bootp_option(struct bootp *bp, L7_uchar8 tag, void *opt, L7_uint32 optlen);

/*functions defined in dhcp_support.c */
void
dhcpClientTask (void);

L7_int32 dhcp_bind( void );

extern L7_uint32 dhcpcDebugMsgLevel;
extern L7_uint32 errNo;
extern L7_char8 *pErrString;

/*********************************************************************
* @purpose  Notification Registration 
*
* @param    L7_uint32  component_ID   @b((input)) componant id
* @param    L7_uint32  (*notify)(L7_bootp_dhcp_t)   @b((input))  function to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dhcpNotificationRegister( L7_COMPONENT_IDS_t component_ID, 
                                  L7_uint32 requestedOptionsMask,
                                  L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr));

/*********************************************************************
* @purpose Deregister the routine to be called when a DHCP request completes.
*
* @param   L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*                                 
* @end
*
*********************************************************************/
L7_RC_t dhcpNotificationDeregister(L7_COMPONENT_IDS_t componentId);

/*********************************************************************
* @purpose  Call registered users with information 
*
* @param L7_bootp_dhcp_t networkParamsPtr @b((input)) pointer to network parameters structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void dhcpNotifyRegisteredUsers(L7_bootp_dhcp_t *networkParamsPtr);

/**************************************************************************
* @purpose  Get the notification semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dhcpNotificationSemGet (void);

/**************************************************************************
* @purpose  Give the notification semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dhcpNotificationSemFree (void);

L7_uint32 dhcpConfiguredServPortAddrGet(void);
L7_uint32 dhcpConfiguredNetworkAddrGet(void);
L7_uint32 dhcpRequestedServPortAddrGet(L7_uint32 prevIpAddr);
L7_uint32 dhcpRequestedNetworkAddrGet(L7_uint32 prevIpAddr);

L7_RC_t
dhcpClientDiscoverInitiate (L7_uint32 intIfNum,
                            L7_MGMT_PORT_TYPE_t mgmtPortType);

L7_RC_t
dhcpClientRenewInitiate (L7_uint32 intIfNum,
                         L7_MGMT_PORT_TYPE_t mgmtPortType);

L7_RC_t
dhcpClientReleaseProcess (L7_uint32 intIfNum,
                           L7_MGMT_PORT_TYPE_t mgmtPortType);

L7_RC_t
dhcpClientImmediateReleaseInitiate (dhcpClientInfo_t *clientIntfInfo);

L7_RC_t
dhcpClientUIEventPost (dhcpClientQueueMsg_t *msg);

L7_RC_t
doDhcp (dhcpClientInfo_t *clientIntfInfo);

dhcpClientInfo_t* 
dhcpClientIntfInfoGet (L7_uint32 intIfNum,
                       L7_MGMT_PORT_TYPE_t mgmtPortType);

dhcpClientInfo_t*
dhcpClientIntfInfoByRtrIfNumGet (L7_uint32 rtrIfNum,
                                 L7_MGMT_PORT_TYPE_t mgmtPortType);

void
dhcpClientMessageReceive (fd_set *readFds);

L7_RC_t
dhcpClientIPAddressUpdate (dhcpClientInfo_t *clientIntfInfo);

L7_RC_t
dhcpClientConfiguredIpAddressProcess (L7_MGMT_PORT_TYPE_t mgmtPortType,
                                      L7_uint32 ipAddr,
                                      L7_uint32 netMask,
                                      L7_uint32 ipGateway);

L7_RC_t
dhcpClientIntfInfoDeInit (dhcpClientInfo_t *clientIntfInfo);

L7_RC_t
dhcpVendorClassOptionStringUpdate (L7_uchar8 *optionString);

L7_RC_t
dhcpVendorClassOptionAdminModeUpdate (L7_BOOL mode);

L7_RC_t
getParamsFromDhcpStruct(struct bootp *dhcpParams, L7_bootp_dhcp_t *networkParams);

void
dhcpClientServiceAndNetworkPortStatusCheck (void);

L7_RC_t
dhcpClientSocketCreate (L7_uint32 intIfNum,
                        dhcpClientInfo_t *clientIntfInfo,
                        L7_MGMT_PORT_TYPE_t mgmtPortType);

void
reset_timeout (dhcpClientInfo_t *clientIntfInfo);

L7_int32
next_timeout (dhcpClientInfo_t *clientIntfInfo);

void no_lease (dhcpLeaseInfo_t *lease);

L7_BOOL
dhcpClientMgmtPortTypeIsOperational (L7_uint32 intIfNum,
                                     L7_MGMT_PORT_TYPE_t mgmtPortType);
void
show_bootp (const L7_char8 *intf, struct bootp *bp);

void
dhcpDebugPacketRxTxTrace (dhcpClientInfo_t *clientIntfInfo, struct bootp *bp,
                          L7_BOOL rxTrace, L7_uint32 pktlen);

L7_RC_t
dhcpClientAllIntfInfoCleanup (void);

L7_RC_t
dhcpClientCleanupIntfInfoByAddress (L7_uint32 ipAddr);

void
dhcpClientSocketClose (dhcpClientInfo_t *clientIntfInfo);

#endif 

