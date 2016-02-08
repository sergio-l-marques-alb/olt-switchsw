/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   dhcps_util.h
 *
 * @purpose    DHCP Server Implementation
 *
 * @component  DHCP Server Component
 *
 * @comments   none
 *
 * @create     09/15/2003
 *
 * @author     athakur
 * @end
 *
 **********************************************************************/
#ifndef _DHCPS_UTIL_H_
#define _DHCPS_UTIL_H_ 

#include "l7_common.h"
#include "dhcps_config.h"

#ifdef _L7_OS_VXWORKS
#include "sysnet_api.h"
#include "types.h"
#endif


#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "avl_api.h"          /* for AVL Tree support */

/*
 ** DHCP server queue and message handling macros 
 */
#define DHCPS_OFFER_VALIDITY_DURATION_SECS 300 /* offer validity duration in seconds */
#define DHCPS_INFINITE_LEASE_TIME          0xffffffff /* infinite lease time (for BootP only) */
#define DHCPS_QUEUE_WAIT_INTERVAL          60000 /* 1 minute timeout for polling scheduled leases */
#define DHCPSMAP_QUEUE                      "dhcpsMap_Fwd_Queue"
#define DHCPS_BUFFER_LEN                   L7_IP_HDR_LEN + L7_UDP_HDR_LEN + L7_DHCP_PACKET_LEN
#define DHCPS_MAX_OPTION_LEN               255 + 128 + 64 -6 /*Option size + Sname field + file field - 2* ( Type + length field)*/
#define DHCPS_OPTION_FIELD_LENGTH          L7_DHCP_PACKET_LEN -1  /* 1 byte is for end option*/
#define DHCPS_MAX_UNDIVIDED_OPT_LENGTH     255
#define DHCPS_OVERFLOW_OPT_OVERHEAD        4 /*First Overload field + 1 byte for end field*/
#define DHCPS_FIELD_USED_COST        3     /* 2 Byte for TL option and 1 byte for end option*/
#define DHCPS_OVERFLOW_OPT_LENGTH    3     /* 3 Byte TLV*/
#define DHCPS_NOT_OVERLOADED             0
#define DHCPS_FILE_OVERLOADED        1
#define DHCPS_SNAME_OVERLOADED       2
#define DHCPS_BOTH_OVERLOADED        3
#define DHCPS_BOTH_CAN_BE_OVERLOADED 4

#define DHCPS_SET_CONFIG_DATA_DIRTY        {pDhcpsMapCfgData->cfgHdr.dataChanged = L7_TRUE;} while(0);
#define DHCPS_SET_LEASE_CONFIG_DATA_DIRTY  {pDhcpsMapLeaseCfgData->cfgHdr.dataChanged = L7_TRUE;} while(0);

/*
 ** DHCP server well known port numbers and broadcast address
 */
#define DHCPS_SERVER_PORT        67
#define DHCPS_CLIENT_PORT        68


/*
 ** DHCP header related defines for this implementation
 */
#define DHCPS_HW_TYPE                    1
#define DHCPS_HW_TYPE_10B                1
#define DHCPS_HW_TYPE_IEEE802            6
#define DHCPS_SNAME_LEN                  64
#define DHCPS_FILE_LEN                   128
#define DHCPS_BOOTP_PKT_LENGTH           300
#define DHCPS_SINGLE_BYTE_OPT_LENGTH     1

/*
 ** The message types defined in the DHCP specification
 */
#define DHCPS_BOOTPREQUEST                 1
#define DHCPS_BOOTPREPLY                   2
#define DHCPS_MESSAGE_TYPE_START           DHCPS_MESSAGE_TYPE_DISCOVER
#define DHCPS_MESSAGE_TYPE_END             DHCPS_MESSAGE_TYPE_INFORM

#define DHCPS_MESSAGE_TYPE_DISCOVER        1
#define DHCPS_MESSAGE_TYPE_OFFER           2
#define DHCPS_MESSAGE_TYPE_REQUEST         3
#define DHCPS_MESSAGE_TYPE_DECLINE         4
#define DHCPS_MESSAGE_TYPE_ACK             5
#define DHCPS_MESSAGE_TYPE_NACK            6
#define DHCPS_MESSAGE_TYPE_RELEASE         7
#define DHCPS_MESSAGE_TYPE_INFORM          8

/* First bit 1 and all others 0 in the 16 bit field */
#define DHCPS_BOOTP_BROADCAST              32768L

/*
 ** The DHCP option codes defined in the DHCP specification
 */
#define DHCPS_OPTION_TYPE_PAD                         0
#define DHCPS_OPTION_TYPE_SUBNET_MASK                 1
#define DHCPS_OPTION_TYPE_ROUTER                      3
#define DHCPS_OPTION_TYPE_DNS_SERVER                  6
#define DHCPS_OPTION_TYPE_HOST_NAME                  12
#define DHCPS_OPTION_TYPE_DOMAINNAME                 15
#define DHCPS_OPTION_TYPE_BROADCAST_ADDR             28
#define DHCPS_OPTION_TYPE_NETBIOSNAMESERVER          44
#define DHCPS_OPTION_TYPE_NETBIOSNODETYPE            46
#define DHCPS_OPTION_TYPE_REQUEST_IP_ADDR            50
#define DHCPS_OPTION_TYPE_LEASE_TIME                 51
#define DHCPS_OPTION_TYPE_OPTION_OVERLOAD            52
#define DHCPS_OPTION_TYPE_MESSAGE                    53
#define DHCPS_OPTION_TYPE_SERVER_ID                  54
#define DHCPS_OPTION_TYPE_PARAMETER_LIST             55
#define DHCPS_OPTION_TYPE_CLIENT_ID                  61
#define DHCPS_OPTION_TYPE_BOOTFILE_NAME              67
#define DHCPS_OPTION_TYPE_AGENT_INFO                 82
#define DHCPS_OPTION_TYPE_END                       255

#define DHCPS_OPTION_VALUE_OFFSET                     2

/*
 ** DHCP magic cookie info
 */
#define DHCPS_MAGIC_COOKIE_LENGTH       4
#define DHCPS_MAGIC_COOKIE_VALUE        0x63825363


#define DHCPS_PING_MAX_RCV_LEN         MAX_ICMP_LEN
#define DHCPS_PING_SELECT_TIMEOUT      L7_DHCPS_DEFAULT_MILLISECONDS_BETWEEN_PINGS
#define DHCPS_PING_TIMEOUT             2000 /* 2 seconds for ping operation timeout */
#define DHCPS_ICMP_ECHOREPLY    0  /* Echo reply */
#define DHCPS_PING_TEST_ID 0x1234  /* ICMP ID for DHCP usage */

/*
 ** DHCP protocol header, attribute and message formats
 */

typedef struct dhcpsHeader_s
{
  L7_uchar8      op;  /* 0: Message opcode/type */
  L7_uchar8      htype;   /* 1: Hardware addr type, ethernet -1 */
  L7_uchar8      hlen;    /* 2: Hardware addr length ethernet - 6*/
  L7_uchar8      hops;    /* 3: Number of relay agent hops from client */
  L7_uint32      xid; /* 4: Transaction ID - 4 Bytes*/
  L7_ushort16    secs;    /* 8: Sec */
  L7_ushort16    flags;   /* 10: Flag bits */
  L7_IP_ADDR_t   ciaddr;  /* 12: Client IP address */
  L7_IP_ADDR_t   yiaddr;  /* 16: Client IP address */
  L7_IP_ADDR_t   siaddr;  /* 18: IP address of next server to talk to */
  L7_IP_ADDR_t   giaddr;  /* 20: DHCP relay agent IP address */
  L7_uchar8      chaddr [L7_DHCPS_HARDWARE_ADDR_MAXLEN];    /* 24: Client hardware address */
  L7_uchar8      sname [DHCPS_SNAME_LEN]; /*  Server name */
  L7_uchar8      file [DHCPS_FILE_LEN];   /*  Boot filename */
}dhcpsHeader_t;

typedef struct dhcpsOption_s
{
  L7_uchar8     type;
  L7_uchar8     length;
  L7_uchar8     value[1];
}dhcpsOption_t;

typedef struct dhcpsPackOption_s
{
  L7_uchar8     code;
  L7_uchar8     status; /* L7_DHCPS_NOT_READY, L7_DHCPS_ACTIVE */
  L7_uchar8     format; /* L7_DHCPS_NONE, .._ASCII, .._HEX, .._IP */
  L7_uint32     length;
  L7_uchar8     value[1];
} dhcpsPackOption_t;

#ifdef __mips__
#define PACKTLV_GET_LENGTH(t) ({ unsigned char *c = (unsigned char *)t; \
   c += 3; \
   ((c[0]<<24) & 0xFF000000) + ((c[1]<<16) & 0x00FF0000) + ((c[2]<<8) & 0x0000FF00) + c[3]; })
#define PACKTLV_SET_LENGTH(t,v) \
do { \
   unsigned char *c = (unsigned char *)t; \
   c += 3; \
   c[0] = ((v)>>24) & 0xFF; \
   c[1] = ((v)>>16) & 0xFF; \
   c[2] = ((v)>>8) & 0xFF; \
   c[3] = (v) & 0xFF; \
} while (0)
#else
#define PACKTLV_GET_LENGTH(t) (t)->length
#define PACKTLV_SET_LENGTH(t,v) (t)->length = v
#endif
#define PACKTLV_SIZE(x) (sizeof(dhcpsPackOption_t) + PACKTLV_GET_LENGTH(x) - 1)

typedef struct
{
  dhcpsHeader_t  header;
  L7_uchar8      magicCookie[DHCPS_MAGIC_COOKIE_LENGTH];
  L7_uchar8      options[1];
} dhcpsPacket_t;


/****************************************
 *
 *  DHCP Server Statistics data
 *
 *****************************************/

typedef struct dhcpsStatData_s
{
  /* Type of packets received from client */
  L7_uint32           numOfDhcpsDiscoverReceived;
  L7_uint32           numOfDhcpsRequestReceived;
  L7_uint32           numOfDhcpsDeclineReceived;
  L7_uint32           numOfDhcpsReleaseReceived;
  L7_uint32           numOfDhcpsInformReceived;
  L7_uint32           numOfBootpOnlyReceived;

  /* Type of packets sent to client */
  L7_uint32           numOfDhcpsOfferSent;
  L7_uint32           numOfDhcpsAckSent;
  L7_uint32           numOfDhcpsNackSent;
  L7_uint32           numOfBootpOnlySent;

  L7_uint32           numOfMalformedMessages;
  L7_uint32           numOfBootpDhcpPacketsDiscarded;
  L7_uint32           opt82CopyFailed;
} dhcpsStatData_t;

/****************************************
 *
 *  Link List Support Structures
 *
 *****************************************/

typedef struct dhcpsLink_s
{
  void                *object;
  struct dhcpsLink_s  *prev;
  struct dhcpsLink_s  *next;
} dhcpsLink_t;

/****************************************
 *
 *  Pool, Lease Node Util Structures
 *
 *****************************************/

typedef struct dhcpsLeaseNode_s
{
  struct leaseCfgData_s   *leaseData;
  struct dhcpsPoolNode_s  *parentPool;
  dhcpsLink_t             poolLeasesLink;
  dhcpsLink_t             clientLeasesLink;
  dhcpsLink_t             scheduledLeasesLink;
  L7_uint32     leaseStartTime;   /* Lease start time   */
  L7_uint32     leaseEndTime;     /* Lease end time   */

} dhcpsLeaseNode_t;

/* Structure to hold the DHCP DISCOVER messages that are asynchronously
 * processed for ping reachability detection
 * Lease in FREE_LEASE state should have been created corresponding to each message */
typedef struct dhcpsAsyncMsg_s
{
  L7_IP_ADDR_t           ipAddr;         /* The ip address being checked for reachability */
  L7_IP_ADDR_t           ipAddrShadow;   /* The copy of above ip address used for debug display */
  L7_uchar8              dhcpPacketData[L7_DHCP_PACKET_LEN]; /* cached packet */
  L7_uint32              dhcpPacketLength;  /* dhcp packet length */
  L7_uint32              intIfNum;       /* incoming interface number */
  L7_uint32              pingsIssued;    /* Number of pings issued */
  L7_uint32              pingTimeoutCnt; /* Timeout count for pinging ip, while using select */
  L7_BOOL                getNextFreeIp;  /* Try for next unreachable IP */
} dhcpsAsyncMsg_t;

typedef struct dhcpsPoolNode_s
{
  struct poolCfgData_s  *poolData;
  dhcpsLink_t           poolLeasesHead;
  dhcpsLink_t           poolsLink;
  L7_BOOL               optionFlag[L7_DHCPS_OPTION_CODE_MAX ];
} dhcpsPoolNode_t;

/****************************************
 *
 *  Client Lease Table Structures
 *
 *****************************************/

# define DHCPS_CLIENTID_KEY_TYPE   0 /* key type is chtype if key is chaddr */

typedef struct dhcpsClientLeaseKey_s
{
  L7_uchar8 keyType;
  L7_char8  keyData[L7_DHCPS_CLIENT_ID_MAXLEN];
} dhcpsClientLeaseKey_t;

typedef struct
{
  dhcpsClientLeaseKey_t key;  /* must have key first! */
  dhcpsLink_t clientLeaseNodes;
  void * next;  /* must end with next! */
} dhcpsClientLeaseNode_t;

typedef struct dhcpsClientLeaseTable_s
{
  L7_uint32                 dhcpsClientLeaseTreeEntryMax;
  avlTreeTables_t           dhcpsClientLeaseTreeHeap[L7_DHCPS_MAX_LEASE_NUMBER];
  dhcpsClientLeaseNode_t    dhcpsClientLeaseDataHeap[L7_DHCPS_MAX_LEASE_NUMBER];
  avlTree_t                 dhcpsClientLeaseTreeData;
} dhcpsClientLeaseTable_t;

/****************************************
 *
 *  DHCP Server Option data
 *
 *****************************************/

typedef struct dhcpsOptionInfo_s
{
  L7_int32 isAllowed;
# define DHCPS_OPTION_UNKNOWN     0
# define DHCPS_OPTION_NOT_ALLOWED 1
# define DHCPS_OPTION_RFC2132     2

  L7_int32 fixedLength;
  L7_int32 multipleOf;
  L7_int32 minLength;
} dhcpsOptionInfo_t;

/****************************************
 *
 *  DHCP Server Information data
 *
 *****************************************/

typedef struct dhcpsInfo_s
{
  /* indicates whether dhcps is initialized or not.*/
  dhcpsStatData_t                 dhcpsStats;
  struct dhcpsLink_s              dhcpsPoolsHead;
  struct dhcpsLink_s              dhcpsExcludedAddrsHead;
  struct dhcpsLeaseNode_s         dhcpsLeaseNodes[L7_DHCPS_MAX_LEASE_NUMBER];
  struct dhcpsLink_s              dhcpsScheduledLeaseHead;
  struct dhcpsClientLeaseTable_s  dhcpsClientLeaseTable;
  struct dhcpsOptionInfo_s        dhcpsOptionInfo[L7_DHCPS_MAX_OPTION_CODE];
  /* More dynamic data struct to be added here*/
} dhcpsInfo_t;


/* DHCP Server control context */
typedef struct dhcpsMapCtrl_s
{
  void *              dhcpsDataSemId;     /* semaphore object handle */
  L7_uint32           msgLvl;             /* debug message control */
  L7_uint32           dhcpsTaskId;
  L7_uint32           dhcpsPingTaskId;    /* Task for tx'ing and rx'ing ping packets */
  L7_BOOL             dhcpsInitialized;
} dhcpsMapCtrl_t;

/* Debug message levels */
#define DHCPS_MAP_MSGLVL_ON     0         /* use where printf is desired */
#define DHCPS_MAP_MSGLVL_HI     1
#define DHCPS_MAP_MSGLVL_MED    50
#define DHCPS_MAP_MSGLVL_LO     100
#define DHCPS_MAP_MSGLVL_OFF    10000     /* some arbitrarily large value */

/* DEBUG build only: general printf-style macro for debug messages */
#define DHCPS_MAP_PRT(mlvl, fmt, args...) \
{ \
  if (dhcpsMapCtrl_g.msgLvl >= (mlvl)) \
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, fmt, ##args); \
}

typedef struct dhcpsMapMsg_s
{
  L7_uint32       msgId;
  L7_uint32       intf;             /* interface that received the packet  */
  L7_IP_ADDR_t    destIp;           /* interface that received the packet  */
  L7_uchar8       dhcpPacketData[L7_DHCP_PACKET_LEN];
  L7_uint32       length;
} dhcpsMapMsg_t;


struct psuedohdr_s
{
  L7_uint32   source_address;
  L7_uint32   dest_address;
  L7_uchar8   place_holder;
  L7_uchar8   protocol;
  L7_ushort16 length;
};

/*
 ** Sends the response back to the requesting client.
 */
#define SendDhcpResponse(requestInfo) IpNetSetEvent(requestInfo->responseWaitEvent);

/*
 ** Extern declerations for the DHCP routines.
 */
#define DHCPS_UDP_PSUEDO_PACKET_LEN \
  sizeof(struct psuedohdr_s) + sizeof(L7_udp_header_t) + L7_DHCP_PACKET_LEN
extern void *dhcpsMap_Queue;

/*********************************************************************
 * @purpose  Initialize DHCP Server task
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsStartTasks(void);

/*********************************************************************
 * @purpose  Initialize the DHCP Server layer application
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FALIURE
 *
 * @notes    Requires that the system router ID be configured.
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapAppsInit(void);

/*********************************************************************
 * @purpose  Apply DHCP Server config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS  Config data applied
 * @returns  L7_FAILURE  Unexpected condition encountered
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsApplyConfigData(void);

/*********************************************************************
 * @purpose  Save DHCP Server user config file to NVStore
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

L7_RC_t dhcpsSave(void);

/*********************************************************************
 * @purpose  Save DHCP Server lease config file to NVStore
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

L7_RC_t dhcpsSaveLeaseCfg(void);

/*********************************************************************
 * @purpose  Restore DHCP Server user config file to factory defaults
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

L7_RC_t dhcpsRestoreProcess(void);

/*********************************************************************
 * @purpose  Check if DHCP Server user config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_BOOL dhcpsHasDataChanged(void);
void dhcpsResetDataChanged(void);

/*********************************************************************
 * @purpose  Build default DHCP Server config data
 *
 * @param    ver         @b{(input)} Software version of Config Data
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

void dhcpsBuildDefaultConfigData(L7_uint32 ver);

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
void dhcpsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/*********************************************************************
 * @purpose  Build default DHCP Server lease config data
 *
 * @param    ver         @b{(input)} Software version of Config Data
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

void dhcpsBuildDefaultLeaseConfigData(L7_uint32 ver);

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
void dhcpsMigrateLeaseConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/*********************************************************************
 * @purpose purge DHCP Server config  and pool data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE
 *
 * @notes    Messages are defined according to interest level.  Higher
 *           values generally display more debug messages.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsPurgeData(void);


/* dhcps_main.c */

/* ip send functions */
/*********************************************************************
 * @purpose  Sends a dhcp Pakcet after building IP and UDP headers to
 *           a send a raw IP and by Broadcast or Unicast Ethernet address
 *           on the specified interface (to the specified hwaddr)
 *
 * @param    intIfnum         @b{(input)} Interface num. on which data to be sent
 * @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 * @param    destIpAddr       @b{(input)} Destination IP Address on which packet is to be sent
 * @param    destPort         @b{(input)} Destination port on which packet is to be sent
 * @param    pDestMacAddr     @b{(input)} Poniter to the destination mac address
 * @param    isSysMgmtIntf    @b{(input)} indicates send on mgmt interface
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsRawPacketSend(L7_uint32 intIfNum, dhcpsPacket_t* dhcpPacket,
    L7_uint32 dhcpPacketLength, L7_IP_ADDR_t destIpAddr,
    L7_ushort16 destPort, L7_uchar8 * pDestMacAddr, 
    L7_BOOL isSysMgmtIntf);

/*********************************************************************
 * @purpose  Builds dhcp packet by adding UDP and IP headers
 *
 * @param    intIfnum         @b{(input)} Interface num. on which data to be sent
 * @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 *
 * @param    dhcpbuff         @b{(output)} buffer to contain the packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsPacketBuild(L7_uint32 intIfNum, L7_uchar8* dhcp_buff,dhcpsPacket_t* dhcpPacket, L7_IP_ADDR_t destIpAddr, L7_ushort16 destPort, L7_uint32 dhcpPacketLength);

/*********************************************************************
 * @purpose  Builds IP header
 *
 * @param    intIfnum         @b{(input)} Interface num. on which data to be sent
 * @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 *
 * @param    dhcpbuff         @b{(input/output)} buffer to contain the packet
 *
 * @returns  L7_SUCCESS
 * @retuns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsPacketIpHeaderBuild(L7_uint32 intIfNum, L7_uchar8* dhcp_buff, L7_IP_ADDR_t destIpAddr, L7_uint32 dhcpPacketLength);

/*********************************************************************
 * @purpose  Builds UDP header
 *
 * @param    srcAddr          @b{(input)} source IP address
 * @param    destAddr         @b{(input)} dest. IP address
 * @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 *
 * @param    dhcpbuff         @b{(input/output)} buffer to contain the packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsPacketUdpHeaderBuild(L7_uchar8* dhcp_buff,L7_uint32 srcAddr, L7_uint32 destAddr, L7_ushort16 destPort, dhcpsPacket_t* dhcpPacket, L7_uint32 dhcpPacketLength);

/*********************************************************************
 * @purpose  Calculates checksum on a buffer
 *
 * @param    addr         @b{(input)} data buffer
 * @param    len          @b{(input)} length of data
 * @param    csum         @b{(input)} checksum
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_ushort16 dhcpsCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum);

/* message processing fns */

/*********************************************************************
 * @purpose  Process Bootp/Dhcp request coming Server and
 *           Bootp/Dhcp reply back to Client
 *
 * @param    msg  @b{(input)} Actual Dhcp Message arrived from interface
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t processDhcpsPacket(dhcpsMapMsg_t *);

/*********************************************************************
* @purpose  Parse DHCP packet contents with options
*
* @param    pDhcpPacket       @b{(input)} Pointer to the Dhcp packet received
* @param    dhcpPacketLength  @b{(input)} Length of the Dhcp packet
* @param    isDhcpPacket      @b{(output)} If packet is bootp or dhcp
* @param    clientId          @b{(output)} Client Identifier
* @param    clientIdLen       @b{(output)} Length of client identifier
* @param    messageType       @b{(output)} Message Type
* @param    optionOverload    @b{(output)} Overload option
* @param    subnetMask        @b{(output)} Subnet Mask option
* @param    requestedIPAddr   @b{(output)} IP address option
* @param    leaseTime         @b{(output)} Requested lease time
* @param    serverID          @b{(output)} ServerID
* @param    ppParameterList   @b{(output)} parameter list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsParseDhcpPacket(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
                             L7_BOOL *isDhcpPacket, 
                             L7_uchar8 *clientId, L7_uchar8 *clientIdLen,
                             L7_uint32 *messageType, L7_uint32 *optionOverload, 
                             L7_uint32 *subnetMask, L7_uint32 *requestedIPAddr,
                             L7_uint32 *leaseTime, L7_uint32 *serverID,
                             dhcpsOption_t **ppParameterList,
                             dhcpsOption_t **option82);

/*********************************************************************
 * @purpose  Process Dhcp request coming from the client and send
 *           Dhcp reply back to Client
 *
 * @param    pDhcpPacket       @b{(input)} Pointer to the Dhcp packet received
 * @param    dhcpPacketLength  @b{(input)} Length of the Dhcp packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t processDhcpsMessages(dhcpsPacket_t* pDhcpPacket, L7_uint32 dhcpPacketLength, L7_uint32 intIfNum, L7_BOOL isBroadcast);

/*********************************************************************
 * @purpose  Create the DHCPOFFER message by populating the relevant fields
 *
 * @param    pLease            @b{(input)} Pointer to the lease node
 * @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    intIfNum          @b{(input)} Interface number
 * @param    pParameterList    @b{(input)} Option Parameter List
 * @param    option82          @b{(input)} Start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSendOfferMessage(dhcpsLeaseNode_t * pLease, dhcpsPacket_t * pDhcpPacket, 
                              L7_uint32 incomingIfNum, dhcpsOption_t *pParameterList, 
                              dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Create the DHCPACK message by populating the relevant fields
 *
 * @param    pLease            @b{(input)} Pointer to the lease node
 * @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    intIfNum          @b{(input)} Interface number
 * @param    pParameterList    @b{(input)} Option Parameter List
 * @param    option82          @b{(input)} start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSendACKMessage(dhcpsLeaseNode_t *pLease, dhcpsPacket_t *pDhcpPacket, 
                            L7_uint32 intIfNum, dhcpsOption_t *pParameterList, 
                            dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Create the DHCPACK message by populating the relevant fields
 *           for DHCPINFORM message
 *
 * @param    pPool             @b{(input)} Matching pool
 * @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    intIfNum          @b{(input)} Interface num. through which packet arrived
 * @param    pParameterList    @b{(input)} Option Parameter List
 * @param    option82          @b{(input)} start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    The sever MUST NOT set the lease time and the yiaddr field while
 *           sending ACK for DHCPINFORM message
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSendInformACKMessage(dhcpsPoolNode_t *pPool, dhcpsPacket_t *pDhcpPacket, 
                                  L7_uint32 intIfNum, dhcpsOption_t *pParameterList, 
                                  dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Create the DHCPNAK message by populating the relevant fields
 *
 * @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength  @b{(input)} Pointer to length of the Dhcp Packet
 * @param    option82          @b{(input)} start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSendNAKMessage(dhcpsPacket_t *pDhcpPacket, L7_uint32 intIfNum, 
                            dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Create and send the BOOTPREPLY by populating the relevant fields
 *
 * @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength  @b{(input)} Pointer to length of the Dhcp Packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSendBootpReply(dhcpsLeaseNode_t *pLease, dhcpsPacket_t *pDhcpPacket, L7_uint32 intIfNum);

/*********************************************************************
 * @purpose  Check if BROADCAST bit in the flags field of the packet is on
 *
 * @param    pDhcpPacket   @b{(input)} Pointer to the dhcp Packet
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_BOOL dhcpsIsBootPBroadcast(dhcpsPacket_t *pDhcpPacket);

/*********************************************************************
 * @purpose  Send Dhcp reply back to Client
 *
 * @param    pDhcpPacket      @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 * @param    intIfNum         @b{(input)} Interface Number
 * @param    isSysMgmtIntf    @b{(input)} indicates send on mgmt interface
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSendPacket(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
    L7_uint32 intIfNum, L7_BOOL isSysMgmtIntf);

/*********************************************************************
 * @purpose  Sends a dhcp Pakcet to a Unicast IP
 *
 * @param    intIfnum         @b{(input)} Interface num. on which data to be sent
 * @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsPacketUnicast(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength, L7_IP_ADDR_t destIPAddr, L7_ushort16 destPort);

/*********************************************************************
 * @purpose  Send Bootp reply back to Client
 *
 * @param    pDhcpPacket      @b{(input)} Pointer to the dhcp Packet
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 * @param    intIfNum         @b{(input)} Interface number
 * @param    offeredIP        @b{(input)} Offered IP
 * @param    isSysMgmtIntf    @b{(input)} indicates send on mgmt interface
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSendBootpReplyPacket(dhcpsPacket_t *pDhcpPacket,
    L7_uint32 dhcpPacketLength, 
    L7_uint32 intIfNum, L7_IP_ADDR_t offeredIP,
    L7_BOOL isSysMgmtIntf);

/* dhcp option processing fns */
/*********************************************************************
 * @purpose  Save the DHCP option
 *
 * @param    pLastOption      @b{(input)}  Pointer to the DHCP option
 * @param    ppMsgBuf         @b{(output)} Pointer to pointer to option
 *                                         of dhcp message
 * @param    pBufLen         @b{(output)}  Pointer to buffer length
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSaveTLVOption(dhcpsOption_t * pLastOption, L7_uchar8 * * ppMsgBuf, L7_uint32 * pBufLen);

/*********************************************************************
 * @purpose  Append value of the TLV structure to the DHCP message buffer
 *
 * @param    pOption          @b{(input/output)} Pointer to the DHCP option
 * @param    pValue           @b{(input)} Ponter to value of DHCP option
 * @param    valueLen         @b{(input)} Length of the DHCP option
 * @param    bufLen           @b{(input)} Length of the buffer
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsTLVOptionAppendValue(dhcpsOption_t * pOption, L7_uchar8 * pValue, L7_uint32 valueLen, L7_uint32 bufLen);

/*********************************************************************
 * @purpose  Add single byte option to the DHCP message buffer
 *
 * @param    optionType       @b{(input)} Type of option
 * @param    pMsg             @b{(input)} Poninter to Message Structure
 * @param    pBufLen          @b{(input/output)} Pointer to length of the buffer
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsAddSingleByteOption(L7_uchar8 optionType, dhcpsPacket_t *pMsg, L7_uint32 * pBufLen);

/*********************************************************************
 * @purpose  Add Option to the DHCP message buffer
 *
 * @param    pMsg             @b{(output)} Pointer to dhcp packet
 * @param    bufLen           @b{(output)} length of the buffer
 * @param    optionType       @b{(input)} Type of DHCP option
 * @param    pValue           @b{(input)} Ponter to value of DHCP option
 * @param    valueLen         @b{(input)} Length of the DHCP option
 * @param    pOverloadState   @b{(output)} To indicate option overload type possible
 * @param    clntOverloadSupported   @b{(output)} If client support overload
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsAddTLVOption(dhcpsPacket_t* pMsg, L7_uint32* pBufLen, L7_uchar8 optionType, L7_uchar8 * pValue, L7_uint32 valueLen, L7_uchar8  * pOverloadStatei, L7_BOOL clntOverloadSupported);
/*********************************************************************
 * @purpose  To add the Overload option
 *
 * @param    pMsg             @b{(input)} Poninter to Message Structure
 * @param    pBufLen          @b{(input/output)} Pointer to length of the buffer
 * @param    overloadState    @b{(output)} To indicate option overload type possible
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL dhcpsAddOverloadOption(dhcpsPacket_t *pMsg, L7_uint32 * pBufLen, L7_uchar8 overloadState );

/*********************************************************************
* @purpose  To add the Overload option
*
* @param    option82      @b{(input)}  pointer to option 82 in client packet
* @param    response      @b{(in/out)} Pointer to length of the buffer
* @param    responseLen   @b{(output)} length of response packet (bytes)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    If client packet includes option 82, copy option verbatim to 
*           response. RFC 3046 says this should be the last option in the
*           response, and that it cannot go in the overloaded sname or 
*           file fields. 
*
* @end
*********************************************************************/
L7_RC_t dhcpsOption82Copy(dhcpsOption_t *option82, dhcpsPacket_t *response, 
                          L7_uint32 *responseLen);

/*********************************************************************
 * @purpose  To add fixed Options in the message. 
 *
 * @param    pPool         @b{(input)} Pointer to the pool node
 * @param    pMsg          @b{(input)} Pointer to Message Buffer
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 * @param    optionType       @b{(input)} Type of DHCP option
 * @param    pOverloadState     @b{(output)} To indicate option overload type possible
 * @param    clientOverload   @b{(output)} Client supports split or not
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE   - In case of critical error.
 * @returns  L7_ERROR     - If unable to add value in the packet.
 * @returns  L7_NOT_EXIST - If code is not part of fixed option
 *
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsAddFixedOption(dhcpsPoolNode_t * pPool,
    dhcpsPacket_t *pMsg,
    L7_uint32 *dhcpPacketLength,
    L7_uchar8  optionType,
    L7_uchar8  *pOverloadState,
    L7_BOOL clientOverload);

/*********************************************************************
 * @purpose  To add an option when option has been stored in packed format.
 *
 * @param    pPool         @b{(input)} Pointer to the pool node
 * @param    pMsg          @b{(input)} Pointer to Message Buffer
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 * @param    optionType       @b{(input)} Type of DHCP option
 * @param    pOverloadState     @b{(output)} To indicate option overload type possible
 * @param    clientOverload   @b{(output)} Client supports split or not
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE   - In case of critical error.
 * @returns  L7_ERROR     - If unable to add value in the packet.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsAddPackedOption(dhcpsPoolNode_t * pPool,
    dhcpsPacket_t *pMsg,
    L7_uint32 *dhcpPacketLength,
    L7_uchar8  optionType,
    L7_uchar8  *pOverloadState,
    L7_BOOL clientOverload);

/*********************************************************************
 * @purpose  Add Options to the DHCP message buffer
 *
 * @param    pPool         @b{(input)} Pointer to the pool node
 * @param    pMsg          @b{(input)} Pointer to Message Buffer
 * @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
 * @param    pParameterList   @b{(input)} Option Parameter List
 * @param    pOverloadState     @b{(output)} To indicate option overload type possible
 * @param    clientOverload   @b{(output)} Client supports split or not
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsAppendOptions(dhcpsPoolNode_t * pPool,
    dhcpsPacket_t *pMsg,
    L7_uint32      *dhcpPacketLength,
    dhcpsOption_t *pParameterList,
    L7_uchar8* pOverloadState,
    L7_BOOL clientOverload);

/*********************************************************************
 * @purpose  Copt the TLV to the DHCP message buffer
 *
 * @param    pOption          @b{(output)} Pointer to dhcp option structure
 * @param    optionType       @b{(input)} Type of DHCP option
 * @param    pValue           @b{(input)} Ponter to value of DHCP option
 * @param    valueLen         @b{(input)} Length of the DHCP option
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void dhcpsCopyTLV(dhcpsOption_t* pOption, L7_uchar8 optionType, L7_uchar8 * pValue, L7_uint32 valueLen);
/* dhcps_map_util.c */

/* ip recv fns */

/*********************************************************************
 * @purpose  Is the given packet destination valid?
 *
 * @param    intIfNum      Interface packet was received on
 * @param    serverIpAddr  Address of DHCP server
 * @param    ipHeader      IP packet
 *
 * @returns  L7_TRUE       If packet destination is valid
 * @returns  L7_FALSE      Otherwise
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL dhcpsIsValidDest(L7_uint32 intIfNum, L7_uint32 serverIpAddr,
    L7_ipHeader_t *ipHeader);

/*********************************************************************
 * @purpose  Process an incoming the Dhcp Packet
 *
 *
 * @param    ipHeader     @b{(input)} IP Packet data
 * @param    ipLen        @b{(input)} IP Packet length
 * @param    intIfNum     @b{(input)} Interface num. through which packet arrived
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t processDhcpsFrames(L7_ipHeader_t *ipHeader, L7_uint32 ipLen, L7_uint32 intIfNum);


/*********************************************************************
 * @purpose  Map the Lease data structures to existing pools
 *
 *
 * @returns  L7_TRUE       If Success
 * @returns  L7_FALSE      Otherwise
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/

L7_RC_t dhcpsInitMappingLeaseConfigData();

/*********************************************************************
 * @purpose  Discard a unwanted packet not useful for DHCP Server
 *
 * @param    hookId        The hook location
 * @param    bufHandle     Handle to the frame to be processed
 * @param    *pduInfo      Pointer to info about this frame
 * @param    continueFunc  Optional pointer to a continue function
 *
 * @returns  SYSNET_PDU_RC_DISCARD     if frame is to be discarded
 * @returns  SYSNET_PDU_RC_IGNORED     continue processing this frame
 *
 * @notes    Recall that DHCP Server packets are targeted to UDP port 67.
 *           This is a convenient way to filter out such packets, since the DHCPS
 *           code uses a datagram socket instead of a raw socket and therefore
 *           does not have access to the destination IP address.
 *
 * @end
 *********************************************************************/

SYSNET_PDU_RC_t dhcpsMapFrameFilter(L7_uint32 hookId, L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo, L7_FUNCPTR_t continueFunc);

/* state machine functions */

/*********************************************************************
 * @purpose  Show the transition of the lease states
 *
 * @param    pLease            @b{(input/output)} Pointer to the lease node
 * @param    newState          @b{(input)} Lease state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsTransitionLeaseState(dhcpsLeaseNode_t * pLease, L7_int32 newState);

/*********************************************************************
 * @purpose  Locate a free IP by searching for gaps in allocated leases
 *           addresses and also ensure that the address is not explicitly
 *           excluded
 *
 * @param    pPool            @b{(input)} Pointer to pool node
 * @param    pIpAddr          @b{(output)} Pointer to free IP Address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsGetFreeIpAddr(dhcpsPoolNode_t * pPool, L7_IP_ADDR_t * pIpAddr);

/*********************************************************************
* @purpose  Create a free lease and async message to be processed by
*           pingTask
 *
 * @param    dhcpsPoolNode_t  @b{(input)}  Pointer to Pool node
 * @param    ipAddr           @b{(input)} IP Address
 * @param    clientIdentifier @b{(input)} Client Identifier
 * @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    dhcpPacketLength @b{(input)} Length of dhcp packet
 * @param    reqLeaseTime     @b{(input)} Requested Lease Time
* @param    incomingIfNum    @b{(input)} Interface number
* @param    getNextFreeIp    @b{(input)} flag to get next free ip
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_TABLE_IS_FULL
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsCreateFreeLease(dhcpsPoolNode_t *pPool, L7_IP_ADDR_t ipAddr,
                             L7_uchar8 *clientIdentifier, L7_uchar8 clientIdLen,
                             dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
                             L7_uint32 reqLeaseTime, L7_uint32 incomingIntfNum,
                             L7_BOOL getNextFreeIp);

/*********************************************************************
 * @purpose  Process DHCPDISCOVER message
 *
 * @param    ipAddrRequested     @b{(input)} Requested IP Address
 * @param    clientID            @b{(input)} Client Identifier
 * @param    clientIdLen         @b{(input)} Client Identifier Length
 * @param    leaseTimeRequested  @b{(input)} Requested Lease Time
 * @param    pDhcpPacket         @b{(input)} Pointer to dhcp packet
* @param    dhcpPacketLength   @b{(input)} Length of the packet
 * @param    incomingIfNum       @b{(input)} Interface number
 * @param    pParameterList      @b{(input)} Option Parameter List
 * @param    option82            @b{(input)} start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsProcessDiscoverMessage(L7_uint32 ipAddrRequested, L7_uchar8 *clientID, 
                                    L7_uchar8 clientIdLen, L7_uint32 leaseTimeRequested, 
                                    dhcpsPacket_t *pDhcpPacket,
                                    L7_uint32 dhcpPacketLength, L7_uint32 incomingIfNum, 
                                    dhcpsOption_t *pParameterList, dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Process DHCPDECLINE message
 *
 * @param    requestedIP      @b{(input)}  Requested IP
 * @param    clientID         @b{(input)} Client Identifier
 * @param    clientIdLen      @b{(input)} Client Identifier Length
 * @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
 * @param    incomingIfNum    @b{(input)} Interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsProcessDeclineMessage(L7_uint32 requestedIP, L7_uchar8 *clientID, L7_uchar8 clientIdLen, dhcpsPacket_t *pDhcpPacket, L7_uint32 incomingIfNum);

/*********************************************************************
 * @purpose  Process DHCPINFORM message
 *
 * @param    clientID            @b{(input)} Client Identifier
 * @param    clientIdLen         @b{(input)} Client Identifier Length
 * @param    leaseTimeRequested  @b{(input)} Requested Lease Time
 * @param    pDhcpPacket         @b{(input)} Pointer to dhcp packet
 * @param    incomingIfNum       @b{(input)} Interface number
 * @param    pParameterList      @b{(input)} Option Parameter List
 * @param    option82            @b{(input)} start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsProcessInformMessage(L7_uchar8 *clientID, L7_uchar8 clientIdLen, 
                                  L7_uint32 leaseTimeRequested, dhcpsPacket_t *pDhcpPacket, 
                                  L7_uint32 incomingIfNum, dhcpsOption_t *pParameterList,
                                  dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Process DHCPREQUEST when the client is in INIT-REBOOT state
 *
 * @param    requestedIP      @b{(input)}  Requested IP
 * @param    clientID         @b{(input)} Client Identifier
 * @param    clientIdLen      @b{(input)} Client Identifier Length
 * @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
 * @param    incomingIfNum    @b{(input)} Interface number
 * @param    pParameterList   @b{(input)} Option Parameter List
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsProcessInitRebootRequest(L7_IP_ADDR_t requestedIP, L7_uchar8 *clientID, 
                                      L7_uchar8 clientIdLen, dhcpsPacket_t *pDhcpPacket, 
                                      L7_uint32 incomingIfNum, dhcpsOption_t *pParameterList,
                                      dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Process DHCPRELEASE message
 *
 * @param    clientID         @b{(input)} Client Identifier
 * @param    clientIdLen      @b{(input)} Client Identifier Length
 * @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
 * @param    incomingIfNum    @b{(input)} Interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsProcessReleaseMessage(L7_uchar8 *clientID, L7_uchar8 clientIdLen, dhcpsPacket_t *pDhcpPacket, L7_uint32 incomingIfNum);

/*********************************************************************
 * @purpose  Process DHCPREQUEST when the client is in RENEW or REBIND state
 *
 * @param    clientID         @b{(input)} Client Identifier
 * @param    clientIdLen      @b{(input)} Client Identifier Length
 * @param    isBroadcast      @b{(input)} Was packet received as broadcast 
 * @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
 * @param    incomingIfNum    @b{(input)} Interface number
 * @param    pParameterList   @b{(input)} Option Parameter List
 * @param    option82         @b{(input)} Start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsProcessRenewRebindRequest (L7_uchar8 *clientID, L7_uchar8 clientIdLen, 
                                        L7_BOOL isBroadcast, dhcpsPacket_t *pDhcpPacket, 
                                        L7_uint32 incomingIfNum, dhcpsOption_t *pParameterList,
                                        dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Process DHCPREQUEST when the client is in SELECTING state
 *
 * @param    requestedIP      @b{(input)}  Requested IP
 * @param    serverID         @b{(input)} Server IP
 * @param    clientID         @b{(input)} Client Identifier
 * @param    clientIdLen      @b{(input)} Client Identifier Length
 * @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
 * @param    incomingIfNum    @b{(input)} Interface number
 * @param    pParameterList   @b{(input)} Option Parameter List
 * @param    option82         @b{(input)} start of option 82 in client packet
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsProcessSelectingRequest(L7_IP_ADDR_t requestedIP, L7_IP_ADDR_t serverID, 
                                     L7_uchar8 *clientID, L7_uchar8 clientIdLen, 
                                     dhcpsPacket_t *pDhcpPacket, L7_uint32 incomingIfNumi, 
                                     dhcpsOption_t *pParameterList, dhcpsOption_t *option82);

/*********************************************************************
 * @purpose  Process BOOTPREQUEST message
 *
 * @param    pDhcpPacket         @b{(input)} Pointer to dhcp packet
* @param    dhcpPacketLength    @b{(input)} Length of dhcp packet
 * @param    incomingIfNum       @b{(input)} Interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsProcessBootpRequestMessage(dhcpsPacket_t *pDhcpPacket,
                                        L7_uint32 dhcpPacketLength,
                                        L7_uint32 incomingIfNum);

/*********************************************************************
* @purpose  Wrapper to osapiPing
*
* @param    ipAddr            @b{(input)} IP Address to ping
* @param    numPackets        @b{(input)} number of packets to send
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dhcpsPing(L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Initialize the ping socket
*
* @param    None
*
* @returns  L7_SUCCESS If socket is successfully initialized
* @returns  L7_FAILURE If socket is failed to be initialized
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSockInit(void);

/*********************************************************************
* @purpose  Send a ping request
*
* @param    ipAddr            @b{(input)} IP Address to ping
* @param    seqNo             @b{(input)} ICMP Sequence Number
*
* @returns  L7_SUCCESS If Echo requests are sent successfully
* @returns  L7_ERROR   If stack returns failure on any socket operation
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dhcpsSendEchoRequest(L7_IP_ADDR_t ipAddr, L7_ushort16 seqNo);

/* macro to access cfg data from node */
#define POOL_DATA(x) (((x))->poolData)
#define LEASE_DATA(x) (((x))->leaseData)




/* prototypes for link list helper fns */
/*********************************************************************
 * @purpose  Insert after the given node
 *
 * @param    link           @b{(input)}  Pointer to the Linked list structure
 * @param    newlink        @b{(input)} Pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListInsertAfterLink(dhcpsLink_t * link, dhcpsLink_t * newlink);

/*********************************************************************
 * @purpose  Insert before the given node
 *
 * @param    link           @b{(input)}  Pointer to the Linked list structure
 * @param    newlink        @b{(input)} Pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListInsertBeforeLink(dhcpsLink_t * link, dhcpsLink_t * newlink);

/*********************************************************************
 * @purpose  Remove the given node
 *
 * @param    link           @b{(input)}  Pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListRemoveLink(dhcpsLink_t * link);

/*********************************************************************
 * @purpose  Get the head node
 *
 * @param    link           @b{(input)}  Pointer to the Linked list structure
 * @param    head           @b{(input)}  Pointer to pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListGetHead(dhcpsLink_t * link, dhcpsLink_t * * head);

/*********************************************************************
 * @purpose  Get the first node
 *
 * @param    link           @b{(input)}  Pointer to the Linked list structure
 * @param    first          @b{(input)}  Pointer to pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListFirstNode(dhcpsLink_t * link, dhcpsLink_t * * first);

/*********************************************************************
 * @purpose  Get the tail node
 *
 * @param    link           @b{(input)}  Pointer to the Linked list structure
 * @param    tail           @b{(input)}  Pointer to pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListGetTail(dhcpsLink_t * link, dhcpsLink_t * * tail);

/*********************************************************************
 * @purpose  Count the number of nodes in the list
 *
 * @param    link           @b{(input)} Pointer to the Linked list structure
 *
 * @returns  count (number of nodes)
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_uint32 dhcpsListGetCount(dhcpsLink_t * link);

/*********************************************************************
 * @purpose  Log message by traversing the list
 *
 * @param    link     @b{(input)}  Pointer to the Linked list structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsListDebugPrint(dhcpsLink_t * link);

/* Pool Management */
/*********************************************************************
 * @purpose  Create an address pool
 *
 * @param    pPoolCfg     @b{(input)}  Pointer to pool config structure
 * @param    pool         @b{(output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsCreatePool(poolCfgData_t * pPoolCfg, dhcpsPoolNode_t * * pool);

/*********************************************************************
 * @purpose  Create manual pool
 *
 * @param    pPoolCfg     @b{(input)}  Pointer to pool config structure
 * @param    pool         @b{(output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dhcpsCreateManualPool(poolCfgData_t * pPoolCfg, dhcpsPoolNode_t * * pool);


/*********************************************************************
 * @purpose  Destroy the pool node
 *
 * @param    ppPool         @b{(input/output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsDestroyPool(dhcpsPoolNode_t * * ppPool);

/*********************************************************************
 * @purpose  Get a pool node by matching client subnet
 *
 * @param    ipAddr         @b{(input)}   IP address
 * @param    pClientKey     @b{(input)}   Client key
 * @param    ppPool         @b{(output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMatchPool(L7_IP_ADDR_t ipAddr, dhcpsClientLeaseKey_t * pClientKey, dhcpsPoolNode_t * * ppPool);

/*********************************************************************
 * @purpose  Matches the subnet address against the pool for validity,
 *           returns the first matching pool.
 *
 * @param    subnetAddr      @b{(input)}  Subnet address
 * @param    ppPool         @b{(output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMatchFirstPoolForSubnet(L7_IP_ADDR_t ipAddr, dhcpsPoolNode_t * * ppPool);

/*********************************************************************
 * @purpose  Matches the subnet address against the pool for validity,
 *           returns the next matching pool.
 *
 * @param    subnetAddr      @b{(input)}  Subnet address
 * @param    ppPool         @b{(output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMatchNextPoolForSubnet(L7_IP_ADDR_t ipAddr, dhcpsPoolNode_t * * ppPool);

/*********************************************************************
 * @purpose  Search a pool node by IP address
 *
 * @param    ipAddr         @b{(input)}         IP address
 * @param    ppPool         @b{(input/output)}  Pointer to pointer to the pool node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindPoolByIp(L7_IP_ADDR_t ipAddr, dhcpsPoolNode_t * * ppPool);

/* Address Exclusion */

/************************TBD WILL BE DONE AS PART OF MANUAL BINDING*/

L7_RC_t dhcpsAddAddrExclusion(L7_IP_ADDR_t fromIpAddr, L7_IP_ADDR_t toIpAddr);
L7_RC_t dhcpsDelAddrExclusion(L7_IP_ADDR_t fromIpAddr);

/*********************************************************************
 * @purpose  Check if an IP Address is excluded
 *
 * @param    ipAddr       @b{(input)}  IP Address
 * @param    isManualExcluded  @b{(input)}  Flag to consider manual as excluded
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_BOOL dhcpsCheckIfExcluded(L7_IP_ADDR_t ipAddr, L7_BOOL isManualExcluded);

/*********************************************************************
 * @purpose  Check if the specified ip address & subnet mask conflicts
 *           with the network or service port configuration and whether
 *           the ip is already configured on another interface
 *
 *
 * @param    ipAddr   @b{(input)}  IP Address
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_BOOL dhcpsIsConflictingIp(L7_uint32 ipAddr);

/* Lease Management */
/*********************************************************************
 * @purpose  Create lease node
 *
 * @param    index          @b{(input)}  index of the array
 * @param    pPool         @b{(input)}   Pointer to the pool node
 * @param    ppLease       @b{(output)}  Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsCreateLeaseNode( L7_uint32  index, dhcpsPoolNode_t *  pPool, dhcpsLeaseNode_t * * ppLease);

/*********************************************************************
 * @purpose  Find lease by IP Address, verify if it can be given, then allocate lease
 *
 * @param    pPool             @b{(input)}  Pointer to the pool node
 * @param    ipAddr            @b{(input)}  IP Address
 * @param    clientIdentifier  @b{(input)}  Client Identifier
 * @param    clientIdLen       @b{(input)}  Client Identifier Length
 * @param    hwAddr            @b{(input)}  Hardware Address
 * @param    hAddrtype         @b{(input)}  Hardware Address Type
 * @param    ppLease           @b{(output)} Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsAllocateLease(dhcpsPoolNode_t *  pPool, L7_IP_ADDR_t ipAddr, L7_uchar8 *clientIdentifier, L7_uchar8 clientIdLen,
    L7_uchar8 *hwAddr, L7_uchar8 hAddrtype, L7_uchar8 hwAddrLength, dhcpsLeaseNode_t * * ppLease);

/*********************************************************************
 * @purpose  Deallocate lease
 *
 * @param    ppLease   @b{(input/output)} Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsDeallocateLease(dhcpsLeaseNode_t * * ppLease);

/*********************************************************************
 * @purpose  Get Client Key
 *
 * @param    pClientIdentifier  @b{(input)}  Pointer to the Client Identifier
 * @param    clientIdLen        @b{(input)}  Length of the Client Identifier
 * @param    pHwAddr            @b{(input)}  Pointer to the Hardware Address
 * @param    hAddrtype          @b{(input)}  Hardware Address Type
 * @param    hwAddrLength       @b{(input)}  Hardware Address Length
 * @param    pLeaseKey          @b{(output)} Pointer to the client lease key
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t getClientKey(L7_uchar8 * pClientIdentifier, L7_uchar8 clientIdLen, L7_uchar8 * pHwAddr, L7_uchar8 hAddrtype, L7_uchar8 hwAddrLength, dhcpsClientLeaseKey_t * pLeaseKey);

/*********************************************************************
 * @purpose  Search for the lease node by IP Address
 *
 * @param    ipAddr         @b{(input)}  IP Address
 * @param    ppLease       @b{(output)}  Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindLeaseNodeByIpAddr(L7_IP_ADDR_t ipAddr, dhcpsLeaseNode_t * * ppLease);

/* Scheduling Leases for Expiry */

/*********************************************************************
 * @purpose  Schedule lease for the given number of seconds
 *
 * @param    pLease          @b{(input/output)}  Pointer to the lease node
 * @param    secs            @b{(input)}  Number of seconds for which lease is scheduled
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsScheduleLease(dhcpsLeaseNode_t * pLease, L7_uint32 secs);

/*********************************************************************
 * @purpose  Unschedule lease
 *
 * @param    pLease          @b{(input/output)}  Pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsUnscheduleLease(dhcpsLeaseNode_t * pLease);

/*********************************************************************
 * @purpose  Get epired leases
 *
 * @param    ppExpiredLease  @b{(input/output)}  Pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsGetExpiredLease(dhcpsLeaseNode_t * * ppExpiredLease);

/* Leases by Pool */

/*********************************************************************
 * @purpose  Search for the first lease node by pool
 *
 * @param    pPool      @b{(input)}   Pointer to the pool node
 * @param    ppLease    @b{(output)}  Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindFirstLeaseByPool(dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease);

/*********************************************************************
 * @purpose  Search for the next lease node by pool
 *
 * @param    pLease         @b{(input)}   Pointer to the lease node
 * @param    ppNextLease    @b{(output)}  Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindNextLeaseByPool(dhcpsLeaseNode_t * pLease, dhcpsLeaseNode_t * * ppNextLease);

/*********************************************************************
 * @purpose  Search for the lease node by IP Address
 *
 * @param    pPool          @b{(input)}  Pointer to the pool node
 * @param    ipAddr         @b{(input)}  IP Address
 * @param    ppLease       @b{(output)}  Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindLeaseByIp(dhcpsPoolNode_t * pPool, L7_IP_ADDR_t ipAddr, dhcpsLeaseNode_t * * ppLease);

/* Leases by Client Key */
/*********************************************************************
 * @purpose  Find the lease of the client by client key
 *
 * @param    pClientKey  @b{(input)} Pointer to the client key
 * @param    pPool       @b{(input)} Pointer to the pool node
 * @param    ppLease     @b{(output)} Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindClientLease(dhcpsClientLeaseKey_t * pClientKey, dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease);

/*********************************************************************
 * @purpose  Find next lease of the client
 *
 * @param    pPool    @b{(input)} Pointer to the pool node
 * @param    ppLease  @b{(output)} Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindNextLeaseByClient(dhcpsPoolNode_t * pPool, dhcpsLeaseNode_t * * ppLease);

/* Client Table Helper Fns */

/*********************************************************************
 * @purpose  Create the client lease table using an AVL tree
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsClientLeaseTableCreate(void);

/*********************************************************************
 * @purpose  Insert an entry into the DHCPS Client lookup table
 *
 * @param    pClientKey  @b{(input)} client key
 * @param    pLease      @b{(input)} client lease
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsInsertClientLeaseTableEntry(dhcpsClientLeaseKey_t * pClientKey, dhcpsLeaseNode_t * pLease);

/*********************************************************************
 * @purpose  Remove an entry into the DHCPS Client lookup table
 *
 * @param    pLease      @b{(input)} client lease
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsRemoveClientLeaseTableEntry(dhcpsLeaseNode_t * pLease);

/*********************************************************************
 * @purpose  Delete an entry from the DHCPS Client lookup table
 *
 * @param    pLease       @b{(input)} Pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsClientLeaseTableSearch(dhcpsClientLeaseKey_t * pClientKey, dhcpsClientLeaseNode_t **ppClientNode);

/*********************************************************************
 * @purpose  Purge the client table of all entries
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsClientLeaseTableDelete(void);

/* utility function */
/*********************************************************************
 * @purpose  Get pool index
 *
 * @param    poolName   @b{(input)}  Pool name
 * @param    poolIndex  @b{(output)} Pointer to pool index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsGetPoolIndex(L7_char8 *poolName, L7_uint32 *poolIndex);

/*********************************************************************
 * @purpose  Validate subnet
 *
 * @param    subnetIp     @b{(input)}  Subnet Address
 * @param    subnetMask   @b{(input)}  Subnet Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsValidateSubnet(L7_uint32 subnetIp, L7_uint32 subnetMask);

/*********************************************************************
 * @purpose  Find scheduled lease by IP
 *
 * @param    ipAddr       @b{(input)}  IP Address
 * @param    ppLease      @b{(output)} Pointer to pointer to the lease node
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsFindScheduledLeaseByIp(L7_uint32 ipAddr, dhcpsLeaseNode_t **ppLease );

/*********************************************************************
 * @purpose  Get the Server ID by giving the interface number
 *
 * @param    incomingIfNum    @b{(input)}  Interface number
 * @param    pIpAddr          @b{(output)} Pointer to IP Address
 * @param    pIpMask          @b{(output)} Pointer to IP Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsGetServerID(L7_uint32 incomingIfNum, L7_IP_ADDR_t * pIpAddr, L7_IP_MASK_t * pIpMask);

/*********************************************************************
 * @purpose  Validate exluded Ip range with existing data    
 *
 * @param    fromAddr    @b{(input)}  Address start range
 * @param    toAddr      @b{(input)}  Address end range
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL dhcpsExcludedAddressRangeValidate(L7_IP_ADDR_t fromAddr, L7_IP_ADDR_t toAddr);

/*********************************************************************
 * @purpose  Initialize the OptionInfo data structure
 *
 * @param    pOptionInfo      @b{(input)}  DHCP option data ptr
 *
 * @returns  L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsInitializeOptionValidationData(dhcpsOptionInfo_t * pOptionInfo);

/*********************************************************************
 * @purpose  Validate a DHCP option code
 *
 * @param    code      @b{(input)}  DHCP option code
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsValidateOptionCode(L7_uchar8 code);

/*********************************************************************
 * @purpose  Validate a DHCP option
 *
 * @param    code      @b{(input)}  DHCP option code
 * @param    data      @b{(input)}  DHCP option data
 * @param    length    @b{(input)}  DHCP option length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsValidateOption(L7_uchar8 code, L7_uchar8 * data, L7_uint32 length);


/*********************************************************************
 * @purpose  Initialise Option data
 *
 * @param    poolOption            @b{(input)}  List of options.
 *
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void dhcpsInitPoolOption(L7_BOOL  * poolOption);

/*********************************************************************
 * @purpose Set the option flag for the configured option in pool node
 *
 * @param    pPoolNode @b{(input)}  DHCPS pool node.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void dhcpsSetPoolOption(dhcpsPoolNode_t * pPoolNode);

/*********************************************************************
 * @purpose  Find the tlv option from buffer
 *
 * @param    pPackArray    @b{(input)}  DHCPS option array.
 * @param    code          @b{(input)}  DHCPS option type.
 * @param    ppTLV         @b{(output)} DHCPS option ptr.
 * @param    pIndex        @b{(output)} Index of option in array.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsFindTLVOption(L7_uchar8* pPackArray, L7_uchar8 code, 
    dhcpsPackOption_t * * ppTLV, L7_uint32 * pIndex);

/*********************************************************************
 * @purpose  Get the tlv data from buffer
 *
 * @param    pPackArray    @b{(input)}  DHCPS option array.
 * @param    code          @b{(input)}  DHCPS option type.
 * @param    buffer        @b{(output)} DHCPS option value.
 * @param    pLength       @b{(output)} DHCPS option length.
 * @param    pDataFormat   @b{(output)} DHCPS stored data format.
 * @param    pStatus       @b{(output)} DHCPS data option status.
 *
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsGetTLVOption( L7_uchar8* pPackArray, L7_uchar8 code,
    L7_uchar8* buffer, L7_uint32* pLength,
    L7_uchar8* pDataFormat, L7_uchar8 *pStatus);

/*********************************************************************
 * @purpose  Set the tlv data from buffer
 *
 * @param    pPackArray     @b{(output)}  DHCPS option array.
 * @param    code           @b{(input)}  DHCPS option type.
 * @param    status         @b{(input)}  DHCPS option status.
 * @param    buffer         @b{(input)} DHCPS option value.
 * @param    length         @b{(input)} DHCPS option length.
 * @param    dataFormat     @b{(input)} DHCPS stored data format.
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSetTLVOption( L7_uchar8* pPackArray, L7_uchar8 code,
    L7_uchar8 status, L7_uchar8* data, L7_uint32 length, L7_uchar8 dataFormat);

/*********************************************************************
 * @purpose  Create TLV option row
 *
 * @param    pPackArray     @b{(input)}  DHCPS option array.
 * @param    code           @b{(input)}  DHCPS option type.
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsTLVOptionCreate( L7_uchar8* pPackArray, L7_uchar8 code);

/*********************************************************************
 * @purpose  Get the first TLV code
 *
 * @param    pPackArray     @b{(input)}  DHCPS option array.
 * @param    pCode          @b{(output)}  DHCPS option type.
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ALREADY_CONFIGURED
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsTLVOptionGetFirst( L7_uchar8* pPackArray, L7_uchar8* pCode);


/*********************************************************************
 * @purpose  Get the next TLV option code
 *
 * @param    pPackArray     @b{(input)}  DHCPS option array.
 * @param    prevCode       @b{(input)}  DHCPS option type.
 * @param    pCode          @b{(output)}  DHCPS option type.
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsTLVOptionGetNext( L7_uchar8* pPackArray, L7_uchar8 prevCode,
    L7_uchar8* pCode);

/*********************************************************************
 * @purpose  Delete the tlv option field
 *
 * @param    pPackArray     @b{(input)}  DHCPS option array.
 * @param    code           @b{(input)}  DHCPS option type.
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsTLVOptionDelete( L7_uchar8* pPackArray, L7_uchar8 code);

/*********************************************************************
 * @purpose  check if client supports long options as per rfc 3396
 *
 * @param    requestList      @b{(output)} Pointer to list of requested options
 * @param    length           @b{(input)} Length of the requested options list
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL dhcpsClientSupportsLongOptions(L7_uchar8 * requestList, L7_uchar8 length);

/*********************************************************************
 * @purpose  Get the strlen where maximum length of a string is fixed
 *
 * @param    L7_uchar8     @b{(input)}  Binary String
 * @param    maxLen        @b{(input)}  Maximum possible length
 *
 * @returns  L7_uint32
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 dhcpsStrnlen(L7_uchar8 * pStr, L7_uint32 maxLen);

/*********************************************************************
 * @purpose  Convert a binary string to Hex
 *
 * @param    binData         @b{(input)} Binary Data
 * @param    binDataLength   @b{(input)} Binary Data Length in bytes
 * @param    hexStrLength    @b{(input)} Output String Length in bytes
 * @param    hexStr          @b{(output)} Hex String
 *
 * @returns  bytes converted
 *
 * @notes    conversion is restricted to hexStrLength
 *
 * @end
 *********************************************************************/
L7_uint32 dhcpsBin2Hex(L7_uchar8 * binData, L7_uint32 binDataLength, L7_uint32 hexStrLength, L7_char8 * hexStr);

#endif /* _DHCPS_UTIL_H_*/

