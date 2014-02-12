/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename comm_structs.h
*
* @purpose The purpose of this file is to have a central location for
*          common structures to be used by the entire system.
*
* @component sysapi
*
* @comments none
*
* @create 9/20/2000
*
* @author bmutz
* @end
*
**********************************************************************/

#ifndef INCLUDE_COMM_STRUCTS
#define INCLUDE_COMM_STRUCTS

#include "commdefs.h"
#include "l7_product.h"
#include "l7_packet.h"
#include "file_exports.h"
#include "ip_exports.h"
#include "dns_exports.h"
#include "dot1ad_exports.h"
#include "dot1ag_exports.h"
#include "avl_api.h"
#include "dot1q_exports.h"

/****************************************
*
*  Common file header
*
*****************************************/

#define L7_FILE_HDR_PAD_LEN     7
#define L7_FILE_HDR_VER_1       0x1        /*  Release 4.0 and prior */
#define L7_FILE_HDR_VER_2       0x2        /*  Introduced in release 4.1 */
#define L7_FILE_HDR_VER_3       0x3        /*  Introduced in release 4.3 - structurally the same as version 2 */

#define L7_FILE_HDR_VER_CURRENT L7_FILE_HDR_VER_3    /*  Current Version */

typedef struct
{
  L7_uchar8   filename[L7_MAX_FILENAME];    /* file name                       */
  L7_uint32   version;                      /* software version of file        */
L7_COMPONENT_IDS_t componentID;
#if 0
  L7_uint32   componentID;                  /* L7 software component ID        */
#endif
  L7_uint32   type;                         /* type of data-component specific */
  L7_uint32   length;                       /* length of data, including hdr   */
  L7_uint32   dataChanged;                  /* file changed if not NULL        */
  L7_uint32   savePointId;                  /* random number generator */
  L7_uint32   targetDevice;                 /* Identifies the hardware compatible with this file */
  L7_uint32   fileHdrVersion;               /* L7_fileHdr_t version */
  L7_uint32   pad[L7_FILE_HDR_PAD_LEN];     /* Space for future enhancements. */
}
L7_fileHdr_t;

/* PTin added: packet type */
#if 1
typedef enum
{
  PTIN_PACKET_NONE=0,
  PTIN_PACKET_IGMP,
  PTIN_PACKET_MLD,
  PTIN_PACKET_DHCP,
  PTIN_PACKET_PPPOE,
  PTIN_PACKET_APS,
  PTIN_PACKET_CCM,
  PTIN_PACKET_IPDTL0,
  PTIN_PACKET_LAST
} ptin_packet_type_t;
#endif

/****************************************
*
*  Common network information needed from bootp/DHCP
*
*****************************************/

/* Flags used to specify what options
   have been offered by DHCP/BOOTP server */
typedef enum
{
  BOOTP_DHCP_IP_ADDR  = 1,
  BOOTP_DHCP_NETMASK  = 2,
  BOOTP_DHCP_GATEWAY  = 4,
  BOOTP_DHCP_SIADDR   = 8,
  BOOTP_DHCP_BOOTFILE = 16,
  BOOTP_DHCP_SNAME    = 32,
  BOOTP_DHCP_OPTION_TFTP_SVR_ADDR  = 64,
  BOOTP_DHCP_OPTION_DNS_SVR_ADDR   = 128,
  BOOTP_DHCP_OPTION_BOOTFILE       = 256,
  BOOTP_DHCP_OPTION_TFTP_SVR_NAME  = 512,
  BOOTP_DHCP_OPTION_VENDOR_SPECIFIC = 1024
}
L7_bootp_dhcp_OfferedOptions;

#define BOOTP_FILE_LEN 128 /* Bootfile name max length */
#define BOOTP_SNAME_LEN 64 /* Server name max length*/

#define DHCP_VENDOR_SPECIFIC_SUBOPTIONS_NUM    3   /* Num of suboptions supported in Option-43.*/
#define DHCP_BOOTP_VEND_OPTIONS_LEN  64  /* should be in sync with L7_BP_VEND_LEN.*/
/* These option-specific structures are needed by the external callers. 
   Hence the definition is made public.*/
typedef struct tlvOption_s
{
  L7_uchar8 type;
  L7_uchar8 len;
  L7_uchar8 option[DHCP_BOOTP_VEND_OPTIONS_LEN]; /* This shows the max length case. Dynamic 
                                       allocation for the option received in the 
                                       packets is not feasible as the option
                                       length could vary each time.Hence static allocating
                                       the Max possible case.*/
}tlvOption_t;
typedef struct dhcpVendorSpecificOption_s
{
  /* The suboptions under this option is expected to be in TLV format*/
  L7_uchar8  numSubOptions;
  tlvOption_t subOptions[DHCP_VENDOR_SPECIFIC_SUBOPTIONS_NUM];
}dhcpVendorSpecificOption_t;


typedef struct
{
  L7_uint32 ip;                                    /* Host ip address */
  L7_uint32 netMask;                               /* Host netmask */
  L7_uint32 gateway;                               /* Gateway */
  L7_uint32 siaddr;                                /* Next server address */
  L7_uint32 optTftpSvrIpAddr;                      /* IP addres of TFTP server, got from option 150 field */
  L7_uint32 optDnsServerIpAddr[L7_DNS_NAME_SERVER_ENTRIES];  /* IP addres of DNS server, got from option 6 field */

  L7_uchar8 bootFile[BOOTP_FILE_LEN + 1];          /* Bootfile name */
  L7_uchar8 optBootFile[BOOTP_FILE_LEN + 1];       /* Bootfile name, got from option 67 field */
  L7_uchar8 sname[BOOTP_SNAME_LEN + 1];            /* Server name */
  L7_uchar8 optTftpSvrName[BOOTP_SNAME_LEN + 1];   /* TFTP server name, got from option 66 field */
  dhcpVendorSpecificOption_t  *vendSpecificOptions;/* DHCP Option-43.*/
  L7_uint32 offeredOptionsMask;                    /* Mask holds offered options IDs. Set mask using OR operation
                                                      with flags defined in L7_bootp_dhcp_OfferedOptions */
}
L7_bootp_dhcp_t;

typedef L7_uint32       L7_IP_ADDR_t;         /* IP Address */
typedef L7_uint32       L7_IP_MASK_t;         /* IP Subnet Mask */

/*******************************************
*  Common network information used by dhcp
********************************************/
typedef enum
{
  L7_MGMT_SERVICEPORT = 0,
  L7_MGMT_NETWORKPORT,
  L7_MGMT_IPPORT
} L7_MGMT_PORT_TYPE_t;

/*---------------------------------*/
/* IP Address Configuration Method */
/*---------------------------------*/
/* The following enum indicates the method used to
 * configure an IP Address on an interface.
 */
typedef enum
{
  L7_INTF_IP_ADDR_METHOD_NONE = 0,   /* No Method */
  L7_INTF_IP_ADDR_METHOD_CONFIG,     /* Manual Configuration */
  L7_INTF_IP_ADDR_METHOD_DHCP,       /* Leased through DHCP */
  L7_INTF_IP_ADDR_RENEW,             /* Renew the address */
  L7_INTF_IP_ADDR_RELEASE            /* Release the address */

} L7_INTF_IP_ADDR_METHOD_t;


/*****************************************/

/* Indices values for vlan masks */
/* Number of bytes in mask */
#define L7_VLAN_INDICES   ((L7_MAX_VLAN_ID) / (sizeof(L7_uchar8) * 8) + 1)
#define L7_VLAN_MAX_MASK_BIT   L7_MAX_VLAN_ID


/* structure definition for the vlan Mask : bitmask for all vlans */
/* Interface storage */
typedef struct
{
  L7_uchar8   value[L7_VLAN_INDICES];
} L7_VLAN_MASK_t;


/****************************************
*
* Traceroute Structure
*
*****************************************/
typedef struct tracertReply_s
{
  L7_int32 timeMsec;
  L7_uchar8 errorSymbol;
} tracertReply_t;


/************************************************
*
*  Parameter structure needed for Outbound Telnet
*
************************************************/

typedef struct
{
  L7_BOOL   modes[3];
  L7_uint32 sock;
  L7_uint32 port;
  L7_char8  *ipAddr;
  L7_uint32 sockout;
} telnetParams_t;

/*********************************************************************
*
* @structures L7_localtime_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct L7_localtime_t
{
        L7_uint32 L7_sec;         /* seconds after the minute   - [0, 59] */
        L7_uint32 L7_min;         /* minutes after the hour     - [0, 59] */
        L7_uint32 L7_hour;      /* hours after midnight         - [0, 23] */
        L7_uint32 L7_mday;      /* day of the month             - [1, 31] */
        L7_uint32 L7_mon;         /* months since January               - [0, 11] */
        L7_uint32 L7_year;      /* years since 1900     */
        L7_uint32 L7_wday;      /* days since Sunday            - [0, 6] */
        L7_uint32 L7_yday;      /* days since January 1         - [0, 365] */
        L7_uint32 L7_isdst;     /* Daylight Saving Time flag */
}  L7_localtime;


typedef struct dnsHost_s
{
  L7_IP_ADDRESS_TYPE_t hostAddrType;
  union
  {
    L7_uint32 ipAddr;
    L7_char8  hostName[L7_DNS_HOST_NAME_LEN_MAX];
  }host;
} dnsHost_t;

/* Structure that holds the address conflict statistics
 * per IP address entry */
typedef struct addrConflictStats_s
{
  L7_uint32   numOfConflictsDetected;
} addrConflictStats_t;

/* Registrant for the notification of address conflict events
 * on mgmt interfaces (service port or network port)
 */
typedef void (*simIPAddrConflictCB_t)(L7_MGMT_PORT_TYPE_t, L7_uint32, L7_uchar8*);

typedef enum 
{
  DS_BINDING_TENTATIVE = 0,
  DS_BINDING_STATIC,
  DS_BINDING_DYNAMIC
} dsBindingType_t;

typedef enum 
{
  DS_LEASESTATUS_UNKNOWN = 0,

  /* v4 */
  DS_LEASESTATUS_V4_DISCOVER = 1,
  DS_LEASESTATUS_V4_OFFER,
  DS_LEASESTATUS_V4_REQUEST,
  DS_LEASESTATUS_V4_ACK,
  DS_LEASESTATUS_V4_NACK,
  DS_LEASESTATUS_V4_DECLINE,
  DS_LEASESTATUS_V4_RELEASE,
  DS_LEASESTATUS_V4_INFORM,

  /* v6 */
  DS_LEASESTATUS_V6_SOLICIT = 11,
  DS_LEASESTATUS_V6_ADVERTISE,
  DS_LEASESTATUS_V6_REQUEST,
  DS_LEASESTATUS_V6_CONFIRM,
  DS_LEASESTATUS_V6_RENEW,
  DS_LEASESTATUS_V6_REBIND,
  DS_LEASESTATUS_V6_REPLY,
  DS_LEASESTATUS_V6_RELEASE,
  DS_LEASESTATUS_V6_DECLINE,
  DS_LEASESTATUS_V6_RECONFIGURE,
  DS_LEASESTATUS_V6_INFORMATIONREQUEST,
} dsLeaseStatus_t;

typedef enum 
{
  IPSG_ENTRY_STATIC,
  IPSG_ENTRY_DYNAMIC
} ipsgEntryType_t;

extern L7_uchar8 *dsBindingTypeNames[];

typedef struct dhcpSnoopBinding_s
{
  /* A MAC address uniquely identifies a node in the bindings tree. */
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];

  /* VLAN station is in. */
  L7_ushort16 vlanId;

  /* PTin added: DHCP snooping */
  #if 1
  /* Inner vlan */
  L7_ushort16 innerVlanId;
  #endif

  /* IP address assigned to the station */
  L7_uint32 ipAddr;

  /* PTin added: DHCPv6 */
#if 1
   /* Ipv6 compatible IP Address */
  L7_uchar8 ipFamily;
  L7_uchar8 ipv6Addr[16];
//   L7_inet_addr_t ptinIpAddr;
#endif

  /* physical port where client is attached. */
  L7_uint32 intIfNum;

  /* Time left on lease (minutes) */
  L7_uint32 remLease;

  /* Current lease status */
  dsLeaseStatus_t leaseStatus;

  /* dynamic, static, tentative */
  dsBindingType_t bindingType;

} dhcpSnoopBinding_t;


typedef struct ipsgBinding_s
{
  /* A MAC address uniquely identifies a node in the bindings tree. */
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];

  /* VLAN station is in. */
  L7_ushort16 vlanId;

  /* IP address assigned to the station */
  L7_uint32 ipAddr;

  /* physical port where client is attached. */
  L7_uint32 intIfNum;

  /* Time left on lease (minutes) */
  L7_uint32 remLease;

  /* dynamic, static, tentative */
  ipsgEntryType_t bindingType;

} ipsgBinding_t;



/* DHCP snooping interface statistics */
typedef struct dhcpSnoopIntfStats_s
{
  /* Number of DHCP client packets whose source MAC address didn't 
   * match the client hardware address. */
  L7_uint32 macVerify;
  
  /* Number of DHCP client packets received on an interface different from the
   * interface where the DHCP snooping binding says the client should be. 
   * Only DHCPRELEASE or DHCPDECLINE. */
  L7_uint32 intfMismatch;

  /* Number of DHCP server messages received on an untrusted port. */
  L7_uint32 untrustedSvrMsg;

} dhcpSnoopIntfStats_t;

/* Dynamic ARP Inspection vlan statistics */
typedef struct daiVlanStats_s
{
  L7_uint32 forwarded;
  L7_uint32 dropped;
  L7_uint32 dhcpDrops;
  L7_uint32 dhcpPermits;
  L7_uint32 aclDrops;
  L7_uint32 aclPermits;
  L7_uint32 sMacFailures;
  L7_uint32 dMacFailures;
  L7_uint32 ipValidFailures;
} daiVlanStats_t;

/*  Dot1ad structure definitions  */
typedef struct dot1adProtocolTunnelingAction_s
{
  L7_enetMacAddr_t     protocolMAC;
  L7_ushort16              protocolId; 
  L7_BOOL                  isProtoIdConfig;
  
  /* The above MAC address and protocolId 
       constitute a unique (like index) way
       To identify a protocol PDU for which the 
       tunnel action can be applied. 
  */
  DOT1AD_TUNNEL_ACTION_t   tunnelAction; 
} dot1adProtocolTunnelingAction_t;


typedef struct dot1adIntfCfg_s
{
    /* Interface type (UNI-P,UNI-S or nni) */
  DOT1AD_INTFERFACE_TYPE_t  intfType;
  
    /* Protocol to tunneling action mapping */
  dot1adProtocolTunnelingAction_t  protoTunnel[L7_DOT1AD_L2_PROTOCOL_ID_MAX];


} dot1adIntfCfg_t;

typedef struct dot1adCfg_s
{
  dot1adIntfCfg_t     intfCfg[L7_DOT1AD_INTF_MAX_COUNT + 1];
  avlTreeTables_t     *treeHeap;
  L7_uint32           treeHeapSize;
  void                *dataHeap;   /* ipsgEntryNode_t* */
  L7_uint32           dataHeapSize;
  avlTree_t           treeData;
} dot1adCfg_t;

typedef  struct hapiBroadDot1adDebugIntfStats_s
{
  L7_uint32 numPduReceived;          /* No. of PDUs received by software */
  
  L7_uint32 numPduTunneled;          /* Number of PDUS tunneled */
  
  L7_uint32 numPduDetunneled;       /* Number of PDUS de-tunneled */
  
  L7_uint32 numPduTerminated;      /* Number of PDUS terminated */
  
  L7_uint32 numPduDiscarded;        /* Number of PDUS discarded */
  
  L7_uint32 numPduDiscarded_shutdown; /* Num of PDUS discarded and shutdown*/

} dot1adDebugIntfStats_t;


#ifdef L7_DOT3AH_PACKAGE

/* 57.5.2.3 Organization Specific Information TLV */
#define DOT3AH_OUI_LENGTH                 3
/* 57-10 Vendor Specific Identifier field */
#define DOT3AH_VSI_LENGTH                 4


typedef struct L7_dot3ah_oui_s
{
  L7_uchar8  addr[DOT3AH_OUI_LENGTH];
}L7_DOT3AH_OUI_t;

typedef struct L7_dot3ah_vsi_s
{
  L7_uchar8  addr[DOT3AH_VSI_LENGTH];
}L7_DOT3AH_VSI_t;

typedef struct dot3ahNodeEntryKey_s
{
  /* A MAC address uniquely identifies a node*/

   L7_enetMacAddr_t macAddr;

  /* OUI uniquely identifies a end product org*/
  L7_DOT3AH_OUI_t oui;

  /* interface on which Node valid */
  L7_uint32                           intIfNum;

}L7_dot3ahNodeEntryKey_t;

/* Structure for storing Discovered EFM-OAM nodes in AVL Tree */
typedef struct dot3ah_node_discvry_s
{
  L7_dot3ahNodeEntryKey_t                key; /* 3 bytes of OUI and
                                            6 bytes of MAC address */
  void*                               next;

}L7_dot3ah_node_discvry_t;

typedef struct L7_dot3ah_if_cfg_s
{
  L7_BOOL               isEFMOAMEnabled;
  L7_BOOL               MinOrMaxRate;
  L7_uint32             efm_link_timer;
  L7_uint32             efm_rem_lb_expiry_timer;
  L7_uint32             efm_pdu_timer;
  L7_uint8              oam_config;
  L7_uint8              infoTlv;
  L7_uint32             err_frame_window;
  L7_uint32             err_frame_thres_low;
  L7_uint32             err_frame_thres_high;
  L7_uint32             err_frame_period_window;
  L7_uint32             err_frame_period_thres_low;
  L7_uint32             err_frame_period_thres_high;
  L7_ushort16           err_frame_sec_sum_window;
  L7_ushort16           err_frame_sec_sum_thres_low;
  L7_ushort16           err_frame_sec_sum_thres_high;
  L7_uint32             max_pdu_rate;
  L7_uint32             min_pdu_rate;

}L7_dot3ahIntfCfg_t;
#endif

typedef enum
{
  L7_EMWEBMSG_INVALID = 0,
  L7_EMWEBMSG_APPLYGLOBALCFG,
  L7_EMWEBMSG_APPLYINTFCFG,
  L7_EMWEBMSG_DO_CALLBACK,

  L7_EMWEBMSG_MAX                           /* This must be last */
} emwebMsgType_t;


#ifndef EW_UNUSED
    #define EW_UNUSED(x) (void)x
#endif

typedef void(*EmWebCbFuncPtr)(void *);           

typedef struct
{
  emwebMsgType_t msgType;
  void          *pContext;
  EmWebCbFuncPtr CbFuncPtr;           
  void          *syncSemaphore;
} emwebMessage_t;

/*
 * This API will write to the EmWeb task pipe.  It is intended to be used to signal the
 * EmWeb task that some particular work needs to be performed in the context of the EmWeb
 * task (e.g., execute CLI commands).
 */
L7_RC_t emwebMessageWrite(emwebMessage_t *message);

#endif

