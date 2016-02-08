/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_util.h
*
* @purpose   Declarations and data structures for captive portal 
*            wired interface owner.
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/25/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#ifndef _WIO_UTIL_H_
#define _WIO_UTIL_H_


#include "l7_common.h"
/* pick up structure to define callbacks with CP */
#include "intf_cb_api.h"
#include "avl_api.h"
#include "sysnet_api.h"
#include "l7_cnfgr_api.h"


/* Maximum number of characters in a trace message. */
#define WIO_MAX_TRACE_LEN 255

/* incl NULL terminator */
#define WIO_MAC_STR_LEN 18    

/* Physical ports are the only wired interfaces where captive portal can be enabled. */
#define WIO_MAX_INTERFACES  L7_MAX_PORT_COUNT

/* Maximum number of connections we can intercept and track, across all clients on
 * all interfaces. */
#define WIO_MAX_INTERCEPTED_CONNECTIONS  512

/* Maximum number of clients. Let all CP clients be wired. */
#define WIO_MAX_CLIENTS  CP_CLIENT_CONN_STATUS_MAX

/* Max number of bytes in pseudo packet used to recompute TCP checksum on 
 * redirected packets. Pseudo header is 12 bytes; so TCP segment must be 
 * <= 2036 bytes. Most will be < 1500 bytes. Might have trouble if a client
 * uses a larger segment size. */
#define WIO_TCP_PSEUDO_PKT_LEN 2048 

/* Size of buffer on receive queue for incoming ARP or IP packet. Will only 
 * handle frames this length or shorter (incl. ethernet header). */
#define WIO_MAX_FRAME_SIZE 1522

typedef enum
{
  WIO_PHASE_INIT_0 = 0,
  WIO_PHASE_INIT_1,
  WIO_PHASE_INIT_2,
  WIO_PHASE_WMU,
  WIO_PHASE_INIT_3,
  WIO_PHASE_EXECUTE,
  WIO_PHASE_UNCONFIG_1,
  WIO_PHASE_UNCONFIG_2,
} wioCnfgrState_t;

/* Authentication states for wired client */
typedef enum 
{
  WIO_CLIENT_UNAUTH = 0,
  WIO_CLIENT_PENDING,
  WIO_CLIENT_AUTH
} e_WioClientState;

/* Interface state for captive portal */
typedef enum
{
  WIO_INTF_STATE_DISABLED = 0,
  WIO_INTF_STATE_ENABLED
} e_WioIntfState;

/* Blocking state of an interface */
typedef enum
{
  WIO_INTF_UNBLOCKED = 0,
  WIO_INTF_BLOCKED
} e_WioIntfBlockState;

/* Events that change the state of captive portal on a wired interface */
typedef enum
{
  WIO_INTF_EVENT_DISABLE = 0,
  WIO_INTF_EVENT_ENABLE
} e_WioIntfEvent;

/* Block/unblock actions */
typedef enum
{
  WIO_INTF_BLOCK = 0,
  WIO_INTF_UNBLOCK
} e_WioIntfBlock;

/* Distinguish among the addional TCP ports that the user can configure
 * CP to intercept. */
typedef enum 
{
  WIO_PORT_FLAVOR_HTTP = 0,
  WIO_PORT_FLAVOR_HTTPS_1,   /* port learned through callback wioCpSetAuthSecurePort1Num */
  WIO_PORT_FLAVOR_HTTPS_2    /* port learned through callback wioCpSetAuthSecurePort2Num */
} e_WioOptPortFlavor;

typedef enum
{
  /* client is authenticated */
  WIO_CLIENT_EVENT_AUTH = 0,

  /* client is not yet authenticated (but is probably in progress) */
  WIO_CLIENT_EVENT_UNAUTH,

  /* client has been de-authenticated (and is not reauthenticating yet) */
  WIO_CLIENT_EVENT_DEAUTH

} e_WioAuthEvent;

typedef enum
{
  /* Trace changes to the client list. */
  WIO_TRACE_CLIENT_LIST = 0x1,

  /* Trace changes to intercepted connections list. */
  WIO_TRACE_CONN_LIST = 0x2,

  /* Trace changes to authentication server's IP address */
  WIO_TRACE_AUTH_SERVER = 0x4,

  /* Trace changes to interface block state */
  WIO_TRACE_BLOCK = 0x8,

  /* Trace component initialization */
  WIO_TRACE_INIT = 0x10,

  /* Captive portal state on interfaces */
  WIO_TRACE_CP_STATE = 0x20

} e_WioTraceFlags;

/* Message types that distinguish work items recieved on this thread's 
 * message queue. */
typedef enum
{
  WIO_CP_INTF_STATE = 0,
  WIO_CP_BLOCK,
  WIO_CP_AUTH,
  WIO_AUTH_PORT,
  WIO_CNFGR_INIT
} e_WioMsgType;

/* used to compute a TCP checksum */
typedef struct psuedo_header_s
{
  L7_uint32   source_address;
  L7_uint32   dest_address;
  L7_uchar8   place_holder;
  L7_uchar8   protocol;
  L7_ushort16 length;
} pseudo_header_t;

typedef struct wioCpEnableMsg_s
{
  /* whether CP enabled or disabled on this interface */
  e_WioIntfEvent cpEvent;

  /* Which interface */
  L7_uint32 intIfNum;

  /* IP address of local authentication server */
  L7_uint32 authServer;

} wioCpEnableMsg_t;

typedef struct wioBlockMsg_s
{
  /* whether interface is blocked or unblocked */
  e_WioIntfBlock block;

  /* which interface */
  L7_uint32 intIfNum;

} wioBlockMsg_t;

typedef struct wioClientAuthMsg_s
{
  /* Whether client is authenticated or not */
  e_WioAuthEvent auth;

  /* client's MAC address */
  L7_enetMacAddr_t clientMac;

} wioClientAuthMsg_t;

typedef struct wioAuthPortMsg_s
{
  /* which interface */
  L7_uint32 intIfNum;

  /* http or https */
  e_WioOptPortFlavor portFlavor;

  /* optional TCP port number to be used for authentication */
  L7_ushort16 tcpPortNum;

} wioAuthPortMsg_t;


typedef struct
{
  e_WioMsgType msgType;

  union
  {
    L7_CNFGR_CMD_DATA_t  cmdData;
    wioCpEnableMsg_t     cpEnableMsg;
    wioBlockMsg_t        blockMsg;
    wioClientAuthMsg_t   clientAuthMsg;
    wioAuthPortMsg_t     authPortMsg;
  } wioMsgData;

} wioEventMsg_t;

typedef struct wioFrameMsg_s
{
  /* Receive interface. Internal interface number. */
  L7_uint32 rxIntf;

  /* VLAN on which message was received */
  L7_ushort16 vlanId;

  /* Frame length */
  L7_uint32 dataLen;

  /* Entire ethernet frame. Don't want to hold mbuf on msg queue. */
  L7_uchar8 frameBuf[WIO_MAX_FRAME_SIZE];

} wioFrameMsg_t;

/* Global statistics useful for debugging */
typedef struct wioDebugStats_s
{
  /* Could not get a connection buffer for a new connection */
  L7_uint32 outOfConnBuffers;

  /* We tried to redirect a packet to the local auth server, but 
   * no auth server address is set. */
  L7_uint32 noAuthServerAddr;

  /* TCP packet is larger than the pseudo packet buffer used to 
   * recompute a TCP checksum */
  L7_uint32 messageTooBig;

  /* message queue high water mark */
  L7_uint32 msgQHighWater;

  /* Frame queue high water mark */
  L7_uint32 frameQHighWater;

  /* Message queue for incoming frames is full */
  L7_uint32 frameMsgQFull;

  /* Message queue semaphore given, but nothing on the queue! */
  L7_uint32 msgQRxError;

  /* Failed to add a new client to client list. Maybe list is full. */
  L7_uint32 clientAddFailed;

  /* Failed to remove a client from client list. Error? */
  L7_uint32 clientRemoveFailed;

  /* Client moved from one interface to another */
  L7_uint32 clientMoves;

  /* Number of outbound packets intercepted */
  L7_uint32 packetsOut;

  /* Output buffer is too short to contain eth + IP + TCP header */
  L7_uint32 outBufferTooSmall;

  /* sysnet gave us non-IP outgoing packet */
  L7_uint32 nonIpPacketsOut;

  /* Number of outbound TCP packets intercepted */
  L7_uint32 tcpOut;

  /* Failure forwarding and ethernet frame */
  L7_uint32 frameTxFailures;

  /* Couldn't get an mbuf for tx */
  L7_uint32 mbufFailures;

  /* Ethernet frame encapsulation failed */
  L7_uint32 encapsFailed;

  /* Number of times a remote address changed on an existing connection */
  L7_uint32 remoteAddrChange;
  
} wioDebugStats_t;

/* Interface statistics useful for debugging */
typedef struct wioIntfDebugStats_s
{
  /* Number of inbound packets intercepted on this interface. Only counted
   * if CP is enabled on this interface. Includes blocked packets. */
  L7_uint32 packetsIn;

  /* Number of inbound ARP packets intercepted on this interface. Only counted
   * if CP is enabled on this interface. */
  L7_uint32 arpPacketsIn;

  /* ARP packets received from an authenticated client. These are ignored. */
  L7_uint32 arpPacketsAuthClient;

  /* Intercepted ARP packets whose destination MAC address is local.  */
  L7_uint32 arpPacketsLocal;

  /* Intercepted ARP packets to be forwarded */
  L7_uint32 arpPacketsForward;

  /* Intercepted ARP packets that are discarded */
  L7_uint32 arpPacketsDiscarded;

  /* Number of intercepted ARP packets too long for message queue */
  L7_uint32 frameTooLong;

  /* Number of outbound packets intercepted on this interface. Only counted
   * if CP is enabled on this interface. Includes blocked packets. */
  L7_uint32 packetsOut;

  /* Modification of outgoing packet to client failed. */
  L7_uint32 failedToModSrcAddr;

  /* Number of incoming packets dropped because interface in blocked state */
  L7_uint32 packetsInBlocked;

  /* Number of outgoing packets dropped because interface in blocked state */
  L7_uint32 packetsOutBlocked;

  /* Number of clients reported to CP on this interface */
  L7_uint32 newClients;

  /* Number of packets passed from unauth clients (DHCP, DNS, etc) */
  L7_uint32 unauthPacketsPassed;

  /* Number of packets dropped from unauth clients */
  L7_uint32 unauthPacketsDropped;

  /* Number of packets redirected to auth server */
  L7_uint32 redirectedPacketsIn;

  /* Number of outbound packets whose source address was changed to 
   * that of the client's original correspondent. */
  L7_uint32 sourceAddrChange;

  /* Ignored incoming packet with invalid source IP address. */
  L7_uint32 invalidSrcAddr;

  /* Failed to add client MAC to fdb */
  L7_uint32 fdbAddFailure;

  /* Failed to remove client MAC from fdb */
  L7_uint32 fdbDelFailure;
  
} wioIntfDebugStats_t;


/* A captive portal client on a wired interface. */
typedef struct wioClient_s
{
  /* Client's MAC address. Must be the first member of the structure, since 
   * this is used as the key in the client AVL tree. */
  L7_enetMacAddr_t clientMacAddr;

  /* client's IP address (host byte order) */
  L7_uint32 clientIpAddr;

  /* client's authentication state */
  e_WioClientState clientState;

  /* Internal interface number of interface where client's traffic arrives at 
   * the switch. */
  L7_uint32 intIfNum;

  /* VLAN the client is in */
  L7_uint32 clientVlan;

  void *next;    /* RESERVED FOR AVL USE */

} wioClient_t;

typedef struct wioClientList_s
{
  L7_uint32           maxClients;
  avlTreeTables_t     *treeHeap;
  L7_uint32           treeHeapSize;
  void                *dataHeap;   /* wioClient_t* */
  L7_uint32           dataHeapSize;
  avlTree_t           treeData;
} wioClientList_t;


/* Structure describing a client TCP connection that has been intercepted. intIfNum
 * clientIpAddr, and clientTcpPort form key for hash list and must be kept together. */
typedef struct wioClientConn_s
{
  /* links used by hash list */
  struct wioClientConn_s *next;
  struct wioClientConn_s *prev;

  /* Internal interface number where client packets arrive at switch */
  L7_uint32 intIfNum;

  /* Client's IP address */
  L7_uint32 clientIpAddr;

  /* TCP port number on client side of connection (host byte order) */
  L7_ushort16 clientTcpPort;

  /* remote address client thinks it is talking to (host byte order) */
  L7_uint32 remoteIpAddr;
  
} wioClientConn_t;



/* Interface data for wired interface owner */
typedef struct wioIntfInfo_s
{
  /* Interface state for captive portal */
  e_WioIntfState wioIntfState;

  /* Whether the interface is administratively blocked */
  e_WioIntfBlockState wioIntfBlock;

  /* L7_TRUE if the user has configured an optional web server port on this
   * interface. I suppose we could use webServerPort == 0 to indicate unset 
   * since a TCP port number cannot be 0. So I guess we have "belt and
   * suspenders" here as Jeff R. says. */
  L7_BOOL webServerPortSet;

  /* Optional http web server port configured by user. These packets are treated 
   * like port 80 (http) for triggering CP authentication. Value is 0 if unconfigured */
  L7_ushort16 webServerPort;

  /* User can also configure two additional https ports. Packets whose 
   * destination port number matches these are treated like packets to 
   * the the standard https port, 443. Value is 0 if unconfigured. */
  L7_ushort16 secureWebServerPort1;
  L7_ushort16 secureWebServerPort2;

  /* The set of CP capabilities is probably the same for all physical ports (and thus
   * could live on wioInfo). But we may add support for other types of wired interfaces
   * (such as VLAN routing interfaces). So go ahead and stick this here. Bit mask indexed
   * by L7_INTF_PARM_CP_TYPES_t. */
  L7_uint32 capabilities;

  wioIntfDebugStats_t debugStats;

} wioIntfInfo_t;


/* Global data for wired interface owner */
typedef struct wioInfo_s
{
  /* The wireless interface owner code is run by multiple threads. Packets are 
   * intercepted on incoming and outgoing threads. The captive portal component
   * invokes some WIO callbacks. So we need a lock to synchronize access to 
   * this component's data. Probably don't have many cases where we could have 
   * multiple simultaneous readers. So we'll just use a simple binary semaphore
   * and avoid the overhead of a read write lock. */
  void *wioLock;

  /* Counting semaphore to indicate when messages are available on either
   * message queue. */
  void *msgQSema;

  /* message queue for WIO's own thread */
  void *wioMsgQ;

  /* message queue for ethernet frames to be switched */
  void *wioFrameMsgQ;

  /* Configuration init state for this component */
  wioCnfgrState_t cnfgrState;

  /* IP address of authentication server (host byte order) */
  L7_uint32 authServerAddr;

  /* Internal interface number of the local interface with the IP address
   * used by the auth server. Only non-zero if the auth server is using 
   * an IP address on a routing interface. */
  L7_uint32 authServerIntf;

  /* captive portal clients list. This list includes clients in the process of
   * becoming authenticated and those that have been authenticated. Those in 
   * process are on this list and on the pending client list. This list is 
   * expected to be much larger than the pending clients list, and is therefore
   * maintained in an AVL tree. A client's MAC address uniquely identifies the
   * client, and is the index to the tree. */
  wioClientList_t wioClientList;

  /* Hash list of clients, indexed by IP address. Every entry in wioClientList 
   * has a corresponding entry in wioClientIpList, so a client can be looked up 
   * either by IP address or by MAC address. */
  void *wioClientIpList;

  /* List of client connections hijacked for authentication. */
  void *wioClientConnList;

  /* Interface data. Indexed by intIfNum. + 1 because intIfNum of first physical
   * port is 1, not 0. */
  wioIntfInfo_t wioIntfInfo[WIO_MAX_INTERFACES + 1];

  /* Set of callback functions registered with captive portal */
  intfCpCallbacks_t *wioCallbacks;

  /* Buffer pool for intercepted connections list. */
  L7_uint32 connPoolId;

  /* For ovall system efficiency, we would rather not register our sysnet 
   * intercept functions unless one or more wired interfaces are enabled
   * for captive portal. So keep track here of whether we are registered. */
  L7_BOOL isRegWithSysnet;

  /* Bit mask to enable debug tracing. Bits indexed by e_wioTraceFlags. */
  L7_uint32 wioTraceFlags;

  /* Global debug statistics */
  wioDebugStats_t debugStats;

} wioInfo_t;


/* wio_util.c */

/* Access functions */
L7_uint32 wioAuthServerAddrGet(void);
L7_RC_t wioAuthServerAddrSet(L7_uint32 authServer);
wioIntfInfo_t *wioIntfInfoGet(L7_uint32 intIfNum);
L7_BOOL wioIntfIsEnabled(L7_uint32 intIfNum);
L7_BOOL wioIntfIsBlocked(L7_uint32 intIfNum);
L7_BOOL wioCpIsActive(void);
L7_ushort16 wioOptWebServerPortGet(L7_uint32 intIfNum);
L7_ushort16 wioOptSecureWebServerPort1Get(L7_uint32 intIfNum);
L7_ushort16 wioOptSecureWebServerPort2Get(L7_uint32 intIfNum);
L7_RC_t wioOptWebServerPortSet(L7_uint32 intIfNum, L7_ushort16 tcpPortNum, 
                               e_WioOptPortFlavor flavor);
L7_BOOL wioIntfIsCapable(L7_uint32 intIfNum, L7_INTF_PARM_CP_TYPES_t capability);
L7_uint32 wioIntfCapabilities(L7_uint32 intIfNum);
L7_RC_t wioDefaultCapabilitiesSet(void);

/* TCP/IP utilities */
L7_ushort16 wioCheckSum(L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum);
L7_RC_t wioUpdateTcpChecksum(L7_ipHeader_t *ipHeader);
L7_BOOL wioSourceIpAddressInvalid(L7_uint32 srcIpAddr);
L7_ushort16 wioIpHdrLen(L7_ipHeader_t *ipHeader);
L7_RC_t wioIpPacketInfoGet(L7_ipHeader_t *ipHeader, 
                           L7_uchar8 *protocol, L7_ushort16 *destPort);

/* packet processing */
L7_BOOL wioPassUnauthClientPacket(L7_ipHeader_t *ipHeader);
L7_BOOL wioRedirectPacketType(L7_uint32 intIfNum, L7_ipHeader_t *ipHeader);
L7_RC_t wioPacketRedirect(L7_enetHeader_t *ethHeader, L7_ipHeader_t *ipHeader, 
                          sysnet_pdu_info_t *pduInfo);
L7_RC_t wioModifySourceAddress(wioClientConn_t *clientConn, 
                               L7_netBufHandle bufHandle);


/* Set interface and client state */
L7_RC_t wioIntfEnable(L7_uint32 intIfNum, L7_uint32 authServer);
L7_RC_t wioIntfDisable(L7_uint32 intIfNum);
L7_RC_t wioIntfBlock(L7_uint32 intIfNum);
L7_RC_t wioIntfUnblock(L7_uint32 intIfNum);
L7_RC_t wioClientAuth(L7_enetMacAddr_t *clientMac);
L7_RC_t wioClientUnauth(L7_enetMacAddr_t *clientMac);
L7_RC_t wioClientDeauth(L7_enetMacAddr_t *clientMac);
L7_RC_t wioUpdateClientConnList(wioClient_t *client,
                                L7_uint32 intIfNum, L7_ipHeader_t *ipHeader);

/* misc */
L7_BOOL wioIsValidIntf(L7_uint32 intIfNum);


/* wio_sysnet.c */
L7_RC_t wioSysnetRegister(void);
L7_RC_t wioSysnetDeregister(void);


/* wio_debug.c */
L7_RC_t wioTraceWrite(L7_uchar8 *wioTrace);
void wioIntfStatsClear(L7_uint32 intIfNum);

/* wio_client_conn.c */
L7_RC_t wioClientConnListCreate(void);
L7_RC_t wioClientConnListDelete(void);
L7_RC_t wioClientConnAdd(L7_uint32 intIfNum, wioClient_t *client,
                         L7_ushort16 clientTcpPort, L7_uint32 remoteIpAddr);
L7_RC_t wioClientConnectionsRemove(L7_uint32 intIfNum, L7_uint32 clientIpAddr);
wioClientConn_t *wioClientConnFind(L7_uint32 intIfNum, L7_uint32 clientIpAddr,
                                   L7_ushort16 clientTcpPort);
L7_uint32 wioConnRemoteAddrGet(L7_uint32 intIfNum, L7_uint32 clientIpAddr,
                               L7_ushort16 clientTcpPort);
void _wioConnListShow(L7_uint32 intIfNum);


/* wio_client_lsit.c */
L7_RC_t wioClientListCreate(void);
L7_RC_t wioClientListDelete(void);
wioClient_t *wioClientAdd(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                          L7_uint32 intIfNum, L7_uint32 vlanId);
L7_RC_t wioClientRemove(L7_enetMacAddr_t *macAddr);
L7_RC_t wioClientsOnIntfRemove(L7_uint32 intIfNum);
L7_RC_t wioClientFind(L7_enetMacAddr_t *macAddr, L7_uint32 matchType, 
                      wioClient_t **client);
wioClient_t *wioClientFindByIpAddr(L7_uint32 ipAddr);
L7_RC_t wioClientStateSet(L7_enetMacAddr_t *macAddr, e_WioClientState newState);
void _wioClientListShow(void);

/* wio_main.c */
void wioTask(void);
SYSNET_PDU_RC_t wioPacketInterceptIn(L7_uint32 hookId,
                                     L7_netBufHandle bufHandle,
                                     sysnet_pdu_info_t *pduInfo,
                                     L7_FUNCPTR_t continueFunc);

SYSNET_PDU_RC_t wioArpIntercept(L7_uint32 hookId,
                                L7_netBufHandle bufHandle,
                                sysnet_pdu_info_t *pduInfo,
                                L7_FUNCPTR_t continueFunc);

SYSNET_PDU_RC_t wioPacketInterceptOut(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t continueFunc);

L7_RC_t wioFrameForward(L7_uint32 intIfNum, L7_ushort16 vlanId,
                        L7_uchar8 *frame, L7_uint32 dataLen);
L7_RC_t wioFrameFlood(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_uint32 dataLen);
L7_RC_t wioFrameUnicast(L7_uint32 intIfNum, L7_ushort16 vlanId,
                        L7_uchar8 *frame, L7_uint32 dataLen);
L7_RC_t wioFrameSend(L7_uint32 intIfNum, L7_ushort16 vlanId, 
                     L7_uchar8 *frame, L7_ushort16 frameLen);
L7_BOOL wioDestMacIsLocal(L7_enetMacAddr_t *destMac, 
                          L7_uint32 intIfNum, L7_ushort16 vlanId);
L7_RC_t wioPacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                       sysnet_pdu_info_t *pduInfo);


#endif /* _WIO_UTIL_H_ */

