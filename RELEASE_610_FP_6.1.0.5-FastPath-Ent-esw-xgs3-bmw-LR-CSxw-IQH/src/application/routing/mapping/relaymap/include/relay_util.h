/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  dhcpRelay_util.h
*
* @purpose   dhcp relay utilities header
*
* @component DHCP_RELAY  Layer
*
* @comments  none
*
* @create    11/27/2001
*
* @author    rrice - May 2008 revision
*
* @end
*
**********************************************************************/


#ifndef _DHCP_RELAY_UTIL_H
#define _DHCP_RELAY_UTIL_H

#ifdef _L7_OS_VXWORKS
#include "l7_common.h"
#include "types.h"
#endif

#include "l3_comm_structs.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "l7_cnfgr_api.h"


#define DHCP_RELAY_QUEUE          "IP_Helper_Fwd_Queue"

/* Used to queue incoming messages. Allow a burst. If we overflow, 
 * assume sender will resend. */
#define DHCP_RELAY_MSG_COUNT      100

/* This application doesn't reassemble fragmented packets. So the max
 * IPv4 packet size is limited by the max L2 frame size. Not bothering 
 * to reduce this by the L2 header length. */
#define IH_PACKET_POOL_BUF_LEN  L7_MAX_FRAME_SIZE

#define DHCP_UDP_PSEUDO_PACKET_LEN  IH_PACKET_POOL_BUF_LEN 

/* Making this fairly small to conserve memory */
#define IH_PACKET_POOL_BUF_NUM  25

#define DHCP_HARDWARE_ADDR_MAXLEN  16
#define DHCP_SNAME_LEN  64
#define DHCP_FILE_LEN 128
#define DHCP_HDR_LEN    236
#define DHCP_OPTION_LEN (L7_L3_MAX_IP_MTU - (L7_IP_HDR_LEN + L7_UDP_HDR_LEN + DHCP_HDR_LEN))
#define DHCP_BUFFER_LEN L7_L3_MAX_IP_MTU 


#define BOOTPREQUEST    1
#define BOOTPREPLY      2

#define BOOTPC      68
#define BOOTPS      67

/* First bit 1 and all others 0 in the 16 bit field */
#define BOOTP_BROADCAST 0x8000

#define DHO_DHCP_AGENT_OPTIONS    82
#define DHO_DHCP_MESSAGE_TYPE   53
#define RAI_CIRCUIT_ID              1
#define DHO_PAD                 0
#define DHO_END               255
#define DHO_CIRCUIT_ID_OPTION_LEN     11
#define DHCP_OPTION_LEN_FIELD_INDEX   1
#define DHCP_OPTION_SUBOPTION_OFFSET  2

#define DHCP_SUB_OPTION_TYPE_1_LENGTH 7
#define DHCP_CIRCUIT_ID_TYPE          0
#define DHCP_CIRCUIT_ID_LENGTH        5

#define DHCP_UNIT_SUBOPTION_VALUE_OFFSET  8
#define DHCP_SLOT_SUBOPTION_VALUE_OFFSET  9
#define DHCP_PORT_SUBOPTION_VALUE_OFFSET  10

/* Max number of characters in an IP helper trace message */
#define IH_TRACE_LEN_MAX 512

/* Debug trace flags */
typedef enum 
{
    /* Trace NIM events processed by IP helper */
    IH_TRACE_NIM_EVENTS = 0x1,

    /* Packet trace */
    IH_TRACE_PACKET = 0x2,

} ihTraceFlag_t;

typedef struct pseudo_header_s
{
  L7_uint32   source_address;
  L7_uint32   dest_address;
  L7_uchar8   place_holder;
  L7_uchar8   protocol;
  L7_ushort16 length;
} pseudo_header_t;

typedef struct ihStats_s
{
  /* Bundle stats reported in UI to avoid separate APIs for every stat. */
  ipHelperStats_t publicStats;

  /* ------------------------------------------------------------------------------------- */
  /* Stats above this line are available in UI. Below line are available through devshell. */
  /* ------------------------------------------------------------------------------------- */

  /* Number of times we failed to enqueue a packet to our thread's msg queue. */
  L7_uint32 pktEnqueueFailed;   

  /* Received a DHCP server packet whose giaddr is not a local address */
  L7_uint32 unknownGiaddr;

  /* Received DHCP client message on interface with no IP address. No giaddr. */
  L7_uint32 noLocalIpAddress;

  /* Received a DHCP server message whose destination IP address is not ours. */
  L7_uint32 dhcpServerMsgToUnknownAddr;

  /* Client message either matches a discard entry or there is no server configured. */
  L7_uint32 noServer;

  /* Socket create failed */
  L7_uint32 noSocket;

  /* Socket bind failed */
  L7_uint32 socketBindFailed;

  /* Socket send failed */
  L7_uint32 socketSendFailed;

  /* Number of BOOTP messages relay to client */
  L7_uint32 bootpMsgToClient;

  /* Number of packets looked at on sysnet intercept. */
  L7_uint32 packetsIntercepted;

  /* Packets intercepted but destination IP address is not this router. */
  L7_uint32 notToMe;

  /* Packet buffer bool empty */
  L7_uint32 noPacketBuffer;

  /* Number of packets queued to udp relay thread */
  L7_uint32 packetsQueued;

  /* Message too long for current implementation */
  L7_uint32 msgTooBig;

  /* Failed to allocated mbuf */
  L7_uint32 mbufFailures;

  /* No IP address on outgoing interface to client */
  L7_uint32 noIpAddrOnOutIf;

} ihStats_t;

typedef struct dhcpRelayInfo_s
{
    /* indicates whether DHCP_RELAY is initialized or not.*/
    L7_BOOL         dhcpRelayInitialized;
    L7_uint32       dhcpRelayTaskId;

    /* L7_TRUE if sysnet intercept currently registered. */
    L7_BOOL regWithSysnet;

  /* IP helper code is run by multiple threads. Packets are 
   * intercepted on the IP MAP forwarding thread. Configuration changes 
   * are made on UI threads. Other DHCP components may call us to transfer
   * packets in. IP helper has its own thread. The server configuation
   * is maintained in an AVL tree with linked lists (lots of pointers). 
   * So we need a lock to synchronize access to this component's data. Probably 
   * don't have many cases with multiple simultaneous readers. So just use a simple 
   * binary semaphore and avoid the overhead of a read write lock. */
    void *ihLock;

    /* Buffer pool for lists of server addresses */
    L7_uint32 serverPoolId;

    /* Buffer pool to hold IP packets on thread message queue */
    L7_uint32 packetPoolId;

    ihStats_t ihStats;

    ihTraceFlag_t ihTraceFlags;

    /* configuration data has changed since last save */
    L7_BOOL         dataChanged;

} dhcpRelayInfo_t;

typedef struct
{
    L7_uchar8      op;      /* 0: Message opcode/type */
    L7_uchar8      htype;   /* 1: Hardware addr type, ethernet -1 */
    L7_uchar8      hlen;    /* 2: Hardware addr length ethernet - 6*/
    L7_uchar8      hops;    /* 3: Number of relay agent hops from client */
    L7_uint32      xid;     /* 4: Transaction ID - 4 Bytes*/
    L7_ushort16    secs;    /* 8: Sec */
    L7_ushort16    flags;   /* 10: Flag bits */
    L7_IP_ADDR_t   ciaddr;  /* 12: Client IP address */
    L7_IP_ADDR_t   yiaddr;  /* 16: Client IP address */
    L7_IP_ADDR_t   siaddr;  /* 18: IP address of next server to talk to */
    L7_IP_ADDR_t   giaddr;  /* 20: DHCP relay agent IP address */
    L7_uchar8      chaddr [DHCP_HARDWARE_ADDR_MAXLEN];    /* 24: Client hardware address */
    L7_uchar8      sname [DHCP_SNAME_LEN]; /*  Server name */
    L7_uchar8      file [DHCP_FILE_LEN];   /*  Boot filename */
    L7_uchar8      options [DHCP_OPTION_LEN];

}L7_dhcp_packet;


typedef enum {
    DHCPRELAYMAP_FIRST_MSG_ID,
    DHCPRELAYMAP_RELAY_PKT,
    DHCPRELAYMAP_CNFGR,
    DHCPRELAYMAP_RESTORE_EVENT,
    DHCPRELAYMAP_INTF_EVENT,
    DHCPRELAYMAP_LAST_MSG_ID
 }DHCPRELAYMAP_MSG_ID_t;

typedef struct udpRelayPkt_s
{
  /* ingress routing interface */
  L7_uint32 intIfNum;   

  /* ingress physical port */
  L7_uint32 rxPort;

  /* ingress VLAN ID. May be needed for DHCP circuit ID suboption. */
  L7_uint32 vlanId;

  /* buffer containing the packet, starting at the IPv4 header */
  L7_ipHeader_t *pktBuffer;

} udpRelayPkt_t;

typedef struct dhcpRelayIntfEventMsg_s
{
  L7_uint32 intIfNum;
  L7_uint32           event;          /* one of L7_PORT_EVENTS_t from NIM */
  NIM_CORRELATOR_t    correlator;
} dhcpRelayIntfEventMsg_t;

typedef struct udpRelayMsg_s
{
    L7_uint32       msgId;

    union
    {
        L7_CNFGR_CMD_DATA_t     cnfgrCmdData;
        dhcpRelayIntfEventMsg_t intfEvent;
        udpRelayPkt_t relayPacket;
    }type;
} udpRelayMsg_t;

#if 0    /* ROBRICE */
struct psuedohdr_s
{
    L7_uint32   source_address;
    L7_uint32   dest_address;
    L7_uchar8   place_holder;
    L7_uchar8   protocol;
    L7_ushort16 length;
};
#endif

extern void *dhcpRelayQueue;

L7_RC_t ihAdminModeApply(L7_uint32 mode);

/*********************************************************************
* @purpose  Relay a UDP packet
*
* @param    ipPkt     @b{(input)}   Start of IPv4 packet header
* @param    intIfNum  @b{(input)}   ingress routing interface
* @param    rxPort    @b{(input)}   ingress physical port
* @param    vlanId    @b{(input)}   ingress VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t relayUdpPacket(L7_ipHeader_t *ipPkt, L7_uint32 intIfNum, 
                       L7_uint32 rxPort, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Adds DHCP relay agent options when sending to server
*
* @param    pDhcpPacket      @b{(input)} Pointer to Dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    intIfNum         @b{(input)} ingress interface
* @param    rxPort           @b{(input)} ingress physical port
* @param    vlanId           @b{(input)} ingress VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihAddDhcpRelayOptions(L7_dhcp_packet* pDhcpPacket, 
                              L7_uint32 *dhcpPacketLength, 
                              L7_uint32 intIfNum, L7_uint32 rxPort,
                              L7_uint32 vlanId);

/*********************************************************************
* @purpose  Sends a a dhcp Pakcet after building IP and UDP headers to
*           a Broadcast IP and Broadcast Ethernet address on the specified
*     interface
*
* @param    intIfnum       @b{(input)} Interface num. on which data to be sent
* @param    clientPort     @b{(input)} physical port where client is attached
* @param    vlanId           @b{(input)} If client on a VLAN routing interface, 
*                                        the interface's VLAN ID. 0 otherwise. 
* @param    dhcpPacket     @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketBroadcast(L7_uint32 intIfNum, L7_uint32 clientPort, 
                            L7_uint32 vlanId, L7_dhcp_packet* dhcpPacket, 
                            L7_uint32 dhcpPacketLength);

/*********************************************************************
* @purpose  Sends a a dhcp Pakcet after building IP and UDP headers to
*           a unicast IP and unicast Ethernet address on the specified
*                             interface
*
* @param    intIfnum             @b{(input)} Interface num. on which data to be sent
* @param    clientPort     @b{(input)} physical port where client is attached
* @param    vlanId           @b{(input)} If client on a VLAN routing interface, 
*                                        the interface's VLAN ID. 0 otherwise. 
* @param    dhcpPacket           @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketUnicast(L7_uint32 intIfNum, L7_uint32 clientPort, 
                          L7_uint32 vlanId, L7_dhcp_packet* dhcpPacket, 
                          L7_uint32 dhcpPacketLength);

/*********************************************************************
* @purpose  Builds dhcp packet by adding UDP and IP headers
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    dhcpPacket       @b{(input)} Pointer to the DHCP packet
* @param    dhcpPacketLength @b{(input)} length of the DHCP packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The start of dhcpPacket is at least 28 bytes from the start
*           of the buffer from the buffer pool; therefore, we can add 
*           new IP and UDP headers in place w/o copying to a temp buffer.
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketBuild(L7_uint32 intIfNum, L7_dhcp_packet *dhcpPacket, 
                        L7_uint32 dhcpPacketLength);

/*********************************************************************
* @purpose  Builds IP header
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    dhcpPacket       @b{(input/output)} start of DHCP packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    destAddr         @b{(input)} dest IP addr (unicast or broadcast)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketIpHeaderBuild(L7_uint32 intIfNum, L7_uchar8 *dhcpPacket,
                                L7_uint32 dhcpPacketLength, L7_uint32 destAddr);

/*********************************************************************
* @purpose  Builds UDP header
*
* @param    srcAddr          @b{(input)} source IP address
* @param    destAddr         @b{(input)} dest. IP address
* @param    srcPort          @b{(input)} source UDP port number
* @param    destPort         @b{(input)} destination UDP port number
* @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the DHCP Packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ihUdpHeaderBuild(L7_uint32 srcAddr, L7_uint32 destAddr, 
                         L7_ushort16 srcPort, L7_ushort16 destPort, 
                         L7_uchar8 *dhcpPacket, L7_uint32 dhcpPacketLength);

/*********************************************************************
* @purpose  Calculates checksum on a buffer
*
* @param    addr       @b{(input)} data buffer
* @param    len        @b{(input)} length of data
* @param    csum     @b{(input)} checksum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 dhcpCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum);

/*********************************************************************
* @purpose  Checks if DHCP relay  config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL dhcpRelayHasDataChanged(void);
void dhcpRelayResetDataChanged(void);

/*********************************************************************
* @purpose  Get the client's interface from the DHCP relay agent option
*           circuit ID suboption and strip option 82 from the packet.
*
* @param    packet           @b{(input)} DHCP packet
* @param    dhcpPacketLength @b{(input)} length of the DHCP Packet
* @param    intIfNum         @b{(output)} interface number where client is attached
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayOption82Strip(L7_dhcp_packet *packet, 
                               L7_uint32 *dhcpPacketLength, 
                               L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Saves DHCP relay config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t dhcpRelaySave(void);

/*********************************************************************
* @purpose  Restores DHCP relay config file to factory defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void dhcpRelayRestoreProcess(void);

/*********************************************************************
* @purpose  Builds default DHCP relay  config data
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpRelayBuildDefaultConfigData(L7_uint32);


/*********************************************************************
* @purpose  Applies relay config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayApplyConfigData(void);

/*********************************************************************
* @purpose  The DHCP relay Task
*
* @param
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

void udpRelay_Task(void);

/*********************************************************************
* @purpose  Is packet a valid DHCP Relay destination?
*
* @param    destAddr      destination IP address
*
* @returns  L7_TRUE       If a valid destination
* @returns  L7_FALSE      Otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL bootpDhcpRelayIsValidDest(L7_uint32 destAddr);

/*********************************************************************
* @purpose  Consume BOOTPS frames
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if frame has been consumed; stop processing it
* @returns  SYSNET_PDU_RC_IGNORED   if frame has been ignored; continue processing it
*
* @notes    none
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t ihUdpRelayIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc);

L7_RC_t ihUdpPacketQueue(L7_ipHeader_t *ipHeader, L7_uint32 ipLen, 
                         L7_uint32 intIfNum, L7_uint32 rxPort, L7_uint32 vlanId);

L7_RC_t ihUdpMessageForward(L7_ipHeader_t *ipPkt, L7_uint32 destAddr,
                            L7_uint32 intIfNum, L7_uint32 rxPort, L7_uint32 vlanId);

/* relay_migrate.c */
/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void dhcpRelayMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/*********************************************************************
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum   internal interface number
* @param    L7_uint32  event      event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dhcpRelayIntfChangeCallback(L7_uint32 intIfNum, 
                                    L7_uint32 event, 
                                    NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Process interface-related events
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes    L7_PORT_ROUTING_ENABLED and L7_PORT_ROUTING_DISABLED events
*           (other than updating information to reflect the interface
*           being acquired/released by IP map for routing)
*           are not handled by this routine, as these events originate
*           from inside the IP Mapping layer. Since the router discovery mapping layer
*           is a "slave" to the IP Mapping layer, these events are
*           handled via its registration with IP map.
*
* @notes    Gets invoked asynchronously. nimEventStatusCallback() is invoked to
*           to tell NIM the event has been handled.
*
* @end
*********************************************************************/

L7_RC_t dhcpRelayIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Determine if the interface type is valid for IPv4
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dhcpRelayIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid for participation
*           in the component.
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dhcpRelayIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Initializes the helper addresses list
*
* @param   pCfg  pointer to interface configuration information.
*
* @returns void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpRelayIntfBuildDefaultConfigData(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dhcpRelayIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dhcpRelayIntfDelete(L7_uint32 intIfNum);

L7_BOOL ihDestPortOnDefaultList(L7_uint32 udpPort);
L7_RC_t ihUdpPortToString(L7_ushort16 udpPort, L7_uchar8 *portString);
L7_RC_t ihPacketPoolCreate(void);
L7_RC_t ihPacketPoolDelete(void);
void ihTraceWrite(L7_uchar8 *traceMsg);


/* relay_svr_tree.c */
L7_RC_t ihServerAddrAdd(ihRelayEntry_t *relayEntry, L7_uint32 serverAddr);
L7_RC_t ihServerAddrDelete(ihRelayEntry_t *relayEntry, L7_uint32 serverAddr);
L7_uint32 ihServerAddrListNext(ihRelayEntry_t *relayEntry, L7_uint32 prevServer);
L7_BOOL ihServerListEmpty(ihRelayEntry_t *relayEntry);
L7_RC_t ihRelayEntryListCreate(void);
L7_RC_t ihRelayEntryListDelete(void);
L7_RC_t ihRelayEntryAdd(L7_uint32 intIfNum, L7_ushort16 udpPort,
                        L7_BOOL discard, L7_uint32 serverAddr);
L7_RC_t ihRelayEntryDelete(L7_uint32 intIfNum, L7_ushort16 udpPort,
                           L7_BOOL discard, L7_uint32 serverAddr);
L7_RC_t ihGlobalRelayEntriesClear(void);
L7_RC_t ihRelayEntriesClear(L7_uint32 intIfNum);
L7_uint32 ihServerAddrGetNext(L7_uint32 intIfNum, L7_ushort16 udpPort,
                              L7_uint32 prevServer, L7_BOOL count);
L7_RC_t ihIntfEntryGetFirst(L7_uint32 intIfNum, L7_ushort16 *udpPort, L7_uint32 *serverAddr,
                            L7_BOOL *discard, L7_uint32 *hitCount);
L7_RC_t ihIntfEntryGetNext(L7_uint32 intIfNum, L7_ushort16 *udpPort, L7_uint32 *serverAddr,
                            L7_BOOL *discard, L7_uint32 *hitCount);
L7_RC_t ihIntfEntryGet(L7_uint32 intIfNum, L7_ushort16 udpPort, L7_uint32 serverAddr,
                       L7_BOOL discard, L7_uint32 *hitCount);
L7_RC_t ihGlobalEntryGet(L7_ushort16 udpPort, L7_uint32 serverAddr, L7_uint32 *hitCount);
L7_uint32 ihRelayEntryCount(void);
L7_BOOL ihRelayEntriesValid(void);
void _relayEntryListShow(void);


#endif
